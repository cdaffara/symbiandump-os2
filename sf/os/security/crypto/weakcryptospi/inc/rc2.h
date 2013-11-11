/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* ** IMPORTANT ** PublishedPartner API's in this file are published to 3rd party developers via the 
* Symbian website. Changes to these API's should be treated as PublishedAll API changes and the Security TA should be consulted.
* RC2 implementation
*
*/


/**
 @file 
 @publishedPartner
 @released 
*/
 
#ifndef __RC2_H__
#define __RC2_H__

#include <blocktransformation.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS

/** OpenSSL PKCS8 Effective Key Length Compatibility.*/
const TUint KPkcs8CompatibilityBits = 128;

/** PKCS12 PBE Effective Key Length Compatibility.*/
const TUint KPkcs12CompatibilityBits = 40;

#endif

/** The expanded key length of an RC2 key.*/
const TUint KRC2ExpandedKeyLen = 64;

/** SSL Effective Key Length Compatibility.*/
const TUint KSSLCompatibilityBits = 1024;

/** The maximum size in bytes for a RC2 key.*/
const TUint KRC2MaxKeySizeBytes = 128;	//	Max key size in this implementation = 128 bytes


/**
* Abstract base class for RC2 encipherment.
*
*/
class CRC2 : public CBlockTransformation
{
public:	
	virtual void Reset();
	virtual TInt BlockSize() const;
	virtual TInt KeySize() const;
protected:
	/** @internalAll */
	CRC2(void);
	virtual void SetKey(const TDesC8& aKey, TInt aEffectiveKeyLenBits);
protected:
	/**
	 * The expanded key buffer.
	 *
	 * Each iK[i] is a 16-bit word.
	 */
	TUint16 iK[KRC2ExpandedKeyLen];	//	128 bytes
	/** 
	 * The input key
	 * 
	 * The key length must fall between 1 and KRC2MaxKeySizeBytes (=128) bytes inclusive.
	 */
	TBuf8<KRC2MaxKeySizeBytes> iKey;
	/** The effective key length in bits */
	TInt iEffectiveKeyLenBits;
};

/**
* Concrete class for RC2 encryption.
*
*/
class CRC2Encryptor : public CRC2
{
public:
	/**
	* Creates an instance of this class.
	*
	* @param aKey					The key to be used for encryption. The key length must fall between 
	*								1 and KRC2MaxKeySizeBytes (=128) bytes inclusive.
	* @param aEffectiveKeyLenBits	Effective key length bits
	*								(defaults KSSLCompatibilityBits = 1024).
	*
	* @leave KErrKeyNotWeakEnough	If the key size is larger than that allowed by the
	*								cipher strength restrictions of the crypto library.
	*								See TCrypto::IsSymmetricWeakEnoughL()
	*/
	IMPORT_C static CRC2Encryptor* NewL(const TDesC8& aKey, 
		TInt aEffectiveKeyLenBits = KSSLCompatibilityBits);
	/**
	* Creates an instance of this class and leaves it on the cleanup stack.
	*
	* @param aKey					The key to be used for encryption. The key length must fall between 
	*								1 and KRC2MaxKeySizeBytes (=128) bytes inclusive.
	* @param aEffectiveKeyLenBits	Effective key length bits 
	*								(defaults KSSLCompatibilityBits = 1024).
	*
	* @leave KErrKeyNotWeakEnough	If the key size is larger than that allowed by the
	*								cipher strength restrictions of the crypto library.
	*								See TCrypto::IsSymmetricWeakEnoughL()
	*/
	IMPORT_C static CRC2Encryptor* NewLC(const TDesC8& aKey, 
		TInt aEffectiveKeyLenBits = KSSLCompatibilityBits);
	virtual void Transform(TDes8& aBlock);
protected:
	/** @internalAll */
	CRC2Encryptor(void);
};

/**
* Concrete class for RC2 decryption.
*
*/
class CRC2Decryptor : public CRC2
{
public:
	/**
	* Creates an instance of this class.
	*
	* @param aKey					The key to be used for decryption. The key length must fall between 
	*								1 and KRC2MaxKeySizeBytes (=128) bytes inclusive.
	* @param aEffectiveKeyLenBits	Effective key length bits 
	*								(defaults KSSLCompatibilityBits = 1024).
	*
	* @leave KErrKeyNotWeakEnough	If the key size is larger than that allowed by the
	*								cipher strength restrictions of the crypto library.
	*								See TCrypto::IsSymmetricWeakEnoughL()
	*/
	IMPORT_C static CRC2Decryptor* NewL(const TDesC8& aKey, 
		TInt aEffectiveKeyLenBits = KSSLCompatibilityBits);

	/**
	* Creates an instance of this class and leaves it on the cleanup stack.
	*
	* @param aKey					The key to be used for decryption. The key length must fall between 
	*								1 and KRC2MaxKeySizeBytes (=128) bytes inclusive.
	* @param aEffectiveKeyLenBits	Effective key length bits 
	*								(defaults KSSLCompatibilityBits = 1024).
	*
	* @leave KErrKeyNotWeakEnough	If the key size is larger than that allowed by the
	*								cipher strength restrictions of the crypto library.
	*								See TCrypto::IsSymmetricWeakEnoughL()
	*/
	IMPORT_C static CRC2Decryptor* NewLC(const TDesC8& aKey, 
		TInt aEffectiveKeyLenBits = KSSLCompatibilityBits);
	virtual void Transform(TDes8& aBlock);
protected:
	/** @internalAll */
	CRC2Decryptor(void);

};

#endif	//	__RC2_H__
