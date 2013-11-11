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

#ifndef __PKCS7_DIGEST_INFO_H__
#define __PKCS7_DIGEST_INFO_H__

#include <e32base.h>
#include <x509cert.h>
#include <signed.h>
#include <asn1dec.h>

/**
 Provides the means to decode PKCS#7 encoded DigestInfo Structure.
 */
class CPKCS7DigestInfo : public CBase
	{
public:	
	/**
	 Creates a new PKCS#7 DigestInfo object. 
	 @param     aRawData contains a PKCS#7 DigestInfo Structure
	 @return    A pointer to the newly allocated object.
	 @leave 	KErrArgument if digestAlgorithm and digest is not present.
	 @leave 	KErrNotSupported if algorithm is other than MD2, MD5 and SHA1.
	 */
    IMPORT_C static CPKCS7DigestInfo* NewL(const TDesC8& aRawData);
    
    /**
	 An Algorithm present DigestAlgorithmIdentifier.
     @return    The enum which identifies the type of Algorithm
                used to obtain the hash.
     */   
	IMPORT_C TAlgorithmId Algorithm() const;
	
	/**
	 Encoded Parameters present in the DigestAlgorithmIdentifier.
	 The client has to check for data length. It is 0 in case there are no EncodedParams
	 @return    The Encoded Parameters which is in the DigestAlgorithmIdentifier.
     */     
	IMPORT_C const TPtrC8& EncodedParams() const;
	
	/**
	 The Digest which is in the DigestInfo.
	 @return    The Digest which is in the DigestInfo and is an Octet String.
     */
	IMPORT_C const TDesC8& Digest() const;
	
	/**
	 Destructor.
	 */
	virtual ~CPKCS7DigestInfo();
	
private:
	/**
	 Constructor.
	 */
    CPKCS7DigestInfo(); 
    
    /**
	 Copy Constructor.
	 @param aDigestInfo A CPKCS7DigestInfo object.
	 */
	CPKCS7DigestInfo(const CPKCS7DigestInfo& aDigestInfo);
	
	/**
	 Assignment operator.
	 @param aDigestInfo A CPKCS7DigestInfo object.
	 @return A reference to CPKCS7DigestInfo class.
	 */
	CPKCS7DigestInfo& operator=(const CPKCS7DigestInfo& aDigestInfo);
    
	/**
	 Decodes the given ASN1 DigestInfo. Below is the ASN1 syntax
	
	 DigestInfo ::= SEQUENCE
	 	{
	    digestAlgorithm  DigestAlgorithmIdentifier,
	    digest           Digest
	    }
	 Digest ::= OCTET String	 
	 DigestAlgorithmIdentifier ::= AlgorithmIdentifier
	 AlgorithmIdentifier ::= SEQUENCE 
	 	{
	    algorithm   ALGORITHM.&id({SupportedAlgorithms}),
	    parameters  ALGORITHM.&Type({SupportedAlgorithms}{@ algorithm}) OPTIONAL
	    }
	    
	 @param     aRawData A descriptor containing the PKCS7 DigestInfo Sequence.
	 @leave KErrArgument if digestAlgorithm and digest is not present.
	 @leave KErrNotSupported if algorithm is other than MD2, MD5 and SHA1.
	 @see TASN1DecGeneric, CX509AlgorithmIdentifier.
	 */
    void ConstructL(const TDesC8& aRawData);
    
private:
    /** 
     The object identifier which identifies the message-digest algorithm.
	 A message-digest algorithm maps an octet string (the message) to 
	 another octet string (the message digest) 
	 */
	TAlgorithmId iAlgorithmId;
	
	/** Encoded Parameters which is in the DigestAlgorithmIdentifier */
    TPtrC8 iEncodedParams;
	
	/** 
	 The Digest is present in the DigestInfo. 
	 It is the result of the message-digesting process 
	 */
	TPtrC8 iDigest;
	};
	
#endif //__PKCS7_DIGEST_INFO_H__
