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
* crypto key pair generator API implementation
* crypto key pair generator API implementation
*
*/


/**
 @file
*/

#include <cryptospi/cryptokeypairgeneratorapi.h>
#include <cryptospi/keypairgeneratorplugin.h>
#include <cryptospi/keypair.h>
#include "legacyselector.h"

using namespace CryptoSpi;

//
// Implementation of Key Pair Generator
//
CKeyPairGenerator* CKeyPairGenerator::NewL(MKeyPairGenerator* aKeyPairGenerator, TInt aHandle)
	{
	CKeyPairGenerator* self=new(ELeave) CKeyPairGenerator(aKeyPairGenerator, aHandle);
	return self;
	}

CKeyPairGenerator::CKeyPairGenerator(MKeyPairGenerator* aKeyPairGenerator, TInt aHandle)
: CCryptoBase(aKeyPairGenerator, aHandle)
	{
	}

EXPORT_C void CKeyPairGenerator::GenerateKeyPairL(TInt aKeySize, const CCryptoParams& aKeyParameters, CKeyPair*& aKeyPair)
	{
	MKeyPairGenerator* ptr=static_cast<MKeyPairGenerator*>(iPlugin);
	ptr->GenerateKeyPairL(aKeySize, aKeyParameters, aKeyPair);
	}

EXPORT_C CKeyPairGenerator::~CKeyPairGenerator()
	{
	}

//
// Implementation of Key Pair Generator Factory
//
EXPORT_C void CKeyPairGeneratorFactory::CreateKeyPairGeneratorL(CKeyPairGenerator*& aKeyPairGenerator,
																TUid aAlgorithmUid,
																const CCryptoParams* aAlgorithmParams)
													
	{
	MPluginSelector* selector=reinterpret_cast<MPluginSelector *>(Dll::Tls());
	if (selector)
		{
		selector->CreateKeyPairGeneratorL(aKeyPairGenerator, aAlgorithmUid, aAlgorithmParams);
		}
	else
		{
		CLegacySelector* legacySelector=CLegacySelector::NewLC();
		legacySelector->CreateKeyPairGeneratorL(aKeyPairGenerator, aAlgorithmUid, aAlgorithmParams);
		CleanupStack::PopAndDestroy(legacySelector); //legacySelector	
		}
	}

//
// Implementation of Asynchronous Key Pair Generator
// (async methods not implemented, so no coverage)
//

#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif

CAsyncKeyPairGenerator* CAsyncKeyPairGenerator::NewL(MAsyncKeyPairGenerator* aKeyPairGenerator, TInt aHandle)
	{
	CAsyncKeyPairGenerator* self=new(ELeave) CAsyncKeyPairGenerator(aKeyPairGenerator, aHandle);
	return self;
	}

CAsyncKeyPairGenerator::CAsyncKeyPairGenerator(MAsyncKeyPairGenerator* aKeyPairGenerator, TInt aHandle)
: CCryptoBase(aKeyPairGenerator, aHandle)
	{
	}

EXPORT_C CAsyncKeyPairGenerator::~CAsyncKeyPairGenerator()
	{
	}

EXPORT_C void CAsyncKeyPairGenerator::GenerateKeyPairL(TInt aKeySize, const CCryptoParams& aKeyParameters, CKeyPair*& aKeyPair, TRequestStatus& aRequestStatus)
	{
	MAsyncKeyPairGenerator* ptr=static_cast<MAsyncKeyPairGenerator*>(iPlugin);
	ptr->GenerateKeyPairL(aKeySize, aKeyParameters, aKeyPair, aRequestStatus);
	}

EXPORT_C void CAsyncKeyPairGenerator::Cancel()
	{
	}

EXPORT_C void CKeyPairGeneratorFactory::CreateAsyncKeyPairGeneratorL(CAsyncKeyPairGenerator*& aKeyPairGenerator,
																	TUid aAlgorithmUid,
																	const CCryptoParams* aAlgorithmParams)
	{
	MPluginSelector* selector=reinterpret_cast<MPluginSelector *>(Dll::Tls());
	if (selector)
		{
		selector->CreateAsyncKeyPairGeneratorL(aKeyPairGenerator, aAlgorithmUid, aAlgorithmParams);
		}
	else
		{
		CLegacySelector* legacySelector=CLegacySelector::NewLC();
		legacySelector->CreateAsyncKeyPairGeneratorL(aKeyPairGenerator, aAlgorithmUid, aAlgorithmParams);
		CleanupStack::PopAndDestroy(legacySelector); //legacySelector	
		}	
	}


