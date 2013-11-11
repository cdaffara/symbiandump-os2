/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "rsakeypairshim.h"
#include <cryptospi/cryptokeypairgeneratorapi.h>
#include <cryptospi/keypair.h>
#include <cryptospi/cryptoparams.h>
#include <cryptospi/cryptospidef.h>

using namespace CryptoSpi;

/* CRSAKeyPair */

const TUint KFermat4 = 65537;

CRSAKeyPairShim* CRSAKeyPairShim::NewLC(TUint aModulusBits, TRSAPrivateKeyType aKeyType)
	{
	CRSAKeyPairShim* self = new(ELeave) CRSAKeyPairShim();
	CleanupStack::PushL(self);
	self->ConstructL(aModulusBits, aKeyType, KFermat4);
	return self;
	}

CRSAKeyPairShim::~CRSAKeyPairShim(void)
	{
	}

CRSAKeyPairShim::CRSAKeyPairShim(void)
	{
	}

void CRSAKeyPairShim::ConstructL(TUint aModulusBits, TRSAPrivateKeyType aKeyType, TInt aPublicExponent)
	{
	CKeyPairGenerator* keyPairGeneratorImpl=NULL;
	CKeyPairGeneratorFactory::CreateKeyPairGeneratorL(
											keyPairGeneratorImpl,
											KRSAKeyPairGeneratorUid,
											NULL);
	CleanupStack::PushL(keyPairGeneratorImpl);

	// put the RSA parameters into an array
	CCryptoParams* keyParameters = CCryptoParams::NewLC();
	keyParameters->AddL(aPublicExponent, KRsaKeyParameterEUid);
	if (aKeyType == EStandard)
		{
		keyParameters->AddL(KRsaPrivateKeyStandard, KRsaKeyTypeUid);
		}
	else if (aKeyType == EStandardCRT)
		{
		keyParameters->AddL(KRsaPrivateKeyCRT, KRsaKeyTypeUid);
		}

	// call the api to create an RSA key pair
	CKeyPair* keyPair = 0;
	keyPairGeneratorImpl->GenerateKeyPairL(aModulusBits, *keyParameters, keyPair);
	
	CleanupStack::PushL(keyPair);

	/* 
	 * Convert the CKeyPair to CRSAPrivateKey{CRT/Standard} and CRSAPublicKey
	 * The public key becomes the owner of n and e RIntegers
	 */
	RInteger n = RInteger::NewL(keyPair->PublicKey().GetBigIntL(KRsaKeyParameterNUid));
	CleanupClosePushL(n);
	RInteger e = RInteger::NewL(keyPair->PublicKey().GetBigIntL(KRsaKeyParameterEUid));
	CleanupClosePushL(e);
	iPublic = CRSAPublicKey::NewL(n, e);

	if (aKeyType == EStandard)
		{
		RInteger n = RInteger::NewL(keyPair->PrivateKey().GetBigIntL(KRsaKeyParameterNUid));
		CleanupClosePushL(n);
		RInteger d = RInteger::NewL(keyPair->PrivateKey().GetBigIntL(KRsaKeyParameterDUid));
		CleanupClosePushL(d);
		iPrivate = CRSAPrivateKeyStandard::NewL(n, d);
		CleanupStack::Pop(2, &n);
		}
	else if (aKeyType == EStandardCRT)
		{
		RInteger n = RInteger::NewL(keyPair->PrivateKey().GetBigIntL(KRsaKeyParameterNUid));
		CleanupClosePushL(n);
		RInteger p = RInteger::NewL(keyPair->PrivateKey().GetBigIntL(KRsaKeyParameterPUid));
		CleanupClosePushL(p);
		RInteger q = RInteger::NewL(keyPair->PrivateKey().GetBigIntL(KRsaKeyParameterQUid));
		CleanupClosePushL(q);
		RInteger dp = RInteger::NewL(keyPair->PrivateKey().GetBigIntL(KRsaKeyParameterDPUid));
		CleanupClosePushL(p);
		RInteger dq = RInteger::NewL(keyPair->PrivateKey().GetBigIntL(KRsaKeyParameterDQUid));
		CleanupClosePushL(q);
		RInteger qinv = RInteger::NewL(keyPair->PrivateKey().GetBigIntL(KRsaKeyParameterQInvUid));
		CleanupClosePushL(qinv);
		iPrivate = CRSAPrivateKeyCRT::NewL(n, p, q, dp, dq, qinv);
		CleanupStack::Pop(6, &n);
		}
	/*
	 * cleanup stack - it should contain keyPairGeneratorImpl, keyParameters, keyPair, n, e
	 */
	CleanupStack::Pop(2, &n);
	CleanupStack::PopAndDestroy(3, keyPairGeneratorImpl);
	}
