/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology 
*/

#if !defined (X509KEYENCODER_H)
#define X509KEYENCODER_H

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

class TX509KeyEncoder
/**
 * Abstract class that is the base class for RSA and DSA key encoder classes.
 * These classes are used to encode the X509 ASN.1 types AlgorithmIdentifier and
 * SubjectPublicKeyInfo.
 *
 * This class is part of the pkcs10 API, and will be changed or removed in a
 * future release.  You should not use it.
 * 
 */
	{
public:
	/**
	 * Constructor that takes an algorithm identifier and saves it into the 
	 * corresponding member variable. It is then used in the
	 * EncodeSignatureAlgorithm() function.
	 * 
	 * @param aDigestAlg	Digest algorithm to use. Currently the following 
	 * 						algorithms are supported: MD2, MD5, and SHA-1.
	 */
	TX509KeyEncoder(TAlgorithmId aDigestAlg);

	/**
	 * Produces the SubjectPublicKeyInfo encoding. 
	 * 
	 * The encoding has the following ASN.1 format:
	 * @code
	 * SubjectPublicKeyInfo {ALGORITHM : IOSet} ::= SEQUENCE {
	 *     algorithm        AlgorithmIdentifier {{IOSet}},
	 *     subjectPublicKey BIT STRING
	 *     }
	 * @endcode
	 */
	IMPORT_C virtual CASN1EncBase* EncodeKeyLC() const = 0;

	/**
	 * Produces the AlgorithmIdentifier encoding.
	 * 
	 * @return	ASN.1 sequence containing signature algorithm
	 */
	IMPORT_C virtual CASN1EncSequence* EncodeSignatureAlgorithmLC() const = 0;

	/**
	 * Produces the DigestAlgorithmIdentifier encoder.
	 *
	 * The encoding has the following ASN.1 format
	 * @code
     *   DigestAlgorithmIdentifier ::= SEQUENCE {
     *     algorithm AlgorithmIdentifier,
     *     parameters ANY DEFINED BY algorithm OPTIONAL }
     *
     *   AlgorithmIdentifier ::= OBJECT IDENTIFIER
	 * @endcode
 	 *
	 * @return Appropriate ASN.1 sequence of type <code>DigestAlgorithmIdentifier</code>
	 */
	IMPORT_C virtual CASN1EncSequence* EncodeDigestAlgorithmLC() const;

protected:
	/** Digest algorithm to use. */
	TAlgorithmId iDigestAlg;
	};

class TX509RSAKeyEncoder : public TX509KeyEncoder
/**
 * Subclasses TC509KeyEncoder to provides key encoding capability for RSA public keys.
 * 
 * This class is part of the pkcs10 API, and will be changed or removed in a
 * future release.  You should not use it.
 * 
 */
	{
public:
	/**
	 * Constructs a RSA key pair encoder, saving reference to the passed 
	 * key pair in the member variable.
	 * 
	 * @param aPublicKey	RSA public key to use for encoding.
	 * @param aDigestAlg	Digest algorithm to use.
	 */
	IMPORT_C TX509RSAKeyEncoder(const CRSAPublicKey& aPublicKey, TAlgorithmId aDigestAlg);

	/**
	 * Produces the SubjectPublicKeyInfo encoding. 
	 * 
	 * The resulting encoding has the following form:
	 * @code
	 *     SEQUENCE-OF
	 *         SEQUENCE-OF
	 *             OID of the encryption algorithm (KRSA)
	 *             NULL
	 *         BIT STRING encoded public key.
	 * @endcode
	 * 
	 * @return	DER-encoded public key information, placed on the cleanup stack.
	 */
	IMPORT_C virtual CASN1EncBase* EncodeKeyLC() const;

	/**
	 * Produces the AlgorithmIdentifier encoding.
  	 * 
	 * This has the following form:
	 * @code
	 *     SEQUENCE-OF
	 *         OID signature-algorithm
	 *         NULL
	 * @endcode
	 * 
	 * @return	ASN.1 sequence containing signature algorithm encoding, 
	 * 			placed on the cleanup stack.
	 */
	IMPORT_C virtual CASN1EncSequence* EncodeSignatureAlgorithmLC() const;

private:
	/**
	 * Saved reference to the RSA public key to be used for encoding.
	 */
	const CRSAPublicKey& iPublicKey;
	};

class TX509DSAKeyEncoder : public TX509KeyEncoder
/**
 * Provides key encoding and signing capability using a DSA public key.
 * 
 * This class is part of the pkcs10 API, and will be changed or removed in a
 * future release.  You should not use it.
 * 
 */
	{
public:
	/**
	 * Constructs a DSA key pair encoder, saving reference to the passed 
	 * public key in the member variable.
	 * 
	 * @param aKeyPublic	DSA public key to use for encoding.
	 * @param aDigestAlg	Digest algorithm to use.
	 */
	IMPORT_C TX509DSAKeyEncoder(const CDSAPublicKey& aKeyPublic, 
								TAlgorithmId aDigestAlg);

	/**
	 * Produces the SubjectPublicKeyInfo encoding. 
  	 * 
	 * The ASN.1 encoding of a DSA key has the following form:
	 * @code
	 * SEQUENCE-OF
	 *     SEQUENCE-OF
	 *         OID dsa (1.2.840.10040.4.1)
	 *         SEQUENCE-OF
	 *             INTEGER p
	 *             INTEGER q
	 *             INTEGER g
	 *     BIT STRING
	 *         INTEGER public value (y)
	 * @endcode
	 * 
	 * @return DER-encoded public key information, placed on the cleanup stack.
	 */
	IMPORT_C virtual CASN1EncBase* EncodeKeyLC() const;

	/**
	 * Produces the AlgorithmIdentifier encoding.
   	 * 
	 * This has the following form:
	 * @code
	 * SEQUENCE-OF
	 *     OID dsa-signature-oid
	 *     SEQUENCE-OF dsa-params
	 *         INTEGER p
	 *         INTEGER q
	 *         INTEGER g
	 * @endcode
	 * 
	 * @return	ASN.1 sequence containing signature algorithm encoding, 
	 *     		placed on the cleanup stack.
	 */
	IMPORT_C virtual CASN1EncSequence* EncodeSignatureAlgorithmLC() const;

private:
	/**
	 * Saved reference to the DSA public key to be used for encoding.
	 */
	const CDSAPublicKey& iPublicKey;
	};

#endif
