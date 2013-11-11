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

#include "si1_0stringdict00.h"
#include "si1_0stringdict00tagtable.h"
#include "si1_0stringdict00attributetable.h"
#include "si1_0stringdict00attributevaluetable.h"

using namespace Xml;

MStringDictionary* CSi1_0StringDict00::NewL(TAny* aStringPool)
	{
	CSi1_0StringDict00* self = new(ELeave) CSi1_0StringDict00(reinterpret_cast<RStringPool*>(aStringPool));
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return (static_cast<MStringDictionary*>(self));
	}



CSi1_0StringDict00::CSi1_0StringDict00(RStringPool* aStringPool)
:	iStringPool(*aStringPool),
	iCodePage(0)
	{
	// do nothing;
	}



void CSi1_0StringDict00::ConstructL()
	{
	// we don't own this stringpool
	iStringPool.OpenL(Si1_0StringDict00TagTable::Table);
	iStringPool.OpenL(Si1_0StringDict00AttributeTable::Table);
	iStringPool.OpenL(Si1_0StringDict00AttributeValueTable::Table);

	iCodepage00Table = CDictionaryCodePage::NewL(
		&Si1_0StringDict00TagTable::Table, 
		&Si1_0StringDict00AttributeTable::Table, 
		&Si1_0StringDict00AttributeValueTable::Table, 
		0); // codepage

	// Construct the correlation mapping
	iCodepage00Table->ConstructIndexMappingL(iTagCodePage00, CDictionaryCodePage::EStringTypeElement);
	iCodepage00Table->ConstructIndexMappingL(iAttributeCodePage00, CDictionaryCodePage::EStringTypeAttribute);
	iCodepage00Table->ConstructIndexMappingL(iAttributeValueCodePage00, CDictionaryCodePage::EStringTypeAttributeValue);
	}




void CSi1_0StringDict00::Release()
	{
	delete (this);
	}




CSi1_0StringDict00::~CSi1_0StringDict00()
	{
	if (iCodepage00Table)
		{
		delete iCodepage00Table;
		}
	}



void CSi1_0StringDict00::ElementL(TInt aToken, RString& aElement) const
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



void CSi1_0StringDict00::AttributeL(TInt aToken, RString& aAttribute) const
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



void CSi1_0StringDict00::AttributeValuePairL(TInt aToken, RString& aAttribute, RString& aValue) const
	{
	AttributeL(aToken, aAttribute);
	AttributeValueL(aToken, aValue);
	};
	


void CSi1_0StringDict00::AttributeValueL(TInt aToken, RString& aValue) const
	{
	TInt index = iCodepage00Table->StringPoolIndexFromToken(
					aToken, CDictionaryCodePage::EStringTypeAttributeValue);

	if (index == KErrXmlStringPoolTableNotFound)
		{
		User::Leave(KErrXmlUnsupportedAttributeValue);
		}

	// when we have multiple codepages per string dictionary we'd do something like iCodepageTable[n]->StringTable()
	aValue = iStringPool.String(
		index,
		*(iCodepage00Table->StringTable(CDictionaryCodePage::EStringTypeAttributeValue)));
	};



TBool CSi1_0StringDict00::CompareThisDictionary(const RString& aDictionaryDescription) const
	{
	// If this string dictionary has many codepages then all these comparisons should go here.
	// Remember, the string dictionary loads up all the RStringTables into its RStringPool
	// on construction. So if the comparison fails we do not have it.
	return (
		(aDictionaryDescription == iStringPool.String(Si1_0StringDict00TagTable::EUri, Si1_0StringDict00TagTable::Table))		||
		(aDictionaryDescription == iStringPool.String(Si1_0StringDict00TagTable::EPublicId, Si1_0StringDict00TagTable::Table))||
		(aDictionaryDescription == iStringPool.String(Si1_0StringDict00TagTable::EFormalPublicId, Si1_0StringDict00TagTable::Table)));
	}



TInt CSi1_0StringDict00::SwitchCodePage(TInt aCodePage)
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

void CSi1_0StringDict00::PublicIdentifier(RString& aPubId)
	{
	aPubId = iStringPool.String(
		Si1_0StringDict00TagTable::EFormalPublicId,
		*(iCodepage00Table->StringTable(CDictionaryCodePage::EStringTypeElement)));
	}


/**
The element types in the Device Information DTD are defined within
a namespace associated with the Uri/Urn available from the StringTable.
The RString need not be closed, but closing is harmless. 

@param				aUri The associated namespace for this string 
					dictionary.
*/

void CSi1_0StringDict00::NamespaceUri(RString& aUri)
	{
	aUri = iStringPool.String(
		Si1_0StringDict00TagTable::EUri,
		*(iCodepage00Table->StringTable(CDictionaryCodePage::EStringTypeElement)));
	}




// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = {
	IMPLEMENTATION_PROXY_ENTRY(0x101FE2A7,CSi1_0StringDict00::NewL)
};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

