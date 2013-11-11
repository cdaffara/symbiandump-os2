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
// Interface class describing class that may be used as output stream for dom tree
//



/**
 @file
 @internalComponent
 @released
*/
#ifndef XMLENGCGZIPFILEDESERIALIZER_H
#define XMLENGCGZIPFILEDESERIALIZER_H

#include <xml/dom/xmlengoutputstream.h>
#include <ezbufman.h>
#include <xml/parser.h>
#include <f32file.h>

#include <ezgzip.h>
class CEZDecompressor;

class CXmlEngGZipFileDeserializer : public CBase, public MEZBufferManager
	{
	public:
        static CXmlEngGZipFileDeserializer* NewLC( Xml::CParser* aParser, RFs& aRfs, const TDesC& aGzFileName, TInt aOutBufSize = 0x8000 );

        static CXmlEngGZipFileDeserializer* NewL( Xml::CParser* aParser, RFs& aRfs, const TDesC& aGzFileName, TInt aOutBufSize = 0x8000 );

	    ~CXmlEngGZipFileDeserializer();
	    
	    TBool InflateL();
	    
        /**
         * Implementation of MEZBufferManager interface.
         */
	    void InitializeL(CEZZStream& aZStream);
	    void NeedInputL(CEZZStream& aZStream);
	    void NeedOutputL(CEZZStream& aZStream);
	    void FinalizeL(CEZZStream& aZStream);


    protected:
	    CXmlEngGZipFileDeserializer( Xml::CParser* aParser );
	    void ConstructL( RFs& aRfs, const TDesC& aGzFileName, TInt aBufSize );
			
	private:
	    
	    /** XML Framework Parser Interface **/
	    Xml::CParser* iParser;

        RFile iInputFile;

        CEZDecompressor *iDecompressor;

	    TInt32 iCrc;
        TEZGZipHeader iHeader;
        TEZGZipTrailer iTrailer;

	    /** Buffers **/
	    TUint8* iInputBuffer;
	    TUint8* iOutputBuffer;
	    TPtr8 iInputDescriptor;  // always points to start of the input Buffer
	    TPtr8 iOutputDescriptor; // always points to start of the output Buffer
	
	    	    
	}; 
	

#endif /* XMLENGCGZIPFILEDESERIALIZER_H */  
