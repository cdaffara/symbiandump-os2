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
* DES encryptor and decryptor implementation
*
*/


/**
 @file 
 @publishedPartner
 @released 
*/
 
#ifndef __DATAENCRYPTIONSTANDARD_H__
#define __DATAENCRYPTIONSTANDARD_H__

#include <blocktransformation.h>
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <securityerr.h>
#endif

/** The size of the key schedule array (in 32-bit words).
* 
*/
const TUint KDESScheduleSizeInWords = 32;

/**
* Abstract base class for DES, implementing features common between DES encryption and
* decryption. From CBlockTransformation
* 
*/
class CDES : public CBlockTransformation
{
public:	
	virtual void Transform(TDes8& aBlock);
	virtual TInt BlockSize() const;
	virtual TInt KeySize() const;
	virtual void Reset();
	virtual ~CDES();
	
 	/**
	Indicates whether a supplied key is weak. If the key is one of the weak keys
 	defined by the crypto library (e.g. {0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01})
 	ETrue is returned.
 	@param aKey	    		The Key to be checked. The key length must be
 	 							KDESKeySize = 8 bytes.
 	@return					Whether the key is weak (ETrue or EFalse)					
 	*/
 	IMPORT_C static TBool IsWeakKey(const TDesC8& aKey);
	
protected:
	/** @internalAll */
	CDES();
	/** @internalAll */
	void DoTransform(TUint32& l, TUint32& r, const TUint32* aKey);
	virtual void SetKey(const TDesC8& aKey, TUint32* aKeyBuffer);
	virtual void ConstructL(const TDesC8& aKey, TBool aCheckWeakKey);
protected:
	/** 
	 * Key schedule array 
	 *
	 * Also used as the first key in triple-DES
	 */
	TUint32 iK1[KDESScheduleSizeInWords];	//	= 32
	/** 
	 * The initial key. 
	 *
	 * The key length must be KDESKeySize = 8 bytes.
	 */
	HBufC8* iKey;
};

/**
* Concrete class for DES encryption.
* 
*/
class CDESEncryptor : public CDES
{
public:
	/**
	* Creates an instance of this class.
	* 
	* @param aKey			The key to be used for encryption. The key length must be
	*						KDESKeySize = 8 bytes.
	* @param aCheckWeakKey	Boolean determining whether to check the key against
	*						a set of known weak key values. Defaults to ETrue. 
	* @return				A pointer to the new CDESEncryptor object.
	*
	* @leave KErrWeakKey			If the key is a weak one, the function leaves having
	*								previously cleaned up any previously allocated memory.
	* @leave KErrKeyNotWeakEnough	If the key size is larger than that allowed by the
	*								cipher strength restrictions of the crypto library.
	* 								See TCrypto::IsSymmetricWeakEnoughL()
	*/
	IMPORT_C static CDESEncryptor* NewL(const TDesC8& aKey, TBool aCheckWeakKey = ETrue);

	/**
	* Creates an instance of this class and leaves it on the cleanup stack.
	*
	* @param aKey			The key to be used for encryption. The key length must be
	*						KDESKeySize = 8 bytes.
	* @param aCheckWeakKey	Boolean determining whether to check the resultant key against
	*						a set of known weak key values. Defaults to ETrue. 
	* @return				A pointer to the new CDESEncryptor object.
	*
	* @leave KErrWeakKey			If the key is a weak one, the function leaves having
	*								previously cleaned up any previously allocated memory.
	* @leave KErrKeyNotWeakEnough	If the key size is larger than that allowed by the
	*								cipher strength restrictions of the crypto library.
	*								See TCrypto::IsSymmetricWeakEnoughL()
	*/
	IMPORT_C static CDESEncryptor* NewLC(const TDesC8& aKey, TBool aCheckWeakKey = ETrue);
protected:
	/** @internalComponent */
	CDESEncryptor(void);
};

/**
* Concrete class for DES decryption.
*
*/
class CDESDecryptor : public CDES
{
public:
	/**
	* Creates an instance of this class.
	*
	* @param aKey			The key to be used for decryption. The key length must be
	*						KDESKeySize = 8 bytes.
	* @param aCheckWeakKey	Boolean determining whether to check the resultant key against
	*						a set of known weak key values. Defaults to ETrue.
	* @return				A pointer to the new CDESDecryptor object.
	*
	* @leave KErrWeakKey			If the key is a weak one, the function leaves having
	*								previously cleaned up any previously allocated memory.
	* @leave KErrKeyNotWeakEnough	If the key size is larger than that allowed by the
	*								cipher strength restrictions of the crypto library.
	*								See TCrypto::IsSymmetricWeakEnoughL()
	*/
	IMPORT_C static CDESDecryptor* NewL(const TDesC8& aKey, TBool aCheckWeakKey = ETrue);

	/**
	* Creates an instance of this class and leaves it on the cleanup stack.
	* 
	* @param aKey			The key to be used for decryption. The key length must be
	*						KDESKeySize = 8 bytes.
	* @param aCheckWeakKey	Boolean determining whether to check the resultant key against
	*						a set of known weak key values. Defaults to ETrue.
	* @return				A pointer to the new CDESDecryptor object.
	*
	* @leave KErrWeakKey			If the key is a weak one, the function leaves having
	*								previously cleaned up any previously allocated memory.
	* @leave KErrKeyNotWeakEnough	If the key size is larger than that allowed by the
	*								cipher strength restrictions of the crypto library.
	*								See TCrypto::IsSymmetricWeakEnoughL()
	*/
	IMPORT_C static CDESDecryptor* NewLC(const TDesC8& aKey, TBool aCheckWeakKey = ETrue);
protected:	//	From CDES
	virtual void SetKey(const TDesC8& aKey, TUint32* aKeyBuffer);
	/** @internalComponent */
	CDESDecryptor(void);
};

#endif	//	__DATAENCRYPTIONSTANDARD_H__
