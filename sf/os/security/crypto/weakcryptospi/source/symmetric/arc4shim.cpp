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



#include "arc4shim.h"

#include <cryptospi/cryptosymmetriccipherapi.h>
#include <cryptospi/cryptospidef.h>
#include <cryptospi/plugincharacteristics.h>
#include <cryptostrength.h>
#include "../common/inlines.h"
#include <cryptospi/keys.h>

using namespace CryptoSpi;

CArc4Shim* CArc4Shim::NewL(const TDesC8& aKey, TUint aDiscardBytes)
	{
	CArc4Shim* self = NewLC(aKey, aDiscardBytes);
	CleanupStack::Pop(self);
	return self;
	}

CArc4Shim* CArc4Shim::NewLC(const TDesC8& aKey, TUint aDiscardBytes)
	{
	CArc4Shim* self = new (ELeave) CArc4Shim();
	CleanupStack::PushL(self);
	self->ConstructL(aKey, aDiscardBytes);
	TCrypto::IsSymmetricWeakEnoughL(BytesToBits(aKey.Size()));
	return self;
	}

void CArc4Shim::ConstructL(const TDesC8& aKey, TUint aDiscardBytes)
	{
	TKeyProperty keyProperty = {KArc4Uid, KNullUid, KSymmetricKey, KNonEmbeddedKeyUid};
	CCryptoParams* keyParam =CCryptoParams::NewLC();
	keyParam->AddL(aKey, KSymmetricKeyParameterUid);
	iKey=CKey::NewL(keyProperty, *keyParam);
	CleanupStack::PopAndDestroy(keyParam);
	
	iAlgorithmParams = CCryptoParams::NewL();
	iAlgorithmParams->AddL(aDiscardBytes, KARC4DiscardBytes);

	CSymmetricCipherFactory::CreateSymmetricCipherL(
												iSymmetricCipherImpl,
												KArc4Uid,
												*iKey,
												KCryptoModeNoneUid,
												KOperationModeNoneUid,
												KPaddingModeNoneUid,
												iAlgorithmParams);
	}

CArc4Shim::~CArc4Shim()
	{
	delete iSymmetricCipherImpl;
	delete iKey;	
	delete iAlgorithmParams;
	}

void CArc4Shim::Reset()
	{
	iSymmetricCipherImpl->Reset();
	}

TInt CArc4Shim::BlockSize() const
	{
	return iSymmetricCipherImpl->BlockSize() >> 3; // Convert to bits
	}

TInt CArc4Shim::KeySize() const
	{
	return iSymmetricCipherImpl->KeySize();
	}

void CArc4Shim::Process(const TDesC8& aInput, TDes8& aOutput)
	{
	TRAP_IGNORE(iSymmetricCipherImpl->ProcessL(aInput, aOutput);)
	}

void CArc4Shim::ProcessFinalL(const TDesC8& aInput, TDes8& aOutput)
	{
	iSymmetricCipherImpl->ProcessFinalL(aInput, aOutput);
	}

TInt CArc4Shim::MaxOutputLength(TInt aInputLength) const
	{
	return iSymmetricCipherImpl->MaxOutputLength(aInputLength);
	}

TInt CArc4Shim::MaxFinalOutputLength(TInt aInputLength) const
	{
	return iSymmetricCipherImpl->MaxFinalOutputLength(aInputLength);
	}

CArc4Shim::CArc4Shim()
	{		
	}

