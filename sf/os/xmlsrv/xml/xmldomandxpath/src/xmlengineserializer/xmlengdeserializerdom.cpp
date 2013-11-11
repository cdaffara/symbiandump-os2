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
// XML DOM Deserializer implementation
//

#include <xml/dom/xmlengserializeerrors.h>
#include <xml/dom/xmlengdeserializer.h>
#include <xml/dom/xmlengdeserializerdom.h>
#include "xmlengdomcontenthandler.h"

#include <xml/parserfeature.h>
#include <xml/dom/xmlengdom.h>
#include <xml/dom/xmlengdocument.h>

using namespace Xml;

// --------------------------------------------------------------------------------------
// Creates an instance of CXmlEngDeserializerDOM
// --------------------------------------------------------------------------------------
//
EXPORT_C CXmlEngDeserializerDOM* CXmlEngDeserializerDOM::NewL() 
    {
	CXmlEngDeserializerDOM* self = new (ELeave) CXmlEngDeserializerDOM; 
	return self;
	}

// --------------------------------------------------------------------------------------
// Set input type as file
// --------------------------------------------------------------------------------------
//
EXPORT_C void CXmlEngDeserializerDOM::SetInputFileL(const TDesC& aFileName, TXmlEngDeserializerType aType)
	{
	if(iInputFileName)
	    {
	    delete iInputFileName;
	    iInputFileName=NULL;
	    }
	iBuffer.Set(KNullDesC8());
	iInputFileName = aFileName.AllocL();
	iType = aType;
	}

// --------------------------------------------------------------------------------------
// Set input type as buffer
// --------------------------------------------------------------------------------------
//
EXPORT_C void CXmlEngDeserializerDOM::SetInputBuffer(const TDesC8& aBuffer, TXmlEngDeserializerType aType)
	{
	if(iInputFileName)
	    {
	    delete iInputFileName;
	    iInputFileName=NULL;
	    }
	iBuffer.Set(aBuffer);
	iType = aType;
	}	
	
// --------------------------------------------------------------------------------------
// Set external data array
// --------------------------------------------------------------------------------------
//
EXPORT_C void CXmlEngDeserializerDOM::UseExternalDataL(RArray<TXmlEngDataContainer>& aList)
	{
	iDataList = &aList;
	}


// --------------------------------------------------------------------------------------
// Fetches external data registered in this deserializer
// --------------------------------------------------------------------------------------
//
EXPORT_C RArray<TXmlEngDataContainer>* CXmlEngDeserializerDOM::ExternalData()
	{
	return iDataList;
	}
  
// --------------------------------------------------------------------------------------
// Serializes TXmlEngNode to file
// --------------------------------------------------------------------------------------
//
EXPORT_C RXmlEngDocument CXmlEngDeserializerDOM::DeserializeL()
    {
    RXmlEngDocument doc;
	if(iInputFileName)
	    {
	    RFs fs; 
	    CleanupClosePushL(fs);
	    User::LeaveIfError(fs.Connect());    				
	    doc =DeserializeL(fs, *iInputFileName, iType);			
	    CleanupStack::PopAndDestroy(&fs);
	    return doc;
	    }
	else if(iBuffer.Length())
	    {
	    return DeserializeL(iBuffer, iType);			
	    }
	else
	    {
	    User::Leave(KXmlEngErrNoParameters);    
	    }
	return doc;
    }

// --------------------------------------------------------------------------------------
// Deserialize document
// --------------------------------------------------------------------------------------
//
EXPORT_C RXmlEngDocument CXmlEngDeserializerDOM::DeserializeL(const TDesC& aFileName,
                                                               TXmlEngDeserializerType aType) 
    {
	RFs fs; 
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	RXmlEngDocument doc = DeserializeL(fs,aFileName,aType);
	CleanupStack::PopAndDestroy(&fs);			
	return doc;
	}

// --------------------------------------------------------------------------------------
// Deserialize document
// --------------------------------------------------------------------------------------
//
EXPORT_C RXmlEngDocument CXmlEngDeserializerDOM::DeserializeL(RFs& aRFs,
                                                   const TDesC& aFileName,
                                                   TXmlEngDeserializerType aType) 
    {
    if(!iImpl)
        {
        User::Leave(KXmlEngErrWrongUseOfAPI);
        }
    
    RXmlEngDocument doc;
	doc.OpenL(*iImpl);
	CleanupClosePushL(doc);
	
    CXmlEngDOMContentHandler* handler = CXmlEngDOMContentHandler::NewL(doc);
    CleanupStack::PushL(handler);
    
    CXmlEngDeserializer* des = CXmlEngDeserializer::NewL(*handler,aType);
    CleanupStack::PushL(des);
	
	des->UseExternalDataL(*iDataList);
	des->EnableFeature(EReportNamespaceMapping);
	des->DeserializeL(aRFs,aFileName);	
		
	CleanupStack::PopAndDestroy(des);
	CleanupStack::PopAndDestroy(handler);	
	CleanupStack::Pop(&doc);
    
    return doc;		
	}

// --------------------------------------------------------------------------------------
// Deserialize document
// --------------------------------------------------------------------------------------
//
EXPORT_C RXmlEngDocument CXmlEngDeserializerDOM::DeserializeL(const TDesC8& aBuffer,
                                                               TXmlEngDeserializerType aType) 
    {
    if(!iImpl)
        {
        User::Leave(KXmlEngErrWrongUseOfAPI);
        }
    
    RXmlEngDocument doc;
	doc.OpenL(*iImpl);
	CleanupClosePushL(doc);
	
    CXmlEngDOMContentHandler* handler = CXmlEngDOMContentHandler::NewL(doc);
    CleanupStack::PushL(handler);
    
    CXmlEngDeserializer* des = CXmlEngDeserializer::NewL(*handler,aType);
    CleanupStack::PushL(des);
	
	des->UseExternalDataL(*iDataList);
	des->EnableFeature(EReportNamespaceMapping);
	des->DeserializeL(aBuffer);	
		
	CleanupStack::PopAndDestroy(des);
	CleanupStack::PopAndDestroy(handler);	
	CleanupStack::Pop(&doc);
    
    return doc;
    }  

// --------------------------------------------------------------------------------------
// Sets DOM implementation that should be used during document creating.
// --------------------------------------------------------------------------------------
//
EXPORT_C void CXmlEngDeserializerDOM::UseDOMImplementationL(RXmlEngDOMImplementation& aImpl)
    {
    iImpl = &aImpl;
    }

      
// --------------------------------------------------------------------------------------
// Default Constructor
// --------------------------------------------------------------------------------------
//
CXmlEngDeserializerDOM::CXmlEngDeserializerDOM( ) 
    {    
    }
    
// --------------------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------------------
//
CXmlEngDeserializerDOM::~CXmlEngDeserializerDOM()
    {
   	delete iInputFileName;  
    }

