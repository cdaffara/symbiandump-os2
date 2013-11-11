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
* DES shim classes definition
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef __DESSHIM_H__
#define __DESSHIM_H__

#include <des.h>

namespace CryptoSpi
	{
	class CSymmetricCipher;
	class CKey;
	}
	
NONSHARABLE_CLASS(CDESEncryptorShim) : public CDESEncryptor
{
public:
	/**
	Creates an CDESEncryptorShim object which has the same interface
	as DESEncryptor but delegates all work to a Crypto SPI plug-in.
	
	@param aKey The encryption key
	@return A pointer to a CDESEncryptorShim instance
	*/
	static CDESEncryptorShim* NewL(const TDesC8& aKey);
	
	/**
	Creates an CDESEncryptorShim object which has the same interface
	as DESEncryptor but delegates all work to a Crypto SPI plug-in.
	
	A pointer to the new object is placed on the cleanup stack
	
	@param aKey The encryption key
	@return A pointer to a CDESEncryptorShim instance
	*/
	static CDESEncryptorShim* NewLC(const TDesC8& aKey);
	
	// From CBlockTransform
	TInt BlockSize() const;
	void Transform(TDes8& aBlock);
	void Reset(void);
	TInt KeySize(void) const;
	
	/// Destructor
	~CDESEncryptorShim();
		
private:
	/// Constructor
	CDESEncryptorShim();
	void ConstructL(const TDesC8& aKey);	
	
	// From CBase
	TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);
	
private:
	/// SPI delegate
	CryptoSpi::CSymmetricCipher* iSymmetricCipherImpl;
		
	/// SPI requires all key to passed as key-objects
	CryptoSpi::CKey* iKey;		
	
	/// Temporary output block, SPI does not overwrite input
	/// DES uses 64bit blocks
	TBuf8<16> iOutputBlock;
};

NONSHARABLE_CLASS(CDESDecryptorShim) : public CDESDecryptor
{
public:
	/**
	Creates an CDESDecryptorShim object which has the same interface
	as DESDecryptor but delegates all work to a Crypto SPI plug-in.
	
	@param aKey The decryption key
	@return A pointer to a CDESDecryptorShim instance
	*/
	static CDESDecryptorShim* NewL(const TDesC8& aKey);
	
	/**
	Creates an CDESDecryptorShim object which has the same interface
	as DESDecryptor but delegates all work to a Crypto SPI plug-in.
	
	A pointer to the new object is placed on the cleanup stack
	
	@param aKey The decryption key
	@return A pointer to a CDESDecryptorShim instance
	*/
	static CDESDecryptorShim* NewLC(const TDesC8& aKey);
	
	// From CBlockTransform
	TInt BlockSize() const;
	void Transform(TDes8& aBlock);
	void Reset(void);
	TInt KeySize(void) const;
	
	/// Destructor
	~CDESDecryptorShim();
	
private:
	/// Constructor	
	CDESDecryptorShim();	
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
	/// DES uses 64bit blocks
	TBuf8<16> iOutputBlock;
};

#endif // __DESSHIM_H__
