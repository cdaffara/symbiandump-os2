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
* RC2 shim classes definition
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef __RC2SHIM_H__
#define __RC2SHIM_H__

#include <rc2.h>

namespace CryptoSpi
	{
	class CCryptoParams;
	class CSymmetricCipher;
	class CKey;
	}
	
NONSHARABLE_CLASS(CRC2EncryptorShim) : public CRC2Encryptor
{
public:
	/**
	Creates an CRC2EncryptorShim object which has the same interface
	as RC2 Encryptor but delegates all work to a Crypto SPI plug-in.
	
	@param aKey The encryption key
	@return A pointer to a CRC2EncryptorShim instance
	*/
	static CRC2EncryptorShim* NewL(const TDesC8& aKey, TInt aEffectiveKeyLenBits);
	
	/**
	Creates an CRC2EncryptorShim object which has the same interface
	as RC2 Encryptor but delegates all work to a Crypto SPI plug-in.
	
	A pointer to the new object is placed on the cleanup stack
	
	@param aKey The encryption key
	@return A pointer to a CRC2EncryptorShim instance
	*/
	static CRC2EncryptorShim* NewLC(const TDesC8& aKey, TInt aEffectiveKeyLenBits);
	
	// From CBlockTransform
	TInt BlockSize() const;
	void Transform(TDes8& aBlock);
	void Reset(void);
	TInt KeySize(void) const;
	
	/// Destructor
	~CRC2EncryptorShim();
		
private:
	/// Constructor
	CRC2EncryptorShim();
	void ConstructL(const TDesC8& aKey, TInt aEffectiveKeyLenBits);	
	
	// From CBase
	TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);
	
private:
	/// SPI delegate
	CryptoSpi::CSymmetricCipher* iSymmetricCipherImpl;
		
	/// SPI requires all key to passed as key-objects
	CryptoSpi::CKey* iKey;		
	
	/// Temporary output block, SPI does not overwrite input
	/// RC2 uses 64bit blocks
	TBuf8<16> iOutputBlock;
	
	/// The effective key length is passed as an algorithm
	/// parameter. This allows it to be externalised.
	CryptoSpi::CCryptoParams* iAlgorithmParams;	
};

NONSHARABLE_CLASS(CRC2DecryptorShim) : public CRC2Decryptor
{
public:
	/**
	Creates an CRC2DecryptorShim object which has the same interface
	as RC2 Decryptor but delegates all work to a Crypto SPI plug-in.
	
	@param aKey The decryption key
	@return A pointer to a CRC2DecryptorShim instance
	*/
	static CRC2DecryptorShim* NewL(const TDesC8& aKey, TInt aEffectiveKeyLenBits);
	
	/**
	Creates an CRC2DecryptorShim object which has the same interface
	as RC2 Decryptor but delegates all work to a Crypto SPI plug-in.
	
	A pointer to the new object is placed on the cleanup stack
	
	@param aKey The decryption key
	@return A pointer to a CRC2DecryptorShim instance
	*/
	static CRC2DecryptorShim* NewLC(const TDesC8& aKey, TInt aEffectiveKeyLenBits);
	
	// From CBlockTransform
	TInt BlockSize() const;
	void Transform(TDes8& aBlock);
	void Reset(void);
	TInt KeySize(void) const;
	
	/// Destructor
	~CRC2DecryptorShim();
	
private:
	/// Constructor	
	CRC2DecryptorShim();	
	void ConstructL(const TDesC8& aKey, TInt aEffectiveKeyLenBits);	
	
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
	/// RC2 uses 64bit blocks
	TBuf8<16> iOutputBlock;
	
	/// The effective key length is passed as an algorithm
	/// parameter. This allows it to be externalised.
	CryptoSpi::CCryptoParams* iAlgorithmParams;
};

#endif // __RC2SHIM_H__
