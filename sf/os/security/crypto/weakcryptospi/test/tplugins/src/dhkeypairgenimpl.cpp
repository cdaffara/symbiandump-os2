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


#include "dhkeypairgenimpl.h"
#include "pluginconfig.h"
#include <cryptospi/keypair.h>

using namespace SoftwareCrypto;
using namespace CryptoSpi;

/* CDHKeyPairGenImpl */
CDHKeyPairGenImpl::CDHKeyPairGenImpl(TUid aImplementationUid) : CKeyPairGenImpl(aImplementationUid)
	{
	
	}

CDHKeyPairGenImpl::~CDHKeyPairGenImpl()
	{
	
	}

CDHKeyPairGenImpl* CDHKeyPairGenImpl::NewL(TUid aImplementationUid)
	{
	CDHKeyPairGenImpl* self = CDHKeyPairGenImpl::NewLC(aImplementationUid);
	CleanupStack::Pop(self);
	return self;
	}

CDHKeyPairGenImpl* CDHKeyPairGenImpl::NewLC(TUid aImplementationUid)
	{
	CDHKeyPairGenImpl* self = new(ELeave) CDHKeyPairGenImpl(aImplementationUid);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CDHKeyPairGenImpl::ConstructL(void)
	{
	CKeyPairGenImpl::ConstructL();
	}

CExtendedCharacteristics* CDHKeyPairGenImpl::CreateExtendedCharacteristicsL()
	{
	// All Symbian software plug-ins have unlimited concurrency, cannot be reserved
	// for exclusive use and are not CERTIFIED to be standards compliant.
	return CExtendedCharacteristics::NewL(KMaxTInt, EFalse);
	}

const CExtendedCharacteristics* CDHKeyPairGenImpl::GetExtendedCharacteristicsL()
	{
	return CDHKeyPairGenImpl::CreateExtendedCharacteristicsL();
	}

TUid CDHKeyPairGenImpl::ImplementationUid() const
	{
	return iImplementationUid;
	}

void CDHKeyPairGenImpl::Reset()
	{
	// does nothing in this plugin
	}

void CDHKeyPairGenImpl::GenerateKeyPairL(TInt /*aKeySize*/, const CCryptoParams& aKeyParameters, CKeyPair*& aKeyPair)
	{
	/*
	 * unpack the parameters, we're expecting the N and G parameters and if present the x parameter (aka private key)
	 */
	const TInteger& N = aKeyParameters.GetBigIntL(KDhKeyParameterNUid);
	const TInteger& G = aKeyParameters.GetBigIntL(KDhKeyParameterGUid);

	/*
	 * do some sanity checking
	 */
	RInteger nminus2 = RInteger::NewL(N);
	CleanupStack::PushL(nminus2);
	--nminus2;
	--nminus2;

	if ((G < TInteger::Two()) || (G > nminus2))
		{
		User::Leave(KErrArgument);
		}

	/*
	 * has a private key x been supplied? if not then generate it
	 */
	RInteger x;
	if (aKeyParameters.IsPresent(KDhKeyParameterxUid))
		{
		x = RInteger::NewL(aKeyParameters.GetBigIntL(KDhKeyParameterxUid));
		}
		else
		{
		// find a random x | 1 <= x <= n-2
		x = RInteger::NewRandomL(TInteger::One(), nminus2);
		}
	CleanupClosePushL(x);
	/*
	 * generate the public key with X = G^(x) mod N
	 */
	RInteger X = TInteger::ModularExponentiateL(G, x, N);
	CleanupClosePushL(X);

	/*
	 * create the keys parameters
	  */
	CCryptoParams* publicKeyParameters = CCryptoParams::NewLC();
	publicKeyParameters->AddL(X, KDhKeyParameterXUid);
	TKeyProperty publicKeyProperties = {KDHKeyPairGeneratorUid, iImplementationUid, 
									KDHPublicKeyUid, KNonEmbeddedKeyUid };
	CCryptoParams* privateKeyParameters = CCryptoParams::NewLC();
	privateKeyParameters->AddL(x, KDhKeyParameterxUid);
	TKeyProperty privateKeyProperties = {KDHKeyPairGeneratorUid, iImplementationUid,
									KDHPrivateKeyUid, KNonEmbeddedKeyUid };

	/*
	 * create the public key
	 */
	CKey* publicKey = CKey::NewL(publicKeyProperties, *publicKeyParameters);
	CleanupStack::PushL(publicKey);

	/*
	 * create the private key
	 */
	CKey* privateKey = CKey::NewL(privateKeyProperties, *privateKeyParameters);
	CleanupStack::PushL(privateKey);

	/*
	 * create the key pair
	 */
	aKeyPair = CKeyPair::NewL(publicKey, privateKey);
	
	/* 
	 * cleanup stack - it should contain nminus2, x (if allocated here), X, publicKeyParameters, privateKeyParameters, publicKey and privateKey
	 */
	CleanupStack::Pop(2, publicKey);
	CleanupStack::PopAndDestroy(5, &nminus2);
	}
