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
// XML XOP Deserializer implementation
//

#include "xmlengdeserializerxop.h"
#include <xml/dom/xmlengerrors.h>
#include "xmlengcontenthandler.h"
#include <xml/utils/xmlengxestd.h>
#include <xml/utils/xmlengxestrings.h>
#include <xml/dom/xmlengserializeerrors.h>

#include <xml/parser.h>
#include <xml/parserfeature.h>
#include <xml/matchdata.h>
#include <bafl/multipartparser.h>
#include <bafl/bodypart.h>
#include <f32file.h>

_LIT8(KXmlMimeType, "text/xml");
_LIT8(KParserType,"libxml2");

// Implemented according to XOP Spec (http://www.w3.org/TR/xop10/)
_LIT8(KMultipartMixed, "multipart/mixed");
_LIT8(KBoundary1, "MIME_boundary"); 
_LIT(KUrlStub, "http://url_stub");  

using namespace Xml;

// --------------------------------------------------------------------------------------
// Creates an instance of CXMLDEDeserializerXOP
// --------------------------------------------------------------------------------------
//
CXmlEngDeserializerXOP* CXmlEngDeserializerXOP::NewL( MContentHandler& aContentHandler,
	 											TBool aCleanXOPInfoset ) 
    {	    
	CXmlEngDeserializerXOP* deserializerXop = new (ELeave) CXmlEngDeserializerXOP(aCleanXOPInfoset);
	CleanupStack::PushL( deserializerXop ); 	
	deserializerXop->ConstructL( aContentHandler );
	CleanupStack::Pop(); //deserializerXop
	return deserializerXop;
    }

// --------------------------------------------------------------------------------------
// Enables CParser feature
// --------------------------------------------------------------------------------------
//
EXPORT_C TInt CXmlEngDeserializerXOP::EnableFeature(TInt aParserFeature)
	{
	if(aParserFeature == EConvertTagsToLowerCase)
		{
		iParserMode |= aParserFeature;
		return KErrNone;		
		}
	return CXmlEngDeserializer::EnableFeature(aParserFeature);
	}
	
// --------------------------------------------------------------------------------------
// Disables CParser feature
// --------------------------------------------------------------------------------------
//
EXPORT_C TInt CXmlEngDeserializerXOP::DisableFeature(TInt aParserFeature)
	{
	if(aParserFeature == EConvertTagsToLowerCase)
		{
		iParserMode &= ~aParserFeature;
		return KErrNone;		
		}	
	return CXmlEngDeserializer::DisableFeature(aParserFeature);
	}

// --------------------------------------------------------------------------------------
// Checks if a feature is enabled
// --------------------------------------------------------------------------------------
//	
EXPORT_C TBool CXmlEngDeserializerXOP::IsFeatureEnabled(TInt aParserFeature) const
	{
	if(aParserFeature == EConvertTagsToLowerCase)
		{
		return iParserMode & aParserFeature;		
		}	
	return iParser->IsFeatureEnabled(aParserFeature);
	}
    
// --------------------------------------------------------------------------------------
// Deserializes document
// --------------------------------------------------------------------------------------
//
void CXmlEngDeserializerXOP::DeserializeL()
    {
	switch(iSerializationOutput)
		{
		case EDeserializeFromFile:
			{
			RFs fs;
			User::LeaveIfError(fs.Connect());
			CleanupClosePushL(fs); 
		    DeserializeL(fs, iInputFileName->Des(), iParsingOptions);
		    CleanupStack::PopAndDestroy(&fs); //fs
		    break;
			}
		case EDeserializeFromBuffer:
			{
			if(iBuffer.Length() == 0)
				{
				User::Leave(KXmlEngErrNoParameters);
				}
			DeserializeL(iBuffer, iParsingOptions);
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
void CXmlEngDeserializerXOP::DeserializeL( const TDesC& aFileName,
										   const TXmlEngParsingOptions& aOptions ) 
    {
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs); 
	DeserializeL(fs, aFileName, aOptions);
	CleanupStack::PopAndDestroy(&fs); //fs
	}

// --------------------------------------------------------------------------------------
// Deserialize document
// --------------------------------------------------------------------------------------
//
void CXmlEngDeserializerXOP::DeserializeL( RFs& aRFs, 
										const TDesC& aFileName,
										const TXmlEngParsingOptions& aOptions ) 
    {
	iBodyPartArray.ResetAndDestroy();     
	if(iData)
	   	{
	   	delete iData;
	   	iData = NULL;
	   	}
	iData = ReadFileL( aRFs, aFileName );
	TPtrC8 dataPtr = iData->Des();
	CXmlEngDeserializerXOP::DeserializeL( dataPtr, aOptions );	
	}

// --------------------------------------------------------------------------------------
// Deserialize document
// --------------------------------------------------------------------------------------
//
void CXmlEngDeserializerXOP::DeserializeL( const TDesC8& aBuffer,
										   const TXmlEngParsingOptions& aOptions ) 
    {
    TXmlEngParsingOptions originalOpts = iParsingOptions;
    iParsingOptions = aOptions;

    if( iCleanXOPInfoset )
    	{
		Xml::ParseL(*iParser, aBuffer);
		}
	else
		{
	    if(!aBuffer.Length())
	    	{
	    	User::Leave(KXmlEngErrNoParameters);
	    	}
	    if(iBodyPartArray.Count())
	        iBodyPartArray.ResetAndDestroy();   	
	    
	   	MultipartParser::ParseL( aBuffer, KMultipartMixed, KBoundary1, KUrlStub, iBodyPartArray );    	

		// get xml document
		CBodyPart* bodyPart = NULL;
		if(iBodyPartArray.Count() > 0)	
			{
			bodyPart = iBodyPartArray[0];		
			}
		else
			{
			//The multipart document wasn't parsed correctly
			User::Leave(KXmlEngErrBadMultipart);
			}
		Xml::ParseL(*iParser, bodyPart->Body());		
		}
	iParsingOptions = originalOpts;
	iBodyPartArray.ResetAndDestroy();
    }        

// --------------------------------------------------------------------------------------
// Retrieves pointer to data referenced by CID from parsed multipart package
// --------------------------------------------------------------------------------------
//
TInt CXmlEngDeserializerXOP::GetData(const TDesC8& aCid, TPtrC8& aData)
	{
	for(TInt i = 0; i < iBodyPartArray.Count(); i++)
		{
		TPtrC8 cid = iBodyPartArray[i]->ContentID();
		if(iBodyPartArray[i]->ContentID() == aCid)
			{
			TInt size = iBodyPartArray[i]->Body().Size();
			iDataPtr.Set(iBodyPartArray[i]->Body());
			aData.Set(iDataPtr);
			return KErrNone;
			}
		}
	return KErrNotFound;
	}
  
// --------------------------------------------------------------------------------------
// Default Constructor
// --------------------------------------------------------------------------------------
//
CXmlEngDeserializerXOP::CXmlEngDeserializerXOP( TBool aCleanXOPInfoset ) 
	 : iData(NULL), iCleanXOPInfoset(aCleanXOPInfoset)
    {
    }
    
// --------------------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------------------
//
CXmlEngDeserializerXOP::~CXmlEngDeserializerXOP()
    {
    if(iData)
    	{
    	delete iData;
    	}
    iBodyPartArray.ResetAndDestroy();
    delete iInternalHandler;  
    }

// --------------------------------------------------------------------------------------
// 2nd phase constructor
// --------------------------------------------------------------------------------------
//
void CXmlEngDeserializerXOP::ConstructL( MContentHandler& aContentHandler )
    {
    if(iInternalHandler)
    	{
    	delete iInternalHandler;
    	iInternalHandler = NULL;
    	}    
    iInternalHandler = CXmlEngContentHandler::NewL(this, &aContentHandler);

    if(iParser)
    	{
    	delete iParser;
    	iParser = NULL;
    	}

    CMatchData* matchData = CMatchData::NewLC();
    matchData->SetMimeTypeL(KXmlMimeType);
    matchData->SetVariantL(KParserType); 
    iParser = CParser::NewL(*matchData, *iInternalHandler);
    CleanupStack::PopAndDestroy(matchData);
    }

// --------------------------------------------------------------------------------------
// Read file
// --------------------------------------------------------------------------------------
//
HBufC8* CXmlEngDeserializerXOP::ReadFileL( RFs& aRFs, const TDesC& aFileName)
    {
    RFile file;
    User::LeaveIfError(file.Open(aRFs, aFileName, EFileRead));
    CleanupClosePushL(file);
    TInt size;
    User::LeaveIfError(file.Size(size));
    HBufC8* buf = HBufC8::NewLC(size);
    TPtr8 bufPtr(buf->Des());
    User::LeaveIfError(file.Read(bufPtr));
    CleanupStack::Pop(buf); // buf
    CleanupStack::PopAndDestroy(&file); // file
    return buf;
    }

/*
// --------------------------------------------------------------------------------------
// SerializationOutput
// --------------------------------------------------------------------------------------
//
TXmlEngDeserializationSource CXmlEngDeserializerXOP::SerializationOutput()
    {
    return iSerializationOutput;
    }
*/


