// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <assert.h>
#include <pcstore/storewritestream.h>
#include <pcstore/storereadstream.h>
#include <pcstore/descriptors.h>
#include <pcstore/storeexception.h>
#include "pcstoreconst.h"
#include "desheader.h"

namespace PCStore
{
/**
Constructs the object with the specified value.

@param aCount The value for this object. 
*/
TCardinality::TCardinality(TInt aCount)
	:iCount(aCount)
	{}

/** 
Externalizes this object to a write stream.

@param aStream The write stream to which the object should be externalized.
@exception TStoreException::EFileWriteError Error occurs when writing to the file.
*/
void TCardinality::Externalize(CStoreWriteStream& aStream) const
	{

	TUint n=iCount;
	if (n<=(KMaxTUint8>>KShiftCardinality8))
		{
		aStream.WriteUint8((n<<KShiftCardinality8));
		}
	else 
		{
		if (n<=(KMaxTUint16>>KShiftCardinality16))
			{
			aStream.WriteUint16((n<<KShiftCardinality16)+0x1);
			}
		else
			{
			assert(n <= (TUint)KMaxCardinality);
			aStream.WriteUint32((n<<KShiftCardinality32)+0x3);
			}
		}
	}

/** 
Internalizes this object from a read stream.

@param aStream The read stream from which the object is to be internalized.
@exception TStoreException::EFileReadError Error occurs when reading from the file.
@exception TStoreException::EStoreCorrupt Store file is corrupted so that an invalid cardinal 
number is read.
*/
void TCardinality::Internalize(CStoreReadStream& aStream)
	{
	TUint n=aStream.ReadUint8();
	if ((n&0x1)==0)
		{
		n>>=KShiftCardinality8;
		}
	else
		{
		if ((n&0x2)==0)
			{
			n+=aStream.ReadUint8()<<8;
			n>>=KShiftCardinality16;
			}
		else
			{
			if ((n&0x4)==0)
				{
				aStream.Read(reinterpret_cast<TUint8*>(&iCount),sizeof(TUint32)-sizeof(TUint8));
				n+=TUint(iCount)<<8; 
				n>>=KShiftCardinality32;
				}
			else
				{
				throw TStoreException(TStoreException::EStoreCorrupt);
				}
			}
		}
	assert(n <= static_cast<TUint>(KMaxCardinality));
	iCount=n;
	}

/** 
Converts this object to TInt,

@return The value for this object
*/
TCardinality::operator TInt() const
	{
	return iCount;
	}

/**
Constructs the header for the specified 8-bit descriptor.

@param aDes8 The reference to the 8-bit descriptor for which to form the header.
*/
CDesHeader::CDesHeader(const CDes8& aDes8)
	:iVal((aDes8.Length()<<1)+1) 
	{}

/**
Constructs the header for the specified 16-bit descriptor.

@param aDes16 The reference to the 16-bit descriptor for which to form the header.
*/
CDesHeader::CDesHeader(const CDes16& aDes16)
	:iVal((aDes16.Length()<<1))
	{}

/**
Externalize the object to the specified write stream.

@param aStream The write stream to which the object should be externalized.
@exception TStoreException::EFileWriteError Error occurs when writing to the file.
*/
void CDesHeader::Externalize(CStoreWriteStream& aStream) const
	{
	iVal.Externalize(aStream);
	}

/** 
Internalizes the object from a read stream.

@param aStream The read stream from which the object is to be internalized.
@exception TStoreException::EStoreCorrupt Store file is corrupted so that an invalid cardinal 
number is read.
@exception TStoreException::EFileReadError Error occurs when reading from the file.
*/
void CDesHeader::Internalize(CStoreReadStream& aStream)
	{
	iVal.Internalize(aStream);
	}

/** 
Gets the length of the descriptor which this header represents.

@return The length of the descriptor which this header represents.
*/
TInt CDesHeader::Length() const
	{
	return TInt(iVal)>>1;
	}
}
