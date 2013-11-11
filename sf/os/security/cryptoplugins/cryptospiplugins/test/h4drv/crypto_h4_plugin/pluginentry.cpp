/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
 @internalComponent
 @released
*/
#include "pluginentry.h"
#include "pluginconfig.h"
#include "keys.h"
#include "randomimpl.h"
#include "rijndaelimpl.h"
#include "randomimpl.h"
#include <cryptospi/cryptospidef.h>


using namespace HwCrypto;

EXPORT_C const TCharacteristics** CCryptoPluginEntry::Enumerate(TUid aInterface, TInt& aNumPlugins)
	{
	const TCharacteristics** ptr(0);
	aNumPlugins = 0;

	switch (aInterface.iUid)
		{
	case KRandomInterface:
		{
		aNumPlugins=sizeof(KRandomCharacteristics)/sizeof(TRandomCharacteristics*);
		ptr= (const TCharacteristics**) &KRandomCharacteristics[0];			
		}
		break;

	case KSymmetricCipherInterface:
		{
		aNumPlugins=sizeof(KSymmetricCipherCharacteristics)/sizeof(TSymmetricCipherCharacteristics*);
		ptr = (const TCharacteristics**) &KSymmetricCipherCharacteristics[0];			
		}
		break;

	default:;
		}
		
	return ptr;
	}

EXPORT_C void CCryptoPluginEntry::GetExtendedCharacteristicsL(TUid aImplementationUid, CExtendedCharacteristics*& aExt)
	{
	switch (aImplementationUid.iUid)
		{
	case KCryptoPluginRandom:          
		aExt=CRandomImpl::StaticGetExtendedCharacteristicsL();
		break;
	case KCryptoPluginAes:
		aExt=CH4RijndaelImpl::StaticGetExtendedCharacteristicsL();
		break;
	default:
		User::Leave(KErrNotSupported);
		}
	}	
	
EXPORT_C void CCryptoPluginEntry::CreateAsymmetricCipherL(MAsymmetricCipher*& /*aPlugin*/,
														  TUid /*aImplementationId*/,
														  const CKey& /*aKey*/,
														  TUid /*aCryptoMode*/,
														  TUid /*aPaddingMode*/,
														  const CCryptoParams* /*aAlgorithmParams*/)
	{
	User::Leave(KErrNotFound);
	}
														
														
														
EXPORT_C void CCryptoPluginEntry::CreateAsymmetricSignerL(MSigner*& /*aPlugin*/,
														  TUid /*aImplementationId*/,
														  const CKey& /*aKey*/,
														  TUid /*aPaddingMode*/,
														const CCryptoParams* /*aAlgorithmParams*/)
	{
	User::Leave(KErrNotFound);
	}
														
														
EXPORT_C void CCryptoPluginEntry::CreateAsymmetricVerifierL(MVerifier*& /*aPlugin*/,
															TUid /*aImplementationId*/,
															const CKey& /*aKey*/,
															TUid /*aPaddingMode*/,
															const CCryptoParams* /*aAlgorithmParams*/)
	{
	User::Leave(KErrNotFound);
	}																

EXPORT_C void CCryptoPluginEntry::CreateHashL(MHash*& /*aPlugin*/,
											  TUid /*aImplementationId*/,
											  TUid /*aOperationMode*/,
											  const CKey* /*aKey*/,											
											const CCryptoParams* /*aAlgorithmParams*/)
	{
	User::Leave(KErrNotFound);
	}
	
EXPORT_C void CCryptoPluginEntry::CreateKeyAgreementL(MKeyAgreement*& /*aPlugin*/,
													  TUid /*aImplementationId*/,
													  const CKey& /*aPrivateKey*/,
													  const CCryptoParams* /*aAlgorithmParams*/)
	{
	User::Leave(KErrNotFound);
	}

EXPORT_C void CCryptoPluginEntry::CreateKeyPairGeneratorL(MKeyPairGenerator*& /*aPlugin*/,
														  TUid /*aImplementationId*/,
														  const CCryptoParams* /*aAlgorithmParams*/)
	{
	User::Leave(KErrNotFound);
	}
	
														
EXPORT_C void CCryptoPluginEntry::CreateRandomL(MRandom*& aPlugin,
												TUid /*aImplementationId*/,
												const CCryptoParams* /*aAlgorithmParams*/)
	{
	aPlugin=CRandomImpl::NewL();
	//User::Leave(KErrNotFound);
	}	

EXPORT_C void CCryptoPluginEntry::CreateSymmetricCipherL(MSymmetricCipher*& aPlugin,
														 TUid aImplementationId,
														 const CKey& aKey,
														 TUid aCryptoMode,
														 TUid aOperationMode,
														 TUid aPadding,
														 const CCryptoParams* /*aAlgorithmParams*/)
	{
	switch (aImplementationId.iUid)
		{
	case KCryptoPluginAes:
		{
		aPlugin=CH4RijndaelImpl::NewL(aKey, aCryptoMode, aOperationMode, aPadding);
		//User::Leave(KErrNotFound);
		}	
		break;
		
	default:
		User::Leave(KErrNotFound);
		}
	}												
														
// End of file
