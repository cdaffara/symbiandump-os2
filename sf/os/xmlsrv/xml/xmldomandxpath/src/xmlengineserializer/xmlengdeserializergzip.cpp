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
// Implementation of GZIP deserializer
//

#include <xml/parser.h>
#include <xml/parserfeature.h>
#include <ezgzip.h>

#include <xml/dom/xmlengerrors.h>
#include "xmlengdeserializergzip.h"
#include "xmlenggzipbufferdeserializebm.h"
#include "xmlenggzipfiledeserializer.h"

#include <xml/dom/xmlengserializeerrors.h>


using namespace Xml;

// --------------------------------------------------------------------------------------
// Constructor
// --------------------------------------------------------------------------------------
//
CXmlEngDeserializerGZIP* CXmlEngDeserializerGZIP::NewL( MContentHandler& aContentHandler )
	{   
	CXmlEngDeserializerGZIP* self = new (ELeave) CXmlEngDeserializerGZIP( );
	CleanupStack::PushL( self ); 
	self->ConstructL( aContentHandler );
	CleanupStack::Pop();
	return self;
	}

// --------------------------------------------------------------------------------------
// Deserialize document
// --------------------------------------------------------------------------------------
//
void CXmlEngDeserializerGZIP::DeserializeL( const TDesC& aFileName,
											const TXmlEngParsingOptions& /*aOptions*/ ) 
    {
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs); 
	DeserializeL(fs, aFileName);
	CleanupStack::PopAndDestroy(&fs);
	}
   
// --------------------------------------------------------------------------------------
// Deserialize document
// --------------------------------------------------------------------------------------
//
void CXmlEngDeserializerGZIP::DeserializeL( RFs& aRFs, 
											const TDesC& aFileName,
											const TXmlEngParsingOptions& /*aOptions*/ ) 
    {
    CXmlEngGZipFileDeserializer* deser = CXmlEngGZipFileDeserializer::NewLC( iParser, aRFs, aFileName, KOutputBufferSize );
    
    while ( deser->InflateL() )
        {
        /* empty */
        }
    
    CleanupStack::PopAndDestroy( deser );
    }

// --------------------------------------------------------------------------------------
// Deserialize document
// --------------------------------------------------------------------------------------
//
void CXmlEngDeserializerGZIP::DeserializeL( const TDesC8& aBuffer,
											const TXmlEngParsingOptions& /*aOptions*/ ) 
    {
    CXmlEngGZipBufferDeserializeBM* bm = CXmlEngGZipBufferDeserializeBM::NewLC( iParser, aBuffer, KOutputBufferSize );
    CEZDecompressor* decompressor = CEZDecompressor::NewLC( *bm );
    
    while ( decompressor->InflateL() )
        {
        /* empty */
        }
    
    CleanupStack::PopAndDestroy( decompressor );
    CleanupStack::PopAndDestroy( bm );
    }

// --------------------------------------------------------------------------------------
// Deserialize document
// --------------------------------------------------------------------------------------
//
void CXmlEngDeserializerGZIP::DeserializeL()
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
		    DeserializeL(fs, iInputFileName->Des());			
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
		default:
			{
			User::Leave(KErrGeneral);
			}						
		}   
    }

// --------------------------------------------------------------------------------------
// Default Constructor
// --------------------------------------------------------------------------------------
//
CXmlEngDeserializerGZIP::CXmlEngDeserializerGZIP()
    {
    }
    
// --------------------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------------------
//
CXmlEngDeserializerGZIP::~CXmlEngDeserializerGZIP()
    {
    }


