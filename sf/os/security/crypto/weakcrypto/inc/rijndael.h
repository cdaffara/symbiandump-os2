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
* Rijndael implementation
*
*/


/**
 @file 
 @publishedPartner
 @released 
*/
 
#ifndef __RIJNDAEL_H__
#define __RIJNDAEL_H__

#include "blocktransformation.h"

/**
* Abstract base class for Rijndael, implementing the parts of Rijndael common to both
* Rijndael encryption and decryption.
*
*/
class CRijndael : public CBlockTransformation
{
public:	//	From CBlockTransformation
	virtual void Reset(void);
	virtual TInt KeySize(void) const;
	/** The destructor frees all resources owned by the object, prior to its destruction. */
	IMPORT_C virtual ~CRijndael(void);
protected:
	/** Default constructor */
	IMPORT_C CRijndael(void);
	virtual void SetKey(const TDesC8& aKey);
	virtual void ConstructL(const TDesC8& aKey);
protected:
	/** 
	 * The key schedule 
	 *
	 * The maximum size is (((KAESMaxBlockSize/4)+6)+1)*4
	 */
	TUint32 iK[60];
	/** The number of rounds */
	TUint iRounds;
	/** 
	 * The input key 
	 *
	 * The key length (in bytes) must be one of the following:
	 * - KAESKeySize128 (=16)
	 * - KAESKeySize192 (=24)
	 * - KAESKeySize256 (=32).
	 */
	HBufC8* iKey;
private:
	CRijndael(const CRijndael&);
	const CRijndael& operator= (const CRijndael&);
};

/**
* Concrete class for AES encryption.
*
*/
class CAESEncryptor : public CRijndael
{
public:	//	From CBlockTransformation
	/**
	* Creates an instance of this class.
	*
	* @param aKey	The key to be used for encryption. The key length must be either
	*				KAESKeySize128 (=16), KAESKeySize192 (=24) or KAESKeySize256 (=32) bytes.
	*
	* @leave KErrKeyNotWeakEnough	If the key size is larger than that allowed by the
	*								cipher strength restrictions of the crypto library.
	*								See TCrypto::IsSymmetricWeakEnoughL()
	*/
	IMPORT_C static CAESEncryptor* NewL(const TDesC8& aKey);
	
	/**
	* Creates an instance of this class and leaves it on the cleanup stack.
	*
	* @param aKey	The key to be used for encryption. The key length must be either
	*				KAESKeySize128 (=16), KAESKeySize192 (=24) or KAESKeySize256 (=32) bytes.
	*
	* @leave KErrKeyNotWeakEnough	If the key size is larger than that allowed by the
	*								cipher strength restrictions of the crypto library.
	*								See TCrypto::IsSymmetricWeakEnoughL()
	*/
	IMPORT_C static CAESEncryptor* NewLC(const TDesC8& aKey);
	virtual TInt BlockSize() const;
	virtual void Transform(TDes8& aBlock);
protected:
	/** @internalAll */
	CAESEncryptor(void);
private:
	CAESEncryptor(const CAESEncryptor&);
	const CAESEncryptor& operator= (const CAESEncryptor&);
};

/**
* Concrete class for AES decryption.
*
*/
class CAESDecryptor : public CRijndael
{
public:	//	From CBlockTransformation
	/**
	* Creates an instance of this class.
	*
	* @param aKey	The key to be used for decryption. The key length must be either
	*				KAESKeySize128 (=16), KAESKeySize192 (=24) or KAESKeySize256 (=32) bytes.
	*
	* @leave KErrKeyNotWeakEnough	If the key size is larger than that allowed by the
	*								cipher strength restrictions of the crypto library. 
	*								See TCrypto::IsSymmetricWeakEnoughL()
	*/
	IMPORT_C static CAESDecryptor* NewL(const TDesC8& aKey);

	/**
	* Creates an instance of this class and leaves it on the cleanup stack.
	*
	* @param aKey	The key to be used for decryption. The key length must be either
	*				KAESKeySize128 (=16), KAESKeySize192 (=24) or KAESKeySize256 (=32) bytes.
	*
	* @leave KErrKeyNotWeakEnough	If the key size is larger than that allowed by the
	*								cipher strength restrictions of the crypto library.
	*								See TCrypto::IsSymmetricWeakEnoughL()
	*/
	IMPORT_C static CAESDecryptor* NewLC(const TDesC8& aKey);
	virtual TInt BlockSize() const;
	virtual void Transform(TDes8& aBlock);
protected:
	virtual void SetKey(const TDesC8& aKey);
	/** @internalAll */
	CAESDecryptor(void);
private:
	CAESDecryptor(const CAESDecryptor&);
	const CAESDecryptor& operator= (const CAESDecryptor&);
};


#endif	//	__RIJNDAEL_H__
