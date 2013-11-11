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


#include "dsakeypairshim.h"
#include <cryptospi/cryptokeypairgeneratorapi.h>
#include <cryptospi/keypair.h>
#include <cryptospi/cryptoparams.h>
#include <cryptospi/cryptospidef.h>


using namespace CryptoSpi;


/* CDSAKeyPair */
CDSAKeyPairShim* CDSAKeyPairShim::NewLC(TUint aKeyBits)
	{
	CDSAKeyPairShim* self = new(ELeave) CDSAKeyPairShim();
	CleanupStack::PushL(self);
	self->ConstructL(aKeyBits);
	return self;
	}

CDSAKeyPairShim::~CDSAKeyPairShim()
	{
	}

CDSAKeyPairShim::CDSAKeyPairShim()
	{
	}

void CDSAKeyPairShim::ConstructL(TUint aKeyBits)
	{
	CKeyPairGenerator* keyPairGeneratorImpl=NULL;
	CKeyPairGeneratorFactory::CreateKeyPairGeneratorL(
											keyPairGeneratorImpl,
											KDSAKeyPairGeneratorUid,
											NULL);
	CleanupStack::PushL(keyPairGeneratorImpl);
	
	
	
	
	// put the DSA parameters into an array
	CCryptoParams* keyParameters = CCryptoParams::NewLC();

	// call the api to create an DSA key pair
	CKeyPair* keyPair = 0;
	keyPairGeneratorImpl->GenerateKeyPairL(aKeyBits, *keyParameters, keyPair);
	CleanupStack::PushL(keyPair);
	
	//Just to keep BC
	if (keyParameters->IsPresent(KDsaKeyGenerationSeedUid))
		{
		const TDesC8& seed = keyParameters->GetTDesC8L(KDsaKeyGenerationSeedUid);
		TInt counter=keyParameters->GetTIntL(KDsaKeyGenerationCounterUid);
		iPrimeCertificate=CDSAPrimeCertificate::NewL(seed, counter);	
		}
	
	//
	//for compatibility convert the CKeyPair to CDSAPrivateKey and CDSAPublicKey
	//
	// the public key becomes the owner of P,Q,G,Y
	const CKey& publicKey = keyPair->PublicKey();
	RInteger P = RInteger::NewL(publicKey.GetBigIntL(KDsaKeyParameterPUid));
	CleanupClosePushL(P);

	RInteger Q = RInteger::NewL(publicKey.GetBigIntL(KDsaKeyParameterQUid));
	CleanupClosePushL(Q);
	
	RInteger G = RInteger::NewL(publicKey.GetBigIntL(KDsaKeyParameterGUid));
	CleanupClosePushL(G);
	
	RInteger Y = RInteger::NewL(publicKey.GetBigIntL(KDsaKeyParameterYUid));
	CleanupClosePushL(Y);
	
	iPublic = CDSAPublicKey::NewL(P, Q, G, Y);
	CleanupStack::Pop(4, &P); //Y,G,Q,P

	// the private key becomes the owner of P1,Q1,G1,X
	const CKey& privateKey = keyPair->PrivateKey();
	RInteger P1 = RInteger::NewL(privateKey.GetBigIntL(KDsaKeyParameterPUid));
	CleanupClosePushL(P1);

	RInteger Q1 = RInteger::NewL(privateKey.GetBigIntL(KDsaKeyParameterQUid));
	CleanupClosePushL(Q1);
	
	RInteger G1 = RInteger::NewL(privateKey.GetBigIntL(KDsaKeyParameterGUid));
	CleanupClosePushL(G1);

	RInteger X = RInteger::NewL(privateKey.GetBigIntL(KDsaKeyParameterXUid));
	CleanupClosePushL(X);

	iPrivate = CDSAPrivateKey::NewL(P1, Q1, G1, X);
	CleanupStack::Pop(4, &P1); //X,G1,Q1,P1
	
	CleanupStack::PopAndDestroy(3, keyPairGeneratorImpl); // keyPair, keyParameters, keyPairGeneratorImpl
	}
