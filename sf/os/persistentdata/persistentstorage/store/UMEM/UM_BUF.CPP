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

#include "UM_STD.H"

EXPORT_C TMemBuf::TMemBuf()
	: iBase(NULL)
/** Constructs an empty object.

Call Set() before using the object. */
	{}

EXPORT_C void TMemBuf::Set(TUint8* aPtr,TUint8* anEnd,TInt aMode)
/** Sets up the stream to use the specified area of plain memory.

@param aPtr The start address for the area of plain memory that hosts the 
stream and that also acts as the intermediate buffer.
@param anEnd The end address for the area of plain memory that hosts the stream 
and that also acts as the intermediate buffer. The addressed byte is outside 
the memory area.
@param aMode The mode in which the stream is to be used. It can be used in 
either or both read and write modes, represented by ERead and EWrite.
@see MStreamBuf::TRead
@see MStreamBuf::TWrite */
	{
	if (aPtr==NULL && anEnd==NULL)
		aPtr=anEnd=(TUint8*)this;	// treat null data as seekable zero-length data
	iBase=aPtr;
	SetBuf(ERead,(aMode&ERead)?aPtr:NULL,anEnd);
	SetBuf(EWrite,(aMode&EWrite)?aPtr:NULL,anEnd);
	}

EXPORT_C TInt TMemBuf::UnderflowL(TInt)
//
// The read buffer is empty.
//
	{
	return 0;
	}

EXPORT_C void TMemBuf::OverflowL()
//
// Ran out of write buffer.
//
	{
	__LEAVE(KErrOverflow);
	}

EXPORT_C TStreamPos TMemBuf::DoSeekL(TMark aMark,TStreamLocation aLocation,TInt anOffset)
//
// Position the mark(s) indicated by aMark at anOffset from aLocation.
//
	{
	TUint8* ptr=0;
	switch (aLocation)
		{
	case EStreamBeginning:
		ptr=Base()+anOffset;
		break;
	case EStreamMark:
		ptr=Ptr(aMark)+anOffset;
		break;
	case EStreamEnd:
		ptr=End()+anOffset;
		break;
	default:
		Panic(EMemLocationInvalid);
		break;
		}
	TInt r=KErrNone;
	if (ptr>End())
		{
		ptr=End();
		r=KErrEof;
		}
	else if (ptr<Base())
		{
		ptr=Base();
		r=KErrEof;
		}
//
	SetPtr(aMark,ptr);
	__LEAVE_IF_ERROR(r);
	return TStreamPos(ptr-Base());
	}

EXPORT_C TDesBuf::TDesBuf()
	: iDes(NULL)
/** Constructs an empty object.

Call Set() before using the object. */
	{}

EXPORT_C void TDesBuf::Set(TDes8& aDes,TInt aMode)
/** Sets up the stream to use the specified descriptor.

@param aDes The descriptor that hosts the stream and that also acts as the 
intermediate buffer.
@param aMode The mode in which the buffer is to be used. It can be used in 
either or both read and write modes, represented by ERead and EWrite.
@see TDes8
@see MStreamBuf::TRead
@see MStreamBuf::TWrite */
	{
	iDes=&aDes;
	TUint8* ptr=(TUint8*)aDes.Ptr();
	SetBuf(ERead,(aMode&ERead)?ptr:NULL,ptr+aDes.Length());
	SetBuf(EWrite,(aMode&EWrite)?ptr:NULL,ptr+aDes.MaxLength());
	}

EXPORT_C TInt TDesBuf::UnderflowL(TInt)
//
// Check if there's any more to be read.
//
	{
	Consolidate();
	return Avail(ERead);
	}

EXPORT_C void TDesBuf::OverflowL()
//
// Ran out of write buffer.
//
	{
	__LEAVE(KErrOverflow);
	}

EXPORT_C void TDesBuf::DoSynchL()
//
// Synchronise descriptor and stream buffer.
//
	{
	Consolidate();
	}

EXPORT_C TStreamPos TDesBuf::DoSeekL(TMark aMark,TStreamLocation aLocation,TInt anOffset)
//
// Position the mark(s) indicated by aMark at anOffset from aLocation.
//
	{
	Consolidate();
	TUint8* ptr=0;
	switch (aLocation)
		{
	case EStreamBeginning:
		ptr=Base()+anOffset;
		break;
	case EStreamMark:
		ptr=Ptr(aMark)+anOffset;
		break;
	case EStreamEnd:
		ptr=End(ERead)+anOffset;
		break;
	default:
		Panic(EMemLocationInvalid);
		break;
		}
	TInt r=KErrNone;
	if (ptr>End(ERead))
		{
		ptr=End(ERead);
		r=KErrEof;
		}
	else if (ptr<Base())
		{
		ptr=Base();
		r=KErrEof;
		}
//
	SetPtr(aMark,ptr);
	__LEAVE_IF_ERROR(r);
	return TStreamPos(ptr-Base());
	}

void TDesBuf::Consolidate()
//
// Update the descriptor's length as well as the read limit.
//
	{
	TUint8* ptr=Ptr(EWrite);
	if (ptr==NULL)
		return;			// ERead only desbuf
	TUint8* base=Base();
	TInt len=Max(Des().Length(),ptr-base);
	Des().SetLength(len);
	SetEnd(ERead,base+len);
	}

EXPORT_C TBufBuf::TBufBuf()
	: iBuf(NULL)
/** Constructs an empty object.

Call Set() before using the object. */
	{}

EXPORT_C void TBufBuf::Set(CBufBase& aBuf,TInt aPos,TInt aMode)
/** Sets up the stream to use the specified dynamic buffer.

@param aBuf The dynamic buffer that hosts the stream and that also acts as 
the intermediate buffer.
@param aPos The offset within the dynamic buffer where the stream starts.
@param aMode The mode in which the stream is to be used. It can be used in 
either or both read and write modes, represented by ERead and EWrite. In addition, 
specify TBufBuf::EInsert to imply insert mode; specify TBufBuf::ETruncate 
to imply truncate mode. If neither TBufBuf::EInsert nor TBufBuf::ETruncate 
are specified, then overwrite mode is implied. Both TBufBuf::EInsert and TBufBuf::ETruncate 
imply EWrite.
@see CBufBase
@see MStreamBuf::TRead
@see MStreamBuf::TWrite */
	{
	iBuf=&aBuf;
	SetPos(ERead|EWrite,aPos);
	if (aMode&(ETruncate|EInsert))
		aMode|=EWrite; // truncate and insert imply write
	iMode=aMode;
	//Initialize base class data members.
	//The first Read/Write call will reinitialize them with non NULL values.
	TStreamBuf::SetBuf(iMode & (EWrite | ERead), NULL, NULL);
	}

EXPORT_C TInt TBufBuf::UnderflowL(TInt)
//
// Establish the buffer's read area.
//
	{
	__ASSERT_ALWAYS(iMode&ERead,Panic(EMemCannotRead));
	__ASSERT_DEBUG(Ptr(ERead)==End(ERead),User::Invariant());
	TInt pos=Pos(ERead);
	TPtr8 seg=Buf().Ptr(pos);
	TUint8* ptr=(TUint8*)seg.Ptr();
	TInt len=seg.Length();
	if (iMode&ETruncate)
		{
		TInt left=Pos(EWrite)-pos;
		__ASSERT_DEBUG(left>=0,User::Invariant());
		if (left<len)
			len=left;
		}
	SetBuf(ERead,ptr,ptr+len);
	SetPos(ERead,pos+len);
	return len;
	}

EXPORT_C void TBufBuf::OverflowL()
//
// Establish the buffer's write area.
//
	{
	__ASSERT_ALWAYS(iMode&EWrite,Panic(EMemCannotWrite));
	__ASSERT_DEBUG(Ptr(EWrite)==End(EWrite)&&Pos(EWrite)<Buf().Size(),User::Invariant());
	TInt pos=Pos(EWrite);
	TPtr8 seg=Buf().Ptr(pos);
	TUint8* ptr=(TUint8*)seg.Ptr();
	TInt len=seg.Length();
	SetBuf(EWrite,ptr,ptr+len);
	SetPos(EWrite,pos+len);
	}

EXPORT_C void TBufBuf::DoSynchL()
//
// Synchronise buffer and stream buffer and compress.
//
	{
	Consolidate();
	Buf().Compress();
	}

EXPORT_C void TBufBuf::DoWriteL(const TAny* aPtr,TInt aLength)
//
// Consume aLength bytes, taking care of any reallocation.
//
	{
	__ASSERT_ALWAYS(iMode&EWrite,Panic(EMemCannotWrite));
	__ASSERT_DEBUG(aLength>=0,Panic(EMemWriteLengthNegative));
	__ASSERT_DEBUG(aLength>0,Panic(EMemWriteNoTransfer));
	TInt len=(iMode&EInsert?0:Min(aLength,Buf().Size()-Mark(EWrite)));
	if (len>0)
		{
		TStreamBuf::DoWriteL(aPtr,len);
		aPtr=(TUint8*)aPtr+len;
		aLength-=len;
		}
	if (aLength>0)
		{
		Consolidate();
		TInt pos=Pos(EWrite);
		Buf().InsertL(pos,aPtr,aLength);
		if (Pos(ERead)>pos)
			MovePos(ERead,aLength);
		SetPos(EWrite,pos+aLength);
		}
	}

EXPORT_C TStreamPos TBufBuf::DoSeekL(TMark aMark,TStreamLocation aLocation,TInt anOffset)
//
// Position the mark(s) indicated by aMark at anOffset from aLocation.
//
	{
	Consolidate();
	TInt size=Buf().Size();
	switch (aLocation)
		{
	case EStreamBeginning:
		break;
	case EStreamMark:
		anOffset+=Pos(aMark);
		break;
	case EStreamEnd:
		anOffset+=size;
		break;
	default:
		Panic(EMemLocationInvalid);
		break;
		}
	TInt r=KErrNone;
	if (anOffset>size)
		{
		anOffset=size;
		r=KErrEof;
		}
	else if (anOffset<0)
		{
		anOffset=0;
		r=KErrEof;
		}
//
	SetPos(aMark,anOffset);
	__LEAVE_IF_ERROR(r);
	return TStreamPos(anOffset);
	}

void TBufBuf::Consolidate()
//
// Empty buffer areas and, in truncate mode, cut off at the current write position.
//
	{
	MovePos(ERead,-Avail(ERead));
	MovePos(EWrite,-Avail(EWrite));
	SetBuf(ERead|EWrite,NULL,NULL);
	if (iMode&ETruncate)
		{
		Buf().Delete(Pos(EWrite),Buf().Size()-Pos(EWrite));
		iMode&=~ETruncate;
		}
	}

void TBufBuf::SetPos(TMark aMark,TInt aPos)
//
// Set the buffer position for the mark(s) indicated by aMark
//
	{
	__ASSERT_ALWAYS(!(aMark&~(ERead|EWrite)),Panic(EMemMarkInvalid));
	if (aMark&ERead)
		SetPos(ERead,aPos);
	if (aMark&EWrite)
		SetPos(EWrite,aPos);
	}

TInt TBufBuf::Pos(TMark aMark) const
//
// Return the buffer position for the mark indicated by aMark.
//
	{
	if (aMark==ERead)
		return Pos(ERead);
//
	__ASSERT_ALWAYS(aMark==EWrite,Panic(EMemMarkInvalid));
	return Pos(EWrite);
	}

