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
* crypto symmetric cipher API implementation
* crypto symmetric cipher API implementation
*
*/


/**
 @file
*/

#include <cryptospi/cryptosymmetriccipherapi.h>
#include "legacyselector.h"
#include <cryptospi/symmetriccipherplugin.h>

using namespace CryptoSpi; 

//
// Implementation of Symmetric Cipher Base Class
//
CSymmetricCipherBase::CSymmetricCipherBase(MSymmetricCipherBase* aSymmetricCipher, TInt aHandle)
: CCryptoBase(aSymmetricCipher, aHandle)
	{
	}

CSymmetricCipherBase::~CSymmetricCipherBase()
	{
	}

EXPORT_C void CSymmetricCipherBase::SetKeyL(const CKey& aKey)
	{
	MSymmetricCipherBase* ptr=static_cast<MSymmetricCipherBase*>(iPlugin);
	ptr->SetKeyL(aKey);
	}

EXPORT_C void CSymmetricCipherBase::SetOperationModeL(TUid aOperationMode)
	{
	MSymmetricCipherBase* ptr=static_cast<MSymmetricCipherBase*>(iPlugin);
	ptr->SetOperationModeL(aOperationMode);
	}

EXPORT_C void CSymmetricCipherBase::SetCryptoModeL(TUid aCryptoMode)
	{
	MSymmetricCipherBase* ptr=static_cast<MSymmetricCipherBase*>(iPlugin);
	ptr->SetCryptoModeL(aCryptoMode);
	}

EXPORT_C void CSymmetricCipherBase::SetPaddingModeL(TUid aPaddingMode)
	{
	MSymmetricCipherBase* ptr=static_cast<MSymmetricCipherBase*>(iPlugin);
	ptr->SetPaddingModeL(aPaddingMode);
	}

EXPORT_C void CSymmetricCipherBase::SetIvL(const TDesC8& aIv)
	{
	MSymmetricCipherBase* ptr=static_cast<MSymmetricCipherBase*>(iPlugin);
	ptr->SetIvL(aIv);
	}

EXPORT_C TInt CSymmetricCipherBase::BlockSize()
	{
	MSymmetricCipherBase* ptr=static_cast<MSymmetricCipherBase*>(iPlugin);
	return ptr->BlockSize();
	}

EXPORT_C TInt CSymmetricCipherBase::KeySize()
	{
	MSymmetricCipherBase* ptr=static_cast<MSymmetricCipherBase*>(iPlugin);
	return ptr->KeySize();
	}	

EXPORT_C TInt CSymmetricCipherBase::MaxOutputLength(TInt aInputLength)
	{
	MSymmetricCipherBase* ptr=static_cast<MSymmetricCipherBase*>(iPlugin);
	return ptr->MaxOutputLength(aInputLength);
	}
	
EXPORT_C TInt CSymmetricCipherBase::MaxFinalOutputLength(TInt aInputLength)
	{
	MSymmetricCipherBase* ptr=static_cast<MSymmetricCipherBase*>(iPlugin);
	return ptr->MaxFinalOutputLength(aInputLength);
	}	

//
// Implementation of Symmetric Cipher
//
CSymmetricCipher* CSymmetricCipher::NewL(MSymmetricCipher* aSymmetricCipher, TInt aHandle)
	{
	CSymmetricCipher* self=new(ELeave) CSymmetricCipher(aSymmetricCipher, aHandle);
	return self;		
	}

CSymmetricCipher::CSymmetricCipher(MSymmetricCipher* aSymmetricCipher, TInt aHandle)
: CSymmetricCipherBase(aSymmetricCipher, aHandle)
	{	
	}

EXPORT_C CSymmetricCipher::~CSymmetricCipher()
	{
	}

EXPORT_C void CSymmetricCipher::ProcessL(const TDesC8& aInput, TDes8& aOutput)
	{
	MSymmetricCipher* ptr=static_cast<MSymmetricCipher*>(iPlugin);
	ptr->ProcessL(aInput, aOutput);
	}


EXPORT_C void CSymmetricCipher::ProcessFinalL(const TDesC8& aInput, TDes8& aOutput)
	{
	MSymmetricCipher* ptr=static_cast<MSymmetricCipher*>(iPlugin);
	ptr->ProcessFinalL(aInput, aOutput);		
	}


//
// Implementation of Symmetric Cipher Factory
//
EXPORT_C void CSymmetricCipherFactory::CreateSymmetricCipherL(CSymmetricCipher*& aCipher,
															TUid aAlgorithmUid,
															const CKey& aKey,
															TUid aCryptoMode,
															TUid aOperationMode,
															TUid aPaddingMode,
															const CCryptoParams* aAlgorithmParams)
	{
	MPluginSelector* selector=reinterpret_cast<MPluginSelector *>(Dll::Tls());
	if (selector)
		{
		selector->CreateSymmetricCipherL(aCipher, aAlgorithmUid, aKey, aCryptoMode, aOperationMode, aPaddingMode, aAlgorithmParams);
		}
	else
		{
		CLegacySelector* legacySelector=CLegacySelector::NewLC();
		legacySelector->CreateSymmetricCipherL(aCipher, aAlgorithmUid, aKey, aCryptoMode, aOperationMode, aPaddingMode, aAlgorithmParams);
		CleanupStack::PopAndDestroy(legacySelector); //selector	
		}
	}


//
// Implementation of Asynchronous Symmetric Cipher
// (async methods not implemented, so no coverage)
//

#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif

CAsyncSymmetricCipher* CAsyncSymmetricCipher::NewL(MAsyncSymmetricCipher* aAsyncSymmetricCipher, TInt aHandle)
	{
	CAsyncSymmetricCipher* self=new(ELeave) CAsyncSymmetricCipher(aAsyncSymmetricCipher, aHandle);
	return self;		
	}

CAsyncSymmetricCipher::CAsyncSymmetricCipher(MAsyncSymmetricCipher* aAsyncSymmetricCipher, TInt aHandle)
: CSymmetricCipherBase(aAsyncSymmetricCipher, aHandle)
	{
	}

EXPORT_C CAsyncSymmetricCipher::~CAsyncSymmetricCipher()
	{
		
	}
	
EXPORT_C void CAsyncSymmetricCipher::ProcessL(const TDesC8& aInput, TDes8& aOutput, TRequestStatus& aRequestStatus)
	{
	MAsyncSymmetricCipher* ptr=static_cast<MAsyncSymmetricCipher*>(iPlugin);
	ptr->ProcessL(aInput, aOutput, aRequestStatus);		
	}

EXPORT_C void CAsyncSymmetricCipher::ProcessFinalL(const TDesC8& aInput, TDes8& aOutput, TRequestStatus& aRequestStatus)
	{
	MAsyncSymmetricCipher* ptr=static_cast<MAsyncSymmetricCipher*>(iPlugin);
	ptr->ProcessFinalL(aInput, aOutput, aRequestStatus);
	}

EXPORT_C void CAsyncSymmetricCipher::Cancel()
	{
	MAsyncSymmetricCipher* ptr=static_cast<MAsyncSymmetricCipher*>(iPlugin);
	ptr->Cancel();		
	}

//
// Implementation of Symmetric Cipher Factory
//
EXPORT_C void CSymmetricCipherFactory::CreateAsyncSymmetricCipherL(CAsyncSymmetricCipher*& aCipher,
														TUid aAlgorithmUid,
														const CKey& aKey,
														TUid aCryptoMode,
														TUid aOperationMode,
														TUid aPaddingMode,
														const CCryptoParams* aAlgorithmParams)
	{
	MPluginSelector* selector=reinterpret_cast<MPluginSelector *>(Dll::Tls());
	if (selector)
		{
		selector->CreateAsyncSymmetricCipherL(aCipher, aAlgorithmUid, aKey, aCryptoMode, aOperationMode, aPaddingMode, aAlgorithmParams);
		}
	else
		{
		CLegacySelector* legacySelector=CLegacySelector::NewLC();
		legacySelector->CreateAsyncSymmetricCipherL(aCipher, aAlgorithmUid, aKey, aCryptoMode, aOperationMode, aPaddingMode, aAlgorithmParams);
		CleanupStack::PopAndDestroy(legacySelector); //selector	
		}	
	}

