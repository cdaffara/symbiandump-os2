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
//

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

class CXmlParser : public CBase, public MParser
	{
public:

	static MParser* NewL(TAny* aInitParams);
	virtual ~CXmlParser();

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

	CXmlParser(TParserInitParams* aInitParams);
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

MParser* CXmlParser::NewL(TAny* aInitParams)
	{

	CXmlParser* self = new(ELeave) CXmlParser(reinterpret_cast<TParserInitParams*>(aInitParams));
	
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);

	return (static_cast<MParser*>(self));
	}



CXmlParser::CXmlParser(TParserInitParams* aInitParams)
:	iContentHandler (reinterpret_cast<MContentHandler*>(aInitParams->iContentHandler)),
	iStringDictionaryCollection (reinterpret_cast<RStringDictionaryCollection*>(aInitParams->iStringDictionaryCollection)),
	iCharSetConverter (reinterpret_cast<CCharSetConverter*>(aInitParams->iCharSetConverter)),
	iElementStack (reinterpret_cast<RElementStack*>(aInitParams->iElementStack))
	{
	}



void CXmlParser::ConstructL()
	{
	// do nothing;   
	}



void CXmlParser::Release()
	{
	delete (this);
	}



CXmlParser::~CXmlParser()
	{
	// We don't own this
	iContentHandler = NULL;
	iStringDictionaryCollection = NULL;
	iCharSetConverter = NULL;
	iElementStack = NULL;
	}


void CXmlParser::ParseChunkL (const TDesC8& /*aDescriptor*/)
	{
	DoParseL();
	}
	
	
void CXmlParser::ParseLastChunkL(const TDesC8& /*aDescriptor*/)
	{
	DoParseL();
	}


RStringPool& CXmlParser::StringPool()
	{
	return iStringDictionaryCollection->StringPool();
	}
	


void CXmlParser::SetContentSink (MContentHandler& aContentHandler)
/**
This method allows for the correct streaming of data to another plugin in the chain.

@post				the next plugin in the chain is set to receive our callbacks.

*/
	{
	iContentHandler = &aContentHandler;
	}


void CXmlParser::DoParseL()
/**
This method gets executed when a request to parse a file is issued. 
This function generates just one element event. 
*/
	{
	OnStartDocumentL();
	OnStartElementL();
	OnEndElementL();
	OnEndDocumentL();	
	}



void CXmlParser::OnStartDocumentL()
	{
	RDocumentParameters documentParameters;

	iContentHandler->OnStartDocumentL(documentParameters, KErrorCodeOnStartDocument);
	}


void CXmlParser::OnEndDocumentL()
	{
	iContentHandler->OnEndDocumentL(KErrorCodeOnEndDocument);
	}



void CXmlParser::OnStartElementL()
	{
	RTagInfo element;
	RAttributeArray attributes;

	iContentHandler->OnStartElementL(element, attributes, KErrorCodeOnStartElement);
	}
	

void CXmlParser::OnEndElementL()
	{
	RTagInfo element;

	iContentHandler->OnEndElementL(element, KErrorCodeOnEndElement);
	}



void CXmlParser::OnContentL()
	{
	const TBuf8<2> bytes;

	iContentHandler->OnContentL(bytes, KErrorCodeOnContent);
	}



void CXmlParser::OnStartPrefixMappingL()
	{
	RString prefix;
	RString uri;

	iContentHandler->OnStartPrefixMappingL(prefix, uri, KErrorCodeOnStartPrefixMapping);
	}


void CXmlParser::OnEndPrefixMappingL()
	{
	RString prefix;

	iContentHandler->OnEndPrefixMappingL(prefix, KErrorCodeOnEndPrefixMapping);
	}



void CXmlParser::OnIgnorableWhiteSpaceL()
	{
	const TBuf8<2> bytes;

	iContentHandler->OnIgnorableWhiteSpaceL(bytes, KErrorCodeOnIgnorableWhiteSpace);
	}



void CXmlParser::OnSkippedEntityL()
	{
	RString name;

	iContentHandler->OnSkippedEntityL(name, KErrorCodeOnSkippedEntity);
	}



void CXmlParser::OnProcessingInstructionL()
	{
	TBuf8<2> target;
	TBuf8<2> data;

	iContentHandler->OnProcessingInstructionL(target, data, KErrorCodeOnProcessingInstruction);
	}


void CXmlParser::OnExtensionL()
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


void CXmlParser::OnError(TInt aError)
	{
	iContentHandler->OnError(aError);
	}


// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = {
	IMPLEMENTATION_PROXY_ENTRY(0x10273875,CXmlParser::NewL),
	IMPLEMENTATION_PROXY_ENTRY(0x10273876,CXmlParser::NewL),
	IMPLEMENTATION_PROXY_ENTRY(0x10273877,CXmlParser::NewL),
	IMPLEMENTATION_PROXY_ENTRY(0x10273878,CXmlParser::NewL),
	IMPLEMENTATION_PROXY_ENTRY(0x10273879,CXmlParser::NewL),
};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
