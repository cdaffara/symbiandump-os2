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


#include "rc2shim.h"

#include <cryptospi/cryptoparams.h>
#include <cryptospi/cryptosymmetriccipherapi.h>
#include <cryptospi/cryptospidef.h>
#include <cryptospi/plugincharacteristics.h>
#include <cryptospi/keys.h>
#include <cryptostrength.h>

#include "../common/inlines.h"

using namespace CryptoSpi;

// CRC2EncryptorShim ////////////////////////////////////////////////////////
CRC2EncryptorShim* CRC2EncryptorShim::NewL(const TDesC8& aKey, TInt aEffectiveKeyLenBits)
	{
	CRC2EncryptorShim* self = CRC2EncryptorShim::NewLC(aKey, aEffectiveKeyLenBits);
	CleanupStack::Pop(self);
	return self;
	}

CRC2EncryptorShim* CRC2EncryptorShim::NewLC(const TDesC8& aKey, TInt aEffectiveKeyLenBits)
	{
	CRC2EncryptorShim* self = new (ELeave) CRC2EncryptorShim();
	CleanupStack::PushL(self);
	self->ConstructL(aKey, aEffectiveKeyLenBits);
	// weak enough if either aKey or aEffectiveKeyLenBits is weak
	TInt minKeySize = Min(aEffectiveKeyLenBits, BytesToBits(aKey.Size()));
	TCrypto::IsSymmetricWeakEnoughL(minKeySize);
	return self;
	}

CRC2EncryptorShim::CRC2EncryptorShim()
	{
	}

CRC2EncryptorShim::~CRC2EncryptorShim()
	{
	delete iSymmetricCipherImpl;
	delete iKey;	
	delete iAlgorithmParams;
	}	

void CRC2EncryptorShim::ConstructL(const TDesC8& aKey, TInt aEffectiveKeyLenBits)
	{
	TKeyProperty keyProperty = {KRc2Uid, KNullUid, KSymmetricKeyUid, KNonEmbeddedKeyUid};
	CCryptoParams* keyParam =CCryptoParams::NewLC();
	keyParam->AddL(aKey, KSymmetricKeyParameterUid);
	iKey=CKey::NewL(keyProperty, *keyParam);
	CleanupStack::PopAndDestroy(keyParam);
	
	iAlgorithmParams = CCryptoParams::NewL();
	iAlgorithmParams->AddL(aEffectiveKeyLenBits, KRC2EffectiveKeyLenBits);
	
	CSymmetricCipherFactory::CreateSymmetricCipherL(
											iSymmetricCipherImpl,
											KRc2Uid,
											*iKey,
											KCryptoModeEncryptUid,
											KOperationModeECBUid,
											KPaddingModeNoneUid,
											iAlgorithmParams);
	}		

TInt CRC2EncryptorShim::BlockSize() const
	{
	// SPI returns block size in BITS
	return BitsToBytes(iSymmetricCipherImpl->BlockSize());
	}	

TInt CRC2EncryptorShim::KeySize() const
	{
	return iSymmetricCipherImpl->KeySize();
	}	

void CRC2EncryptorShim::Transform(TDes8& aBlock)
	{
	iOutputBlock.Zero();
	TRAP_IGNORE(iSymmetricCipherImpl->ProcessL(aBlock, iOutputBlock);)
	aBlock = iOutputBlock;
	}

void CRC2EncryptorShim::Reset()
	{
	iSymmetricCipherImpl->Reset();
	}	

TInt CRC2EncryptorShim::Extension_(TUint aExtensionId, TAny*& a0, TAny* /*a1*/)
	{
	TInt ret(KErrExtensionNotSupported);
	
	if (KSymmetricCipherInterface == aExtensionId && iSymmetricCipherImpl)
		{
		a0=iSymmetricCipherImpl;
		ret=KErrNone;	
		}		
	return ret;
	}	

// CRC2DecryptorShim ////////////////////////////////////////////////////////
CRC2DecryptorShim* CRC2DecryptorShim::NewL(const TDesC8& aKey, TInt aEffectiveKeyLenBits)
	{
	CRC2DecryptorShim* self = CRC2DecryptorShim::NewLC(aKey, aEffectiveKeyLenBits);
	CleanupStack::Pop(self);
	return self;
	}


CRC2DecryptorShim* CRC2DecryptorShim::NewLC(const TDesC8& aKey, TInt aEffectiveKeyLenBits)
	{
	CRC2DecryptorShim* self = new (ELeave) CRC2DecryptorShim();
	CleanupStack::PushL(self);
	self->ConstructL(aKey, aEffectiveKeyLenBits);
	// weak enough if either aKey or aEffectiveKeyLenBits is weak
	TInt minKeySize = Min(aEffectiveKeyLenBits, BytesToBits(aKey.Size()));
	TCrypto::IsSymmetricWeakEnoughL(minKeySize);
	return self;
	}

CRC2DecryptorShim::CRC2DecryptorShim()
	{	
	}

CRC2DecryptorShim::~CRC2DecryptorShim()
	{
	delete iSymmetricCipherImpl;
	delete iKey;
	delete iAlgorithmParams;			
	}


void CRC2DecryptorShim::ConstructL(const TDesC8& aKey, TInt aEffectiveKeyLenBits)
	{
	TKeyProperty keyProperty = {KRc2Uid, KNullUid, KSymmetricKeyUid, KNonEmbeddedKeyUid};
	CCryptoParams* keyParam =CCryptoParams::NewLC();
	keyParam->AddL(aKey, KSymmetricKeyParameterUid);
	iKey=CKey::NewL(keyProperty, *keyParam);
	CleanupStack::PopAndDestroy(keyParam);

	iAlgorithmParams = CCryptoParams::NewL();
	iAlgorithmParams->AddL(aEffectiveKeyLenBits, KRC2EffectiveKeyLenBits);
	
	CSymmetricCipherFactory::CreateSymmetricCipherL(
												iSymmetricCipherImpl,
												KRc2Uid,
												*iKey,
												KCryptoModeDecryptUid,
												KOperationModeECBUid,
												KPaddingModeNoneUid,
												iAlgorithmParams);	
	}	

TInt CRC2DecryptorShim::BlockSize() const
	{
	// SPI returns block size in BITS
	return BitsToBytes(iSymmetricCipherImpl->BlockSize());
	}
	
TInt CRC2DecryptorShim::KeySize() const
	{
	return iSymmetricCipherImpl->KeySize();
	}	

void CRC2DecryptorShim::Transform(TDes8& aBlock)
	{
	iOutputBlock.Zero();	
	TRAP_IGNORE(iSymmetricCipherImpl->ProcessL(aBlock, iOutputBlock);)
	aBlock = iOutputBlock;	
	}

void CRC2DecryptorShim::Reset()
	{
	iSymmetricCipherImpl->Reset();
	}

TInt CRC2DecryptorShim::Extension_(TUint aExtensionId, TAny*& a0, TAny* /*a1*/)
	{
	TInt ret(KErrExtensionNotSupported);
	
	if (CryptoSpi::KSymmetricCipherInterface == aExtensionId && iSymmetricCipherImpl)
		{
		a0=iSymmetricCipherImpl;
		ret=KErrNone;	
		}		
	return ret;
	}
