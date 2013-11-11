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
// DBMS server stream buffer classes
// 
//



#include "SD_STD.H"

HDbsBuf* HDbsBuf::NewLC(const RDbsObject& aObject,TDbsFunction aFunction,TIpcArgs& aArgs)
	{
	HDbsBuf* self=new(ELeave) HDbsBuf;
	self->PushL();
	self->ConstructL(aObject,aFunction,aArgs);
	return self;
	}

HDbsBuf* HDbsBuf::NewL(const RDbsObject& aObject,TDbsFunction aFunction,TIpcArgs& aArgs)
	{
	HDbsBuf* self=NewLC(aObject,aFunction,aArgs);
	CleanupStack::Pop();
	return self;
	}

void HDbsBuf::ConstructL(const RDbsObject& aObject,TDbsFunction aFunction,TIpcArgs& aArgs)
	{
	TPckg<TDbsStreamBuf> pckg(iBuf);
	aArgs.Set(3,&pckg);
	iIpc.OpenL(aObject,aFunction,aArgs);
	TUint8* base=iBuf.iData;
	// if reading we already have one buffer-full of data
	TInt avail=Max(0,Min(iBuf.iExt,KDbsStreamBufSize));
	SetBuf(ERead,base,base+avail);
	SetPos(ERead,avail);
	SetBuf(EWrite,base,base);
	SetPos(EWrite,0);
	}

TInt HDbsBuf::UnderflowL(TInt)
//
// Fill the buffer's read area.
//
	{
	// when handle is null there is no data to read from server
	if(!iIpc.Handle())
		return 0;

	__ASSERT(Avail(ERead)==0);
	TUint8* base=iBuf.iData;
	IpcWriteL(base,Lag(EWrite));
	SetBuf(EWrite,base,base);
//
	TInt len=IpcReadL(base,iBuf.ESize);
	SetBuf(ERead,base,base+len);
	return len;
	}

void HDbsBuf::OverflowL()
//
// Set up the buffer's write area.
//
	{
	__ASSERT(Avail(EWrite)==0);
	TUint8* base=iBuf.iData;
	MovePos(ERead,Lag(ERead));
	SetBuf(ERead,base,base);
//
	IpcWriteL(base,Lag(EWrite));
	SetBuf(EWrite,base,base+iBuf.ESize);
	}

void HDbsBuf::DoRelease()
	{
	delete this;
	}

void HDbsBuf::DoSynchL()
//
// Synchronise this buffer with its file, giving up on outstanding writes in case of failure.
//
	{
	TUint8* base=iBuf.iData;
	MovePos(ERead,Lag(ERead));
	TInt lag=Lag(EWrite);
	SetBuf(ERead|EWrite,base,base);
	iBuf.iExt=-1;
	IpcWriteL(base,lag);
	iIpc.SendReceiveL(EDbsStreamSynch);
	}

TInt HDbsBuf::DoReadL(TAny* aPtr,TInt aMaxLength)
//
// Read direct from ipc if asked to transfer more than a bufferful.
//
	{
	__ASSERT(aMaxLength>=0);
	__ASSERT(aMaxLength>0);
	TInt avail=Avail(ERead);
	__ASSERT(avail>=0&&Avail(EWrite)>=0);
	if (avail>0)
		{
		TInt len=Min(aMaxLength,avail);
		TUint8* ptr=Ptr(ERead);
		aPtr=Mem::Copy(aPtr,ptr,len);
		SetPtr(ERead,ptr+len);
		aMaxLength-=len;
		if (aMaxLength==0)
			return len; // that's it
		}
	__ASSERT(Avail(ERead)==0);
	if (aMaxLength<iBuf.ESize)
		return avail+TStreamBuf::DoReadL(aPtr,aMaxLength);
//
	// when handle is null there is no more data to read from server
	if(!iIpc.Handle())
		return avail;

	TUint8* base=iBuf.iData;
	IpcWriteL(base,Lag(EWrite));
	SetBuf(ERead|EWrite,base,base);
	return avail+IpcReadL(aPtr,aMaxLength);
	}

void HDbsBuf::DoWriteL(const TAny* aPtr,TInt aLength)
//
// Write direct to ipc if asked to transfer more than a bufferful.
//
	{
	__ASSERT(aLength>=0);
	__ASSERT(aLength>0);
	TInt avail=Avail(EWrite);
	__ASSERT(Avail(ERead)>=0&&avail>=0);
	if (avail>0)
		{
		TInt len=Min(aLength,avail);
		SetPtr(EWrite,Mem::Copy(Ptr(EWrite),aPtr,len));
		aLength-=len;
		if (aLength==0)
			return; // done
//
		aPtr=(TUint8*)aPtr+len;
		}
	__ASSERT(Avail(EWrite)==0);
	if (aLength<iBuf.ESize)
		TStreamBuf::DoWriteL(aPtr,aLength);
	else
		{
		TUint8* base=iBuf.iData;
		IpcWriteL(base,Lag(EWrite));
		MovePos(ERead,Lag(ERead));
		SetBuf(ERead|EWrite,base,base);
		IpcWriteL(aPtr,aLength);
		}
	}

TStreamPos HDbsBuf::DoSeekL(TMark aMark,TStreamLocation aLocation,TInt anOffset)
//
// Position the mark(s) indicated by aMark at anOffset from aLocation.
//
	{
	TUint8* base=iBuf.iData;
	TInt end=EndL();
//
	switch (aLocation)
		{
	case EStreamBeginning:
		break;
	case EStreamMark:
		switch (aMark)
			{
		case ERead:
			anOffset+=Mark(ERead);
			break;
		case EWrite:
			anOffset+=Mark(EWrite);
			break;
		default:
			Panic(EDbsStreamMarkInvalid);
			break;
			}
		break;
	case EStreamEnd:
		anOffset+=end;
		break;
	default:
		Panic(EDbsStreamLocationInvalid);
		break;
		}
	TInt r=KErrNone;
	if (anOffset<0)
		{
		anOffset=0;
		r=KErrEof;
		}
	else if (anOffset>end)
		{
		anOffset=end;
		r=KErrEof;
		}
//
	__ASSERT_ALWAYS(!(aMark&~(ERead|EWrite)),Panic(EDbsStreamMarkInvalid));
	if (aMark&ERead)
		{
		TInt lag=anOffset-Pos(ERead);
		if (lag>=base-End(ERead)&&lag<=0)
			SetPtr(ERead,End(ERead)+lag);
		else
			{
			SetPos(ERead,anOffset);
			SetBuf(ERead,base,base);
			}
		}
	if (aMark&EWrite&&anOffset!=Mark(EWrite))
		{
		IpcWriteL(base,Lag(EWrite));
		SetPos(EWrite,anOffset);
		SetBuf(EWrite,base,base);
		}
	__LEAVE_IF_ERROR(r);
	return TStreamPos(anOffset);
	}

TInt HDbsBuf::IpcReadL(TAny* aPtr,TInt aMaxLength)
//
// Read from the server at the current read position.
//
	{
	__ASSERT(aMaxLength>=0);
	if (aMaxLength==0)
		return 0;
//
	TPtr8 des((TUint8*)aPtr,aMaxLength);
	TInt pos=Pos(ERead);
		
	TInt len=iIpc.SendReceiveL(EDbsStreamRead,TIpcArgs(pos,&des,aMaxLength));
	pos+=len;
	if (len<aMaxLength)
		iBuf.iExt=pos; // end-of-file encountered
	SetPos(ERead,pos);
	return len;
	}

void HDbsBuf::IpcWriteL(const TAny* aPtr,TInt aLength)
//
// Write to the server at the current write position.
//
	{
	__ASSERT(aLength>=0);
	if (aLength==0)
		return;
//
	TPtrC8 ptr((TUint8*)aPtr,aLength);
	TInt ext=iBuf.iExt;
	iBuf.iExt=-1;
	TInt pos=Pos(EWrite);
	iIpc.SendReceiveL(EDbsStreamWrite,TIpcArgs(pos,&ptr));
	pos+=aLength;
	if (ext>=0&&pos>ext)
		iBuf.iExt=pos;
	SetPos(EWrite,pos);
	}

TInt HDbsBuf::EndL()
//
// Determine the end of the stream
//
	{
	TInt ext=iBuf.iExt;
	if (ext<0)
		iBuf.iExt=ext=iIpc.SendReceiveL(EDbsStreamSize);
	return Max(ext,Mark(EWrite));
	}

// Class HDbsReadBuf

inline HDbsReadBuf::HDbsReadBuf(const TDesC8& aDes)
	{
	TUint8* ptr=CONST_CAST(TUint8*,aDes.Ptr());
	Set(ptr,ptr+aDes.Length(),ERead);
	}

HDbsReadBuf* HDbsReadBuf::NewL(const TDesC8& aDes)
	{
	return new(ELeave) HDbsReadBuf(aDes);
	}

void HDbsReadBuf::DoRelease()
	{
	delete this;
	}

// Class HDbsStream
TInt HDbsStream::ReadL(const RMessage2& aMessage)
	{
	TInt pos=aMessage.Int0();
	if (pos!=iRPos)
		iHost.SeekL(iHost.ERead,EStreamBeginning,pos);
	iRPos=-1;
	TInt len=aMessage.Int2();
	pos+=len;
	TInt tfr=len;
	for (;;)
		{
		TUint8 buf[KDbsStreamIoSize];
		TInt read=iHost.ReadL(buf,Min(tfr,KDbsStreamIoSize));
		if (read==0)
			break;
		aMessage.WriteL(1,TPtrC8(buf,read),len-tfr);
		tfr-=read;
		if (tfr==0)
			break;
		if (read<KDbsStreamIoSize)
			break;
		}
	iRPos=pos-tfr;
	return len-tfr;
	}

void HDbsStream::WriteL(const RMessage2& aMessage)
	{
	TInt pos=aMessage.Int0();
	if (pos!=iWPos)
		iHost.SeekL(iHost.EWrite,EStreamBeginning,pos);
	iWPos=-1;
	TInt offset=0;
	TBuf8<KDbsStreamIoSize> buf;
	for (;;)
		{
		aMessage.ReadL(1,buf,offset);
		TInt len=buf.Length();
		if (len==0)
			break;
		iHost.WriteL(buf.Ptr(),len);
		offset+=len;
		if (len<KDbsStreamIoSize)
			break;
		}
	iWPos=pos+offset;
	}

// Class HBufBuf

void HBufBuf::DoRelease()
	{
	delete this;
	}

HBufBuf* HBufBuf::NewLC()
	{
	HBufBuf* self=new(ELeave) HBufBuf;
	self->PushL();
	self->iBuf=CBufSeg::NewL(EGranularity);
	self->Set(*self->iBuf,0,ERead|EWrite);
	return self;
	}

