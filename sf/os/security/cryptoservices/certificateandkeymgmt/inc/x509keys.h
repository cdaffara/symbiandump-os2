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
* X.509 key classes and utility classes for key encoding/decoding.
*
*/


/**
 @file
 @publishedAll
 @released
*/

#if !defined (__X509KEYS_H__)
#define __X509KEYS_H__

#include <e32base.h>
#include <e32std.h>
#include <asymmetrickeys.h>
#include <asymmetric.h>
#include <hash.h>
#include <bigint.h>
#include <signed.h>

// Forward declarations
class CASN1EncBase;
class CASN1EncContainer;
class CASN1EncSequence;
class CASN1EncBitString;


#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <x509keyencoder.h>
#endif

class CX509RSAPublicKey : public CRSAPublicKey
/** Adds the capability to decode DER-encoded RSA public keys.
*
* Adds a commitment to a specific encoding scheme allowing X.509 RSA public key 
* superclasses to remain encoding-independent.
*
*
* @since v6.0 
*/
	{
public:
	/** Creates a new RSA Public key object from the specified buffer containing the 
	* encoded binary representation.
	* 
	* Initialises the object from its encoded binary form into an internal representation.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				A pointer to the new CX509RSAPublicKey object. */
	IMPORT_C static CX509RSAPublicKey* NewL(const TDesC8& aBinaryData);
	
	/** Creates a new RSA Public Key object from the specified buffer containing the
	* encoded binary representation, and puts a pointer to it onto the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				A pointer to the new CX509RSAPublicKey object. */
	IMPORT_C static CX509RSAPublicKey* NewLC(const TDesC8& aBinaryData);
	
	/** Creates a new RSA Public key object from the specified buffer containing the 
	* encoded binary representation, starting at the specified offset.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				A pointer to the new CX509RSAPublicKey object. */
	IMPORT_C static CX509RSAPublicKey* NewL(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Creates a new RSA Public key object from the specified buffer containing the 
	* encoded binary representation, starting at the specified offset, and puts 
	* a pointer to it onto the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				A pointer to the new CX509RSAPublicKey object. */
	IMPORT_C static CX509RSAPublicKey* NewLC(const TDesC8& aBinaryData, TInt& aPos);
private:
	void ConstructL(const TDesC8& aBinaryData, TInt& aPos);
	CX509RSAPublicKey();
	};

class TASN1EncRSAPublicKey
/**
 * Class for encoding RSA public keys to ASN.1 encoding.
 * 
 * @since v8.0 
 */
	{
public:
	/** 
	 * Encodes the supplied public key into buffer in DER format ASN.1:
	 * @code
	 * 		SEQUENCE-OF
	 * 			INTEGER modulus
	 * 			INTEGER exponent
	 * @endcode
	 * 
	 * @param aKey	Key to encode.
	 * @return		Allocated buffer containing DER encoding of
	 *     			the supplied key aKey.
	 */
	IMPORT_C HBufC8* EncodeDERL(const CRSAPublicKey& aKey) const;
	};

class TASN1DecRSAPublicKey
/** 
 * Class for decoding RSA public keys from ASN.1 DER encoding.
 * 
 * @since v8.0
 */
	{
public:
	/** 
	 * Decodes an RSA key from the supplied buffer starting at the 
	 * specified position.
	 * 
	 * @param aDER	Buffer containing DER ASN.1 encoding of the key.
	 * @param aPos	Starting position in the buffer (updated on exit).
	 * @return		A pointer to the new CRSAPublicKey object.
	 */
	IMPORT_C CRSAPublicKey* DecodeDERL(const TDesC8& aDER, TInt& aPos) const;
	};

class TASN1DecRSAKeyPair
/** 
 * Class for decoding RSA key pairs from ASN.1 DER encoding.
 * 
 * @since v8.0
 */
	{
public:
	/**
	 * Decodes an RSA key pair from buffer containing ASN.1 
	 * DER-encoded private key. The encoding of a private key 
	 * contains public key components as well.
	 * 
	 * @param aDER			DER-encoded private key.
	 * @param aPos			Position in the buffer to start decoding 
	 *     					(updated on exit).
	 * @param aPublicKey	On return, the RSA public key object
	 * @param aPrivateKey	On return, the RSA private key object
	 * @param aKeyType		Key type, default is @c EStandardCRT
	 */
	IMPORT_C void DecodeDERL(const TDesC8& aDER, TInt& aPos, 
								CRSAPublicKey*& aPublicKey,
								CRSAPrivateKey*& aPrivateKey, 
								TRSAPrivateKeyType aKeyType = EStandardCRT);
	};

class CX509DSAPublicKey : public CDSAPublicKey
/** Encapsulates the X.509 DSA public key.
* 
* Adds a commitment to a specific encoding scheme allowing superclasses to remain 
* encoding-independent. 
* 
* @since v6.0 
*/
// DSA public key, params, signature.
	{
public:
	/** Creates a new X.509 DSA public key object.
	* 
	* @param aParamsData	A non-modifiable descriptor representing the entire encoding.
	* @param aBinaryData	The encoded binary representation.
	* @return				A pointer to the new CX509DSAPublicKey object. */
	IMPORT_C static CX509DSAPublicKey* NewL(const TDesC8& aParamsData, const TDesC8& aBinaryData);

	/** Creates a new X.509 DSA public key object, and puts a pointer to it onto the cleanup stack.
	* 
	* @param aParamsData	A non-modifiable descriptor representing the entire encoding.
	* @param aBinaryData	The encoded binary representation.
	* @return				A pointer to the new CX509DSAPublicKey object. */
	IMPORT_C static CX509DSAPublicKey* NewLC(const TDesC8& aParamsData, const TDesC8& aBinaryData);
	
	/** Creates a new X.509 DSA public key object.
	* 
	* @param aParams		The DSA parameters.
	* @param aBinaryData	The encoded binary representation. 
	* @return 				A pointer to the new CX509DSAPublicKey object. */
	IMPORT_C static CX509DSAPublicKey* NewL(const CDSAParameters& aParams, const TDesC8& aBinaryData);
	
	/** Creates a new X.509 DSA public key object, and puts a pointer to it onto the cleanup stack.
	* 
	* @param aParams		The DSA parameters.
	* @param aBinaryData	The encoded binary representation. 
	* @return 				A pointer to the new CX509DSAPublicKey object. */
	IMPORT_C static CX509DSAPublicKey* NewLC(const CDSAParameters& aParams, const TDesC8& aBinaryData);
	
	/** Creates a new X.509 DSA public key object.
	* 
	* @param aParamsData	A non-modifiable descriptor representing the entire encoding.
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The position from which to start decoding.
	* @return 				A pointer to the new CX509DSAPublicKey object. */
	IMPORT_C static CX509DSAPublicKey* NewL(const TDesC8& aParamsData, const TDesC8& aBinaryData,TInt& aPos);
	
	/** Creates a new X.509 DSA public key object, and puts a pointer to it onto the cleanup stack.
	* 
	* @param aParamsData	A non-modifiable descriptor representing the entire encoding.	
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The position from which to start decoding.
	* @return 				A pointer to the new CX509DSAPublicKey object. */
	IMPORT_C static CX509DSAPublicKey* NewLC(const TDesC8& aParamsData, const TDesC8& aBinaryData, TInt& aPos);
	
	/** Creates a new X.509 DSA public key object.
	* 
	* @param aParams		The DSA parameters.
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The position from which to start decoding. 
	* @return 				A pointer to the new CX509DSAPublicKey object. */
	IMPORT_C static CX509DSAPublicKey* NewL(const CDSAParameters& aParams, const TDesC8& aBinaryData, TInt& aPos);
	
	/** Creates a new X.509 DSA public key object, and puts a pointer to it onto the cleanup stack.
	* 
	* @param aParams 		The DSA parameters.
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The position from which to start decoding. 
	* @return				A pointer to the new CX509DSAPublicKey object. */
	IMPORT_C static CX509DSAPublicKey* NewLC(const CDSAParameters& aParams, const TDesC8& aBinaryData, TInt& aPos);
public:
	/** Gets the DSA parameters from the encoding key.
	* 
	* @param aParamsData	A non-modifiable descriptor representing the entire encoding.
	* @return 				The DSA parameters. */
	IMPORT_C static CDSAParameters* DSAParametersL(const TDesC8& aParamsData);
protected:
	/** @internalComponent */
	void ConstructL(const TDesC8& aParamsData, const TDesC8& aBinaryData, TInt& aPos);
	/** @internalComponent */
	void ConstructL(const CDSAParameters& aParams, const TDesC8& aBinaryData, TInt& aPos);
	/** @internalComponent */
	CX509DSAPublicKey();
	};
 
class TASN1DecDSAKeyPair
/** 
 * Class for decoding DSA key pairs from ASN.1 DER encoding.
 * 
 * @since v8.0
 */
	{
public:
	/**
	 * Decodes a DSA key pair from a buffer containing an ASN.1 
	 * DER-encoded private key. 
	 * 
	 * The encoding of the private key contains public key components as well. 
	 * 
	 * The DER encoding has the following format:
	 * @verbatim
	 *     SEQUENCE-OF
     *         INTEGER version (==0, ignored)
	 *         INTEGER p (public prime)
	 *         INTEGER q (160-bit public subprime, q | p-1)
	 *         INTEGER g (public generator of subgroup)
	 *         INTEGER x (private key)
	 *         INTEGER y (public key y=g^x)
	 * @endverbatim
	 * 
	 * @param aDER			DER-encoded private key.
	 * @param aPos			Position in the buffer to start decoding 
	 *		     			(updated on exit).
	 * @param aPublicKey	On return, the DSA public key object
	 * @param aPrivateKey	On return, the DSA private key object
	 */
	IMPORT_C void DecodeDERL(const TDesC8& aDER, TInt& aPos, 
								CDSAPublicKey*& aPublicKey, CDSAPrivateKey*& aPrivateKey);
	};

/**
 * Class for encoding DSA public keys to ASN.1 encoding.
 * 
 * @since v8.0
 */
class TASN1EncDSAPublicKey
	{
public:
	/** 
	 * Encodes the supplied public key into a buffer in DER format.
	 * 
	 * Note that the encoding has the following format:
	 * @code
	 *     SEQUENCE-OF
	 *         SEQUENCE-OF
	 *             INTEGER p
	 *             INTEGER q
	 *             INTEGER g
	 *         BIT STRING (encoded INTEGER public value)
	 * @endcode
	 * 
	 * @param aKey	Key to encode.
	 * @return		Sequence containing public key information.
	 */
	IMPORT_C CASN1EncSequence* EncodeDERL(const CDSAPublicKey& aKey) const;

	/**
	 * Encodes DSA parameters into an ASN.1 encoding structure suitable for 
	 * inclusion into other objects, like a PKCS#10 certificate request.
	 *
	 * Note that the encoding has the following form:
	 * @code
	 *     SEQUENCE-OF
	 *         INTEGER p
	 *         INTEGER q
	 *         INTEGER g
	 * @endcode
	 *
	 * @param aKey	DSA public key.
	 * @return		ASN.1 encoding structure on the cleanup stack.
	 */
	IMPORT_C CASN1EncSequence* EncodeParamsLC(const CDSAPublicKey& aKey) const;

	/** 
	 * Encodes a public key as a bit string.
	 *
	 * @param aKey 	DSA public key.
	 * @return	ASN.1 bit string (public key). This is left on the cleanup stack.
	 */

	IMPORT_C CASN1EncBitString* EncodePublicValueLC(const CDSAPublicKey& aKey) const;
	};

class CX509DSASignature : public CDSASignature
/** Encapsulates the X.509 DSA signature.
* 
* Adds a commitment to a specific encoding scheme allowing superclasses to remain 
* encoding-independent. 
* 
* @since v6.0 */
	{
public:
	/** Creates a new DSA Signature object from the specified buffer containing the 
	* encoded binary representation.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				A pointer to the new CX509DSASignature object. */
	IMPORT_C static CX509DSASignature* NewL(const TDesC8& aBinaryData);
	
	/** Creates a new DSA Signature object from the specified buffer containing the 
	* encoded binary representation, and puts a pointer to it onto the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				A pointer to the new CX509DSASignature object. */
	IMPORT_C static CX509DSASignature* NewLC(const TDesC8& aBinaryData);
	
	/** Creates a new DSA Signature object from the specified buffer containing the 
	* encoded binary representation, starting at the specified offset.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				A pointer to the new CX509DSASignature object. */
	IMPORT_C static CX509DSASignature* NewL(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Creates a new DSA Signature object from the specified buffer containing the 
	* encoded binary representation, starting at the specified offset, and puts 
	* a pointer to it onto the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				A pointer to the new CX509DSASignature object. */
	IMPORT_C static CX509DSASignature* NewLC(const TDesC8& aBinaryData, TInt& aPos);
private:
	void ConstructL(const TDesC8& aBinaryData, TInt& aPos);
	CX509DSASignature();
	};

class CX509DHPublicKey : public CDHPublicKey
/** Provides clients with the information they need for Diffie-Hellman key exchange 
* within a protocol. 
* 
* @since v6.0 */
	{
public:
	/** Creates a new CX509DHPublicKey object from the specified buffer containing the encoded 
	* binary representation.
	* 
	* @param aParamsData	A non-modifiable descriptor representing the entire encoding.
	* @param aKeyData 		
	* @return 				A pointer to the new CX509DHPublicKey object.*/
	IMPORT_C static CX509DHPublicKey* NewL(const TDesC8& aParamsData, const TDesC8& aKeyData);

	/** Creates a new CX509DHPublicKey object from the specified buffer containing the encoded 
	* binary representation, and puts a pointer to it onto the cleanup stack.
	* 
	* @param aParamsData	A non-modifiable descriptor representing the entire encoding.
	* @param aKeyData 		
	* @return 				A pointer to the new CX509DHPublicKey object.*/
	IMPORT_C static CX509DHPublicKey* NewLC(const TDesC8& aParamsData, const TDesC8& aKeyData);
public:
	/** Destructor.
	* 
	* Frees all resources owned by the object, prior to its destruction. */
	IMPORT_C virtual ~CX509DHPublicKey();
protected:
	/** @internalComponent */
	CX509DHPublicKey();
	/** @internalComponent */
	void ConstructL(const TDesC8& aParamsData, const TDesC8& aKeyData);
	};

class CX509DHKeyPair : public CDHKeyPair
/** This class represents the Diffie-Hellman Key Pair.
*
* @since v8.0 */
{
public:
	/** Creates a new DH key pair object from the specified buffer containing 
	* the encoded binary representation .
	*  
	* @param aParamsData	A non-modifiable descriptor representing the entire encoding.
	* @return				A pointer to the new CX509DHKeyPair object.
	*/
	IMPORT_C static CX509DHKeyPair* NewL(const TDesC8& aParamsData);

	/** Creates a new DH Key Pair object from the specified buffer containing the encoded binary  
	* representation, and puts a pointer to it onto the cleanup stack.
	*
	* @param aParamsData	A non-modifiable descriptor representing the entire encoding.
	* @return				A pointer to the new CX509DHKeyPair object.
	*/
	IMPORT_C static CX509DHKeyPair* NewLC(const TDesC8& aParamsData);
public:

	/** Virtual Destructor.
	* Frees all resources owned by the object, prior to its destruction. 
	*
	*/
	IMPORT_C virtual ~CX509DHKeyPair();
protected:
	/** @internalComponent */
	CX509DHKeyPair();
	/** @internalComponent */
	void ConstructL(const TDesC8& aParamsData);
};

class CX509DHValidationParams : public CBase
/** Validates Diffie-Hellman (DH) Domain parameters.
* 
* Provides access to the DH Validation Parameters, which are used to determine 
* if the DH Public Key has been generated in conformance with the algorithm 
* specified in ESDH (see RFC 2631). 
* 
* @since v6.0 */
	{
public:
	/** Creates a new DH Validation parameters object from the specified buffer containing 
	* the encoded binary representation.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				A pointer to the new CX509DHValidationParams object. */
	IMPORT_C static CX509DHValidationParams* NewL(const TDesC8& aBinaryData);
	
	/** Creates a new DH Validation parameters object from the specified buffer containing 
	* the encoded binary representation, and puts a pointer to it onto the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				A pointer to the new CX509DHValidationParams object. */
	IMPORT_C static CX509DHValidationParams* NewLC(const TDesC8& aBinaryData);
	
	/** Creates a new DH Validation parameters object from the specified buffer containing 
	* the encoded binary representation, starting at the specified offset.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				A pointer to the new CX509DHValidationParams object. */
	IMPORT_C static CX509DHValidationParams* NewL(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Creates a new DH Validation parameters object from the specified buffer containing 
	* the encoded binary representation, starting at the specified offset, and puts 
	* a pointer to it onto the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				A pointer to the new CX509DHValidationParams object. */
	IMPORT_C static CX509DHValidationParams* NewLC(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Gets a DSA prime generation seed.
	* 
	* @return	The bit string parameter used as the seed. */
	IMPORT_C const TPtrC8 Seed() const;
	
	/** Gets the output from a DSA prime generation counter.
	* 
	* @return	The integer value output. */
	IMPORT_C const TInteger& PGenCounter() const;
	
	/** Destructor.
	* 
	* Frees all resources owned by the object, prior to its destruction. */
	virtual ~CX509DHValidationParams();
protected:
	/** @internalComponent */
	CX509DHValidationParams();
	/** @internalComponent */
	void ConstructL(const TDesC8& aBinaryData, TInt& aPos);
	HBufC8* iSeed;
	RInteger iPGenCounter;
	};

class CX509DHDomainParams : public CBase
/** Encapsulates the compulsory Diffie-Hellman domain parameter values P and G 
* (See RFC 2459). 
* 
* @since v6.0 */
	{
public:
	/** Creates a new DH Domain parameters object from the specified buffer containing 
	* the encoded binary representation.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				A pointer to the new CX509DHDomainParams object. */
	IMPORT_C static CX509DHDomainParams* NewL(const TDesC8& aBinaryData);
	
	/** Creates a new DH Domain parameters object from the specified buffer containing 
	* the encoded binary representation, and puts a pointer to it onto the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				A pointer to the new CX509DHDomainParams object. */
	IMPORT_C static CX509DHDomainParams* NewLC(const TDesC8& aBinaryData);
	
	/** Creates a new DH Domain parameters object from the specified buffer containing 
	* the encoded binary representation, starting at the specified offset.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				A pointer to the new CX509DHDomainParams object. */
	IMPORT_C static CX509DHDomainParams* NewL(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Creates a new DH Domain parameters object from the specified buffer containing 
	* the encoded binary representation, starting at the specified offset, and puts 
	* a pointer to it onto the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				A pointer to the new CX509DHDomainParams object. */
	IMPORT_C static CX509DHDomainParams* NewLC(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Gets the compulsory parameter value P.
	* 
	* @return	The compulsory parameter value P. */
	IMPORT_C const TInteger& P() const;	
	
	/** Gets the compulsory parameter value G.
	* 
	* @return	The compulsory parameter value G. */
	IMPORT_C const TInteger& G() const;	

//the next 3 members are optional, in which case NULL is returned
//the returned objects remain the property of this object

//N.B. according to RFC 2459 the Q member is *not* optional, 
//however it is not essential for doing DH, and empirical studies
//suggest it doesn't get included much, so I'm relaxing the spec here
//to permit DomainParams objects which contain no Q.

	/** Gets the optional value Q.
	* 
	* @return	The optional value Q. */
	IMPORT_C const TInteger& Q() const;	
	
	/** Gets the optional value J.
	* 
	* @return	The optional value J. */
	IMPORT_C const TInteger& J() const;									
	
	/** Gets the optional validation parameters.
	* 
	* @return	The optional validation parameters. */
	IMPORT_C const CX509DHValidationParams* ValidationParams() const;
	
	/** Destructor.
	* 
	* Frees all resources owned by the object, prior to its destruction. */
	virtual ~CX509DHDomainParams();
protected:
	/** @internalComponent */
	CX509DHDomainParams();
	/** @internalComponent */
	void ConstructL(const TDesC8& aBinaryData, TInt& aPos);
	RInteger iP;
	RInteger iG;
	RInteger iQ;
	RInteger iJ;
	CX509DHValidationParams* iValidationParams;
	};

#endif
