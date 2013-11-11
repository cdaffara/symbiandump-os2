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
// Implementation of serializer
//

#include <xml/dom/xmlengserializer.h>
#include <xml/dom/xmlengerrors.h>
#include "xmlengserializerxop.h"
#include "xmlengserializergzip.h"
#include <xml/dom/xmlengdocument.h>
#include <xml/dom/xmlengserializeerrors.h>


// --------------------------------------------------------------------------------------
// Creates an instance of CXmlEngSerializer
// --------------------------------------------------------------------------------------
//
EXPORT_C CXmlEngSerializer* CXmlEngSerializer::NewL(TXmlEngSerializerType aType) 
    {
    switch(aType)
        {
        case ESerializerXOP:
            {
            return CXmlEngSerializerXOP::NewL( EFalse );
            }
        case ESerializerXOPInfoset:
            {
            return CXmlEngSerializerXOP::NewL( ETrue );
            }			
        case ESerializerGZip:
            {
            return CXmlEngSerializerGZIP::NewL();
            }			
        case ESerializerDefault:		
        default:
            {
            CXmlEngSerializer* self = new (ELeave) CXmlEngSerializer();
            //2nd phase construction not needed currently
            //CleanupStack::PushL(self);
            //self->ConstructL();
            //CleanupStack::Pop(self);            
            return self;
            }
        }
    }

// --------------------------------------------------------------------------------------
// Set output type as file
// --------------------------------------------------------------------------------------
//
EXPORT_C void CXmlEngSerializer::SetOutputL(const TDesC& aFileName)
    {
    if(iOutFileName)
        {
        delete iOutFileName;
        iOutFileName=NULL;
        }
    iOutFileName = aFileName.AllocL();
    iSerializationOutput = ESerializeToFile;
    }

// --------------------------------------------------------------------------------------
// Set output type as buffer
// --------------------------------------------------------------------------------------
//
EXPORT_C void CXmlEngSerializer::SetOutput(RBuf8& aBuffer)
    {
    iBuffer = &aBuffer;
    iSerializationOutput = ESerializeToBuffer;
    }

// --------------------------------------------------------------------------------------
// Set output type as output stream
// --------------------------------------------------------------------------------------
//
EXPORT_C void CXmlEngSerializer::SetOutput(MXmlEngOutputStream& aStream)
    {
    iOutputStream = &aStream;
    iSerializationOutput = ESerializeToStream;
    }	

// --------------------------------------------------------------------------------------
// Set serialization options
// --------------------------------------------------------------------------------------
//
EXPORT_C void CXmlEngSerializer::SetSerializationOptions(TXmlEngSerializationOptions& aOptions)
    {
    iSerializationOptions = &aOptions;
    }

// --------------------------------------------------------------------------------------
// Serializes TXmlEngNode to file
// --------------------------------------------------------------------------------------
//
EXPORT_C TInt CXmlEngSerializer::SerializeL( TXmlEngNode aRoot)
    {
    TXmlEngSerializationOptions* defPtr = NULL;
    TXmlEngSerializationOptions def = TXmlEngSerializationOptions();
    if(iSerializationOptions )
    	{
    	defPtr = iSerializationOptions;
    	}
    else
        {
        defPtr = &def;
        }
    
    RXmlEngDocument doc = aRoot.OwnerDocument(); //CXmlEngSerializer isn't owner of doc - in case
        										   //of leave, cleanup done by the caller
    switch(iSerializationOutput)
        {
        case ESerializeToFile:
            {
            return SerializeL(iOutFileName->Des(), aRoot, *defPtr);
            }
        case ESerializeToBuffer:
            {
            return SerializeL(*iBuffer, aRoot, *defPtr);
            }
        case ESerializeToStream:
            {
            if(!iOutputStream)
                {
                User::Leave(KXmlEngErrNoParameters);
                }
            return doc.SaveL(*iOutputStream, aRoot, *defPtr );			
            }
            default:
                {
                User::Leave(KErrNotSupported);
                }						
        }
        return KErrGeneral; //should never happen
    }    

// --------------------------------------------------------------------------------------
// Serializes TXmlEngNode to file
// --------------------------------------------------------------------------------------
//
EXPORT_C TInt CXmlEngSerializer::SerializeL( RFs& aRFs, 
										  const TDesC& aFileName, 
										  const TXmlEngNode aRoot,										  
										  const TXmlEngSerializationOptions& aOptions )
    {
    return aRoot.OwnerDocument().SaveL(aRFs, aFileName, aRoot, aOptions);
    }

// --------------------------------------------------------------------------------------
// Serializes TXmlEngNode to file
// --------------------------------------------------------------------------------------
//
EXPORT_C TInt CXmlEngSerializer::SerializeL( const TDesC& aFileName, 
                                            const TXmlEngNode aRoot,
                                            const TXmlEngSerializationOptions& aOptions )
    {
    TInt size;
    RFs rfs;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);
    size = aRoot.OwnerDocument().SaveL(rfs, aFileName, aRoot, aOptions);
    CleanupStack::PopAndDestroy(&rfs);
    return size;
    }

// --------------------------------------------------------------------------------------
// Serializes TXmlEngNode to buffer
// --------------------------------------------------------------------------------------
//
EXPORT_C TInt CXmlEngSerializer::SerializeL( RBuf8& aBuffer, 
                                        const TXmlEngNode aRoot, 
                                        const TXmlEngSerializationOptions& aOptions)
    {
    return aRoot.OwnerDocument().SaveL(aBuffer, aRoot, aOptions);
    } 
   
// --------------------------------------------------------------------------------------
// Default Constructor
// --------------------------------------------------------------------------------------
//
CXmlEngSerializer::CXmlEngSerializer()
	: iBuffer(NULL),
	  iOutputStream(NULL),
	  iSerializationOptions(NULL)
    {
    }

void CXmlEngSerializer::ConstructL()
    {
    }
    
// --------------------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------------------
//
CXmlEngSerializer::~CXmlEngSerializer()
    {
    delete iOutFileName;
    }
    

