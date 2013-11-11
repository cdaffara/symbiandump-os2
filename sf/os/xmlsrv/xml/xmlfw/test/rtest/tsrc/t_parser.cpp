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
#include <xml/wbxmlextensionhandler.h>
#include <xml/stringdictionarycollection.h>
#include <xml/taginfo.h>
#include <xml/attribute.h>
#include <xml/documentparameters.h>
#include <xml/contenthandler.h>
#include <xml/plugins/parserinterface.h>
#include <xml/plugins/parserinitparams.h>

#include "t_testconstants.h"

using namespace Xml;

class CDummyParser : public CBase, public MParser
	{
public:

	static MParser* NewL(TAny* aInitParams);
	virtual ~CDummyParser();

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

	CDummyParser(TParserInitParams* aInitParams);
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

MParser* CDummyParser::NewL(TAny* aInitParams)
	{

	CDummyParser* self = new(ELeave) CDummyParser(reinterpret_cast<TParserInitParams*>(aInitParams));
	return (static_cast<MParser*>(self));
	}



CDummyParser::CDummyParser(TParserInitParams* aInitParams)
:	iContentHandler (reinterpret_cast<MContentHandler*>(aInitParams->iContentHandler)),
	iStringDictionaryCollection (reinterpret_cast<RStringDictionaryCollection*>(aInitParams->iStringDictionaryCollection)),
	iCharSetConverter (reinterpret_cast<CCharSetConverter*>(aInitParams->iCharSetConverter)),
	iElementStack (reinterpret_cast<RElementStack*>(aInitParams->iElementStack))
	{
	}


void CDummyParser::Release()
	{
	delete (this);
	}



CDummyParser::~CDummyParser()
	{
	// We don't own this
	iContentHandler = NULL;
	iStringDictionaryCollection = NULL;
	iCharSetConverter = NULL;
	iElementStack = NULL;
	}


void CDummyParser::ParseChunkL (const TDesC8& /*aDescriptor*/)
	{
	DoParseL();
	}
	
	
void CDummyParser::ParseLastChunkL(const TDesC8& /*aDescriptor*/)
	{
	DoParseL();
	}


RStringPool& CDummyParser::StringPool()
	{
	return iStringDictionaryCollection->StringPool();
	}
	


void CDummyParser::SetContentSink (MContentHandler& aContentHandler)
/**
This method allows for the correct streaming of data to another plugin in the chain.

@post				the next plugin in the chain is set to receive our callbacks.

*/
	{
	iContentHandler = &aContentHandler;
	}



void CDummyParser::DoParseL()
	{
	OnStartDocumentL();
	OnEndDocumentL();
	OnStartElementL();
	OnEndElementL();
	OnContentL();
	OnStartPrefixMappingL();
	OnEndPrefixMappingL();
	OnIgnorableWhiteSpaceL();
	OnSkippedEntityL();
	OnProcessingInstructionL();
	OnExtensionL();
	//OnError(KErrEof);
	}



void CDummyParser::OnStartDocumentL()
	{
	RDocumentParameters documentParameters;

	iContentHandler->OnStartDocumentL(documentParameters, KErrorCodeOnStartDocument);
	}


void CDummyParser::OnEndDocumentL()
	{
	iContentHandler->OnEndDocumentL(KErrorCodeOnEndDocument);
	}



void CDummyParser::OnStartElementL()
	{
	RTagInfo element;
	RAttributeArray attributes;

	iContentHandler->OnStartElementL(element, attributes, KErrorCodeOnStartElement);
	}
	

void CDummyParser::OnEndElementL()
	{
	RTagInfo element;

	iContentHandler->OnEndElementL(element, KErrorCodeOnEndElement);
	}



void CDummyParser::OnContentL()
	{
	const TBuf8<2> bytes;

	iContentHandler->OnContentL(bytes, KErrorCodeOnContent);
	}



void CDummyParser::OnStartPrefixMappingL()
	{
	RString prefix;
	RString uri;

	iContentHandler->OnStartPrefixMappingL(prefix, uri, KErrorCodeOnStartPrefixMapping);
	}


void CDummyParser::OnEndPrefixMappingL()
	{
	RString prefix;

	iContentHandler->OnEndPrefixMappingL(prefix, KErrorCodeOnEndPrefixMapping);
	}



void CDummyParser::OnIgnorableWhiteSpaceL()
	{
	const TBuf8<2> bytes;

	iContentHandler->OnIgnorableWhiteSpaceL(bytes, KErrorCodeOnIgnorableWhiteSpace);
	}



void CDummyParser::OnSkippedEntityL()
	{
	RString name;

	iContentHandler->OnSkippedEntityL(name, KErrorCodeOnSkippedEntity);
	}



void CDummyParser::OnProcessingInstructionL()
	{
	TBuf8<2> target;
	TBuf8<2> data;

	iContentHandler->OnProcessingInstructionL(target, data, KErrorCodeOnProcessingInstruction);
	}


void CDummyParser::OnExtensionL()
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


void CDummyParser::OnError(TInt aError)
	{
	iContentHandler->OnError(aError);
	}


// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = {
	IMPLEMENTATION_PROXY_ENTRY(0x101FBE53,CDummyParser::NewL)
};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

