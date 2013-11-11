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
// Interface class describing class that may be used as
// output stream for dom tree
//



/**
 @file
 @internalComponent
 @released
*/
#ifndef XMLENGCGZIPBUFFERDESERIALIZEBM_H
#define XMLENGCGZIPBUFFERDESERIALIZEBM_H

#include <xml/dom/xmlengoutputstream.h>

#include <ezbufman.h>
#include <xml/parser.h>

class CXmlEngGZipBufferDeserializeBM : public CBase, public MEZBufferManager
	{
	public:
        static CXmlEngGZipBufferDeserializeBM* NewLC( Xml::CParser* aParser, const TDesC8& aInputBuffer, TInt aOutBufSize = 0x8000 );

        static CXmlEngGZipBufferDeserializeBM* NewL( Xml::CParser* aParser, const TDesC8& aInputBuffer, TInt aOutBufSize = 0x8000 );

	    ~CXmlEngGZipBufferDeserializeBM();
	    
        /**
         * Implementation of MEZBufferManager interface.
         */
	    void InitializeL(CEZZStream& aZStream);
	    void NeedInputL(CEZZStream& aZStream);
	    void NeedOutputL(CEZZStream& aZStream);
	    void FinalizeL(CEZZStream& aZStream);


    protected:
	    CXmlEngGZipBufferDeserializeBM( Xml::CParser* aParser, const TDesC8& aInputBuffer );
	    void ConstructL( TInt aOutBufSize );
			
	private:
	    
	    /** XML Framework Parser Interface **/
	    Xml::CParser*				iParser;

	    /** Buffer to be parsed **/
	    TPtrC8	iInputDescriptor;
        TPtr8 iOutputDescriptor; // always points to start of the output Buffer	    	
	    
	    /** chars array **/
	    TUint8* iOutputBuffer;
	
	    	    
	}; 

#endif /* CGZipBufferDeserializeBM_H */  
