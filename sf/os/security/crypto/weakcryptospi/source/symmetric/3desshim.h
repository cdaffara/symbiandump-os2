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
* 3DES shim classes definition
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef __3DESSHIM_H__
#define __3DESSHIM_H__

#include <3des.h>

namespace CryptoSpi
	{
	class CSymmetricCipher;
	class CKey;
	}
	
NONSHARABLE_CLASS(C3DESEncryptorShim) : public C3DESEncryptor
{
public:
	/**
	Creates an C3DESEncryptorShim object which has the same interface
	as 3DESEncryptor but delegates all work to a Crypto SPI plug-in.
	
	@param aKey The encryption key
	@return A pointer to a C3DESEncryptorShim instance
	*/
	static C3DESEncryptorShim* NewL(const TDesC8& aKey);
	
	/**
	Creates an C3DESEncryptorShim object which has the same interface
	as 3DESEncryptor but delegates all work to a Crypto SPI plug-in.
	
	A pointer to the new object is placed on the cleanup stack
	
	@param aKey The encryption key
	@return A pointer to a C3DESEncryptorShim instance
	*/
	static C3DESEncryptorShim* NewLC(const TDesC8& aKey);
	
	// From CBlockTransform
	TInt BlockSize() const;
	void Transform(TDes8& aBlock);
	void Reset(void);
	TInt KeySize(void) const;
	
	/// Destructor
	~C3DESEncryptorShim();
		
private:
	/// Constructor
	C3DESEncryptorShim();
	void ConstructL(const TDesC8& aKey);	
	
	// From CBase
	TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);
	
private:
	/// SPI delegate
	CryptoSpi::CSymmetricCipher* iSymmetricCipherImpl;
		
	/// SPI requires all key to passed as key-objects
	CryptoSpi::CKey* iKey;		
	
	/// Temporary output block, SPI does not overwrite input
	/// 3DES uses 64bit blocks
	TBuf8<16> iOutputBlock;
};

NONSHARABLE_CLASS(C3DESDecryptorShim) : public C3DESDecryptor
{
public:
	/**
	Creates an C3DESDecryptorShim object which has the same interface
	as 3DESDecryptor but delegates all work to a Crypto SPI plug-in.
	
	@param aKey The decryption key
	@return A pointer to a C3DESDecryptorShim instance
	*/
	static C3DESDecryptorShim* NewL(const TDesC8& aKey);
	
	/**
	Creates an C3DESDecryptorShim object which has the same interface
	as 3DESDecryptor but delegates all work to a Crypto SPI plug-in.
	
	A pointer to the new object is placed on the cleanup stack
	
	@param aKey The decryption key
	@return A pointer to a C3DESDecryptorShim instance
	*/
	static C3DESDecryptorShim* NewLC(const TDesC8& aKey);
	
	// From CBlockTransform
	TInt BlockSize() const;
	void Transform(TDes8& aBlock);
	void Reset(void);
	TInt KeySize(void) const;
	
	/// Destructor
	~C3DESDecryptorShim();
	
private:
	/// Constructor	
	C3DESDecryptorShim();	
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
	/// 3DES uses 64bit blocks
	TBuf8<16> iOutputBlock;
};

#endif // __3DESSHIM_H__
