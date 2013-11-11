/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
 @released
*/

#ifndef CMSUTILS_H
#define CMSUTILS_H

#include <signed.h>
#include <hash.h>
#include <cmscertchoice.h>

class CCmsCertificateChoice;
class CX509AlgorithmIdentifier;
class CX509Certificate;
class CMessageDigest;
class CRSAPrivateKey;
class CDSAPrivateKey;
class TASN1DecGeneric;
class CASN1EncObjectIdentifier;
class CASN1EncBase;

/**
A utility class contains a set of static functions for encoding and decoding the 
CMS signed data relevant CMS structures.
*/
class CmsUtils
	{
public:
	/**
	Decode the content type in CMS content info and encapsulated info.
	@param aASN1DecGeneric the ASN decoded structure of the content info
	@return the type of the content info.
	*/
	static TInt DecodeContentTypeL(const TASN1DecGeneric* aASN1DecGeneric);

	/**
	Encode the content type in CMS content info and encapsulated info.
	@param aContentType the type of the content info
	@return the ASN.1 oid instance of the content type.
	*/
	static CASN1EncObjectIdentifier* EncodeContentTypeLC(TInt aContentType);
	
	/**
	Add an encoded attribute certificate in the certificate set if it is not in the list already.
	@param aCertList the certificate list to which the certificate is added
	@param aCert the encoded certificate to be added.
	@param aType the type of the encoded certificate.
	*/
	static void AddCertificateL(RPointerArray<CCmsCertificateChoice>& aCertList, const TDesC8& aCert, CCmsCertificateChoice::TCertificateType aType);
	
	/**
	Add an X509 certificate in the certificate set if it is not in the list already.
	@param aCertList the certificate list to which the certificate is added
	@param aCert the x509 certificate to be added.
	*/	
	static void AddCertificateL(RPointerArray<CCmsCertificateChoice>& aCertList, const CX509Certificate& aCert);
	
	/**
	Add an algorithm in the algorithm set if it is not in the list already.
	@param aAlgorithmIdList the algorithm list to which the algorithm is added
	@param aDigestAlgorithm the digest algorithm identifier
	*/		
	static void AddAlgorithmIdentifierL(RPointerArray<CX509AlgorithmIdentifier>& aAlgorithmIdList, TAlgorithmId aDigestAlgorithm);

	/**
	Decode the algorithm identifier set
	@param aDigestAlgorithms the algorithm list to which the decoded algorithms is added
	@param aRawData the encoded algorithm identifier set
	*/			
	static void DecodeDigestAlgorithmsL(RPointerArray<CX509AlgorithmIdentifier>& aDigestAlgorithms, const TDesC8& aRawData);
	
	/**
	Decode the certificate set
	@param aDigestAlgorithms the certficate list to which the decoded certificates is added
	@param aRawData the encoded certficate set
	*/
	static void DecodeCertificatesL(RPointerArray<CCmsCertificateChoice>& aCertificates, const TDesC8& aRawData);
	
	/**
	Encode the certficate set and put the encoding on the cleanup stack
	@param aCertList the certificates list to be encoded.
	@return the ASN.1 DER encoding of certificate set
	*/
	static CASN1EncBase* EncodeCertificatesLC(const RPointerArray<CCmsCertificateChoice>& aCertList);
	
	/**
	Encode the digest algorithm set and put the encoding on the cleanup stack.
	@param aCertList the certificates list to be encoded.
	@return the ASN.1 DER encoding of certificate set
	*/	
	static CASN1EncBase* EncodeDigestAlgorithmsLC(const RPointerArray<CX509AlgorithmIdentifier>& aDigestAlgorithms);
	
	/**
	Dncode the octet string
	@param aRawData the certificates list to be encoded.
	@param aBuf the buffer to hold the octet string.
	*/		
	static void DecodeOctetStringL(const TDesC8& aRawData, HBufC8*& aBuf);
	
	/**
	Create a hash from the given algorithm identifier and put it on the cleanup stack.
	@param aAlgorithm the hash identifier.
	@return the hash
	*/			
	static CMessageDigest* CreateHashLC(TAlgorithmId aAlgorithm);
	
	/**
	Create the DSA signature of the given hash.
	@param aHash the hash used to create signature.
	@param aKey the DSA private key used to create signature
	@return the descriptor holds the signature.
	*/
	static HBufC8* CreateSignatureL(const TDesC8& aHash, const CDSAPrivateKey& aKey);
	
	/**
	Create the RSA signature of the given hash.
	@param aHash the hash used to create signature.
	@param aKey the RSA private key used to create signature
	@return the descriptor holds the signature.
	*/	
	static HBufC8* CreateSignatureL(const TDesC8& aHash, const CRSAPrivateKey& aKey);
	
	/**
	Create the RSA signature of the given data, algorithm and private key.
	@param aDataToBeSigned the data content to be signed.
	@param aIsHash if the first paramter is a hash
	@param aAlgorithm the digest algorithm used to create hash
	@param aKey the RSA private key.
	@return the descriptor holds the signature.
	*/		
	static HBufC8* CreateSignatureL(const TDesC8& aDataToBeSigned, TBool aIsHash, TAlgorithmId aAlgorithm, const CRSAPrivateKey& aKey);
	
	/**
	Create the DSA signature of the given data, algorithm and private key.
	@param aDataToBeSigned the data content to be signed.
	@param aIsHash if the first paramter is a hash
	@param aAlgorithm the digest algorithm used to create hash
	@param aKey the DSA private key.
	@return the descriptor holds the signature.
	*/			
	static HBufC8* CreateSignatureL(const TDesC8& aDataToBeSigned, TBool aIsHash, TAlgorithmId aAlgorithm, const CDSAPrivateKey& aKey);
	
	/**
	Create the raw data from the ASN.1 class.
	@param aEncoding the reference of the ASN.1 class
	@return the descritor holds the raw data.
	*/				
	static HBufC8* CreateDEREncodingLC(const CASN1EncBase& aEncoding);
	};

#endif // CMSUTILS_H

