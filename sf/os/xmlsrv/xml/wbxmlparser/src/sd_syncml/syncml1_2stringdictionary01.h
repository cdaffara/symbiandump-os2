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
// Plugin interface
// 
//


#ifndef __SYNCMLSTRINGDICTIONARY01_V1_2_H__
#define __SYNCMLSTRINGDICTIONARY01_V1_2_H__

#include <e32std.h>
#include <stringpool.h>

#include <xml/plugins/stringdictionary.h>

class CSyncml1_2StringDictionary01 : public CBase, public Xml::MStringDictionary
/**
The Syncml1_2StringDictionary01 class is a derived class defining the API of a single string dictionary. 

Derived classes must add all elements, attributes names, and attribute values that belong
to this string dictionary as static string tables to the StringPool. WBXML token mappings between 
strings and WBXML tokens should also be maintained.

@internalTechnology
*/
	{
public:

	static Xml::MStringDictionary* NewL(TAny* aStringPool);
	virtual ~CSyncml1_2StringDictionary01();

	void ConstructL();

	void ElementL(TInt aToken, RString& aElement) const;
	void AttributeL(TInt aToken, RString& aAttribute) const;
	void AttributeValuePairL(TInt aToken, RString& aAttribute, RString& aValue) const;
	void AttributeValueL(TInt aToken, RString& aValue) const;

	TBool CompareThisDictionary(const RString& aDictionaryDescription) const;

	TInt SwitchCodePage(TInt aCodePage);
	void PublicIdentifier(RString& aPubId);
	void NamespaceUri(RString& aUri);

	void Release();

public:

	// CodePage tokens.
	//
	// Note: There will be correlation between these token values and their
	// strings via the CDictionaryCodePage using the RStringPool.

	enum TTokenValue {
		// CodePage 01
		// -//SYNCML//DTD MetInf 1.2//EN
		ETokenValueAnchor				= 0x05,
		ETokenValueEMI					= 0x06,
		ETokenValueFormat				= 0x07,
		ETokenValueFreeID				= 0x08,
		ETokenValueFreeMem				= 0x09,
		ETokenValueLast					= 0x0A,
		ETokenValueMark					= 0x0B,
		ETokenValueMaxMsgSize			= 0x0C,
		ETokenValueMem					= 0x0D,
		ETokenValueMetInf				= 0x0E,
		ETokenValueNext					= 0x0F,
		ETokenValueNextNonce			= 0x10,
		ETokenValueSharedMem			= 0x11,
		ETokenValueSize					= 0x12,
		ETokenValueType					= 0x13,
		ETokenValueVersion				= 0x14,
		ETokenValueMaxObjSize			= 0x15,
		ETokenValueFieldLevel			= 0x16,
	};


private:

	CSyncml1_2StringDictionary01(RStringPool* aStringPool);

private:

/**
The StringPool for this string dictionary.
We don't own this.
*/
	RStringPool				iStringPool;

	Xml::CDictionaryCodePage*	iCodepage01Table;

	TInt					iCodePage;

	};


const TInt iCodePage01[] = 
	{CSyncml1_2StringDictionary01::ETokenValueAnchor,
	 CSyncml1_2StringDictionary01::ETokenValueEMI,
	 CSyncml1_2StringDictionary01::ETokenValueFormat,
	 CSyncml1_2StringDictionary01::ETokenValueFreeID,
	 CSyncml1_2StringDictionary01::ETokenValueFreeMem,
	 CSyncml1_2StringDictionary01::ETokenValueLast,
	 CSyncml1_2StringDictionary01::ETokenValueMark,
	 CSyncml1_2StringDictionary01::ETokenValueMaxMsgSize,
	 CSyncml1_2StringDictionary01::ETokenValueMem,
	 CSyncml1_2StringDictionary01::ETokenValueMetInf,
	 CSyncml1_2StringDictionary01::ETokenValueNext,
	 CSyncml1_2StringDictionary01::ETokenValueNextNonce,
	 CSyncml1_2StringDictionary01::ETokenValueSharedMem,
	 CSyncml1_2StringDictionary01::ETokenValueSize,
	 CSyncml1_2StringDictionary01::ETokenValueType,
	 CSyncml1_2StringDictionary01::ETokenValueVersion,
	 CSyncml1_2StringDictionary01::ETokenValueMaxObjSize,
	 CSyncml1_2StringDictionary01::ETokenValueFieldLevel,
	 0}; // ok to end with zero as this is used by a global token

#endif //__SYNCMLSTRINGDICTIONARY01_V1_2_H__
