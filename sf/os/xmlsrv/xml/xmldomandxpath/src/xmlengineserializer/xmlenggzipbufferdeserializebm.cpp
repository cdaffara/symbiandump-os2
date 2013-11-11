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
// GZIP output stream functions
//

#include "xmlenggzipbufferdeserializebm.h"
#include <ezstream.h>
#include <ezcompressor.h>



    CXmlEngGZipBufferDeserializeBM* CXmlEngGZipBufferDeserializeBM::NewLC(  Xml::CParser* aParser, const TDesC8& aInputBuffer, TInt aOutBufSize  )
	    {
	    CXmlEngGZipBufferDeserializeBM* bm = new (ELeave) CXmlEngGZipBufferDeserializeBM( aParser, aInputBuffer );
	    CleanupStack::PushL( bm );
	    bm->ConstructL( aOutBufSize );
	    return bm;	
	    }

    CXmlEngGZipBufferDeserializeBM* CXmlEngGZipBufferDeserializeBM::NewL( Xml::CParser* aParser, const TDesC8& aInputBuffer, TInt aOutBufSize )
	    {
	    CXmlEngGZipBufferDeserializeBM* bm = CXmlEngGZipBufferDeserializeBM::NewLC( aParser, aInputBuffer, aOutBufSize );
	    CleanupStack::Pop( bm );
	    return bm;	
	    }

   CXmlEngGZipBufferDeserializeBM::CXmlEngGZipBufferDeserializeBM( Xml::CParser* aParser, const TDesC8& aInputBuffer )
        : iOutputDescriptor(NULL,0)
        {
        iParser = aParser;
        iInputDescriptor.Set(aInputBuffer);
        }

   void CXmlEngGZipBufferDeserializeBM::ConstructL( TInt aOutBufSize )
        {
	    if ( aOutBufSize <= 0 ) 
	        {
	        User::Leave(KErrArgument);
	        }
	    iOutputBuffer = new (ELeave) TUint8[aOutBufSize];
	    iOutputDescriptor.Set(iOutputBuffer,0,aOutBufSize);
        }

   CXmlEngGZipBufferDeserializeBM::~CXmlEngGZipBufferDeserializeBM()
        {
        delete iOutputBuffer;
        }
    
    void CXmlEngGZipBufferDeserializeBM::InitializeL( CEZZStream& aZStream )
    	{
    	iParser->ParseBeginL();
    	aZStream.SetInput( iInputDescriptor );
    	aZStream.SetOutput( iOutputDescriptor );
    	}

    void CXmlEngGZipBufferDeserializeBM::NeedInputL( CEZZStream& aZStream )
    	{
    	//no input except iInputDescriptor
        aZStream.SetInput( KNullDesC8 );
    	}

    void CXmlEngGZipBufferDeserializeBM::NeedOutputL( CEZZStream& aZStream )
    	{
    	TPtrC8 od = aZStream.OutputDescriptor();    	
    	iParser->ParseL( od );
    	aZStream.SetOutput( iOutputDescriptor );
    	}

    void CXmlEngGZipBufferDeserializeBM::FinalizeL( CEZZStream& aZStream )
        {
        TPtrC8 od = aZStream.OutputDescriptor();
        iParser->ParseL( od );
        iParser->ParseEndL();
    	}

  
