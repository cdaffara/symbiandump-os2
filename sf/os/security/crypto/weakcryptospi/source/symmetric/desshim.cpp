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


#include "desshim.h"

#include <cryptospi/cryptosymmetriccipherapi.h>
#include <cryptospi/cryptospidef.h>
#include <cryptospi/plugincharacteristics.h>
#include <cryptospi/keys.h>
#include <cryptostrength.h>

#include "../common/inlines.h"

using namespace CryptoSpi;

// CDESEncryptorShim ////////////////////////////////////////////////////////
CDESEncryptorShim* CDESEncryptorShim::NewL(const TDesC8& aKey)
	{
	CDESEncryptorShim* self = CDESEncryptorShim::NewLC(aKey);
	CleanupStack::Pop(self);
	return self;
	}

CDESEncryptorShim* CDESEncryptorShim::NewLC(const TDesC8& aKey)
	{
	CDESEncryptorShim* self = new (ELeave) CDESEncryptorShim();
	CleanupStack::PushL(self);
	self->ConstructL(aKey);
	// DES only used 7 bits out of every key byte
	TCrypto::IsSymmetricWeakEnoughL(BytesToBits(aKey.Size()) - aKey.Size());
	return self;
	}

CDESEncryptorShim::CDESEncryptorShim()
	{
	}

CDESEncryptorShim::~CDESEncryptorShim()
	{
	delete iSymmetricCipherImpl;
	delete iKey;			
	}	

void CDESEncryptorShim::ConstructL(const TDesC8& aKey)
	{
	TKeyProperty keyProperty = {KDesUid, KNullUid, KSymmetricKey, KNonEmbeddedKeyUid};
	CCryptoParams* keyParam =CCryptoParams::NewLC();
	keyParam->AddL(aKey, KSymmetricKeyParameterUid);
	iKey=CKey::NewL(keyProperty, *keyParam);
	CleanupStack::PopAndDestroy(keyParam);
	CSymmetricCipherFactory::CreateSymmetricCipherL(
											iSymmetricCipherImpl,
											KDesUid,
											*iKey,
											KCryptoModeEncryptUid,
											KOperationModeECBUid,
											KPaddingModeNoneUid,
											NULL);
	}		

TInt CDESEncryptorShim::BlockSize() const
	{
	// SPI returns block size in BITS
	return BitsToBytes(iSymmetricCipherImpl->BlockSize());
	}	

TInt CDESEncryptorShim::KeySize() const
	{
	return iSymmetricCipherImpl->KeySize();
	}	

void CDESEncryptorShim::Transform(TDes8& aBlock)
	{
	iOutputBlock.Zero();
	TRAP_IGNORE(iSymmetricCipherImpl->ProcessL(aBlock, iOutputBlock);)
	aBlock = iOutputBlock;
	}

void CDESEncryptorShim::Reset()
	{
	iSymmetricCipherImpl->Reset();
	}	

TInt CDESEncryptorShim::Extension_(TUint aExtensionId, TAny*& a0, TAny* /*a1*/)
	{
	TInt ret(KErrNotSupported);
	
	if (CryptoSpi::KSymmetricCipherInterface == aExtensionId && iSymmetricCipherImpl)
		{
		a0=iSymmetricCipherImpl;
		ret=KErrNone;	
		}		
	return ret;
	}	

// CDESDecryptorShim ////////////////////////////////////////////////////////
CDESDecryptorShim* CDESDecryptorShim::NewL(const TDesC8& aKey)
	{
	CDESDecryptorShim* self = CDESDecryptorShim::NewLC(aKey);
	CleanupStack::Pop(self);
	return self;
	}


CDESDecryptorShim* CDESDecryptorShim::NewLC(const TDesC8& aKey)
	{
	CDESDecryptorShim* self = new (ELeave) CDESDecryptorShim();
	CleanupStack::PushL(self);
	self->ConstructL(aKey);
	// DES only used 7 bits out of every key byte
	TCrypto::IsSymmetricWeakEnoughL(BytesToBits(aKey.Size()) - aKey.Size());
	return self;
	}

CDESDecryptorShim::CDESDecryptorShim()
	{	
	}

CDESDecryptorShim::~CDESDecryptorShim()
	{
	delete iSymmetricCipherImpl;
	delete iKey;			
	}


void CDESDecryptorShim::ConstructL(const TDesC8& aKey)
	{
	TKeyProperty keyProperty = {KDesUid, KNullUid, KSymmetricKey, KNonEmbeddedKeyUid};
	CCryptoParams* keyParam =CCryptoParams::NewLC();
	keyParam->AddL(aKey, KSymmetricKeyParameterUid);
	iKey=CKey::NewL(keyProperty, *keyParam);
	CleanupStack::PopAndDestroy(keyParam);
	CSymmetricCipherFactory::CreateSymmetricCipherL(
											iSymmetricCipherImpl,
											KDesUid,
											*iKey,
											KCryptoModeDecryptUid,
											KOperationModeECBUid,
											KPaddingModeNoneUid,
											NULL);	
	}	

TInt CDESDecryptorShim::BlockSize() const
	{
	// SPI returns block size in BITS
	return BitsToBytes(iSymmetricCipherImpl->BlockSize());
	}
	
TInt CDESDecryptorShim::KeySize() const
	{
	return iSymmetricCipherImpl->KeySize();
	}	

void CDESDecryptorShim::Transform(TDes8& aBlock)
	{
	iOutputBlock.Zero();	
	TRAP_IGNORE(iSymmetricCipherImpl->ProcessL(aBlock, iOutputBlock);)
	aBlock = iOutputBlock;	
	}

void CDESDecryptorShim::Reset()
	{
	iSymmetricCipherImpl->Reset();
	}

TInt CDESDecryptorShim::Extension_(TUint aExtensionId, TAny*& a0, TAny* /*a1*/)
	{
	TInt ret(KErrNotSupported);
	
	if (CryptoSpi::KSymmetricCipherInterface == aExtensionId && iSymmetricCipherImpl)
		{
		a0=iSymmetricCipherImpl;
		ret=KErrNone;	
		}		
	return ret;
	}
