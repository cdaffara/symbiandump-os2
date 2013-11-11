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

#ifndef CMSSIGNEDOBJECT_H
#define CMSSIGNEDOBJECT_H

#include <cmsdefs.h>
#include <signed.h>
#include <cmscertchoice.h>

class CCmsSignerInfo;
class CDSAPrivateKey;
class CRSAPrivateKey;
class CCmsSignerIdentifier;
class CCmsContentInfo;
class CEncapsulatedContentInfo;
class CX509Certificate;
class CX509AlgorithmIdentifier;
class CCmsCertificateChoice;
class CASN1EncSequence;
class CASN1EncBase;

const TInt KCmsMaxSignedDataElements = 6;
/**
 A representation of a RFC2630 (signed data) entity.
 */
class CCmsSignedObject : public CSignedObject
	{
public:

	/**
	Index of CMS object fields.
	*/
	enum
		{
		/**
		Index of version field
		*/
		EVersionNumber = 0,
		
		/**
		Index of digest algorithm set field
		*/		
		EDigestAlgorithms = 1,

		/**
		Index of encapsulated content info field
		*/				
		EEncapsulatedContentInfo = 2,
		
		/**
		Index of certificate set field
		*/						
		ECertificates = 3,
		
		/**
		Index of revocation list field
		*/						
		ERevocationLists = 4,
		
		/**
		Index of signer info set field
		*/						
		ESignedInfo = 5
		};

	/**
	Creates a CMS signed data object as defined in RFC2630. The CMS signed data created by 
		this API contains no signer info. SignL() method can be called to add more signer info.
	@param aType The type of the encapsulated content.
	@param aIsDetached A boolean indicating whether the encapsulated data is detached.
	@param aContentData The encapsulated data. 
		If aIsDetached is EFalse, aContentData must not be KNullDesC8. Otherwise this API leave 
		with KErrArgument.
		If aIsDetached is ETrue, aContentData can be KNullDesC8. But user must provide hash 
		value when later calling SignL(). Otherwise SignL() leaves with KErrArguement.	
	@return The fully constructed object.
	*/
	IMPORT_C static CCmsSignedObject* NewL(TCmsContentInfoType aType,
											TBool aIsDetached,
											const TDesC8& aContentData);

	/**
	Creates a CMS signed data object as defined in RFC2630, and leaves the object on the cleanup stack.
		this API contains no signer info. SignL() method can be called to add more signer info.
	@param aType The type of the encapsulated content.
	@param aIsDetached A boolean indicating whether the encapsulated data is detached.
	@param aContentData The encapsulated data.
		If aIsDetached is EFalse, aContentData must not be KNullDesC8. Otherwise this API leave 
		with KErrArgument.
		If aIsDetached is ETrue, aContentData can be KNullDesC8. But user must provide hash 
		value when later calling SignL(). Otherwise SignL() leaves with KErrArguement.	
	@return The fully constructed object.
	*/
	IMPORT_C static CCmsSignedObject* NewLC(TCmsContentInfoType aType,
											TBool aIsDetached,
											const TDesC8& aContentData);
	
	/**
	Creates a CMS signed data object as defined in RFC2630. This API only creates detached signed data 
		as no data content is provided. The CMS signed data created by this API contains one signer info. 
		SignL() method can be called to add more signer info.
	@param aType Encapsulated Content data type.
	@param aHashValue The hash value of the data content to be signed.
	@param aDigestAlgorithm The digest algorithm used to create the hash.
	@param aKey The DSA private key used to sign.
	@param aCert The signer's certificate.
	@param aAddCertificate A boolean indicating whether the signer's certificate is added to the signed data object.
	@return	The fully constructed object.
	*/	
	IMPORT_C static CCmsSignedObject* NewL(TCmsContentInfoType aType,
										const TDesC8& aHashValue,
										TAlgorithmId aDigestAlgorithm,
										const CDSAPrivateKey& aKey,
										const CX509Certificate& aCert,
										TBool aAddCertificate);
											
	/**
	Creates a CMS signed data object as defined in RFC2630 and leaves the object on the cleanup stack.
        This API only creates detached signed data as no data content is provided. The CMS signed data 
        created by this API contains one signer info. SignL() method can be called to add more signer info.
	@param aType Encapsulated Content data type.
	@param aHashValue The hash value of the data content to be signed.
	@param aDigestAlgorithm The digest algorithm used to create the hash.
	@param aKey The DSA private key used to sign.
	@param aCert The signer's certificate.
	@param aAddCertificate A boolean indicating whether the signer's certificate is added to the signed data object.
	@return	The fully constructed object.
	*/			
	IMPORT_C static CCmsSignedObject* NewLC(TCmsContentInfoType aType,
										const TDesC8& aHashValue,
										TAlgorithmId aDigestAlgorithm,
										const CDSAPrivateKey& aKey,
										const CX509Certificate& aCert,
										TBool aAddCertificate);

	/**
	Creates a CMS signed data object as defined in RFC2630. This API only creates detached signed data 
		as no data content is provided. The CMS signed data created by this API contains one signer info. 
		SignL() method can be called to add more signer info.
	@param aType Encapsulated Content data type.
	@param aHashValue The hash value of the data content to be signed.
	@param aDigestAlgorithm The digest algorithm used to create the hash.
	@param aKey The RSA private key used to sign.
	@param aCert aCert The signer's certificate.
	@param aAddCertificate A boolean indicating whether the signer's certificate is added to the signed data object.
	@return	The fully constructed object.
	*/	
	IMPORT_C static CCmsSignedObject* NewL(TCmsContentInfoType aType,
										const TDesC8& aHashValue,
										TAlgorithmId aDigestAlgorithm,
										const CRSAPrivateKey& aKey,
										const CX509Certificate& aCert,
										TBool aAddCertificate);
											
	/**
	Creates a CMS signed data object as defined in RFC2630 and leaves the object on the cleanup stack.
        This API only creates detached signed data as no data content is provided. The CMS signed data 
        created by this API contains one signer info. SignL() method can be called to add more signer info.
	@param aType Encapsulated Content data type.
	@param aHashValue The hash value of the data content to be signed.
	@param aDigestAlgorithm The digest algorithm used to create the hash.
	@param aKey The RSA private key used to sign.
	@param aCert The signer's certificate.
	@param aAddCertificate A boolean indicating whether the signer's certificate is added to the signed data object.
	@return	The fully constructed object.
	*/			
	IMPORT_C static CCmsSignedObject* NewLC(TCmsContentInfoType aType,
										const TDesC8& aHashValue,
										TAlgorithmId aDigestAlgorithm,
										const CRSAPrivateKey& aKey,
										const CX509Certificate& aCert,
										TBool aAddCertificate);

	/**
	Creates a CMS signed data object as defined in RFC2630.
	@param aContentInfo	The CMS content info that contains the encoded signed object.
	@return			The fully constructed object.
	*/
	IMPORT_C static CCmsSignedObject* NewL(const CCmsContentInfo& aContentInfo);
	
	/**
	Creates a CMS signed data object as defined in RFC2630 and leaves it on the cleanup stack.
	@param aContentInfo	The CMS content info that contains the encoded signed object.
	@return			The fully constructed object.
	*/	
	IMPORT_C static CCmsSignedObject* NewLC(const CCmsContentInfo& aContentInfo);


	/**
	Creates one signature and adds it to the Signer info list. The signing certificate
	is added to the certificate list if the last boolean parameter aAddCertificate is true and 
	it does not exist in the list. The digest algorithm is added to the digest algorithm list if it 
	does not exist in the list. Calling this API multiple times will create multiple signatures.
	@param aHashValue The hash value to be signed. If this is an empty string,
					  the content data to be signed must have been passed in via 
					  NewL method and hash value will be calculated by the implementation
					  of this method. 
	@param aDigestAlgorithm The digest algorithm used to create the hash.
	@param aKey the DSA private key used to sign.
	@param aCert the signer's certificate.
	@param aAddCertificate A boolean indicating whether the signer's certificate is added to the signed data object.
	@leave KErrArgument if no hash nor data content is provided.
	*/
	IMPORT_C void SignL(const TDesC8& aHashValue,
						TAlgorithmId aDigestAlgorithm,
						const CDSAPrivateKey& aKey,
						const CX509Certificate& aCert,
						TBool aAddCertificate);
						

	/**
	Creates one signature and adds it to the Signer info list. The signing certificate
	is added to the certificate list if the last boolean parameter aAddCertificate is true and 
	it does not exist in the list. The digest algorithm is added to the digest algorithm list if it 
	does not exist in the list. Calling this API multiple times will create multiple signatures.
	@param aHashValue The hash value to be signed. If this is an empty string,
					  the content data to be signed must have been passed in via 
					  NewL method and hash value will be calculated by the implementation
					  of this method. 
	@param aDigestAlgorithm The digest algorithm used to create the hash.
	@param aKey the RSA private key used to sign.
	@param aCert the signer's certificate.
	@param aAddCertificate A boolean indicating whether the signer's certificate is added to the signed data object.
	@leave KErrArgument if no hash nor data content is provided.
	*/
	IMPORT_C void SignL(const TDesC8& aHashValue,
						TAlgorithmId aDigestAlgorithm,
						const CRSAPrivateKey& aKey,
						const CX509Certificate& aCert,
						TBool aAddCertificate);

	/**
	Destructor
	*/
	IMPORT_C ~CCmsSignedObject();

	/*
	virtual from signedobject class
	*/
	IMPORT_C virtual const TPtrC8* DataElementEncoding(const TUint aIndex) const;
	IMPORT_C virtual void InternalizeL(RReadStream& aStream) ;
	IMPORT_C virtual const TPtrC8 SignedDataL() const;
		
	/**
	Returns whether the certificate list exists.
	@return	Boolean indicating whether the certificate list exists.
	*/
	IMPORT_C TBool IsCertificateSetPresent() const;

	/**
	Returns whether the certificate revocation list exists.
	@return	Boolean indicating whether the certificate Revocation list exists.
	*/
	IMPORT_C TBool IsCertificateRevocationListsPresent() const;

	/**
	Returns the version of this CMS signed object.
	@return The version of this CMS signed object.
	*/
	IMPORT_C TInt Version() const;

	/**
	Returns the employed algorithm list.
	@return	The employed algorithm list reference.
	*/
	IMPORT_C const RPointerArray<CX509AlgorithmIdentifier>& DigestAlgorithms() const;
	
	/**
	Returns the certificates list.
	@return	The certificates list reference.
	*/
	IMPORT_C const RPointerArray<CCmsCertificateChoice>& Certificates() const;
	
	/**
	Returns the encapsulated content info of this signed object.
	@return The encapsulated content info reference.
	*/
	IMPORT_C const CEncapsulatedContentInfo& ContentInfo() const;

	
	/**
	Retrieves the list of SignerInfo objects.
	@return The signer info list reference.
	*/
	IMPORT_C const RPointerArray<CCmsSignerInfo>& SignerInfo() const;


	/**
	Creates the ASN1 sequence of this CMS signed object and leaves it on the cleanup stack.
	@return  ASN1 sequence of this object.
	*/
	IMPORT_C CASN1EncSequence* EncodeASN1DERLC() const;
	
	/**
	Appends the X509 certificate to the certificate list.
	@param aCert The X509 certificate to be appended.
	*/
	IMPORT_C void AddCertificateL(const CX509Certificate& aCert);


	/**
	Appends an encoded attribute certificate to the certificate list. 
	@param aCert The encoded certificate to be appended.
	@param aType The type of the encoded certificate..
	*/
	IMPORT_C void AddCertificateL(const TDesC8& aCert, CCmsCertificateChoice::TCertificateType aType);	
	
	/**
	Validates the signer and creates the certificate chain for that signer. This API is used to validate attached signature.
	@param aSignerInfo The signer to be validated.
	@param aCertChainEncoding The certificate chain. This is created and pushed onto the cleanup stack by the function.
	@leave KErrNotFound There is no matching certificate.
	@return Boolean that identifies whether the signer can be validated.
    */
	IMPORT_C TBool ValidateSignerLC(const CCmsSignerInfo& aSignerInfo, HBufC8*& aCertChainEncoding);

	/**
	Validates the signer and creates the certificate chain for that signer. This API is used to validate attached signature.
	@param aSignerInfo The signer to be validated.
	@param aCertificates The certificate list provided by the user to validate the signature.
	@param aCertChainEncoding The certificate chain. This is created and pushed onto the cleanup stack by the function.
	@return Boolean that identifies whether the signer can be validated.
	@leave KErrNotFound There is no matching certificate.
	*/
	IMPORT_C TBool ValidateSignerLC(const CCmsSignerInfo& aSignerInfo, const RPointerArray<CX509Certificate>& aCertificates, HBufC8*& aCertChainEncoding);



	/**
	Validates the signer and creates the certificate chain for that signer. This API is used to validate detached signature.
	@param aSignerInfo The signer to be validated.
	@param aCertChainEncoding The certificate chain. This is created and pushed onto the cleanup stack by the function.
	@param aIsHash The flag represent if the next parameter is the hash of the data content.
	@param aContentDataOrHash the descriptor that contains the data content or its hash
	@leave KErrNotFound There is no matching certificate.
	@return Boolean that identifies whether the signer can be validated.
    */
	IMPORT_C TBool ValidateSignerLC(const CCmsSignerInfo& aSignerInfo, HBufC8*& aCertChainEncoding, TBool aIsHash, const TDesC8& aContentDataOrHash);

	/**
	Validates the signer and creates the certificate chain for that signer. This API is used to validate detached signature.
	@param aSignerInfo The signer to be validated.
	@param aCertificates The certificate list provided by the user to validate the signature.
	@param aCertChainEncoding The certificate chain. This is created and pushed onto the cleanup stack by the function.
	@param aIsHash The flag represent if the next parameter is the hash of the data content.
	@param aContentDataOrHash the descriptor that contains the data content or its hash	
	@return Boolean that identifies whether the signer can be validated.
	@leave KErrNotFound There is no matching certificate.
	*/
	IMPORT_C TBool ValidateSignerLC(const CCmsSignerInfo& aSignerInfo, const RPointerArray<CX509Certificate>& aCertificates, HBufC8*& aCertChainEncoding, TBool aIsHash, const TDesC8& aContentDataOrHash);

	
private:
	/**
	Constructor
	*/
	CCmsSignedObject();
	
	
private:
	/**
	Second phase constructor for decoding a CMS signed data object
	@param aContentInfo the content info which contains the CMS signed data.
	*/		
	void ConstructL(const CCmsContentInfo& aContentInfo);
	
	/**
	Second phase constructor for constructing a CMS signed data object from data content.
	@param aType the encapsulated content info type.
	@param aIsDetached if the CMS signed data does not contains the data content being signed.
	@param aContentData the content data descriptor.
	*/			
	void ConstructL(TCmsContentInfoType aType, TBool aIsDetached, const TDesC8& aContentData);
	
	/**
	Second phase constructor for constructing a CMS signed data object from data content hash
	@param aType the encapsulated content info type.
	@param aHashValue the hash of the data content to create the signature.
	@param aDigestAlgorithm the digest algorithm.
	@param aKey the DSA private to create signature.
	@param aCert the signer's certficate
	@param aAddCertificate a flag to represent if the signer's certificate is added to certificate set.
	*/				
	void ConstructL(TCmsContentInfoType aType,
					const TDesC8& aHashValue,
					TAlgorithmId aDigestAlgorithm,
					const CDSAPrivateKey& aKey,
					const CX509Certificate& aCert,
					TBool aAddCertificate);
	/**
	Second phase constructor for constructing a CMS signed data object from data content hash
	@param aType the encapsulated content info type.
	@param aHashValue the hash of the data content to create the signature.
	@param aDigestAlgorithm the digest algorithm.
	@param aKey the RSA private to create signature.
	@param aCert the signer's certficate
	@param aAddCertificate a flag to represent if the signer's certificate is added to certificate set.
	*/								
	void ConstructL(TCmsContentInfoType aType,
					const TDesC8& aHashValue,
					TAlgorithmId aDigestAlgorithm,
					const CRSAPrivateKey& aKey,
					const CX509Certificate& aCert,
					TBool aAddCertificate);
	/**
	Append the algorithm to the algoritm list
	@param aDigestAlgorithm the algorithm ID.
	*/
	void AddDigestAlgorithmL(TAlgorithmId aDigestAlgorithm);
					
	/**
	Build the signer's identifier from the signer's certificate. If the signer's certificate
	contains the subject identifier extension, the signer identifier is subject id extension.
	otherwise, the signer identifier is isuuer name and serial number.
	@param aCert the signer's certificate.
	@return a CMS signer identifier instance pointer
	*/
	CCmsSignerIdentifier* BuildSignerIdentifierLC(const CX509Certificate& aCert);
	
	/**
	Build the signer list, algorithm list and certificate list in the CMS signer data.
	@param aDigestAlgorithm the digest algorithm identifier.
	@param aIsHash A flag the represent if the next descriptor is the hash value rather that original data
	@param aValue the data content or its hash.
	@param aKey the DSA private used to sign.
	@param aCert the signer's certificate
	@param aAddCertificate the flag to represent if the certificate is added to the certificate set
	*/	
	void BuildSignerInfoCertListAndAlgoritmListL(TAlgorithmId aDigestAlgorithm,
												TBool aIsHash,
												const TDesC8& aValue,
												const CDSAPrivateKey& aKey,
												const CX509Certificate& aCert,
												TBool aAddCertificate);

	/**
	Build the signer list, algorithm list and certificate list in the CMS signer data.
	@param aDigestAlgorithm the digest algorithm identifier.
	@param aIsHash A flag the represent if the next descriptor is the hash value rather that original data
	@param aValue the data content or its hash.
	@param aKey the RSA private used to sign.
	@param aCert the signer's certificate
	@param aAddCertificate the flag to represent if the certificate is added to the certificate set
	*/		
	void BuildSignerInfoCertListAndAlgoritmListL(TAlgorithmId aDigestAlgorithm,
												TBool aIsHash,
												const TDesC8& aValue,
												const CRSAPrivateKey& aKey,
												const CX509Certificate& aCert,
												TBool aAddCertificate);
	/**
	Initialise the signed data base class members for the validation process.
	@param aRawData the raw data of the CMS signed data.
	*/
	void InitSignedObjectL(const TDesC8& aRawData);
	
	
	/**
	Decode the CMS Signer data.
	@param aRawData the raw data of the CMS signed data.
	*/
	void DecodeSignedDataL(const TDesC8& aRawData);	

	/**
	Decode the digest algorithm set.
	@param the raw data of the algorithm list.
	*/
	void DecodeDigestAlgorithmsL(const TDesC8& aRawData);

	/**
	Decode the encapsulated content info
	@param the raw data of the encapsulated content info.
	*/
	void DecodeEncapsulatedContentInfoL(const TDesC8& aRawData);
	
	/**
	Decode the certificate set.
	@param the raw data of the certificate list
	*/	
	void DecodeCertificatesL(const TDesC8& aRawData);
	
	/**
	Decode the certificate revocation set. Not implemented now!
	@param the raw data of the certificate revocation list.
	*/		
	void DecodeRevocationListsL(const TDesC8& aRawData);

	/**
	Decode the signer info set.
	@param the raw data of the certificate revocation list.
	*/			
	void DecodeSignerInfoL(const TDesC8& aRawData);

	/**
	Encode the certificate set
	@return the encoding of the certificate set
	*/													
	CASN1EncBase* EncodeCertificatesLC() const;
	
	/**
	Encode the algorithm set
	@return the encoding of the digest algorithm set
	*/
	CASN1EncBase* EncodeAlgorithmsLC() const;
	
	/**
	Encode the signer info set
	@return the encoding of the certificate set
	*/
	CASN1EncBase* EncodeSignerInfoLC() const;
	
	/**
	Validate the signature by the given certificate.
	@param aSignerInfo the signer info reference contains the signature
	@param aEndEntityCert the certificate used to create the signature.
	@return if the signature can be validated
	*/
	TBool ValidateSignatureL(const CCmsSignerInfo& aSignerInfo, const CX509Certificate& aEndEntityCert);
	
	/**
	This function is called when validating a detached CMS signed object.
	It sets the data content being signed so that the signed data can be validated.
	@param aContentData The data content being signed.
	*/
	void SetContentData(const TDesC8& aContentData);

	/**
	This function is called when validating a detached CMS signed object.
	It sets the hash being signed so that the signed data can be validated.
	@param aHash The hash being signed.
	*/
	void SetHash(const TDesC8& aHash);
	
	
private:
	/**
	Reprents if the certificate set is present
	*/
	TBool iIsCertificateSetPresent;
	
	/**
	Reprents if the certificate revocationlisy is present
	*/	
	TBool iIsCertificateRevocationListsPresent;
	
	/**
	Version of the Signed object
	*/
	TInt iVersion;
	
	/**
	Algorithm Set
	*/
	RPointerArray<CX509AlgorithmIdentifier> iDigestAlgorithms;
	
	/**
	Encapsulated Content List
	*/
	CEncapsulatedContentInfo* iContentInfo;
	
	/**
	Certificate Set
	*/
	RPointerArray<CCmsCertificateChoice> iCertificates;
	
	/**
	Signer Info Set
	*/
	RPointerArray<CCmsSignerInfo> iSignerInfo;
	
	/**
	Array of Encoded fields
	*/	
	TFixedArray<TPtrC8*, KCmsMaxSignedDataElements> iDataElements;
	
	/**
	The data content being signed
	*/
	TPtrC8 iContentData;
	
	/**
	The Hash being signed
	*/
	TPtrC8 iHash;
	};


#endif //CMSSIGNEDOBJECT_H
