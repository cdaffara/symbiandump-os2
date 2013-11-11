// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// SAX libxml2 parser plugin
//

#include "xmlengsaxplugin.h"
#include "xmlengsaxpluginerror.h"
#include "libxml2_globals_private.h"

MParser* CXMLEngineSAXPlugin::NewL(TAny* aInitParams)
	{

	CXMLEngineSAXPlugin* self = new(ELeave) 
		CXMLEngineSAXPlugin(reinterpret_cast<TParserInitParams*>(aInitParams));
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);

	return (static_cast<MParser*>(self));
	}
	
	
CXMLEngineSAXPlugin::CXMLEngineSAXPlugin(TParserInitParams* aInitParams)
:	iContentHandler (reinterpret_cast<MContentHandler*>(aInitParams->iContentHandler)),
	iStringDictionaryCollection (reinterpret_cast<RStringDictionaryCollection*>
											(aInitParams->iStringDictionaryCollection)),
	iCharSetConverter (reinterpret_cast<CCharSetConverter*>(aInitParams->iCharSetConverter)),
	iElementStack (reinterpret_cast<RElementStack*>(aInitParams->iElementStack))
	{
	}	


void CXMLEngineSAXPlugin::ConstructL() 
	{

	//settings flag as EFalse
	Flag_Error_Status = EFalse;
		
	//setting the callbacks for LibXML2 sax parser
	iParserEvents.externalSubset = ExternalSubset;
	iParserEvents.entityDecl = EntityDeclaration;		
	iParserEvents.getEntity = GetEntityCal;
	iParserEvents.resolveEntity = ResolveEntity;	
	iParserEvents.startDocument = StartDocument;
	iParserEvents.endDocument = EndDocument;		
	iParserEvents.characters = Characters;
	iParserEvents.startPrefixMapping = StartPrefixMapping;		
	iParserEvents.endPrefixMapping = EndPrefixMapping;
	iParserEvents.initialized = XML_SAX2_MAGIC;
	iParserEvents.startElementNs = StartElementNs;
	iParserEvents.endElementNs = EndElementNs;
	iParserEvents.ignorableWhitespace = IgnorableWhitespace;		
	iParserEvents.processingInstruction = ProcessingInstruction;
	iParserEvents.serror = StructuredError;
	iParserEvents.getParameterEntity = GetParameterEntity;
		
	//checking if content handler or dictionary are null
	User::LeaveIfNull(iContentHandler);  		
	User::LeaveIfNull(iStringDictionaryCollection);
	
	iStringPool = iStringDictionaryCollection->StringPool();
	XmlEngineAttachL();	
	}

CXMLEngineSAXPlugin::~CXMLEngineSAXPlugin()
    {
    XmlEngineCleanup(iParserContext);    	
    iParserContext = NULL;
        
    //releasing rest of objects
    iStringDictionaryCollection = NULL;
	iContentHandler = NULL;
	iCharSetConverter = NULL;
	iElementStack = NULL;
	
	//releasing entity object
	iEntity.name = NULL;
	iEntity.orig = NULL;
	iEntity.content = NULL;
	}


//From MParser
		
/** 
 * Enable a feature. 
 */
TInt CXMLEngineSAXPlugin::EnableFeature(TInt aParserFeature)
	{
	if(aParserFeature & ~(KFeatures))
		return KErrNotSupported;

	iParserMode |= aParserFeature;

	return KErrNone;
	}
		 
/** 
 * Disable a feature. 
 */
TInt CXMLEngineSAXPlugin::DisableFeature(TInt aParserFeature)
	{
	if(aParserFeature & ~(KFeatures))
		return KErrNotSupported;

	iParserMode &= ~aParserFeature;

	return KErrNone;
	}	

/** 
 * See if a feature is enabled. 
 */	
TBool CXMLEngineSAXPlugin::IsFeatureEnabled(TInt aParserFeature) const
	{
	return iParserMode & aParserFeature;
	}

void CXMLEngineSAXPlugin::StopParsing(TInt aError)
    {
    if(iParserContext)
        {
        xmlStopParser(iParserContext);
        iParserContext->errNo = aError;    
        if ( aError == KErrNoMemory ) 
            {
            xmlSetOOM();
            }
        }    
    }

/** 
 * Parses a descriptor that contains part of a document. 
 */
void CXMLEngineSAXPlugin::ParseChunkL(const TDesC8& aDescriptor)
	{	
	
	TInt result = KErrNone;
	
	if (!iParserContext)
		{		
		//creating sax parser object
	    iParserContext = xmlCreatePushParserCtxt( 
   						(xmlSAXHandler*) &iParserEvents, 
   						this, 
  						(const char *) aDescriptor.Ptr(), 
						aDescriptor.Length(),
   						NULL
   						);
		if(!iParserContext)
			{
			CleanupAndLeaveL(KErrNoMemory);			
			}
		
		//creating empty document object
		iParserContext->myDoc = xmlNewDoc(BAD_CAST "SAX compatibility mode document");
		if(!iParserContext->myDoc)
			{
			CleanupAndLeaveL(KErrNoMemory);			
			}
		iParserContext->myDoc->intSubset = xmlNewDtd(iParserContext->myDoc, BAD_CAST "fake", NULL, NULL);
		if(!iParserContext->myDoc->intSubset)
			{
			CleanupAndLeaveL(KErrNoMemory);			
			}
		
		//parsing process
	    result = xmlParseChunk(iParserContext, NULL, 0, 0);					
		}
	else
		{
		//parsing process
		result = xmlParseChunk(
				iParserContext, 
				(const char *) aDescriptor.Ptr(), 
				aDescriptor.Length(),
				0);		
		}
					
	//handling error situation
	//if fatal error, function throws exception 
	//in any other case next chunk is taken (recoverable)
	if ((result == XML_ERR_NO_MEMORY) || (iParserContext->lastError.level == XML_ERR_FATAL)
	        || (result < 0))
		{
		CleanupAndLeaveL(GetErrorNum(result));	
		}
	}
	
/** 
 * Parses a descriptor that contains the last  part of a document. 
 */
void CXMLEngineSAXPlugin::ParseLastChunkL(const TDesC8& /*aDescriptor*/)
	{
	if (!iParserContext)
		{
		User::Leave(EXmlParserError);
		}

	//parsing process
	TInt result(KErrNone);
	result = xmlParseChunk(iParserContext, NULL, 0, 1);
	if ((result == XML_ERR_NO_MEMORY) || (iParserContext->lastError.level == XML_ERR_FATAL)
	        || (result < 0))
		{
		CleanupAndLeaveL(GetErrorNum(result));			
		}			
		
	//releasing context to the parser
    xmlParserCtxtPtr ctxt = reinterpret_cast<xmlParserCtxtPtr>(iParserContext);
    if(ctxt->myDoc)                     
       {
       xmlFreeDoc(ctxt->myDoc); 
       }
    xmlFreeParserCtxt(ctxt);
	iParserContext = NULL;	
	
	}

/** 
 * Interfaces don't have a destructor, so we have an explicit method instead. 
 */
void CXMLEngineSAXPlugin::Release()
	{
	delete this;
	}

void CXMLEngineSAXPlugin::CleanupAndLeaveL(TInt aError)
	{
	// reseting OOM flag
	if((aError == KErrNoMemory) && xmlOOMFlag())
	    {
	    xmlResetOOM();
	    }

	//releasing context to the parser
	if(iParserContext)
	    {	    
        xmlParserCtxtPtr ctxt = reinterpret_cast<xmlParserCtxtPtr>(iParserContext);
        if(ctxt->myDoc)                     
            {
            xmlFreeDoc(ctxt->myDoc); 
            }
        xmlFreeParserCtxt(ctxt);		
	    }
    iParserContext = NULL;
	
	if (aError)
		{
		User::Leave(aError);			
		}
	else
		{
		User::Leave(EXmlParserError);
		}
	}

// From MContentSouce
	
/** 
 * This method tells us what's the next content handler in the chain. 
 */			
void CXMLEngineSAXPlugin::SetContentSink (MContentHandler& aContentHandler)
	{
	iContentHandler = &aContentHandler;
	}


//Public getter and setter method
		
/**
 * This method returns a parser context object. 
 */	
xmlParserCtxtPtr CXMLEngineSAXPlugin::getParserContext() const
	{
	return iParserContext;
	}
	
xmlEntity& CXMLEngineSAXPlugin::GetEntity()
	{
	return iEntity;
	}


// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = {
	IMPLEMENTATION_PROXY_ENTRY(0x101f9783, CXMLEngineSAXPlugin::NewL)
	};


EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
	
