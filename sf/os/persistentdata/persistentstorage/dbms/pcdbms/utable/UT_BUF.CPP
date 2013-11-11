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

#include "UT_STD.H"

// Class CDbTableCursor::HWriteBuf

inline CDbTableCursor::HWriteBuf::HWriteBuf(CDbTableCursor& aCursor,const TDbColumn& aColumn,TDbColType aType)
	: iCursor(aCursor),iColumn(aColumn),iType(aType),iBlob(0),iInlineBuf(0,0),iSize(0),iOverflowBuf(0)
	{aCursor.AddSink();}

void CDbTableCursor::HWriteBuf::ConstructL()
	{
	iBlob=&iColumn.InitBlobL();
	iInlineBuf.Set(iBlob->InlineBuffer(),0,iCursor.BlobsL().InlineLimit());
	Set(iInlineBuf);
	}

CDbTableCursor::HWriteBuf* CDbTableCursor::HWriteBuf::NewL(CDbTableCursor& aCursor,const TDbColumn& aColumn,TDbColType aType)
	{
	__ASSERT(TDbCol::IsLong(aType));
//
	HWriteBuf* self=new(ELeave) HWriteBuf(aCursor,aColumn,aType);
	self->PushL();
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

inline CDbTableCursor::HWriteBuf::~HWriteBuf()
	{
	if (iOverflowBuf)
		iOverflowBuf->Release();
	if (iBlob)
		iColumn.CommitBlob(*iBlob);
	iCursor.ReleaseSink();
	}

void CDbTableCursor::HWriteBuf::DoRelease()
	{
	delete this;
	}

inline TBool CDbTableCursor::HWriteBuf::IsBinary() const
	{return iType==EDbColLongBinary;}

void CDbTableCursor::HWriteBuf::FlipL()
//
// switch from inline to out of line stream
//
	{
	__ASSERT( !iOverflowBuf );
//
	TStreamPos rpos(0);
	if ( IsBinary() )	// seeking only allowed for binary (non-encrypted) data
		rpos = TDesBuf::TellL( ERead );
	iOverflowBuf = iCursor.BlobsL().CreateL( iBlobId, iType );
	const TUint8* base = iInlineBuf.Ptr();
	TInt len = Ptr( EWrite ) - base;
	if ( len )
		iOverflowBuf->WriteL( base, len );
	if ( IsBinary() )	// seeking only allowed for binary (non-encrypted) data
		iOverflowBuf->SeekL( ERead, rpos );
	}

void CDbTableCursor::HWriteBuf::DoSynchL()
//
// update the row buffer for the blob
//
	{
	MStreamBuf* buf=iOverflowBuf!=NULL ? iOverflowBuf : this;
	TInt size=IsBinary() ? buf->SizeL() : iSize;
	if (iOverflowBuf!=NULL)
		{
		buf->SynchL();
		iBlob->SetId(iBlobId);
		}
	iBlob->SetSize(size);
	}

TInt CDbTableCursor::HWriteBuf::DoReadL(TAny* aPtr,TInt aMaxLength)
	{
	__ASSERT(IsBinary());
//
	if (iOverflowBuf!=NULL)
		return iOverflowBuf->ReadL(aPtr,aMaxLength);
	return TDesBuf::DoReadL(aPtr,aMaxLength);
	}

TStreamTransfer CDbTableCursor::HWriteBuf::DoReadL(MStreamInput& aInput,TStreamTransfer aTransfer)
	{
	__ASSERT(IsBinary());
//
	if (iOverflowBuf!=NULL)
		return iOverflowBuf->ReadL(aInput,aTransfer);
	return TDesBuf::DoReadL(aInput,aTransfer);
	}

void CDbTableCursor::HWriteBuf::DoWriteL(const TAny* aPtr,TInt aLength)
	{
	if (iOverflowBuf==NULL)
		{
		if (aLength<=Avail(EWrite))
			{
			TDesBuf::DoWriteL(aPtr,aLength);
			iSize+=aLength;
			return;
			}
		FlipL();
		}
	iOverflowBuf->WriteL(aPtr,aLength);
	iSize+=aLength;
	}

TStreamTransfer CDbTableCursor::HWriteBuf::DoWriteL(MStreamOutput& aOutput,TStreamTransfer aTransfer)
	{
	TInt size=iSize;
	TStreamTransfer t1=aTransfer[KMaxTInt];
	TStreamTransfer t2=iOverflowBuf ? iOverflowBuf->WriteL(aOutput,t1) : TDesBuf::DoWriteL(aOutput,t1);
	TInt bytes=t1.Left()-t2.Left();
	iSize=size+bytes;
	return aTransfer-bytes;
	}

TStreamPos CDbTableCursor::HWriteBuf::DoSeekL(TMark aMark,TStreamLocation aLocation,TInt aOffset)
	{
	__ASSERT(IsBinary());
//
	if (iOverflowBuf!=NULL)
		return iOverflowBuf->SeekL(aMark,aLocation,aOffset);
	return TDesBuf::DoSeekL(aMark,aLocation,aOffset);
	}

// Class CDbTableCursor::HMemBuf

CDbTableCursor::HMemBuf::HMemBuf(CDbTableCursor& aCursor)
	:iCursor(aCursor)
	{
	aCursor.AddSource();
	}

CDbTableCursor::HMemBuf* CDbTableCursor::HMemBuf::NewL(CDbTableCursor& aCursor,const TDesC8& aDes)
	{
	HMemBuf* self=new(ELeave) HMemBuf(aCursor);
	TUint8* ptr=const_cast<TUint8*>(aDes.Ptr());
	self->Set(ptr,ptr+aDes.Length(),ERead);
	return self;
	}

inline CDbTableCursor::HMemBuf::~HMemBuf()
	{iCursor.ReleaseSource();}

void CDbTableCursor::HMemBuf::DoRelease()
	{	
	delete this;
	}

// Class CDbTableCursor::HHeapBuf

inline CDbTableCursor::HHeapBuf::HHeapBuf( CDbTableCursor& aCursor )
	:HMemBuf( aCursor )
	{}

CDbTableCursor::HHeapBuf* CDbTableCursor::HHeapBuf::NewL( CDbTableCursor& aCursor, const TDbBlob& aBlob, TDbColType aType )
	{
	__ASSERT( aBlob.Size() <= EMaxBlobBuffer );
	TAny* mem = User::AllocL(_FOFF(HHeapBuf,iBuf[aBlob.Size()]));	// get the extra size for the entries, leaves on error
	HHeapBuf* self = new( mem ) HHeapBuf(aCursor);	// do an in place new, now we've got the memory
	self->PushL();
	MStreamBuf* buf = aCursor.BlobsL().ReadLC( aBlob.Id(), aType );
	__DEBUG( TInt sz = ) buf->ReadL( &self->iBuf[0], aBlob.Size() );
	__ASSERT(sz == aBlob.Size());
	CleanupStack::PopAndDestroy();	// buf
	self->Set( &self->iBuf[0], &self->iBuf[aBlob.Size() ], ERead );
	CleanupStack::Pop();			// self
	return self;
	}

// Class CDbTableCursor::HReadBuf

inline CDbTableCursor::HReadBuf::HReadBuf(CDbTableCursor& aCursor)
	:iCursor(aCursor),iHost(0)
	{aCursor.AddBlobSource();}

CDbTableCursor::HReadBuf* CDbTableCursor::HReadBuf::NewLC(CDbTableCursor& aCursor)
	{
	HReadBuf* self=new(ELeave) HReadBuf(aCursor);
	self->PushL();
	return self;
	}

inline CDbTableCursor::HReadBuf::~HReadBuf()
	{
	if (iHost)
		iHost->Release();
	iCursor.ReleaseBlobSource();
	}

void CDbTableCursor::HReadBuf::DoRelease()
	{
	delete this;
	}

TInt CDbTableCursor::HReadBuf::DoReadL(TAny* aPtr,TInt aMaxLength)
	{
	__ASSERT(iHost);
	return iHost->ReadL(aPtr,aMaxLength);
	}

TStreamTransfer CDbTableCursor::HReadBuf::DoReadL(MStreamInput& aInput,TStreamTransfer aTransfer)
	{
	__ASSERT(iHost);
	return iHost->ReadL(aInput,aTransfer);
	}

TStreamPos CDbTableCursor::HReadBuf::DoSeekL(TMark aMark,TStreamLocation aLocation,TInt aOffset)
	{
	__ASSERT(iHost);
	return iHost->SeekL(aMark,aLocation,aOffset);
	}
