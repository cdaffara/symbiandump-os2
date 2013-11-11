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
*
*/


#ifndef __CBCMODESHIM_H__
#define __CBCMODESHIM_H__

#include <cbcmode.h>

/**
@file
@internalComponent
@released
*/

namespace CryptoSpi 
	{
	class CSymmetricCipher;
	}

/**
CBC mode encryptor shim class that delegates all work to CModeCBCEncryptor.
This should only be instantiated by CModeCBCEncryptor::NewLC
*/
NONSHARABLE_CLASS(CModeCBCEncryptorShim) : public CModeCBCEncryptor
{
public:
	/**
	Creates a new CModeCBCEncryptorShim. If the block transfer 
	@param aBT A pointer to the block transform object.
	@param aIV The initialisation vector to use.
	@return A pointer to a CModeCBCEncryptorShim instance or NULL if the block transform
	does not support SPI or the SPI plug-in does not support CBC mode
	*/
	static CModeCBCEncryptorShim* NewL(CBlockTransformation* aBT, const TDesC8& aIV);

	// From CBase
	TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);

	// From CBlockTransformation
	void Reset();
	TInt BlockSize() const;
	TInt KeySize() const;
	void Transform(TDes8& aBlock);	

	// From CBlockChainingMode
	void SetIV(const TDesC8& aIv);
	
private:
	/// Constructor
	CModeCBCEncryptorShim(CryptoSpi::CSymmetricCipher* aSymmetricCipherImpl);
	
	// Just invokes ContructL in super-class
	void ConstructL(CBlockTransformation* aBT, const TDesC8& aIv);
	
	// owned by block transform - iBT
	CryptoSpi::CSymmetricCipher* iSymmetricCipherImpl;		
};

/**
CBC mode decryptor shim class that delegates all work to CModeCBCDecryptor.
This should only be instantiated by CModeCBCDecryptor::NewLC
*/
NONSHARABLE_CLASS(CModeCBCDecryptorShim) : public CModeCBCDecryptor
{
public:
	/**
	Creates a new CModeCBCEncryptorShim.
	@param aBT A pointer to the block transform object.
	@param aIV The initialisation vector to use.
	@return A pointer to a CModeCBCDecryptorShim instance or NULL if the block transform
	does not support SPI or the SPI plug-in does not support CBC mode
	*/
	static CModeCBCDecryptorShim* NewL(CBlockTransformation* aBT, const TDesC8& aIV);

	// From CBase
	TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);

	// From CBlockTransformation
	void Reset();
	TInt BlockSize() const;
	TInt KeySize() const;
	void Transform(TDes8& aBlock);	

	// From CBlockChainingMode
	void SetIV(const TDesC8& aIv);
	
private:
	/// Constructor
	CModeCBCDecryptorShim(CryptoSpi::CSymmetricCipher* aSymmetricCipherImpl);
	
	// Just invokes ContructL in super-class
	void ConstructL(CBlockTransformation* aBT, const TDesC8& aIv);
		
	// owned by block transform - iBT
	CryptoSpi::CSymmetricCipher* iSymmetricCipherImpl;
};

#endif	//	__CBCMODESHIM_H__
