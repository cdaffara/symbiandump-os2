/*
* Copyright (c) 1998-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* X509 certificate implementation
*
*/


/**
 @file 
 @publishedAll
 @released
*/
 
#ifndef __X509CERT_H__
#define __X509CERT_H__

#include <e32base.h>
#include <e32std.h>
#include <s32std.h>
#include <signed.h>
#include <x500dn.h>
#include <asn1enc.h>

/* These OIDs have been defined for PKCS#1 with the 
 * superior reference to OID 1.2.840.113549.1.1
 * {iso(1) member-body(2) us(840) rsadsi(113549) pkcs(1) pkcs-1(1)}
 */ 
// RSA (PKCS #1 v1.5) key transport algorithm
_LIT(KRSA,"1.2.840.113549.1.1.1");
// MD2 with RSA Encryption
_LIT(KMD2WithRSA,"1.2.840.113549.1.1.2");
// RSA (PKCS #1 v1.5) with MD5 signature
_LIT(KMD5WithRSA,"1.2.840.113549.1.1.4");
// RSA (PKCS #1 v1.5) with SHA-1 signature 
_LIT(KSHA1WithRSA,"1.2.840.113549.1.1.5");
// SHA256 with RSA Encryption
_LIT(KSHA256WithRSA,"1.2.840.113549.1.1.11");
// SHA384 with RSA Encryption 
_LIT(KSHA384WithRSA,"1.2.840.113549.1.1.12");
// SHA512 with RSA Encryption 
_LIT(KSHA512WithRSA,"1.2.840.113549.1.1.13");
// SHA224 with RSA Encryption 
_LIT(KSHA224WithRSA,"1.2.840.113549.1.1.14");

/* 
 * These OIDs have been defined for PKIX with the 
 * superior reference to OID 1.2.840.10040.
 */
// Digital Signature Algorithm (DSA), also known as the Digital Signature Standard (DSS) 
_LIT(KDSA,"1.2.840.10040.4.1");
// ANSI X9.57 DSA signature generated with SHA-1 hash (DSA x9.30) 
_LIT(KDSAWithSHA1,	"1.2.840.10040.4.3");

/*
 * These OIDs have been defined for ANSI X9 standard with 
 * superior reference to oid 1.2.840.10046.
 * {iso(1) member-body(2) us(840) ansi-x942(10046) number-types(2)}
 */
// ANSI X9.42 Ephemeral-Static Diffie-Hellman key agreement algorithm 
_LIT(KDH,"1.2.840.10046.2.1");

/*
 * These OIDs have been defined for digest algorithms with 
 * superior reference to oid 1.2.840.113549.2
 * {iso(1) member-body(2) us(840) rsadsi(113549) digestAlgorithm(2)}
 */
_LIT(KMD5,"1.2.840.113549.2.5");
_LIT(KMD2,"1.2.840.113549.2.2");

/*
 * These OIDs have been defined for hash algorithms conforming NIST algorithms 
 * with superior reference to oid 2.16.840.1.101.3.4.2
 * {joint-iso-itu-t(2) country(16) us(840) organization(1) gov(101) csor(3) nistAlgorithm(4) hashAlgs(2)}
 */
_LIT(KSHA256,"2.16.840.1.101.3.4.2.1");
_LIT(KSHA384,"2.16.840.1.101.3.4.2.2");
_LIT(KSHA512,"2.16.840.1.101.3.4.2.3");
_LIT(KSHA224,"2.16.840.1.101.3.4.2.4");

/*
 * These OIDs are for OIW Security Special Interest Group defined algorithms  
 * with superior reference to oid 1.3.14.3.2
 * {iso(1) identified-organization(3) oiw(14) secsig(3) algorithms(2)}
 */
// SHA1 with RSA signature (obsolete)
_LIT(KSHA1WithRSASignature,"1.3.14.3.2.29");
// Hash algorithm identifier SHA-1 (Secure Hash Algorithm, Revision 1) 
_LIT(KSHA1,"1.3.14.3.2.26");

/** The maximum number of allowed data elements.
* 
* @since v7.0 */
const TInt KX509MaxDataElements = 10;

class CRSAPublicKey;
class CDSAPublicKey;
class CDSASignature;
class CDSAParameters;

class CPKCS1SignatureResult : public CRSASignatureResult
/** The signature result in PKCS#1 format.
* 
* @since v6.0 */
	{
public:
	/** Creates a new algorithm ID object copied from an existing object.
	* 
	* @param aDigestAlgorithm	The algorithm ID object to be copied.
	* @param aDigest			A non-modifiable descriptor representing the digest algorithm.
	* @return					A pointer to the new algorithm ID object. */
	IMPORT_C static CPKCS1SignatureResult* NewL(const CAlgorithmIdentifier& aDigestAlgorithm, const TDesC8& aDigest);
	
	/** Creates a new algorithm ID object copied from an existing
	* object, and puts a pointer to the new object onto the cleanup stack.
	* 
	* @param aDigestAlgorithm	The algorithm ID object to be copied.
	* @param aDigest			A non-modifiable descriptor representing the digest algorithm.
	* @return					A pointer to the new algorithm ID object. */
	IMPORT_C static CPKCS1SignatureResult* NewLC(const CAlgorithmIdentifier& aDigestAlgorithm, const TDesC8& aDigest);
	
	/** Tests whether the signature result is valid.
	* 
	* @param aResult	The signature result to be verified.
	* @return			ETrue, if the signature result is verified; EFalse, otherwise. */
	IMPORT_C virtual TBool VerifyL(const TDesC8& aResult);
private:
	void ConstructL(const CAlgorithmIdentifier& aDigestAlgorithm, const TDesC8& aDigest);
	TBool DoVerifyL(const TDesC8& aResult);
	};

class TX509KeyFactory : public TKeyFactory
/** Constructs the public key objects used for signature verification, from their 
* encoded binary form, for X.509 certificates.
* 
* @since v6.0 */
	{
public:
	/** Gets the RSA public key from the encoding key.
	* 
	* @param aEncoding	A non-modifiable descriptor representing the entire encoding.
	* @return			The RSA Public key. */
	IMPORT_C CRSAPublicKey* RSAPublicKeyL(const TDesC8& aEncoding) const;
	
	/** Gets the RSA signature result.
	* 
	* @param aDigestAlgorithm	The digest algorithm ID.
	* @param aDigest			A non-modifiable descriptor representing the digest algorithm.
	* @return					The RSA signature result. */
	IMPORT_C CRSASignatureResult* RSASignatureResultL(const CAlgorithmIdentifier& aDigestAlgorithm, TDesC8& aDigest) const;
	
	/** Gets the DSA public key from the encoding key.
	* 
	* @param aParamsEncoding	A non-modifiable descriptor representing 
	*							the encoded binary representation of the DSA parameters
	* @param aEncoding			A non-modifiable descriptor representing the entire encoding.
	* @return					The DSA public key. 
	* @since 					v8.0 */
	IMPORT_C CDSAPublicKey* DSAPublicKeyL(const TDesC8& aParamsEncoding, const TDesC8& aEncoding) const;
	
	/** Gets the digital DSA signature from the encoding key.
	* 
	* @param aEncoding	A non-modifiable descriptor representing the entire encoding.
	* @return			The DSA signature. */
	IMPORT_C CDSASignature* DSASignatureL(const TDesC8& aEncoding) const;
	
	/** Gets the DSA parameters from the encoding key.
	* 
	* @param aEncoding	A non-modifiable descriptor representing the entire encoding.
	* @return			The DSA parameters. */
	IMPORT_C CDSAParameters* DSAParametersL(const TDesC8& aEncoding) const;
	
	/** Gets the DSA public key from the encoding key.
	* 
	* @param aParams	The DSA parameters
	* @param aEncoding	A non-modifiable descriptor representing the entire encoding.
	* @return			The DSA public key. */
	IMPORT_C CDSAPublicKey* DSAPublicKeyL(const CDSAParameters& aParams, const TDesC8& aEncoding) const;
	};

class CX509ValidityPeriod : public CValidityPeriod
/** The period for which an X.509 certificate is valid.
* 
* @since v6.0 */
	{
public:
	/** Creates a new X.509 validity period object from the specified buffer containing 
	* the binary coded representation.
	* 
	* Initialises the object from its encoded binary form into an internal representation.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new validity period object. */
	IMPORT_C static CX509ValidityPeriod* NewL(const TDesC8& aBinaryData);
	
	/** Creates a new X.509 validity period object from the specified buffer containing 
	* the binary coded representation, and puts a pointer to it onto the cleanup stack.
	* 
	* Initialises the object from its encoded binary form into an internal representation.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new validity period object. */
	IMPORT_C static CX509ValidityPeriod* NewLC(const TDesC8& aBinaryData);
	
	/** Creates a new X.509 validity period object from the specified buffer containing 
	* the binary coded representation, starting at the specified offset.
	* 
	* Initialises the object from its encoded binary form into an internal representation.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new validity period object. */
	IMPORT_C static CX509ValidityPeriod* NewL(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Creates a new X.509 validity period object from the specified buffer containing 
	* the binary coded representation, starting at the specified offset, and puts 
	* a pointer to it onto the cleanup stack.
	* 
	* Initialises the object from its encoded binary form into an internal representation.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new validity period object. */
	IMPORT_C static CX509ValidityPeriod* NewLC(const TDesC8& aBinaryData, TInt& aPos);
private:
	CX509ValidityPeriod();
	void ConstructL(const TDesC8& aBinaryData, TInt& aPos);
	};

class CX509AlgorithmIdentifier : public CAlgorithmIdentifier
/** Algorithm identifier for an X.509 certificate.
*
* @since v6.0 */
	{
public:
	/** Creates a new X.509 algorithm identifier object from the specified buffer containing 
	* the binary coded representation.
	* 
	* The function initialises the object from its encoded binary form into an internal 
	* representation.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new algorithm identifier object. */
	IMPORT_C static CX509AlgorithmIdentifier* NewL(const TDesC8& aBinaryData);
	
	/** Creates a new X.509 algorithm identifier object from the specified buffer containing 
	* the binary coded representation, and puts a pointer to it onto the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new algorithm identifier object. */
	IMPORT_C static CX509AlgorithmIdentifier* NewLC(const TDesC8& aBinaryData);
	
	/** Creates a new X.509 algorithm identifier object from the specified buffer containing 
	* the binary coded representation, starting at the specified offset.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new algorithm identifier object. */
	IMPORT_C static CX509AlgorithmIdentifier* NewL(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Creates a new X.509 algorithm identifier object from the specified buffer containing 
	* the binary coded representation, starting at the specified offset, and puts 
	* a pointer to it onto the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new algorithm identifier object. */
	IMPORT_C static CX509AlgorithmIdentifier* NewLC(const TDesC8& aBinaryData, TInt& aPos);

	/**
	Creates a X509 Algorithm Identifier object from the given algorithm ID and the encoded parameter.
	@param aAlgorithmId The algorithm Id used to build the object.
	@param aEncodedParams The encoded parameter for the algorithm ID.	
	@return The fully constructed object.
	*/
	IMPORT_C static CX509AlgorithmIdentifier* NewL(TAlgorithmId aAlgorithmId, const TDesC8& aEncodedParams);

	/**
	Creates a X509 Algorithm Identifier object from the given algorithm ID and the encoded parameter
	and leaves the object on the cleanup stack.
	@param aAlgorithmId The algorithm Id used to build the object.
	@param aEncodedParams The encoded parameter for the algorithm ID.	
	@return The fully constructed object.
	*/
	IMPORT_C static CX509AlgorithmIdentifier* NewLC(TAlgorithmId aAlgorithmId, const TDesC8& aEncodedParams);

	/**
	Creates the ASN1 DER sequence of the X509 algorithm identifier object
	and leaves it on the cleanup stack.
	@return  ASN1 DER sequence of this object.
	*/
	IMPORT_C CASN1EncSequence* EncodeASN1DERLC() const;

private:
	CX509AlgorithmIdentifier();
	CX509AlgorithmIdentifier(TAlgorithmId& aAlgorithmId);
	void InitializeL(const TDesC8& aBinaryData, TInt& aPos);
	};

class CX509SigningAlgorithmIdentifier : public CSigningAlgorithmIdentifier
/** Encapsulates the IDs of the algorithms used for signing an X.509 certificate.
*
* @since v6.0 */
	{
public:
	/** Creates a new X.509 signing algorithm Id object from the specified buffer containing 
	* the binary coded representation.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new signing algorithm Id object. */
	IMPORT_C static CX509SigningAlgorithmIdentifier* NewL(const TDesC8& aBinaryData);
	
	/** Creates a new X.509 signing algorithm Id object from the specified buffer containing 
	* the binary coded representation, and puts a pointer to it onto the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new signing algorithm Id object. */
	IMPORT_C static CX509SigningAlgorithmIdentifier* NewLC(const TDesC8& aBinaryData);
	
	/** Creates a new X.509 signing algorithm Id object from the specified buffer containing 
	* the binary coded representation, starting at the specified offset.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new signing algorithm Id object. */
	IMPORT_C static CX509SigningAlgorithmIdentifier* NewL(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Creates a new X.509 signing algorithm Id object from the specified buffer containing 
	* the binary coded representation,starting at the specified offset, and puts 
	* a pointer to it onto the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new signing algorithm Id object. */
	IMPORT_C static CX509SigningAlgorithmIdentifier* NewLC(const TDesC8& aBinaryData, TInt& aPos);
	/** Creates a new X.509 signing algorithm Id object from the specified algorithm pair
	* 
	* @param aAsymmetricAlgorithm	The asymmetric algorithm
	* @param aDigestAlgorithm		The digest algorithm
	* @return						The new signing algorithm Id object. */
 	IMPORT_C static CX509SigningAlgorithmIdentifier* NewL(const CAlgorithmIdentifier& aAsymmetricAlgorithm, const CAlgorithmIdentifier& aDigestAlgorithm);
	/** Creates a new X.509 signing algorithm Id object from the specified algorithm pair
	* 
	* @param aAsymmetricAlgorithm	The asymmetric algorithm
	* @param aDigestAlgorithm		The digest algorithm
	* @return						The new signing algorithm Id object. */
 	IMPORT_C static CX509SigningAlgorithmIdentifier* NewLC(const CAlgorithmIdentifier& aAsymmetricAlgorithm, const CAlgorithmIdentifier& aDigestAlgorithm);
private:
	CX509SigningAlgorithmIdentifier();
	void ConstructL(const TDesC8& aBinaryData, TInt& aPos);
	void ConstructL(const CAlgorithmIdentifier& aAsymmetricAlgorithm, const CAlgorithmIdentifier& aDigestAlgorithm);
	};

class CX509SubjectPublicKeyInfo : public CSubjectPublicKeyInfo
/** X.509 subject public key information.
* 
* @since v6.0 */
	{
public:
	/** Creates a new X.509 subject public key object from the specified buffer containing 
	* the binary coded representation.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new subject public key object. */
	IMPORT_C static CX509SubjectPublicKeyInfo* NewL(const TDesC8& aBinaryData);
	
	/** Creates a new X.509 subject public key object from the specified buffer containing 
	* the binary coded representation, and puts a pointer to it onto the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new subject public key object. */
	IMPORT_C static CX509SubjectPublicKeyInfo* NewLC(const TDesC8& aBinaryData);
	
	/** Creates a new X.509 subject public key object from the specified buffer containing 
	* the binary coded representation, starting at the specified offset.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The subject public key object. */
	IMPORT_C static CX509SubjectPublicKeyInfo* NewL(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Creates a new X.509 subject public key object from the specified buffer containing 
	* the binary coded representation, starting at the specified offset, and puts 
	* a pointer to it onto the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new subject public key object. */
	IMPORT_C static CX509SubjectPublicKeyInfo* NewLC(const TDesC8& aBinaryData, TInt& aPos);
private:
	CX509SubjectPublicKeyInfo();
	void ConstructL(const TDesC8& aBinaryData, TInt& aPos);
	};

class CX509CertExtension : public CBase
/** A generic X.509 certificate extension.
* 
* The creation of a generic extension is the first step in the creation of a 
* specific extension. Client code gets the decoded Object Identifier (OID) encapsulated 
* by an instance of this class and uses it to create the specific extension.
* 
* Consists of an any-defined-by structure along with a boolean flag saying whether 
* the extension is critical or not.
* 
* @since v6.0 */
	{
public:
	/** Creates a new generic X.509 certificate extension object from an existing object.
	* 
	* This is equivalent to a copy constructor.
	* 
	* @param aExtension	The generic certificate extension to be copied.
	* @return			The new generic certificate extension object. */
	IMPORT_C static CX509CertExtension* NewL(const CX509CertExtension& aExtension);
	
	/** Creates a new generic X.509 certificate extension object from an existing object, 
	* and puts a pointer to it onto the cleanup stack.
	* 
	* This is equivalent to a copy constructor.
	* 
	* @param aExtension	The generic certificate extension to be copied.
	* @return			The new generic X.509 certificate extension object. */
	IMPORT_C static CX509CertExtension* NewLC(const CX509CertExtension& aExtension);
	
	/** Creates a new generic X.509 certificate extension object from the specified 
	* buffer containing the binary coded representation.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new generic X.509 certificate extension object. */
	IMPORT_C static CX509CertExtension* NewL(const TDesC8& aBinaryData);
	
	/** Creates a new generic X.509 certificate extension object from the specified 
	* buffer containing the binary coded representation, and puts a pointer to it 
	* onto the cleanup stack .
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new generic X.509 certificate extension object. */
	IMPORT_C static CX509CertExtension* NewLC(const TDesC8& aBinaryData);
	
	/** Creates a new generic X.509 certificate extension object from the specified 
	* buffer containing the binary coded representation, starting at the specified offset.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new generic X.509 certificate extension object. */
	IMPORT_C static CX509CertExtension* NewL(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Creates a new generic X.509 certificate extension object from the specified 
	* buffer containing the binary coded representation, starting at the specified 
	* offset, and puts a pointer to it onto the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new generic X.509 certificate extension object. */
	IMPORT_C static CX509CertExtension* NewLC(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Creates a new generic X.509 certificate extension object from the specified 
	* extension object id, the critcal flag and the extension data.
	* 
	* @param aCertExtOID	The OID of the certficate extension.
	* @param aCritical		Flag to convey criticality of the extension.
	* @param aCertExtValue	The data of the specific extension.
	* @return				The new generic X.509 certificate extension object. */
	IMPORT_C static CX509CertExtension* NewL(const TDesC& aCertExtOID, 
							const TBool aCritical,
							const TDesC8& aCertExtValue);

	/** Creates a new generic X.509 certificate extension object from the specified 
	* extension object id, the critcal flag and the extension data, and puts a 
	* pointer to it onto the cleanup stack.
	* 
	* @param aCertExtOID	The OID of the certficate extension.
	* @param aCritical		Flag to convey criticality of the extension.
	* @param aCertExtValue	The data of the specific extension.
	* @return				The new generic X.509 certificate extension object. */
	IMPORT_C static CX509CertExtension* NewLC(const TDesC& aCertExtOID, 
							const TBool aCritical,
							const TDesC8& aCertExtValue);

	/** Destructor.
	* 
	* Frees all resources owned by the object, prior to its destruction. */
	IMPORT_C ~CX509CertExtension();
	
	/** Tests whether certificate processing code must process this extension for certificate 
	* validation to succeed.
	* 
	* @return	ETrue, if this extension must be processed for validation to succeed; 
	* 			EFalse, otherwise. */
	IMPORT_C TBool Critical() const;
	
	/** Gets the Object Identifier (OID) of the certficate extension.
	* 
	* @return	The OID of the certficate extension. */
	IMPORT_C TPtrC Id() const;	//OID for the extension
	
	/** Gets the encoded binary representation of the specific extension.
	* 
	* @return	A pointer descriptor representing the specific extension. */
	IMPORT_C TPtrC8 Data() const;	//the extension itself 

	/** Creates and returns a DER encoded X.509 certificate extension object in the form 
	* of a ASN.1 Sequence and puts it onto the cleanup stack. This method can be called
	* repeatedly to get copies of the sequence.
	* 
	* @return	A pointer to a DER encoded ASN.1 sequence */
	IMPORT_C CASN1EncSequence * EncodeASN1DERLC() const;

private:
	CX509CertExtension();
	void ConstructL(const TDesC8& aBinaryData, TInt& aPos);
	void ConstructL(const CX509CertExtension& aExtension);
	void ConstructL(const TDesC& aCertExtOID, 
					const TBool aCritical,
					const TDesC8& aCertExtValue);
	HBufC* iId;
	TBool iCritical;
	HBufC8* iData;
	};

class CX509Certificate : public CCertificate
/** An X.509 certificate.
* 
* @since v6.0 */
	{
public:
/** Enumerates values for encoded data element positions in the TBSCertificate data structure.
* 
* These values are to be used as parameters to the DataElementEncoding() function. */
	enum
		{
		EVersionNumber = 0,	//optional - may be NULL
		ESerialNumber = 1,
		EAlgorithmId = 2,
		EIssuerName = 3,
		EValidityPeriod = 4,
		ESubjectName = 5,
		ESubjectPublicKeyInfo = 6,
		EIssuerUID = 7,		//optional - may be NULL
		ESubjectUID = 8,	//optional - may be NULL
		EExtensionList = 9	//optional - may be NULL
		};
	
	/** Creates a new X.509 certificate object from the specified buffer containing 
	* the binary coded representation.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new X.509 certificate object. */
	IMPORT_C static CX509Certificate* NewL(const TDesC8& aBinaryData);
	
	/** Creates a new X.509 certificate object from the specified buffer containing 
	* the binary coded representation, and puts a pointer to it onto the cleanup stack.
	* 
	* Initialises the object from its encoded binary form into an internal representation.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new X.509 certificate object. */
	IMPORT_C static CX509Certificate* NewLC(const TDesC8& aBinaryData);

	/** Creates a new X.509 certificate object from the specified buffer containing 
	* the binary coded representation, starting at the specified offset.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new X.509 certificate object. */
	IMPORT_C static CX509Certificate* NewL(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Creates a new X.509 certificate object from the specified buffer containing 
	* the binary coded representation, starting at the specified offset, and puts 
	* a pointer to it onto the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new X.509 certificate object. */
	IMPORT_C static CX509Certificate* NewLC(const TDesC8& aBinaryData, TInt& aPos);

	/** Creates a new X.509 certificate object from the specified read stream.
	* 
	* @param aStream	Stream from which the contents should be internalised. 
	* @return			The new X.509 certificate object. */
	IMPORT_C static CX509Certificate* NewL(RReadStream& aStream);
	
	/** Creates a new X.509 certificate object from the specified read stream, and 
	* puts a pointer to it onto the cleanup stack.
	* 
	* Construction is from the stream.
	* 
	* @param aStream	Stream from which the contents should be internalised. 
	* @return			The new X.509 certificate object. */
	IMPORT_C static CX509Certificate* NewLC(RReadStream& aStream);

	/** Creates a new X.509 certificate object from an existing object.
	* 
	* This is equivalent to a copy constructor.
	* 
	* @param aCert	The X.509 certificate to be copied.
	* @return		The new X.509 certificate object. */
	IMPORT_C static CX509Certificate* NewL(const CX509Certificate& aCert);
	
	/** Creates a new X.509 certificate object from an existing object.
	* 
	* This is equivalent to a copy constructor.
	* 
	* @param aCert	The X.509 certificate to be copied.
	* @return		The new X.509 certificate object. */
	IMPORT_C static CX509Certificate* NewLC(const CX509Certificate& aCert);

   /** Retrieves the subject key identifier of a certificate based on the decision whether
	* it is a CA certificate or not. If it is a CA certificate then returns the stored subject key id 
	* as an extension in the certificate, any other certificate it returns the keyidentifier as 
	* calculated by Symbian's proprietary logic.
	* @return	The subject key identifier for the certificate. */
	IMPORT_C TKeyIdentifier SubjectKeyIdL() const;
				

	/** Destructor.
	* 
	* Frees all resources owned by the object, prior to its destruction. */
	IMPORT_C ~CX509Certificate();
	
	/** Tests whether the specified X.509 certificate is equal to this X.509 certificate.
	* 
	* X.509 certificates are equal if both the serial number and the issuer name 
	* are the same.
	* 
	* @param aCert	The X.509 certificate to be compared.
	* @return		ETrue, if the certificates are equal;EFalse, otherwise. */
	IMPORT_C TBool IsEqualL(const CX509Certificate& aCert) const;
	
	//extra accessors
	/** Gets the certificate's signed data.
	* 
	* @return	A non-modifiable pointer descriptor representing the certificate's 
	* 			signed data. */
	IMPORT_C const TPtrC8 SignedDataL() const;
	
	/** Gets the version number of the certificate.
	* 
	* @return	The version number of the certificate. */
	IMPORT_C TInt Version() const;
	
	/** Gets the X.500 Distinguished Name that identifies the issuer.
	* 
	* @return	The X.500 Distinguished Name that identifies the issuer. */
	IMPORT_C const CX500DistinguishedName& IssuerName() const;
	
	/** Gets the X.500 Distinguished Name that identifies the subject.
	* 
	* @return	The X.500 Distinguished Name that identifies the subject. */
	IMPORT_C const CX500DistinguishedName& SubjectName() const;
	
			//return all your generic extensions
	/** Gets all generic certificate extensions.
	* 
	* @return	The certificate extensions. */
	IMPORT_C const CArrayPtrFlat<CX509CertExtension>& Extensions() const;
		
		//return a particular extension: this is NOT OWNED by the client
		//returns NULL if the ext is not found
	/** Gets the certificate extension identified by the specified object identifier (OID).
	* 
	* @param aExtensionName	The OID identifying the extension.
	* @return				The certificate extension: Note that ownership is not transferred to 
	* 						the caller. */
	IMPORT_C const CX509CertExtension* Extension(const TDesC& aExtensionName) const;
	
	/** Internalises an object of this class from a read stream.
	* 
	* The presence of this function means that the standard templated operator>>() 
	* can be used to internalise objects of this class.
	* 
	* Note that the function has assignment semantics. It replaces the old value 
	* of the object with a new value read from the read stream.
	* 
	* @param aStream	Stream from which the object is to be internalised. */
	IMPORT_C void InternalizeL(RReadStream& aStream);
		
		//return the encoding for a data element at the index specified
	/** Gets the encoded data for the specified encoded data element (in the To Be 
	* Signed (TBS) certificate data structure) of the signed object.
	* 
	* @param aIndex	The encoded data element position in the TBSCertificate data 
	* 				structure. (See the CX509Certificate enumeration.)
	* @return 		The encoded data for the specified data element of the signed object. 
	* @since 		v7.0 */
	IMPORT_C virtual const TPtrC8* DataElementEncoding(const TUint aIndex) const;

public:	// from CCertificate
	/** Gets the issuer of the certificate.
	* 
	* @return 	A heap descriptor representing the issuer of the certificate. */
	IMPORT_C HBufC* IssuerL() const;
	
	/** Gets the subject of the certificate.
	* 
	* @return	A heap descriptor representing the issuer of the certificate. */
	IMPORT_C HBufC* SubjectL() const;
	
	/** Tests whether the certificate is self-signed.
	* 
	* @return	ETrue, if it is self-signed; EFalse, otherwise. */
	IMPORT_C TBool IsSelfSignedL() const;
	
	/** Gets a key identifier for the certificate. This is a unique identifier, calculated according 
	* to the recommended method of computing it from RFC3280, section 4.2.1.2. Please note that this
	* method does NOT return the value of the Subject Key Id extension, if it is present.
	* 
	* @return	A unique key identifier for the certificate. */
	IMPORT_C TKeyIdentifier KeyIdentifierL() const;
	
	/** Gets the subject key identifier for the certificate. This identifier is extracted from the
	* certificate (if the corresponding extension exists), or calculated (if the extension doesn't exist). If calculated, the recommendation from
	* section 4.2.1.2, RFC3280 is used (hash of the public key). Please note, that for subject key ids
	* extracted from the certificate there is a length limit - if the extension is longer than 160 bits,
	* it is ignored and the value is calculated instead.
	* 
	* @return	The subject key identifier for the certificate. */
	IMPORT_C TKeyIdentifier SubjectKeyIdentifierL() const;

private:
	CX509Certificate();
	void ConstructL(const TDesC8& aBinaryData, TInt& aPos);
	void ConstructL(const CX509Certificate& aCertificate);
	void ConstructCertL();
	void InitDataElementsL(const CX509Certificate& aCertificate);
	HBufC8* DecodeUidL(const TDesC8& aBinaryData, TBool& aHasElementAlready);
	void DecodeExtsL(const TDesC8& aBinaryData, TBool& aHasElementAlready);
	//private data
	TInt iVersion;
	CX500DistinguishedName* iIssuerName;
	CX500DistinguishedName* iSubjectName;
	HBufC8* iIssuerUid;
	HBufC8* iSubjectUid;
	CArrayPtrFlat<CX509CertExtension>* iExtensions;
	TFixedArray<TPtrC8*, KX509MaxDataElements>* iDataElements;
	};

#endif
