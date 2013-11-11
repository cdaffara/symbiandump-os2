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

#include <ecom/ecom.h>

#include <xml/contentprocessor.h>
#include <xml/contenthandler.h>
#include <xml/matchdata.h>
#include <xml/stringdictionarycollection.h>
#include <xml/xmlframeworkconstants.h>
#include <xml/xmlframeworkerrors.h>
#include "XmlFrameworkPanics.h"
#include <xml/plugins/charsetconverter.h>
#include <xml/plugins/contentprocessorinitparams.h>
#include <xml/plugins/parserinitparams.h>
#include <xml/plugins/parserinterface.h>

#include "parserimpl.h"

/** 
Indicates the custom resolver implementation for XML parser framework. 

@internalComponent
*/
const TUid KCustomResolverUid			= {0x10273862};

using namespace Xml;

void TParserImpl::DestroyContentProcessorPtrMapping(TAny* aPtr)
/**
Tidy memory associated with this object.

@param				aPtr The ContentProcessor pointer.

*/
	{
	TDtorContentProcessorPtrMapping* ptr = static_cast<TDtorContentProcessorPtrMapping*>(aPtr);
	 
	if (ptr->iDtorKey != KNullUid && ptr->iContentProcessor)
		{
		// Tell ECom to destroy its plugin details
		REComSession::DestroyedImplementation(ptr->iDtorKey);

		// Free memory
		ptr->iContentProcessor->Release();
		}

	delete (ptr);
	}



TParserImpl::TParserImpl()
/**
Default constructor

@post				This object is properly constructed.

*/
:	iParser(NULL),
	iCharSetConverter(NULL),
	iStringDictionaryCollection(NULL)
	{
	
	}



void TParserImpl::OpenL(const CMatchData& aCriteria, MContentHandler& aCallback) 
/**
This method opens and sets all the objects contents.
The StringDictionaryCollection is created and Opened.

@pre				The object has just been constructed and no other values have been set.
@post				The objects members have been set to the values given.

@param				aCriteria Detailed criteria for parser resolution.
@param				aCallback client to pass data to.

*/
	{
	Close();

	if (!iCharSetConverter)
		iCharSetConverter = CCharSetConverter::NewL();

	if (!iStringDictionaryCollection)
		{
		iStringDictionaryCollection = new(ELeave) RStringDictionaryCollection();
		iStringDictionaryCollection->OpenL();
		}
	
	//verify mime type 
	if (aCriteria.MimeType() != KNullDesC8)
		{
		// Create and remember the current mime type
		iCurrentParserMatchData = aCriteria;
		}
	else
		User::Leave(KErrArgument);

	iDefParserMatchData = aCriteria;
	iParser = ConstructParserL(aCriteria, aCallback);
	iClient = &aCallback;
	}


void TParserImpl::Close()
/**
This method cleans up the object before destruction. It releases all resources in
accordance to the R Class pattern.

The framework will Close the StringDictionaryCollection and if owned will
delete it.

@post				This object may be allowed to go out of scope.

*/
	{
	// Close any remaining RStrings owned by iElementStack. This will prevent a
	// memory leak from occuring when parsing a document that is not complete AND
	// ParseEndL is NOT called before destroying the CParser object.
	for (TInt i=0;i<iElementStack.Count();i++)
		iElementStack[i].Close();
	
	iElementStack.Close();

	// Destroy the chain of plugins
	DestroyChain();
	
	if (iParser)
		{
		// Delete the Parser
		REComSession::DestroyedImplementation(iParserDtorKey);

		iParser->Release();
		iParser = NULL;
		}

	delete iCharSetConverter;
	iCharSetConverter = NULL;
			
	if (iStringDictionaryCollection)
		{
		iStringDictionaryCollection->Close();
		delete iStringDictionaryCollection;
		iStringDictionaryCollection = NULL;
		}

	iClient = NULL;

	REComSession::FinalClose();
	}


void TParserImpl::ParseChunkL(const TDesC8& aChunk)
/**
This method starts the parser parsing a descriptor.
The descriptor does not contain the last part of the document. 

@pre				OpenL has been called

@param				aChunk the chunk to parse - not the last chunk

*/
	{
	iParser->ParseChunkL(aChunk);
	}


void TParserImpl::ParseLastChunkL(const TDesC8& aFinalChunk)
/**
This method starts the parser parsing a descriptor.
The descriptor contains the last part of the document. 

@pre				OpenL has been called

@param				aFinalChunk the final chunk to parse

*/
	{		
	iParser->ParseLastChunkL(aFinalChunk);
	}


void TParserImpl::SetContentHandler(MContentHandler& aCallback)
/**
Allows the user to set/change the client for this parse session.

@pre				OpenL has been called

@param				aCallback the client at the end of the callback
					chain that is to receive the parsed document information.

*/
	{
	iClient = &aCallback;
	
	MContentSource* prev;
	if (iDtorKeyAndPluginList.Count())
		{
		// set the last plugin to point to the client
		prev = iDtorKeyAndPluginList[iDtorKeyAndPluginList.Count()-1]->iContentProcessor;
		}
	else
		{
		// set the parser to point to the client as there is no chain
		prev = iParser;
		}
	prev->SetContentSink(*iClient);
	}
	
	
	
void TParserImpl::SetProcessorChainL(const RContentProcessorUids& aPlugins)
/**
Allows the user to change the client and set the plugin chain for 
this parse session.

@pre				OpenL has been called

@param				aCallback the client at the end of the callback
					chain that is to receive the parsed document information.
@param				aPlugins a list of plugin implementation uids that
					make up the callback chain.

*/
	{
	// If there is an existing chain delete all but the parser
	DestroyChain();
	
	// Rebuild the chain.
	MContentHandler* callback = BuildChainL(*iClient, aPlugins);
	
	iParser->SetContentSink(*callback);
	}
	

void TParserImpl::ResetMimeTypeL()
	{
	
	if ( !(iCurrentParserMatchData == iDefParserMatchData) )
		{
		SetMimeTypeL(iDefParserMatchData);
		}
	}
	

void TParserImpl::SetMimeTypeL(const CMatchData& aCriteria)
/**
Allows the user to set/change the parser mime type used for 
this parse session.

@pre				OpenL has been called

@param				aParserMimeType the mime type of the requested parser

*/
	{
	// Check if we are already using this parser
	if (aCriteria == iCurrentParserMatchData)
		{
		// There is no change to the parser. 
		return;
		}

	// Delete the Parser
	if (iParser)
		{
		REComSession::DestroyedImplementation(iParserDtorKey);
		iParser->Release();
		iParser = NULL;
		}

	MContentHandler* next;
	if (iDtorKeyAndPluginList.Count())
		{
		// point the parser to the first plugin in the chain
		next = iDtorKeyAndPluginList[0]->iContentProcessor;
		}
	else
		{
		// point the parser to the client as there is no chain
		next = iClient;
		}
	// Create the new parser and point to the next plugin
	iParser = ConstructParserL(aCriteria, *next);
	
	// Remember what parser we are using.
	iCurrentParserMatchData = aCriteria;
	}
	
	
TInt TParserImpl::EnableFeature(TInt aParserFeature)
	{
	__ASSERT_ALWAYS(iParser, Panic(EXmlFrameworkPanicUnexpectedLogic));

	return iParser->EnableFeature(aParserFeature);
	}

TInt TParserImpl::DisableFeature(TInt aParserFeature)
	{
	__ASSERT_ALWAYS(iParser, Panic(EXmlFrameworkPanicUnexpectedLogic));

	return iParser->DisableFeature(aParserFeature);
	}

TBool TParserImpl::IsFeatureEnabled(TInt aParserFeature) const
	{
	__ASSERT_ALWAYS(iParser, Panic(EXmlFrameworkPanicUnexpectedLogic));

	return iParser->IsFeatureEnabled(aParserFeature);
	}


void TParserImpl::AddPreloadedDictionaryL(const TDesC8& aPublicId)
	{
	iStringDictionaryCollection->OpenDictionaryL(aPublicId);   
	}


RStringPool& TParserImpl::StringPool()
	{
	return iStringDictionaryCollection->StringPool();
	}


RStringDictionaryCollection& TParserImpl::StringDictionaryCollection()
	{
	return *iStringDictionaryCollection;
	}


MContentHandler* TParserImpl::BuildChainL(MContentHandler& aCallback, 
										  const RContentProcessorUids& aPlugins)
/**
This method builds the chain of plugins, so that information may flow down
the chain.

The first in the chain is the parser, the last is the client.

@param				aCallback client to pass data to.
@param				aPlugins the list of plugins in the chain.

@return				the first plugin in the chain. If there is no chain
					the client is returned.

*/
	{
	// Start with the last plugin first and point it to the callback passed.
	// Then work you way up the chain, ending with the Parser calling the first plugin.

	TInt count = aPlugins.Count();
    MContentHandler* callback = &aCallback;

	while (count>0)
		{
		callback = ConstructPluginInReverseL(aPlugins[--count], *callback);
		}

	return (callback);
	}



void TParserImpl::DestroyChain()
/**

Destroys the client and the chain of plugins not including the parser.

@post				The chain and client are destroyed and parsing
					cannot take place.
*/
	{
	TInt count = iDtorKeyAndPluginList.Count();
	while (count)
		{
		DestroyContentProcessorPtrMapping(iDtorKeyAndPluginList[--count]);
		}
	iDtorKeyAndPluginList.Reset();
	}


MParser* TParserImpl::ConstructParserL(const CMatchData& aCriteria, 
							   		   MContentHandler& aCallback)
/**
This method constructs a MParser derived object.

@return				A pointer to the parser found.
@leave				KErrXmlParserPluginNotFound If ECom fails to find 
					the object a leave occurs.
@leave				KErrArgument If CMatchData is lacking mandatory mime type field.

@param				aCriteria Detailed criteria for parser resolution.
@param				aCallback client to pass data to.

*/
	{
	TInt err;
	HBufC8* stream = NULL;
	
	// Check entry criteria
	if (aCriteria.MimeType() == KNullDesC8)
		{
		User::Leave(KErrArgument);
		}
		
	// Set resolving parameters to find a plug-in with a matching parser
	TEComResolverParams resolverParams;
	resolverParams.SetWildcardMatch(ETrue);
	stream = aCriteria.PackToBufferL();
	CleanupStack::PushL(stream);
	resolverParams.SetDataType(*stream);
	
	// Package up the parameters to pass
	TParserInitParams initParams;
	initParams.iCharSetConverter = iCharSetConverter;
	initParams.iContentHandler = &aCallback;
	initParams.iStringDictionaryCollection = iStringDictionaryCollection;
	initParams.iElementStack = &iElementStack;

	// do this for now until I work out what to do with the other params
	TAny* any = NULL;
	TRAP(err, any = REComSession::CreateImplementationL(KParserInterfaceUid,
													iParserDtorKey, 
													(TAny*)&initParams, 
													resolverParams,
													KCustomResolverUid));
	CleanupStack::PopAndDestroy(stream);
	if (err != KErrNone)
		{
		if (err == KErrNotFound)
			{
			User::Leave(KErrXmlParserPluginNotFound);
			}
		User::Leave(err);
		}
	
	MParser* object = static_cast<MParser*>(any);
	return object;
	}
	
MContentProcessor* TParserImpl::ConstructPluginInReverseL(const TUid& aImplementationUid, 
														 MContentHandler& aCallback)
/**
This method constructs a CContentProcessor and places it in reverse order in the chain.

@return				A pointer to the plugin found.
@leave				KErrXmlPluginNotFound If ECom fails to find 
					the object a leave occurs.

@param				aImplementationUid The implementation uid of the plugin.
@param				aCallback client to pass data to.

*/
	{
	TDtorContentProcessorPtrMapping* mapping = new(ELeave) TDtorContentProcessorPtrMapping;
	mapping->iDtorKey = KNullUid;
	mapping->iContentProcessor = NULL;

	CleanupStack::PushL(TCleanupItem(DestroyContentProcessorPtrMapping, mapping));
	
	// Package up the parameters to pass
	TContentProcessorInitParams initParams;
	initParams.iContentHandler = &aCallback;
	initParams.iStringDictionaryCollection = iStringDictionaryCollection;
	initParams.iElementStack = &iElementStack;

	// Client knows specific implementation.
	// Pass the callback object of the next thing in the chain.
	TAny* any = NULL;
	
	TRAPD(err, any = REComSession::CreateImplementationL(aImplementationUid,
														 mapping->iDtorKey,
														 &initParams));
													
	if (err != KErrNone)
		{
		if (err == KErrNotFound)
			{
			User::Leave(KErrXmlPluginNotFound);
			}

		User::Leave(err);
		}
		
	mapping->iContentProcessor = static_cast<MContentProcessor*>(any);

	// We insert at the start as we are building the chain backwards, last till first.
	User::LeaveIfError(iDtorKeyAndPluginList.Insert(mapping, 0));

	CleanupStack::Pop(mapping);
	return (mapping->iContentProcessor); 
	}




