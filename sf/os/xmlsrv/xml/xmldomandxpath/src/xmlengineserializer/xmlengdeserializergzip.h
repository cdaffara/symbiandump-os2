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



/**
 @file
 @internalComponent
 @released
*/
#ifndef XMLENGDESERIALIZER_GZIP__H
#define XMLENGDESERIALIZER_GZIP__H

#include <xml/utils/xmlengxestd.h>
#include <xml/dom/xmlengdeserializer.h>

/**
 * GZIP XML deserializer
 * 
 */
class CXmlEngDeserializerGZIP: public CXmlEngDeserializer
{
public:
    static CXmlEngDeserializerGZIP* NewL( Xml::MContentHandler& aContentHandler );

    /**
     * Deserializes XML
     *
     *
     * @leave KErrNoMemory, KErrGeneral, KXmlEngErrWrongEncoding.
     */
	void DeserializeL(); 
  
      /**
     * Deserializes XML
     *
     * @param aFileName File to be parsed
     * @param aOptions Parsing options     
	 *
     * @leave KErrNoMemory, KErrGeneral, KXmlEngErrWrongEncoding.
     */
     void DeserializeL( const TDesC& aFileName,
						const TXmlEngParsingOptions& aOptions = 
							TXmlEngParsingOptions() );
	
    /**
     * Deserializes XML
     *
     * @param aRFs File Server session
     * @param aFileName File to be parsed
     * @param aOptions Parsing options     
	 *
     * @leave KErrNoMemory, KErrGeneral, KXmlEngErrWrongEncoding.
     */    
    void DeserializeL( RFs& aRFs, const TDesC& aFileName,
					   const TXmlEngParsingOptions& aOptions = 
							TXmlEngParsingOptions() );
        
    /**
     * Deserializes XML
     *
     * @param aBuffer Buffer to be parsed
     * @param aOptions Parsing options     
	 *
     * @leave KErrNoMemory, KErrGeneral, KXmlEngErrWrongEncoding.
     */
    void DeserializeL( const TDesC8& aBuffer,
					   const TXmlEngParsingOptions& aOptions = 
							TXmlEngParsingOptions() );
    
    /**
     * Destructor.
     */
    ~CXmlEngDeserializerGZIP();

protected:
    /**
     * Default constructor.
     */
    CXmlEngDeserializerGZIP( );

private:
	    /**
	    * Default output buffers size.
	    */
	    static const TInt KOutputBufferSize = 4096;
};

#endif /* XMLENGDESERIALIZER_GZIP__H */
