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


#include "keyconverter.h"
#include <cryptospi/cryptoparams.h>
#include <cryptospi/cryptospidef.h>
#include <cryptospi/keys.h>
#include <cryptospi/keypair.h>

using namespace CryptoSpi;

/* CKeyConverter */

CKey* KeyConverter::CreateKeyL(const CDHPrivateKey& aPrivateKey)
	{
	/*
	 * setup key attributes
	 */
	TKeyProperty keyProperty;
	keyProperty.iAlgorithmUid = KDHAgreementUid;

	/*
	 * extract key parameters from the compatibility dh key object
	 */
	CCryptoParams* keyParameters = CCryptoParams::NewLC();
	keyParameters->AddL(aPrivateKey.N(), KDhKeyParameterNUid);
	keyParameters->AddL(aPrivateKey.G(), KDhKeyParameterGUid);
	keyParameters->AddL(aPrivateKey.x(), KDhKeyParameterxUid);

	/*
	 * create a CKey from the parameters
	 */
	CKey* newPrivateKey = CKey::NewL(keyProperty, *keyParameters);

	CleanupStack::PopAndDestroy(1, keyParameters);
	return newPrivateKey;
	}

CKey* KeyConverter::CreateKeyL(const CDHPublicKey& aPublicKey)
	{
	/*
	 * setup key attributes
	 */
	TKeyProperty keyProperty;
	keyProperty.iAlgorithmUid = KDHAgreementUid;

	/*
	 * extract key parameters from the compatibility dh key object
	 */
	CCryptoParams* keyParameters = CCryptoParams::NewLC();
	keyParameters->AddL(aPublicKey.N(), KDhKeyParameterNUid);
	keyParameters->AddL(aPublicKey.G(), KDhKeyParameterGUid);
	keyParameters->AddL(aPublicKey.X(), KDhKeyParameterXUid);

	/*
	 * create a CKey from the parameters
	 */
	CKey* newPublicKey = CKey::NewL(keyProperty, *keyParameters);

	CleanupStack::PopAndDestroy(1, keyParameters);
	return newPublicKey;
	}

CKey* KeyConverter::CreateKeyL(const CDSAPrivateKey& aPrivateKey)
	{
	TKeyProperty keyProperty = {KDsaSignerUid, KNullUid, KDsaPrivateKeyUid, KNonEmbeddedKeyUid};

	CCryptoParams* keyParameters = CCryptoParams::NewLC();
	keyParameters->AddL(aPrivateKey.P(), KDsaKeyParameterPUid);
	keyParameters->AddL(aPrivateKey.Q(), KDsaKeyParameterQUid);
	keyParameters->AddL(aPrivateKey.G(), KDsaKeyParameterGUid);
	keyParameters->AddL(aPrivateKey.X(), KDsaKeyParameterXUid);
		
	CKey* newPrivateKey = CKey::NewL(keyProperty, *keyParameters);
	CleanupStack::PopAndDestroy(keyParameters);
	return newPrivateKey;
	}
	
CKey* KeyConverter::CreateKeyL(const CDSAPublicKey& aPublicKey)
	{
	TKeyProperty keyProperty = {KDsaSignerUid, KNullUid, KDsaPublicKeyUid, KNonEmbeddedKeyUid};

	CCryptoParams* keyParameters = CCryptoParams::NewLC();
	keyParameters->AddL(aPublicKey.P(), KDsaKeyParameterPUid);
	keyParameters->AddL(aPublicKey.Q(), KDsaKeyParameterQUid);
	keyParameters->AddL(aPublicKey.G(), KDsaKeyParameterGUid);
	keyParameters->AddL(aPublicKey.Y(), KDsaKeyParameterYUid);

	CKey* newPublicKey = CKey::NewL(keyProperty, *keyParameters);
	CleanupStack::PopAndDestroy(keyParameters);
	return newPublicKey;
	}

// RSA convertors ///////////////////////////////////////////////////////////////////////////
CKey* KeyConverter::CreateKeyL(const CRSAPrivateKeyCRT& aPrivateKey)
	{
	TKeyProperty keyProperty = {KRsaCipherUid,
		NULL,
		KRsaPrivateKeyCRTUid,
		KNonEmbeddedKeyUid };

	CCryptoParams* keyParameters = CCryptoParams::NewLC();
	keyParameters->AddL(aPrivateKey.N(), KRsaKeyParameterNUid);
	keyParameters->AddL(aPrivateKey.P(), KRsaKeyParameterPUid);
	keyParameters->AddL(aPrivateKey.Q(), KRsaKeyParameterQUid);
	keyParameters->AddL(aPrivateKey.QInv(), KRsaKeyParameterQInvUid);
	keyParameters->AddL(aPrivateKey.DP(), KRsaKeyParameterDPUid);
	keyParameters->AddL(aPrivateKey.DQ(), KRsaKeyParameterDQUid);
		
	CKey* newPrivateKey = CKey::NewL(keyProperty, *keyParameters);
	CleanupStack::PopAndDestroy(keyParameters);
	return newPrivateKey;
	}
	
CKey* KeyConverter::CreateKeyL(const CRSAPrivateKeyStandard& aPrivateKey)
	{
	TKeyProperty keyProperty = {KRsaCipherUid,
		NULL,
		KRsaPrivateKeyStandardUid,
		KNonEmbeddedKeyUid };

	CCryptoParams* keyParameters = CCryptoParams::NewLC();
	keyParameters->AddL(aPrivateKey.N(), KRsaKeyParameterNUid);
	keyParameters->AddL(aPrivateKey.D(), KRsaKeyParameterDUid);
		
	CKey* newPrivateKey = CKey::NewL(keyProperty, *keyParameters);
	CleanupStack::PopAndDestroy(keyParameters);
	return newPrivateKey;
	}
	
CKey* KeyConverter::CreateKeyL(const CRSAPublicKey& aPublicKey)
	{
	TKeyProperty keyProperty = {KRsaCipherUid,
		NULL,
		KRsaPublicKeyUid,
		KNonEmbeddedKeyUid };

	CCryptoParams* keyParameters = CCryptoParams::NewLC();	
	keyParameters->AddL(aPublicKey.N(), KRsaKeyParameterNUid);
	keyParameters->AddL(aPublicKey.E(), KRsaKeyParameterEUid);

	CKey* newPublicKey = CKey::NewL(keyProperty, *keyParameters);
	CleanupStack::PopAndDestroy(keyParameters);
	return newPublicKey;
	}

CKey* KeyConverter::CreateKeyL(const CRSAPrivateKey& aPrivateKey)
	{
	// Determine which type of private key
	if (aPrivateKey.PrivateKeyType() == EStandard)
		{
		const CRSAPrivateKeyStandard* stdKey = static_cast<const CRSAPrivateKeyStandard*>(&aPrivateKey);
		return KeyConverter::CreateKeyL(*stdKey);
		}
	else if (aPrivateKey.PrivateKeyType() == EStandardCRT)
		{
		const CRSAPrivateKeyCRT* crtKey = static_cast<const CRSAPrivateKeyCRT*>(&aPrivateKey);
		return KeyConverter::CreateKeyL(*crtKey);
		}
	return NULL;  // Keep the compiler happy
	}

// Methods which are not supported or not exposed out of library can be excluded from the coverage.
#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif

CKeyPair* KeyConverter::CreateKeyPairL(const CDSAKeyPair& /*aKeyPair*/)
	{
	return NULL;	
	}

CKeyPair* KeyConverter::CreateKeyPairL(const CRSAKeyPair& aKeyPair)
	{
	CKey* newPrivateKey = KeyConverter::CreateKeyL(aKeyPair.PrivateKey());
	CleanupStack::PushL(newPrivateKey);
	CKey* newPublicKey = KeyConverter::CreateKeyL(aKeyPair.PublicKey());
	CleanupStack::PushL(newPublicKey);
	CKeyPair* newKeyPair = CKeyPair::NewL(newPublicKey, newPrivateKey);
	CleanupStack::Pop(2, newPrivateKey);
	return newKeyPair;
	}

CKeyPair* KeyConverter::CreateKeyPairL(const CDHKeyPair& aKeyPair)
	{
	CKey* newPrivateKey = KeyConverter::CreateKeyL(aKeyPair.PrivateKey());
	CleanupStack::PushL(newPrivateKey);
	CKey* newPublicKey = KeyConverter::CreateKeyL(aKeyPair.PublicKey());
	CleanupStack::PushL(newPublicKey);
	CKeyPair* newKeyPair = CKeyPair::NewL(newPublicKey, newPrivateKey);
	CleanupStack::Pop(2, newPrivateKey);
	return newKeyPair;
	}
