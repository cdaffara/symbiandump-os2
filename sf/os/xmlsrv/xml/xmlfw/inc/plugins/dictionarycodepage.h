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

#ifndef __DICTIONARYCODEPAGE_H__
#define __DICTIONARYCODEPAGE_H__

#include <e32base.h>
#include <stringpool.h>


namespace Xml
{

class CDictionaryCodePage : public CBase
/**
The CDictionaryCodePage, represents a single numeric code page for Elements, Attributes, and
AttributeValues.

This object refers to the appropriate string pool tables, and creates mappings between
strings in these tables and their token values.

Used mainly for wbxml document parsing, and allows for the quick comparison of strings.

This object is associated with a string dictionary via a user defined class.

@see RStringPool
@see CStringDictionary

@publishedPartner
@released
*/
	{

public:

	enum TStringType
/**
A structure for describing the types of string pool table associated with this class.
*/
		{
			EStringTypeElement,
			EStringTypeAttribute,
			EStringTypeAttributeValue
		};


public:

	IMPORT_C static CDictionaryCodePage* NewL(const TStringTable* aElementTable, 
											  const TStringTable* aAttributeTable, 
											  const TStringTable* aValueTable, 
											  TUint8 aCodePage);

	IMPORT_C virtual ~CDictionaryCodePage();
	
	IMPORT_C const TStringTable* StringTable(TStringType aType) const;
	IMPORT_C TUint8 CodePage() const;

	IMPORT_C TInt StringPoolIndexFromToken(TInt aToken, TStringType aType) const;
	IMPORT_C TInt TokenFromStringPoolIndex(TInt aIndex, TStringType aType) const;
	
	IMPORT_C void ConstructIndexMappingL(const TInt* aStringPoolToTokenMapping, TStringType aType);

private:

	CDictionaryCodePage(const TStringTable* aElementTable, const TStringTable* aAttributeTable, 
		                const TStringTable* aValueTable, TUint8 aCodePage);

	CDictionaryCodePage(const CDictionaryCodePage& aOriginal);
	CDictionaryCodePage& operator=(const CDictionaryCodePage& aRhs);

private:

	struct TStringPoolTokenMapping
/**
The TStringPoolTokenMapping struct stores a mapping between table index and 
token values for a single string entry.
*/
		{
		TInt iTokenValue;
		TInt iTableIndex;
		};

	static TInt CompareStringPoolTokenMappingTable(const TStringPoolTokenMapping& aFirst, 
		                                           const TStringPoolTokenMapping& aSecond);
private:

/**
Pointer to the static Element string pool table.
We do not own this.
*/	
	const TStringTable*				iElementTable;

/**
Pointer to the static Attribute string pool table.
We do not own this.
*/	
	const TStringTable*				iAttributeTable;

/**
Pointer to the static AttributeValue string pool table.
We do not own this.
*/	
	const TStringTable*				iValueTable;

/**
Array to obtain a Element Token from String Pool index.
*/	
	RArray<TInt>					iElementStringPoolIndexToToken;

/**
Array to obtain a Element String Pool Index from a token.
*/	
	RArray<TStringPoolTokenMapping>	iElementTokenToStringPoolIndex;

/**
Array to obtain a Attribute Token from String Pool index.
*/	
	RArray<TInt>					iAttributeStringPoolIndexToToken;

/**
Array to obtain a Attribute String Pool Index from a token.
*/	
	RArray<TStringPoolTokenMapping>	iAttributeTokenToStringPoolIndex;

/**
Array to obtain a Value Token from String Pool index.
*/	
	RArray<TInt>					iValueStringPoolIndexToToken;
																		
/**
Array to obtain a Value String Pool Index from a token.
*/	
	RArray<TStringPoolTokenMapping>	iValueTokenToStringPoolIndex;


/**
The numeric codepage this object represents.
*/	
	TUint8							iCodePage;

	};

}

#endif // __DICTIONARYCODEPAGE_H__
