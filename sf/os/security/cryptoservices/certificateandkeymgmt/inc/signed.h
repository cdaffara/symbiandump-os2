/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedAll
 @released
*/
 
#ifndef __SIGNED_H__
#define __SIGNED_H__

#include <e32base.h>
#include <e32std.h>
#include <s32std.h>
#include <securitydefs.h>

class CRSAPublicKey;
class CDSAPublicKey;
class CDSASignature;
class CDSAParameters;

/**  Enumerates the identity of the algorithm.
 *
 */
enum TAlgorithmId
	{
	/** An RSA algorithm. */
	ERSA,
	/** A DSA algorithm. */
	EDSA,
	/** A DH algorithm. */
	EDH,
	/** A MD2 algorithm. */
	EMD2,
	/** A MD5 algorithm. */
	EMD5,
	/** A SHA-1 algorithm. */
	ESHA1,
	/** A SHA-224 algorithm. */
	ESHA224,
	/** A SHA-256 algorithm. */
	ESHA256,
	/** A SHA-384 algorithm. */
	ESHA384,
	/** A SHA-512 algorithm. */
	ESHA512
	};

class CValidityPeriod : public CBase
/** The period for which the certificate is valid. 
* 
* @since v6.0 */
	{
public:
	/** Tests whether the specified date and time is within the validity period.
	* 
	* @param aTime	The date and time to be tested.
	* @return 		ETrue, if the date and time is within the validity period;
	* 				EFalse, otherwise. */
	IMPORT_C TBool Valid(const TTime& aTime) const;

	/** Gets the start of the validity period.
	* 
	* @return	The start date and time. */
	IMPORT_C const TTime& Start() const;

	/** Gets the end of the validity period.
	* 
	* @return	The end date and time. */
	IMPORT_C const TTime& Finish() const;

	/** Copy constructor.
	* 
	* @param aValidityPeriod	The validity period object to be copied. */
	IMPORT_C CValidityPeriod(const CValidityPeriod& aValidityPeriod);

protected:
	/** Default constructor. */
	IMPORT_C CValidityPeriod();

	/** The start time of the validity period. */
	TTime iStart;

	/** The end time of the validity period. */
	TTime iFinish;
	};

class CAlgorithmIdentifier : public CBase
/** Contains an algorithm ID and any encoded parameters required by that algorithm.
* 
* An object of this type creates and owns a heap descriptor to contain the encoded 
* parameters. 
* 
* @since v6.0 */
	{
public:
	/** Creates a new algorithm ID object copied from an existing object.	
	* 
	* @param aAlgorithmIdentifier	The algorithm ID object to be copied. 
	* @return 						A pointer to the new algorithm ID object. */
	IMPORT_C static CAlgorithmIdentifier* NewL(const CAlgorithmIdentifier& aAlgorithmIdentifier);

	/** Creates a new algorithm ID object copied from an existing object, and puts 
	* a pointer to the new object onto the cleanup stack.
	* 
	* @param aAlgorithmIdentifier	The algorithm ID object to be copied. 
	* @return 						A pointer to the new algorithm ID object. */
	IMPORT_C static CAlgorithmIdentifier* NewLC(const CAlgorithmIdentifier& aAlgorithmIdentifier);

	/** Creates a new algorithm ID object.
	* 
	* @param aAlgorithmId	The algorithm ID.
	* @param aEncodedParams	The encoded parameters.
	* @return 				A pointer to the new algorithm ID object. */
	IMPORT_C static CAlgorithmIdentifier* NewL(TAlgorithmId& aAlgorithmId, const TDesC8& aEncodedParams);

	/** Creates a new algorithm ID object, and puts a pointer to the new object onto 
	* the cleanup stack.
	* 
	* @param aAlgorithmId	The algorithm ID.
	* @param aEncodedParams	The encoded parameters.
	* @return 				A pointer to the new algorithm ID object. */
	IMPORT_C static CAlgorithmIdentifier* NewLC(TAlgorithmId& aAlgorithmId, const TDesC8& aEncodedParams);
	
	/** Tests whether this algorithm identifier object is equal to the specified algorithm 
	* identifier object. 
	* 
	* @param aAlgorithmIdentifier	The algorithm identifier object to be compared.
	* @return 						ETrue, if this algorithm identifier object is equal to the specified 
	* 								algorithm identifier object; EFalse otherwise. */
	IMPORT_C TBool operator == (const CAlgorithmIdentifier& aAlgorithmIdentifier) const;
	
	/** Gets the algorithm identifier.
	* 
	* @return	The algorithm identifier. */
	IMPORT_C TAlgorithmId Algorithm() const;	//ID for the algorithm
	
	/** Gets the encoded parameters for the algorithm identifier.
	* 
	* Note that this object owns the heap descriptor that owns the encoded parameters.
	* 
	* @return 	The encoded parameters. */
	IMPORT_C TPtrC8 EncodedParams() const;				//the encoded parameters
	
	/** Destructor.
	*
	* Frees all resources owned by the object. */
	IMPORT_C ~CAlgorithmIdentifier();

protected:
	/** Default constructor. */
	IMPORT_C CAlgorithmIdentifier();

	/** Constructor taking the specified parameters.
	* 
	* @param aAlgorithmId	The algorithm ID. */
	IMPORT_C CAlgorithmIdentifier(TAlgorithmId& aAlgorithmId);
	
	/** Second-phase constructor taking an existing algorithm identifier object.
	* 
	* @param aAlgorithmIdentifier	The algorithm identifier object. */
	IMPORT_C virtual void ConstructL(const CAlgorithmIdentifier& aAlgorithmIdentifier);
	
	/** Second-phase constructor taking encoded parameters.
	* 
	* @param aEncodedParams	The encoded parameters. */
	IMPORT_C virtual void ConstructL(const TDesC8& aEncodedParams);
	
	/** The algorithm ID. */
	TAlgorithmId iAlgorithmId;
	
	/** The encoded parameters for the algorithm ID. */
	HBufC8* iEncodedParams;
	};

class CSigningAlgorithmIdentifier : public CBase
/** Contains two CAlgorithmIdentifier objects for comparison purposes.
* 
* Implements an equality operator. 
* 
* @since v6.0 */
	{
public:
	/** Constructs a new Signing Algorithm Identifier object, copying an existing Signing 
	* Algorithm Identifier object.
	* 
	* @param aSigningAlgorithmIdentifier	The Signing Algorithm Identifier object.
	* @return								The new Signing Algorithm Identifier object. */
	IMPORT_C static CSigningAlgorithmIdentifier* NewL(const CSigningAlgorithmIdentifier& aSigningAlgorithmIdentifier);
	
	/** Constructs a new Signing Algorithm Identifier object, copying an existing Signing 
	* Algorithm Identifier object, and puts a pointer to it onto the cleanup stack.
	* 
	* @param aSigningAlgorithmIdentifier	The Signing Algorithm Identifier object.
	* @return 								The new Signing Algorithm Identifier object. */
	IMPORT_C static CSigningAlgorithmIdentifier* NewLC(const CSigningAlgorithmIdentifier& aSigningAlgorithmIdentifier);
	
	/** Tests whether the Signing Algorithm Identifier object is equal to the specified 
	* Signing Algorithm Identifier object.
	* 
	* @param aSigningAlgorithmIdentifier  The Signing Algorithm Identifier object to be compared.
	* @return 							  ETrue, if this object's Signing Algorithm Identifier value 
	* 									  is equal to the specified Signing Algorithm Identifier 
	*									  object's value; EFalse, otherwise. */
	IMPORT_C TBool operator == (const CSigningAlgorithmIdentifier& aSigningAlgorithmIdentifier) const;
	
	/** Gets the signature ID of the asymmetric algorithm.
	* 
	* @return	The signature ID of the asymmetric algorithm. */
	IMPORT_C const CAlgorithmIdentifier& AsymmetricAlgorithm() const;
	
	/** Gets the signature ID of the digest algorithm.
	* 
	* @return	The signature ID of the digest algorithm. */
	IMPORT_C const CAlgorithmIdentifier& DigestAlgorithm() const;
	
	/** Destructor.
	* 
	* Frees all resources owned by the object, prior to its destruction. */
	IMPORT_C ~CSigningAlgorithmIdentifier();

protected:
	/** Second-phase constructor.
	 * @internalAll
	 */
	void ConstructL(const CSigningAlgorithmIdentifier& aSigningAlgorithmIdentifier);
	
	/** The signature ID of the asymmetric algorithm. */
	CAlgorithmIdentifier* iAsymmetricAlgorithm;

	/** The signature ID of the digest algorithm. */
	CAlgorithmIdentifier* iDigestAlgorithm;
	};

class CSubjectPublicKeyInfo : public CBase
/** A base class for a container that holds information about a subject public key.
* 
* It contains the algorithm ID, the encoded public key and the encoded parameters. 
* 
* @since v6.0 
*/
//algorithm ID + encoded public key + encoded parameters
	{
public:
	/** Creates a new subject public key object copied from an existing object.	
	* 
	* @param aSubjectPublicKeyInfo 	The subject public key object to be copied.
	* @return 						A pointer to the new public key object. */
	IMPORT_C static CSubjectPublicKeyInfo* NewL(const CSubjectPublicKeyInfo& aSubjectPublicKeyInfo);
	
	/** Creates a new subject public key object copied from an existing object and 
	* puts a pointer to the new object onto the cleanup stack.	
	* 
	* @param aSubjectPublicKeyInfo 	The subject public key object to be copied.
	* @return 						A pointer to the new public key object. */
	IMPORT_C static CSubjectPublicKeyInfo* NewLC(const CSubjectPublicKeyInfo& aSubjectPublicKeyInfo);
	
	/** Gets the algorithm ID.
	* 
	* @return	The algorithm ID. */
	IMPORT_C TAlgorithmId AlgorithmId() const;
	
	/** Gets the encoded parameters required by the algorithm.	
	* 
	* @return	A non-modifiable pointer descriptor representing the encoded parameters. */
	IMPORT_C const TPtrC8 EncodedParams() const;
	
	/** Gets the encoded public key data.
	* 
	* @return 	A non-modifiable pointer descriptor representing the encoded public 
	* 			key data. */
	IMPORT_C const TPtrC8 KeyData() const;
	
	/** Destructor.
	*
	* Frees all resources owned by the object. */
	IMPORT_C ~CSubjectPublicKeyInfo();
protected:
	/** Second-phase constructor.
	* 
	* @param aSubjectPublicKeyInfo	The subject public key object to be copied. */
	IMPORT_C virtual void ConstructL(const CSubjectPublicKeyInfo& aSubjectPublicKeyInfo);
	
	/** The algorithm ID. */
	CAlgorithmIdentifier* iAlgId;
	
	/** A heap descriptor representing the encoded key data. */
	HBufC8* iEncodedKeyData;
	};

class CRSASignatureResult : public CBase
/** The RSA public key algorithm signature result.
* 
* Derived classes:
* @li CWTLSRSASignatureResult
* @li CPKCS1SignatureResult.
*
* @see TKeyFactory::RSASignatureResultL()
*
* @since v6.0 */
	{
public:
	/** Tests whether the signature result is valid.
	* 
	* @param aResult	The signature result.
	* @return			ETrue if the signature result is valid, otherwise EFalse. */
	IMPORT_C virtual TBool VerifyL(const TDesC8& aResult) = 0;
	
	/** Destructor.
	*
	* Frees all resources owned by the object. */
	IMPORT_C ~CRSASignatureResult();
protected:
	/** Compares this RSA Signature Result object with the specified RSA Signature 
	* Result object for equality.
	*
	* @param aResult 	The RSA Signature Result object to be compared.
	* @return 			ETrue, if they are the same; EFalse, otherwise. */
	IMPORT_C TBool operator == (const CRSASignatureResult& aResult) const;
	
	/** The digest algorithm ID. */
	CAlgorithmIdentifier* iDigestAlgorithm;
	
	/** A heap descriptor representing the digest algorithm. */
	HBufC8* iDigest;
	};

//signed object
class TKeyFactory
/** Constructs the public key objects used for signature verification from their 
* encoded binary form. 
* 
* @since v6.0 */
	{
public:
	/** Gets the RSA public key.
	* 
	* @param aEncoding 	A non-modifiable descriptor representing the entire encoding.
	* @return 			The RSA Public key. */
	virtual CRSAPublicKey* RSAPublicKeyL(const TDesC8& aEncoding) const = 0;
	
	/** Gets the RSA signature result.
	* 
	* @param aDigestAlgorithm 	The algorithm ID.
	* @param aDigest 			A non-modifiable descriptor representing the digest algorithm.
	* @return 					The RSA signature result. */
	virtual CRSASignatureResult* RSASignatureResultL(const CAlgorithmIdentifier& aDigestAlgorithm, TDesC8& aDigest) const = 0;
	
	/** Gets the DSA public key.
	* 
	* @param aParams 	The DSA parameters
	* @param aEncoding 	A non-modifiable descriptor representing the entire encoding.
	* @return 			The DSA public key. */
	virtual CDSAPublicKey* DSAPublicKeyL(const CDSAParameters& aParams, const TDesC8& aEncoding) const = 0;
	
	/** Gets the digital DSA signature given an encoding key.
	* 
	* @param aEncoding 	A non-modifiable descriptor representing the entire encoding.
	* @return 			The DSA signature. */
	virtual CDSASignature* DSASignatureL(const TDesC8& aEncoding) const = 0;
	
	/** Gets the DSA parameters.
	* 
	* @param aEncoding 	A non-modifiable descriptor representing the entire encoding.
	* @return 			The DSA parameters. */
	virtual CDSAParameters* DSAParametersL(const TDesC8& aEncoding) const = 0;
	//	New function for TKeyFactory API
	virtual CDSAPublicKey* DSAPublicKeyL(const TDesC8& aParamsEncoding, const TDesC8& aEncoding) const = 0;
	};

class CSigningKeyParameters : public CBase
/** Contains the parameter information required by some signing algorithms.
* 
* The DSA signing algorithm needs parameters as well as a key. Currently, this 
* class only contains DSA parameters. 
* 
* @since v6.0 */
	{
public:
	/** Creates a new signing key parameters object.
	* 
	* @return	A pointer to the new signing key parameters object. */
	IMPORT_C static CSigningKeyParameters* NewL();
	
	/** Creates a new signing key parameters object and puts a pointer to the new object 
	* onto the cleanup stack.
	* 
	* @return	A pointer to the new signing key parameters object. */
	IMPORT_C static CSigningKeyParameters* NewLC();
	
	/** Creates a new signing key parameters object copied from an existing object.
	* 
	* @param aParameters 	The signing key parameters object to be copied.
	* @return 				A pointer to the new parameters object. */
	IMPORT_C static CSigningKeyParameters* NewL(const CSigningKeyParameters& aParameters);
	
	/** Creates a new signing key parameters object copied from an existing object 
	* and puts a pointer to the new object onto the cleanup stack.
	* 
	* @param aParameters  The signing key parameters object to be copied.
	* @return 			  A pointer to the new signing key parameters object. */
	IMPORT_C static CSigningKeyParameters* NewLC(const CSigningKeyParameters& aParameters);
	
	/** Destructor.
	* 
	* Frees all resources owned by the object. */
	IMPORT_C ~CSigningKeyParameters();
	
	/** Sets the DSA parameters.
	* 
	* @param aParams	The DSA parameters. */
	IMPORT_C void SetDSAParamsL(const CDSAParameters& aParams);
	
	/** Gets the DSA parameters.
	* 
	* @return	The DSA parameters. 
	* @internalAll
	*/
	const CDSAParameters* DSAParams() const;
private:
	CSigningKeyParameters();
	void ConstructL(const CSigningKeyParameters& aParameters);
	CDSAParameters* iDSAParams;
	};


class CSignedObject : public CBase
/** Base class for certificates. 
* 
* @since v6.0 */
	{
public:	
	/** Verifies a signature using the specified encoded key.
	* 
	* @param aEncodedKey 	The encoded key. 
	* @return 				ETrue if the signature is valid, otherwise EFalse. */
	IMPORT_C TBool VerifySignatureL(const TDesC8& aEncodedKey) const;
	
	/** Verifies a signature using the specified encoded key and hash.
	* 
	* @param aEncodedKey 	The encoded key. 
	* @param aHash			The hash of the data to be validated.
	* @return 				ETrue if the signature is valid, otherwise EFalse. */
	IMPORT_C TBool VerifySignatureL(const TDesC8& aEncodedKey, const TDesC8& aHash) const;	
	
	/** Gets the digital signature.
	* 
	* @return	A non-modifiable pointer descriptor representing the digital signature. */
	IMPORT_C const TPtrC8 Signature() const;
	
	/** Gets the signed data.
	* 
	* @return	A non-modifiable pointer descriptor representing the signed data. */
	IMPORT_C virtual const TPtrC8 SignedDataL() const = 0;
	
	/** Gets the fingerprint.
	* 
	* The fingerprint returned is the SHA1 hash of the encoding of the entire object.
	* 
	* @return	A non-modifiable pointer descriptor representing the finger print. */
	IMPORT_C const TPtrC8 Fingerprint() const;
	
	/** Gets the entire encoding.
	* 
	* @return	A non-modifiable pointer descriptor representing the entire encoding. */
	IMPORT_C const TPtrC8 Encoding() const;
	
	/** Gets the signing algorithm ID used.
	* 
	* @return	The signing algorithm ID. */
	IMPORT_C const CSigningAlgorithmIdentifier& SigningAlgorithm() const;
	
	/** Externalises the encoding of the entire object to a write stream.
	* 
	* The fingerprint and the signed data can be regenerated after restoration.
	* 
	* The presence of this function means that the standard templated operator<<() 
	* can be used to externalise objects of this class.
	* 
	* @param aStream	Stream to which the object should be externalised. */
	IMPORT_C virtual void ExternalizeL(RWriteStream& aStream) const;
	
	/** Internalises the encoded object from a read stream.
	
	* The class makes use of a specification-specific parser class for extracting 
	* the various elements, that is provided by a subclass of CSignedObject. For 
	* this reason this function is pure virtual.
	* 
	* The presence of this function means that the standard templated operator>>() 
	* can be used to internalise objects of this class.
	* 
	* @param aStream	Stream from which the contents of the field should be internalised. */
	IMPORT_C virtual void InternalizeL(RReadStream& aStream) = 0;
	
	/** Sets the signing key parameters.
	* 
	* @param aParameters	The signing key parameters. */
	IMPORT_C void SetParametersL(const CSigningKeyParameters& aParameters);
	
	/** Gets the encoded data for the specified encoded data element, in the (to be 
	* signed) tbsCertificate data structure, of the signed object.
	* 
	* @param aIndex	The encoded data element position in the tbsCertificate data 
	* 				structure. See the enumeration: CX509Certificate::Anonymous.
	* @return 		The encoded data for the specified data element of the signed object. */
	IMPORT_C virtual const TPtrC8* DataElementEncoding(const TUint aIndex) const = 0;
	
	/** Destructor.
	* 
	* Frees all resources owned by the object. */
	IMPORT_C ~CSignedObject();

protected:
	/** Verifies a RSA signature using the specified encoded key.
	* 
	* @param aEncodedKey 	The encoded key. 
	* @return 				ETrue if the signature is valid, otherwise EFalse. 
	* @internalAll
	*/
	TBool VerifyRSASignatureL(const TDesC8& aEncodedKey) const;
	
	/** @internalAll */	
	TBool VerifyRSASignatureL(const TDesC8& aEncodedKey, const TDesC8& aHash) const;
	
	/** A pointer to a key factory object. */
	TKeyFactory* iKeyFactory;
	
	/** A heap descriptor representing the entire encoding. */
	HBufC8* iEncoding;
	
	/** The digital signature. */
	HBufC8* iSignature;
	
	/** The fingerprint.
	* 
	* The SHA1 hash of the encoding of the entire object. */
	HBufC8* iFingerprint;
	
	/** The signing key parameters */
	CSigningKeyParameters* iParameters;
	
	/** The signing algorithm ID. */
	CSigningAlgorithmIdentifier* iSigningAlgorithm;
	};

class CCertificate : public CSignedObject
/** A data structure that binds a public key to a given individual.
* 
* A certificate is a signed object, and adds a serial number, a validity period 
* and a subject public key.
* 
* This is a base class for classes that implement certificates of particular types. 
* 
* @since v6.0 */
	{
public:
	/** Destructor.
	* 
	* Frees all resources owned by the object. */
	IMPORT_C ~CCertificate();
	
	/** Gets the subject public key information.
	* 
	* @return	The subject public key information. */
	IMPORT_C const CSubjectPublicKeyInfo& PublicKey() const;
	
	/** Gets the serial number.
	* 
	* @return	A non-modifiable pointer descriptor representing the serial number. */
	IMPORT_C const TPtrC8 SerialNumber() const;
	
	/** Gets the validity period.
	* 
	* @return	The validity period. */
	IMPORT_C const CValidityPeriod& ValidityPeriod() const;
	
	/** Tests whether a certificate is self-signed.
	* 
	* @return	ETrue, if it is self-signed; EFalse, otherwise. */
	IMPORT_C virtual TBool IsSelfSignedL() const = 0;
	
	/** Gets the subject.
	* 
	* @return	A heap descriptor representing the subject. */
	IMPORT_C virtual HBufC* SubjectL() const = 0;
	
	/** Gets the issuer.
	* 
	* @return	A heap descriptor representing the issuer. */
	IMPORT_C virtual HBufC* IssuerL() const = 0;
	
	/** Gets the key identifier.
	* 
	* @return	The key identifier. */
	IMPORT_C virtual TKeyIdentifier KeyIdentifierL() const;

protected:
	/** The serial number. */
	HBufC8* iSerialNumber;

	/** The validity period. */
	CValidityPeriod* iValidityPeriod;

	/** The subject public key information. */
	CSubjectPublicKeyInfo* iSubjectPublicKeyInfo;
	};

#endif
