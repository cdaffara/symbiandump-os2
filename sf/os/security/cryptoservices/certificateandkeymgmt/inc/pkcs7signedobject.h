/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
 @released
*/

#ifndef __PKCS7_SIGNED_OBJECT_H__
#define __PKCS7_SIGNED_OBJECT_H__

#include <signed.h>
#include <e32base.h>

const TInt KPKCS7MaxDataElements = 6;

class CPKCS7ContentInfo;
class CPKCS7ExtendedCertificateOrCertificate;
class CPKCS7SignerInfo;
class TASN1DecGeneric;
class CX509AlgorithmIdentifier;
class CX509Certificate;

/**
 A representation of a PKCS7 (signed data) entity
 This class is only used for update notifications and so does not provide
 full functionality. In particular, there is no means for creating a new 
 PKCS7 object (other than from binary data),there is no means to modify it 
 and revokation lists are not supported.  The optional fields in signerinfo
 are not supported and are ignored.
 @internalTechnology
 */
class CPKCS7SignedObject : public CSignedObject
	{
public:
	/**
	Values for encoded data element positions in tbsCert data structure.
	These values are passed as the index to the DataElementEncoding() function
	Objects containing revokation lists are not supported
	*/
	enum
		{
		EVersionNumber = 0,	
		EDigestAlgorithms = 1,
		EContentInfo = 2,
		ECertificates = 3,
		ERevocationLists = 4,
		ESignedInfo = 5
		};
	/**
	Creates a PKCS7 object from the RFC2315 representation
	@param aRawData	The binary ASN1 encoded entity
	@return			The fully constructed object
	*/
	IMPORT_C static CPKCS7SignedObject* NewL(const CPKCS7ContentInfo& aContentInfo);

	/**
	Creates a PKCS7-2 object from the RFC2315 representation
	@param aRawData	The binary ASN1 encoded entity
	@return			The fully constructed object
	*/
	IMPORT_C static CPKCS7SignedObject* NewLC(const CPKCS7ContentInfo& aContentInfo);

	/** (From CSignedObject) 
	Accesses one of the encoded fields
	@param aIndex	One of the data element positions listed in the preceding enum
	@return			The encoded field
	*/
	IMPORT_C virtual const TPtrC8* DataElementEncoding(const TUint aIndex) const;
	/** (From CSignedObject) 
	Not supported
	*/
	IMPORT_C virtual void InternalizeL(RReadStream& aStream) ;
	/** (From CSignedObject) 
	Provides access to the signed data (PKCS7-2) part of the PKCS7 object
	@return			The signed data
	*/
	IMPORT_C virtual const TPtrC8 SignedDataL() const;
	/**
	Provides access to the digest algorithms
	@return			The digest algorithms
	*/
	IMPORT_C const RPointerArray<CX509AlgorithmIdentifier>& DigestAlgorithms() const;
	/**
	Provides access to the PKCS7 version number
	@return			The version number
	*/
	IMPORT_C TInt Version() const;
	/**
	Provides access to the PKCS7 version number
	@return			The version number
	*/
	IMPORT_C const CPKCS7ContentInfo& ContentInfo() const;
	/**
	Provides access to the certificates
	@return			The certificates
	*/
	IMPORT_C const RPointerArray<CPKCS7ExtendedCertificateOrCertificate>& Certificates() const;
	/**
	Provides access to the signer information
	@return			The signer information
	*/
	IMPORT_C const RPointerArray<CPKCS7SignerInfo>& SignerInfo() const;
	/**
	Validates the signer and creates the certificate chain for that signer
	@param aSignerInfo			The signer to be validated
	@param aCertChainEncoding	The certificate chain.  This is created and pushed onto the
								cleanup stack by the function.
	*/
	IMPORT_C TBool ValidateSignerL(const CPKCS7SignerInfo& aSignerInfo, HBufC8*& aCertChainEncoding);
	/** The destructor */
	IMPORT_C ~CPKCS7SignedObject();
protected:
	IMPORT_C CPKCS7SignedObject(void);
	IMPORT_C void ConstructL(const CPKCS7ContentInfo& aContentInfo);


private:
	CPKCS7SignedObject(const CPKCS7SignedObject&);
	void InitSignedObjectL(const TDesC8& aRawData);
	void DecodeSignedDataL(const TDesC8& aRawData);
	void DecodeDigestAlgorithmsL(const TDesC8& aRawData);
	void DecodeCertificatesL(const TDesC8& aRawData);
	void DecodeRevocationListsL(const TDesC8& aRawData);
	void DecodeSignerInfoL(const TDesC8& aRawData);

	TBool ValidateSignatureL(const CPKCS7SignerInfo& aSignerInfo, const CX509Certificate& aEndEntityCert);
protected:
	RPointerArray<CX509AlgorithmIdentifier> iDigestAlgorithms;
	TInt iVersion;
	CPKCS7ContentInfo* iContentInfo;
	RPointerArray<CPKCS7ExtendedCertificateOrCertificate> iCertificates;
	RPointerArray<CPKCS7SignerInfo> iSignerInfo;
	TFixedArray<TPtrC8*, KPKCS7MaxDataElements> iDataElements;
	};

#endif //__PKCS7_SIGNED_OBJECT_H__
