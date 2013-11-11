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
* blocktransformationshim.cpp
*
*/


#include "bufferedtransformationshim.h"

#include <cryptospi/cryptospidef.h>
#include <padding.h>
#include <cryptospi/cryptosymmetriccipherapi.h>
#include <cryptospi/plugincharacteristics.h>
#include "../common/inlines.h"

// CBufferedEncryptorShim
CBufferedEncryptorShim::CBufferedEncryptorShim(CryptoSpi::CSymmetricCipher* aSymmetricCipherImpl) :
	iSymmetricCipherImpl(aSymmetricCipherImpl)
	{
	}

CBufferedEncryptorShim* CBufferedEncryptorShim::NewL(CBlockTransformation* aBT, CPadding* aPadding)
	{
	CBufferedEncryptorShim* self(0);
	
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
			
		// See if the padding mode is recognised by CryptoSpi and if so, check
		// whether the plug-in supports that padding mode.
		TUid paddingMode;
		TAny* paddingPtr = &paddingMode;		
		err = aPadding->GetExtension(CryptoSpi::KPaddingInterface, paddingPtr, 0);		
		if (err == KErrNone && cipherCharacteristics->IsPaddingModeSupported(paddingMode))
			{
			impl->SetCryptoModeL(CryptoSpi::KCryptoModeEncryptUid);
			impl->SetPaddingModeL(paddingMode);		
			self = new(ELeave) CBufferedEncryptorShim(impl);
			CleanupStack::PushL(self);
			self->ConstructL(aBT, aPadding);
			CleanupStack::Pop(self);
			}
		}				
	return self;
	}

void CBufferedEncryptorShim::ConstructL(CBlockTransformation* aBT, CPadding* aPadding)
	{
	CBufferedEncryptor::ConstructL(aBT, aPadding);
	}

void CBufferedEncryptorShim::Process(const TDesC8& aInput, TDes8& aOutput)
	{
	TRAP_IGNORE(iSymmetricCipherImpl->ProcessL(aInput, aOutput);)
	}
	
TInt CBufferedEncryptorShim::MaxOutputLength(TInt aInputLength) const
	{
	return iSymmetricCipherImpl->MaxOutputLength(aInputLength);
	}
	
void CBufferedEncryptorShim::Reset()
	{
	iSymmetricCipherImpl->Reset();
	}
	
TInt CBufferedEncryptorShim::BlockSize() const
	{
	return BitsToBytes(iSymmetricCipherImpl->BlockSize());
	}
	
TInt CBufferedEncryptorShim::KeySize() const
	{
	return iSymmetricCipherImpl->KeySize();
	}
	
void CBufferedEncryptorShim::ProcessFinalL(const TDesC8& aInput, TDes8& aOutput)
	{
	iSymmetricCipherImpl->ProcessFinalL(aInput, aOutput);
	}
	
TInt CBufferedEncryptorShim::MaxFinalOutputLength(TInt aInputLength) const
	{
	return iSymmetricCipherImpl->MaxFinalOutputLength(aInputLength);
	}

// CBufferedDecryptorShim
CBufferedDecryptorShim::CBufferedDecryptorShim(CryptoSpi::CSymmetricCipher* aSymmetricCipherImpl) :
	iSymmetricCipherImpl(aSymmetricCipherImpl)
	{
	}

CBufferedDecryptorShim* CBufferedDecryptorShim::NewL(CBlockTransformation* aBT, CPadding* aPadding)
	{
	CBufferedDecryptorShim* self(0);
	
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
			
		// See if the padding mode is recognised by CryptoSpi and if so, check
		// whether the plug-in supports that padding mode.
		TUid paddingMode;
		TAny* paddingPtr = &paddingMode;		
		err = aPadding->GetExtension(CryptoSpi::KPaddingInterface, paddingPtr, 0);		
		if (err == KErrNone && cipherCharacteristics->IsPaddingModeSupported(paddingMode))
			{
			impl->SetCryptoModeL(CryptoSpi::KCryptoModeDecryptUid);			
			impl->SetPaddingModeL(paddingMode);
			
			self = new(ELeave) CBufferedDecryptorShim(impl);
			CleanupStack::PushL(self);
			self->ConstructL(aBT, aPadding);
			CleanupStack::Pop(self);
			}
		}				
	return self;
	}

void CBufferedDecryptorShim::ConstructL(CBlockTransformation* aBT, CPadding* aPadding)
	{
	CBufferedDecryptor::ConstructL(aBT, aPadding);
	}

void CBufferedDecryptorShim::Process(const TDesC8& aInput, TDes8& aOutput)
	{
	TRAP_IGNORE(iSymmetricCipherImpl->ProcessL(aInput, aOutput);)
	}
	
TInt CBufferedDecryptorShim::MaxOutputLength(TInt aInputLength) const
	{
	return iSymmetricCipherImpl->MaxOutputLength(aInputLength);
	}
	
void CBufferedDecryptorShim::Reset()
	{
	iSymmetricCipherImpl->Reset();
	}
	
TInt CBufferedDecryptorShim::BlockSize() const
	{
	return BitsToBytes(iSymmetricCipherImpl->BlockSize());
	}
	
TInt CBufferedDecryptorShim::KeySize() const
	{
	return iSymmetricCipherImpl->KeySize();
	}
	
void CBufferedDecryptorShim::ProcessFinalL(const TDesC8& aInput, TDes8& aOutput)
	{
	iSymmetricCipherImpl->ProcessFinalL(aInput, aOutput);
	}
	
TInt CBufferedDecryptorShim::MaxFinalOutputLength(TInt aInputLength) const
	{
	return iSymmetricCipherImpl->MaxFinalOutputLength(aInputLength);
	}
