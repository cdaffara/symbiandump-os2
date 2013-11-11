// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "SYSIF.H"
#include <estlib.h>
#include "FDESC.H"
#include "PIPEDESC.H"
#include <stdlib.h>

/**
@internalComponent
*/
const TInt KCPosixMajorVersionNumber=2;
const TInt KCPosixMinorVersionNumber=0;
/**
@internalComponent
*/
enum TPosixServerPanic 
	{
	EPosix_BadWaitCompletion=1,
	EPosix_NoPendingIO=2,
	};
/**
@internalComponent
*/
enum PosixMessage {
	// asynchronous operations which need SetActive/RunL
	PMread, PMwrite, PMfsync, 
	PMsendto, PMrecvfrom, PMconnect, PMshutdown, PMaccept,
//NOTIFY PROBLEM
//	PMioctl, 
	PMioctl, PMioctlN,
	// asynchronous operations which just delay completion
	PMwaitpid,
	// synchronous operations
	PMdup, PMdup2,
	PMopen, PMclose, PMlseek, PMfstat,
	PMgetcwd,
	PMchdir, PMmkdir, PMrmdir, 
	PMchmod, PMunlink, PMstat, PMrename,
	PMResolvePath,
	PMsocket, PMbind, PMlisten, PMsockname, PMgetsockopt, PMsetsockopt,
	PMioctlcomplete, PMcancel,
	PMTerminateProcess,
	PMgetenv, PMsetenv, PMunsetenv,
	PMpopen3, 
	// inter-process operations
	PMAreYouMyMother, PMHelloMum, PMPipeWrite, PMPipeRead, PMPipeIoctl, PMPipeClose, PMPipeCancel
	};
/**
@internalComponent
*/
struct PosixParams
	{
	int ret;
	int fid;
	int pint[3];
	const char* cptr[2];
	char* ptr[2];
	const wchar_t* cwptr[2];
	wchar_t* wptr[2];
	wchar_t** eptr[1];
	unsigned long len[1];
	unsigned long* lenp[1];
	TUSockAddr addr;
	};

/**
Package which holds client's process Id.
@internalComponent
*/
typedef TPckgBuf<TInt> TPosixIPCPid;

/**
Package used by server to pass data to client.
@internalComponent
*/
struct PosixIPCReply
	{
	TUint iVarCount;
	TUint iEnvironmentSize;
	TUint iWorkingDirectorySize;
	TUint iPipeMask;
	};
/**
@internalComponent
*/
typedef TPckgBuf<PosixIPCReply> TPosixIPCReply;


//
class RPosixSession : public RSessionBase
/**
@internalComponent
*/
	{
public:
	TInt Connect();
	TInt Connect(TDesC& aServerName);
	
	// for specific posix functions
	int Request(TInt aFunction, int& anErrno, PosixParams& params) const;
	void Request(TInt aFunction, int& anErrno, PosixParams& params, TRequestStatus& aStatus) const;
	// for generic messages
	TInt Request(TInt aFunction, const TIpcArgs& aArg) const;
	void Request(TInt aFunction, const TIpcArgs& aArg, TRequestStatus &aStatus) const;
	
	static TVersion Version();
	};


//
class CPosixRequest;
class CPosixServer;
class CPosixIPCSession;
NONSHARABLE_CLASS(CPosixProcess) : public CActive
/**
@internalComponent
*/
	{
public:
	CPosixProcess(CPosixServer& aServer);
	~CPosixProcess();
	void POpen3L(PosixParams* aParams);
	static CPosixProcess* Find(CPosixProcess* head, TInt pid);
	static void Release(CPosixProcess** aHead, CPosixProcess* aProc);
	void Sizes(TPosixIPCReply& aReply) const;
	void CopyToChildL(const RMessage2& aMessage);
	inline TInt IsAlive() const {return IsActive();}
	inline void Queue(CPosixRequest* aWaiter); 
protected:
	void RunL();
	void DoCancel();

private:
	inline CEnvironment& Env() const;
	inline CFileTable& Fids() const;
	inline RFs& Fs() const;
	inline RCommServ& Cs() const;

public:
	CPosixProcess* iNextProcess;
	TInt iPid;
	TInt iExitReason;
private:
	CPosixServer& iServer;

	RProcess iChild;
	CPosixRequest* iWaiters;

	TUint iVarCount;
	HBufC16* iEnvironment;
	HBufC* iWorkingDirectory;
	CPipeDesc* iPipes[3];
	};
//
class CPosixRequest;

/**
@internalComponent
*/
NONSHARABLE_CLASS(CPosixServer) : public CServer2
	{
public:
	static TInt ThreadFunction(TAny* aPtr);
	static void InitL(TInt aPriority);
	RFs& Fs()           { return iFs; }
	RCommServ& Cs()     { return iCs; }
	CFileTable& Fids()  { return iFids; }
	RSocketServ& Ss()   { return iSs; }
	CEnvironment& Env() { return iEnv; }

	int POpen3(PosixParams* aParams, int& anErrno);
	CPosixProcess* Child(TInt pid)	{ return CPosixProcess::Find(iChildren, pid); }
	void Release(CPosixProcess* aChild) { CPosixProcess::Release(&iChildren, aChild); }
	inline void WaitForAnyChild(CPosixRequest* aWaiter);
	CPosixRequest* Waiters();
	static void ServerPanic(TPosixServerPanic aPanic);

protected:
	CPosixServer(TInt aPriority);
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
	void FindParentL();
	void DefaultConsoleL();
private:
	RFs iFs;
	CFileTable iFids;
	RSocketServ iSs;
	RCommServ	iCs;

	CEnvironment iEnv;
	RPosixSession iParent;
	CPosixProcess* iChildren;
	CPosixRequest* iWaitAnyQueue;
	};

//
class CPosixSession;
NONSHARABLE_CLASS(CPosixRequest) : public CActive
/**
@internalComponent
*/
	{
public:
	CPosixRequest(CPosixServer& aServer);
	~CPosixRequest();
	void Service(const RMessage2& aMessage);
	void EnList(CPosixRequest*& aHead);	// simple single linked list
	void WaitCompleted(TInt aPid, TInt aReason);
protected:
	void RunL();
	void DoCancel();
private:
	RFs& Fs() const           { return iServer.Fs(); }
	RCommServ& Cs() const           { return iServer.Cs(); }
	CFileTable& Fids() const  { return iServer.Fids(); }
	RSocketServ& Ss() const   { return iServer.Ss(); }
	CEnvironment& Env() const { return iServer.Env(); }
private:
	void QueueAsynch(const RMessage2& aMessage);
	void StartAsynch();
	void EndAsynch(TInt aResult);

	CPosixServer& iServer;

	RMessage2 iMessage;

	CFileDescBase* iFile;
	CSocketDesc* iNewF;
	int iNewFid;
	TPtr8 iPtr;

	CPosixRequest* iNext;	// for Enlist
	TSglQueLink iLink;
	enum CFileDescBase::IOQueues iQueue;

	friend class TPosixRequestQueue;
	friend class CFileDescBase;		// friend functions AddLast & Remove perhaps? 
	};
//
inline CEnvironment& CPosixProcess::Env() const { return iServer.Env(); }
inline CFileTable& CPosixProcess::Fids() const  { return iServer.Fids(); }
inline RFs& CPosixProcess::Fs() const           { return iServer.Fs(); }
inline RCommServ& CPosixProcess::Cs() const           { return iServer.Cs(); }
inline void CPosixProcess::Queue(CPosixRequest* aWaiter)          { aWaiter->EnList(iWaiters); }
inline void CPosixServer::WaitForAnyChild(CPosixRequest* aWaiter) { aWaiter->EnList(iWaitAnyQueue); }

//
/**
@internalComponent
*/
NONSHARABLE_CLASS(CPosixSession) : public CSession2
	{
public:
	CPosixSession(CPosixServer& aServer);
	virtual void ServiceL(const RMessage2& aMessage);
protected:
	CPosixRequest iActive;
	};

NONSHARABLE_CLASS(CPosixIPCSession) : public CPosixSession
/**
@internalComponent
*/
	{
public:
	inline CPosixIPCSession(CPosixServer& aServer) 
		: CPosixSession(aServer) {}
	virtual void ServiceL(const RMessage2& aMessage);	 // override the local ServiceL
	void SetPipes(CPipeDesc* aPipes[3]);
	~CPosixIPCSession();

private:
	TInt AreYouMyMotherL(const RMessage2& aMessage);
	TInt HelloMumL(const RMessage2& aMessage);
	void PipeRead(const RMessage2& aMessage);
	void PipeWrite(const RMessage2& aMessage);
	void PipeIoctl(const RMessage2& aMessage);
	void PipeClose(const RMessage2& aMessage);
	void PipeCancel(const RMessage2& aMessage);

	CPipeDesc* iPipes[3];
	};
//
NONSHARABLE_CLASS(CProcessSystemInterface) : public MSystemInterface, public CBase
/**
@internalComponent
*/
	{
public:
	CProcessSystemInterface();
	~CProcessSystemInterface();

	TInt Connect();
	virtual MSystemInterface& Clone();
	virtual void Release();
	virtual void TerminateProcess(int status);

	virtual int dup (int fid, int& anErrno);
	virtual int dup2 (int fid, int fid2, int& anErrno);
	virtual int open (const wchar_t* name, int mode, int perms, int& anErrno);
	virtual int read (int fid, char* buf, unsigned long len, int& anErrno);
	virtual int write (int fid, const char* buf, unsigned long len, int& anErrno);
	virtual int fsync (int fid, int& anErrno);
	virtual int close (int fid, int& anErrno);
	virtual int lseek (int fid, int offset, int whence, int& anErrno);
	virtual int fstat (int fid, struct stat *st, int& anErrno);
	virtual int ioctl (int fid, int cmd, void* param, int& anErrno);
	virtual int ioctl (int fid, int cmd, void* param, TRequestStatus& aStatus, int& anErrno);
	virtual int ioctl_complete (int fid, int cmd, void* param, TRequestStatus& aStatus, int& anErrno);
	virtual int ioctl_cancel (int fid, int& anErrno);

	virtual wchar_t * getcwd (wchar_t * buf, unsigned long len, int& anErrno);

	virtual int chdir (const wchar_t* path, int& anErrno);
	virtual int mkdir (const wchar_t* path, int perms, int& anErrno);
	virtual int rmdir (const wchar_t* path, int& anErrno);
	virtual int chmod (const wchar_t* path, int perms, int& anErrno);
	virtual int unlink (const wchar_t* path, int& anErrno);
	virtual int stat (const wchar_t* name, struct stat *st, int& anErrno);
	virtual int rename (const wchar_t* oldname, const wchar_t* newname, int& anErrno);

	virtual TInt ResolvePath (TParse& aResult, const wchar_t* path, TDes* aFilename);

	virtual int socket (int family, int style, int protocol, int& anErrno);
	virtual int recvfrom (int fd, char* buf, unsigned long cnt, int flags, struct sockaddr* from, unsigned long* fromsize, int& anErrno);
	virtual int sendto (int fd, const char* buf, unsigned long cnt, int flags, struct sockaddr* to, unsigned long tosize, int& anErrno);
	virtual int shutdown (int fd, int how, int& anErrno);
	virtual int listen (int fd, int n, int& anErrno);
	virtual int accept (int fd, int& anErrno);
	virtual int bind (int fd, struct sockaddr* addr, unsigned long size, int& anErrno);
	virtual int connect (int fd, struct sockaddr* addr, unsigned long size, int& anErrno);
	virtual int sockname (int fd, struct sockaddr* addr, unsigned long* size, int anEnd, int& anErrno);
	virtual int getsockopt (int fd, int level, int opt, void* buf, unsigned long* len, int& anErrno);
	virtual int setsockopt (int fd, int level, int opt, void* buf, unsigned long len, int& anErrno);

	virtual wchar_t* getenv (const wchar_t* name); 
	virtual void unsetenv (const wchar_t* name);
	virtual int setenv (const wchar_t* name, const wchar_t* value, int rewrite, int& anErrno);

	virtual int popen3 (const wchar_t *file, const wchar_t *cmd, const wchar_t *mode, wchar_t** envp, int fids[3], int& anErrno);
	virtual int waitpid (int pid, int* status, int options, int& anErrno);

private:
	int Request(TInt aFunction, int& anErrno);
	void Request(TInt aFunction, int& anErrno, TRequestStatus& aStatus);
	RPosixSession iSession;
	PosixParams iParams;
	};
