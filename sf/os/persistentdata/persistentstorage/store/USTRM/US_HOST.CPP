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

#include "US_STD.H"

#define UNUSED_VAR(a) a = a

EXPORT_C TInt TStreamMark::ReadL(TStreamExchange& aHost,TDes8& aDes,TRequestStatus& aStatus)
//
// Read asynchronously.
//
	{
	return aHost.DoReadL(aDes,aDes.MaxLength(),aStatus,*this);
	}

EXPORT_C TInt TStreamMark::WriteL(TStreamExchange& aHost,const TDesC8& aDes,TRequestStatus& aStatus)
/** Writes data, asynchronously, from the specified descriptor into the shared 
stream.

The maximum number of bytes to be written is the value of the maximum length 
of the descriptor.

If the function leaves, then no write request will have been initiated.

@param aHost The object that manages shared streaming.
@param aDes The source descriptor for the data to be written into the shared 
stream.
@param aStatus The request status that indicates the completion status of this 
asynchronous request.
@return The maximum number of bytes to be written, as used in this request. 
This can be different to the maximum length of the descriptor; this is dependent 
on the implementation. */
	{
	return aHost.DoWriteL(aDes,aDes.Length(),aStatus,*this);
	}

EXPORT_C MStreamBuf* TStreamExchange::Host()
/** Gets a pointer to the stream buffer that acts as the shared host.

The function refreshes the active read/write marks and gives the caller exclusive 
use of the shared host.

@return A pointer to a stream buffer that acts as shared host. */
	{
	__ASSERT_DEBUG((iRMrk==NULL||iRMrk!=iWMrk&&iRMrk->IsTracking(iRMrk))&&(iWMrk==NULL||iWMrk->IsTracking(iWMrk)),User::Invariant());
	if (iRMrk!=NULL)
		{
		TRAPD(ignore,*iRMrk=Buf().TellL(MStreamBuf::ERead));
        UNUSED_VAR(ignore);
		iRMrk=NULL;
		}
	if (iWMrk!=NULL)
		{
		TRAPD(ignore,*iWMrk=Buf().TellL(MStreamBuf::EWrite));
        UNUSED_VAR(ignore);
		iWMrk=NULL;
		}
	return iHost;
	}

EXPORT_C MStreamBuf* TStreamExchange::HostL()
/** Gets a pointer to the stream buffer that acts as the shared host, and leaves 
if this object is not active.

The function refreshes the active read/write marks and gives the caller exclusive 
use of the shared host.

@return A pointer to a stream buffer that acts as shared host.
@see IsActive() */
	{
	MStreamBuf* host=Host();
	if (host==NULL)
		__LEAVE(KErrNotReady);
	return host;
	}

EXPORT_C void TStreamExchange::Release()
/** Releases the reference to the shared host, and drops any active read or write 
marks. */
	{
	__ASSERT_DEBUG((iRMrk==NULL||iRMrk!=iWMrk&&iRMrk->IsTracking(iRMrk))&&(iWMrk==NULL||iWMrk->IsTracking(iWMrk)),User::Invariant());
	iHost=NULL;
	iRMrk=NULL;
	iWMrk=NULL;
	}

EXPORT_C TInt TStreamExchange::SizeL() const
/** Gets the size of the shared host buffer.

@return The size of the shared host buffer. */
	{
	return BufL().SizeL();
	}

EXPORT_C TBool TStreamExchange::RefersTo(const TStreamMark& aMark)
//
// Return whether aMark is referred to.
//
	{
	__ASSERT_DEBUG((iRMrk==NULL||iRMrk!=iWMrk&&iRMrk->IsTracking(iRMrk))&&(iWMrk==NULL||iWMrk->IsTracking(iWMrk)),User::Invariant());
	return iRMrk==aMark||iWMrk==aMark;
	}

EXPORT_C void TStreamExchange::Drop(const TStreamMark& aMark)
//
// Drop any reference to aMark.
//
	{
	__ASSERT_DEBUG((iRMrk==NULL||iRMrk!=iWMrk&&iRMrk->IsTracking(iRMrk))&&(iWMrk==NULL||iWMrk->IsTracking(iWMrk)),User::Invariant());
	if (iRMrk==aMark)
		iRMrk=NULL;
	else if (iWMrk==aMark)
		iWMrk=NULL;
	}

EXPORT_C void TStreamExchange::GetL(TStreamMark& aMark)
//
// Refresh and drop any reference to aMark.
//
	{
	__ASSERT_DEBUG((iRMrk==NULL||iRMrk!=iWMrk&&iRMrk->IsTracking(iRMrk))&&(iWMrk==NULL||iWMrk->IsTracking(iWMrk)),User::Invariant());
	if (iRMrk==aMark)
		{
		iRMrk=NULL;
		aMark=Buf().TellL(MStreamBuf::ERead);
		}
	else if (iWMrk==aMark)
		{
		iWMrk=NULL;
		aMark=Buf().TellL(MStreamBuf::EWrite);
		}
	}

EXPORT_C TInt TStreamExchange::DoReadL(TAny* aPtr,TInt aMaxLength,TStreamMark& aMark)
//
// Read up to aMaxLength bytes starting at aMark.
//
	{
	__ASSERT_DEBUG(iWMrk==NULL||iWMrk!=iRMrk&&iWMrk->IsTracking(iWMrk),User::Invariant());
	if (iRMrk!=aMark)
		return PrepareAndReadL(aPtr,aMaxLength,aMark);
//
	__ASSERT_DEBUG(aMark.IsTracking(iRMrk),User::Invariant());
	iRMrk=NULL;
	TInt len=Buf().ReadL(aPtr,aMaxLength);
	iRMrk=aMark;
	return len;
	}

EXPORT_C TInt TStreamExchange::DoReadL(TDes8& aDes,TInt aMaxLength,TRequestStatus& aStatus,TStreamMark& aMark)
//
// Read up to aMaxLength bytes asynchronously.
//
	{
	__ASSERT_DEBUG(iWMrk==NULL||iWMrk!=iRMrk&&iWMrk->IsTracking(iWMrk),User::Invariant());
	if (iRMrk!=aMark)
		return PrepareAndReadL(aDes,aMaxLength,aStatus,aMark);
//
	__ASSERT_DEBUG(aMark.IsTracking(iRMrk),User::Invariant());
	iRMrk=NULL;
	TInt len=Buf().ReadL(aDes,aMaxLength,aStatus);
	iRMrk=aMark;
	return len;
	}

EXPORT_C TStreamTransfer TStreamExchange::DoReadL(MStreamInput& anInput,TStreamTransfer aTransfer,TStreamMark& aMark)
//
// Push up to aTransfer bytes into anInput, starting at aMark.
//
	{
	__ASSERT_DEBUG(iWMrk==NULL||iWMrk!=iRMrk&&iWMrk->IsTracking(iWMrk),User::Invariant());
	if (iRMrk!=aMark)
		return PrepareAndReadL(anInput,aTransfer,aMark);
//
	__ASSERT_DEBUG(aMark.IsTracking(iRMrk),User::Invariant());
	iRMrk=NULL;
	TStreamTransfer trans=Buf().ReadL(anInput,aTransfer);
	iRMrk=aMark;
	return trans;
	}

EXPORT_C void TStreamExchange::DoWriteL(const TAny* aPtr,TInt aLength,TStreamMark& aMark)
//
// Write aLength bytes starting at aMark.
//
	{
	__ASSERT_DEBUG(iRMrk==NULL||iRMrk!=iWMrk&&iRMrk->IsTracking(iRMrk),User::Invariant());
	if (iWMrk==aMark)
		{
		__ASSERT_DEBUG(aMark.IsTracking(iWMrk),User::Invariant());
		iWMrk=NULL;
		Buf().WriteL(aPtr,aLength);
		iWMrk=aMark;
		}
	else
		PrepareAndWriteL(aPtr,aLength,aMark);
	}

EXPORT_C TInt TStreamExchange::DoWriteL(const TDesC8& aDes,TInt aMaxLength,TRequestStatus& aStatus,TStreamMark& aMark)
//
// Write up to aMaxLength bytes asynchronously.
//
	{
	__ASSERT_DEBUG(iRMrk==NULL||iRMrk!=iWMrk&&iRMrk->IsTracking(iRMrk),User::Invariant());
	if (iWMrk!=aMark)
		return PrepareAndWriteL(aDes,aMaxLength,aStatus,aMark);
//
	__ASSERT_DEBUG(aMark.IsTracking(iWMrk),User::Invariant());
	iWMrk=NULL;
	TInt len=Buf().WriteL(aDes,aMaxLength,aStatus);
	iWMrk=aMark;
	return len;
	}

EXPORT_C TStreamTransfer TStreamExchange::DoWriteL(MStreamOutput& anOutput,TStreamTransfer aTransfer,TStreamMark& aMark)
//
// Pull up to aTransfer bytes from anOutput, starting at aMark.
//
	{
	__ASSERT_DEBUG(iRMrk==NULL||iRMrk!=iWMrk&&iRMrk->IsTracking(iRMrk),User::Invariant());
	if (iWMrk!=aMark)
		return PrepareAndWriteL(anOutput,aTransfer,aMark);
//
	__ASSERT_DEBUG(aMark.IsTracking(iWMrk),User::Invariant());
	iWMrk=NULL;
	TStreamTransfer trans=Buf().WriteL(anOutput,aTransfer);
	iWMrk=aMark;
	return trans;
	}

EXPORT_C TStreamPos TStreamExchange::DoSeekL(TStreamMark& aMark,TStreamLocation aLocation,TInt anOffset)
//
// Position aMark at anOffset from aLocation.
//
	{
	__ASSERT_DEBUG((iRMrk==NULL||iRMrk!=iWMrk&&iRMrk->IsTracking(iRMrk))&&(iWMrk==NULL||iWMrk->IsTracking(iWMrk)),User::Invariant());
	if (iRMrk==aMark)
		{
		iRMrk=NULL;
		TStreamPos pos=Buf().SeekL(MStreamBuf::ERead,aLocation,anOffset);
		iRMrk=aMark;
		return pos;
		}
//
	if (iWMrk==aMark)
		{
		iWMrk=NULL;
		TStreamPos pos=Buf().SeekL(MStreamBuf::EWrite,aLocation,anOffset);
		iWMrk=aMark;
		return pos;
		}
//
	return MarkSeekL(aMark,aLocation,anOffset);
	}

void TStreamExchange::PrepareForReadingL(TStreamMark& aMark)
//
// Prepare the host buffer to start reading at aMark.
//
	{
	__ASSERT_DEBUG(iRMrk==NULL||iRMrk->IsTracking(iRMrk),User::Invariant());
	if (iWMrk==aMark)
		{
		iWMrk=NULL;
		aMark=Buf().TellL(MStreamBuf::EWrite);
		}
	else if (aMark.IsEmpty())
		__LEAVE(KErrNotReady);
//
	if (iRMrk!=NULL)
		{
		TRAPD(ignore,*iRMrk=Buf().TellL(MStreamBuf::ERead));
        UNUSED_VAR(ignore);
		iRMrk=NULL;
		}
//
	TStreamPos pos=aMark.Position();
	aMark.Track(iRMrk);
	BufL().SeekL(MStreamBuf::ERead,pos);
	}

void TStreamExchange::PrepareForWritingL(TStreamMark& aMark)
//
// Prepare the host buffer to start writing at aMark.
//
	{
	__ASSERT_DEBUG(iWMrk==NULL||iWMrk->IsTracking(iWMrk),User::Invariant());
	if (iRMrk==aMark)
		{
		iRMrk=NULL;
		aMark=Buf().TellL(MStreamBuf::ERead);
		}
	else if (aMark.IsEmpty())
		__LEAVE(KErrNotReady);
//
	if (iWMrk!=NULL)
		{
		TRAPD(ignore,*iWMrk=Buf().TellL(MStreamBuf::EWrite));
        UNUSED_VAR(ignore);
		iWMrk=NULL;
		}
//
	TStreamPos pos=aMark.Position();
	aMark.Track(iWMrk);
	BufL().SeekL(MStreamBuf::EWrite,pos);
	}

TInt TStreamExchange::PrepareAndReadL(TAny* aPtr,TInt aMaxLength,TStreamMark& aMark)
	{
	PrepareForReadingL(aMark);
	TInt len=Buf().ReadL(aPtr,aMaxLength);
	iRMrk=aMark;
	return len;
	}

TInt TStreamExchange::PrepareAndReadL(TDes8& aDes,TInt aMaxLength,TRequestStatus& aStatus,TStreamMark& aMark)
	{
	PrepareForReadingL(aMark);
	TInt len=Buf().ReadL(aDes,aMaxLength,aStatus);
	iRMrk=aMark;
	return len;
	}

TStreamTransfer TStreamExchange::PrepareAndReadL(MStreamInput& anInput,TStreamTransfer aTransfer,TStreamMark& aMark)
	{
	PrepareForReadingL(aMark);
	TStreamTransfer trans=Buf().ReadL(anInput,aTransfer);
	iRMrk=aMark;
	return trans;
	}

void TStreamExchange::PrepareAndWriteL(const TAny* aPtr,TInt aLength,TStreamMark& aMark)
	{
	PrepareForWritingL(aMark);
	Buf().WriteL(aPtr,aLength);
	iWMrk=aMark;
	}

TInt TStreamExchange::PrepareAndWriteL(const TDesC8& aDes,TInt aMaxLength,TRequestStatus& aStatus,TStreamMark& aMark)
	{
	PrepareForWritingL(aMark);
	TInt len=Buf().WriteL(aDes,aMaxLength,aStatus);
	iWMrk=aMark;
	return len;
	}

TStreamTransfer TStreamExchange::PrepareAndWriteL(MStreamOutput& anOutput,TStreamTransfer aTransfer,TStreamMark& aMark)
	{
	PrepareForWritingL(aMark);
	TStreamTransfer trans=Buf().WriteL(anOutput,aTransfer);
	iWMrk=aMark;
	return trans;
	}

TStreamPos TStreamExchange::MarkSeekL(TStreamMark& aMark,TStreamLocation aLocation,TInt anOffset)
	{
	if (aLocation==EStreamMark)
		{
		if (aMark.IsEmpty())
			__LEAVE(KErrNotReady);
//
		aLocation=EStreamBeginning;
		anOffset+=aMark.Position().Offset();
		}
	aMark.Clear();
	TStreamPos pos=BufL().SeekL(0,aLocation,anOffset);
	aMark=pos;
	return pos;
	}

EXPORT_C void TStreamMark::__DbgChkPos(TStreamPos aPos)
//
// Check for a negative position.
//
	{
	__ASSERT_ALWAYS(aPos>=KStreamBeginning,Panic(EStreamPosInvalid));
	}

