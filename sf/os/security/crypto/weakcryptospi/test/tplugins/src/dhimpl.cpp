/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "dhimpl.h"
#include "pluginconfig.h"

using namespace SoftwareCrypto;
using namespace CryptoSpi;

/* CDHImpl */
CDHImpl::CDHImpl()
	{
	}

CDHImpl::~CDHImpl()
	{
	}

CDHImpl* CDHImpl::NewL(const CKey& aPrivateKey, const CCryptoParams* aParams)
	{
	CDHImpl* self = CDHImpl::NewLC(aPrivateKey, aParams);
	CleanupStack::Pop(self);
	return self;
	}

CDHImpl* CDHImpl::NewLC(const CKey& aPrivateKey, const CCryptoParams* aParams)
	{
	CDHImpl* self = new(ELeave) CDHImpl();
	CleanupStack::PushL(self);
	self->ConstructL(aPrivateKey, aParams);
	return self;
	}

CKey* CDHImpl::AgreeL(const CKey& aOtherPublicKey, const CCryptoParams* aParams)
	{
	/*
	 * unpack the parameters, we're expecting the N and G parameters
	 */
	const TInteger& N = aParams->GetBigIntL(KDhKeyParameterNUid);
	const TInteger& G = aParams->GetBigIntL(KDhKeyParameterGUid);
	const TInteger& privateN = iSharedParams->GetBigIntL(KDhKeyParameterNUid);
	const TInteger& privateG = iSharedParams->GetBigIntL(KDhKeyParameterGUid);
	const TInteger& X = aOtherPublicKey.GetBigIntL(KDhKeyParameterXUid);
	const TInteger& x = iPrivateKey->GetBigIntL(KDhKeyParameterxUid);

	/*
	 * both DH keys (ie our private and their public keys) must use the same N and G parameters
	 */
	if ((N != privateN) || (G != privateG))
		{
		User::Leave(KErrArgument);
		}

	/*
	 * do the key agreement algo X ^ x mod N
	 */
	RInteger result = TInteger::ModularExponentiateL(X, x, N);
	CleanupClosePushL(result);

	/*
	 * create the agreed key
	 */
	CCryptoParams* agreedKeyParameters = CCryptoParams::NewLC();
	agreedKeyParameters->AddL(result, KSymmetricKeyParameterUid);
	TKeyProperty agreedKeyProperties = {KDHAgreementUid, KTestPlugin02DHKeyAgree_1Uid,
									KDHAgreedKeyUid, KNonEmbeddedKeyUid };
	CKey* agreedKey = CKey::NewL(agreedKeyProperties, *agreedKeyParameters);

	// cleanup result, agreedKeyParameters
	CleanupStack::PopAndDestroy(2, &result);
	return agreedKey;
	}

TUid CDHImpl::ImplementationUid() const
	{
	return KTestPlugin02DHKeyAgree_1Uid;
	}

CExtendedCharacteristics* CDHImpl::CreateExtendedCharacteristicsL()
	{
	// All Symbian software plug-ins have unlimited concurrency, cannot be
	// reserved for exclusive use and are not CERTIFIED to be standards compliant.
	return CExtendedCharacteristics::NewL(KMaxTInt, EFalse);
	}

const CExtendedCharacteristics* CDHImpl::GetExtendedCharacteristicsL()
	{
	return CDHImpl::CreateExtendedCharacteristicsL();
	}

