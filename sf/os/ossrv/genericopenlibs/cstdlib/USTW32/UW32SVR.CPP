// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#if defined(_UNICODE)
#if !defined(UNICODE)
#define UNICODE
#endif
#endif
#define WIN32_LEAN_AND_MEAN
#pragma warning( disable : 4201 ) // nonstandard extension used : nameless struct/union
#include <windows.h>
#include <winbase.h>
#include <TCHAR.H>
#pragma warning( default : 4201 ) // nonstandard extension used : nameless struct/union

#include "ESTW32SV.H"

#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason)
	{
	return KErrNone;
	}
#endif

// -------------- CLIENT IMPLEMENTATION -----------------------------

EXPORT_C void RWin32Stream::StartServer()
//
// Static function which spawns the three servers
//
	{
	CWin32Stream::StartServer(Kstdin);
	CWin32Stream::StartServer(Kstdout);
	CWin32Stream::StartServer(Kstderr);
	}

EXPORT_C TInt RWin32Stream::Open(TInt aStream)
	{
	TBuf<8> stream;
	if (CWin32Stream::StreamName(aStream,stream)!=KErrNone)
		return KErrArgument;
	TVersion version(1,0,0);
	return CreateSession(stream,version,1);	// only one message allowed - no concurrency
	}

TInt RWin32Stream::CheckEOF(TRequestStatus& aStatus)
	{
	if (Handle()!=0)
		return KErrNone;
	TRequestStatus* aStatusPtr=&aStatus;
	User::RequestComplete(aStatusPtr,KErrEof);
	return KErrEof;
	}
	
EXPORT_C void RWin32Stream::Read(TRequestStatus& aStatus, TDes8& aDes)
	{
	Read(aStatus, aDes, aDes.MaxLength());
	}

EXPORT_C void RWin32Stream::Read(TRequestStatus& aStatus, TDes8& aDes, TInt aLength)
	{
	if (CheckEOF(aStatus))
		return;
	SendReceive(EWin32Read,TIpcArgs(&aDes,aLength),aStatus);
	}

EXPORT_C void RWin32Stream::Write(TRequestStatus& aStatus, const TDesC8& aDes)
	{
	Write(aStatus, aDes, aDes.Length());
	}

EXPORT_C void RWin32Stream::Write(TRequestStatus& aStatus, const TDesC8& aDes, TInt aLength)
//
// Write aLength bytes
//
	{
	if (CheckEOF(aStatus))
		return;
	SendReceive(EWin32Write,TIpcArgs(&aDes,aLength),aStatus);
	}

EXPORT_C void RWin32Stream::Flush(TRequestStatus& aStatus)
//
// Flush output
//
	{
	if (CheckEOF(aStatus))
		return;
	SendReceive(EWin32Flush,aStatus);
	}

// -------------- SERVER IMPLEMENTATION -----------------------------
//
// This is only relevant to WINS, so we know that in truth it's all a single address space
// and we can just operate on the descriptors directly.

//#define ESTW32_LOGGING	// log the stream output into a file

TInt CWin32Stream::StreamName(TInt aStream, TDes& aBuf)
	{
	switch (aStream)
		{
	case Kstdin:
		aBuf=_L("stdin"); break;
	case Kstdout:
		aBuf=_L("stdout"); break;
	case Kstderr:
		aBuf=_L("stderr"); break;
	default:
		return KErrArgument;
		}
	return KErrNone;
	}

struct rendezvous 
	{
	TRequestStatus*	iStatus;
	TInt	iStream;
	RThread iCaller;
	TBuf<8> iStreamName;
	};

void CWin32Stream::StartServer(TInt aStream)
//
// Static function which spawns a server thread
//
	{
	TRequestStatus status(KRequestPending);
	struct rendezvous rv;
	rv.iStatus=&status;
	rv.iStream=aStream;
	TInt err=StreamName(aStream,rv.iStreamName);
	if (err!=KErrNone)
		return;
	err=rv.iCaller.Duplicate(RThread(),EOwnerProcess);
	if (err!=KErrNone)
		return;
	RThread server;
	err=server.Create(rv.iStreamName,ThreadFunction,2048,4096,65536,(TAny*)&rv,EOwnerProcess);
	if (err==KErrNone) 
		{
		server.Resume();
		User::WaitForRequest(status);
		server.Close();
		}
	rv.iCaller.Close();
	}

TInt CWin32Stream::ThreadFunction(TAny* aPtr)
	{
	struct rendezvous* rvp=(struct rendezvous*)aPtr;
	TInt err=KErrNoMemory;
	CWin32Stream* stream = new CWin32Stream();
	if (stream!=0)
		err=stream->Init(rvp->iStream, rvp->iStreamName);
	rvp->iCaller.RequestComplete(rvp->iStatus,err);	// let the caller continue
	if (err==KErrNone)
		{
		FOREVER
			stream->HandleMessage();
		}
	else
		{
		delete stream;
		}
	return err;
	}

TInt CWin32Stream::Init(TInt aStream, const TDesC& aStreamName)
	{
	iHandle = GetStdHandle((STD_INPUT_HANDLE)-(int)aStream);
	if (iHandle==INVALID_HANDLE_VALUE)
		return KErrNotFound;

#ifdef ESTW32_LOGGING
	static char* logs[3]={ "x:\\stdin.log", "x:\\stdout.log", "x:\\stderr.log"};
	iLogHandle=CreateFile(logs[aStream], GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
#else
	iLogHandle=INVALID_HANDLE_VALUE;
#endif

	return iServer.CreateGlobal(aStreamName);
	}

void CWin32Stream::HandleMessage()
	{
	iServer.Receive(iMessage);
	TInt err=KErrNone;
	TInt f=iMessage.Function();
	switch (f)
		{
	case RMessage2::EConnect:
	case RMessage2::EDisConnect:
		// RServer2 connection management - nothing interesting to be done
		break;

	case EWin32Read:
		err=ReadStream(iMessage);
		break;
	case EWin32Write:
		err=WriteStream(iMessage);
		break;
	case EWin32Flush:
		FlushStream();
		break;

	default:
		err=KErrNotSupported;
		break;
		}
	iMessage.Complete(err);
	}

TInt CWin32Stream::ReadStream(RMessage2& aMessage)
	{
	TDes8* bufDes = (TDes8*)(aMessage.Ptr0());
	int length = aMessage.Int1();
	unsigned long nbytes;

	if (ReadFile(iHandle, (TAny*)(bufDes->Ptr()), length, &nbytes, 0) && nbytes>0)
		{
		bufDes->SetLength(nbytes);
		return KErrNone;
		}
	return MapWin32Error(KErrEof);
	}

TInt CWin32Stream::WriteStream(RMessage2& aMessage)
	{
	TDesC8* bufDes = (TDesC8*)(aMessage.Ptr0());
	int length = aMessage.Int1();
	int offset = 0;
	unsigned long nbytes;

#ifdef ESTW32_LOGGING
	WriteFile(iLogHandle, bufDes->Ptr(), length, &nbytes, 0);
	FlushFileBuffers(iLogHandle);
#endif

	while (length>offset)
		{
		if (!WriteFile(iHandle, bufDes->Ptr()+offset, length-offset, &nbytes, 0))
			return MapWin32Error(KErrEof);
		offset+=nbytes;
		}
	return KErrNone;
	}

void CWin32Stream::FlushStream()
	{
	FlushFileBuffers(iHandle);	// don't care if it works or not
	}

#include <winerror.h>

TInt CWin32Stream::MapWin32Error(TInt aDefault)
	{
	switch (GetLastError())
		{
	case ERROR_INVALID_HANDLE:
		return KErrBadHandle;
	case ERROR_HANDLE_EOF:
		return KErrEof;
		}
	return aDefault;
	}
