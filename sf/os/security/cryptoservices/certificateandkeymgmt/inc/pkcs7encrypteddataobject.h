/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


/**
 @file
 @publishedPartner
 @released 
*/

#ifndef __PKCS7_ENCRYPTED_DATA_OBJECT_H__
#define __PKCS7_ENCRYPTED_DATA_OBJECT_H__

#include <asn1dec.h>
#include <signed.h>
#include <pbedata.h>
#include <asnpkcs.h>
#include <pkcs7contentinfo_v2.h>
#include <pkcs12kdf.h>

/**
 A representation of a PKCS7 (Encrypted data) entity
 This class decodes the PKCS7 Encrypted Data content. 
 It provides various exported methods which will 
 return the values present in the EncryptedData ::= SEQUENCE.
 */
class CPKCS7EncryptedDataObject : public CBase
	{
public:
	enum TContentType
	/**
	 Identifies the type of ContentType present in the EncryptedContentInfo ::= SEQUENCE
	 */
		{
	    EPkcs7Data = 1
		};  
		
   	/**
	 Creates a new PKCS#7 EncryptedData object. 
	 @param     aContentInfo contains a reference to CPKCS7ContentInfo.
	 @return    A pointer to the newly allocated object.
	 */
	IMPORT_C static CPKCS7EncryptedDataObject* NewL(const CPKCS7ContentInfo& aContentInfo);
	
	/**
	 Destructor.
	 */
	virtual ~CPKCS7EncryptedDataObject();
    /**
     Provides access to the version number within the EncryptedData SEQUENCE.
     Version is the syntax version number.It shall be 0 for this version of the standard
	 @return    The version number
	 */ 
    IMPORT_C TInt Version() const;
    
	/**
	 Provides access to the ContentType present within the EncryptedContentInfo. 
	 ContentType indicates the type of content.This is represented using the OID.
     @return    ContentType present in the EncryptedContentInfo structure.
     */ 
    IMPORT_C CPKCS7EncryptedDataObject::TContentType ContentType() const;
    
    /**
     Provides access to the encryptedContent within the EncryptedContentInfo SEQUENCE.
     encryptedContent is OPTIONAL in EncryptedContentInfo SEQUENCE
     This will return a NULL pointer in case there is no encryptedContent
	 @return    The encryptedContent
     */
	IMPORT_C const TDesC8& EncryptedContentInfoData() const;
	
    /**
     Provides access to the contentEncryptionAlgorithm within the 
     EncryptedContentInfo SEQUENCE.
	 @return    A CPBEncryptParms object which has the Encrypt Parameters 
     			EncryptedContent present in EncryptedContentInfo Sequence
     */
    IMPORT_C const CPBEncryptParms& EncryptParams() const;
    
    /**
	 This method decrypts the encrypted information. The caller assumes 
	 ownership of the returned object.
	 @param     aPassword is the password used for decryption.
	 @return    The plaintext data obtained after decryption.
	 @leave     KErrNotSupported if otherthan pkcs12 pbeIds used.
	 @leave     KErrGeneral if decrypt descriptor length is less than 0.
	 @see		PKCS12KDF, TPBPassword, CPBEncryptElement, CPBDecryptor.
	 */
	IMPORT_C HBufC8* DecryptDataL(const TDesC& aPassword) const;
	
private:
	/**
	 Constructor.
	 */
    CPKCS7EncryptedDataObject(void);
    
    /**
     Copy Constructor.
     @param aEncryptedDataObject A CPKCS7EncryptedDataObject object.
     */
    CPKCS7EncryptedDataObject(const CPKCS7EncryptedDataObject& aEncryptedDataObject);
    
    /**
	 Assignment operator.
	 @param aEncryptedDataObject A CPKCS7EncryptedDataObject object.
	 @return A reference to CPKCS7EncryptedDataObject class.
	 */
	CPKCS7EncryptedDataObject& operator=(const CPKCS7EncryptedDataObject& aEncryptedDataObject);
    
    /**    	
     This decrypt the encrypted data. Below is the ASN1 syntax.
	 
	 EncryptedData ::= SEQUENCE
	  	 {
	     version                Version,
	     encryptedContentInfo   EncryptedContentInfo
	  	 } 
	 EncryptedContentInfo ::= SEQUENCE 
	  	 {
	     contentType                 PKCS7-CONTENT-TYPE.&id({PKCS7ContentTable}),
	     contentEncryptionAlgorithm  ContentEncryptionAlgorithmIdentifier,
	     encryptedContent            [0]  ENCRYPTED{PKCS7-CONTENT-TYPE.&Type({PKCS7ContentTable}{@.contentType})} OPTIONAL
	  	 }
	 EncryptedContent ::= OCTET STRING
	 
	 ContentInfo ::= SEQUENCE 
	  	{
	    contentType ContentType,
	    content[0] EXPLICIT ANY DEFINED BY contentType OPTIONAL 
	  	}
	 ContentType ::= OBJECT IDENTIFIER
	 
	 @param 	aContentInfo Contains a reference to  CPKCS7ContentInfo
	 @leave		KErrArgument if the data is not valid PKCS#7 EncryptedData Structure.
	 @see       CPKCS7ContentInfo
     */
	void ConstructL(const CPKCS7ContentInfo& aContentInfo);
	
	/**
     Provides access to the Encrypt Parameters present within the 
     ContentEncryptionAlgorithm Sequence. Below is the ASN1 Syntax.
     
     ContentEncryptionAlgorithmIdentifier ::= AlgorithmIdentifier
     AlgorithmIdentifier: A type that identifies an algorithm (by object identifier) and associated parameters
     AlgorithmIdentifier ::= SEQUENCE 
               {
               algorithm   ALGORITHM.&id({SupportedAlgorithms}),
               parameters  ALGORITHM.&Type({SupportedAlgorithms}{@ algorithm}) OPTIONAL
               }

	 @param     aBinaryData  which is the ContentEncryptionAlgorithmIdentifier
	            and is AlgorithIdentifier Type.
	            AlgorithmIdentifier: A type that identifies an 
	            algorithm (by object identifier) and associated parameters.
     @return    Encrypt Parameters. This Identifies the 
	            content-encryption algorithm (and any associated parameters) 
	            under which the content is encrypted.
	 @leave     KErrArgument if the data is not valid PKCS#7 ContentEncryptionAlgorithm 
	 			sequence.
	 @see 	    CPBEncryptParms, TASN1DecPKCS5.
     */ 
    CPBEncryptParms* DecodeContentEncryptionAlgorithmL(const TDesC8& aBinaryData) const;
    
	
private:
     /** version is the syntax version number in PKCS#7 EncryptedData ::= SEQUENCE*/
     TInt iVersion;
     
	 /** Indicates the type of content in PKCS7 EncryptedContentInfo Sequence */
	 TContentType iContentType;
	 
	 /** Identifies the content-encryption algorithm (and any associated parameters) 
	     under which the content is encrypted */
     CPBEncryptParms* iEncryptParams;
          
	 /** Contains the encrypted content */
	 TPtrC8 iEncryptedContent;
};
 
#endif //__PKCS7_ENCRYPTED_DATA_OBJECT_H__
