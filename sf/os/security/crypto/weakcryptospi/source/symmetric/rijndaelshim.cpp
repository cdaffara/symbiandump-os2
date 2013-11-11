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


#include "rijndaelshim.h"

#include <cryptospi/cryptosymmetriccipherapi.h>
#include <cryptospi/cryptospidef.h>
#include <cryptospi/plugincharacteristics.h>
#include <cryptospi/keys.h>
#include <cryptostrength.h>

#include "../common/inlines.h"

using namespace CryptoSpi;

// CAESEncryptorShim ////////////////////////////////////////////////////////
CAESEncryptorShim* CAESEncryptorShim::NewL(const TDesC8& aKey)
	{
	CAESEncryptorShim* self = CAESEncryptorShim::NewLC(aKey);
	CleanupStack::Pop(self);
	return self;
	}

CAESEncryptorShim* CAESEncryptorShim::NewLC(const TDesC8& aKey)
	{
	CAESEncryptorShim* self = new (ELeave) CAESEncryptorShim();
	CleanupStack::PushL(self);
	self->ConstructL(aKey);
	TCrypto::IsSymmetricWeakEnoughL(BytesToBits(aKey.Size()));
	return self;
	}

CAESEncryptorShim::CAESEncryptorShim()
	{
	}

CAESEncryptorShim::~CAESEncryptorShim()
	{
	delete iSymmetricCipherImpl;
	delete iKey;
	}	

void CAESEncryptorShim::ConstructL(const TDesC8& aKey)
	{
	TKeyProperty keyProperty = {KAesUid, KNullUid, KSymmetricKeyUid, KNonEmbeddedKeyUid};
	CCryptoParams* keyParam =CCryptoParams::NewLC();
	keyParam->AddL(aKey, KSymmetricKeyParameterUid);
	iKey=CKey::NewL(keyProperty, *keyParam);
	CleanupStack::PopAndDestroy(keyParam);
	CSymmetricCipherFactory::CreateSymmetricCipherL(
												iSymmetricCipherImpl,
												KAesUid,
												*iKey,
												KCryptoModeEncryptUid,
												KOperationModeECBUid,
												KPaddingModeNoneUid,
												NULL);
	}		

TInt CAESEncryptorShim::BlockSize() const
	{
	return BitsToBytes(iSymmetricCipherImpl->BlockSize());
	}	

TInt CAESEncryptorShim::KeySize() const
	{
	return iSymmetricCipherImpl->KeySize();
	}	

void CAESEncryptorShim::Transform(TDes8& aBlock)
	{
	iOutputBlock.Zero();
	TRAP_IGNORE(iSymmetricCipherImpl->ProcessL(aBlock, iOutputBlock);)
	aBlock = iOutputBlock;
	}

void CAESEncryptorShim::Reset()
	{
	iSymmetricCipherImpl->Reset();
	}	

TInt CAESEncryptorShim::Extension_(TUint aExtensionId, TAny*& a0, TAny* /*a1*/)
	{
	TInt ret(KErrExtensionNotSupported);
	
	if (CryptoSpi::KSymmetricCipherInterface == aExtensionId && iSymmetricCipherImpl)
		{
		a0=iSymmetricCipherImpl;
		ret=KErrNone;	
		}		
	return ret;
	}	

// CAESDecryptorShim ////////////////////////////////////////////////////////		

CAESDecryptorShim* CAESDecryptorShim::NewL(const TDesC8& aKey)
	{
	CAESDecryptorShim* self = CAESDecryptorShim::NewLC(aKey);
	CleanupStack::Pop(self);
	return self;
	}


CAESDecryptorShim* CAESDecryptorShim::NewLC(const TDesC8& aKey)
	{
	CAESDecryptorShim* self = new (ELeave) CAESDecryptorShim();
	CleanupStack::PushL(self);
	self->ConstructL(aKey);
	TCrypto::IsSymmetricWeakEnoughL(BytesToBits(aKey.Size()));
	return self;
	}

CAESDecryptorShim::CAESDecryptorShim()
	{	
	}

CAESDecryptorShim::~CAESDecryptorShim()
	{
	delete iSymmetricCipherImpl;
	delete iKey;			
	}

void CAESDecryptorShim::ConstructL(const TDesC8& aKey)
	{
	TKeyProperty keyProperty = {KAesUid, KNullUid, KSymmetricKeyUid, KNonEmbeddedKeyUid};
	CCryptoParams* keyParam =CCryptoParams::NewLC();
	keyParam->AddL(aKey, KSymmetricKeyParameterUid);
	iKey=CKey::NewL(keyProperty, *keyParam);
	CleanupStack::PopAndDestroy(keyParam);
	CSymmetricCipherFactory::CreateSymmetricCipherL(
												iSymmetricCipherImpl,
												KAesUid,
												*iKey,
												KCryptoModeDecryptUid,
												KOperationModeECBUid,
												KPaddingModeNoneUid,
												NULL);	
	}	

TInt CAESDecryptorShim::BlockSize() const
	{
	return BitsToBytes(iSymmetricCipherImpl->BlockSize());
	}
	
TInt CAESDecryptorShim::KeySize() const
	{
	return iSymmetricCipherImpl->KeySize();
	}	

void CAESDecryptorShim::Transform(TDes8& aBlock)
	{
	iOutputBlock.Zero();	
	TRAP_IGNORE(iSymmetricCipherImpl->ProcessL(aBlock, iOutputBlock);)
	aBlock = iOutputBlock;	
	}

void CAESDecryptorShim::Reset()
	{
	iSymmetricCipherImpl->Reset();
	}

TInt CAESDecryptorShim::Extension_(TUint aExtensionId, TAny*& a0, TAny* /*a1*/)
	{
	TInt ret(KErrExtensionNotSupported);
	
	if (CryptoSpi::KSymmetricCipherInterface == aExtensionId && iSymmetricCipherImpl)
		{
		a0=iSymmetricCipherImpl;
		ret=KErrNone;	
		}		
	return ret;
	}		
