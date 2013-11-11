// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include <e32std.h>

#include <xml/xmlframeworkerrors.h>
#include "XmlFrameworkPanics.h"
#include <xml/plugins/dictionarycodepage.h>

using namespace Xml;

const TInt KTokenGranularity=30;

EXPORT_C CDictionaryCodePage* CDictionaryCodePage::NewL(const TStringTable* aElementTable, 
											  			const TStringTable* aAttributeTable, 
											  			const TStringTable* aValueTable, 
											  			TUint8 aCodePage)

	{
	return new(ELeave) CDictionaryCodePage(aElementTable, 
										   aAttributeTable, 
										   aValueTable, 
										   aCodePage);
	}
	
	
EXPORT_C CDictionaryCodePage::~CDictionaryCodePage()
/**
Destructor.

@post				This object may be allowed to go out of scope.

*/
	{
	iElementStringPoolIndexToToken.Close();
	iElementTokenToStringPoolIndex.Close();	
	iAttributeStringPoolIndexToToken.Close();
	iAttributeTokenToStringPoolIndex.Close();	
	iValueStringPoolIndexToToken.Close();
	iValueTokenToStringPoolIndex.Close();	
	}


CDictionaryCodePage::CDictionaryCodePage(const TStringTable* aElementTable, 
										const TStringTable* aAttributeTable, 
										const TStringTable* aValueTable, 
										TUint8 aCodePage) 
/**
Constructor.

@post				This object is properly constructed.

@param				aElementTable the element string table associated with this code page.
@param				aAttributeTable the attribute string table associated with this code page.
@param				aValueTable the attributevalue string table associated with this code page.
@param				aCodePage the codepage number for this object.

*/
:	iElementTable(aElementTable), // we do not own these StringTable
	iAttributeTable(aAttributeTable),
	iValueTable(aValueTable),
	iElementStringPoolIndexToToken(KTokenGranularity), 
	iElementTokenToStringPoolIndex(KTokenGranularity,
								   _FOFF(TStringPoolTokenMapping, iTokenValue)), // key offset
	iAttributeStringPoolIndexToToken(KTokenGranularity), 
	iAttributeTokenToStringPoolIndex(KTokenGranularity, 
									 _FOFF(TStringPoolTokenMapping, iTokenValue)), // key offset
	iValueStringPoolIndexToToken(KTokenGranularity), 
	iValueTokenToStringPoolIndex(KTokenGranularity, 
								 _FOFF(TStringPoolTokenMapping, iTokenValue)), // key offset
	iCodePage(aCodePage)
	{
	// do nothing;
	}


EXPORT_C const TStringTable* CDictionaryCodePage::StringTable(TStringType aType) const
/**
This method obtains the correct string table based on the type passed.

@return				the correct string table for that type.

@param				aType the type of string table required, e.g. element.
@panic				EXmlFrameworkPanicUnexpectedLogic If the type cannot be recognised.

*/
	{
	const TStringTable* table = 0;

	switch (aType)
		{
		case EStringTypeElement:
			{
			table = iElementTable;
			break;
			}
		case EStringTypeAttribute:
			{
			table = iAttributeTable;
			break;
			}
		case EStringTypeAttributeValue:
			{
			table = iValueTable;
			break;
			}
		default:
			{
			__ASSERT_ALWAYS(EFalse, Panic(EXmlFrameworkPanicUnexpectedLogic));
			}
		};
	return table;
	}



EXPORT_C TUint8 CDictionaryCodePage::CodePage() const
/**
This method returns the codepage number for this page.

@return				the codepage number.

*/
	{
	return iCodePage;
	}



EXPORT_C TInt CDictionaryCodePage::StringPoolIndexFromToken(TInt aToken, TStringType aType) const
/**
This method obtains a String Pool index from a token value

@return				the String Pool index
@return				KErrXmlStringPoolTableNotFound

@param				aToken is the token value.
@param				aType is the type of the token, e.g. element.

@panic				EXmlFrameworkPanicUnexpectedLogic If the type cannot be recognised.

*/
	{
	const RArray<TStringPoolTokenMapping>* toIndex = 0;

	switch (aType)
		{
		case EStringTypeElement:
			{
			toIndex = &iElementTokenToStringPoolIndex;
			break;
			}
		case EStringTypeAttribute:
			{
			toIndex = &iAttributeTokenToStringPoolIndex;
			break;
			}
		case EStringTypeAttributeValue:
			{
			toIndex = &iValueTokenToStringPoolIndex;
			break;
			}
		default:
			{
			__ASSERT_ALWAYS(EFalse, Panic(EXmlFrameworkPanicUnexpectedLogic));
			}
		};

	TStringPoolTokenMapping map;
	map.iTokenValue = aToken;
	TInt index;

	// Since tokens are not numerically adjacent numbers a search is carried out:
	if (toIndex->FindInSignedKeyOrder(map, index) == KErrNone)
		{
		return ((*toIndex)[index]).iTableIndex;
		}
	return KErrXmlStringPoolTableNotFound;
	}



EXPORT_C TInt CDictionaryCodePage::TokenFromStringPoolIndex(TInt aIndex, TStringType aType) const
/**
This method obtains a Token value from a String Pool index.

@return				the Token value.

@param				aIndex is the String Pool index
@param				aType is the type of the token, e.g. element.

@panic				EXmlFrameworkPanicUnexpectedLogic If the type cannot be recognised.

*/
	{
	const RArray<TInt>* toToken = 0;

	switch (aType)
		{
		case EStringTypeElement:
			{
			toToken = &iElementStringPoolIndexToToken;
			break;
			}
		case EStringTypeAttribute:
			{
			toToken = &iAttributeStringPoolIndexToToken;
			break;
			}
		case EStringTypeAttributeValue:
			{
			toToken = &iValueStringPoolIndexToToken;
			break;
			}
		default:
			{
			__ASSERT_ALWAYS(EFalse, Panic(EXmlFrameworkPanicUnexpectedLogic));
			}
		};

	return ((*toToken)[aIndex]);
	}



EXPORT_C void CDictionaryCodePage::ConstructIndexMappingL(const TInt* aStringPoolToTokenMapping, 
													 TStringType aType)
/**
This method constructs the internal correlation between indices and tokens.

@param				aStringPoolToTokenMapping array of token values, NULL terminated.
@param				aType is the type of the token, e.g. element.

@panic				EXmlFrameworkPanicUnexpectedLogic If the type cannot be recognised.

*/
	{
	RArray<TInt>* toToken = 0;
	RArray<TStringPoolTokenMapping>* toIndex = 0;

	switch (aType)
		{
		case EStringTypeElement:
			{
			toToken = &iElementStringPoolIndexToToken;
			toIndex = &iElementTokenToStringPoolIndex;
			break;
			}
		case EStringTypeAttribute:
			{
			toToken = &iAttributeStringPoolIndexToToken;
			toIndex = &iAttributeTokenToStringPoolIndex;
			break;
			}
		case EStringTypeAttributeValue:
			{
			toToken = &iValueStringPoolIndexToToken;
			toIndex = &iValueTokenToStringPoolIndex;
			break;
			}
		default:
			{
			__ASSERT_ALWAYS(EFalse, Panic(EXmlFrameworkPanicUnexpectedLogic));
			}
		};

	TInt element = 0;
	for (TInt count=0; (element = aStringPoolToTokenMapping[count]) != 0; ++count)
		{
		// The index to token mapping is straight forward.
		// We simply add a table index. 
		// The table index is the same index into the RArray saving space.
		User::LeaveIfError(toToken->Append(element));

		// The token to index mapping is slightly more complicated,
		// and we use a compare function to extract it.
		TStringPoolTokenMapping mapping;
		mapping.iTokenValue = element;		// This is the primary key (Token)
		mapping.iTableIndex = count;								// This is the Table index
		User::LeaveIfError(toIndex->InsertInOrder(
								mapping, 
								TLinearOrder<TStringPoolTokenMapping>(
									CDictionaryCodePage::CompareStringPoolTokenMappingTable)));
		}	
	}



TInt CDictionaryCodePage::CompareStringPoolTokenMappingTable(const TStringPoolTokenMapping& aFirst, 
														const TStringPoolTokenMapping& aSecond)
/**
This method compares two codepages and determines the order of two objects of a given class type.

@return				zero, if the two objects are equal
@return				a negative value, if the first object is less than the second.
@return				a positive value, if the first object is greater than the second.

@param				aFirst the first object.
@param				aSecond the second object.
*/
	{
	return aFirst.iTokenValue - aSecond.iTokenValue;
	}
