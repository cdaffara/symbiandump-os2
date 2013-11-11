// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include "IPCBuf.h"
#include "SqlAssert.h"
#include "SqlDbSession.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "IPCBufTraces.h"
#endif
#include "SqlTraceDef.h"

/**
Standard, phase-one HIpcBuf factory method.
Created HIpcBuf instance will be used for transfering large text or binary objects from/to SQL server.
The created HIpcBuf instance will be placed in the cleanup stack.

@param aSession A reference to RSqlDbSession instance.
@param aFunction Prepared function code (with all statement handle bits set)
@param aArgs A set of IPC arguments to be sent to the SQL server.

@return A pointer to the created HIpcBuf instance

@leave KErrNoMemory Out of memory.
*/
HIpcBuf* HIpcBuf::NewLC(RSqlDbSession& aSession, TInt aFunction, TIpcArgs& aArgs)
	{
	HIpcBuf* self = new (ELeave) HIpcBuf(aSession);
	self->PushL();
	self->ConstructL(aFunction, aArgs);
	return self;
	}

/**
Standard, phase-one HIpcBuf factory method.
Created HIpcBuf instance will be used for transfering large text or binary objects from/to SQL server.

@param aSession A reference to RSqlDbSession instance.
@param aFunction Prepared function code (with all statement handle bits set)
@param aArgs A set of IPC arguments to be sent to the SQL server.

@return A pointer to the created HIpcBuf instance

@leave KErrNoMemory Out of memory.
*/
HIpcBuf* HIpcBuf::NewL(RSqlDbSession& aSession, TInt aFunction, TIpcArgs& aArgs)
	{
	HIpcBuf* self = NewLC(aSession, aFunction, aArgs);
	CleanupStack::Pop();
	return self;
	}

/**
Standard, phase-two HIpcBuf construction method.

@param aFunction The command code which will be sent to the SQL server
@param aArgs A set of IPC arguments to be sent to the SQL server.

@leave KErrNoMemory Out of memory.

Usage of the IPC call arguments:
Arg 2: [in/out]  IPC buffer
iBuf.iExt: [in]  stream size in bytes
*/
void HIpcBuf::ConstructL(TInt aFunction, TIpcArgs& aArgs)
	{
	TPckg<TIpcStreamBuf> pckg(iBuf);
	aArgs.Set(2, &pckg);
	__SQLLEAVE_IF_ERROR(iHandle = iSession.SendReceive(aFunction, aArgs));
    SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, HIPCBUF_CONSTRUCTL, "0x%X;HIpcBuf::ConstructL;iHandle=%d", (TUint)this, iHandle));
	TUint8* base = iBuf.iData;
	// if reading we already have one buffer-full of data
	TInt avail = Max(0, Min(iBuf.iExt, KIpcBufSize));
	SetBuf(ERead, base, base + avail);
	SetPos(ERead, avail);
	SetBuf(EWrite, base, base);
	SetPos(EWrite, 0);
	}

/**
@param aSession A reference to a sesion object.
*/
HIpcBuf::HIpcBuf(RSqlDbSession& aSession) :
	iSession(aSession),
	iHandle(0),
	iRPos(0),
	iWPos(0)
	{
	iBuf.iExt = -1;
	}

/**
*/
HIpcBuf::~HIpcBuf()
	{
    SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, HIPCBUF_HIPCBUFL2, "0x%X;HIpcBuf::~HIpcBuf;iHandle=%d", (TUint)this, iHandle));
	if(iHandle > 0) //iHandle is valid only when > 0.
		{
		(void)iSession.SendReceive(::MakeMsgCode(ESqlSrvStreamClose, ESqlSrvStreamHandle, iHandle));
		}
	}

/**
Fill the buffer's read area.
*/
TInt HIpcBuf::UnderflowL(TInt)
	{
	// when handle is null there is no data to read from server
	if(!iHandle)
		{
		return 0;
		}
	__ASSERT_DEBUG(Avail(ERead) == 0, __SQLPANIC(ESqlPanicInternalError));
	TUint8* base=iBuf.iData;
	IpcWriteL(base,Lag(EWrite));
	SetBuf(EWrite,base,base);

	TInt len=IpcReadL(base,iBuf.ESize);
	SetBuf(ERead,base,base+len);
	return len;
	}

/**
Set up the buffer's write area.
*/
void HIpcBuf::OverflowL()
	{
	__ASSERT_DEBUG(Avail(EWrite) == 0, __SQLPANIC(ESqlPanicInternalError));
	
	TUint8* base = iBuf.iData;
	MovePos(ERead, Lag(ERead));
	SetBuf(ERead, base, base);

	IpcWriteL(base, Lag(EWrite));
	SetBuf(EWrite, base, base + iBuf.ESize);
	}

/**
Destroys HIpcBuf instance.
*/
void HIpcBuf::DoRelease()
	{
	delete this;
	}

/**
Synchronise this buffer with its file, giving up on outstanding writes in case of failure.
*/
void HIpcBuf::DoSynchL()
	{
	TUint8* base = iBuf.iData;
	MovePos(ERead, Lag(ERead));
	TInt lag = Lag(EWrite);
	SetBuf(ERead | EWrite, base, base);
	iBuf.iExt = -1;
	IpcWriteL(base, lag);
	__SQLLEAVE_IF_ERROR(iSession.SendReceive(::MakeMsgCode(ESqlSrvStreamSynch, ESqlSrvStreamHandle, iHandle)));
	}

/**
Read direct from ipc if asked to transfer more than a bufferful.
*/
TInt HIpcBuf::DoReadL(TAny* aPtr, TInt aMaxLength)
	{
	__ASSERT_DEBUG(aMaxLength > 0, __SQLPANIC(ESqlPanicInternalError));
	TInt avail = Avail(ERead);
	__ASSERT_DEBUG(avail >= 0 && Avail(EWrite) >= 0, __SQLPANIC(ESqlPanicInternalError));
	if(avail > 0)
		{
		TInt len = Min(aMaxLength, avail);
		TUint8* ptr = Ptr(ERead);
		aPtr = Mem::Copy(aPtr, ptr, len);
		SetPtr(ERead, ptr + len);
		aMaxLength -= len;
		if(aMaxLength == 0)
			return len; // that's it
		}
	__ASSERT_DEBUG(Avail(ERead) == 0, __SQLPANIC(ESqlPanicInternalError));
	if(aMaxLength < iBuf.ESize)
		return avail + TStreamBuf::DoReadL(aPtr, aMaxLength);

	// when handle is null there is no more data to read from server
	if(!iHandle)
		{
		return 0;
		}

	TUint8* base = iBuf.iData;
	IpcWriteL(base, Lag(EWrite));
	SetBuf(ERead | EWrite, base, base);
	return avail + IpcReadL(aPtr, aMaxLength);
	}

/**
Write direct to ipc if asked to transfer more than a bufferful.
*/
void HIpcBuf::DoWriteL(const TAny* aPtr,TInt aLength)
	{
	__ASSERT_DEBUG(aLength > 0, __SQLPANIC(ESqlPanicInternalError));
	TInt avail = Avail(EWrite);
	__ASSERT_DEBUG(Avail(ERead) >= 0 && avail >= 0, __SQLPANIC(ESqlPanicInternalError));
	if(avail > 0)
		{
		TInt len = Min(aLength, avail);
		SetPtr(EWrite, Mem::Copy(Ptr(EWrite), aPtr, len));
		aLength -= len;
		if(aLength == 0)
			return; // done

		aPtr = (TUint8*)aPtr + len;
		}
	__ASSERT_DEBUG(Avail(EWrite) == 0, __SQLPANIC(ESqlPanicInternalError));
	if(aLength < iBuf.ESize)
		TStreamBuf::DoWriteL(aPtr, aLength);
	else
		{
		TUint8* base = iBuf.iData;
		IpcWriteL(base, Lag(EWrite));
		MovePos(ERead, Lag(ERead));
		SetBuf(ERead | EWrite, base, base);
		IpcWriteL(aPtr, aLength);
		}
	}

/**
Position the mark(s) indicated by aMark at aOffset from aLocation.
*/
TStreamPos HIpcBuf::DoSeekL(TMark aMark, TStreamLocation aLocation, TInt aOffset)
	{
	TUint8* base = iBuf.iData;
	TInt end = EndL();

	switch(aLocation)
		{
	case EStreamBeginning:
		break;
	case EStreamMark:
		switch(aMark)
			{
		case ERead:
			aOffset += Mark(ERead);
			break;
		case EWrite:
			aOffset += Mark(EWrite);
			break;
		default:
		    __ASSERT_ALWAYS(0, __SQLPANIC(ESqlPanicStreamMarkInvalid));
			break;
			}
		break;
	case EStreamEnd:
		aOffset += end;
		break;
	default:
	    __ASSERT_ALWAYS(0, __SQLPANIC(ESqlPanicStreamLocationInvalid));
		break;
		}
	TInt r = KErrNone;
	if(aOffset < 0)
		{
		aOffset = 0;
		r = KErrEof;
		}
	else if(aOffset > end)
		{
		aOffset = end;
		r = KErrEof;
		}

	__ASSERT_ALWAYS(!(aMark & ~(ERead | EWrite)), __SQLPANIC(ESqlPanicStreamMarkInvalid));
	if(aMark & ERead)
		{
		TInt lag = aOffset - Pos(ERead);
		if(lag >= base - End(ERead) && lag <= 0)
			SetPtr(ERead, End(ERead) + lag);
		else
			{
			SetPos(ERead, aOffset);
			SetBuf(ERead, base, base);
			}
		}
	if(aMark & EWrite && aOffset != Mark(EWrite))
		{
		IpcWriteL(base, Lag(EWrite));
		SetPos(EWrite, aOffset);
		SetBuf(EWrite, base, base);
		}
	__SQLLEAVE_IF_ERROR(r);
	return TStreamPos(aOffset);
	}

/**
Read from the server at the current read position.
Arg 0:            not used
Arg 1: [out]      from which position to read
Arg 2: [in/out]   IPC buffer
Arg 3: [out]      max length of the requested data
*/
TInt HIpcBuf::IpcReadL(TAny* aPtr, TInt aMaxLength)
	{
	__ASSERT_DEBUG(aMaxLength >= 0, __SQLPANIC(ESqlPanicInternalError));
	if(aMaxLength == 0)
		return 0;

	TPtr8 des((TUint8*)aPtr, aMaxLength);
	TInt pos = Pos(ERead);
		
	TInt len = __SQLLEAVE_IF_ERROR(iSession.SendReceive(::MakeMsgCode(ESqlSrvStreamRead, ESqlSrvStreamHandle, iHandle), TIpcArgs(0, pos, &des, aMaxLength)));
	pos += len;
	if(len < aMaxLength)
		iBuf.iExt = pos; // end-of-file encountered
	SetPos(ERead, pos);
	return len;
	}

/**
Write to the server at the current write position.
Arg 0:            not used
Arg 1: [out]      from which position to write
Arg 2: [in/out]   IPC buffer
*/
void HIpcBuf::IpcWriteL(const TAny* aPtr, TInt aLength)
	{
	__ASSERT_DEBUG(aLength >= 0, __SQLPANIC(ESqlPanicInternalError));
	if(aLength == 0)
		return;

	TPtrC8 ptr((TUint8*)aPtr, aLength);
	TInt ext = iBuf.iExt;
	iBuf.iExt = -1;
	TInt pos = Pos(EWrite);
	__SQLLEAVE_IF_ERROR(iSession.SendReceive(::MakeMsgCode(ESqlSrvStreamWrite, ESqlSrvStreamHandle, iHandle), TIpcArgs(0, pos, &ptr)));
	pos += aLength;
	if(ext >=0 && pos > ext)
		iBuf.iExt = pos;
	SetPos(EWrite, pos);
	}

/**
Determine the end of the stream
*/
TInt HIpcBuf::EndL()
	{
	TInt ext = iBuf.iExt;
	if(ext < 0)
		{
		iBuf.iExt = ext = __SQLLEAVE_IF_ERROR(iSession.SendReceive(::MakeMsgCode(ESqlSrvStreamSize, ESqlSrvStreamHandle, iHandle)));
		}
	return Max(ext, Mark(EWrite));
	}

