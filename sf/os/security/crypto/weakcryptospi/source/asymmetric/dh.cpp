/*
* Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <asymmetric.h>
#include <asymmetrickeys.h>
#include <cryptospi/keys.h>
#include "keyconverter.h"
#include <cryptospi/cryptokeyagreementapi.h>
#include <cryptospi/cryptospidef.h>

using namespace CryptoSpi;

CKeyAgreement* GetKeyAgreementCryptoInterfaceLC(TUid aKeyAgreementAlgoUid,
	  CKey& aPrivateKey, CCryptoParams* aParams)
	{
	CKeyAgreement* keyAgreementImpl =  0;
	CKeyAgreementFactory::CreateKeyAgreementL(keyAgreementImpl,
												aKeyAgreementAlgoUid, aPrivateKey,
												aParams);
	CleanupStack::PushL(keyAgreementImpl);
	return keyAgreementImpl;
	}

EXPORT_C CDH* CDH::NewL(const CDHPrivateKey& aPrivateKey)
	{
	CDH* self = CDH::NewLC(aPrivateKey);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CDH* CDH::NewLC(const CDHPrivateKey& aPrivateKey)
	{
	CDH* self = new (ELeave) CDH(aPrivateKey);
	CleanupStack::PushL(self);
	return self;
	}

EXPORT_C HBufC8* CDH::AgreeL(const CDHPublicKey& aPublicKey) const
	{
	/*
	 * both DH keys (ie our private and their public keys) must use the same N and G parameters
	 */
	if ((aPublicKey.N() != iPrivateKey.N()) || (aPublicKey.G() != iPrivateKey.G()))
		{
		User::Leave(KErrArgument);
		}

	CKey* privateKey = KeyConverter::CreateKeyL(iPrivateKey);
	CleanupStack::PushL(privateKey);

	/*
	 *  package the common parameters N and G into a crypto params array
	 * we've already checked that both the private and public keys have the 
	 * same N and G so we only need build this array once for both creating 
	 * and calling the interface
	 */
	CCryptoParams* keyParameters = CCryptoParams::NewLC();
	keyParameters->AddL(aPublicKey.N(), KDhKeyParameterNUid);
	keyParameters->AddL(aPublicKey.G(), KDhKeyParameterGUid);

	/* 
	 * get a DH key agreement interface
	 */
	CKeyAgreement* keyAgreementImpl =  GetKeyAgreementCryptoInterfaceLC(KDHAgreementUid, *privateKey, keyParameters);

	/* 
	 * call the api to get a DH agreed key
	 */
	CKey* publicKey = KeyConverter::CreateKeyL(aPublicKey);
	CleanupStack::PushL(publicKey);

	CKey* agreedKey = keyAgreementImpl->AgreeL(*publicKey, keyParameters);
	CleanupStack::PushL(agreedKey);

	/*
	 * extract the agreed key
	 */
	const TInteger& agreedKeyData = agreedKey->GetBigIntL(KSymmetricKeyParameterUid);
	HBufC8 *agreedKeyToReturn = agreedKeyData.BufferLC();
	CleanupStack::Pop(agreedKeyToReturn);

	/* 
	 * cleanup stack - it should contain privateKey, keyAgreementImpl, publicKey, keyParameters and agreedKey
	 */
	CleanupStack::PopAndDestroy(5, privateKey);

	return agreedKeyToReturn;
	}

EXPORT_C CDH::CDH(const CDHPrivateKey& aPrivateKey) : iPrivateKey(aPrivateKey)
	{
	}
