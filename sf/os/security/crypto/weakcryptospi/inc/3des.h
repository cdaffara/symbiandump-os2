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
* 3DES implementation
*
*/


/**
 @file 
 @publishedPartner
 @released 
*/
 
#ifndef __3DES_H__
#define __3DES_H__

#include <des.h>

/**
* Abstract base class for triple-DES.
*
* Implements features common to triple-DES encryption and decryption.
*
*/
class C3DES : public CDES
{
public:
	virtual void Transform(TDes8& aBlock);
	virtual void Reset();
	virtual TInt BlockSize() const;
	virtual TInt KeySize() const;
protected:
	/** @internalAll */
	C3DES();
	virtual void ConstructL(const TDesC8& aKey);
	/**
	 * Initialises the three key schedule arrays from the specified key.
	 * 
	 * @param aKey	The key to be used for encryption. The key length
	 *				must be K3DESKeySize = 24 bytes.
	 */
	virtual void DoSetKey(const TDesC8& aKey) = 0;
	
protected:
	/** The second key schedule array */
	TUint32 iK2[KDESScheduleSizeInWords];	//	= 32
	/** The third key schedule array */
	TUint32 iK3[KDESScheduleSizeInWords];
};

/**
* Concrete class for triple-DES encryption.
*
*/
class C3DESEncryptor : public C3DES
{
public:
	/**
	* Creates an instance of this class.
	*
	* @param aKey	The key to be used for encryption. The key length
	*				must be K3DESKeySize = 24 bytes.
	* @return		A pointer to the new C3DESEncryptor object.
	*
	* @leave KErrKeyNotWeakEnough	If the key size is larger than that allowed by the
	*								cipher strength restrictions of the crypto library.
	*								See TCrypto::IsSymmetricWeakEnoughL()
	*/
	IMPORT_C static C3DESEncryptor* NewL(const TDesC8& aKey);

	/**
	* Creates an instance of this class and leaves it on the cleanup stack.
	*
	* @param aKey	The key to be used for encryption. The key length
	*				must be K3DESKeySize = 24 bytes.
	* @return		A pointer to the new C3DESEncryptor object.
	*
	* @leave KErrKeyNotWeakEnough 	If the key size is larger than that allowed by the
	* 								cipher strength restrictions of the crypto library.
	* 								See TCrypto::IsSymmetricWeakEnoughL()
	*/
	IMPORT_C static C3DESEncryptor* NewLC(const TDesC8& aKey);
protected:
	virtual void DoSetKey(const TDesC8& aKey);
};

/**
* Concrete class for triple-DES decryption.
*
*/
class C3DESDecryptor : public C3DES
{
public:
	/**
	* Creates an instance of this class.
	*
	* @param aKey	The key to be used for decryption. The key length
	*				must be K3DESKeySize = 24 bytes.
	* @return		A pointer to the new C3DESDecryptor object.
	*
	* @leave KErrKeyNotWeakEnough	If the key size is larger than that allowed by the
	*								cipher strength restrictions of the crypto library.
	*								See TCrypto::IsSymmetricWeakEnoughL()
	*/
	IMPORT_C static C3DESDecryptor* NewL(const TDesC8& aKey);

	/**
	* Creates an instance of this class and leaves it on the cleanup stack.
	*
	* @param aKey	The key to be used for decryption. The key length
	*				must be K3DESKeySize = 24 bytes.
	* @return		A pointer to the new C3DESDecryptor object.
	*
	* @leave KErrKeyNotWeakEnough	If the key size is larger than that allowed by the
	*								cipher strength restrictions of the crypto library.  
	*								See TCrypto::IsSymmetricWeakEnoughL()
	*/
	IMPORT_C static C3DESDecryptor* NewLC(const TDesC8& aKey);
protected:
	virtual void DoSetKey(const TDesC8& aKey);
};

#endif	//	__3DES_H__
