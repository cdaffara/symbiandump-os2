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


#include "dhkeypairshim.h"
#include <bigint.h>
#include <cryptospi/cryptokeypairgeneratorapi.h>
#include <cryptospi/keypair.h>
#include <cryptospi/cryptoparams.h>
#include <cryptospi/cryptospidef.h>


using namespace CryptoSpi;


/* CDHKeyPair */
CDHKeyPairShim* CDHKeyPairShim::NewLC(RInteger& aN, RInteger& aG)
	{
	CDHKeyPairShim* self = new(ELeave) CDHKeyPairShim();
	CleanupStack::PushL(self);
	self->ConstructL(aN, aG);
	return self;
	}

CDHKeyPairShim* CDHKeyPairShim::NewLC(RInteger& aN, RInteger& aG, RInteger& ax)
	{
	CDHKeyPairShim* self = new(ELeave) CDHKeyPairShim();
	CleanupStack::PushL(self);
	self->ConstructL(aN, aG, ax);
	return self;
	}

CDHKeyPairShim::~CDHKeyPairShim(void)
	{
	}

CDHKeyPairShim::CDHKeyPairShim(void)
	{
	}	

void CDHKeyPairShim::ConstructL(RInteger& aN, RInteger& aG)
	{
	RInteger x = RInteger::NewL();
	CleanupClosePushL(x);
	KeyConstructorL(aN, aG, x, EFalse);
	CleanupStack::PopAndDestroy(1, &x);
	}

void CDHKeyPairShim::ConstructL(RInteger& aN, RInteger& aG, RInteger& ax)
	{
	KeyConstructorL(aN, aG, ax, ETrue);
	}

void CDHKeyPairShim::KeyConstructorL(RInteger& aN, RInteger& aG, RInteger& ax, TBool xIncluded)
	{
	RInteger& nminus2 = aN;
	
	/*
	 * do some sanity checks
	 */
	--nminus2;
	--nminus2;
	if( aG < TInteger::Two() || aG > nminus2 )
		{
		User::Leave(KErrArgument);
		}

	if (xIncluded)
		{
		if( ax < TInteger::One() || ax > nminus2 )
			{
			User::Leave(KErrArgument);
			}
		}

	/*
	 *find out how big the key should be - the key must be in the range x | 1 <= x <= n-2
	 * nminus2 is the largest the key can be so get the number of bits required to represent that number
	 */
	const TInt keySize = nminus2.BitCount();

	// increment aN back to its original value
	++nminus2;
	++nminus2;

	// obtain an RSA key pair generator interface
	
	CKeyPairGenerator* keyPairGeneratorImpl=NULL;
	CKeyPairGeneratorFactory::CreateKeyPairGeneratorL(
											keyPairGeneratorImpl,
											KDHKeyPairGeneratorUid,
											NULL);
	CleanupStack::PushL(keyPairGeneratorImpl);
	
	/* 
	 * put the DH parameters into an array
	 */
	CCryptoParams* keyParameters = CCryptoParams::NewLC();
	keyParameters->AddL(aN, KDhKeyParameterNUid);
	keyParameters->AddL(aG, KDhKeyParameterGUid);
	if (xIncluded)
		{
		// the private key x has been supplied so add it to the params array so the key generator algo can use it
		keyParameters->AddL(ax, KDhKeyParameterxUid);
		ax.Close();
		}

	/* 
	 * call the api to create a DH key pair
	 */
	CKeyPair* keyPair = 0;
	keyPairGeneratorImpl->GenerateKeyPairL(keySize, *keyParameters, keyPair);
	CleanupStack::PushL(keyPair);

	/* 
	 * for compatibility convert the CKeyPair to CDHPrivateKey and CDHPublicKey
	 */

	// create new RInteger copies of aN, aG and x so the private key can own them
	RInteger NCopy = RInteger::NewL(aN);
	CleanupClosePushL(NCopy);
	RInteger GCopy = RInteger::NewL(aG);
	CleanupClosePushL(GCopy);
	RInteger x = RInteger::NewL(keyPair->PrivateKey().GetBigIntL(KDhKeyParameterxUid));
	CleanupClosePushL(x);
	iPrivate = CDHPrivateKey::NewL(NCopy, GCopy, x);
	CleanupStack::Pop(3, &NCopy);
	
	// the public key becomes the owner of aN, aG and X
	RInteger X = RInteger::NewL(keyPair->PublicKey().GetBigIntL(KDhKeyParameterXUid));
	CleanupClosePushL(X);
	iPublic = CDHPublicKey::NewL(aN, aG, X);
	CleanupStack::Pop(&X);	

	/* 
	 * cleanup stack - it should contain keyPairGeneratorImpl, keyParameters, keyPair, X, NCopy, GCopy and x
	 */	
	CleanupStack::PopAndDestroy(3, keyPairGeneratorImpl);
	}
