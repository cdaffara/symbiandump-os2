/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include <e32std.h>

#include <s32strm.h>
#include "FLDBLTIN.H"
#include "FLDDEF.H"
#include "FLDNUMS.H"

#include "FLDSTD.H"
_LIT(KFormatString,"%D%M%Y%/0%1%/1%2%/2%3%/3");

#define UNUSED_VAR(a) a = a

//////////////////////////////////////
// The built-in fields
//////////////////////////////////////

 

EXPORT_C CDateTimeField::CDateTimeField()
	: iFormatString(KFormatString)
/** The default C++ constructor initialises the object's date/time format string 
to %D%M%Y%/0%1%/1%2%/2%3%/3. This produces fields in the format "31/05/2000" 
(with no time component, and using the current locale's date separators). */
	{}


 
	
EXPORT_C void CDateTimeField::SetFormat(const TDesC& aFormat)
/** Sets the date/time format string. For information on the format string, see 
the documentation of TTime::FormatL().

@param aFormat The new date/time format string. */
	{iFormatString = aFormat;}


 

EXPORT_C const TDesC& CDateTimeField::FormatString()const
/** Gets the field's date/time format string.

@return The date/time format string. */
	{return iFormatString;}


 

EXPORT_C TInt CDateTimeField::Value(TPtr& aValueText)
/** Gets the current local date/time, then formats it according to the field's format 
string. aValueText is set to the formatted date/time string if the buffer 
is large enough to contain it. If not, the function returns the length which 
is required to contain the formatted date/time string.

@param aValueText Descriptor which on return contains the current date/time, 
formatted according to the format string. 
@return Zero if aValueText is long enough to hold the formatted local date/time string. 
Otherwise, the length of the buffer which is required to hold the string. */
	{
	TBuf<80> buf; // temporary solution!!!
	TTime time;
	time.HomeTime();
	TRAPD(error,time.FormatL(buf,iFormatString));
    UNUSED_VAR(error);
	if (aValueText.MaxLength() < buf.Length())
		return buf.Length();
	else 
		{
		aValueText = buf;
		return 0;
		}
	}


 

EXPORT_C  void CDateTimeField::ExternalizeL(RWriteStream& aStream)const
// stream the formatting
/** Externalises the format string to a write stream. The presence of this function 
means that the standard templated operator<<() (defined in s32strm.h) is available 
to externalise objects of this class.

@param aStream Stream to which the format string should be externalised. */
	{
	TBuf8<64> format; 
	format.Copy(iFormatString);
	TInt len = format.Length();
	aStream.WriteInt32L(len);
	aStream.WriteL(format,len);
	}


 

EXPORT_C void CDateTimeField::InternalizeL(RReadStream& aStream)
// stream the formatting
/** Internalises the format string from a read stream. The presence of this function 
means that the standard templated operator>>() (defined in s32strm.h) is available 
to internalise objects of this class.

@param aStream Stream from which the format string should be internalised. */
	{
	TInt len = aStream.ReadInt32L();
	TBuf8<64> narrowFormat;
	aStream.ReadL(narrowFormat,len);
	iFormatString.Copy(narrowFormat);
	}


 
EXPORT_C TUid CDateTimeField::Type()const
/** Gets the field's type UID.

@return KDateTimeFieldUid. */
	{return KDateTimeFieldUid;}

// CPageFieldBase

 

EXPORT_C void CPageFieldBase::InternalizeL(RReadStream& aStream)
/** Internalises the numeric style value from a read stream. The presence of this 
function means that the standard templated operator>>() (defined in s32strm.h) 
is available to internalise objects of this class.

@param aStream Stream from which the numeric style should be internalised. */
	{
	iStyle = (TNumberStyle)aStream.ReadInt8L();
	}


 

EXPORT_C void CPageFieldBase::ExternalizeL(RWriteStream& aStream)const
/** Externalises the numeric style value to a write stream. The presence of this 
function means that the standard templated operator<<() (defined in s32strm.h) 
is available to externalise objects of this class.

@param aStream Stream to which the numeric style should be externalised. */
	{
	aStream.WriteInt8L(iStyle);
	}


TInt CPageFieldBase::InsertValue(TPtr& aValueText,TInt aValue)
	{
	TInt chars=0;
	switch(iStyle)
		{
		case EArabic:
			{
			TArabicNumeral arabic;
			chars = arabic.DeneryToChar(aValueText,aValue);
			break;
			}
		case ERomanUpper:
			{
			TRomanNumeral romanU;
			chars = romanU.DeneryToChar(aValueText,aValue);
			aValueText.UpperCase();
			break;
			}
		case ERomanLower:
			{
			TRomanNumeral romanL;
			chars = romanL.DeneryToChar(aValueText,aValue);
			aValueText.LowerCase();
			break;
			}
		case EAlphabeticUpper:
			{
			TAlphabeticNumeral alphaU;
			chars = alphaU.DeneryToChar(aValueText,aValue);
			aValueText.UpperCase();
			break;
			}
		case EAlphabeticLower:
			{
			TAlphabeticNumeral alphaL;
			chars = alphaL.DeneryToChar(aValueText,aValue);
			aValueText.LowerCase();
			break;
			}
		default:
			{
			chars = KErrGeneral;
			break;
			}
		}
	return chars;
	}


 

EXPORT_C CPageFieldBase::TNumberStyle CPageFieldBase::NumberStyle()const
/** Gets the numeric style.

@return The numeric style. */
	{return iStyle;}


// CPageNumField

 
EXPORT_C TInt CPageNumField::Value(TPtr& aValueText)
/** Gets the current page number, by calling UpdateFieldPageNum() (implemented 
by the object passed to the field using SetPageNumInfo()).

Notes

SetPageNumInfo() must have been called beforehand, or a panic occurs.

The text object should support pagination and pagination should have occurred 
before evaluating the field.

@param aValueText Descriptor which on return contains the current page number, 
converted into the appropriate style, as set by CPageFieldBase::SetNumberStyle(). 

@return Zero if aValueText is long enough to hold the string. Otherwise, the 
length of the buffer which is required to hold the string. */
	{
	__ASSERT_ALWAYS(iPageNumInfo,Panic(ENoMFieldPageNumInfo));
	//
	return InsertValue(aValueText,iPageNumInfo->UpdateFieldPageNum());
	}


 
EXPORT_C TUid CPageNumField::Type()const
/** Gets the field's type UID.

@return KPageNumberFieldUid. */
	{return KPageNumberFieldUid;}


// CNumPagesField

 

EXPORT_C TInt CNumPagesField::Value(TPtr& aValueText)
/** Gets the total number of pages in the document, by calling UpdateFieldNumPages() 
(implemented by the object passed to the field using SetNumPagesInfo()).

Notes

SetNumPagesInfo() must have been called beforehand, or a panic occurs.

The text object should support pagination and pagination should have occurred 
before evaluating the field.

@param aValueText Descriptor which on return contains the number of pages 
in the document, converted into the appropriate style, as set by CPageFieldBase::SetNumberStyle(). 

@return Zero if aValueText is long enough to hold the string. Otherwise, the 
length of the buffer which is required to hold the string. */
	{
	__ASSERT_ALWAYS(iNumPagesInfo,Panic(ENoMFieldNumPagesInfo));
	//
	return InsertValue(aValueText,iNumPagesInfo->UpdateFieldNumPages());
	}


 
EXPORT_C TUid CNumPagesField::Type()const
/** Gets the field's type UID.

@return KNumPagesFieldUid. */
	{return KNumPagesFieldUid;}


// CFileNameField

 

EXPORT_C TStreamId CFileNameField::StoreL(CStreamStore& /*aStore*/)const
// Replace base method, since this class has NO persistent representation.
//
	{return KNullStreamId;}


 

EXPORT_C void CFileNameField::RestoreL(const CStreamStore& /*aStore*/,TStreamId /*aId*/)
// Replace base method, since this class has NO persistent representation.
//
	{return;}


 

EXPORT_C TInt CFileNameField::Value(TPtr& aValueText)
/** Gets the document's filename, by calling UpdateFieldFileName() (implemented 
by the object passed to the filename field using SetFileNameInfo()).

Note

SetFileNameInfo() must have been called beforehand, or a panic occurs.

@param aValueText Descriptor which on return contains the document's filename. 

@return Zero if aValueText is long enough to hold the filename. Otherwise, 
the length of the buffer which is required to hold the filename. */
	{
	__ASSERT_ALWAYS(iFileNameInfo,Panic(ENoMFieldFileNameInfo));

	return iFileNameInfo->UpdateFieldFileName(aValueText);
	}


 
EXPORT_C TUid CFileNameField::Type()const
/** Gets the field's type UID.

@return KFileNameFieldUid. */
	{return KFileNameFieldUid;}
