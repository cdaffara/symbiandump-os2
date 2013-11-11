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


#include "3desshim.h"

#include <cryptospi/cryptosymmetriccipherapi.h>
#include <cryptospi/cryptospidef.h>
#include <cryptospi/plugincharacteristics.h>
#include <cryptospi/keys.h>
#include <cryptostrength.h>
#include "../common/inlines.h"

using namespace CryptoSpi;

// C3DESEncryptorShim ////////////////////////////////////////////////////////
C3DESEncryptorShim* C3DESEncryptorShim::NewL(const TDesC8& aKey)
	{
	C3DESEncryptorShim* self = C3DESEncryptorShim::NewLC(aKey);
	CleanupStack::Pop(self);
	return self;
	}

C3DESEncryptorShim* C3DESEncryptorShim::NewLC(const TDesC8& aKey)
	{
	C3DESEncryptorShim* self = new (ELeave) C3DESEncryptorShim();
	CleanupStack::PushL(self);
	self->ConstructL(aKey);
	// DES only used 7 bits out of every key byte
	TCrypto::IsSymmetricWeakEnoughL(BytesToBits(aKey.Size()) - aKey.Size());
	return self;
	}

C3DESEncryptorShim::C3DESEncryptorShim()
	{
	}

C3DESEncryptorShim::~C3DESEncryptorShim()
	{
	delete iSymmetricCipherImpl;
	delete iKey;
	}	

void C3DESEncryptorShim::ConstructL(const TDesC8& aKey)
	{
	TKeyProperty keyProperty = {K3DesUid, KNullUid, KSymmetricKey, KNonEmbeddedKeyUid};
	CCryptoParams* keyParam =CCryptoParams::NewLC();
	keyParam->AddL(aKey, KSymmetricKeyParameterUid);
	iKey=CKey::NewL(keyProperty, *keyParam);
	CleanupStack::PopAndDestroy(keyParam);
	
	CSymmetricCipherFactory::CreateSymmetricCipherL(
											iSymmetricCipherImpl,
											K3DesUid,
											*iKey,
											KCryptoModeEncryptUid,
											KOperationModeECBUid,
											KPaddingModeNoneUid,
											NULL);
	}		

TInt C3DESEncryptorShim::BlockSize() const
	{
	// SPI returns block size in BITS
	return iSymmetricCipherImpl->BlockSize() >> 3;
	}	

TInt C3DESEncryptorShim::KeySize() const
	{
	return iSymmetricCipherImpl->KeySize();
	}	

void C3DESEncryptorShim::Transform(TDes8& aBlock)
	{
	iOutputBlock.Zero();
	TRAP_IGNORE(iSymmetricCipherImpl->ProcessL(aBlock, iOutputBlock);)
	aBlock = iOutputBlock;
	}

void C3DESEncryptorShim::Reset()
	{
	iSymmetricCipherImpl->Reset();
	}	

TInt C3DESEncryptorShim::Extension_(TUint aExtensionId, TAny*& a0, TAny* /*a1*/)
	{
	TInt ret(KErrExtensionNotSupported);
	
	if (CryptoSpi::KSymmetricCipherInterface == aExtensionId && iSymmetricCipherImpl)
		{
		a0=iSymmetricCipherImpl;
		ret=KErrNone;	
		}		
	return ret;
	}	

// C3DESDecryptorShim ////////////////////////////////////////////////////////
C3DESDecryptorShim* C3DESDecryptorShim::NewL(const TDesC8& aKey)
	{
	C3DESDecryptorShim* self = C3DESDecryptorShim::NewLC(aKey);
	CleanupStack::Pop(self);
	return self;
	}


C3DESDecryptorShim* C3DESDecryptorShim::NewLC(const TDesC8& aKey)
	{
	C3DESDecryptorShim* self = new (ELeave) C3DESDecryptorShim();
	CleanupStack::PushL(self);
	self->ConstructL(aKey);
	// DES only used 7 bits out of every key byte
	TCrypto::IsSymmetricWeakEnoughL(BytesToBits(aKey.Size()) - aKey.Size());
	return self;
	}

C3DESDecryptorShim::C3DESDecryptorShim()
	{	
	}

C3DESDecryptorShim::~C3DESDecryptorShim()
	{
	delete iSymmetricCipherImpl;
	delete iKey;
	}


void C3DESDecryptorShim::ConstructL(const TDesC8& aKey)
	{
	TKeyProperty keyProperty = {K3DesUid, KNullUid, KSymmetricKey, KNonEmbeddedKeyUid};
	CCryptoParams* keyParam =CCryptoParams::NewLC();
	keyParam->AddL(aKey, KSymmetricKeyParameterUid);
	iKey=CKey::NewL(keyProperty, *keyParam);
	CleanupStack::PopAndDestroy(keyParam);
	CSymmetricCipherFactory::CreateSymmetricCipherL(
											iSymmetricCipherImpl,
											K3DesUid,
											*iKey,
											KCryptoModeDecryptUid,
											KOperationModeECBUid,
											KPaddingModeNoneUid,
											NULL);	
	}	

TInt C3DESDecryptorShim::BlockSize() const
	{
	// SPI returns block size in BITS
	return BitsToBytes(iSymmetricCipherImpl->BlockSize());
	}
	
TInt C3DESDecryptorShim::KeySize() const
	{
	return iSymmetricCipherImpl->KeySize();
	}	

void C3DESDecryptorShim::Transform(TDes8& aBlock)
	{
	iOutputBlock.Zero();
	TRAP_IGNORE(iSymmetricCipherImpl->ProcessL(aBlock, iOutputBlock);)
	aBlock = iOutputBlock;	
	}

void C3DESDecryptorShim::Reset()
	{
	iSymmetricCipherImpl->Reset();
	}

TInt C3DESDecryptorShim::Extension_(TUint aExtensionId, TAny*& a0, TAny* /*a1*/)
	{
	TInt ret(KErrExtensionNotSupported);
	
	if (CryptoSpi::KSymmetricCipherInterface == aExtensionId && iSymmetricCipherImpl)
		{
		a0=iSymmetricCipherImpl;
		ret=KErrNone;	
		}		
	return ret;
	}
