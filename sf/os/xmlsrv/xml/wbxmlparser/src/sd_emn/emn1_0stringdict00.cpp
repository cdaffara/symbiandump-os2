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

#include "emn1_0stringdict00.h"
#include "emn1_0stringdict00tagtable.h"
#include "emn1_0stringdict00attributetable.h"


using namespace Xml;

MStringDictionary* CEmn1_0StringDict00::NewL(TAny* aStringPool)
	{
	CEmn1_0StringDict00* self = new(ELeave) CEmn1_0StringDict00(reinterpret_cast<RStringPool*>(aStringPool));
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return (static_cast<MStringDictionary*>(self));
	}



CEmn1_0StringDict00::CEmn1_0StringDict00(RStringPool* aStringPool)
:	iStringPool(*aStringPool),
	iCodePage(0)
	{
	// do nothing;
	}



void CEmn1_0StringDict00::ConstructL()
	{
	// we don't own this stringpool
	iStringPool.OpenL(Emn1_0StringDict00TagTable::Table);
	iStringPool.OpenL(Emn1_0StringDict00AttributeTable::Table);
	

	iCodepage00Table = CDictionaryCodePage::NewL(
		&Emn1_0StringDict00TagTable::Table, 
		&Emn1_0StringDict00AttributeTable::Table, 
		0,0); // codepage

	// Construct the correlation mapping
	iCodepage00Table->ConstructIndexMappingL(iTagCodePage00, CDictionaryCodePage::EStringTypeElement);
	iCodepage00Table->ConstructIndexMappingL(iAttributeCodePage00, CDictionaryCodePage::EStringTypeAttribute);
	}




void CEmn1_0StringDict00::Release()
	{
	delete (this);
	}




CEmn1_0StringDict00::~CEmn1_0StringDict00()
	{
	if (iCodepage00Table)
		{
		delete iCodepage00Table;
		}
	}



void CEmn1_0StringDict00::ElementL(TInt aToken, RString& aElement) const
	{
	TInt index = iCodepage00Table->StringPoolIndexFromToken(
					aToken, CDictionaryCodePage::EStringTypeElement);

	if (index == KErrXmlStringPoolTableNotFound)
		{
		User::Leave(KErrXmlUnsupportedElement);
		}

	// when we have multiple codepages per string dictionary we'd do something like iCodepageTable[n]->StringTable()
	aElement = iStringPool.String(
		index,
		*(iCodepage00Table->StringTable(CDictionaryCodePage::EStringTypeElement)));
	}



void CEmn1_0StringDict00::AttributeL(TInt aToken, RString& aAttribute) const
	{
	TInt index = iCodepage00Table->StringPoolIndexFromToken(
					aToken, CDictionaryCodePage::EStringTypeAttribute);

	if (index == KErrXmlStringPoolTableNotFound)
		{
		User::Leave(KErrXmlUnsupportedAttribute);
		}

	// when we have multiple codepages per string dictionary we'd do something like iCodepageTable[n]->StringTable()
	aAttribute = iStringPool.String(
		index,
		*(iCodepage00Table->StringTable(CDictionaryCodePage::EStringTypeAttribute)));
	};



void CEmn1_0StringDict00::AttributeValuePairL(TInt aToken, RString& aAttribute, RString& aValue) const
	{
	AttributeL(aToken, aAttribute);
	AttributeValueL(aToken, aValue);
	};
	


void CEmn1_0StringDict00::AttributeValueL(TInt /*aToken*/, RString& /*aValue*/) const
	{
/*	TInt index = iCodepage00Table->StringPoolIndexFromToken(
					aToken, CDictionaryCodePage::EStringTypeAttributeValue);

	if (index == KErrXmlStringPoolTableNotFound)
		{
		User::Leave(KErrXmlUnsupportedAttributeValue);
		}

	// when we have multiple codepages per string dictionary we'd do something like iCodepageTable[n]->StringTable()
	aValue = iStringPool.String(
		index,
		*(iCodepage00Table->StringTable(CDictionaryCodePage::EStringTypeAttributeValue)));*/
	};



TBool CEmn1_0StringDict00::CompareThisDictionary(const RString& aDictionaryDescription) const
	{
	// If this string dictionary has many codepages then all these comparisons should go here.
	// Remember, the string dictionary loads up all the RStringTables into its RStringPool
	// on construction. So if the comparison fails we do not have it.
	return (
		(aDictionaryDescription == iStringPool.String(Emn1_0StringDict00TagTable::EUri, Emn1_0StringDict00TagTable::Table))		||
		(aDictionaryDescription == iStringPool.String(Emn1_0StringDict00TagTable::EPublicId, Emn1_0StringDict00TagTable::Table))||
		(aDictionaryDescription == iStringPool.String(Emn1_0StringDict00TagTable::EFormalPublicId, Emn1_0StringDict00TagTable::Table)));
	}



TInt CEmn1_0StringDict00::SwitchCodePage(TInt aCodePage)
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

void CEmn1_0StringDict00::PublicIdentifier(RString& aPubId)
	{
	aPubId = iStringPool.String(
		Emn1_0StringDict00TagTable::EFormalPublicId,
		*(iCodepage00Table->StringTable(CDictionaryCodePage::EStringTypeElement)));
	}


/**
The element types in the Device Information DTD are defined within
a namespace associated with the Uri/Urn available from the StringTable.
The RString need not be closed, but closing is harmless. 

@param				aUri The associated namespace for this string 
					dictionary.
*/

void CEmn1_0StringDict00::NamespaceUri(RString& aUri)
	{
	aUri = iStringPool.String(
		Emn1_0StringDict00TagTable::EUri,
		*(iCodepage00Table->StringTable(CDictionaryCodePage::EStringTypeElement)));
	}




// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = {
	IMPLEMENTATION_PROXY_ENTRY(0x20022C28,CEmn1_0StringDict00::NewL)
};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

