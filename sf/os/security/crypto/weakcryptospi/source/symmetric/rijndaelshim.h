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
* rijndael shim classes definition
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef __RIJNDAELSHIM_H__
#define __RIJNDAELSHIM_H__

#include <rijndael.h>

namespace CryptoSpi
	{
	class CSymmetricCipher;
	class CKey;
	}
	
NONSHARABLE_CLASS(CAESEncryptorShim) : public CAESEncryptor
{
public:
	/**
	Creates an CAESEncryptorShim object which has the same interface
	as AESEncryptor but delegates all work to a Crypto SPI plug-in.
	
	@param aKey The encryption key
	@return A pointer to a CAESEncryptorShim instance
	*/
	static CAESEncryptorShim* NewL(const TDesC8& aKey);
	
	/**
	Creates an CAESEncryptorShim object which has the same interface
	as AESEncryptor but delegates all work to a Crypto SPI plug-in.
	
	A pointer to the new object is placed on the cleanup stack
	
	@param aKey The encryption key
	@return A pointer to a CAESEncryptorShim instance
	*/
	static CAESEncryptorShim* NewLC(const TDesC8& aKey);
	
	// From CBlockTransform
	TInt BlockSize() const;
	void Transform(TDes8& aBlock);
	void Reset(void);
	TInt KeySize(void) const;
	
	/// Destructor
	~CAESEncryptorShim();
		
private:
	/// Constructor
	CAESEncryptorShim();
	void ConstructL(const TDesC8& aKey);	
	
	// From CBase
	TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);
	
private:
	/// SPI delegate
	CryptoSpi::CSymmetricCipher* iSymmetricCipherImpl;
		
	/// SPI requires all key to passed as key-objects
	CryptoSpi::CKey* iKey;		
	
	/// Temporary output block, SPI does not overwrite input
	/// AES uses 64bit blocks
	TBuf8<16> iOutputBlock;
};

NONSHARABLE_CLASS(CAESDecryptorShim) : public CAESDecryptor
{
public:
	/**
	Creates an CAESDecryptorShim object which has the same interface
	as AESDecryptor but delegates all work to a Crypto SPI plug-in.
	
	@param aKey The decryption key
	@return A pointer to a CAESDecryptorShim instance
	*/
	static CAESDecryptorShim* NewL(const TDesC8& aKey);
	
	/**
	Creates an CAESDecryptorShim object which has the same interface
	as AESDecryptor but delegates all work to a Crypto SPI plug-in.
	
	A pointer to the new object is placed on the cleanup stack
	
	@param aKey The decryption key
	@return A pointer to a CAESDecryptorShim instance
	*/
	static CAESDecryptorShim* NewLC(const TDesC8& aKey);
	
	// From CBlockTransform
	TInt BlockSize() const;
	void Transform(TDes8& aBlock);
	void Reset(void);
	TInt KeySize(void) const;
	
	/// Destructor
	~CAESDecryptorShim();
	
private:
	/// Constructor	
	CAESDecryptorShim();	
	void ConstructL(const TDesC8& aKey);	
	
	/**
	From CBase, to allow CBufferedTransform & CBlockChainingMode
	to determine whether the functionality may be delegated to
	the SPI object.
	*/ 
	TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);
	
private:	
	/// SPI delegate
	CryptoSpi::CSymmetricCipher* iSymmetricCipherImpl;
		
	/// SPI requires all key to passed as key-objects
	CryptoSpi::CKey* iKey;		
	
	/// Temporary output block, SPI does not overwrite input
	/// AES uses 64bit blocks
	TBuf8<16> iOutputBlock;
};

#endif // __RIJNDAELSHIM_H__
