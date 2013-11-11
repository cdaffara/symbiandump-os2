/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#ifndef SYSIF_H
#define SYSIF_H
#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include <es_sock.h>
#include <c32comm.h>
#include <sys/select.h>
#include <spawn.h>
#include <sys/types.h>
#include <wchar.h>
#include "ipcclient.h"
#include <dirent.h>
#include <sys/socket.h>
#include <stdapis/net/if.h>
#include <commdb.h>
#include <commdbconnpref.h>
#include <rpipe.h>

#include<tz.h>
#include <e32atomics.h>

#ifdef SYMBIAN_OE_POSIX_SIGNALS
#include "signalclient.h"
#include "tsignalmessage.h"

typedef void (*TSignalHandler)(int);
typedef void (*TSignalAction)(int, siginfo_t *, void *);
#endif

/*
@internalComponent
*/
_LIT(KEstlibInit, "STDLIBS-INIT");

_LIT(KFDTransferSvr,"FDTrnsfrSvr");
//Forward declaration for references below
class RPipe;
class CFileDesTransferServer;
class RFileDesTransferSession;
class CFileDescBase;
/* TODO: We don't have to do this in CFileTable. Move to methods */
class CFileSocketDesc;
class CSocketDesc;
struct TChild
/*
@internalComponent
*/
	{
	TInt iFid;
	TProcessId iPid;
	RProcess iProc;

#ifdef __X86GCC__
	// The underlying data for a TProcessId is a TUint64. GCC raises an error if this is intialised to a negative value. Consider changing for all platforms.
	TChild(TInt aFid = -1, TProcessId aPid = 0) : iFid(aFid), iPid(aPid)
#else
	TChild(TInt aFid = -1, TProcessId aPid = -1) : iFid(aFid), iPid(aPid)
#endif //__X86GCC__
		{
		// nada
		}

#ifdef __X86GCC__
	// The underlying data for a TProcessId is a TUint64. GCC raises an error if this is intialised to a negative value. Consider changing for all platforms.
	TChild(RProcess &aProc, TProcessId aFid = 0) : iProc(aProc), iFid(aFid)
#else
	TChild(RProcess &aProc, TProcessId aFid = -1) : iProc(aProc), iFid(aFid)
#endif //__X86GCC__
		{
		iPid = aProc.Id();
		}


	static TBool MatchByFid(const TChild& aChild1, const TChild& aChild2)
		{
		if (aChild1.iFid == aChild2.iFid)
			{
			return ETrue;
			}
		return EFalse;
		}

	static TBool MatchByPid(const TChild& aChild1, const TChild& aChild2)
		{
		if (aChild1.iPid == aChild2.iPid)
			{
			return ETrue;
			}
		return EFalse;
		}
	};

class TCLSICleanup
/*
@internalComponent
*/
	{
public:
	void StorePtrs(RHeap* aHeap, RFs* aFs, RSocketServ* aSs, RCommServ* aCs, RFastLock* aSsLock, RFastLock* aCsLock,RFastLock* aDefConnLock,RFastLock* aAESelectLock,RFastLock* aTzServerLock)
		{
		iHeap = aHeap;
		iFs = aFs;
		iSs = aSs;
		iCs = aCs;
		iSsLock = aSsLock;
		iCsLock = aCsLock;
		iDefConnLock = aDefConnLock;
		iAESelectLock = aAESelectLock;
		iTzServerLock = aTzServerLock;
		}

	~TCLSICleanup()
		{
		iFs->Close();
		iSs->Close();
		iSsLock->Close();
		iCs->Close();
		iCsLock->Close();
		iHeap->Close();
		iDefConnLock->Close();
		iAESelectLock->Close();
		iTzServerLock->Close();
		}
private:
	RHeap* iHeap;
	RFs* iFs;
	RSocketServ* iSs;
	RCommServ* iCs;
	RFastLock* iSsLock;
	RFastLock* iCsLock;
	RFastLock* iDefConnLock;
	RFastLock* iAESelectLock;
	RFastLock* iTzServerLock;
	};


class TFileTableCleanup
{
public:
	void SaveUserHeap(RHeap* aHeap)
		{
		iUserHeap = aHeap;
		}

	~TFileTableCleanup()
	{
		User::SwitchHeap(iUserHeap);
	}

private:
	RHeap* iUserHeap;
};


class CFileTable // codescanner::missingcclass
/*
@internalComponent
*/
	{
public:
	CFileTable();
	~CFileTable();

	TInt Init(RHeap* aHeap = NULL);
	void Close();
	void Default(CFileDescBase* aRedirMedia, CFileDescBase* aStdErrMedia);
	TInt Reserve();
	TInt Attach(TInt aFid, CFileDescBase* aFile);
	TInt Detach(TInt aFid, CFileDescBase*& aDetached);
	TInt At(TInt aFid, CFileDescBase*& aFound) const;
	TInt Reserve(TInt aFids[3]);
	TInt Detach(TInt aFids[3]);

	int dup (int fid, int& anErrno);
	int dup2 (int fid1, int fid2, int& anErrno);
	int open (const wchar_t* name, int mode, int perms, int& anErrno);
	int close (int fid, int& anErrno);

	int UserClose (int fid, int& anErrno);

	off_t lseek (int fid, off_t offset, int whence, int& anErrno);
	int fstat (int fid, struct stat* st, int& anErrno);
	int socket (int family, int style, int protocol, int& anErrno, RSocketServ& aSs);
	int listen (int fd, int n, int& anErrno);
	int bind (int fd, const struct sockaddr* addr, unsigned long size, int& anErrno);
	int sockname (int fd, struct sockaddr* address, unsigned long* aAddrLen,int anEnd, int& anErrno);
	int getsockopt (int fd, int level, int opt, void* buf, unsigned long* len, int& anErrno);
	int setsockopt (int fd, int level, int opt, void* buf, unsigned long len, int& anErrno);
	int ioctlcomplete (int fid, int cmd, void* param, TRequestStatus& aStatus, int& anErrno);
	int ioctlcancel (int fid, int& anErrno);
	int fcntl (int aFid, int aCmd, long anArg, int& anErrno);

	TInt Asynch (int fid, CFileDescBase*& aFile);

	int popen (const wchar_t* file, const wchar_t* cmd,const char* mode, int& anErrno);
	int pclose (int aFid, int& anErrno);
	int popen3 (const wchar_t *file, const wchar_t *cmd, wchar_t** envp, int fids[3], int& anErrno);
	int system (const wchar_t* aCmd, const wchar_t* aCmdArg, int& anErrno);
	int waitpid (int pid, int* status, int options, int& anErrno);

	int posix_spawn (int* pid, const wchar_t* wpath,
					 const posix_spawn_file_actions_t* file_actions,
					 const posix_spawnattr_t* attrp,
					 const wchar_t* wargs, wchar_t** wenvp);

	void TransferToChild (RProcess& child, TInt aFds[], TInt aWhich, TInt aSlot);
	void PassEnvToChild (RProcess& child, wchar_t** aEnvp);
	void PassFileActionsToChild (RProcess& child,
								 const posix_spawn_file_actions_t* aFileActions);

#ifdef SYMBIAN_OE_POSIX_SIGNALS
	void TransferSigMaskToChild(RProcess& childproc,TDes& sigmask);
	void CheckOrigins (wchar_t**& wenvp, int& aCount, TUint64 & aSigMask);
#else
	void CheckOrigins (wchar_t**& wenvp, int& aCount);
#endif
	TInt InheritFiles(CFileDesTransferServer* server);
	int pipe (int fildes[2], int& anErrno);
	int mkfifo (const wchar_t *pathname, mode_t mode, int& anErrno, RFs& aFs);
	int CreatePipeDesc (int fildes, RPipe& aHandle, TUint16 aMode = 0);
	void ClosePipe (int fds[2], int& err);

	static CFileDescBase* FifoOpen (const wchar_t* pathName, mode_t mode, int perms, int& anErrno);
	int link (const wchar_t *oldpathName, const wchar_t *newpathName, int& anErrno, RFs& aFs);
	static CFileDescBase* LinkOpen (const wchar_t* pathName, mode_t mode, int perms, int& anErrno, RFs& aFs);
	int unlink (RFs& aFs, const wchar_t *pathName, int& anErrno);
	int AcceptFileSocket(CFileSocketDesc* pfs, const struct sockaddr_un* addr, int& anErrno, RFs& aFs);
	TInt RConnectionCount();
	TInt RConnectionAt (TInt aRcIndex, RConnection *&aRc);
	TInt AddRConnectionPtr (RConnection *aRc, TInt &aRcIndex);
	void RemoveRConnectionPtrAt (TInt aRcIndex);
	void DeleteRConnectionList();

	CFileDesTransferServer* NewFDTransferServerL(RSemaphore &sem);
	void StartFDTransferServer(CFileDesTransferServer* pServer);
	void CloseFDTransferServer(CFileDesTransferServer* pServer);

	int SetEcho(int aFd, TUint8 aEcho, int& aErrno);

	void DoTransferAll(RSemaphore &sem,TDesC& name);
	TInt GetFileCount() const
		{
		return iFids.Count();
		}

	void Lock()
		{
		iFidLock.Wait();
		}

	void Unlock()
		{
		iFidLock.Signal();
		}

private:
	TFileTableCleanup	iCleanup;
	CArrayPtrSeg<CFileDescBase> iFids;
	// invoked by dup
	TInt Dup(TInt aFid);
	// invoked by both dup2 and fcntl with F_DUPFD
	TInt Dup2(TInt aFid, TInt aFid2, TBool aCloseFid2);
	// the real dup-er.
    TInt DupFd(TInt aFid1, TInt aFid2);
	void Release(TInt aFid);

	//Lock for protecting FIDs across threads
	RFastLock iFidLock;

	//stores pids of all children of this process
	RArray<TChild> iChildren;

	//Methods to manipulate iChildren
	void AddChild(const TChild& aChild);
	void RemoveChild(TInt aIdx);

	void ExpandFTableL(TInt count);

	const RArray<TChild>& GetChildren() const
		{
		return iChildren;
		}

	TInt FindChild(const TInt aFid) const
		{
		return iChildren.Find(TChild(aFid),
							  TIdentityRelation<TChild>(TChild::MatchByFid));
		}

	TInt FindChild(const TProcessId aPid) const
		{
		return iChildren.Find(TChild(-1, aPid),
							  TIdentityRelation<TChild>(TChild::MatchByPid));
		}

	CArrayPtrSeg<RConnection> iRConnectionList;
	//Lock for protecting iRConnectionList across threads
	RFastLock iRcLock;

	// A pointer to the private heap
	RHeap* iPrivateHeap;
	friend class CFileDesTransferSession;
	};


class PosixFilesystem
/*
@internalComponent
*/
	{
public:
	static wchar_t* getcwd(RFs& aFs, wchar_t* buf, unsigned long len, int& anErrno);
	static int chdir(RFs& aFs, const wchar_t* path, int& anErrno);
	static int mkdir(RFs& aFs, const wchar_t* path, int perms, int& anErrno);
	static int rmdir(RFs& aFs, const wchar_t* path, int& anErrno);
	static int chmod(RFs& aFs, const wchar_t* path, int perms, int& anErrno);
	static int reg_unlink(RFs& aFs, const wchar_t* path, int& anErrno);
	static int statbackend(RFs& aFs, const wchar_t* name, struct stat* st, int& anErrno);
	static int rename(RFs& aFs, const wchar_t* oldname, const wchar_t* newname, int& anErrno);
	static TInt ResolvePath(TParse& aResult, const wchar_t* path, TDes* aFilename);
	static TInt SetDefaultDir(RFs& aFs);
	static TInt CreateDefaultPrivatePath(RFs& aFs);
	static int utime(RFs& aFs, const wchar_t* name, const struct utimbuf* filetimes, int& anErrno);
	};


class TAccessPointRecord
	{
public:	
	TUint32 iId;
	TBuf<KCommsDbSvrMaxColumnNameLength> iName;
	TUint32 iDialogPref;
	TUint32 iDirection;
	TUint32 iService;
	TBuf<KCommsDbSvrMaxColumnNameLength> iServiceType;
	TUint32 iBearer;
	TBuf<KCommsDbSvrMaxColumnNameLength> iBearerType;
	TUint32 iNetwork;
	TUint32 iNetworkWeighting;
	TUint32 iLocation;
public:
	TAccessPointRecord();
	
	};

// This structure encapsulates an aselect request
NONSHARABLE_STRUCT ( TaselectRequestInfo )
	{
	TThreadId iId;
	int iMaxFd;
	fd_set* iReadFds;
	fd_set* iWriteFds;
	fd_set* iExceptFds;
	struct timeval *itvptr;
	TRequestStatus* iRequestStatus;
	};

// Holds the information about aselect requests, this used required
// for cancellation of the aselect requests
NONSHARABLE_CLASS ( TASelectRequest )
	{
public:
	// Thread id of the thread processing the aselect
	TThreadId iThreadId;
	// Request object used for the request
	TRequestStatus* iRequestStatus;
	};

NONSHARABLE_CLASS(CLocalSystemInterface) : public CBase
/*
@internalComponent
*/
	{
public:


	IMPORT_C CLocalSystemInterface();
	IMPORT_C ~CLocalSystemInterface();

#ifdef __SYMBIAN_COMPILE_UNUSED__
	CLocalSystemInterface& Clone();
	void Release();
	void TerminateProcess(int status);
#endif //__SYMBIAN_COMPILE_UNUSED__

	int AddToDirList(DIR *aDir);
	int RemoveDirFromList(DIR* aDir);
	int FindInDirList(DIR* aDir);

	void Exit(int code);

	int dup (int fid, int& anErrno);
	int dup2 (int fid, int fid2, int& anErrno);
	int open (const wchar_t* name, int mode, int perms, int& anErrno);
	int read (int fid, char* buf, unsigned long len, int& anErrno);
	int write (int fid, const char* buf, unsigned long len, int& anErrno);
	int fsync (int fid, int& anErrno);
	int close (int fid, int& anErrno);
	off_t lseek (int fid, off_t offset, int whence, int& anErrno);
	int fstat (int fid, struct stat* st, int& anErrno);
	int ioctl (int fid, int cmd, void* param, int& anErrno);
	int ioctl (int fid, int cmd, void* param, TRequestStatus& aStatus, int& anErrno);
	int ioctl_complete (int fid, int cmd, void* param, TRequestStatus& aStatus, int& anErrno);

#ifdef __SYMBIAN_COMPILE_UNUSED__
	int ioctl_cancel (int fid, int& anErrno);
#endif //__SYMBIAN_COMPILE_UNUSED__

	wchar_t* getcwd (wchar_t* buf, unsigned long len, int& anErrno);

	int chdir (const wchar_t* path, int& anErrno);
	int mkdir (const wchar_t* path, int perms, int& anErrno);
	int rmdir (const wchar_t* path, int& anErrno);
	int chmod (const wchar_t* path, int perms, int& anErrno);
	int stat (const wchar_t* name, struct stat* st, int& anErrno);
	int lstat (const wchar_t* name, struct stat *st, int& anErrno);
	int rename (const wchar_t* oldname, const wchar_t* newname, int& anErrno);

	IMPORT_C TInt ResolvePath (TParse& aResult, const wchar_t* path, TDes* aFilename);
	int socket (int family, int style, int protocol, int& anErrno);
	int recvfrom (int fd, char* buf, unsigned long cnt, int flags, struct sockaddr* from, unsigned long* fromsize, int& anErrno);
	int sendto (int fd, const char* buf, unsigned long cnt, int flags, struct sockaddr* to, unsigned long tosize, int& anErrno);
	int shutdown (int fd, int how, int& anErrno);
	int listen (int fd, int n, int& anErrno);
	int accept (int fd, struct sockaddr *addr, size_t *size, int& anErrno);
	int bind (int fd, const struct sockaddr* addr, unsigned long size, int& anErrno);
	int connect (int fd, const struct sockaddr* addr, unsigned long size, int& anErrno);
	int sockname (int fd, struct sockaddr* addr, unsigned long* size, int anEnd, int& anErrno);
	int getsockopt (int fd, int level, int opt, void* buf, unsigned long* len, int& anErrno);
	int setsockopt (int fd, int level, int opt, void* buf, unsigned long len, int& anErrno);

	int popen3 (const wchar_t *file, const wchar_t *cmd, wchar_t** envp, int fids[3], int& anErrno);
	int waitpid (int pid, int* status, int options, int& anErrno);

	int fcntl (int aFid, int aCmd, long anArg, int& anErrno);
	int utime (const wchar_t* name, const struct utimbuf* filetimes, int& anErrno);

	int popen (const wchar_t* file, const wchar_t* cmd, const char* mode, int& anErrno);
	int pclose (int aFid, int& anErrno);
	IMPORT_C void CheckOrigins (wchar_t**& wenvp, int& aCount);
#ifdef SYMBIAN_OE_POSIX_SIGNALS
	TInt SignalHandler();
	IMPORT_C void InitSignalHandler();
	void RegisterExitNotification(TProcessId aPid);

	inline TUint64 GetSigMask()const
	{
		return iSigMask;
	}

	int kill(pid_t pid, int sig, int& anErrno);
	int raise(int sig, int& anErrno);
	int sigqueue(pid_t pid, int sig, const union sigval val, int& anErrno);

	int sigfillset(sigset_t *set, int& anErrno);
	int sigaddset(sigset_t *set, int signo, int& anErrno);
	int sigdelset(sigset_t *set, int signo, int& anErrno);
	int sigismember(const sigset_t *set, int signo, int& anErrno);
	int sigandset(sigset_t * set, const sigset_t * left, const sigset_t * right, int& anErrno);
	int sigorset(sigset_t * set, const sigset_t * left, const sigset_t * right, int& anErrno);
	int sigisemptyset(const sigset_t * set, int& anErrno);
	int sigemptyset(sigset_t * set, int& anErrno);

	int sigprocmask(int how, const sigset_t *set, sigset_t *oset, int& anErrno);
	int sighold(int signo, int& anErrno);
	int sigrelse(int signo, int& anErrno);
	int sigpause(int signo, int& anErrno);

	int sigwait(const sigset_t *set, int *sig, int& anErrno);
	int sigtimedwait(const sigset_t *set, siginfo_t *info, const struct timespec *timeout, int& anErrno);
	int sigwaitinfo(const sigset_t *set, siginfo_t *info, int& anErrno);
	int sigaction(int sig, const struct sigaction *act, struct sigaction *oact, int& anErrno);
	TSignalHandler sigset(int aSigNum, TSignalHandler disp, int& anErrno);
	TSignalHandler bsd_signal(int aSigNum, TSignalHandler aFunc,int& anErrno);
	TSignalHandler signal(int aSigNum, TSignalHandler aFunc,int& anErrno);
	int sigignore(int aSigNum, int& anErrno);
	unsigned int alarm(unsigned int seconds);
	int sigpending(sigset_t *set,int& anErrno);

#ifdef SYMBIAN_OE_LIBRT	
	//start timers...
	IMPORT_C TInt AddTimer(const TInt& aTimerId);
	IMPORT_C TInt DeleteTimer(const TInt& aTimerId);
	IMPORT_C TInt IncrementOverrun(const TInt& aTimerId);
	IMPORT_C TInt ResetOverrun(const TInt& aTimerId);
	IMPORT_C TInt Overrun(const TInt& aTimerId);
	IMPORT_C TInt RaiseTimerSignal(int sig, int timerid);
	//end timers
#endif //SYMBIAN_OE_LIBRT
#endif // SYMBIAN_OE_POSIX_SIGNALS
    int pipe (int fildes[2], int& anErrno );
    int mkfifo (const wchar_t *pathname, mode_t mode, int& anErrno);
    int link (const wchar_t *oldpathName, const wchar_t *newpathName, int& anErrno );
    int unlink (const wchar_t *pathName, int& anErrno );

	int select (int maxfd, fd_set* readfds,fd_set* writefds,fd_set* exceptfds, struct timeval* tvptr, int& anErrno);

	int aselect(int maxfd, fd_set *readfds, fd_set *writefds,fd_set *exceptfds, struct timeval *tvptr, TRequestStatus* requeststatus,int& anErrno);

	int cancelaselect(TRequestStatus* requeststatus, int& anErrno, TBool perform_cleanup = EFalse);		
		
	int eselect(int maxfd, fd_set *readfds, fd_set *writefds,fd_set *exceptfds, struct timeval *tvptr, int arraycount,TRequestStatus* waitarray,int& anErrno);
	
	static TInt handleaselect(TAny* aPtr);
	
	RArray<TASelectRequest>& ASelectRequest();
	
	RFastLock& ASelectLock();
	
	inline RFastLock& DefConnLock() { return iDefConnLock; }
	
	inline TInt AddSocket(CSocketDesc* aPtr)
	    {
        RHeap* oheap = User::SwitchHeap(iPrivateHeap);
	    TInt ret = iSocketArray.InsertInAddressOrder(aPtr);
	    User::SwitchHeap(oheap);	    
	    return ret;
	    }
	
	inline void RemoveSocket(CSocketDesc* aPtr)
	    {
	    TInt index = iSocketArray.FindInAddressOrder(aPtr);
	    if (index != -1)
	        {
			RHeap* oheap = User::SwitchHeap(iPrivateHeap);
            iSocketArray.Remove(index);            
            iSocketArray.Compress();
            User::SwitchHeap(oheap);
	        }
	    }
	
	int system (const wchar_t* aCmd, const wchar_t* aCmdArg, int& anErrno );
	const wchar_t* GetDirName (int aFid);
	IMPORT_C int Truncate (int aFid, off_t anOffset, int& anErrno);
	int fchmod (int fd , mode_t  perms, int& anErrno);

	int posix_spawn (int* pid, const wchar_t* wpath,
					 const posix_spawn_file_actions_t* file_actions,
					 const posix_spawnattr_t* attrp,
					 const wchar_t* wargs, wchar_t** wenvp);

	int SetEcho(int aFd, TUint8 aEcho, int& aErrno);
	
	//Set the default interface for network operations
	int setdefaultif(const struct ifreq* aIfReq);
	int unsetdefaultif(TBool allow_bringup = ETrue);
	
	IMPORT_C CFileDescBase* GetDesc(int aFid);

	// functions returning session to File Server
	IMPORT_C RFs& FileSession();
    RCommServ& CommServSession();
    RSocketServ& SockServSession();

	//Dynamic Memory related APIs
	IMPORT_C void* Alloc(size_t aNBytes);
	IMPORT_C void* ReAlloc(void* aPtr, size_t aNBytes);
	IMPORT_C void Free(void* aPtr);
	IMPORT_C void* AllocTLD(TInt aSize);

	//Return the private Heap memory
	IMPORT_C RHeap* Heap();

	//Return the Comms Server Lock
	RFastLock& CommsSessionLock();

	//Return reference to file table
	const CFileTable& FileTable() const;
	
	//Get the default connection (RConnection) instance 
	RConnection& GetDefaultConnection();
	
	//Get the saved session path of the file server session
	TInt GetSessionPath(TDes& aPath);
	
	//Save the file server session path
	TInt SaveSessionPath(const TDesC& aPath);
	
	static void WaitForNRequest(TRequestStatus aStatusArray[], TInt aNum);

private:
	//Helper function to read a record from the IAP table
	void ReadRecordFromIapTableL(CCommsDbTableView* aView, TAccessPointRecord &aRecord);
	//Get the connection preferences from an IAP name
	TInt GetConnectionPreferencesL(TBuf<KCommsDbSvrMaxColumnNameLength> aIapName, 
			 					   TCommDbConnPref &aConnPref);

	// Helper function used by "select" api. 
	// The function returns "True", if an ioctl call has been made and it is to be waited upon.
	// The function returns "False", (if an ioctl call is not made)/(an ioctl call was made but is not to be waited upon, and will get cancelled).	
	TBool SelectPollIssueIoctl(fd_set** aInputFds, fd_set** aOutputFds, 
			TRequestStatus* aReqArray, TInt *aReqFds, TInt aNumReqs, TInt& nDescriptorsSet) ;
	TInt CreateaselectThread(int maxfd, fd_set* readfds, fd_set* writefds,
				fd_set* exceptfds, struct timeval* tvptr, 
				TRequestStatus* requeststatus, int& anErrno);
	
	// Helper function for the setdefaultif() API to restart the 
	// default RConnection with the new settings.
	TInt StartDefConnection();
	
	// Helper function for doing an on-demand connection to RTz server
	TInt OnDemandTZServerConnection();
private:
	// NOTE: iCleanup should be the first member of CLSI, since iPrivateHeap
	// will be destroyed from within iCleanup destructor.
	TCLSICleanup	iCleanup;

	CFileTable 		iFids;

	//The backend file session for the application
	RFs 			iFs;
	//The current session path of the application.
	TFileName 		iSessionPath;
	//Lock to guard the session path
	RFastLock 		iSessionPathLock;
	
	RSocketServ 	iSs;
	RCommServ 		iCs;

	//Lock for protecting Socket Session across threads
	RFastLock 		iSSLock;
	//Lock for protecting Comms Session across threads
	RFastLock 		iCSLock;
	
	//Private Heap which will be used by the library for all memory allocation
	RHeap* 			iPrivateHeap;

	//used for Opendir
	const static TInt KDirGran = 2;
	RPointerArray<TAny> iOpenDirList;

	//in class consts
	const static TInt KTLDInfoListGran = 4;
	const static TInt KPtrsGran = 2;

	//Thread-TLS pointers mapping
	class TTLDInfo
		{
	public:
		TTLDInfo(TThreadId id, TAny* ptr) : iTid(id), iPtrs(CLocalSystemInterface::KPtrsGran)
			{
			iPtrs.Append(ptr);
			}
		void Close(RHeap* pHeap)
			{
			TInt nptrs = iPtrs.Count();
			for (TInt j = 0; j < nptrs; ++j)
				{
				pHeap->Free(iPtrs[j]);
				}
			iPtrs.Close();
			}
		TThreadId iTid;
		RPointerArray<TAny> iPtrs;
		};

	//Array of Thread-TLS pointers mappings
	RArray<TTLDInfo> iTLDInfoList;

	//Protect the array from concurrent access
	RFastLock	iTLDListLock;
	//The default connection to be used for all network apis. 
	RConnection iDefConnection;
	//Protect the iDefConnection from concurrent GetDefaultConnection calls
	RFastLock   iDefConnLock;
	//Calling unsetdefaultif(), instead of setdefaultif(NULL) ensures that the torn down
	//connection is not brought back up again, by subsequent socket/network calls.
	//This variable enables this facility. It is reset to default (ETrue) by setdefaultif(<pref>)
	TBool iDefConnResurrect;
	//Lock for protecting iASelectRequest across threads
	RFastLock iASelectLock;
		
	// iASelectRequest holds the TASelectRequest objects of the aselect requests
	RArray<TASelectRequest> iASelectRequest;
	
	// Default connection settings, set/cleared using setdefaultif
	TConnPref* iDefConnPref;
    RTz     iTzServer;	
	RFastLock iTzServerLock;
	TBool	iIsRTzConnected;
    RPointerArray<CSocketDesc> iSocketArray;
#ifdef SYMBIAN_OE_POSIX_SIGNALS
	// Signal handler thread
	RThread 				iSignalHandlerThread;
	RPipe					iSignalReadPipe;
	RPipe					iSignalWritePipe;
	TBool					iSignalLoopRunning;
	RSignalSession			iSignalSession;
	TUint64					iSigMask;
	RArray<TSignalMessage>	iBlockedSigArray;
	RMutex					iBlockedSAMutex;
	TRequestStatus			iPipeReadStatus;

	// For thread(s) to wait till signals initialization happens
	TInt					iSigInitWaitCount;
	RSemaphore				iSigInitWaitSemaphore;
	RMutex					iSigInitWaitMutex;
	TBool					iSignalsInitialized;
	
	RSemaphore				iSignalInitSemaphore;

	class TChildWaiter
		{
	public:
		TChildWaiter()
			{
			iWaiterStatus = KRequestPending;
			iWaiterPid = 0;
			}

		virtual ~TChildWaiter() {}

		TRequestStatus iWaiterStatus;
		TProcessId iWaiterPid;

		static TBool MatchByPid(const TChildWaiter& aWaiter1, const TChildWaiter& aWaiter2)
			{
			if((aWaiter1.iWaiterPid == aWaiter2.iWaiterPid) != 0)
				return ETrue;
			else
				return EFalse;
			}

		static TBool MatchNotPending(const TChildWaiter& aWaiter1, const TChildWaiter& aWaiter2)
			{
			if(aWaiter1.iWaiterStatus == KRequestPending || aWaiter2.iWaiterStatus == KRequestPending)
				return EFalse;
			else
				return ETrue;
			}
		};

	class TSignalActionInfo
		{
	public:
		TSignalActionInfo()
			{
			iSigHandler = SIG_DFL;
			iSigAction = 0;
			iSigFlags = 0;
			}

		virtual ~TSignalActionInfo() {}

		TSignalHandler 	iSigHandler;
		TSignalAction	iSigAction;
		TUint32			iSigFlags;
		};

	TFixedArray<TSignalActionInfo,SIGRTMAX> iSigActions;
	RMutex          		iSigHandlerMutex;
	RTimer 					iAlarmTimer;
    TTime 					iAlarmStartTime;
    TRequestStatus 			iAlarmStatus;
    TBool 					iIsTimerActive;
    TUint					iCurrentTimeout;

    void HandleSignal(TSignalMessage& aMsg, TBool aIsDequeued = EFalse);
	void InitializeSignalHandlers();
	void CallDefaultHandler(int aSigNum);
	void WaitOrHandleSignal(TSignalMessage aMsg);
	TInt SuspendOtherThreads();
	TInt ResumeOtherThreads();
	TInt EnsureSignalsInitialized();
	TInt SigWaitInternal(TUint64 aWaitSet,TSignalMessage* aMsg,TInt aTimeout,TBool aTimeCheckFailed=EFalse);

	class TSignalWaiter
		{
	public:
		TSignalWaiter(TUint64 aWaitMask, TSignalMessage* aMsgPtr)
			{
			iSigWaitSemaphore.CreateLocal(0);
			iSigWaitMessagePtr = aMsgPtr;
			iSigWaitMask = aWaitMask;

			RThread	lThread;
			iSigWaitThreadId = lThread.Id();
			lThread.Close();
			}

		virtual ~TSignalWaiter()
			{
			iSigWaitSemaphore.Close();
			}

		TUint64				iSigWaitMask;
		RSemaphore			iSigWaitSemaphore;
		TSignalMessage*		iSigWaitMessagePtr;
		TThreadId			iSigWaitThreadId;

		static TBool MatchByMask(const TSignalWaiter& aWaiter1, const TSignalWaiter& aWaiter2)
			{
			if((aWaiter1.iSigWaitMask & aWaiter2.iSigWaitMask) != 0)
				return ETrue;
			else
				return EFalse;
			}

		static TBool MatchByThreadId(const TSignalWaiter& aWaiter1, const TSignalWaiter& aWaiter2)
			{
			if(aWaiter1.iSigWaitThreadId == aWaiter2.iSigWaitThreadId)
				return ETrue;
			else
				return EFalse;
			}
		};

	RArray<TSignalWaiter>	iSignalWaiterArray;
	RArray<TChildWaiter>	iChildWaiterArray;
	RMutex					iSignalWaiterMutex;

#ifdef SYMBIAN_OE_LIBRT		
	//timers..
	TInt FindTimer(const TInt& aTimerId);
	class TOverrun
		{
		public:
			TOverrun(const TInt& aTimerId):iTimerId(aTimerId), Overruns(0) 
				{
				}

		TInt iTimerId;
		TInt Overruns;
		};
		
	RArray<TOverrun> 		iTimerOverruns;
	RMutex					iTimerOverrunsMutex;
	//timers..
#endif // SYMBIAN_OE_LIBRT		
	
#endif // SYMBIAN_OE_POSIX_SIGNALS
public:

	IMPORT_C RTz & TZServer(TInt& aStatus);
	
//ipc server session
RIpcSession iIpcS;
friend class RFileDesTransferSession;
	};


/*
* Return global backend object to libraries
*/
IMPORT_C CLocalSystemInterface* Backend();




// LIBC-BACKEND specific Symbian Error Codes
const TInt KErrMaxFdOpened = -1025;
const TInt KErrDirectoryOpen = -1026;



// Directory enumeration

NONSHARABLE_STRUCT(__EPOC32_WDIR) : public CBase	// aka "class __EPOC32_WDIR with everything public"
	{
public:
	__EPOC32_WDIR() {}
	~__EPOC32_WDIR();

	TInt Open();
	TInt Open(const TDesC& aPath);
	TInt Open(const wchar_t *_path,int*);
	virtual TInt UpdateNarrow();
	HBufC* iPath;
	CDir* iEntries;
	TInt iIndex;		// counting down, 0 means "finished"
	struct wdirent iCurrent;
	TInt iCount ;
	TBuf16<KMaxFileName> iCurrentName;
	/* *****************************************************************
	Overloading new and delete operators so that they will
	allocate and deallocare memory from/to the private heap of backend
	********************************************************************/
	inline TAny* operator new(TUint aSize, TAny* aBase) __NO_THROW
		{
		Mem::FillZ(aBase, aSize); return aBase;
		}

	inline TAny* operator new(TUint aSize) __NO_THROW
		{
		return Backend()->Alloc(aSize);
		}

	inline TAny* operator new(TUint aSize, TLeave)
		{
		TAny* ptr = Backend()->Alloc(aSize);
		if (ptr == NULL)
			{
			User::Leave(KErrNoMemory);
			}
		return ptr;
		}

	inline TAny* operator new(TUint aSize, TUint aExtraSize) __NO_THROW
		{
		return Backend()->Alloc(aSize + aExtraSize);
		}

	inline TAny* operator new(TUint aSize, TLeave, TUint aExtraSize)
		{
		TAny* ptr = Backend()->Alloc(aSize + aExtraSize);
		if (ptr == NULL)
			{
			User::Leave(KErrNoMemory);
			}
		return ptr;
		}

	inline void operator delete(TAny *aPtr) __NO_THROW
		{
		Backend()->Free( aPtr );
		}
	};

NONSHARABLE_STRUCT(__EPOC32_DIR) : public __EPOC32_WDIR
	{
public:
	__EPOC32_DIR(){}
	~__EPOC32_DIR(){}

	virtual TInt UpdateNarrow();
	struct dirent iCurrentNarrow;
	TBuf8<KMaxFileName> iCurrentNarrowName;
	};

NONSHARABLE_STRUCT (_iconv_t)
	{
	TUint toCode;
	TUint fromCode;
	};

class TUSockAddr : public TSockAddr
/*
Utility class for converting struct sockaddr to and from EPOC32 TSockAddr
@internalComponent
*/
	{
public:
	TUSockAddr() : TSockAddr(), iError(0) {}

	TUSockAddr(TAny* addr);			// constructor form of Prepare
	IMPORT_C TUSockAddr(const TAny* addr, TUint len);	// constructor form of Set
private:
	void Prepare(TAny* addr);
	void Set(const TAny* addr, TUint len);
public:
	IMPORT_C void Get(TAny* addr, unsigned long* len);
public:
	TInt iError; // To store error status of TUSockAddr functions, refers errno.h. Notebly not Symbian error codes.
	};





#endif //SYSIF_H

