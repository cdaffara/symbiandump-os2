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
// Abstraction representing a UNIX file descriptor
// CFileDescBase maintains the simple semantics of
// Dup() and Close(), so the final Close() will
// delete the object and hence call the destructor.
// 
//

#ifndef _FDESC_H
#define _FDESC_H

#include <e32std.h>
#include <e32atomics.h>
#include <e32cons.h>
#include <f32file.h>
#include <es_sock.h>
#include <c32comm.h>
#include <stddef.h>
#include <cdblen.h>
#include <commdb.h>
#include <commdbconnpref.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <rpipe.h>
#include <in_sock.h>

#include "base.h"
#include "StdioClient.h"

//FD attributes used internally by the backend
const TUint KInvalidFd 		= 0x00000001;
const TUint KMMapedFd 		= 0x00000002;
const TUint KCloseonExec    = 0x00000004;
const TUint KPipeFd         = 0x00000008;
const TUint KIoctlOutstanding = 0x00000010;
const TUint KConsoleFd         = 0x00000020;
const TUint KFifoFd			= 0x00000030;
const TUint KSpawnCloseInChild = 0x00000040;

#define SET_CLOSE_ON_EXEC_FLG   (1 << 2) 

#if defined(SYMBIAN_OE_LARGE_FILE_SUPPORT) && !defined(SYMBIAN_OE_NO_LFS)

//Large File Support - Use RFile64 and 64 bit file sizes
#define RFILE 	RFile64
#define FSIZE	TInt64

#else

//No large file support - Use RFile and 32 bit file sizes
#define RFILE 	RFile
#define FSIZE	TInt

#endif //SYMBIAN_OE_LARGE_FILE_SUPPORT && !SYMBIAN_OE_NO_LFS

// Atomic socket operations support

#define ATOMICSOCKETOP(realcall,on_error) \
    { \
    if (__e32_atomic_tau_ord32((void *)&iCount, 0x8000, 0, 1) >= 0x8000) \
        { \
        on_error; \
        } \
    else \
        { \
        realcall; \
        __e32_atomic_tas_ord32((void *)&iCount, 0, -1, 0); \
        } \
    }

#define NOP 

//Enumarations for Polling
enum TPollMode
    {
    EReadyForReading = 1,
    EReadyForWriting = 2,
    EAnyException = 4
    };
  
//Enumarations for Redirection Server
enum TRedirConnState
	{
	ENotConnected,
	EConnected,
	ENoServer
	};

//Enumerations for the console echo
enum TConsoleEcho
	{
	EOff = 0, //Turn Off the echo
	EOn = 1,  //Turn On the echo
	EPrintValid = 33, //Printable ascii character set <33-126>
	EPrintInvalid=127 
	};
	
//  The base class for all flavours of FileDescriptor
//
class CSocketDesc;

NONSHARABLE_CLASS(CFileDescBase) : public CBase
/*
@internalComponent
*/
	{
public:

	static CFileDescBase* Open(const wchar_t* name, int mode, int perms, TInt& err);
	static CFileDescBase* Socket(RSocketServ& aSs, int family, int style, int protocol, TInt& err);

	virtual void Read(TDes8& aDesc, TRequestStatus& aStatus);
	virtual TInt ReadCompletion(TDes8& aDesc, TInt aStatus);
	virtual void ReadCancel();

	virtual void Write(TDes8& aDesc, TRequestStatus& aStatus);
	virtual TInt WriteCompletion(TDes8& aDesc, TInt aStatus);
	virtual void WriteCancel();
	virtual TInt SetAtt(TUint aSetAttMask, TUint aClearAttMask);

	virtual void Ioctl(int aCmd, void* aParam, TRequestStatus& aStatus);
	virtual TInt IoctlCompletion(int aCmd, void* aParam, TInt aStatus);
	virtual void IoctlCancel();

	virtual void RecvFrom(TDes8& aDesc, TSockAddr& from, int flags, TRequestStatus& aStatus);
	virtual void RecvFromCancel();

	virtual void SendTo(TDes8& aDesc, const struct sockaddr* anAddr, unsigned long aAddrLen,int flags, TRequestStatus& aStatus);

	virtual void SendToCancel();

	virtual TInt CompletionStatus(TInt& aLength, TInt aStatus);

	virtual void Sync(TRequestStatus& aStatus);
	virtual void SyncCancel();

	virtual void Accept(CFileDescBase*& aNewSocket, TRequestStatus& aStatus, RSocketServ& aSs,TSockAddr *aAddr);
	virtual void AcceptCancel();

	virtual void Connect(const struct sockaddr* aAddr,unsigned long size,TRequestStatus& aStatus);
	virtual void ConnectCancel();
	virtual TBool GetConnectionProgress();
	virtual void SetConnectionProgress( TBool aInProgress );

	virtual void Shutdown(TUint aHow,TRequestStatus& aStatus);
	virtual void ShutdownCancel();
	
	virtual TBool TimedRead() {return EFalse;}	//default implementation
	TInt TimeoutValue() const {return iReadTimeout;}

	virtual TInt LSeek(off_t& offset, int whence);
	virtual TInt FStat(struct stat* st);
	virtual TInt Bind(const struct sockaddr* addr, unsigned long size);
	virtual TInt Listen(TUint qSize);
	virtual TInt SockName(int anEnd, struct sockaddr* anAddr,unsigned long* aSize);
	virtual TInt GetSockOpt(TInt anOptionName,TInt anOptionLevel,TDes8& anOption);
	virtual TInt SetSockOpt(TInt anOptionName,TInt anOptionLevel,TDesC8& anOption);
	virtual TInt Fcntl(TUint anArg, TUint aCmd);
	
	virtual TInt Poll(TPollMode aMode,TBool& aReadyStatus,TInt& aErrno);
	virtual TInt Poll(TUint aEvents);
	virtual TInt NotifyActivity(TUint aEvents, TRequestStatus& aRequest, TTimeIntervalMicroSeconds32 timeout);
	virtual void TweakWatchedEvents(TUint& events);
	virtual TInt TweakReadyEvents(TInt errval);
	virtual void CancelNotify();

	//Truncate the File
	virtual TInt Truncate(off_t /*anOffset*/)
		{
		return KErrNotSupported;
		}
	//Type of the Descriptor
	virtual TInt Type()  
		{
		return EBaseDesc;
		}
		
	//Set/reset the Attributes of the FD
	 inline void SetAttributes( TUint32 aBit, TBool aFlag )
		{
		if( aFlag )
			{
			//Set the bit to 1
			iFdAttrib |= aBit;
			}
		else
			{
			//Reset the bit to 0
			iFdAttrib &= ~aBit;
			}
		}

	//Get the Attributes of the FD
	inline const TUint32 Attributes() const
		{
		return iFdAttrib;
		}

	inline const TUint32 FcntlFlag() const
		{
		return iFcntlFlag;
		}

	static inline void Cleanup(TAny *aPtr)
		{
		((CFileDescBase*)aPtr)->Close();
		}
		
	inline void PushLC() 
		{
		CleanupStack::PushL(TCleanupItem(Cleanup,this));
		}

	IMPORT_C TInt Close();
	virtual void UserClose() {return;}
	inline CFileDescBase* Dup();
	inline void SetFid(TInt aFid) { iFid = aFid; }
	virtual void SetFids(void *aFids);
	
	inline void operator delete(TAny *aPtr) __NO_THROW
		{
		Backend()->Free(aPtr);
		}
		
	inline void operator delete(TAny *aPtr, TLeave) __NO_THROW
		{
		Backend()->Free(aPtr);
		}
		
	inline void operator delete(TAny *aPtr, TAny*) __NO_THROW
		{
		Backend()->Free(aPtr);
		}
		
	TBool ReadIsTimed;
	TBool ReadWasCancelled;

protected:
	CFileDescBase( TUint aFcntl = 0, TUint32 aFdAttrib = 0 );
	virtual TInt FinalClose();
	static void Complete(TRequestStatus& aStatus, TInt aResult);
	TInt iReadTimeout;
	//Used in case of CRedirDesc
	TInt iFid;
	//For FD's fcntl
	TUint iFcntlFlag;
	//For FD's other attributes
	TUint32 iFdAttrib;
	
private:
	//Check if the path corresponds to a directory
	static TBool CheckIfDirectory(const TDesC& aPath, RFs& aFs);
	
public:
    enum TDescType
        {
        EBaseDesc,
        EFileDesc,
        EFileTempDesc,
        ESocketDesc,
        ESerialDesc,
        EDirDesc,
        ETtyDesc,
        EPopenPipeDesc,
        EPipeDesc,
        ERedirDesc,
        EStdErrDesc,
        EFileSocketDesc
        };
private:
    TInt iDupCount;
public:
	// Poll is required to return any of the requested events. 
	// In case of any error, "iPollErr" should be checked by the socket-descriptor
	// to take any corrective action.
	TInt iPollErr;
	};


inline CFileDescBase* CFileDescBase::Dup () 
	{ iDupCount += 1; return this; }


NONSHARABLE_CLASS(CTtyDesc) : public CFileDescBase
/*
Abstraction of a teletype device, which will be used for
the console.
@internalComponent
*/
	{
public:
	inline CTtyDesc(CConsoleBase *c);
	inline CTtyDesc();
	~CTtyDesc();
     virtual TInt Type()
	    {
	    return ETtyDesc;
	    }
	void Read(TDes8& aDesc, TRequestStatus &aStatus);
	void ReadCancel();
	TInt ReadCompletion(TDes8& aDesc, TInt aStatus);
	void Write(TDes8& aDesc, TRequestStatus& aStatus);
	void Ioctl(int aCmd, void* aParam, TRequestStatus& aStatus);
	TInt IoctlCompletion(int aCmd, void* aParam, TInt aStatus);
	TInt SetEcho(TUint8 aEcho);
protected:
	virtual TInt FinalClose();
private:
	void MapCodeAndEcho(TDes8& aDesc, TKeyCode aCode);
	void CheckConsoleCreated();
	void Write(TDes8& aDesc);
	CConsoleBase *iConsole;
	//To store last non-modifiable data position and flag for reading data
	TInt  iCurPosX;
	TInt  iCurPosY;
	TBool iReadingData;
	TUint8 iEchoVal;
	};

inline CTtyDesc::CTtyDesc(CConsoleBase *c) : CFileDescBase(), iConsole(c), iEchoVal(EOn){}
inline CTtyDesc::CTtyDesc() : CFileDescBase(), iEchoVal(EOn) {}


NONSHARABLE_CLASS(CRedirDesc) : public CFileDescBase
/*
client-side CRedirDesc desc class which will be used to connecting to the Redirection Server
@internalComponent
*/
	{
public:
	CRedirDesc();
	virtual TInt Type()
	    {
	    return ERedirDesc;
	    }
	virtual void Read(TDes8& aDesc, TRequestStatus& aStatus);
	virtual void Write(TDes8& aDesc, TRequestStatus& aStatus);
	TInt WriteCompletion(TDes8& aDesc, TInt /*aStatus*/);
	TInt ReadCompletion(TDes8& aDesc, TInt /*aStatus*/);
	virtual TInt Fcntl(TUint anArg, TUint aCmd);
	TInt FinalClose();
	TInt Configure();
	TInt Poll(TUint aEvents);
	TInt NotifyActivity(TUint aEvents, TRequestStatus& aRequest, TTimeIntervalMicroSeconds32 timeout);
	TInt TweakReadyEvents(TInt errval);
	void CancelNotify();
	TInt SetEcho(TUint8 aEcho);
	TBool iReadNone;
	TBool iWriteNone;
private:
	//Handle to StdioSession
	RStdioSession iSession;
	TRedirConnState iStatus;
	RFastLock iLock;
	};


NONSHARABLE_CLASS(CStdErrDesc) : public CFileDescBase

/*
client-side Standard Error desc class which will be used to put all stderr to RDebug::Printf
@internalComponent
*/
	{
public:
	CStdErrDesc();
	virtual void Write(TDes8& aDesc, TRequestStatus& aStatus);
	TInt WriteCompletion(TDes8& aDesc, TInt /*aStatus*/);
	virtual TInt Fcntl(TUint anArg, TUint aCmd);
	virtual TInt Type()
	    {
	    return EStdErrDesc;
	    }
	};

NONSHARABLE_CLASS(CFileDesc) : public CFileDescBase
/*
Abstractions for a plain file and a temporary file
@internalComponent
*/
	{
public:
	CFileDesc();
	~CFileDesc();

	void SetState(const TDes& params);

	TInt Open(RFs& aSession, const TDesC& aName, int mode, int perms);
	TInt LSeek(off_t& offset, int whence);
	void Read(TDes8& aDesc, TRequestStatus& aStatus);
	void Write(TDes8& aDesc, TRequestStatus& aStatus);
	TInt FStat(struct stat *st);
	void Sync(TRequestStatus &aStatus);
	TInt IoctlCompletion(int aCmd, void* aParam, TInt aStatus);
	TInt Fcntl(TUint anArg, TUint aCmd);
	static void MapStat(struct stat& st, const TTime& aModTime, TUint& aAttr, const mode_t aMode = S_IFREG);
	TInt Truncate(off_t anOffset);
	TInt SetAtt(TUint aSetAttMask, TUint aClearAttMask);
	TInt ProcessLockParams(FSIZE& pos, FSIZE &lock_len, TInt& lock_type, struct flock* anArg);
	inline int CreateLock() 
		{
		return iLock.CreateLocal();
		}

	inline RFILE& FileHandle()
		{
		return iFile;
		}
	
	inline const FSIZE Offset() const
		{
		return iPos;
		}
	
    inline const FSIZE Size() const
		{
			return iSize;
		}
	inline void setSize(TUint32 param)
		{
		iSize = param;
		}
	inline const FSIZE Extent() const
		{
		return iExt;
		}
     virtual TInt Type()
	    {
	    return EFileDesc;
	    }
protected:
	virtual TInt FinalClose();
	TInt Alloc();
private:
	TInt FileRead(TUint8* aPtr,TInt aLength);
	TInt FileWrite(TUint8* aPtr,TInt aLength);
	TInt Flush();
	TInt DoSync();
	TInt DoRead(TDes8& aDesc);
	TInt DoWrite(TDes8& aDesc);
	FSIZE Pos();
	FSIZE Ext();
	
private:
	enum {EAlloc,EReading,EWriting};
	enum {EBufferSize = 0x600,EReadAhead = 0x200};
protected:
	RFILE	iFile;
	TInt16	iDrive;	// for use with stat()
private:
	TUint8 iState;
	TUint8* iBuffer;
	TUint8* iPtr;
	TUint8* iEnd;
	
	FSIZE iSize;
	FSIZE iPos;
	FSIZE iExt;
	
	//For locking the descriptor before any operation.
	//To make it thread safe.
	RFastLock iLock;
	friend class RFileDesTransferSession;
	friend class CFileDesTransferSession;
	};

NONSHARABLE_CLASS(CTempFileDesc) : public CFileDesc
/*
@internalComponent
*/
	{
public:
	TInt Open(const wchar_t* aName, TInt mode);
	virtual TInt Type()
	    {
	    return EFileTempDesc;
	    }
protected:
	virtual TInt FinalClose();
private:
	RFs iSession;
	TFileName iName;
	};

/*
Implements common functionality between CSocketDesc and CFileSocketDesc.
Not meant to be instantaible.
*/	
NONSHARABLE_CLASS(CSockDescBase) : public CFileDescBase
	{
public:
	 virtual TInt Socket(RSocketServ& aSs, int family, int style, int protocol);
	 TInt Fcntl(TUint anArg, TUint aCmd);
	 TInt FStat(struct stat *st);
	 TInt FinalClose();
	 void Read(TDes8& aDesc, TRequestStatus& aStatus);
	 void Write(TDes8& aDesc, TRequestStatus& aStatus);
	 void RecvFrom(TDes8& aDesc, TSockAddr& from, int flags, TRequestStatus& aStatus);
	 void SendTo(TDes8& aDesc, TSockAddr& to, int flags, TRequestStatus& aStatus);
	 TInt Poll(TUint aEvents);
	 TInt NotifyActivity(TUint aEvents, TRequestStatus& aRequest, TTimeIntervalMicroSeconds32 aTimeout);
	 void TweakWatchedEvents(TUint& events);
	 TInt TweakReadyEvents(TInt errval);
	 void CancelNotify();
	 TInt Listen(TUint qSize);
	 void ReadCancel();
	 TInt ReadCompletion(TDes8& aBuf, TInt aStatus);
	 void RecvFromCancel();
	 void SendToCancel();
	 void WriteCancel();
	 TInt SockName(int anEnd, TSockAddr& anAddr);
	 void Shutdown(TUint aHow,TRequestStatus& aStatus);
	 TInt CompletionStatus(TInt& aLength, TInt aStatus);
	 const TUint& GetSelectEvents()
		 {
		 iIoctlLock.Signal();
		 return iSelectEvents();
		 }
	 
	 inline TInt CreateLock() 
		 {
	     TInt err = iIoctlLock.CreateLocal(1) || iReadLock.CreateLocal() || iWriteLock.CreateLocal();
	     if (err != KErrNone)
	         {
             // closes on unopened handles are safe
             iIoctlLock.Close();
             iReadLock.Close();
             iWriteLock.Close();
	         }
	     return err;
		 }
		 
	 inline RSemaphore& GetIoctlLock()
		 {
		 return iIoctlLock;
		 }
	 inline TBool GetConnectionProgress()
		 {
		 return iConnectInProgress;
		 }

	 inline void SetConnectionProgress( TBool aInProgress )
		 {
		 iConnectInProgress = aInProgress;
		 }
	 
protected:
		//rearrange these 
	CSockDescBase():iAddrFamily(-1),iProtocol(KUndefinedProtocol),iIoctlFlag(EFalse),iCount(0)
	{
	
	}	
	RSocket iSocket;
	TSockXfrLength iLength;
	TFileName iPath;
	TPckgBuf<TUint> iSelectEvents;
	TInt iAddrFamily;
	TInt iProtocol;
	TInt iStyle;
	// For preventing simultaneous ioctl calls.
	// No native support.
	RSemaphore iIoctlLock;
	// Is this required?
	TBool iIoctlFlag;
	// Safeguard against issue of a second Read/Write request while the first is pending.
	// The iReadLock also guards OpenUsingPreference
	//For locking the descriptor before any operation.
	//To make it thread safe.
	RFastLock iReadLock;
	RFastLock iWriteLock;

	//Flag to mark the connect status of a non-blocking socket as "in progress"
	//to prevent duplicate connection request
	TBool iConnectInProgress;
	volatile TInt iCount;
protected:

	inline TInt isStream() const	// inline, but private
		{ 
		return iStyle==SOCK_STREAM; 
		}
	};

NONSHARABLE_CLASS(CSocketDesc) : public CSockDescBase
/*
Abstraction for a socket
@internalComponent
*/
	{
public:
	CSocketDesc();
	TInt Socket(RSocketServ& aSs, int family, int style, int protocol);
	void Read(TDes8& aDesc, TRequestStatus& aStatus);
	TInt ReadCompletion (TDes8& /*aBuf*/, TInt aStatus);	
	void ReadCancel();
	void Write(TDes8& aDesc, TRequestStatus& aStatus);
	void WriteCancel();
	void Sync(TRequestStatus& aStatus);
	void Ioctl(int aCmd, void* aParam, TRequestStatus& aStatus);
	TInt IoctlCompletion(int aCmd, void* aParam, TInt aStatus);
	void IoctlCancel();

	void RecvFrom(TDes8& aDesc, TSockAddr& from, int flags, TRequestStatus& aStatus);
	void RecvFromCancel();
	void SendTo(TDes8& aDesc, const struct sockaddr* anAddr, unsigned long aAddrLen, int flags, TRequestStatus& aStatus);

	void SendToCancel();

	void Shutdown(TUint aHow,TRequestStatus& aStatus);
	void Accept(CFileDescBase*& aNewSocket, TRequestStatus& aStatus, RSocketServ& aSs,TSockAddr *aAddr);
	void AcceptCancel();
	void Connect(const struct sockaddr* aAddr,unsigned long size,TRequestStatus& aStatus);
	void ConnectCancel();

	TInt Bind(const struct sockaddr* addr, unsigned long size);
	TInt Listen(TUint qSize);
	TInt SockName(int anEnd, struct sockaddr* anAddr,unsigned long* aSize);
	TInt GetSockOpt(TInt anOptionName,TInt anOptionLevel,TDes8& anOption);
	TInt SetSockOpt(TInt anOptionName,TInt anOptionLevel,TDesC8& anOption);
	TInt Fcntl(TUint anArg, TUint aCmd);
	
	TInt Poll(TPollMode aMode,TBool& aReadyStatus,TInt& aErrno);
	TInt Poll(TUint aEvents);
	void CancelNotify();
	void TempClose();
	
	void SetFids(void *aFids);
	virtual TInt Type()
	    {
	    return ESocketDesc;
	    }
	
protected:
	TInt FinalClose();
private:
	
    TInt GetInterfaceIndex(TUint32 aAddr);
	TInt GetInterfaceList(void *aParam);
	TInt GetActiveInterface( void *aParam);
	TInt GetInterfaceIndexByName(void *aParam);
	TInt GetActiveInterfaceList(void *aParam);
	TInt SetInterfaceByName(void *aParam);
	TInt StartInterface(void *aParam);
	TInt StartActiveInterface(void *aParam);
	TInt StopInterface(void *aParam);
	TInt OpenUsingPreference();
	void AccessPointListL(CArrayFixFlat<TAccessPointRecord> *&aRecordPtr, TInt &aLength);
	void AccessPointCountL(TInt &aCount);
	void ReadRecordFromIapTableL(CCommsDbTableView* aView, TAccessPointRecord &aRecord);
	void OpenIapTableLC(CCommsDatabase **aIapDatabase, CCommsDbTableView **aView);
	TInt ActiveConnectionCount(TInt &aCount);
	TInt ActiveConnectionListL(CArrayFixFlat<TAccessPointRecord> *aRecordPtr, TInt &aLength);
	TInt GetRConnectionDetails(RConnection *aRc, TAccessPointRecord &aApr);
	TInt StartConnection(void *aParam);
	TInt GetConnectionPreference(TCommDbConnPref &aApr);
	void GetIapDetailsByNameL(TBuf<KCommsDbSvrMaxColumnNameLength> aIapName, TAccessPointRecord &aRecord);
	void StopSubConnection();
	void StopConnection();
	TInt StartSubConnection(void *aParam);
	TInt GetInterface(void *Param, TInt aType);
	TInt GetInterafceNumber(void *aParam);
	TInt GetIpAddress( void *aParam );
	
	TInt RouteRequest(TInt aReq, void *aParam);
	TInt GetInterfaceHWAddress(void *aParam);
	
	TInt ConvertRtEntry(TSoInetRouteInfo& aRouteInfo, struct rtentry *aRouteEntry);
	TInt ConvertSockAddr(TInetAddr& aInetAddr, struct sockaddr_in *aSockAddr);	
		
	TInt GetInterfaceByName(const TDesC& aIfName, TPckgBuf<TSoInetInterfaceInfo>& aIface);
	TInt Copy(TInetAddr& aDest, TInetAddr& aSrc);
	
	TInt GetRemoteIpAddress( void *aParam );
	TInt GetInterafceParamInfo( void *aParam,TInt aType);
	void FindConnectionDetailsL(CArrayFixFlat<TAccessPointRecord>*& aRecordPtr, TInt& aCount);    
	void FindConnectionInfoL(TAccessPointRecord &aRecord,char *ptr);
	TInt GetInterfaceDetails( void *aParam ,TInt aFlag, TInt aType );
	TInt SetInterafceParamInfo( void *aParam,TInt aType);
	TInt SetInterfaceDetails( void *aParam ,TInt aFlag, TInt aType );
	
	TInt maybe_reopen_socket()
	    {
	    TInt err = KErrNone;
	    if (!__e32_atomic_load_acq32(&iSocketPtr))
	        {
            iReadLock.Wait();
            if (!iSocketPtr)
                {
                err = OpenUsingPreference();
                }
            iReadLock.Signal();
	        }
	    return err;
	    }
	
	enum InterfaceType
		{
		EACCESS_POINT,
		EACTIVE_CONNECTION,
		EACTIVE_GETIP,
		EACCESS_GETMETRIC,
		EACCESS_GETMTU,
		EACCESS_GETNETMASK,
		EACCESS_GETBROADCAST,
		EACCESS_GETPHYSADDR,
		EACCESS_GETFLAGS,
		EACCESS_SETMETRIC,
		EACCESS_SETMTU,
		EACCESS_SETFLAGS,
		EACCESS_SETPHYSADDR,
		EACTIVE_SETIP,
		EACCESS_SETNETMASK,
		EACCESS_SETBROADCAST
		};

	TPtr8 iIoctlBuf;
	RSocket *iSocketPtr;
	TAccessPointRecord iConnPref;
	RConnection iConnection;
	RConnection *iConnectionPtr;
	RSubConnection iSubConnection;
	RSubConnection *iSubConnectionPtr;
	TInt iRConnectionIndex;
	RSocketServ *iSockServPtr;
	void *iFids;

	};


class CNotifier;

NONSHARABLE_CLASS(CSerialDesc) : public CFileDescBase
/*
@internalComponent
*/
	{

	friend class CNotifier;

	public:
	CSerialDesc() : CFileDescBase(), iReadThreshold(-1), iRequestedSignals(0),
					iNotifyStatus(NULL), iNotifyParamPtr(NULL) {}

	TInt Open(const wchar_t* name, int mode, int perms);
	TInt FinalClose();

	void Read(TDes8& aDesc, TRequestStatus& aStatus);
	void ReadCancel();
	
	TInt& TimeoutValue() const;
	void Write(TDes8& aDesc, TRequestStatus& aStatus);
	TInt WriteCompletion(TDes8& aDesc, TInt aStatus);

	
	void Ioctl(int aCmd, void* aParam, TRequestStatus& aStatus);
	TInt IoctlCompletion(int aCmd, void* aParam, TInt aStatus);
	void IoctlCancel();

	TBool TimedRead();
	void UserClose();
	virtual TInt Type()
	    {
	    return  ESerialDesc;
	    }

protected:

private:

	void NotifyDataAvailable(TRequestStatus& aStatus);
	void NotifyOutputEmpty(TRequestStatus& aStatus);
	void NotifyBreak(TRequestStatus& aStatus);
	void NotifyWriteErrors(TRequestStatus& aStatus, TUint* aRequestParams, TUint aSignalsMask);
	void NotifySignalChange(TRequestStatus& iStatus, TUint& aRequestParam, TUint aSignalsMask);
	void NotifyDataAvailableCancel();
	void NotifyOutputEmptyCancel();
	void NotifyBreakCancel();
	void NotifyWriteErrorsCancel();
	void NotifySignalChangeCancel();
	void Notify(TInt aVal);		//complete the notify request
	TInt NotifiesSupported();	//return the notifies supported at the moment
	TBool RequestedNotifiesSupported(TInt aRequested);	//see if the notifies requested are supported
	void CancelNotifiers(const CNotifier* aCompletedNotifier);	//cancel them all apart from the passed one
	TUint Signals();

	TInt ReadCompletion (TDes8& aBuf, TInt aStatus);	

	TInt iReadThreshold;
	RComm iCommPort;

	TInt iRequestedSignals;
	TRequestStatus* iNotifyStatus;
	TUint* iNotifyParamPtr;
	TUint* iRequestDataPtr;

	CNotifier* iDataAvailableNotifier;
	CNotifier* iOutputEmptyNotifier;
	CNotifier* iBreakNotifier;
	CNotifier* iErrorsNotifier;
	CNotifier* iSignalsNotifier;

	};

	
NONSHARABLE_CLASS(CPipeDescBase) : public CFileDescBase
/*
Base class with the RPipe handle and common methods
@internalComponent
*/
	{
public:
	CPipeDescBase(const TUint16 aMode = 0) : CFileDescBase()
		{
		iFdAttrib |= KPipeFd;
		if (aMode & O_NONBLOCK)
			{
			iFcntlFlag |= O_NONBLOCK;
		}
		}
		
	TInt Fcntl(TUint anArg, TUint aCmd);
	TInt FStat(struct stat *st);
	void Ioctl(int aCmd, void* aParam, TRequestStatus& aStatus);
	TInt FinalClose();
	
	RPipe& Handle()
		{
		return iHandle;
		}
		
	TUint& FcntlFlag()
		{
		return iFcntlFlag;
		}
		
	TUint32& FDAttrib()
		{
		return iFdAttrib;
		}
	virtual TInt Type()
	    {
	    return EPipeDesc;
	    }

protected:
	RPipe iHandle;
	};
	
NONSHARABLE_CLASS(CPipeReadDesc) : public CPipeDescBase
/*
Handle to the read-end of an RPipe object.
@internalComponent
*/
	{
public:
	CPipeReadDesc(const TUint16 aMode = 0) : CPipeDescBase(aMode)
		{
		iFcntlFlag |= O_RDONLY;
		}
		
	void Read(TDes8& aDesc, TRequestStatus& aStatus);
	void Write(TDes8& aDesc, TRequestStatus& aStatus);
	TInt Poll(TUint aEvents);
	TInt NotifyActivity(TUint aEvents, TRequestStatus& aRequest, TTimeIntervalMicroSeconds32 /*timeout*/);
	TInt TweakReadyEvents(TInt errval);
	void CancelNotify();
	
protected:
	TInt DoRead(TDes8& aDesc);
	};
	
NONSHARABLE_CLASS(CPipeWriteDesc) : public CPipeDescBase
/*
Handle to the write-end of an RPipe object.
@internalComponent
*/
	{
public:
	CPipeWriteDesc(const TUint16 aMode = 0) : CPipeDescBase(aMode)
		{
		iFcntlFlag |= O_WRONLY;
		}
		
	void Write(TDes8& aDesc, TRequestStatus& aStatus);
	void Read(TDes8& aDesc, TRequestStatus& aStatus);
	TInt Poll(TUint aEvents);
	TInt NotifyActivity(TUint aEvents, TRequestStatus& aRequest, TTimeIntervalMicroSeconds32 timeout);
	TInt TweakReadyEvents(TInt errval);
	void CancelNotify();
	
protected:
	TInt DoWrite(TDes8& aDesc);
	};
NONSHARABLE_CLASS(CFileSocketDesc) : public CSockDescBase
/*
Class representing AF_UNIX socket descriptors
Implemented using 2 FIFOs
@internalComponent
*/
	{
public:
	CFileSocketDesc(const TUint16 aMode = 0) 
		{
		if (aMode & O_NONBLOCK)
			{
			iFcntlFlag |= O_NONBLOCK;
			}
		iFcntlFlag |= O_RDWR;
		iPath.Zero();
		}
		
	void Ioctl(int aCmd, void* aParam, TRequestStatus& aStatus);
	TInt FinalClose();
	void Read(TDes8& aDesc, TRequestStatus& aStatus);
	void Write(TDes8& aDesc, TRequestStatus& aStatus);
	void RecvFrom(TDes8& aDesc, TSockAddr& from, int flags, TRequestStatus& aStatus);
	void SendTo(TDes8& aDesc, const struct sockaddr* anAddr, unsigned long aAddrLen, int flags, TRequestStatus& aStatus);
	TInt Listen(TUint qSize);
	TInt GetSockOpt(TInt anOptionName,TInt anOptionLevel,TDes8& anOption);
	TInt SetSockOpt(TInt anOptionName,TInt anOptionLevel,TDesC8& anOption);
	TInt Bind(const struct sockaddr* addr, unsigned long size);
	void Connect(const struct sockaddr* aAddr,unsigned long size, TRequestStatus& aStatus);
	void Accept(CFileDescBase*& aNewSocket, TRequestStatus& aStatus, RSocketServ& aSs, TSockAddr *);
	TInt Socket(RSocketServ& aSs, int family, int style, int protocol);
	TInt SockName(int anEnd, struct sockaddr* anAddr,unsigned long* aSize);
	void AcceptCancel();
	void ConnectCancel();

	TUint& FcntlFlag()
		{
		return iFcntlFlag;
		}
		
	TUint32& FDAttrib()
		{
		return iFdAttrib;
		}
	virtual TInt Type()
	    {
	    return EFileSocketDesc;
	    }

	TInt GetLocalSockAddrByPort(struct sockaddr_un* aAddr,unsigned long* aAddrLen,TUint aPortNum);
	TInt GetLocalSockPortByPath(const struct sockaddr_un* aAddr,unsigned long aAddrLen,TUint& aPortNum);

private:
	TInt ValidateAddress(const struct sockaddr_un* aAddr,unsigned long* aAddrLen);
	TInt RemoveLocalSockAddr();
private:
	TBuf8<KMaxFileName> iPath;
	RFastLock iReadLock;
	RFastLock iWriteLock;
	class TAutoFastLock
		{
	public:
		TAutoFastLock(RFastLock &aLock):iAutoLock(aLock)
			{
			iAutoLock.Wait() ;
			}
		~TAutoFastLock()
			{
			iAutoLock.Signal() ;
			}
	private:
		RFastLock &iAutoLock;
		};
	};
	
NONSHARABLE_CLASS(CDirectoryDesc) : public CFileDescBase
/*
Abstractions for a directory 
@internalComponent
*/
	{
public:
	CDirectoryDesc();
	~CDirectoryDesc();

	TInt Open(RFs& aSession, const TDesC& aName, int mode, int perms);
	void Read(TDes8& aDesc, TRequestStatus& aStatus);
	void Write(TDes8& aDesc, TRequestStatus& aStatus);
	TInt Fcntl(TUint anArg, TUint aCmd);
	TInt LSeek(off_t& offset, int whence);
	TInt FStat(struct stat *st);
	//Get the Name of the directory
	inline const wchar_t* GetDirName()
		{
		return (const wchar_t*)iDirName.Ptr();
		}
	virtual TInt Type()
	    {
	    return EDirDesc;
	    }

private:
	RDir	iDir;
	//Name of the directory
	TFileName iDirName;
	};

#ifdef SYMBIAN_OE_LIBRT
NONSHARABLE_CLASS(CSharedMemDesc) : public CFileDescBase
/*
Abstractions for shared memory 
@internalComponent
*/
	{
public:
	CSharedMemDesc();
	~CSharedMemDesc();

	TInt Open(const wchar_t* aName, int mode, int perms);
	void Read(TDes8& aDesc, TRequestStatus& aStatus);
	void Write(TDes8& aDesc, TRequestStatus& aStatus);
	TInt Fcntl(TUint anArg, TUint aCmd);
	TInt FStat(struct stat *st);
	TInt LSeek(off_t& offset, int whence);
private:
	TInt ShmRead(TUint8* aPtr,TInt aLength);
	TInt ShmWrite(TUint8* aPtr,TInt aLength);
	TInt DoShmRead(TDes8& aDesc);
	TInt DoShmWrite(TDes8& aDesc);
	TInt Pos();
	TInt Ext();	
protected:
	TInt FinalClose();	
private:
	RFastLock iLock;	
	void* iPtr;
	TInt iSize;
	TInt iPos;
	TInt iExt;
	TInt iPerms;
	TInt iKey;
	};
#endif //SYMBIAN_OE_LIBRT
#endif // !_FDESC_H
