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
* RSA shim classes definition
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef __RSASHIM_H__
#define __RSASHIM_H__

#include <asymmetric.h>

namespace CryptoSpi
	{
	class CAsymmetricCipher;
	class CKey;
	class CSigner;
	class CVerifier;
	}
	
NONSHARABLE_CLASS(CRSAPKCS1v15EncryptorShim) : public CRSAPKCS1v15Encryptor
{
public:
	/**
	Creates an RSAPKCS1v15EncryptorShim object which has the same interface
	as CRSAPKCS1v15Encryptor but delegates all work to a Crypto SPI plug-in.
	
	@param aKey The encryption key
	@return A pointer to a CRSAPKCS1v15EncryptorShim instance
	*/
	static CRSAPKCS1v15EncryptorShim* NewL(const CRSAPublicKey& aKey);

	/**
	Creates an RSAPKCS1v15EncryptorShim object which has the same interface
	as CRSAPKCS1v15Encryptor but delegates all work to a Crypto SPI plug-in.
	
	A pointer to the new object is placed on the cleanup stack
	
	@param aKey The encryption key
	@return A pointer to a CRSAPKCS1v15EncryptorShim instance
	*/
	static CRSAPKCS1v15EncryptorShim* NewLC(const CRSAPublicKey& aKey);
	
	// From CRSAPKCS1v15Encryptor
	void EncryptL(const TDesC8& aInput, TDes8& aOutput) const;
	TInt MaxInputLength(void) const;
	TInt MaxOutputLength(void) const;
	
	/// Destructor
	~CRSAPKCS1v15EncryptorShim();
	
private:
	/// Constructor
	CRSAPKCS1v15EncryptorShim(const CRSAPublicKey& aKey);
	void ConstructL(const CRSAPublicKey& aKey);
	
private:
	/// SPI delegate
	CryptoSpi::CAsymmetricCipher* iAsymmetricCipherImpl;
		
	/// SPI requires all key to passed as key-objects
	CryptoSpi::CKey* iKey;
};

NONSHARABLE_CLASS(CRSAPKCS1v15DecryptorShim) : public CRSAPKCS1v15Decryptor
{
public:
	/**
	Creates an RSAPKCS1v15DecryptorShim object which has the same interface
	as CRSAPKCS1v15Decryptor but delegates all work to a Crypto SPI plug-in.
	
	@param aKey The decryption key
	@return A pointer to a CRSAPKCS1v15DecryptorShim instance
	*/
	static CRSAPKCS1v15DecryptorShim* NewL(const CRSAPrivateKey& aKey);
	
	/**
	Creates an RSAPKCS1v15EncryptorShim object which has the same interface
	as CRSAPKCS1v15Decryptor but delegates all work to a Crypto SPI plug-in.
	
	A pointer to the new object is placed on the cleanup stack
	
	@param aKey The decryption key
	@return A pointer to a CRSAPKCS1v15DecryptorShim instance
	*/
	static CRSAPKCS1v15DecryptorShim* NewLC(const CRSAPrivateKey& aKey);
	
	// From CRSAPKCS1v15Decryptor
	void DecryptL(const TDesC8& aInput, TDes8& aOutput) const;
	TInt MaxInputLength(void) const;
	TInt MaxOutputLength(void) const;
	
	/// Destructor
	~CRSAPKCS1v15DecryptorShim();
	
private:
	/// Constructor
	CRSAPKCS1v15DecryptorShim(const CRSAPrivateKey& aKey);
	void ConstructL(const CRSAPrivateKey& aKey);
	
private:
	/// SPI delegate
	CryptoSpi::CAsymmetricCipher* iAsymmetricCipherImpl;
	
	/// SPI requires all key to passed as key-objects
	CryptoSpi::CKey* iKey;
};

NONSHARABLE_CLASS(CRSAPKCS1v15SignerShim) : public CRSAPKCS1v15Signer
	{
public:
	/**
	Creates a new CRSAPKCS1v15SignerShim object which has the same interface
	as CRSAPKCS1v15Signer but delegates all work to a Crypto SPI plug-in.

	 @param aKey	The RSA private key to be used for signing
	 @return A pointer to a CRSAPKCS1v15SignerShim instance
	 @leave KErrKeySize	If the key length is too small
	 */
	static CRSAPKCS1v15SignerShim* NewL(const CRSAPrivateKey& aKey);

	/**
	Creates a new CRSAPKCS1v15SignerShim object which has the same interface
	as CRSAPKCS1v15Signer but delegates all work to a Crypto SPI plug-in.

	 @param aKey	The RSA private key to be used for signing
	 @return A pointer to a CRSAPKCS1v15SignerShim instance
	 @leave KErrKeySize	If the key length is too small
	 */
	static CRSAPKCS1v15SignerShim* NewLC(const CRSAPrivateKey& aKey);

	// From CRSAPKCS1v15Signer
	virtual CRSASignature* SignL(const TDesC8& aInput) const;
	virtual TInt MaxInputLength(void) const;
	virtual TInt MaxOutputLength(void) const;
	/** The destructor frees all resources owned by the object, prior to its destruction.*/
	~CRSAPKCS1v15SignerShim(void);
protected:
	
	CRSAPKCS1v15SignerShim(const CRSAPrivateKey& aKey);
	void ConstructL(const CRSAPrivateKey& aKey);

protected:
	/// SPI delegate
	CryptoSpi::CSigner* iSignerImpl;

	/// SPI requires all key to passed as key-objects
	CryptoSpi::CKey* iKey;
private:
	CRSAPKCS1v15SignerShim(const CRSAPKCS1v15SignerShim&);
	CRSAPKCS1v15SignerShim& operator=(const CRSAPKCS1v15SignerShim&);
	};

/**
* This class verifies RSA signatures given a message and its supposed
* signature.  It follows the RSA PKCS#1 v1.5 with PKCS#1 v1.5 padding specification
* with the following exception: the VerifyL() function does <b>not</b> hash or
* in any way manipulate the input data before checking.  Thus in order to verify
* RSA signatures in PKCS#1 v1.5 format, the input data needs to follow PKCS#1 v1.5 
* specification, i.e. be ASN.1 encoded and prefixed  by ASN.1 encoded digestId.
* 
* @internalComponent
* @released 
*/
NONSHARABLE_CLASS(CRSAPKCS1v15VerifierShim) : public CRSAPKCS1v15Verifier
	{
public:
	/**
	@internalComponent
	
	Creates a new CRSAPKCS1v15VerifierShim object which has the same interface
	as CRSAPKCS1v15Verifier but delegates all work to a Crypto SPI plug-in.
	
	@param aKey	The RSA public key to be used for verifying
	@return A pointer to a CRSAPKCS1v15VerifierShim instance
	@leave KErrKeySize	If the key length is too small
	 */
	static CRSAPKCS1v15VerifierShim* NewL(const CRSAPublicKey& aKey);

	/**
	@internalComponent
	
	Creates a new CRSAPKCS1v15VerifierShim object which has the same interface
	as CRSAPKCS1v15Verifier but delegates all work to a Crypto SPI plug-in.

	 The returned pointer is put onto the cleanup stack.

	 @param aKey	The RSA public key to be used for verifying
	 @return A pointer to a CRSAPKCS1v15VerifierShim instance

	 @leave KErrKeySize	If the key length is too small
	 */
	static CRSAPKCS1v15VerifierShim* NewLC(const CRSAPublicKey& aKey);
	
	// CRSAPKCS1v15Verifier
	virtual TInt MaxInputLength(void) const;
	virtual TInt MaxOutputLength(void) const;

	// RSAVerifier
	virtual TBool VerifyL(const TDesC8& aInput, const CRSASignature& aSignature) const;
	virtual HBufC8* InverseSignLC(const CRSASignature& aSignature) const;

	/** The destructor frees all resources owned by the object, prior to its destruction. */
	virtual ~CRSAPKCS1v15VerifierShim(void);
protected:
	CRSAPKCS1v15VerifierShim(const CRSAPublicKey& aKey);
	void ConstructL(const CRSAPublicKey& aKey);

protected:
	/// SPI delegate
	CryptoSpi::CVerifier* iVerifierImpl;

	/// SPI requires all key to passed as key-objects
	CryptoSpi::CKey* iKey;
private:
	CRSAPKCS1v15VerifierShim(const CRSAPKCS1v15VerifierShim&);
	CRSAPKCS1v15VerifierShim& operator=(const CRSAPKCS1v15VerifierShim&);
	};

#endif // __RSASHIM_H__
