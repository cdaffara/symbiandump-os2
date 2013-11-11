// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of Test parser. 
// 
//

/**
 @file 
 @internalComponent
*/

#include <e32std.h>
#include <ecom/implementationproxy.h>

#include <xml/attribute.h>
#include <xml/contenthandler.h>
#include <xml/documentparameters.h>
#include <xml/stringdictionarycollection.h>
#include <xml/taginfo.h>
#include <xml/wbxmlextensionhandler.h>
#include <xml/xmlframeworkerrors.h>

#include <xml/plugins/parserinterface.h>
#include <xml/plugins/parserinitparams.h>

#include "t_testconstants.h"

using namespace Xml;

class Tes1tXmlParser : public CBase, public MParser
	{
public:

	static MParser* NewL(TAny* aInitParams);
	virtual ~Tes1tXmlParser();

	// From MParser

	TInt EnableFeature(TInt /*aParserFeature*/)
		{
		return KErrNotSupported;
		}
	TInt DisableFeature(TInt /*aParserFeature*/)
		{
		return KErrNone;
		}
	TBool IsFeatureEnabled(TInt /*aParserFeature*/) const
		{
		return EFalse;
		}
	void Release();
	void ParseChunkL (const TDesC8& aDescriptor);
	void ParseLastChunkL(const TDesC8& aDescriptor);

	// From MContentSouce
	
	void SetContentSink (MContentHandler& aContentHandler);

	RStringPool& StringPool();

private:

	Tes1tXmlParser(TParserInitParams* aInitParams);
	void ConstructL();

	void DoParseL();

	inline void OnStartDocumentL();
	inline void OnEndDocumentL();
	inline void OnStartElementL();
	inline void OnEndElementL();
	inline void OnContentL();
	inline void OnStartPrefixMappingL();
	inline void OnEndPrefixMappingL();
	inline void OnIgnorableWhiteSpaceL();
	inline void OnSkippedEntityL();
	inline void OnProcessingInstructionL();
	inline void OnExtensionL();
	inline void OnError(TInt aError);

private:
	MContentHandler*	iContentHandler;
	RStringDictionaryCollection*		iStringDictionaryCollection;
	CCharSetConverter* iCharSetConverter;
	RElementStack* iElementStack;
	};

MParser* Tes1tXmlParser::NewL(TAny* aInitParams)
	{

	Tes1tXmlParser* self = new(ELeave) Tes1tXmlParser(reinterpret_cast<TParserInitParams*>(aInitParams));
	
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);

	return (static_cast<MParser*>(self));
	}



Tes1tXmlParser::Tes1tXmlParser(TParserInitParams* aInitParams)
:	iContentHandler (reinterpret_cast<MContentHandler*>(aInitParams->iContentHandler)),
	iStringDictionaryCollection (reinterpret_cast<RStringDictionaryCollection*>(aInitParams->iStringDictionaryCollection)),
	iCharSetConverter (reinterpret_cast<CCharSetConverter*>(aInitParams->iCharSetConverter)),
	iElementStack (reinterpret_cast<RElementStack*>(aInitParams->iElementStack))
	{
	}



void Tes1tXmlParser::ConstructL()
	{
	// do nothing;   
	}



void Tes1tXmlParser::Release()
	{
	delete (this);
	}



Tes1tXmlParser::~Tes1tXmlParser()
	{
	// We don't own this
	iContentHandler = NULL;
	iStringDictionaryCollection = NULL;
	iCharSetConverter = NULL;
	iElementStack = NULL;
	}


void Tes1tXmlParser::ParseChunkL (const TDesC8& /*aDescriptor*/)
	{
	DoParseL();
	}
	
	
void Tes1tXmlParser::ParseLastChunkL(const TDesC8& /*aDescriptor*/)
	{
	DoParseL();
	}


RStringPool& Tes1tXmlParser::StringPool()
	{
	return iStringDictionaryCollection->StringPool();
	}
	


void Tes1tXmlParser::SetContentSink (MContentHandler& aContentHandler)
/**
This method allows for the correct streaming of data to another plugin in the chain.

@post				the next plugin in the chain is set to receive our callbacks.

*/
	{
	iContentHandler = &aContentHandler;
	}


void Tes1tXmlParser::DoParseL()
/**
This method is called when the request to parse a document is issued. 
For testing purposes this method always generates one element event, 
so the test suite can recognize the plugin parser it is using. 

*/
	{
	OnStartDocumentL();
	OnStartElementL();
	OnEndElementL();
	OnEndDocumentL();	
	}



void Tes1tXmlParser::OnStartDocumentL()
	{
	RDocumentParameters documentParameters;

	iContentHandler->OnStartDocumentL(documentParameters, KErrorCodeOnStartDocument);
	}


void Tes1tXmlParser::OnEndDocumentL()
	{
	iContentHandler->OnEndDocumentL(KErrorCodeOnEndDocument);
	}



void Tes1tXmlParser::OnStartElementL()
	{
	RTagInfo element;
	RAttributeArray attributes;

	iContentHandler->OnStartElementL(element, attributes, KErrorCodeOnStartElement);
	}
	

void Tes1tXmlParser::OnEndElementL()
	{
	RTagInfo element;

	iContentHandler->OnEndElementL(element, KErrorCodeOnEndElement);
	}



void Tes1tXmlParser::OnContentL()
	{
	const TBuf8<2> bytes;

	iContentHandler->OnContentL(bytes, KErrorCodeOnContent);
	}



void Tes1tXmlParser::OnStartPrefixMappingL()
	{
	RString prefix;
	RString uri;

	iContentHandler->OnStartPrefixMappingL(prefix, uri, KErrorCodeOnStartPrefixMapping);
	}


void Tes1tXmlParser::OnEndPrefixMappingL()
	{
	RString prefix;

	iContentHandler->OnEndPrefixMappingL(prefix, KErrorCodeOnEndPrefixMapping);
	}



void Tes1tXmlParser::OnIgnorableWhiteSpaceL()
	{
	const TBuf8<2> bytes;

	iContentHandler->OnIgnorableWhiteSpaceL(bytes, KErrorCodeOnIgnorableWhiteSpace);
	}



void Tes1tXmlParser::OnSkippedEntityL()
	{
	RString name;

	iContentHandler->OnSkippedEntityL(name, KErrorCodeOnSkippedEntity);
	}



void Tes1tXmlParser::OnProcessingInstructionL()
	{
	TBuf8<2> target;
	TBuf8<2> data;

	iContentHandler->OnProcessingInstructionL(target, data, KErrorCodeOnProcessingInstruction);
	}


void Tes1tXmlParser::OnExtensionL()
	{
	RString data;
	TInt token = 0;

	MWbxmlExtensionHandler* ptr = 
		static_cast<MWbxmlExtensionHandler*>
			(iContentHandler->GetExtendedInterface(MWbxmlExtensionHandler::EExtInterfaceUid));

	if (!ptr)
		{
		User::Leave(KErrXmlUnsupportedExtInterface);
		}
		
	ptr->OnExtensionL(data, token, KErrorCodeOnExtension);	
	}


void Tes1tXmlParser::OnError(TInt aError)
	{
	iContentHandler->OnError(aError);
	}


// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = {
	IMPLEMENTATION_PROXY_ENTRY(0x10273864,Tes1tXmlParser::NewL),
	IMPLEMENTATION_PROXY_ENTRY(0x101F9782,Tes1tXmlParser::NewL),
	IMPLEMENTATION_PROXY_ENTRY(0x101faa00,Tes1tXmlParser::NewL),	
	IMPLEMENTATION_PROXY_ENTRY(0x101faa01,Tes1tXmlParser::NewL)	
};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

