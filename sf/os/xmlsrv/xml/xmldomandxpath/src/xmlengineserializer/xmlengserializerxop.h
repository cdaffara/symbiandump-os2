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
// Implementation of XOP serializer
//



/**
 @file
 @internalComponent
 @released
*/
#ifndef XMLENGSERIALIZERXOP_H
#define XMLENGSERIALIZERXOP_H

#include <xml/dom/xmlengserializer.h>
#include <xml/dom/xmlengdataserializer.h>
#include <f32file.h>

class CDesC8ArrayFlat;
class TXmlEngNode;
class TXmlEngDataContainer;
class MXmlEngSXOPOutputStream;

const TInt KInitBufferSize = 4096; //initial output buffer size
const TInt KXopStubMaxSize = 256; // href='cid:' + cid length

/**
 * XOP serializer
 */
class CXmlEngSerializerXOP: public CXmlEngSerializer, public MXmlEngDataSerializer
    {
 public:
    /**
     * Creates an instance of CXmlEngSerializerXOP class
     *
	 * @param aCleanXOPInfoset Indicates that MIME headers 
	 *		  should be skipped upon serialization
	 * @return Pointer to instance of CXmlEngSerializerXOP class
     *
     * @leave KErrNoMemory, KErrGeneral, KXmlEngErrWrongEncoding.
     */
	IMPORT_C static CXmlEngSerializerXOP* NewL( TBool aCleanXOPInfoset  );
 
    /**
     * Serializes TXmlEngNode
     *
	 * @param aRoot Root node of a DOM tree to be serialized
	 * @return Number of bytes written
     *
     * @leave KErrNoMemory, KErrGeneral, KXmlEngErrWrongEncoding.
     */
	TInt SerializeL(const TXmlEngNode aRoot);

    /**
     * Serializes TXmlEngNode to file in XOP format
     *
	 * @param aFileName File to which DOM tree will be serialized
	 * @param aRoot Root node of a DOM tree to be serialized	 
	 * @param aOptions Serialization options
	 * @return Number of bytes written
     *
     * @leave KErrNoMemory, KErrGeneral, KXmlEngErrWrongEncoding.
     */
	TInt SerializeL(const TDesC& aFileName,
	                const TXmlEngNode aRoot,
					const TXmlEngSerializationOptions& aOptions);

    /**
     * Serializes TXmlEngNode to file in XOP format
     *
	 * @param aRFs File server handle     
	 * @param aFileName File to which DOM tree will be serialized
	 * @param aRoot Root node of a DOM tree to be serialized
	 * @param aOptions Serialization options	 	 
	 * @return Number of bytes written
     *
     * @leave KErrNoMemory, KErrGeneral, KXmlEngErrWrongEncoding.
     */
	TInt SerializeL(RFs& aRFs,
					const TDesC& aFileName,
					const TXmlEngNode aRoot,
					const TXmlEngSerializationOptions& aOptions);

    /**
     * Serializes TXmlEngNode to buffer in XOP format
     *
	 * @param aBuffer Buffer to which DOM tree will be serialized
	 * @param aRoot Root node of a DOM tree to be serialized
	 * @param aOptions Serialization options
	 * @return Number of bytes written
     *
     * @leave KErrNoMemory, KErrGeneral, KXmlEngErrWrongEncoding.
     */
	TInt SerializeL( RBuf8& aBuffer, 
                     const TXmlEngNode aRoot, 
					 const TXmlEngSerializationOptions& aOptions );	 						  
	 						  
	/**
     * Destructor.
     */
    ~CXmlEngSerializerXOP();

  	/**
     * Determines how data node should be serialized
	 * @param aRoot Root node of a DOM tree to be serialized     	
     *
     */    
    TPtrC8 SerializeDataL(TXmlEngNode aNode);
 
protected: //functions

    
private: //functions
    /**
     * Constructor
     *
	 * @param aCleanXOPInfoset Indicates that MIME headers 
	 *		  should be skipped upon serialization     
     */
    CXmlEngSerializerXOP( TBool aCleanXOPInfoset );

    /**
     * Second phase constructor
     */
    void ConstructL();

    /**
     * Serializes DOM tree to a stream
     */
	TInt StreamSerializeL(MXmlEngSXOPOutputStream& aOutputStream, 
						  const TXmlEngNode aRoot,
						  const TXmlEngSerializationOptions& aOptions);

    /**
     * Generates random CID
     *
	 * @param aCid randomly generated CID string
     */
	 void GenerateRandomCid(TDes8& aCid);

    /**
     * Opens already existing or creates a new file
     *
     * @param aFileName file name
     * @param aFile Instance of class RFile
     */
	 void OpenOrCreateL(TDesC& aFileName, RFile& aFile);

    /**
     * Sets callback for text nodes in serialization options
     *
     * @param aOptions Serialization options
     */
	 void SetCallbackL(TXmlEngSerializationOptions& aOptions);

	/**
     * Leaves if a DOM tree contains 'Include' elements from XOP 
     * namespace 'http://www.w3.org/2004/08/xop/include'
     *
     * @param aRoot Root node of a DOM tree to be serialized
     */
	 void LeaveIfXopIncludeL(TXmlEngNode aRoot);
	 	 
	/**
     * Verifies if a DOM tree contains 'Include' elements from XOP 
     * namespace 'http://www.w3.org/2004/08/xop/include'
     *
     * @param aRoot Root node of a DOM tree to be serialized
     * @return ETrue if a DOM tree contains Include element from XOP namespace,
     *		   EFalse otherwise.
     */
	 TBool HasXopInclude(TXmlEngNode aRoot);

	/**
     * Verifies if container's parent node has xmlmime:contentType 
     * attribute information item. If found, the function returns ETrue 
     * and xmlmime:contentType attribute value. Otherwise, function
     * returns EFalse and aContentType is undefined.
     *
     * @param aContainer Data container whose parent node is to be searched
     * @return ETrue if xmlmime:contentType attribute is found,
     *		   EFalse otherwise.
     */	 
	 TBool GetContentTypeValue(TXmlEngDataContainer aContainer, TPtrC8& aContentType);

	/**
     * Decodes input data from base64 to binary octets and creates
     * heap buffer with the decoded data. The buffer is pushed on the cleanup stack.
     *
     * @param aEncodedData Base64 encoded buffer
     * @return Buffer with decoded data in binary octets
     */	 
	 HBufC8* CreateDecodedBufLC(TPtrC8 aEncodedData);
	 
	/**
     * Appends given string to output buffer. If the serializer's 
     * output type is file, the output buffer is released to the output stream 
     * after reaching a specified threshold and the output buffer is cleared.
     *
     * @param aString String to write
     * @param aFlush Indicates that buffer is written immediately  
     */		 
	 void BufferedWriteL(const TDesC8& aString, TBool aFlush = EFalse);

	/**
     * Flushes the output buffer and closes the output file.
     */		 
	 void CommitWriteL();	 
	     
public: //data

private: //data
	/** array that stores containers to be serialized **/
	RArray<TXmlEngDataContainer> iDataContainerArray;

	/** output buffer **/
	HBufC8* iOutputBuffer;	

	/** output buffer pointer **/
	TPtr8* iOutputBufferPtr;
	
	/** size of data written **/
	TInt iDataWritten;

	/**	Indicates that MIME headers should be skipped upon serialization **/
	TBool iCleanXOPInfoset;	

	/** file server handle **/
	RFs iRFs;
	
	/** output file handle **/
	RFile iOutputFile;

	/** buffer for storing xop stub that replaces binary data in xop document **/
	HBufC8* iXopStub;

	/** pointer to stub buffer **/
	TPtr8* iXopStubPtr;	// pointer to TPtr8 to avoid initialization at construction	
	
	/** pointer to current output stream **/
	MXmlEngSXOPOutputStream* iTmpOutputStream;
    };



#endif /* XMLENGSERIALIZERXOP_H */
