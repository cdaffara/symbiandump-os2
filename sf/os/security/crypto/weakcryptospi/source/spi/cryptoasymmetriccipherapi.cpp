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
* crypto asymmetric cipher API implementation
* crypto asymmetric cipher API implementation
*
*/


/**
 @file
*/

#include <cryptospi/cryptoasymmetriccipherapi.h>
#include <cryptospi/asymmetriccipherplugin.h>
#include "legacyselector.h"

using namespace CryptoSpi;


//
// Implementation of Asymmetric Cipher base class
//
CAsymmetricCipherBase::CAsymmetricCipherBase(MAsymmetricCipherBase* aAsymmetricCipher, TInt aHandle)
: CCryptoBase(aAsymmetricCipher, aHandle)
	{
	}
	
CAsymmetricCipherBase::~CAsymmetricCipherBase()
	{		
	}

EXPORT_C void CAsymmetricCipherBase::SetKeyL(const CKey& aKey)
	{
	MAsymmetricCipherBase* ptr=static_cast<MAsymmetricCipherBase*>(iPlugin);
	ptr->SetKeyL(aKey);		
	}

EXPORT_C void CAsymmetricCipherBase::SetCryptoModeL(TUid aCryptoMode)
	{
	MAsymmetricCipherBase* ptr=static_cast<MAsymmetricCipherBase*>(iPlugin);
	ptr->SetCryptoModeL(aCryptoMode);		
	}

EXPORT_C void CAsymmetricCipherBase::SetPaddingModeL(TUid aPaddingMode)
	{
	MAsymmetricCipherBase* ptr=static_cast<MAsymmetricCipherBase*>(iPlugin);
	ptr->SetPaddingModeL(aPaddingMode);
	}

EXPORT_C TInt CAsymmetricCipherBase::GetMaximumInputLengthL()
	{
	MAsymmetricCipherBase* ptr=static_cast<MAsymmetricCipherBase*>(iPlugin);
	return ptr->GetMaximumInputLengthL();
	}

EXPORT_C TInt CAsymmetricCipherBase::GetMaximumOutputLengthL()
	{
	MAsymmetricCipherBase* ptr=static_cast<MAsymmetricCipherBase*>(iPlugin);
	return ptr->GetMaximumOutputLengthL();				
	}


//
// Implementation of Synchronous Asymmetric Cipher 
//
CAsymmetricCipher* CAsymmetricCipher::NewL(MAsymmetricCipher* aAsymmetricCipher, TInt aHandle)
	{
	CAsymmetricCipher* self=new(ELeave) CAsymmetricCipher(aAsymmetricCipher, aHandle);
	return self;				
	}

CAsymmetricCipher::CAsymmetricCipher(MAsymmetricCipher* aAsymmetricCipher, TInt aHandle)
: CAsymmetricCipherBase(aAsymmetricCipher, aHandle)
	{
	}

EXPORT_C CAsymmetricCipher::~CAsymmetricCipher()
	{
	}

EXPORT_C void CAsymmetricCipher::ProcessL(const TDesC8& aInput, TDes8& aOutput)
	{
	MAsymmetricCipher* ptr=static_cast<MAsymmetricCipher*>(iPlugin);
	ptr->ProcessL(aInput, aOutput);						
	}

//
// Implementation of Asymmetric Cipher Factory
//
EXPORT_C void CAsymmetricCipherFactory::CreateAsymmetricCipherL(CAsymmetricCipher*& aCipher,
																TUid aAlgorithmUid,
																const CKey& aKey,
																TUid aCryptoMode,
																TUid aPaddingMode,
																const CCryptoParams* aAlgorithmParams)
												
	{
	MPluginSelector* selector=reinterpret_cast<MPluginSelector *>(Dll::Tls());
	if (selector)
		{
		selector->CreateAsymmetricCipherL(aCipher, aAlgorithmUid, aKey, aCryptoMode, aPaddingMode, aAlgorithmParams);
		}
	else
		{
		CLegacySelector* legacySelector=CLegacySelector::NewLC();
		legacySelector->CreateAsymmetricCipherL(aCipher, aAlgorithmUid, aKey, aCryptoMode, aPaddingMode, aAlgorithmParams);
		CleanupStack::PopAndDestroy(legacySelector); //legacySelector
		}
	}


//
// Implementation of Asynchronous Asymmetric Cipher 
// (async methods not implemented, so no coverage)
//

#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif

CAsyncAsymmetricCipher* CAsyncAsymmetricCipher::NewL(MAsyncAsymmetricCipher* aAsyncAsymmetricCipher, TInt aHandle)
	{
	CAsyncAsymmetricCipher* self=new(ELeave) CAsyncAsymmetricCipher(aAsyncAsymmetricCipher, aHandle);
	return self;						
	}

CAsyncAsymmetricCipher::CAsyncAsymmetricCipher(MAsyncAsymmetricCipher* aAsyncAsymmetricCipher, TInt aHandle)
:CAsymmetricCipherBase(aAsyncAsymmetricCipher, aHandle)
	{
		
	}

EXPORT_C CAsyncAsymmetricCipher::~CAsyncAsymmetricCipher()
	{
		
	}

EXPORT_C void CAsyncAsymmetricCipher::ProcessL(const TDesC8& aInput, TDes8& aOutput, TRequestStatus& aRequestStatus)
	{
	MAsyncAsymmetricCipher* ptr=static_cast<MAsyncAsymmetricCipher*>(iPlugin);
	ptr->ProcessL(aInput, aOutput, aRequestStatus);								
	}

EXPORT_C void CAsyncAsymmetricCipher::Cancel()
	{
	MAsyncAsymmetricCipher* ptr=static_cast<MAsyncAsymmetricCipher*>(iPlugin);
	ptr->Cancel();		
	}

EXPORT_C void CAsymmetricCipherFactory::CreateAsyncAsymmetricCipherL(CAsyncAsymmetricCipher*& aCipher,
																	TUid aAlgorithmUid,
																	const CKey& aKey,
																	TUid aCryptoMode,
																	TUid aPaddingMode,
																	const CCryptoParams* aAlgorithmParams)
	{
	MPluginSelector* selector=reinterpret_cast<MPluginSelector *>(Dll::Tls());
	if (selector)
		{
		selector->CreateAsyncAsymmetricCipherL(aCipher, aAlgorithmUid, aKey, aCryptoMode, aPaddingMode, aAlgorithmParams);
		}
	else
		{
		CLegacySelector* legacySelector=CLegacySelector::NewLC();
		legacySelector->CreateAsyncAsymmetricCipherL(aCipher, aAlgorithmUid, aKey, aCryptoMode, aPaddingMode, aAlgorithmParams);
		CleanupStack::PopAndDestroy(legacySelector); //legacySelector
		}
	}

