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
#include <memory.h>
#include <pcstore/checkeduid.h>
#include <pcstore/descriptors.h>
#include <pcstore/storeexception.h>
#include "pcstoreconst.h"

namespace PCStore
{
const TUint crcTab[256] =
	{
	0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,0x8108,0x9129,0xa14a,
	0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,
	0x72f7,0x62d6,0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,0x2462,
	0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,0xa56a,0xb54b,0x8528,0x9509,
	0xe5ee,0xf5cf,0xc5ac,0xd58d,0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,
	0x46b4,0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,0x48c4,0x58e5,
	0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,
	0x9969,0xa90a,0xb92b,0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
	0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,0x6ca6,0x7c87,0x4ce4,
	0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,
	0x8d68,0x9d49,0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,0xff9f,
	0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,0x9188,0x81a9,0xb1ca,0xa1eb,
	0xd10c,0xc12d,0xf14e,0xe16f,0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,
	0x6067,0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,0x02b1,0x1290,
	0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,
	0xe54f,0xd52c,0xc50d,0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
	0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,0x26d3,0x36f2,0x0691,
	0x16b0,0x6657,0x7676,0x4615,0x5634,0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,
	0xb98a,0xa9ab,0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,0xcb7d,
	0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,0x4a75,0x5a54,0x6a37,0x7a16,
	0x0af1,0x1ad0,0x2ab3,0x3a92,0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,
	0x8dc9,0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,0xef1f,0xff3e,
	0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,
	0x3eb2,0x0ed1,0x1ef0
	};

/**
Performs a CCITT CRC checksum on the specified data.

On return from this function, the referenced 16 bit integer contains the checksum.

@param aCrc    A reference to a 16 bit integer to contain the checksum.
@param aPtr    A pointer to the start of the data to be checksum.
@param aLength The length of the data to be checksum.
*/
void Crc(TUint16& aCrc,const void* aPtr,TInt aLength)
	{
	const TUint8* pB=static_cast<const TUint8*>(aPtr);
	const TUint8* pE=pB+aLength;
	TUint crc=aCrc;
	while (pB<pE)
		{
		crc=(crc<<8)^crcTab[((crc>>8)^*pB++)&0xff];
		}
	aCrc=(TUint16)crc;
	}


/**
Generate a checksum for the object of TCheckedUid.

@param aPtr The pointer to the TCheckedUid.
@return The resulted checksum for the TCheckedUid.
*/
TUint checkSum(const void *aPtr)
	{
	const TUint8* pB=static_cast<const TUint8*>(aPtr);
	const TUint8* pE=pB+(KMaxCheckedUid*sizeof(TUid));
	TUint8 buf[(KMaxCheckedUid*sizeof(TUid))>>1];
	TUint8* pT=(&buf[0]);
	while (pB<pE)
		{
		*pT++=(*pB);
		pB+=2;
		}
	TUint16 crc=0;
	Crc(crc,&buf[0],(KMaxCheckedUid*sizeof(TUid))>>1);
	return(crc);
	}

/**
Constructor that creates a zero-valued UID type.
*/
TUidType::TUidType()
	{
	memset(this,0,sizeof(TUidType));
	}


/**
Constructor that creates a UID type and sets the UID1 component
to the specified value.

The UID2 and UID3 components are set to KNullUid.

@param aUid1 Value for UID1.
*/
TUidType::TUidType(TUid aUid1)
	{
	memset(this,0,sizeof(TUidType));
	iUid[0]=aUid1;
	}


/**
Constructor that creates a UID type and sets the UID1 and UID2 components
to the specified values. 

The UID3 component is set to KNullUid.

@param aUid1 Value for UID1. 
@param aUid2 Value for UID2.
*/
TUidType::TUidType(TUid aUid1, TUid aUid2)
	{
	iUid[0]=aUid1;
	iUid[1]=aUid2;
	iUid[2]=KNullUid;
	}

/**
Constructor that creates a UID type and sets all three UID components
to the specified values.

@param aUid1 Value for UID1.
@param aUid2 Value for UID2.
@param aUid3 Value for UID3.
*/
TUidType::TUidType(TUid aUid1, TUid aUid2, TUid aUid3)
	{
	iUid[0]=aUid1;
	iUid[1]=aUid2;
	iUid[2]=aUid3;
	}


/** 
Compares this UID type for inequality with the specified UID type.

@param aUidType The UID type to be compared.
@return TRUE, if any component UID is not equal to the corresponding component 
UID in the specified UID type; FALSE otherwise.
*/
TBool TUidType::operator !=(const TUidType& aUidType) const
	{
	return(!(*this==aUidType));
	}

/**
Compares this UID type for equality with the specified UID type.

@param aUidType The reference to the UID type to be compared. 
@return TRUE, if each component UID is equal to the corresponding component 
UID in the specified UID type; FALSE, otherwise.
*/
TBool TUidType::operator ==(const TUidType& aUidType) const 
	{
	return(iUid[0]==aUidType.iUid[0] &&
		iUid[1]==aUidType.iUid[1] &&
		iUid[2]==aUidType.iUid[2]);
	}


/**
Gets the most derived UID.

Taking the three UID components as a hierarchy with UID1 being the most general, 
UID2 being more specific than UID1 and UID3 being more specific than UID2, 
then the function returns:

UID3, if UID3 is not KNullUid.
UID2, if UID2 is not KNullUid.
UID1, otherwise

@return The most derived UID.
*/
TUid TUidType::MostDerived() const
	{
	if (iUid[2]!=KNullUid) 
		{
		return(iUid[2]);
		}
	if (iUid[1]!=KNullUid) 
		{
		return(iUid[1]);
		}
	return(iUid[0]);
	}

/**
Gets the UID component as identified by the specified index.

@param aIndex Index value indicating which UID component to return.
	0 specifies UID1,
	1 specifies UID2,
	2 specifies UID3.

aIndex must be between 0 and 2.

@return The reference to the required UID component.
*/
const TUid& TUidType::operator[](TInt aIndex) const
	{
	assert(aIndex >= 0 && aIndex <= 2);
	return(iUid[aIndex]);
	}

/**
Tests the object for a valid (non-KNullUid) UID type.

@return TRUE, if at least one of the component UIDs is not KNullUid; FALSE, 
if all component UIDs are KNullUid.
*/
TBool TUidType::IsValid() const 
	{
	 return(MostDerived()!=KNullUid);
	}


/**
Default constructor.

Initialises the object to binary zeroes.
*/
TCheckedUid::TCheckedUid()
	{
	memset(this,0, sizeof(TCheckedUid));
	} 


/**
Constructor taking an existing Uid type.

The constructor calculates a checksum.

@param aUidType The reference to a Uid type to be packaged.
*/
TCheckedUid::TCheckedUid(const TUidType& aUidType) 
	{
	Set(aUidType);
	}


/**
Constructor taking an existing TCheckedUid object encapsulated within
a descriptor.

The checksum is recalculated and must match the checksum value passed in the 
encapsulated TCheckedUid object, otherwise the content of this object is reset 
to binary zeroes.

aDes should be an 8-bit descriptor which holds the contents of a TCheckedUid object.

@param aDes The reference to an 8-bit descriptor containing an existing TCheckedUid 
object. 
*/
TCheckedUid::TCheckedUid(const CDes8& aDes)
	{
	Set(aDes);
	}

/**
Sets the specified Uid type to be packaged, and recalculates the checksum.

@param aUidType The reference to a Uid type to be set.
*/
void TCheckedUid::Set(const TUidType& aUidType)
	{
	iType = aUidType;
	iCheck = Check();
	}

/**
Sets an existing TCheckedUid object encapsulated within a descriptor.

The checksum is recalculated and must match the checksum value passed in the 
encapsulated TCheckedUid object, otherwise the content of this object is reset 
to binary zeroes.

aDes should be an 8-bit descriptor which holds the contents of a TCheckedUid object.

@param aDes The reference to a descriptor containing an existing TCheckedUid object.
*/
void TCheckedUid::Set(const CDes8& aDes)
	{
	assert(aDes.Length() == sizeof(TCheckedUid));
	memcpy(this,aDes.Ptr(),sizeof(TCheckedUid));
	if (iCheck!=Check())
		{
		memset(this,0,sizeof(TCheckedUid));
		}
	}

/**
Gets a descriptor to represent this object's data.

@return The descriptor for this object's data. The descriptor's length
is the same as the length of a TCheckedUid object.
*/
CDes8 TCheckedUid::Des() const
	{
	return(CDes8 (reinterpret_cast<const TUint8*>(this),sizeof(TCheckedUid)));
	}

/**
Gets the Uid type contained in this object.

@return The Uid type.
*/
const TUidType&  TCheckedUid::UidType() const
	{
	return(iType);
	}

/**
Calculates the checksum of the UIDs.

@return The checksum.
*/
TUint TCheckedUid::Check() const
	{
	return((checkSum((reinterpret_cast<TUint8*>(const_cast<TCheckedUid*>(this)))+1)<<16)|checkSum(this));
	}
}
