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

#include "xmlenggzipfiledeserializer.h"
#include <ezstream.h>
#include <ezdecompressor.h>


    CXmlEngGZipFileDeserializer* CXmlEngGZipFileDeserializer::NewLC( Xml::CParser* aParser, RFs& aRfs, const TDesC& aGzFileName, TInt aBufSize )
	    {
	    CXmlEngGZipFileDeserializer* bm = new (ELeave) CXmlEngGZipFileDeserializer( aParser );
	    CleanupStack::PushL( bm );
	    bm->ConstructL( aRfs, aGzFileName, aBufSize );
	    return bm;	
	    }

    CXmlEngGZipFileDeserializer* CXmlEngGZipFileDeserializer::NewL( Xml::CParser* aParser, RFs& aRfs, const TDesC& aGzFileName, TInt aBufSize )
	    {
	    CXmlEngGZipFileDeserializer* bm = CXmlEngGZipFileDeserializer::NewLC( aParser, aRfs, aGzFileName, aBufSize );
	    CleanupStack::Pop( bm );
	    return bm;	
	    }

   CXmlEngGZipFileDeserializer::CXmlEngGZipFileDeserializer( Xml::CParser* aParser )
        : iInputDescriptor(NULL,0), iOutputDescriptor(NULL,0)
        {
        iParser = aParser;
        }

   void CXmlEngGZipFileDeserializer::ConstructL( RFs& aRfs, const TDesC& aGzFileName, TInt aBufSize )
        {
        EZGZipFile::LocateAndReadTrailerL(aRfs,aGzFileName,iTrailer);
	    User::LeaveIfError(iInputFile.Open(aRfs,aGzFileName,EFileStream | EFileRead | EFileShareAny));
	    EZGZipFile::ReadHeaderL(iInputFile,iHeader);
	    iCrc = crc32(iCrc,NULL,0);
        
	    if ( aBufSize <= 0 ) 
	        {
	        User::Leave(KErrArgument);
	        }
	    iInputBuffer = new (ELeave) TUint8[aBufSize];
	    iOutputBuffer = new (ELeave) TUint8[aBufSize];

	    iInputDescriptor.Set(iInputBuffer,0,aBufSize);
	    iOutputDescriptor.Set(iOutputBuffer,0,aBufSize);
	
	    
	    iDecompressor = CEZDecompressor::NewL(*this,-CEZDecompressor::EMaxWBits);
        }

   CXmlEngGZipFileDeserializer::~CXmlEngGZipFileDeserializer()
        {
        //do NOT destroy iParser
        delete iDecompressor;
        delete[] iInputBuffer;
        delete[] iOutputBuffer;
        iInputFile.Close();
        }
 
   TBool CXmlEngGZipFileDeserializer::InflateL()
	    {
	    TBool keepGoing = iDecompressor->InflateL();

	    if (!keepGoing)
		    {
		    if (iCrc != iTrailer.iCrc32)
			    User::Leave(KEZlibErrBadGZipCrc);
		    iInputFile.Close();
		    }
	    return keepGoing;
	    }
    
    void CXmlEngGZipFileDeserializer::InitializeL( CEZZStream& aZStream )
    	{
    	iParser->ParseBeginL();
	    User::LeaveIfError(iInputFile.Read(iInputDescriptor));
	    aZStream.SetInput(iInputDescriptor);
    	aZStream.SetOutput( iOutputDescriptor );
    	}

    void CXmlEngGZipFileDeserializer::NeedInputL( CEZZStream& aZStream )
    	{
	    User::LeaveIfError(iInputFile.Read(iInputDescriptor));
	    aZStream.SetInput(iInputDescriptor);
    	}

    void CXmlEngGZipFileDeserializer::NeedOutputL( CEZZStream& aZStream )
    	{
    	TPtrC8 od = aZStream.OutputDescriptor();
	    iCrc = crc32(iCrc,od.Ptr(),od.Size());    	
    	iParser->ParseL( od );
    	aZStream.SetOutput( iOutputDescriptor );
    	}

    void CXmlEngGZipFileDeserializer::FinalizeL( CEZZStream& aZStream )
        {
        TPtrC8 od = aZStream.OutputDescriptor();
        iCrc = crc32(iCrc,od.Ptr(),od.Size());
        iParser->ParseL( od );
        iParser->ParseEndL();
    	}
