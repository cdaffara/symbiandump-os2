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


#ifndef __BUFFEREDTRANSFORMATIONSHIM_H__
#define __BUFFEREDTRANSFORMATIONSHIM_H__

#include <bufferedtransformation.h>

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
 Shim class that delegates all operations to an Crypto Spi object.
 This object is constructed automatically by CBufferedEncryptorShim and should
 not be used directly.
 */
NONSHARABLE_CLASS(CBufferedEncryptorShim) : public CBufferedEncryptor
{
public:
	/**
	Creates a new CBufferedEncryptorShim.
	N.B. It is the reponsiblity of the CALLER to check that block transform
	object contains an SPI delegate and that the delegate supports the specified
	padding mode.
	*/
	static CBufferedEncryptorShim* NewL(CBlockTransformation* aBT, CPadding* aPadding);
	
	// From CBufferedTransformation
	void Process(const TDesC8& aInput, TDes8& aOutput);
	TInt MaxOutputLength(TInt aInputLength) const;
	void Reset();
	TInt BlockSize() const;
	TInt KeySize() const;	
	void ProcessFinalL(const TDesC8& aInput, TDes8& aOutput);
	TInt MaxFinalOutputLength(TInt aInputLength) const;
private:	
	CBufferedEncryptorShim(CryptoSpi::CSymmetricCipher* aSymmetricCipherImpl);
	void ConstructL(CBlockTransformation* aBT, CPadding* aPadding);
	
	// owned by block transform - iBT
	CryptoSpi::CSymmetricCipher* iSymmetricCipherImpl;
};

/**
 Shim class that delegates all operations to an Crypto Spi object.
 This object is constructed automatically by CBufferedEncryptorShim and should
 not be used directly.
 */
NONSHARABLE_CLASS(CBufferedDecryptorShim) : public CBufferedDecryptor
{
public:
	/**
	Creates a new CBufferedDecryptorShim.
	N.B. It is the reponsiblity of the CALLER to check that block transform
	object contains an SPI delegate and that the delegate supports the specified
	padding mode.
	*/
	static CBufferedDecryptorShim* NewL(CBlockTransformation* aBT, CPadding* aPadding);
	
	// From CBufferedTransformation	
	void Process(const TDesC8& aInput, TDes8& aOutput);
	TInt MaxOutputLength(TInt aInputLength) const;
	void Reset();
	TInt BlockSize() const;
	TInt KeySize() const;	
	void ProcessFinalL(const TDesC8& aInput, TDes8& aOutput);
	TInt MaxFinalOutputLength(TInt aInputLength) const;
private:	
	CBufferedDecryptorShim(CryptoSpi::CSymmetricCipher* aSymmetricCipherImpl);
	void ConstructL(CBlockTransformation* aBT, CPadding* aPadding);
	
	// owned by block transform - iBT
	CryptoSpi::CSymmetricCipher* iSymmetricCipherImpl;
};

#endif	//	__BUFFEREDTRANSFORMATIONSHIM_H__
