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
* crypto random API implementation
* crypto random API implementation
*
*/


/**
 @file
*/

#include <cryptospi/cryptorandomapi.h>
#include <cryptospi/randomplugin.h>
#include <cryptospi/plugincharacteristics.h>
#include "legacyselector.h"


using namespace CryptoSpi;

//
// Synchronous Random Generator
//

CRandom* CRandom::NewL(MRandom* aRandom, TInt aHandle)
	{
	CRandom* self = new(ELeave) CRandom(aRandom, aHandle);
	return self;
	}

EXPORT_C CRandom::~CRandom()
	{
	}

EXPORT_C void CRandom::GenerateRandomBytesL(TDes8& aDest)
	{
	((MRandom*)iPlugin)->GenerateRandomBytesL(aDest);
	}
	
CRandom::CRandom(MRandom* aRandom, TInt aHandle) 
: CCryptoBase(aRandom, aHandle)
	{
	}

//
// Random Factory Implementation
//
EXPORT_C void CRandomFactory::CreateRandomL(CRandom*& aRandom,
										TUid aAlgorithmUid,
										const CCryptoParams* aAlgorithmParams)
	{
	MPluginSelector* selector=reinterpret_cast<MPluginSelector *>(Dll::Tls());
	if (selector)
		{
		selector->CreateRandomL(aRandom, aAlgorithmUid, aAlgorithmParams);	
		}
	else
		{
		CLegacySelector* legacySelector=CLegacySelector::NewLC();
		legacySelector->CreateRandomL(aRandom, aAlgorithmUid, aAlgorithmParams);
		CleanupStack::PopAndDestroy(legacySelector); //legacySelector			
		}
	}

//
// Asynchronous Random Generator
// (async methods not implemented, so no coverage)
//

#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif

CAsyncRandom* CAsyncRandom::NewL(MAsyncRandom* aAsyncRandom, TInt aHandle)
	{
	CAsyncRandom* self = new(ELeave) CAsyncRandom(aAsyncRandom, aHandle);
	return self;
	}

EXPORT_C CAsyncRandom::~CAsyncRandom()
	{
	}
	
EXPORT_C void CAsyncRandom::GenerateRandomBytesL(TDes8& aDest, TRequestStatus& /*aStatus*/)
	{
	((MRandom*)iPlugin)->GenerateRandomBytesL(aDest);
	}

CAsyncRandom::CAsyncRandom(MAsyncRandom* aRandom, TInt aHandle)
: CCryptoBase(aRandom, aHandle)
	{
	}

EXPORT_C void CRandomFactory::CreateAsyncRandomL(CAsyncRandom*& aRandom,
										TUid aAlgorithmUid,
										const CCryptoParams* aAlgorithmParams)
	{
	MPluginSelector* selector=reinterpret_cast<MPluginSelector *>(Dll::Tls());
	if (selector)
		{
		selector->CreateAsyncRandomL(aRandom, aAlgorithmUid, aAlgorithmParams);	
		}
	else
		{
		CLegacySelector* legacySelector=CLegacySelector::NewLC();
		legacySelector->CreateAsyncRandomL(aRandom, aAlgorithmUid, aAlgorithmParams);
		CleanupStack::PopAndDestroy(legacySelector); //legacySelector			
		}	
	}

