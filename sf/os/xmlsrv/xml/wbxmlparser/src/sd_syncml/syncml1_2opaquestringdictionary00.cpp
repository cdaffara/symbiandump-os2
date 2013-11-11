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

#include "syncml1_2opaquestringdictionary00.h"
#include "syncml1_2opaquecodepage00tagtable.h"

using namespace Xml;

MStringDictionary* CSyncml1_2OpaqueStringDictionary00::NewL(TAny* aStringPool)
	{
	CSyncml1_2OpaqueStringDictionary00* self = new(ELeave) CSyncml1_2OpaqueStringDictionary00(reinterpret_cast<RStringPool*>(aStringPool));
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return (static_cast<MStringDictionary*>(self));
	}



CSyncml1_2OpaqueStringDictionary00::CSyncml1_2OpaqueStringDictionary00(RStringPool* aStringPool)
:	iStringPool(*aStringPool),
	iCodePage(0)
	{
	// do nothing;
	}



void CSyncml1_2OpaqueStringDictionary00::ConstructL()
	{
	// we don't own this stringpool
	iStringPool.OpenL(Syncml1_2OpaqueCodePage00TagTable::Table);

	iCodepage00Table = CDictionaryCodePage::NewL(&Syncml1_2OpaqueCodePage00TagTable::Table, 0, 0, 0);

	// Construct the correlation mapping
	iCodepage00Table->ConstructIndexMappingL(iCodePage00, CDictionaryCodePage::EStringTypeElement);
	}




void CSyncml1_2OpaqueStringDictionary00::Release()
	{
	delete (this);
	}




CSyncml1_2OpaqueStringDictionary00::~CSyncml1_2OpaqueStringDictionary00()
	{
	if (iCodepage00Table)
		{
		delete iCodepage00Table;
		}
	}



void CSyncml1_2OpaqueStringDictionary00::ElementL(TInt aToken, RString& aElement) const
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



void CSyncml1_2OpaqueStringDictionary00::AttributeL(TInt /*aToken*/, RString& /*aAttribute*/) const
	{
	// SyncML does not have attributes
	User::Leave(KErrXmlUnsupportedAttribute);
	};



void CSyncml1_2OpaqueStringDictionary00::AttributeValuePairL(TInt /*aToken*/, RString& /*aAttribute*/, RString& /*aValue*/) const
	{
	// SyncML does not have attributes
	User::Leave(KErrXmlUnsupportedAttribute);
	};
	


void CSyncml1_2OpaqueStringDictionary00::AttributeValueL(TInt /*aToken*/, RString& /*aValue*/) const
	{
	// SyncML does not have attributes
	User::Leave(KErrXmlUnsupportedAttributeValue);
	};



TBool CSyncml1_2OpaqueStringDictionary00::CompareThisDictionary(const RString& aDictionaryDescription) const
	{
	// If this string dictionary has many codepages then all these comparisons should go here.
	// Remember, the string dictionary loads up all the RStringTables into its RStringPool
	// on construction. So if the comparison fails we do not have it.
	return (
		(aDictionaryDescription == iStringPool.String(Syncml1_2OpaqueCodePage00TagTable::EUrn, Syncml1_2OpaqueCodePage00TagTable::Table))		||
		(aDictionaryDescription == iStringPool.String(Syncml1_2OpaqueCodePage00TagTable::EPublicId, Syncml1_2OpaqueCodePage00TagTable::Table))||
		(aDictionaryDescription == iStringPool.String(Syncml1_2OpaqueCodePage00TagTable::EFormalPublicId, Syncml1_2OpaqueCodePage00TagTable::Table)));
	}



TInt CSyncml1_2OpaqueStringDictionary00::SwitchCodePage(TInt aCodePage)
	{
	// We only have one codepage so can't switch
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

void CSyncml1_2OpaqueStringDictionary00::PublicIdentifier(RString& aPubId)
	{
	// Better to use the element table as some documents don't have attributes.
	aPubId = iStringPool.String(
		Syncml1_2OpaqueCodePage00TagTable::EFormalPublicId,
		*(iCodepage00Table->StringTable(CDictionaryCodePage::EStringTypeElement)));
	}


/**
The element types in the Device Information DTD are defined within
a namespace associated with the Uri/Urn available from the StringTable.
The RString need not be closed, but closing is harmless. 

@param				aUri The associated namespace for this string 
					dictionary.
*/

void CSyncml1_2OpaqueStringDictionary00::NamespaceUri(RString& aUri)
	{
	// Better to use the element table as some documents don't have attributes.
	aUri = iStringPool.String(
		Syncml1_2OpaqueCodePage00TagTable::EUrn,
		*(iCodepage00Table->StringTable(CDictionaryCodePage::EStringTypeElement)));
	}


// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = {
	IMPLEMENTATION_PROXY_ENTRY(0x10207FA4,CSyncml1_2OpaqueStringDictionary00::NewL)
};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

