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
* crypto key agreement API implementation
* crypto key agreement API implementation
*
*/


/**
 @file
*/

#include <cryptospi/cryptokeyagreementapi.h>
#include <cryptospi/keyagreementplugin.h>
#include "legacyselector.h"

using namespace CryptoSpi;

//
// Implementation of Key Agreement
//
CKeyAgreement* CKeyAgreement::NewL(MKeyAgreement* aKeyAgreement, TInt aHandle)
	{
	CKeyAgreement* self=new(ELeave) CKeyAgreement(aKeyAgreement, aHandle);
	return self;
	}

CKeyAgreement::CKeyAgreement(MKeyAgreement* aKeyAgreement, TInt aHandle)
: CCryptoBase(aKeyAgreement, aHandle)
	{
	}

EXPORT_C CKeyAgreement::~CKeyAgreement()
	{
	}
	
EXPORT_C void CKeyAgreement::SetKeyL(const CKey& aSelfPrivateKey, const CCryptoParams* aParams)
	{
	MKeyAgreement* ptr=static_cast<MKeyAgreement*>(iPlugin);
	ptr->SetKeyL(aSelfPrivateKey, aParams);
	}

EXPORT_C CKey* CKeyAgreement::AgreeL(const CKey& aOtherPublicKey, const CCryptoParams* aParams)
	{
	MKeyAgreement* ptr=static_cast<MKeyAgreement*>(iPlugin);
	return ptr->AgreeL(aOtherPublicKey, aParams);
	}

//
// Implementation of Key agreement factory
//
EXPORT_C void CKeyAgreementFactory::CreateKeyAgreementL(CKeyAgreement*& aKeyAgreement,
													TUid aAlgorithmUid,
													const CKey& aPrivateKey,
													const CCryptoParams* aAlgorithmParams)
	{
	MPluginSelector* selector=reinterpret_cast<MPluginSelector *>(Dll::Tls());
	if (selector)
		{
		selector->CreateKeyAgreementL(aKeyAgreement, aAlgorithmUid, aPrivateKey, aAlgorithmParams);
		}
	else
		{
		CLegacySelector* legacySelector=CLegacySelector::NewLC();
		legacySelector->CreateKeyAgreementL(aKeyAgreement, aAlgorithmUid, aPrivateKey, aAlgorithmParams);
		CleanupStack::PopAndDestroy(legacySelector); //legacySelector	
		}
	}
	
//
// Implementation of Asynchronous Key Agreement
// (async methods not implemented, so no coverage)
//

#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif

CAsyncKeyAgreement* CAsyncKeyAgreement::NewL(MAsyncKeyAgreement* /*aKeyAgreement*/, TInt /*aHandle*/)
	{
		ASSERT(EFalse);
		return 0;
	}

CAsyncKeyAgreement::CAsyncKeyAgreement(MAsyncKeyAgreement* aKeyAgreement, TInt aHandle)
: CCryptoBase(aKeyAgreement, aHandle)
	{
		
	}

EXPORT_C CAsyncKeyAgreement::~CAsyncKeyAgreement()
	{
	}

EXPORT_C void CAsyncKeyAgreement::SetKeyL(const CKey& aSelfPrivateKey, const CCryptoParams* aParams)
	{
	MAsyncKeyAgreement* ptr=static_cast<MAsyncKeyAgreement*>(iPlugin);
	ptr->SetKeyL(aSelfPrivateKey, aParams);
	}

EXPORT_C void CAsyncKeyAgreement::AgreeL(const CKey& aOtherPublicKey, CKey& aKey, const CCryptoParams* aParams, TRequestStatus& aRequestStatus)
	{
	MAsyncKeyAgreement* ptr=static_cast<MAsyncKeyAgreement*>(iPlugin);
	return ptr->AgreeL(aOtherPublicKey, aKey, aParams, aRequestStatus);
	}

EXPORT_C void CKeyAgreementFactory::CreateAsyncKeyAgreementL(CAsyncKeyAgreement*& aKeyAgreement,
														TUid aAlgorithmUid,
														const CKey& aPrivateKey,
														const CCryptoParams* aAlgorithmParams)
	{
	MPluginSelector* selector=reinterpret_cast<MPluginSelector *>(Dll::Tls());
	if (selector)
		{
		selector->CreateAsyncKeyAgreementL(aKeyAgreement, aAlgorithmUid, aPrivateKey, aAlgorithmParams);
		}
	else
		{
		CLegacySelector* legacySelector=CLegacySelector::NewLC();
		legacySelector->CreateAsyncKeyAgreementL(aKeyAgreement, aAlgorithmUid, aPrivateKey, aAlgorithmParams);
		CleanupStack::PopAndDestroy(legacySelector); //legacySelector	
		}		
	}

