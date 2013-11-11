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
 @publishedPartner
 @released
*/

#ifndef CMSSIGNERINFO_H
#define CMSSIGNERINFO_H

#include <e32base.h>

class CX509AlgorithmIdentifier;
class CPKCS7IssuerAndSerialNumber;
class CCmsSignerIdentifier;
class CASN1EncSequence;
class CX509Certificate;
class CDSAPrivateKey;
class CRSAPrivateKey;
class CMessageDigest;
class CASN1EncOctetString;
class CASN1EncBase;


/**
Represents CMS signer information as defined in RFC2630.
SignedAttributes and UnsignedAttributes are not supported.
*/
class CCmsSignerInfo : public CBase
	{
public:

	/**
	@internalComponent
	
	Creates a CMS signer info object as defined in RFC2630.
	@param aDataToBeSigned The data content or its hash to be signed.
	@param aIsHash Indicates whether the first parameter is a hash.
	@param aKey The DSA private key used to sign.
	@param aSignerIdentifier The signer identifier. Ownership of
		the signer identifier is taken by this newly created signer info object.
		If construction fails, ownership is not transferred. The user
		needs to push aSignerIdentifier onto the cleanup stack before calling this
		method, and pop it off the cleanup stack after successful construction.
	@param aDigestAlgorithm	The digest algorithm used to create the hash.
		Ownership of the digest algorithm instance is taken by
		this newly created signer info object. If construction fails, ownership
		is not transferred. The user needs to push aDigestAlgorithm onto the cleanup stack before
		calling this method and pop it off the cleanup stack after successful construction.
	@param aSignatureAlgorithm The signature algorithm to create the signature.
		Ownership of the signature algorithm instance is taken by this
		newly created signer info object. If construction fails, ownership is not transferred.
		The user needs to push aDigestAlgorithm onto the cleanup stack before calling this
		method and pop it off the cleanup stack after successful construction.
	@return	The fully constructed object.
	*/
	static CCmsSignerInfo* NewL(const TDesC8& aDataToBeSigned,
								TBool aIsHash,
								const CDSAPrivateKey& aKey,
								CCmsSignerIdentifier* aSignerIdentifier,
								CX509AlgorithmIdentifier* aDigestAlgorithm,
								CX509AlgorithmIdentifier* aSignatureAlgorithm);
	/**
	@internalComponent
	
	Creates a CMS signer info object as defined in RFC2630
	and leaves it on the cleanup stack.
	@param aDataToBeSigned The data content or its hash to be signed.
	@param aIsHash Indicates whether the first parameter is a hash.
	@param aKey The DSA private key used to sign.
	@param aSignerIdentifier The signer identifier. Ownership of
		the signer identifier is taken by this newly created signer info object.
		If construction fails, ownership is not transferred. The user
		needs to push aSignerIdentifier onto the cleanup stack before calling this
		method, and pop it off the cleanup stack after successful construction.
	@param aDigestAlgorithm	The digest algorithm used to create the hash.
		Ownership of the digest algorithm instance is taken by
		this newly created signer info object. If construction fails, ownership
		is not transferred. The user needs to push aDigestAlgorithm onto the cleanup stack before
		calling this method and pop it off the cleanup stack after successful construction.
	@param aSignatureAlgorithm The signature algorithm to create the signature.
		Ownership of the signature algorithm instance is taken by this
		newly created signer info object. If construction fails, ownership is not transferred.
		The user needs to push aDigestAlgorithm onto the cleanup stack before calling this
		method and pop it off the cleanup stack after successful construction.
	@return	The fully constructed object.
	*/
	static CCmsSignerInfo* NewLC(const TDesC8& aDataToBeSigned,
								TBool aIsHash,
								const CDSAPrivateKey& aKey,
								CCmsSignerIdentifier* aSignerIdentifier,
								CX509AlgorithmIdentifier* aDigestAlgorithm,
								CX509AlgorithmIdentifier* aSignatureAlgorithm);
	/**
	@internalComponent
	
	Creates a CMS signer info object as defined in RFC2630.
	@param aDataToBeSigned The data content or its hash to be signed.
	@param aIsHash Indicates whether the first parameter is a hash.
	@param aKey The RSA private key used to sign.
	@param aSignerIdentifier The signer identifier. Ownership of
		the signer identifier is taken by this newly created signer info object.
		If construction fails, ownership is not transferred. The user
		needs to push aSignerIdentifier onto the cleanup stack before calling this
		method, and pop it off the cleanup stack after successful construction.
	@param aDigestAlgorithm	The digest algorithm used to create the hash.
		Ownership of the digest algorithm instance is taken by
		this newly created signer info object. If construction fails, ownership
		is not transferred. The user needs to push aDigestAlgorithm onto the cleanup stack before
		calling this method and pop it off the cleanup stack after successful construction.
	@param aSignatureAlgorithm The signature algorithm to create the signature.
		Ownership of the signature algorithm instance is taken by this
		newly created signer info object. If construction fails, ownership is not transferred.
		The user needs to push aDigestAlgorithm onto the cleanup stack before calling this
		method and pop it off the cleanup stack after successful construction.
	@return	The fully constructed object.
	*/
	static CCmsSignerInfo* NewL(const TDesC8& aDataToBeSigned,
								TBool aIsHash,
								const CRSAPrivateKey& aKey,
								CCmsSignerIdentifier* aSignerIdentifier,
								CX509AlgorithmIdentifier* aDigestAlgorithm,
								CX509AlgorithmIdentifier* aSignatureAlgorithm);
	/**
	@internalComponent
	
	Creates a CMS signer info object as defined in RFC2630
	and leaves it on the cleanup stack.
	@param aDataToBeSigned The data content or its hash to be signed.
	@param aIsHash Indicates whether the first parameter is a hash.
	@param aKey The RSA private key used to sign.
	@param aSignerIdentifier The signer identifier. Ownership of
		the signer identifier is taken by this newly created signer info object.
		If construction fails, ownership is not transferred. The user
		needs to push aSignerIdentifier onto the cleanup stack before calling this
		method, and pop it off the cleanup stack after successful construction.
	@param aDigestAlgorithm	The digest algorithm used to create the hash.
		Ownership of the digest algorithm instance is taken by
		this newly created signer info object. If construction fails, ownership
		is not transferred. The user needs to push aDigestAlgorithm onto the cleanup stack before
		calling this method and pop it off the cleanup stack after successful construction.
	@param aSignatureAlgorithm The signature algorithm to create the signature.
		Ownership of the signature algorithm instance is taken by this
		newly created signer info object. If construction fails, ownership is not transferred.
		The user needs to push aDigestAlgorithm onto the cleanup stack before calling this
		method and pop it off the cleanup stack after successful construction.
	@return	The fully constructed object.
	*/										
	static CCmsSignerInfo* NewLC(const TDesC8& aDataToBeSigned,
								TBool aIsHash,
								const CRSAPrivateKey& aKey,
								CCmsSignerIdentifier* aSignerIdentifier,
								CX509AlgorithmIdentifier* aDigestAlgorithm,
								CX509AlgorithmIdentifier* aSignatureAlgorithm);
								
	/**
	@internalComponent
	
	Creates a CMS signer info object as defined in RFC2630.
	@param aRawData the encoded signer info.
	@return	The fully constructed object.
	*/
	static CCmsSignerInfo* NewL(const TDesC8& aRawData);

	/**
	@internalComponent
	
	Creates a CMS signer info object as defined in RFC2630
	and leaves it on the cleanup stack.
	@param aRawData The encoded signer info.
	@return	The fully constructed object.
	*/
	static CCmsSignerInfo* NewLC(const TDesC8& aRawData);	
	virtual ~CCmsSignerInfo();

	/**
	Returns the version of the CMS signer info object.
	@return	The version of the CMS signer info object.
	*/
	IMPORT_C TInt Version() const;
	
	/**
	Returns	whether signed attributes are present or not.
	@return Boolean indicating whether signed attributes are present or not.
	*/
	IMPORT_C TBool IsSignedAttributesPresent() const;

	/**
	Returns	whether unsigned attributes are present or not.
	@return Boolean indicating whether unsigned attributes are present or not.
	*/
	IMPORT_C TBool IsUnsignedAttributesPresent() const;	
	
	/**
	Returns the digest algorithm identifier.
	@return The digest algorithm identifier reference.
	*/
	IMPORT_C const CX509AlgorithmIdentifier& DigestAlgorithm() const;

	/**
	Returns the signature algorithm identifier.
	@return The signature algorithm identifier reference.
	*/			
	IMPORT_C const CX509AlgorithmIdentifier& SignatureAlgorithm() const;
	
	/**
	Returns the signature value.
	@return The signature value.
	*/
	IMPORT_C const TPtrC8 SignatureValue() const;

	/**
	Returns the signer identifier.
	@return the signer identifier reference.
	*/
	IMPORT_C const CCmsSignerIdentifier& SignerIdentifier() const;

	/**
	@internalComponent

	Creates the ASN.1 sequence of this CMS signed object and leaves it on the cleanup stack.
	@return  ASN.1 sequence of this object.
	*/
	CASN1EncSequence* EncodeASN1DERLC() const;	
	
private:
	/**
	Constructor.
	*/
	CCmsSignerInfo();
	

private:
	/**
	second phase constructor
	@param aDataToBeSigned the data or its hash
	@param aIsHash a flag to represent if the first paramter is hash or data content
	@param aKey the DSA private key used to create the signature.
	@param aSignerIdentifier the signer identifier.
	@param aDigestAlgorithm	the digest algorithm used to create hash.
	@param aSignatureAlgorithm	the signature alogorithm to create signature
	*/
	void ConstructL(const TDesC8& aDataToBeSigned, 
					TBool aIsHash, 
					const CDSAPrivateKey& aKey,					
					CCmsSignerIdentifier* aSignerIdentifier,
					CX509AlgorithmIdentifier* aDigestAlgorithm,
					CX509AlgorithmIdentifier* aSignatureAlgorithm);

	/**
	second phase constructor
	@param aDataToBeSigned the data or its hash
	@param aIsHash a flag to represent if the first paramter is hash or data content
	@param aKey the RSA private key used to create the signature.
	@param aSignerIdentifier the signer identifier.
	@param aDigestAlgorithm	the digest algorithm used to create hash.
	@param aSignatureAlgorithm	the signature alogorithm to create signature.	
	*/	
	void ConstructL(const TDesC8& aDataToBeSigned, 
					TBool aIsHash, 
					const CRSAPrivateKey& aKey,
					CCmsSignerIdentifier* aSignerIdentifier,
					CX509AlgorithmIdentifier* aDigestAlgorithm,
					CX509AlgorithmIdentifier* aSignatureAlgorithm);
	
	/**
	Second phase constructor
	@param aRawData the encoded the CMS content info
	*/		
	void ConstructL(const TDesC8& aRawData);
	
	/**
	Encode the signer identifier
	@return encoding of the signer identifier
	*/			
	CASN1EncBase* EncodeSignerIdentifierLC() const;
	
	/**
	Decode the signature
	@param aRawData the encoded signature
	*/
	void DecodeEncryptedDigestL(const TDesC8& aRawData);
	
	/**
	Decode the signer identifier
	@param aRawData the encoded signer identifier
	*/	
	void DecodeSignerIdentifierL(const TDesC8& aRawData);
	
private:
	/**
	Represents if the Signed Attribute is present
	*/
	TBool iSignedAttributesPresent;
	
	/**
	Represents if the Unsigned Attribute is present
	*/	
	TBool iUnsignedAttributesPresent;
	
	/**
	The version of the signer info
	*/
	TInt iVersion;
	
	/**
	the signer identifier
	*/
	CCmsSignerIdentifier* iSignerIdentifier;
	
	/**
	The digest algorithm identifier
	*/
	CX509AlgorithmIdentifier* iDigestAlgorithm;
	/**
	The signature algorithm identifier
	*/	
	CX509AlgorithmIdentifier* iSignatureAlgorithm;
		
	/**
	the signature value
	*/
	HBufC8* iSignatureValue;
	};

#endif




















