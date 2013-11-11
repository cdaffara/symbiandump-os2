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

#ifndef __EMNSTRINGDICT00_H__
#define __EMNSTRINGDICT00_H__

#include <e32std.h>
#include <stringpool.h>

#include <xml/plugins/stringdictionary.h>


class CEmn1_0StringDict00 : public CBase, public Xml::MStringDictionary
/**
The Emn1_0StringDict00 class is a derived class defining the API of a single string dictionary. 

Derived classes must add all elements, attributes names, and attribute values that belong
to this string dictionary as static string tables to the StringPool. WBXML token mappings between 
strings and WBXML tokens should also be maintained.

@internalTechnology
*/
	{
public:
	static Xml::MStringDictionary* NewL(TAny* aStringPool);
	virtual ~CEmn1_0StringDict00();
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
		// -//WAPFORUM//DTD EMN 1.0//EN
		ETagTokenEmn 						= 0x05,
	};


	enum TAttributeToken {
		// CodePage 00
		// -//WAPFORUM//DTD EMN 1.0//EN
		EAttributeTokenMailbox 				= 0x05,
		EAttributeTokenTimestamp 				= 0x06,
	};



	

private:

	CEmn1_0StringDict00(RStringPool* aStringPool);

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
	{CEmn1_0StringDict00::ETagTokenEmn,
	 0}; // ok to end with zero as this is used by a global token


const TInt iAttributeCodePage00[] = 
	{CEmn1_0StringDict00::EAttributeTokenMailbox,
	 CEmn1_0StringDict00::EAttributeTokenTimestamp,
	 0}; // ok to end with zero as this is used by a global token



#endif //__EMNSTRINGDICT00_H__
