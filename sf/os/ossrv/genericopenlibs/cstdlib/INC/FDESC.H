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
#include <e32cons.h>
#include <f32file.h>
#include <es_sock.h>
#include <c32comm.h>


#include <stddef.h>
#include <redircli.h>
#include <redircliinternal.h>

// Queuing support for asynchronous requests
//
class CPosixRequest;
class TPosixRequestQueue : public TSglQue<CPosixRequest>
/**
@internalComponent
*/
	{
public:
	inline TPosixRequestQueue();
	};

//  The base class for all flavours of FileDescriptor
//
class CSerialTimer;
class CPosixRequest;
class CSocketDesc;
NONSHARABLE_CLASS(CFileDescBase) : public CBase
/**
@internalComponent
*/
	{
public:

	static CFileDescBase* Open(RSessionBase& aSession, const wchar_t* name, int mode, int perms, TInt& err);
	static CFileDescBase* Socket(RSocketServ& aSs, int family, int style, int protocol, TInt& err);

	virtual void Read(TDes8& aDesc, TRequestStatus& aStatus);
	virtual TInt ReadCompletion(TDes8& aDesc, TInt aStatus);
	virtual void ReadCancel();

	virtual void Write(TDes8& aDesc, TRequestStatus& aStatus);
	virtual TInt WriteCompletion(TDes8& aDesc, TInt aStatus);
	virtual void WriteCancel();

	virtual void Ioctl(int aCmd, void* aParam, TRequestStatus& aStatus);
	virtual TInt IoctlCompletion(int aCmd, void* aParam, TInt aStatus);
	virtual void IoctlCancel();

	virtual void RecvFrom(TDes8& aDesc, TSockAddr& from, int flags, TRequestStatus& aStatus);
	virtual TInt RecvFromCompletion(TInt& aLength, TInt aStatus);
	virtual void RecvFromCancel();

	virtual void SendTo(TDes8& aDesc, TSockAddr& to, int flags, TRequestStatus& aStatus);
	virtual TInt SendToCompletion(TDes8& aDesc, TInt aStatus);
	virtual void SendToCancel();

	virtual void Sync(TRequestStatus& aStatus);
	virtual void SyncCancel();

	virtual void Accept(CSocketDesc*& aNewSocket, TRequestStatus& aStatus, RSocketServ& aSs);
	virtual void AcceptCancel();

	virtual void Connect(TSockAddr& anAddr,TRequestStatus& aStatus);
	virtual void ConnectCancel();

	virtual void Shutdown(TUint aHow,TRequestStatus& aStatus);
	virtual void ShutdownCancel();

	virtual TBool TimedRead() {return EFalse;}	//default implementation
	TInt TimeoutValue() const {return iReadTimeout;}

	virtual TInt LSeek(int& offset, int whence);
	virtual TInt FStat(struct stat* st);
	virtual TInt Bind(TSockAddr& anAddr);
	virtual TInt Listen(TUint qSize);
	virtual TInt SockName(int anEnd, TSockAddr& anAddr);
	virtual TInt GetSockOpt(TUint anOptionName,TUint anOptionLevel,TDes8& anOption);
	virtual TInt SetSockOpt(TUint anOptionName,TUint anOptionLevel,TDesC8& anOption);

	static void Cleanup(TAny *aPtr);
	void PushLC();

	TInt Close();
	virtual void UserClose() {return;}
	inline CFileDescBase* Dup();
	
	enum IOQueues 
		{ 
		IOreadQ=0, IOwriteQ=1, IOioctlQ=2,IOioctlNQ=3,
		IOmaxQ=IOioctlNQ 
		};

	void AddLast(CPosixRequest& aRequest, IOQueues aQueue);
	void Remove(CPosixRequest& aRequest, IOQueues aQueue);

	TBool ReadIsTimed;
	TBool ReadWasCancelled;
	CSerialTimer * TimedMessage;

protected:
	CFileDescBase();
	virtual TInt FinalClose();
	static void Complete(TRequestStatus& aStatus, TInt aResult);
	TInt iReadTimeout;

private:
	TInt iDupCount;
	TFixedArray<TPosixRequestQueue,IOmaxQ+1> iQueues;
	};

inline CFileDescBase* CFileDescBase::Dup () 
	{ iDupCount += 1; return this; }


NONSHARABLE_CLASS(CTtyDesc) : public CFileDescBase
/**
Abstraction of a teletype device, which will be used for
the console.
@internalComponent
*/
	{
public:
	inline CTtyDesc(CConsoleBase *c);
	inline CTtyDesc();
	~CTtyDesc();

	void Read(TDes8& aDesc, TRequestStatus &aStatus);
	void ReadCancel();
	TInt ReadCompletion(TDes8& aDesc, TInt aStatus);
	void Write(TDes8& aDesc, TRequestStatus& aStatus);
	void Ioctl(int aCmd, void* aParam, TRequestStatus& aStatus);
	TInt IoctlCompletion(int aCmd, void* aParam, TInt aStatus);

protected:
	virtual TInt FinalClose();
private:
	void MapCodeAndEcho(TDes8& aDesc, TKeyCode aCode);
	void CheckConsoleCreated();
	void Write(TDes8& aDesc);
	CConsoleBase *iConsole;
	};

inline CTtyDesc::CTtyDesc(CConsoleBase *c) : CFileDescBase(), iConsole(c) {}
inline CTtyDesc::CTtyDesc() : CFileDescBase() {}

NONSHARABLE_CLASS(CRedirDesc) : public CFileDescBase
/**
client-side connection to the redirection server
@internalComponent
*/
	{
public:
	CRedirDesc();
	virtual TInt Connect();
	virtual void Read(TDes8& aDesc, TRequestStatus& aStatus);
	virtual void Write(TDes8& aDesc, TRequestStatus& aStatus);
	virtual void Sync(TRequestStatus& aStatus);
	TInt FinalClose();
private:
	RRedirSession2 iRedirSession;
	TBool iAttemptedConnection;
	};

NONSHARABLE_CLASS(CFileDesc) : public CFileDescBase
/**
Abstractions for a plain file and a temporary file
@internalComponent
*/
	{
public:
	CFileDesc();
	~CFileDesc();

	TInt Open(RFs& aSession, const TDesC& aName, int mode, int perms);
	TInt LSeek(int& offset, int whence);
	void Read(TDes8& aDesc, TRequestStatus& aStatus);
	void Write(TDes8& aDesc, TRequestStatus& aStatus);
	TInt FStat(struct stat *st);
	void Sync(TRequestStatus &aStatus);
	TInt IoctlCompletion(int aCmd, void* aParam, TInt aStatus);

	static void MapStat(struct stat& st, const TTime& aModTime, TUint& aAttr);

protected:
	virtual TInt FinalClose();
private:
	TInt FileRead(TUint8* aPtr,TInt aLength);
	TInt FileWrite(TUint8* aPtr,TInt aLength);
	TInt Flush();
	TInt DoSync();
	TInt DoRead(TDes8& aDesc);
	TInt DoWrite(TDes8& aDesc);
	TInt Pos();
	TInt Ext();
	TInt Alloc();
private:
	enum {EAlloc,EReading,EWriting};
	enum {EBufferSize = 0x600,EReadAhead = 0x200};
protected:
	RFile	iFile;
	TInt16	iDrive;	// for use with stat()
private:
	TUint8 iState;
	TUint8* iBuffer;
	TUint8* iPtr;
	TUint8* iEnd;
	TInt iSize;
	TInt iPos;
	TInt iExt;
	};

NONSHARABLE_CLASS(CTempFileDesc) : public CFileDesc
/**
@internalComponent
*/
	{
public:
	TInt Open(RFs& aSession, const TDesC& aPath);
protected:
	virtual TInt FinalClose();
private:
	RFs iSession;
	TFileName iName;
	};

NONSHARABLE_CLASS(CSocketDesc) : public CFileDescBase
/**
Abstraction for a socket
@internalComponent
*/
	{
public:
	CSocketDesc() : CFileDescBase(), iIoctlBuf(0,0) {}

	TInt Socket(RSocketServ& aSs, int family, int style, int protocol);

	TInt LSeek(int& offset, int whence);
	void Read(TDes8& aDesc, TRequestStatus& aStatus);
	void ReadCancel();
	void Write(TDes8& aDesc, TRequestStatus& aStatus);
	void WriteCancel();
	TInt FStat(struct stat* st);
	void Sync(TRequestStatus& aStatus);
	void Ioctl(int aCmd, void* aParam, TRequestStatus& aStatus);
	TInt IoctlCompletion(int aCmd, void* aParam, TInt aStatus);
	void IoctlCancel();

	void RecvFrom(TDes8& aDesc, TSockAddr& from, int flags, TRequestStatus& aStatus);
	TInt RecvFromCompletion(TInt& aLength, TInt aStatus);
	void RecvFromCancel();
	void SendTo(TDes8& aDesc, TSockAddr& to, int flags, TRequestStatus& aStatus);
	void SendToCancel();

	void Shutdown(TUint aHow,TRequestStatus& aStatus);
	void Accept(CSocketDesc*& aNewSocket, TRequestStatus& aStatus, RSocketServ& aSs);
	void AcceptCancel();
	void Connect(TSockAddr& anAddr,TRequestStatus& aStatus);
	void ConnectCancel();

	TInt Bind(TSockAddr& anAddr);
	TInt Listen(TUint qSize);
	TInt SockName(int anEnd, TSockAddr& anAddr);
	TInt GetSockOpt(TUint anOptionName,TUint anOptionLevel,TDes8& anOption);
	TInt SetSockOpt(TUint anOptionName,TUint anOptionLevel,TDesC8& anOption);

protected:
	TInt FinalClose();
private:
	inline TInt isStream() const;

	RSocket iSocket;
	TInt iStyle;
	TSockXfrLength iLength;
	TPtr8 iIoctlBuf;
	};

class TUSockAddr : public TSockAddr
/**
Utility class for converting struct sockaddr to and from EPOC32 TSockAddr
@internalComponent
*/
	{
public:
	TUSockAddr() : TSockAddr() {}

	TUSockAddr(TAny* addr);			// constructor form of Prepare
	TUSockAddr(TAny* addr, TUint len);	// constructor form of Set

	void Prepare(TAny* addr);
	void Set(TAny* addr, TUint len);
	void Get(TAny* addr, unsigned long* len);
	};

NONSHARABLE_CLASS(CSerialTimer) : public CTimer
/**
the serial timer class
@internalComponent
*/
  	{
public:
	CSerialTimer();
	~CSerialTimer();

public:
	static CSerialTimer* NewLC(CFileDescBase*);
	static CSerialTimer* NewL(CFileDescBase*);

public:
	void ConstructL(CFileDescBase*);
	void IssueRequest(); 
	void DoCancel();
	void RunL();

private:
	CFileDescBase* iFile;
	};


class CNotifier;

NONSHARABLE_CLASS(CSerialDesc) : public CFileDescBase
/**
@internalComponent
*/
	{

	friend class CNotifier;

	public:
	CSerialDesc() : CFileDescBase(), iReadThreshold(-1), iRequestedSignals(0),
					iNotifyStatus(NULL), iNotifyParamPtr(NULL) {}

	TInt Open(RCommServ& aSession, const wchar_t* name, int mode, int perms);
	TInt FinalClose();

	void Read(TDes8& aDesc, TRequestStatus& aStatus);
	void ReadCancel();
	
	TInt& TimeoutValue() const;
	void Write(TDes8& aDesc, TRequestStatus& aStatus);
	
	void Ioctl(int aCmd, void* aParam, TRequestStatus& aStatus);
	TInt IoctlCompletion(int aCmd, void* aParam, TInt aStatus);
	void IoctlCancel();

	TBool TimedRead();
	void UserClose();

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


#endif // !_FDESC_H
