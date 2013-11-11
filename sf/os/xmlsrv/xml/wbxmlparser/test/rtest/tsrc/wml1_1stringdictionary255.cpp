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

#include <e32std.h>
#include <ecom/implementationproxy.h>

#include <xml/xmlframeworkerrors.h>
#include <xml/plugins/dictionarycodepage.h>

#include "wml1_1stringdictionary255.h"
#include "wml1_1codepage255attributetable.h"
#include "wml1_1codepage255attributevaluetable.h"


using namespace Xml;


MStringDictionary* CWml1_1StringDictionary255::NewL(TAny* aStringPool)
	{
	CWml1_1StringDictionary255* self = new(ELeave) CWml1_1StringDictionary255(reinterpret_cast<RStringPool*>(aStringPool));
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return (static_cast<MStringDictionary*>(self));
	}



CWml1_1StringDictionary255::CWml1_1StringDictionary255(RStringPool* aStringPool)
:	iStringPool(*aStringPool),
	iCodePage(255)
	{
	// do nothing;
	}



void CWml1_1StringDictionary255::ConstructL()
	{
	// we don't own this stringpool
	iStringPool.OpenL(Wml1_1CodePage255AttributeTable::Table);
	iStringPool.OpenL(Wml1_1CodePage255AttributeValueTable::Table);

	iCodePage255Table = CDictionaryCodePage::NewL(
		0, 
		&Wml1_1CodePage255AttributeTable::Table, 
		&Wml1_1CodePage255AttributeValueTable::Table, 
		0); // codepage

	// Construct the correlation mapping
	iCodePage255Table->ConstructIndexMappingL(iAttributeCodePage255, CDictionaryCodePage::EStringTypeAttribute);
	iCodePage255Table->ConstructIndexMappingL(iAttributeValueCodePage255, CDictionaryCodePage::EStringTypeAttributeValue);
	}




void CWml1_1StringDictionary255::Release()
	{
	delete (this);
	}




CWml1_1StringDictionary255::~CWml1_1StringDictionary255()
	{
	if (iCodePage255Table)
		{
		delete iCodePage255Table;
		}
	}



void CWml1_1StringDictionary255::ElementL(TInt /*aToken*/, RString& /*aElement*/) const
	{
	User::Leave(KErrXmlUnsupportedElement);
	}



void CWml1_1StringDictionary255::AttributeL(TInt aToken, RString& aAttribute) const
	{
	TInt index = iCodePage255Table->StringPoolIndexFromToken(
					aToken, CDictionaryCodePage::EStringTypeAttribute);

	if (index == KErrXmlStringPoolTableNotFound)
		{
		User::Leave(KErrXmlUnsupportedAttribute);
		}

	// when we have multiple codepages per string dictionary we'd do something like iCodepageTable[n]->StringTable()
	aAttribute = iStringPool.String(
		index,
		*(iCodePage255Table->StringTable(CDictionaryCodePage::EStringTypeAttribute)));
	};



void CWml1_1StringDictionary255::AttributeValuePairL(TInt aToken, RString& aAttribute, RString& aValue) const
	{
	AttributeL(aToken, aAttribute);
	AttributeValueL(aToken, aValue);
	};
	


void CWml1_1StringDictionary255::AttributeValueL(TInt aToken, RString& aValue) const
	{
	TInt index = iCodePage255Table->StringPoolIndexFromToken(
					aToken, CDictionaryCodePage::EStringTypeAttributeValue);

	if (index == KErrXmlStringPoolTableNotFound)
		{
		User::Leave(KErrXmlUnsupportedAttributeValue);
		}

	// when we have multiple codepages per string dictionary we'd do something like iCodepageTable[n]->StringTable()
	aValue = iStringPool.String(
		index,
		*(iCodePage255Table->StringTable(CDictionaryCodePage::EStringTypeAttributeValue)));
	};



TBool CWml1_1StringDictionary255::CompareThisDictionary(const RString& aDictionaryDescription) const
	{
	// If this string dictionary has many codepages then all these comparisons should go here.
	// Remember, the string dictionary loads up all the RStringTables into its RStringPool
	// on construction. So if the comparison fails we do not have it.
	return (
		(aDictionaryDescription == iStringPool.String(Wml1_1CodePage255AttributeTable::EUri, Wml1_1CodePage255AttributeTable::Table))		||
		(aDictionaryDescription == iStringPool.String(Wml1_1CodePage255AttributeTable::EPublicId, Wml1_1CodePage255AttributeTable::Table))||
		(aDictionaryDescription == iStringPool.String(Wml1_1CodePage255AttributeTable::EFormalPublicId, Wml1_1CodePage255AttributeTable::Table)));
	}



TInt CWml1_1StringDictionary255::SwitchCodePage(TInt aCodePage)
	{
	// We only have one codepage sp can't switch
	if (aCodePage != iCodePage)
		{
		return KErrXmlMissingStringDictionary;
		}
	return iCodePage;
	}

		


		
/**
This method obtains the public identifier from the StringTable.
Either the formal or non formal public id will do.
The stringDictionary .rss files must list both these as wbxml
documents have one or the other.

@param				aPubId The public identifier for this string 
					dictionary.
*/

void CWml1_1StringDictionary255::PublicIdentifier(RString& aPubId)
	{
	// Better to use the element table as some documents don't have attributes.
	aPubId = iStringPool.String(
		Wml1_1CodePage255AttributeTable::EFormalPublicId,
		*(iCodePage255Table->StringTable(CDictionaryCodePage::EStringTypeAttribute)));
	}


/**
The element types in the Device Information DTD are defined within
a namespace associated with the Uri/Urn available from the StringTable.
The RString need not be closed, but closing is harmless. 

@param				aUri The associated namespace for this string 
					dictionary.
*/

void CWml1_1StringDictionary255::NamespaceUri(RString& aUri)
	{
	// Better to use the element table as some documents don't have attributes.
	aUri = iStringPool.String(
		Wml1_1CodePage255AttributeTable::EUri,
		*(iCodePage255Table->StringTable(CDictionaryCodePage::EStringTypeAttribute)));
	}




// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
	{{{0x101FD309}, (TProxyNewLPtr)&CWml1_1StringDictionary255::NewL}};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

