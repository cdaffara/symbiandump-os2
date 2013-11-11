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

#include <s32ucmp.h>
#include "US_STD.H"

EXPORT_C void ExternalizeL(TInt64 anInt64,RWriteStream& aStream)
//
// Write a 64-bit integer out to aStream.
//
	{
	aStream.WriteUint32L(I64LOW(anInt64));
	aStream.WriteUint32L(I64HIGH(anInt64));
	}

EXPORT_C void InternalizeL(TInt64& anInt64,RReadStream& aStream)
//
// Read a 64-bit integer in from aStream.
//
	{
	TUint low=aStream.ReadUint32L();
	TUint high=aStream.ReadUint32L();
	anInt64 = MAKE_TINT64(high,low);
	}

EXPORT_C void ExternalizeL(const TCheckedUid& aUid,RWriteStream& aStream)
//
// Write a checked uid type out to aStream.
//
	{
	aStream.WriteL(aUid.Des());
	}

EXPORT_C void InternalizeL(TCheckedUid& aUid,RReadStream& aStream)
//
// Read a checked uid type in from aStream.
//
	{
	TBuf8<sizeof(TCheckedUid)> buf;
	aStream.ReadL(buf);
	aUid.Set(buf);
	}

EXPORT_C void ExternalizeL(TPoint aPoint,RWriteStream& aStream)
//
// Write a point out to aStream.
//
	{
	aStream.WriteInt32L(aPoint.iX);
	aStream.WriteInt32L(aPoint.iY);
	}

EXPORT_C void ExternalizeL(TSize aSize,RWriteStream& aStream)
//
// Write a size out to aStream.
//
	{
	aStream.WriteInt32L(aSize.iWidth);
	aStream.WriteInt32L(aSize.iHeight);
	}

EXPORT_C void ExternalizeL(const TRect& aRect,RWriteStream& aStream)
//
// Write a rectangle out to aStream.
//
	{
	aStream<<aRect.iTl;
	aStream<<aRect.iBr;
	}

EXPORT_C void InternalizeL(TPoint& aPoint,RReadStream& aStream)
//
// Read a point in from aStream.
//
	{
	aPoint.iX=aStream.ReadInt32L();
	aPoint.iY=aStream.ReadInt32L();
	}

EXPORT_C void InternalizeL(TSize& aSize,RReadStream& aStream)
//
// Read a size in from aStream.
//
	{
	aSize.iWidth=aStream.ReadInt32L();
	aSize.iHeight=aStream.ReadInt32L();
	}

EXPORT_C void InternalizeL(TRect& aRect,RReadStream& aStream)
//
// Read a rectangle in from aStream.
//
	{
	aStream>>aRect.iTl;
	aStream>>aRect.iBr;
	}

EXPORT_C void ExternalizeL(const TDesC8& aDes8,RWriteStream& aStream)
//
// Write an 8-bit descriptor out to aStream.
//
	{
	TDesHeader header(aDes8);
	aStream<<header;
	aStream.WriteL(aDes8.Ptr(),aDes8.Length());
	}

EXPORT_C void ExternalizeL(const TDesC16& aDes16,RWriteStream& aStream)
//
// Write a 16-bit descriptor out to aStream.
//
	{
	TDesHeader header(aDes16);
	aStream << header;

#ifdef _UNICODE
	// In the Unicode build, compress the data using the Standard Unicode Compression Scheme.
	TMemoryUnicodeSource source(aDes16.Ptr());
	TUnicodeCompressor compressor;
	compressor.CompressL(aStream,source,KMaxTInt,aDes16.Length());
	
#else
	aStream.WriteL(aDes16.Ptr(),aDes16.Length());
#endif
	}

EXPORT_C void InternalizeL(TDes8& aDes8,RReadStream& aStream)
//
// Read an 8-bit descriptor in from aStream.
//
	{
	TDesInternalizer interL;
	aStream>>interL.Header();
	interL(aDes8,aStream);
	}

EXPORT_C void InternalizeL(TDes16& aDes16,RReadStream& aStream)
//
// Read a 16-bit descriptor in from aStream.
//
	{
	TDesInternalizer interL;
	aStream>>interL.Header();
	interL(aDes16,aStream);
	}

EXPORT_C HBufC8* HBufC8::NewL(RReadStream& aStream,TInt aMaxLength)
/**
Creates, and returns a pointer to, a new 8-bit heap descriptor that has been
initialised with data from the specified read stream; leaves on failure.

Data is assigned to the new descriptor from the specified stream. This variant
assumes that the stream contains the length of the data followed by the data
itself.

The requested maximum length of the descriptor buffer is the length value taken
from the stream. Note that the size of the allocated cell, and the resulting
maximum length of the descriptor, may be larger than requested due to the way
memory is allocated in Symbian OS. This rounding up effect is also dependent
on platform and build type.

Note that:
1. To use this variant, project files must also link against estor.lib
2. The length of the data in the stream is represented by a TCardinality object.

@param aStream     The stream from which the data length and the data to be
                   assigned to the new descriptor, are taken.
@param aMaxLength  The upper limit on the length of data that the descriptor is
                   to represent.

@return A pointer to the new 8-bit heap descriptor. The function leaves, 
        if the new 8-bit heap descriptor cannot be created.

@leave KErrOverflow if the length of the data as read from the stream is
                    greater than the upper limit as specified by aMaxLength.

@panic USER 30, if aMaxLength is negative.

@see TCardinality
*/
	{
	HBufC8* buf8=NewLC(aStream,aMaxLength);
	CleanupStack::Pop();
	return buf8;
	}

EXPORT_C HBufC8* HBufC8::NewLC(RReadStream& aStream,TInt aMaxLength)
/**
Creates, adds a pointer onto the cleanup stack, and returns a pointer to,
a new 8-bit heap descriptor that has been initialised with data from the
specified read stream; leaves on failure.

Data is assigned to the new descriptor from the specified stream. This variant
assumes that the stream contains the length of the data followed by the data
itself.

The requested maximum length of the descriptor buffer is the length value taken
from the stream. Note that the size of the allocated cell, and the resulting
maximum length of the descriptor, may be larger than requested due to the way
memory is allocated in Symbian OS. This rounding up effect is also dependent
on platform and build type.

Note that:
1. To use this variant, project files must also link against estor.lib
2. The length of the data in the stream is represented by a TCardinality object.

@param aStream     The stream from which the data length and the data to be
                   assigned to the new descriptor, are taken.
@param aMaxLength  The upper limit on the length of data that the descriptor is
                   to represent.

@return A pointer to the new 8-bit heap descriptor. The function leaves, 
        if the new 8-bit heap descriptor cannot be created.

@leave KErrOverflow if the length of the data as read from the stream is
                    greater than the upper limit as specified by aMaxLength.

@panic USER 30, if aMaxLength is negative.

@see TCardinality
*/
	{
	TDesInternalizer interL;
	aStream>>interL.Header();
	TInt len=interL.Header().Length();
	if (len>aMaxLength)
		__LEAVE(KErrOverflow);
//
	HBufC8* buf8=NewLC(len);
	TPtr8 ptr8=buf8->Des();
	interL(ptr8,aStream);
	return buf8;
	}

EXPORT_C HBufC16* HBufC16::NewL(RReadStream& aStream,TInt aMaxLength)
/**
Creates, and returns a pointer to, a new 16-bit heap descriptor that has been
initialised with data from the specified read stream; leaves on failure.

Data is assigned to the new descriptor from the specified stream. This variant
assumes that the stream contains the length of the data followed by the data
itself.

The requested maximum length of the descriptor buffer is the length value taken
from the stream. Note that the size of the allocated cell, and the resulting
maximum length of the descriptor, may be larger than requested due to the way
memory is allocated in Symbian OS. This rounding up effect is also dependent
on platform and build type.

Note that:
1. To use this variant, project files must also link against estor.lib
2. The length of the data in the stream is represented by a TCardinality object.

@param aStream     The stream from which the data length and the data to be
                   assigned to the new descriptor, are taken.
@param aMaxLength  The upper limit on the length of data that the descriptor is
                   to represent.

@return A pointer to the new 16-bit heap descriptor. The function leaves, 
        if the new 16-bit heap descriptor cannot be created.

@leave KErrOverflow if the length of the data as read from the stream is
                    greater than the upper limit as specified by aMaxLength.

@panic USER 18, if aMaxLength is negative.

@see TCardinality
*/
	{
	HBufC16* buf16=NewLC(aStream,aMaxLength);
	CleanupStack::Pop();
	return buf16;
	}

EXPORT_C HBufC16* HBufC16::NewLC(RReadStream& aStream,TInt aMaxLength)
/**
Creates, adds a pointer onto the cleanup stack, and returns a pointer to,
a new 16-bit heap descriptor that has been initialised with data from the
specified read stream; leaves on failure.

Data is assigned to the new descriptor from the specified stream. This variant
assumes that the stream contains the length of the data followed by the data
itself.

The requested maximum length of the descriptor buffer is the length value taken
from the stream. Note that the size of the allocated cell, and the resulting
maximum length of the descriptor, may be larger than requested due to the way
memory is allocated in Symbian OS. This rounding up effect is also dependent
on platform and build type.

Note that:
1. To use this variant, project files must also link against estor.lib
2. The length of the data in the stream is represented by a TCardinality object.

@param aStream     The stream from which the data length and the data to be
                   assigned to the new descriptor, are taken.
@param aMaxLength  The upper limit on the length of data that the descriptor is
                   to represent.

@return A pointer to the new 16-bit heap descriptor. The function leaves, 
        if the new 16-bit heap descriptor cannot be created.

@leave KErrOverflow if the length of the data as read from the stream is
                    greater than the upper limit as specified by aMaxLength.

@panic USER 30, if aMaxLength is negative.

@see TCardinality
*/
	{
	TDesInternalizer interL;
	aStream>>interL.Header();
	TInt len=interL.Header().Length();
	if (len>aMaxLength)
		__LEAVE(KErrOverflow);
//
	HBufC16* buf16=NewLC(len);
	TPtr16 ptr16=buf16->Des();
	interL(ptr16,aStream);
	return buf16;
	}

EXPORT_C void ExternalizeL(const CBufBase& aBuf,RWriteStream& aStream)
//
// Write a buffer out to aStream.
//
	{
	TInt size=aBuf.Size();
	aStream<<TCardinality(size);
	TInt pos=0;
	while (size>0)
		{
		TPtr8 seg=((CBufBase&)aBuf).Ptr(pos);
		TInt len=seg.Size();
		aStream.WriteL(seg.Ptr(),len);
		size-=len;
		pos+=len;
		}
	}

EXPORT_C void InternalizeL(CBufBase& aBuf,RReadStream& aStream)
//
// Read a buffer in from aStream.
//
	{
	TCardinality card;
	aStream>>card;
//
	aBuf.ResizeL(TInt(card));
	TInt pos=0;
	for (;;)
		{
		TPtr8 seg=aBuf.Ptr(pos);
		TInt len=seg.Size();
		if (len==0)
			return;
//
		aStream.ReadL((TUint8*)seg.Ptr(),len);
		pos+=len;
		}
	}

EXPORT_C void ArrayExternalizeCountL(TInt aCount,RWriteStream& aStream)
//
// Write an array's count out to aStream.
//
	{
	aStream<<TCardinality(aCount);
	}

EXPORT_C void DoExternalizeAllL(const CArrayFixBase& anArray,RWriteStream& aStream,TExternalizer<TAny> anExter)
//
// Write an array's contents out to aStream.
//
	{
	for (TInt i=0,n=anArray.Count();i<n;i++)
		anExter(anArray.At(i),aStream);
	}

EXPORT_C TInt ArrayInternalizeCountL(RReadStream& aStream)
//
// Read an array's count in from aStream.
//
	{
	TCardinality card;
	aStream>>card;
//
	return TInt(card);
	}

EXPORT_C void DoInternalizeAllL(CArrayFixBase& anArray,RReadStream& aStream,TInternalizer<TAny> anInter)
//
// Read an array's contents in from aStream.
//
	{
	for (TInt i=0,n=anArray.Count();i<n;i++)
		anInter(anArray.At(i),aStream);
	}

