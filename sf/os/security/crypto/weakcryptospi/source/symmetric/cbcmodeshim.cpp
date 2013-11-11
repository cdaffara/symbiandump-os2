/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "cbcmodeshim.h"

#include <cryptopanic.h>
#include <cryptospi/cryptospidef.h>
#include <padding.h>
#include <cryptospi/cryptosymmetriccipherapi.h>
#include <cryptospi/plugincharacteristics.h>
#include "../common/inlines.h"

// CModeCBCEncryptorShim
CModeCBCEncryptorShim::CModeCBCEncryptorShim(CryptoSpi::CSymmetricCipher* aSymmetricCipherImpl) :
	iSymmetricCipherImpl(aSymmetricCipherImpl)
	{
	}

CModeCBCEncryptorShim* CModeCBCEncryptorShim::NewL(CBlockTransformation* aBT, const TDesC8& aIv)
	{
	CModeCBCEncryptorShim* self(0);
	
	// Check whether the block transform contains an SPI plug-in
	TAny* implPtr(0);
	TInt err = aBT->GetExtension(CryptoSpi::KSymmetricCipherInterface, implPtr, NULL);	
	if (err == KErrNone && implPtr)
		{
		CryptoSpi::CSymmetricCipher* impl(static_cast<CryptoSpi::CSymmetricCipher*>(implPtr));
		
		const CryptoSpi::TCharacteristics* c(0);
		impl->GetCharacteristicsL(c);
	
		const CryptoSpi::TSymmetricCipherCharacteristics* cipherCharacteristics(
			static_cast<const CryptoSpi::TSymmetricCipherCharacteristics*>(c));
			
		// Verify that the plug-in supports CBC mode
		if (err == KErrNone && 
			cipherCharacteristics->IsOperationModeSupported(CryptoSpi::KOperationModeCBCUid))
			{
			// Set block transform to encrypt-cbc
			impl->SetCryptoModeL(CryptoSpi::KCryptoModeEncryptUid);
			impl->SetOperationModeL(CryptoSpi::KOperationModeCBCUid);
			impl->SetIvL(aIv);		
			self = new(ELeave) CModeCBCEncryptorShim(impl);
			CleanupStack::PushL(self);
			self->ConstructL(aBT, aIv);
			CleanupStack::Pop(self);
			}
		}				
	return self;
	}

void CModeCBCEncryptorShim::ConstructL(CBlockTransformation* aBT, const TDesC8& aIv)
	{
	CModeCBCEncryptor::ConstructL(aBT, aIv);
	}

void CModeCBCEncryptorShim::Reset()
	{
	iSymmetricCipherImpl->Reset();
	}
	
TInt CModeCBCEncryptorShim::BlockSize() const
	{
	return BitsToBytes(iSymmetricCipherImpl->BlockSize());
	}
	
TInt CModeCBCEncryptorShim::KeySize() const
	{
	return iSymmetricCipherImpl->KeySize();
	}
	
void CModeCBCEncryptorShim::Transform(TDes8& aBlock) 
	{
	// This function will never get called if a buffered
	// encryptor is used because Process and ProcessFinalL call
	// iSymmetricCipherImpl directly
	iBT->Transform(aBlock);	
	}
	
void CModeCBCEncryptorShim::SetIV(const TDesC8& aIv)
	{
	TRAPD(err, iSymmetricCipherImpl->SetIvL(aIv));
	if (err == KErrOverflow)
		{
		User::Panic(KCryptoPanic, ECryptoPanicInputTooLarge);
		}
	else if (err != KErrNone)
		{
		// SetIvL should only leave if the aIv is incorrect
		User::Panic(KCryptoPanic, KErrArgument);
		}
	}

TInt CModeCBCEncryptorShim::Extension_(TUint aExtensionId, TAny*& a0, TAny* /*a1*/)
	{
	TInt ret(KErrExtensionNotSupported);
	
	if (CryptoSpi::KSymmetricCipherInterface == aExtensionId)
		{		
		a0=iSymmetricCipherImpl;
		ret=KErrNone;	
		}		
	return ret;
	}		

// CModeCBCDecryptorShim
CModeCBCDecryptorShim::CModeCBCDecryptorShim(CryptoSpi::CSymmetricCipher* aSymmetricCipherImpl) :
	iSymmetricCipherImpl(aSymmetricCipherImpl)
	{
	}

CModeCBCDecryptorShim* CModeCBCDecryptorShim::NewL(CBlockTransformation* aBT, const TDesC8& aIv)
	{
	CModeCBCDecryptorShim* self(0);
	
	// Check whether the block transform contains an SPI plug-in
	TAny* implPtr(0);
	TInt err = aBT->GetExtension(CryptoSpi::KSymmetricCipherInterface, implPtr, NULL);	
	if (err == KErrNone && implPtr)
		{
		CryptoSpi::CSymmetricCipher* impl(static_cast<CryptoSpi::CSymmetricCipher*>(implPtr));
		
		const CryptoSpi::TCharacteristics* c(0);
		impl->GetCharacteristicsL(c);
	
		const CryptoSpi::TSymmetricCipherCharacteristics* cipherCharacteristics(
			static_cast<const CryptoSpi::TSymmetricCipherCharacteristics*>(c));
			
		// Verify that the plug-in supports CBC mode
		if (err == KErrNone && 
			cipherCharacteristics->IsOperationModeSupported(CryptoSpi::KOperationModeCBCUid))
			{
			// Set block transform to encrypt-cbc
			impl->SetCryptoModeL(CryptoSpi::KCryptoModeDecryptUid);
			impl->SetOperationModeL(CryptoSpi::KOperationModeCBCUid);
			impl->SetIvL(aIv);				
			self = new(ELeave) CModeCBCDecryptorShim(impl);
			CleanupStack::PushL(self);
			self->ConstructL(aBT, aIv);
			CleanupStack::Pop(self);
			}
		}				
	return self;
	}

void CModeCBCDecryptorShim::ConstructL(CBlockTransformation* aBT, const TDesC8& aIv)
	{
	CModeCBCDecryptor::ConstructL(aBT, aIv);
	}
	
void CModeCBCDecryptorShim::Reset()
	{
	iSymmetricCipherImpl->Reset();
	}
	
TInt CModeCBCDecryptorShim::BlockSize() const
	{
	return BitsToBytes(iSymmetricCipherImpl->BlockSize());
	}
	
TInt CModeCBCDecryptorShim::KeySize() const
	{
	return iSymmetricCipherImpl->KeySize();
	}

void CModeCBCDecryptorShim::Transform(TDes8& aBlock) 
	{
	// This function will never get called if a buffered
	// encryptor is used because Process and ProcessFinalL call
	// iSymmetricCipherImpl directly
	iBT->Transform(aBlock);	
	}

void CModeCBCDecryptorShim::SetIV(const TDesC8& aIv)
	{
	TRAPD(err, iSymmetricCipherImpl->SetIvL(aIv));
	if (err == KErrOverflow)
		{
		User::Panic(KCryptoPanic, ECryptoPanicInputTooLarge);
		}
	else if (err != KErrNone)
		{
		// SetIvL should only leave if the aIv is incorrect
		User::Panic(KCryptoPanic, KErrArgument);
		}
	}
	
TInt CModeCBCDecryptorShim::Extension_(TUint aExtensionId, TAny*& a0, TAny* /*a1*/)
	{
	TInt ret(KErrExtensionNotSupported);
	
	if (CryptoSpi::KSymmetricCipherInterface == aExtensionId)
		{		
		a0=iSymmetricCipherImpl;
		ret=KErrNone;	
		}		
	return ret;
	}	
