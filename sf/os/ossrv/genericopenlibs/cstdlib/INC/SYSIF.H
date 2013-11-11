// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include <es_sock.h>
#include "ENVIRON.H"
#include <stdlib.h>
#include <c32comm.h>


class MSystemInterface
/**
@internalComponent
*/
	{
public:
	virtual MSystemInterface& Clone()=0;
	virtual void Release()=0;
	virtual void TerminateProcess(int status)=0;

	virtual int dup (int fid, int& anErrno)=0;
	virtual int dup2 (int fid, int fid2, int& anErrno)=0;
	virtual int open (const wchar_t* name, int mode, int perms, int& anErrno)=0;
	virtual int read (int fid, char* buf, unsigned long len, int& anErrno)=0;
	virtual int write (int fid, const char* buf, unsigned long len, int& anErrno)=0;
	virtual int fsync (int fid, int& anErrno)=0;
	virtual int close (int fid, int& anErrno)=0;
	virtual int lseek (int fid, int offset, int whence, int& anErrno)=0;
	virtual int fstat (int fid, struct stat* st, int& anErrno)=0;
	virtual int ioctl (int fid, int cmd, void* param, int& anErrno)=0;
	virtual int ioctl (int fid, int cmd, void* param, TRequestStatus& aStatus, int& anErrno)=0;
	virtual int ioctl_complete (int fid, int cmd, void* param, TRequestStatus& aStatus, int& anErrno)=0;
	virtual int ioctl_cancel (int fid, int& anErrno)=0;

	virtual wchar_t * getcwd (wchar_t * buf, unsigned long len, int& anErrno)=0;
	virtual int chdir (const wchar_t* path, int& anErrno)=0;
	virtual int mkdir (const wchar_t* path, int perms, int& anErrno)=0;
	virtual int rmdir (const wchar_t* path, int& anErrno)=0;
	virtual int chmod (const wchar_t* path, int perms, int& anErrno)=0;
	virtual int unlink (const wchar_t* path, int& anErrno)=0;
	virtual int stat (const wchar_t* name, struct stat* st, int& anErrno)=0;
	virtual int rename (const wchar_t* oldname, const wchar_t* newname, int& anErrno)=0;

	virtual TInt ResolvePath (TParse& aResult, const wchar_t* path, TDes* aFilename)=0;

	virtual int socket (int family, int style, int protocol, int& anErrno)=0;
	virtual int recvfrom (int fd, char* buf, unsigned long cnt, int flags, struct sockaddr* from, unsigned long* fromsize, int& anErrno)=0;
	virtual int sendto (int fd, const char* buf, unsigned long cnt, int flags, struct sockaddr* to, unsigned long tosize, int& anErrno)=0;
	virtual int shutdown (int fd, int how, int& anErrno)=0;
	virtual int listen (int fd, int n, int& anErrno)=0;
	virtual int accept (int fd, int& anErrno)=0;
	virtual int bind (int fd, struct sockaddr* addr, unsigned long size, int& anErrno)=0;
	virtual int connect (int fd, struct sockaddr* addr, unsigned long size, int& anErrno)=0;
	virtual int sockname (int fd, struct sockaddr* addr, unsigned long* size, int anEnd, int& anErrno)=0;
	virtual int getsockopt (int fd, int level, int opt, void* buf, unsigned long* len, int& anErrno)=0;
	virtual int setsockopt (int fd, int level, int opt, void* buf, unsigned long len, int& anErrno)=0;

	virtual wchar_t* getenv (const wchar_t* name)=0; 
	virtual void unsetenv (const wchar_t* name)=0;
	virtual int setenv (const wchar_t* name, const wchar_t* value, int rewrite, int& anErrno)=0;

	virtual int popen3 (const wchar_t *file, const wchar_t *cmd, const wchar_t *mode, wchar_t** envp, int fids[3], int& anErrno)=0;
	virtual int waitpid (int pid, int* status, int options, int& anErrno)=0;
	};

class CFileDescBase;
class CFileTable
/**
@internalComponent
*/
	{
public:
	CFileTable();
	~CFileTable();

	void InitL();
	void Close();
	void Default(CFileDescBase* aConsole);
	TInt Reserve();
	TInt Attach(TInt aFid, CFileDescBase* aFile);
	TInt Detach(TInt aFid, CFileDescBase*& aDetached);
	TInt At(TInt aFid, CFileDescBase*& aFound) const;
	TInt Reserve(TInt aFids[3]);
	TInt Detach(TInt aFids[3]);

	int dup (int fid, int& anErrno);
	int dup2 (int fid, int fid2, int& anErrno);
//	int open (const wchar_t* name, int mode, int perms, int& anErrno, RFs& aFs);
	int open (const wchar_t* name, int mode, int perms, int& anErrno, RSessionBase& aFs);
	int close (int fid, int& anErrno);
	
	int userclose (int fid, int& anErrno);

	int lseek (int fid, int offset, int whence, int& anErrno);
	int fstat (int fid, struct stat* st, int& anErrno);
	int socket (int family, int style, int protocol, int& anErrno, RSocketServ& aSs);
	int listen (int fd, int n, int& anErrno);
	int bind (int fd, TSockAddr& address, int& anErrno);
	int sockname (int fd, TSockAddr& address, int anEnd, int& anErrno);
	int getsockopt (int fd, int level, int opt, void* buf, unsigned long* len, int& anErrno);
	int setsockopt (int fd, int level, int opt, void* buf, unsigned long len, int& anErrno);
	int ioctlcomplete (int fid, int cmd, void* param, TRequestStatus& aStatus, int& anErrno);
	int ioctlcancel (int fid, int& anErrno);

	TInt Asynch (int fid, CFileDescBase*& aFile);
	
private:
	CArrayPtrSeg<CFileDescBase> iFids;
	TInt Dup(TInt& aFid);
	TInt Dup2(TInt aFid, TInt aFid2);
	void Release(TInt aFid);
	};

class PosixFilesystem
/**
@internalComponent
*/
	{
public:
	static wchar_t* getcwd (RFs& aFs, wchar_t* buf, unsigned long len, int& anErrno);
	static int chdir (RFs& aFs, const wchar_t* path, int& anErrno);
	static int mkdir (RFs& aFs, const wchar_t* path, int perms, int& anErrno);
	static int rmdir (RFs& aFs, const wchar_t* path, int& anErrno);
	static int chmod (RFs& aFs, const wchar_t* path, int perms, int& anErrno);
	static int unlink (RFs& aFs, const wchar_t* path, int& anErrno);
	static int stat (RFs& aFs, const wchar_t* name, struct stat* st, int& anErrno);
	static int rename (RFs& aFs, const wchar_t* oldname, const wchar_t* newname, int& anErrno);
	static TInt ResolvePath (RFs& aFs, TParse& aResult, const wchar_t* path, TDes* aFilename);
	static TInt SetDefaultDir (RFs& aFs);
	};

NONSHARABLE_CLASS(CLocalSystemInterface) : public CBase, public MSystemInterface
/**
To make CleanupStack::PushL() and CleanupStack::Pop() work properly in the 2-phase constructor, 
derivation order must be: public CBase, public MSystemInterface.

@internalComponent
*/
	{
public:
	static CLocalSystemInterface* NewL();	
	~CLocalSystemInterface();

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
	virtual int fstat (int fid, struct stat* st, int& anErrno);
	virtual int ioctl (int fid, int cmd, void* param, int& anErrno);
	virtual int ioctl (int fid, int cmd, void* param, TRequestStatus& aStatus, int& anErrno);
	virtual int ioctl_complete (int fid, int cmd, void* param, TRequestStatus& aStatus, int& anErrno);
	virtual int ioctl_cancel (int fid, int& anErrno);

	virtual wchar_t* getcwd (wchar_t* buf, unsigned long len, int& anErrno);


	virtual int chdir (const wchar_t* path, int& anErrno);
	virtual int mkdir (const wchar_t* path, int perms, int& anErrno);
	virtual int rmdir (const wchar_t* path, int& anErrno);
	virtual int chmod (const wchar_t* path, int perms, int& anErrno);
	virtual int unlink (const wchar_t* path, int& anErrno);
	virtual int stat (const wchar_t* name, struct stat* st, int& anErrno);
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
	CLocalSystemInterface();	
	void ConstructL();		
	
private:
	TInt iUseCount;
	CFileTable iFids;
	RFs iFs;
	RSocketServ iSs;

	RCommServ	iCs;
	
	CEnvironment iEnv;
	};

#include <sys/reent.h>
/**
@internalComponent
*/
inline MSystemInterface& Interface(struct _reent* r)
	{ return (*REINTERPRET_CAST(MSystemInterface*,r->_system)); }
