// Copyright (c) 1998-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#pragma BullseyeCoverage off

//
// Panic the process with STORE-Stream as the category.
//
GLDEF_C void Panic(TStreamPanic aPanic)
	{
	_LIT(KCategory,"STORE-Stream");
	User::Panic(KCategory,aPanic);
	}

#pragma BullseyeCoverage on

EXPORT_C void TCardinality::ExternalizeL(RWriteStream& aStream) const
/** Externalises this object to a write stream.

The existence of this function means that the standard templated operator<<() 
can be used to externalise objects of this class.

@param aStream Stream to which the object should be externalised.
@see operator<<() */
	{
	TUint n=iCount;
	if (n<=(KMaxTUint8>>KShiftCardinality8))
		aStream.WriteUint8L((n<<KShiftCardinality8));
	else if (n<=(KMaxTUint16>>KShiftCardinality16))
		aStream.WriteUint16L((n<<KShiftCardinality16)+0x1);
	else
		{
		__ASSERT_DEBUG(n<=(TUint)KMaxCardinality,Panic(EStreamCardinalityOutOfRange));
		aStream.WriteUint32L((n<<KShiftCardinality32)+0x3);
		}
	}

EXPORT_C void TCardinality::InternalizeL(RReadStream& aStream)
/** Internalizes this object from a read stream.

The existence of this function means that the standard templated operator>>() 
can be used to internalise objects of this class.

@param aStream Stream store from which the objet is to be internalised.
@see operator>>() */
	{
	TUint n=aStream.ReadUint8L();
	if ((n&0x1)==0)
		n>>=KShiftCardinality8;
	else if ((n&0x2)==0)
		{
		n+=aStream.ReadUint8L()<<8;
		n>>=KShiftCardinality16;
		}
	else if ((n&0x4)==0)
		{
		aStream.ReadL((TUint8*)&iCount,sizeof(TUint32)-sizeof(TUint8));
		n+=TUint(iCount)<<8; // platform dependency
		n>>=KShiftCardinality32;
		}
	else
		__LEAVE(KErrCorrupt);
//
	__ASSERT_DEBUG(n<=(TUint)KMaxCardinality,User::Invariant());
	iCount=n;
	}

// An MUnicodeSink implementation to write Unicode values into 8-bit bytes; anything outside 0..255 becomes 1
#ifdef _UNICODE
class TNarrowUnicodeSink: public MUnicodeSink
	{
	public:
	TNarrowUnicodeSink(TUint8* aPtr): iPtr(aPtr) { }
	void WriteUnicodeValueL(TUint16 aValue) { *iPtr++ = (TUint8)(aValue > 255 ? 1 : aValue); }

	private:
	TUint8* iPtr;
	};
#endif

void TDesInternalizer::operator()(TDes8& aDes8,RReadStream& aStream) const
//
// Read an 8-bit descriptor in from aStream.
//
	{
	TInt len=iHeader.Length();
	if (len>aDes8.MaxLength())
		__LEAVE(KErrOverflow);
//
	TUint8* ptr=(TUint8*)aDes8.Ptr();
	if (iHeader.IsWidth8())
		aStream.ReadL(ptr,len);
	else
		{
		__ASSERT_DEBUG(iHeader.IsWidth16(),User::Invariant());
#ifdef _UNICODE
		// In the Unicode build 16-bit descriptors need to be expanded from the Standard Unicode Compression Scheme.
		TNarrowUnicodeSink sink(ptr);
		TUnicodeExpander expander;
		expander.ExpandL(sink,aStream,len);

#else
		TUint8* end=ptr+len;
		while (ptr!=end)
			{
			TUint c=aStream.ReadUint16L();
			if (c>=0x100)
				c=1;
			*ptr++=TUint8(c);
			}
#endif
		}
	aDes8.SetLength(len);
	}

void TDesInternalizer::operator()(TDes16& aDes16,RReadStream& aStream) const
//
// Read a 16-bit descriptor in from aStream.
//
	{
	TInt len=iHeader.Length();
	if (len>aDes16.MaxLength())
		__LEAVE(KErrOverflow);
//
	TUint16* ptr=(TUint16*)aDes16.Ptr();
	if (iHeader.IsWidth16())
		{
#ifdef _UNICODE
		// In the Unicode build 16-bit descriptors need to be expanded from the Standard Unicode Compression Scheme.
		TMemoryUnicodeSink sink(ptr);
		TUnicodeExpander expander;
		expander.ExpandL(sink,aStream,len);

#else
		aStream.ReadL(ptr,len);
#endif
		}
	else
		{
		__ASSERT_DEBUG(iHeader.IsWidth8(),User::Invariant());
		TUint16* end=ptr+len;
		while (ptr!=end)
			*ptr++=aStream.ReadUint8L();
		}
	aDes16.SetLength(len);
	}

EXPORT_C void TStreamTransfer::__DbgChkNonNegative(TInt aLength)
//
// Check for a negative value.
//
	{
	__ASSERT_ALWAYS(aLength>=0,Panic(EStreamTransferNegative));
	}

EXPORT_C void TCardinality::__DbgChkRange(TInt aCount)
//
// Check for a negative count.
//
	{
	__ASSERT_ALWAYS(aCount<=KMaxCardinality,Panic(EStreamCardinalityOutOfRange));
	}

