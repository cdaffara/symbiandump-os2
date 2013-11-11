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


#ifndef __ASNPKCS_H__
#define __ASNPKCS_H__

#include <e32std.h>
#include <e32base.h>
#include <s32file.h>
#include <signed.h>
#include <x509cert.h>
#include <mctkeystore.h>

/**
 * @file
 * @publishedPartner 
 * @released
 */

class CASN1EncSequence;
class CPBEncryptParms;

/**
 * This class provides the means to encode PKCS#5 parameters 
 * into an ASN1 sequence as specified in the PKCS#5 specifications.
 * 
 */
class TASN1EncPKCS5
	{
public:
	/**
	 * Returns an ASN1 sequence encoding the given PKCS#5 PBE parameters.
	 * The ASN1 syntax used is specified in the PKCS#5 v2.0  specifications. 
	 * Refer to the specs for a detailed description of the returned sequence.
	 *
	 * This class is used, for instance, by TASN1EncPKCS8 to specify the PBE
	 * parameters of encrypted private keys.
	 *
	 * @param aParms The PBE parameters to be encoded
	 *
	 * @return An ASN1 sequence encoding the given PBE parameters.
	 */
	IMPORT_C static CASN1EncSequence* EncodeDERL(const CPBEncryptParms& aParms);
	};


/**
 * This class provides the means to decode an ASN1 sequence encoding
 * PKCS#5 PBE parameters.
 * 
 */
class TASN1DecPKCS5
	{
public:
	/**
	 * Decodes a ASN1 sequence encoding PKCS#5 PBE parameters.
	 * The ASN1 syntax is specified in the PKCS#5 v2.0  specifications.
	 *
	 * @param aBinaryData A descriptor containing the ASN1 data in binary format.
	 *
	 * @return The decoded PBE parameters.
	 */
	IMPORT_C static CPBEncryptParms* DecodeDERL(const TDesC8& aBinaryData);
	};

//!
//!	Converts stored key data and key info to PKCS8 and returns ASN1 encoding thereof
//!	
class CDecPKCS8Data;

/// The minimum number of bytes necessary to determine that data is cleartext pkcs8
const TInt KIsPKCS8DataMinLength = 24;

/// The minimum number of bytes necessary to determine that data is encrypted pkcs8
const TInt KIsEncryptedPKCS8DataMinLength = 36;

/**
 * Provides the means to decode PKCS#8 encoded private keys.
 * 
 */
class TASN1DecPKCS8
	{
public:
	/**
	 * Decodes DER encoded ASN1 data representing a PKCS#8 clear text private key.
	 * See the PKCS#8 specifications for the ASN1 syntax.
	 *
	 * @param aBinaryData A descriptor containing the ASN1 data.
	 *
	 * @return A pointer to a CDecPKCS8Data object containing the decoded private key.
	 */
	IMPORT_C static CDecPKCS8Data* DecodeDERL(const TDesC8& aBinaryData);

	/**
	 * Decodes DER encoded ASN1 data representing a PKCS#8 encrypted  private key.
	 * See the PKCS#8 specifications for the ASN1 syntax.
	 *
	 * @param aBinaryData A descriptor containing the ASN1 data.
	 * @param aPassword The password to decrypt the key.
	 *
	 * @return A pointer to a CDecPKCS8Data object containing the decoded private key.
	 */
	IMPORT_C static CDecPKCS8Data* DecodeEncryptedDERL(const TDesC8& aBinaryData, const TDesC8& aPassword);

	/**
	 * Determines if some binary data is a pkcs#8 clear text private key.
	 * 
	 * @param aBinaryData A descriptor containing the data.  This must be at
	 * least KIsPKCS8DataMinLength bytes long.
	 *
	 * @return ETrue if binary data is pkcs#8 clear text private key or EFalse if it is not.
	 */
	IMPORT_C static TBool IsPKCS8Data(const TDesC8& aBinaryData);

	/**
	 * Determines if some binary data is an encrypted pkcs#8 private key.
	 * 
	 * @param aBinaryData A descriptor containing the data.
	 *
	 * @return ETrue if binary data is an encrypted pkcs#8 private key or EFalse if it is not.
	 */
	IMPORT_C static TBool IsEncryptedPKCS8Data(const TDesC8& aBinaryData);

private:
	static TBool IsASN1Sequence(const TDesC8& aBinaryData, TInt& aPos);
	static TBool IsExpectedData(const TDesC8& aBinaryData, TInt& aPos, const TDesC8& aExpectedData);
};
	

//!
//!	Server side object decodes a PKCS8 data object incoming from client
//!	On construction, decodes the data to determine version, key
//!	algorithm and gives access to the key data by creating the appropriate
//!	MPKCS8DecodedKeyPairData object for the algorithm
//!	
//!	PrivateKeyInfo ::= SEQUENCE {
//!	version Version,
//!	privateKeyAlgorithm PrivateKeyAlgorithmIdentifier,
//!	privateKey PrivateKey,
//!	attributes [0] IMPLICIT Attributes OPTIONAL }
//!	
//!	Version ::= INTEGER
//!	PrivateKeyAlgorithmIdentifier ::= AlgorithmIdentifier
//!	PrivateKey ::= OCTET STRING
//!	Attributes ::= SET OF Attribute
//!
class MPKCS8DecodedKeyPairData;	//	Forward declare


/**
 * This class provides the means to decode PKCS#8 encoded private keys.
 * 
 */
class CDecPKCS8Data : public CBase
{
public:
	/**
	 * @internalComponent
	 * 
	 * Decodes a ASN1 sequence encoding PKCS#8 encrypted  private key.
	 *
	 * @param aData A descriptor containing the data.
	 *
	 * @return A pointer to a CDecPKCS8Data object containing the decoded private key.
	 */
	static CDecPKCS8Data* NewL(const TDesC8& aData);
	
public:
	/** 
     * Destructor
     */
	virtual ~CDecPKCS8Data();
	
public:
	/*
	 * Returns the version number of the certificate.
	 *
	 * @return Version number of the certificate.
	 */
	inline TInt Version() const;
	
	/*
	 * Return the algorithm identifier.
	 *
	 * @return algorithm identifier.
	 */
	inline TAlgorithmId Algorithm() const;
	
	/*
	 * Returns the key pair data. This depends on the value returned by CDecPKCS8Data::Algorithm()
	 *
	 * @return either RSA or DSA to M class key pair data. 
	 */
	inline MPKCS8DecodedKeyPairData* KeyPairData() const;
	
	/*
	 * Returns a DER-encoded set of PKCS8 attributes (use TASN1DecSet to decode)
	 *
	 * @return a PKCS8 attributes
	 */
	inline const TDesC8& PKCS8Attributes() const;
	
protected:
	/** @internalComponent */
	CDecPKCS8Data();
	/** @internalComponent */
	void ConstructL(const TDesC8& aData);
	
private:	//	No copying
	CDecPKCS8Data(const CDecPKCS8Data&);
	CDecPKCS8Data& operator=(CDecPKCS8Data&);
	
private:
	TInt iVersion;
	TAlgorithmId iAlgorithmID;
	MPKCS8DecodedKeyPairData* iKeyPairData;
	HBufC8* iAttributes;
};


//!	Mixin class for generic actions to be performed on a keypair
//!	
//!
class MPKCS8DecodedKeyPairData
{
public:
	/** 
	 * Gets a key identifier
	 *
     * @param aKeyIdentifier A descriptor containing a key identifier (SHA1 hash of modulus)
     */
	virtual void GetKeyIdentifierL(TKeyIdentifier& aKeyIdentifier) const = 0;
	virtual TUint KeySize() const = 0;	
	virtual void Release() = 0;
	
protected:
	virtual ~MPKCS8DecodedKeyPairData();
};

class CRSAPublicKey;
class CRSAPrivateKey;
class TASN1DecGeneric;

//!	Represents an RSA key pair and provides the means to externalize it to
//!	a stream and generate a key identifier (SHA1 hash of modulus)
//!
class CPKCS8KeyPairRSA : public CBase, public MPKCS8DecodedKeyPairData
{
public:
	/**
	 * @internalComponent
	 *
	 * Constructs the ASN1 PKCS#8 RSA private key.	 
 	 *
 	 * @param aSource A descriptor containing the key identifier
 	 *
 	 * @return A pointer to a MPKCS8DecodedKeyPairData object containing the decoded private key.
	 */ 
	static MPKCS8DecodedKeyPairData* NewL(const TASN1DecGeneric& aSource);
	
public:
	/** 
     * Destructor
     */
	virtual ~CPKCS8KeyPairRSA();
	
public:
	/** 
	 * Gets a key identifier
	 *
     * @param aKeyIdentifier A descriptor containing a key identifier
     */
	virtual void GetKeyIdentifierL(TKeyIdentifier& aKeyIdentifier) const;	
	virtual TUint KeySize() const;	
	virtual void Release();
	
	/** 
	 * Contructs a RSA Public Key
	 *
 	 * @return A RSA Public Key
     */
	inline const CRSAPublicKey& PublicKey() const;
		
	/** 
	 * Contructs a RSA Private Key
	 *
 	 * @return A RSA Private Key
     */
	inline const CRSAPrivateKey& PrivateKey() const;
	
protected:
	CPKCS8KeyPairRSA(){}	
	/** @internalComponent */
	void ConstructL(const TASN1DecGeneric& aSource);
	
private:
	CRSAPublicKey* iPublicKey;
	CRSAPrivateKey* iPrivateKey;
};


class CDSAPublicKey;
class CDSAPrivateKey;

//!	Represents a DSA key pair and provides the means to externalize it to
//!	a stream and generate a key identifier
//!
class CPKCS8KeyPairDSA : public CBase, public MPKCS8DecodedKeyPairData
{
public:
	/** 
	 * @internalComponent
	 * 
	 * Contructs the ASN1 PKCS#8 DSA private key
	 *
	 * @param aParamsData A block of PKCS#8 parameters data for DER data to decode
	 *
	 * @param aSource A descriptor containing a key identifier		
	 *
	 * @return A pointer to MPKCS8DecodedKeyPairData object containing the decoded key.
	 */
	static MPKCS8DecodedKeyPairData* NewL(const TDesC8& aParamsData, const TASN1DecGeneric& aSource);
	
public:
	/** 
     * Destructor
     */
	virtual ~CPKCS8KeyPairDSA();
	
public:
	/** 
	 * Gets a key identifier
	 *
     * @param aKeyIdentifier A descriptor containing a key identifier
     */
	virtual void GetKeyIdentifierL(TKeyIdentifier& aKeyIdentifier) const;
	virtual TUint KeySize() const;
	virtual void Release();
	
	/** 
	 * Contructs a DSA Public Key
	 *
 	 * @return A DSA Public Key
     */	
	inline const CDSAPublicKey& PublicKey() const;
	
	/** 
	 * Contructs a RSA Private Key
	 *
 	 * @return A RSA Private Key
     */	
	inline const CDSAPrivateKey& PrivateKey() const;
	
protected:
	CPKCS8KeyPairDSA(){}
	/** @internalComponent */
	void ConstructL(const TDesC8& aParamsData, const TASN1DecGeneric& aSource);
	
private:
	CDSAPublicKey* iPublicKey;
	CDSAPrivateKey* iPrivateKey;
};

class CRSAPrivateKeyCRT;
class CASN1EncOctetString;
class CASN1EncBase;
class CPBEncryptParms;
class CPBEncryptor; 
class RInteger;

/**
 * Encodes the given private key using the pkcs#8 standard.
 *
 * The returned ASN1 sequence respects the following grammar:
 * 
 *	PrivateKeyInfo ::= SEQUENCE {
 *	version Version,
 *	privateKeyAlgorithm PrivateKeyAlgorithmIdentifier,
 *	privateKey PrivateKey,
 *	attributes [0] IMPLICIT Attributes OPTIONAL }
 *	
 *	Version ::= INTEGER
 *	PrivateKeyAlgorithmIdentifier ::= AlgorithmIdentifier
 *	PrivateKey ::= OCTET STRING
 *	Attributes ::= SET OF Attribute
 *
 * The PrivateKeyAlgorithmIdentifier format it depends on the 
 * specific algorithm it represents. For RSA is specified in 
 * the PKCS#1 document, for DSA in PKCS#11.
 *
 * Or, in the case of encrypted private keys:
 *
 *	EncryptedPrivateKeyInfo ::= SEQUENCE {
 *	encryptionAlgorithm EncryptionAlgorithmIdentifier,
 *	encryptedData EncryptedData }
 *	
 *	EncryptionAlgorithmIdentifier ::= AlgorithmIdentifier
 *	EncryptedData ::= OCTET STRING
 *
 * AlgorithmIdentifier is the ASN1 sequence defined in the
 * PKCS#5 standard. 
 */	
class TASN1EncPKCS8
	{
public:
	/**
	 * Returns the ASN1 PKCS#8 encoding of a RSA private key.
	 *
	 * The private key syntax for this key type is defined in 
	 * the PKCS#1 document. It follows the grammar:
	 *
	 * RSAPrivateKey ::= SEQUENCE {
	 *   version Version,
	 *   modulus INTEGER, -- n
	 *   publicExponent INTEGER, -- e
	 *	 privateExponent INTEGER, -- d
	 *	 prime1 INTEGER, -- p
	 *	 prime2 INTEGER, -- q
	 *	 exponent1 INTEGER, -- d mod (p-1)
	 *	 exponent2 INTEGER, -- d mod (q-1)
	 *	 coefficient INTEGER, -- (inverse of q) mod p
	 *	 otherPrimeInfos OtherPrimeInfos OPTIONAL
 	 *	}
 	 *
 	 * @param aPrivateKey The private key to be encoded (must be in CRT format)
 	 * @param aPublicKey The corresponding public key.
 	 * @param attributes A set of attributes of the extended information
 	 * 					 that is encrypted along with the private-key
 	 *					 information.
	 *
 	 * @return An ASN1 Sequence encoding the key.
	 */
	IMPORT_C static CASN1EncSequence* EncodeL(const CRSAPrivateKeyCRT& aPrivateKey,
	                                          const CRSAPublicKey& aPublicKey, const TDesC8& attributes);
	/**
	 * Encodes an RSA key in encrypted format.
	 * 
 	 * @param aPrivateKey The private key to be encoded (must be in CRT format)
 	 * @param aPublicKey The corresponding public key.
	 * @param aEncryptor The object used to encrypt the data.
	 * @param aData The encryption parameters of the given encryptor.
	 *              These parameters are stored in the resulting sequence.
 	 * @param attributes A set of attributes of the extended information
 	 * 					 that is encrypted along with the private-key
 	 *					 information.
	 *
 	 * @return An ASN1 Sequence encoding the encrypted key.
	 */
	IMPORT_C static CASN1EncSequence* EncodeEncryptedL(const CRSAPrivateKeyCRT& aPrivateKey, 
	                                                   const CRSAPublicKey& aPublicKey, 
	                                                   CPBEncryptor& aEncryptor, 
	                                                   CPBEncryptParms& aData, const TDesC8& attributes);

	/**
	 * Returns the ASN1 PKCS#8 encoding of a DSA private key.
	 *
	 * The private key syntax for this key type is defined in 
	 * the PKCS#11 document. 
	 *
 	 * @param aPrivateKey The private key to be encoded 
 	 * @param attributes A set of attributes of the extended information
 	 * 					 that is encrypted along with the private-key
 	 *					 information.
	 *
 	 * @return An ASN1 Sequence encoding the key.
	 */
	IMPORT_C static CASN1EncSequence* EncodeL(const CDSAPrivateKey& aPrivateKey, const TDesC8& attributes);

	/**
	 * Encodes a DSA key in encrypted format.
	 * 
 	 * @param aPrivateKey The private key to be encoded.
	 * @param aEncryptor The object used to encrypt the data.
	 * @param aData The encryption parameters of the given encryptor.
	 *              These parameters are stored in the resulting sequence.
 	 * @param attributes
	 *
 	 * @return An ASN1 Sequence encoding the encrypted key.
	 */
	IMPORT_C static CASN1EncSequence* EncodeEncryptedL(const CDSAPrivateKey& aPrivateKey, 
	                                                   CPBEncryptor& aEncryptor, 
	                                                   CPBEncryptParms& aData,
							  const TDesC8& attributes);

private:
	/**
	 *
	 * Converts the ASN1 element to an octet string.
	 *
	 * @param aAsnElement The ASN1 element to be converted
	 * 
	 * @return An ASN1 Octet string representing the input element.
	 */
 	static CASN1EncOctetString* ElementToOctetL(CASN1EncBase& aAsnElement);

	/**
 	 * Given a ASN1 sequence representing a private key and a CPBEncryptor object,
 	 * it returns an ASN1 octet string containing the key sequence encrypted by
 	 * the given encryptor.
 	 *
 	 * @param aKeySeq The key sequence to be encrypted.
  	 * @param aEncryptor The CPBEncryptor object used to encrypt the given key.
	 *
	 * @return An ASN1 octet string containing the encrypted key.
 	 */
	static CASN1EncOctetString* EncryptKeySequenceL(CASN1EncSequence& aKeySeq, 
	                                                CPBEncryptor& aEncryptor);

	/**
	 *
	 * Given a valid key sequence and appropriate PBE encryptors it 
	 * encrypts the key and creates a PKCS#8 sequence of type
	 * EncryptedPrivateKeyInfo.
	 *
	 * @param aPrivateKeySequence A ASN1 sequence of the private key to be 
	 *                            encrypted. Generally the structure of the
	 *                            sequence will depend on the key type.
	 * @param aEncryptor The PBE encryptor to be used to encrypt the key.
	 * @param aData The PBE encryptor parameters. This information must be 
	 *              included in the final ASN1 sequence.
	 *
	 * @return An ASN1 sequence of type EncryptedPrivateKeyInfo.
	 */
	static CASN1EncSequence* EncryptedSequenceL(CASN1EncSequence& aPrivateKeySequence, 
                                                    CPBEncryptor& aEncryptor, 
                                                    CPBEncryptParms& aData);

	/**
	* Given a CRT RSA private key it calculates the RSA private exponent "d".
	*
	* @param aPrivateKey The RSA private key in CRT format we are interested in.
	* @param aPublicKey The RSA public key
	*
	* @return The RSA private exponent "d".
	*/                                                    
    static const RInteger CalculateRSAPrivExpL(const CRSAPrivateKeyCRT& aPrivateKey, const CRSAPublicKey& aPublicKey);

	/**
	 * Adds the given DER encoded ASN1 structure to the given sequence. If the structure is KNullDesC8
	 * nothing is added. This method is used by the encoder to add the optional pkcs8 attributes to 
	 * the ASN1 pkcs8 key it generates. PKCS8 attributes are stored as uninterpreted DER encoded
	 * binary data in the keystore.
	 *
	 * @param aAttribute The DER encoded ASN1 structure.
	 * @param aSeq The sequence to which we want to add the attributes.
	 */
	static void AddAttributesL(CASN1EncSequence& aSeq, const TDesC8& aAttribute);
	};

// Inline function definition //

inline TInt CDecPKCS8Data::Version() const 
	{
	return (iVersion);
	}

inline TAlgorithmId CDecPKCS8Data::Algorithm() const 
	{
	return (iAlgorithmID);
	}

inline MPKCS8DecodedKeyPairData* CDecPKCS8Data::KeyPairData() const 
	{
	return (iKeyPairData);
	}


inline const TDesC8& CDecPKCS8Data::PKCS8Attributes() const 
	{
	if (iAttributes)
		return (*iAttributes);
	else
		return (KNullDesC8);
	}	

inline const CRSAPublicKey& CPKCS8KeyPairRSA::PublicKey() const
	{
	return *iPublicKey;
	}

inline const CRSAPrivateKey& CPKCS8KeyPairRSA::PrivateKey() const
	{
	return *iPrivateKey;
	}

inline const CDSAPublicKey& CPKCS8KeyPairDSA::PublicKey() const
	{
	return *iPublicKey;
	}

inline const CDSAPrivateKey& CPKCS8KeyPairDSA::PrivateKey() const
	{
	return *iPrivateKey;
	}



#endif	//	__ASNPKCS_H__
