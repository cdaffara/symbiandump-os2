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

#include "t_stringdictionary00.h"
#include "t_stringdictionary00tagtable.h"

using namespace Xml;

MStringDictionary* CTestStringDictionary00::NewL(TAny* aStringPool)
	{
	CTestStringDictionary00* self = new(ELeave) CTestStringDictionary00(reinterpret_cast<RStringPool*>(aStringPool));
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return (static_cast<MStringDictionary*>(self));
	}



CTestStringDictionary00::CTestStringDictionary00(RStringPool* aStringPool)
:	iStringPool(*aStringPool),
	iCodePage(0)
	{
	// do nothing;
	}



void CTestStringDictionary00::ConstructL()
	{
	// we don't own this stringpool
	iStringPool.OpenL(t_StringDictionary00TagTable::Table);

	iCodepage00Table = CDictionaryCodePage::NewL(&t_StringDictionary00TagTable::Table, 0, 0, 0);

	// Construct the correlation mapping
	iCodepage00Table->ConstructIndexMappingL(iCodePage00, CDictionaryCodePage::EStringTypeElement);
	}



void CTestStringDictionary00::Release()
	{
	delete (this);
	}




CTestStringDictionary00::~CTestStringDictionary00()
	{
	if (iCodepage00Table)
		{
		delete iCodepage00Table;
		}
	}



void CTestStringDictionary00::ElementL(TInt aToken, RString& aElement) const
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



void CTestStringDictionary00::AttributeL(TInt /*aToken*/, RString& /*aAttribute*/) const
	{
	// SyncML does not have attributes
	User::Leave(KErrXmlUnsupportedAttribute);
	};



void CTestStringDictionary00::AttributeValuePairL(TInt /*aToken*/, RString& /*aAttribute*/, RString& /*aValue*/) const
	{
	// SyncML does not have attributes
	User::Leave(KErrXmlUnsupportedAttribute);
	};
	


void CTestStringDictionary00::AttributeValueL(TInt /*aToken*/, RString& /*aValue*/) const
	{
	// SyncML does not have attributes
	User::Leave(KErrXmlUnsupportedAttributeValue);
	};



TBool CTestStringDictionary00::CompareThisDictionary(const RString& aDictionaryDescription) const
	{
	// If this Dictionary has many codepages then all these comparisons should go here.
	// Remember, the Dictionary loads up all the RStringTables into its RStringPool
	// on construction. So if the comparison fails we do not have it.
	if ((aDictionaryDescription == iStringPool.String(t_StringDictionary00TagTable::EUrn, t_StringDictionary00TagTable::Table))				||
		(aDictionaryDescription == iStringPool.String(t_StringDictionary00TagTable::EPublicId, t_StringDictionary00TagTable::Table))			||
		(aDictionaryDescription == iStringPool.String(t_StringDictionary00TagTable::EFormalPublicId, t_StringDictionary00TagTable::Table)))
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}



TInt CTestStringDictionary00::SwitchCodePage(TInt aCodePage)
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

void CTestStringDictionary00::PublicIdentifier(RString& aPubId)
	{
	// Better to use the element table as some documents don't have attributes.
	aPubId = iStringPool.String(
		t_StringDictionary00TagTable::EFormalPublicId,
		*(iCodepage00Table->StringTable(CDictionaryCodePage::EStringTypeElement)));
	}


/**
This method obtains the namespace uri from the StringTable.
				
@param				aUri The public identifier for this string 
					dictionary.
*/

void CTestStringDictionary00::NamespaceUri(RString& aUri)
	{
	// Better to use the element table as some documents don't have attributes.
	aUri = iStringPool.String(
		t_StringDictionary00TagTable::EUrn,
		*(iCodepage00Table->StringTable(CDictionaryCodePage::EStringTypeElement)));
	}



// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = {
	IMPLEMENTATION_PROXY_ENTRY(0x101FD172,CTestStringDictionary00::NewL)
};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

