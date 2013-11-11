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

#ifndef __SISTRINGDICT00_H__
#define __SISTRINGDICT00_H__

#include <e32std.h>
#include <stringpool.h>

#include <xml/plugins/stringdictionary.h>


class CSi1_0StringDict00 : public CBase, public Xml::MStringDictionary
/**
The Si1_0StringDict00 class is a derived class defining the API of a single string dictionary. 

Derived classes must add all elements, attributes names, and attribute values that belong
to this string dictionary as static string tables to the StringPool. WBXML token mappings between 
strings and WBXML tokens should also be maintained.

@internalTechnology
*/
	{
public:
	static Xml::MStringDictionary* NewL(TAny* aStringPool);
	virtual ~CSi1_0StringDict00();
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

	enum TTagToken {
		// CodePage 00
		// -//WAPFORUM//DTD SI 1.0//EN
		ETagTokenSi 						= 0x05,
		ETagTokenIndication 				= 0x06,
		ETagTokenInfo 						= 0x07,
		ETagTokenItem 						= 0x08,
	};


	enum TAttributeToken {
		// CodePage 00
		// -//WAPFORUM//DTD SI 1.0//EN
		EAttributeTokenAction1 				= 0x05,
		EAttributeTokenAction2 				= 0x06,
		EAttributeTokenAction3 				= 0x07,
		EAttributeTokenAction4 				= 0x08,
		EAttributeTokenAction5 				= 0x09,
		EAttributeTokenCreated 				= 0x0A,
		EAttributeTokenHref1 				= 0x0B,
		EAttributeTokenHref2 				= 0x0C,
		EAttributeTokenHref3 				= 0x0D,
		EAttributeTokenHref4 				= 0x0E,
		EAttributeTokenHref5 				= 0x0F,
		EAttributeTokenSiExpires 			= 0x10,
		EAttributeTokenSiId 				= 0x11,
		EAttributeTokenClass 				= 0x12,	
	};



	enum TAttributeValueToken {
		// CodePage 00
		// -//WAPFORUM//DTD SI 1.0//EN
		EAttributeValueTokenAction1 		= 0x05,
		EAttributeValueTokenAction2 		= 0x06,
		EAttributeValueTokenAction3 		= 0x07,
		EAttributeValueTokenAction4 		= 0x08,
		EAttributeValueTokenAction5 		= 0x09,
		EAttributeValueTokenCreated			= 0x0A,
		EAttributeValueTokenHref1			= 0x0B,
		EAttributeValueTokenHref2 			= 0x0C,
		EAttributeValueTokenHref3 			= 0x0D,
		EAttributeValueTokenHref4 			= 0x0E,
		EAttributeValueTokenHref5 			= 0x0F,
		EAttributeValueTokenSiExpires		= 0x10,
		EAttributeValueTokenSiId			= 0x11,
		EAttributeValueTokenClass			= 0x12,
		EAttributeValueTokenCom 			= 0x85,
		EAttributeValueTokenEdu 			= 0x86,
		EAttributeValueTokenNet 			= 0x87,
		EAttributeValueTokenOrg 			= 0x88,
	};



private:

	CSi1_0StringDict00(RStringPool* aStringPool);

private:

/**
The StringPool for this string dictionary.
We don't own this.
*/
	RStringPool				iStringPool;

	Xml::CDictionaryCodePage*	iCodepage00Table;

	TInt					iCodePage;

	};


const TInt iTagCodePage00[] = 
	{CSi1_0StringDict00::ETagTokenSi,
	 CSi1_0StringDict00::ETagTokenIndication,
	 CSi1_0StringDict00::ETagTokenInfo,
	 CSi1_0StringDict00::ETagTokenItem,
	 0}; // ok to end with zero as this is used by a global token


const TInt iAttributeCodePage00[] = 
	{CSi1_0StringDict00::EAttributeTokenAction1,
	 CSi1_0StringDict00::EAttributeTokenAction2,
	 CSi1_0StringDict00::EAttributeTokenAction3,
	 CSi1_0StringDict00::EAttributeTokenAction4,
	 CSi1_0StringDict00::EAttributeTokenAction5,
	 CSi1_0StringDict00::EAttributeTokenCreated,
	 CSi1_0StringDict00::EAttributeTokenHref1,
	 CSi1_0StringDict00::EAttributeTokenHref2,
	 CSi1_0StringDict00::EAttributeTokenHref3,
	 CSi1_0StringDict00::EAttributeTokenHref4,
	 CSi1_0StringDict00::EAttributeTokenHref5,
	 CSi1_0StringDict00::EAttributeTokenSiExpires,
	 CSi1_0StringDict00::EAttributeTokenSiId,
	 CSi1_0StringDict00::EAttributeTokenClass,	
	 0}; // ok to end with zero as this is used by a global token



const TInt iAttributeValueCodePage00[] = 
	{CSi1_0StringDict00::EAttributeValueTokenAction1,
	 CSi1_0StringDict00::EAttributeValueTokenAction2,
	 CSi1_0StringDict00::EAttributeValueTokenAction3,
	 CSi1_0StringDict00::EAttributeValueTokenAction4,
	 CSi1_0StringDict00::EAttributeValueTokenAction5,
	 CSi1_0StringDict00::EAttributeValueTokenCreated,
	 CSi1_0StringDict00::EAttributeValueTokenHref1,
	 CSi1_0StringDict00::EAttributeValueTokenHref2,
	 CSi1_0StringDict00::EAttributeValueTokenHref3,
	 CSi1_0StringDict00::EAttributeValueTokenHref4,
	 CSi1_0StringDict00::EAttributeValueTokenHref5,
	 CSi1_0StringDict00::EAttributeValueTokenSiExpires,
	 CSi1_0StringDict00::EAttributeValueTokenSiId,
	 CSi1_0StringDict00::EAttributeValueTokenClass,
	 CSi1_0StringDict00::EAttributeValueTokenCom,
	 CSi1_0StringDict00::EAttributeValueTokenEdu,
	 CSi1_0StringDict00::EAttributeValueTokenNet,
	 CSi1_0StringDict00::EAttributeValueTokenOrg,
	 0}; // ok to end with zero as this is used by a global token


#endif //__SISTRINGDICT00_H__
