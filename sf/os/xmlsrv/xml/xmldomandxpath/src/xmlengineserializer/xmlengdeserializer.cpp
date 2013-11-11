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
// XML Deserializer implementation
//

#include <xml/dom/xmlengdeserializer.h>
#include <xml/dom/xmlengerrors.h>
#include "xmlengdeserializerxop.h"
#include "xmlengdeserializergzip.h"
#include <xml/utils/xmlengxestd.h>
#include <xml/utils/xmlengxestrings.h>
#include <xml/dom/xmlengdatacontainer.h>
#include <xml/dom/xmlengparsingoptions.h>
#include <xml/dom/xmlengserializeerrors.h>

#include <xml/parser.h>
#include <xml/parserfeature.h>
#include <xml/matchdata.h>
#include <f32file.h>

_LIT8(KXmlMimeType, "text/xml");
_LIT8(KParserType,"libxml2");

using namespace Xml;

// --------------------------------------------------------------------------------------
// Creates an instance of CXmlEngDeserializer
// --------------------------------------------------------------------------------------
//
EXPORT_C CXmlEngDeserializer* CXmlEngDeserializer::NewL( MContentHandler& aContentHandler,	TXmlEngDeserializerType aType ) 															
    {
	switch(aType)
		{
		case EDeserializerXOP:
			{
			return CXmlEngDeserializerXOP::NewL( aContentHandler, EFalse );			
			}
		case EDeserializerXOPInfoset:
			{
			return CXmlEngDeserializerXOP::NewL( aContentHandler, ETrue );			
			}			
		case EDeserializerGZip:
			{
			return CXmlEngDeserializerGZIP::NewL( aContentHandler );
			}
		case EDeserializerDefault:		
		default:
		    {
	        CXmlEngDeserializer* self = new (ELeave) CXmlEngDeserializer; 
	        CleanupStack::PushL(self);
	        self->ConstructL( aContentHandler );
	        CleanupStack::Pop(self);
	        return self;
		    }
        }
    }


// --------------------------------------------------------------------------------------
// Set input type as file
// --------------------------------------------------------------------------------------
//
EXPORT_C void CXmlEngDeserializer::SetInputFileL(const TDesC& aFileName)
	{
	if(iInputFileName)
	    {
	    delete iInputFileName;
	    iInputFileName=NULL;
	    }
	iInputFileName = aFileName.AllocL();
	iSerializationOutput = EDeserializeFromFile;
	}

// --------------------------------------------------------------------------------------
// Set input type as buffer
// --------------------------------------------------------------------------------------
//
EXPORT_C void CXmlEngDeserializer::SetInputBuffer(const TDesC8& aBuffer)
	{
	iBuffer.Set(aBuffer);
	iSerializationOutput = EDeserializeFromBuffer;
	}

// --------------------------------------------------------------------------------------
// Set content handler
// --------------------------------------------------------------------------------------
//
EXPORT_C void CXmlEngDeserializer::SetContentHandlerL(MContentHandler& aContentHandler)
	{
	TInt i;
   	delete iParser;
   	iParser = NULL;
   	ConstructL( aContentHandler );
   	for(i=1; i<=ELastFeature; i*=2)
   		{
   		if(i & iFeatures)
   			{
   			iParser->EnableFeature(i);
   			}   			
   		}
	}	

// --------------------------------------------------------------------------------------
// Set parsing options
// May be used to alter deserializer output
// --------------------------------------------------------------------------------------
//	
EXPORT_C void CXmlEngDeserializer::SetParsingOptions(TXmlEngParsingOptions& aOptions)
	{
	iParsingOptions = aOptions;
	}

// --------------------------------------------------------------------------------------
// Set parsing options
// May be used to alter deserializer output
// --------------------------------------------------------------------------------------
//	
EXPORT_C const TXmlEngParsingOptions& CXmlEngDeserializer::ParsingOptions()
	{
	return iParsingOptions;
	}
	
// --------------------------------------------------------------------------------------
// Set external data array
// --------------------------------------------------------------------------------------
//
EXPORT_C void CXmlEngDeserializer::UseExternalDataL(RArray<TXmlEngDataContainer>& aList)
	{
	iDataList = &aList;
	}

// --------------------------------------------------------------------------------------
// Fetches external data registered in this deserializer
// --------------------------------------------------------------------------------------
//
EXPORT_C RArray<TXmlEngDataContainer>* CXmlEngDeserializer::ExternalData()
	{
	return iDataList;
	}

// --------------------------------------------------------------------------------------
// Enables CParser feature
// --------------------------------------------------------------------------------------
//
EXPORT_C TInt CXmlEngDeserializer::EnableFeature(TInt aParserFeature)
	{
	TInt res = iParser->EnableFeature(aParserFeature);
	if(!res)
		{
		iFeatures |= aParserFeature;	
		}
	return res; 
	}
	
// --------------------------------------------------------------------------------------
// Disables CParser feature
// --------------------------------------------------------------------------------------
//
EXPORT_C TInt CXmlEngDeserializer::DisableFeature(TInt aParserFeature)
	{
	TInt res = iParser->DisableFeature(aParserFeature);
	if(!res)
		{
		iFeatures &= ~aParserFeature;	
		}
	return res;	
	}

// --------------------------------------------------------------------------------------
// Checks if a feature is enabled
// --------------------------------------------------------------------------------------
//	
EXPORT_C TBool CXmlEngDeserializer::IsFeatureEnabled(TInt aParserFeature) const
	{
	return iParser->IsFeatureEnabled(aParserFeature);
	}
  
// --------------------------------------------------------------------------------------
// Serializes TXmlEngNode to file
// --------------------------------------------------------------------------------------
//
EXPORT_C void CXmlEngDeserializer::DeserializeL()
    {
	switch(iSerializationOutput)
		{
		case EDeserializeFromFile:
			{
			if(!iInputFileName)
				{
				User::Leave(KXmlEngErrNoParameters);
				}
		    RFs fs; 
		    CleanupClosePushL(fs);
		    User::LeaveIfError(fs.Connect());    				
		    DeserializeL( fs, iInputFileName->Des() );			
			CleanupStack::PopAndDestroy(&fs);
			break;
			}
		case EDeserializeFromBuffer:
			{
			if(iBuffer.Length() == 0)
				{
				User::Leave(KXmlEngErrNoParameters);
				}
			DeserializeL(iBuffer);			
			break;
			}
		case EDeserializeFromStream:
			{
			// Currently this feature is not supported. Implement when need arises or remove.
			User::Leave(KErrNotSupported);
			}			
		default:
			{
			User::Leave(KErrNotSupported);
			break;
			}						
		}   
    }

// --------------------------------------------------------------------------------------
// Deserialize document
// --------------------------------------------------------------------------------------
//
EXPORT_C void CXmlEngDeserializer::DeserializeL( const TDesC& aFileName,
											const TXmlEngParsingOptions& /*aOptions*/) 
    {
	RFs fs; 
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());    				    
	ParseL(*iParser,fs,aFileName);			
	CleanupStack::PopAndDestroy(&fs);	
	}

// --------------------------------------------------------------------------------------
// Deserialize document
// --------------------------------------------------------------------------------------
//
EXPORT_C void CXmlEngDeserializer::DeserializeL( RFs& aRFs, 
											const TDesC& aFileName,
											const TXmlEngParsingOptions& /*aOptions*/) 
    {
	ParseL(*iParser,aRFs,aFileName);	
	}

// --------------------------------------------------------------------------------------
// Deserialize document
// --------------------------------------------------------------------------------------
//
EXPORT_C void CXmlEngDeserializer::DeserializeL( const TDesC8& aBuffer,
											const TXmlEngParsingOptions& /*aOptions*/) 
    {
  	ParseL(*iParser,aBuffer);
    }        
// --------------------------------------------------------------------------------------
// Default Constructor
// --------------------------------------------------------------------------------------
//
CXmlEngDeserializer::CXmlEngDeserializer()
    { 
	iParsingOptions = TXmlEngParsingOptions();           
    }
    
// --------------------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------------------
//
CXmlEngDeserializer::~CXmlEngDeserializer()
    {
   	delete iParser; 
   	delete iInputFileName;  
    }

// --------------------------------------------------------------------------------------
// 2nd phase constructor
// --------------------------------------------------------------------------------------
//
void CXmlEngDeserializer::ConstructL( MContentHandler& aContentHandler )
    {
    if(iParser)
    	{
    	delete iParser;
    	iParser = NULL;
    	}

    CMatchData* matchData = CMatchData::NewLC();
    matchData->SetMimeTypeL(KXmlMimeType);
    matchData->SetVariantL(KParserType); 
    iParser = CParser::NewL(*matchData, aContentHandler);
    CleanupStack::PopAndDestroy(&(*matchData));
    }


// --------------------------------------------------------------------------------------
// Retrieves pointer to data container referenced by CID 
// --------------------------------------------------------------------------------------
//
TXmlEngDataContainer* CXmlEngDeserializer::GetDataContainer(const TDesC8& aCid)
	{
	for(TInt i = 0; i < iDataList->Count(); i++)
		{
		if( (*iDataList)[i].Cid() == aCid )
			{
			return &(*iDataList)[i];
			}
		}	
	return NULL;
	}

