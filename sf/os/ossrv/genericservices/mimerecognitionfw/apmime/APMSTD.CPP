// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <s32strm.h>

#include "APMSTD.H"
#include "APMPAN.H"

//
// class TDataType
//

EXPORT_C TDataType::TDataType()
	:iDataType(),
	iUid(KNullUid)
/** Default constructor.

Sets an empty data type string and sets the associated UID to KNullUid. */
	{}

EXPORT_C TDataType::TDataType(const TDataType& aDataType)
	:iDataType(aDataType.iDataType),
	iUid(aDataType.iUid)
/** Copy constructor.

@param aDataType The data type to be copied. */
	{}

EXPORT_C TDataType::TDataType(const TDesC8& aDataType)
	:iDataType(aDataType),
	iUid(KNullUid)
/** Constructor taking a descriptor.

The data type string is copied from the specified descriptor. By default, 
the associated UID is set to KNullUid.

If the data type string begins with the characters "x-epoc/x-app", identifying 
data as a native Symbian type, then the associated UID is set to the UID value 
which is expected to follow the characters "x-epoc/x-app".

If no sensible UID can be extracted from the characters following: "x-epoc/x-app", 
then the associated UID is set to KNullUid.

@param aDataType The source for the data type string. Note that this is an 
8 bit descriptor. */
	{
	ParseDes();
	}

EXPORT_C TDataType::TDataType(TUid aUid)
	:iUid(aUid)
/** Constructor taking a UID.

The data type string is set to "x-epoc/x-app" concatenated with the decimal 
character representation of the specified UID value. The data type is assumed 
to identify data as a native Symbian type.

@param aUid The associated UID. */
	{
	iDataType=KApaAppTypeDes;
	iDataType.AppendNum(STATIC_CAST(TInt,iUid.iUid));
	}

EXPORT_C TInt TDataType::operator==(const TDataType& aDataType) const
/** Compares this data type for equality with the specified data type.

Two data types are equal if their data type strings are the same. Text is 
folded for the purpose of the comparison, removing accents and converting 
letters to a common case form.

@param aDataType The data type taking part in the comparison.
@return True, if the data types are equal; false, otherwise. */
	{
	return (iDataType.CompareF(aDataType.iDataType)==0);
	}

EXPORT_C TInt TDataType::operator!=(const TDataType& aDataType) const
/** Compares this data type for inequality with the specified data type.

Two data types are only equal if their data type strings are the same. Text 
is folded for the purpose of the comparison, removing accents and converting 
letters to a common case form.

@param aDataType The data type taking part in the comparison.
@return True, if the data types are not equal; false, otherwise. */
	{
	return iDataType.CompareF(aDataType.iDataType);
	}

EXPORT_C TBool TDataType::IsNative() const
/** Tests whether the data type is a native Symbian type.

@return True, if the data type is a native Symbian type; false, otherwise. */
	{
	return iUid!=KNullUid;
	}

EXPORT_C TUid TDataType::Uid() const
/** Gets the UID associated with this data type.

@return The UID. */
	{
	return iUid;
	}

EXPORT_C TBuf<KMaxDataTypeLength> TDataType::Des() const
/** Gets a copy of the data type string.

@return A modifiable buffer descriptor containing a copy of the data type 
string. Note that this descriptor is a build independent type. */
	{
#ifdef _UNICODE
	TBuf16<KMaxDataTypeLength> buf;
	buf.Copy(iDataType);
	return buf;
#else
	return iDataType;
#endif // _UNICODE
	}

EXPORT_C TPtrC8 TDataType::Des8() const
/** Gets an 8 bit non-modifiable pointer descriptor to the data type string.

@return A pointer descriptor to the data type string. */
	{
	return iDataType;
	}

void TDataType::ParseDes()
	{
	TInt desLen=KApaAppTypeDes.Length();
	if (iDataType.Length()<=desLen)
		iUid=KNullUid;
	else
		{
		if (iDataType.Left(desLen).CompareF(KApaAppTypeDes)==0)
			{
			TLex8 lex(iDataType);
			lex.SkipAndMark(desLen);
			TInt err=lex.Val(iUid.iUid);
			if (err!=KErrNone)
				iUid=KNullUid;
			}
		}
	}

EXPORT_C void TDataType::InternalizeL(RReadStream& aReadStream)
/** Internalizes the data type from a stream.

The presence of this function means that the standard templated stream operator>>() 
is available to internalize objects of this class.

@param aReadStream The read stream. */
	{
	aReadStream >> iDataType;
	ParseDes();
	}

EXPORT_C void TDataType::ExternalizeL(RWriteStream& aWriteStream) const
/** Externalizes the data type to a stream.

The presence of this function means that the standard templated stream operator<<() 
is available to externalise objects of this class.

@param aWriteStream The write stream. */
	{
	aWriteStream << iDataType;
	}

//
// class TDataTypeWithPriority
//

/** Default constructor */
EXPORT_C TDataTypeWithPriority::TDataTypeWithPriority()
	{
	}

/** Constructor taking a data type and a priority value.

@param aDataType The data type.
@param aPriority The priority value. */
EXPORT_C TDataTypeWithPriority::TDataTypeWithPriority(const TDataType& aDataType, TDataTypePriority aPriority)
	: iDataType(aDataType),
	iPriority(aPriority)
	{}
	
/** Internalizes the data type from a stream.

The presence of this function means that the standard templated stream operator>>() 
is available to internalize objects of this class.

@param aReadStream The read stream. */
EXPORT_C void TDataTypeWithPriority::InternalizeL(RReadStream& aReadStream)
	{
	aReadStream >> iDataType;
	aReadStream >> iPriority;
	}

/** Externalizes the data type to a stream.

The presence of this function means that the standard templated stream operator<<() 
is available to externalise objects of this class.

@param aWriteStream The write stream. */
EXPORT_C void TDataTypeWithPriority::ExternalizeL(RWriteStream& aWriteStream) const
	{
	aWriteStream << iDataType;
	aWriteStream << iPriority;
	}

GLDEF_C void Panic(TApmPanic aPanic)
//
// Panic the process with APMIME as the category.
//
	{
	_LIT(KApMimePanicCat,"APMIME");
	User::Panic(KApMimePanicCat,aPanic);
	}


