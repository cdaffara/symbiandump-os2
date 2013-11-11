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
// Serialize node to GZIP file
//



/**
 @file
 @internalComponent
 @released
*/
#ifndef XMLENGSERIALIZERGZIP_H
#define XMLENGSERIALIZERGZIP_H

#include <xml/dom/xmlengserializer.h>

/**
 * XML serializer to GZIP format
 */
class CXmlEngSerializerGZIP: public CXmlEngSerializer
    {
 public:
    /**
     * Creates an instance of CXmlEngSerializerGZIP class
     *
	 * @return Pointer to instance of CXmlEngSerializerGZIP class
     *
     * @leave KErrNoMemory, KErrGeneral, KXmlEngErrWrongEncoding.
     */
	static CXmlEngSerializerGZIP* NewL();
 
	/**
     * Destructor.
     *
     */
    ~CXmlEngSerializerGZIP();
    
    /**
     * Serializes TXmlEngNode
     *
	 * @param aRoot Node to be serialized
	 * @return Number of bytes written
     *
     * @leave KErrNoMemory, KErrGeneral, KXmlEngErrWrongEncoding.
     */
	TInt SerializeL(const TXmlEngNode aRoot = TXmlEngNode());
    
    /**
     * Serializes TXmlEngNode
     *
	 * @param aRoot Node to be serialized     
	 * @param aFileName File name to which document will be serialized
	 * @param aOptions Serialization options	 	 
	 * @return Number of bytes written
     *
     * @leave KErrNoMemory, KErrGeneral, KXmlEngErrWrongEncoding.
     */
	TInt SerializeL(const TDesC& aFileName, 
	    			const TXmlEngNode aRoot = TXmlEngNode(),
					const TXmlEngSerializationOptions& aOptions = TXmlEngSerializationOptions(0));	
									  		
    /**
     * Serializes TXmlEngNode
     *
	 * @param aRoot Node to be serialized     
	 * @param aRFs File server session
	 * @param aFileName File name to which document will be serialized
	 * @param aOptions Serialization options	 	 
	 * @return Number of bytes written
     *
     * @leave KErrNoMemory, KErrGeneral, KXmlEngErrWrongEncoding.
     */
     TInt SerializeL(RFs& aRFs, 
					 const TDesC& aFileName, 
					 const TXmlEngNode aRoot = TXmlEngNode(),
					 const TXmlEngSerializationOptions& aOptions = TXmlEngSerializationOptions(0));	     
	
    /**
     * Serializes TXmlEngNode to file
     *
	 * @param aRoot XML data buffer
	 * @param aBuffer Buffer to which document will be serialized
	 * @param aOptions Serialization options
	 * @return Number of bytes written
     *
     * @leave KErrNoMemory, KErrGeneral, KXmlEngErrWrongEncoding.
     */
	TInt SerializeL(RBuf8& aBuffer, 
	                const TXmlEngNode aRoot = TXmlEngNode(),
		   	        const TXmlEngSerializationOptions& aOptions = TXmlEngSerializationOptions(0));

private:
    /**
     * Constructor
     */
    CXmlEngSerializerGZIP();
            
    TInt SerializeToFileL(RFs& aRFs,
                          const TDesC& aFileName,
                          const TXmlEngNode aNode,
                          const TXmlEngSerializationOptions& aOpt);

    TInt SerializeToBufferL(RBuf8& aBuf,
                            const TXmlEngNode aNode,
                            const TXmlEngSerializationOptions& aOpt);
    
    TInt SerializeToStreamL(MXmlEngOutputStream& aStream,
                            const TXmlEngNode aNode,
                            const TXmlEngSerializationOptions& aOpt);
    };

#endif /* XMLENGSERIALIZERGZIP_H */
