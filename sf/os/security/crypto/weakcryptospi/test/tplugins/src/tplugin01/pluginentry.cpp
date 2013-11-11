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
* plugin entry implementation
* plugin entry implementation
*
*/


/**
 @file
*/

#include "pluginentry.h"
#include "pluginconfig.h"
#include "md2impl.h"
#include "hmacimpl.h"
#include <cryptospi/keys.h>
#include "desimpl.h"
#include "rsakeypairgenextendimpl.h"
#include "rsaverifyimpl.h"
#include <cryptospi/cryptospidef.h>

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
#include "macimpl.h"
#include "rijndaelimpl.h"
#endif

using namespace SoftwareCrypto;

EXPORT_C const TCharacteristics** CCryptoPluginEntry::Enumerate(TUid aInterface, TInt& aNumPlugins)
	{
	const TCharacteristics** ptr(0);
	switch (aInterface.iUid)
		{
	case KHashInterface:
		{
		aNumPlugins=sizeof(KHashCharacteristics)/sizeof(THashCharacteristics*);
		ptr = (const TCharacteristics**) &KHashCharacteristics[0];			
		}
		break;
		
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
	case KMacInterface:
		{
		aNumPlugins=sizeof(KMacCharacteristics)/sizeof(TMacCharacteristics*);
		ptr = (const TCharacteristics**) &KMacCharacteristics[0];
		}
		break;
#endif

	case KSymmetricCipherInterface:
		{
		aNumPlugins=sizeof(KSymmetricCipherCharacteristics)/sizeof(TSymmetricCipherCharacteristics*);
		ptr = (const TCharacteristics**) &KSymmetricCipherCharacteristics[0];			
		}
		break;
		
	case KKeypairGeneratorInterface:
		{
		aNumPlugins=sizeof(KKeyPairGeneratorCharacteristics)/sizeof(TAsymmetricKeypairGeneratorCharacteristics*);
		ptr = (const TCharacteristics**) &KKeyPairGeneratorCharacteristics[0];			
		}
		break;
		
	case KVerifierInterface:
		{
		aNumPlugins=sizeof(KVerifierCharacteristics)/sizeof(TAsymmetricSignatureCharacteristics*);
		ptr = (const TCharacteristics**) &KVerifierCharacteristics[0];
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
		case KTestPlugin01Md2_1:
		case KTestPlugin01Md2_2:
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
		case KTestPlugin01MacMd2_1:
		case KTestPlugin01MacMd2_2:
#endif
			{
			aExt = CMD2Impl::CreateExtendedCharacteristicsL();	
			}
		   	break;

		case KTestPlugin01Des_1:
		case KTestPlugin01Des_2:
		case KTestPlugin01Des_3:
		case KTestPlugin01Des_4:
			{
			aExt = CDesImpl::CreateExtendedCharacteristicsL();	
			}
		   	break;
	   
		case KTestPlugin01RsaKeyGen_1:
			{
			aExt = CRSAKeyPairGenExtendImpl::CreateExtendedCharacteristicsL();
			}
		   	break;
	   
		case KTestPlugin01RsaVerifier_1:
		case KTestPlugin01RsaVerifier_2:
			{
			aExt = CRSAVerifierImpl::CreateExtendedCharacteristicsL();	
			}
		   	break;

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
		case KTestPlugin01XcbcMac96:
		case KTestPlugin01Aes:
			{
			aExt = CRijndaelImpl::CreateExtendedCharacteristicsL();
			}
			break;
#endif
			
		default:
			{
			User::Leave(KErrNotSupported);	
			}
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
														
														
														
														
EXPORT_C void CCryptoPluginEntry::CreateAsymmetricVerifierL(MVerifier*& aPlugin,
															TUid aImplementationId,
															const CKey& aKey,
															TUid aPaddingMode,
															const CCryptoParams* /*aAlgorithmParams*/)
	{
	switch (aImplementationId.iUid)
		{
		
		case KTestPlugin01RsaVerifier_1:
		case KTestPlugin01RsaVerifier_2:
			aPlugin = CRSAVerifierImpl::NewL(aImplementationId, aKey, aPaddingMode);
			break;
			
		default:
			User::Leave(KErrNotFound);
		}
	}																

EXPORT_C void CCryptoPluginEntry::CreateHashL(MHash*& aPlugin,
											TUid aImplementationId,
											TUid aOperationMode,
											const CKey* aKey,
											const CCryptoParams* /*aAlgorithmParams*/)
	{
	aPlugin=CSoftwareHash::NewL(aImplementationId, aOperationMode, aKey);
	}

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
EXPORT_C void CCryptoPluginEntry::CreateHashL(MHash*& aPlugin,
											TUid aImplementationId,
											const CCryptoParams* /*aAlgorithmParams*/)
	{
	aPlugin=CSoftwareHash::NewL(aImplementationId);
	}

EXPORT_C void CCryptoPluginEntry::CreateMacL(MMac*& aPlugin,
											const TUid aImplementationId,
											const CKey& aKey,
											const CCryptoParams* aAlgorithmParams)
	{
	aPlugin=CMacImpl::NewL(aKey, aImplementationId, aAlgorithmParams);
	}
#endif


EXPORT_C void CCryptoPluginEntry::CreateKeyAgreementL(MKeyAgreement*& /*aPlugin*/,
														TUid /*aImplementationId*/,
														const CKey& /*aPrivateKey*/,
														const CCryptoParams* /*aAlgorithmParams*/)
	{
	User::Leave(KErrNotFound);	
	}
														
EXPORT_C void CCryptoPluginEntry::CreateKeyPairGeneratorL(MKeyPairGenerator*& aPlugin,
														TUid aImplementationId,
														const CCryptoParams* /*aAlgorithmParams*/)
	{
	//create requested key pair generator implementation in this plugin
	switch (aImplementationId.iUid)
		{
			
		case KTestPlugin01RsaKeyGen_1:
			{
			aPlugin = CRSAKeyPairGenExtendImpl::NewL(aImplementationId);
			}
			break;
			
		default:
			User::Leave(KErrNotFound);
			}		
	}
	
														
EXPORT_C void CCryptoPluginEntry::CreateRandomL(MRandom*& /*aPlugin*/,
												TUid /*aImplementationId*/,
												const CCryptoParams* /*aAlgorithmParams*/)
	{
	User::Leave(KErrNotFound);
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
		case KTestPlugin01Des_1:
		case KTestPlugin01Des_2:
		case KTestPlugin01Des_3:
		case KTestPlugin01Des_4:
			{
			aPlugin=CDesImpl::NewL(aImplementationId, aKey, aCryptoMode, aOperationMode, aPadding);
			}	
			break;		

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
		case KTestPlugin01Aes:
			{
			aPlugin=CRijndaelImpl::NewL(aKey, aCryptoMode, aOperationMode, aPadding, KTestPlugin01AesUid);
			}
			break;
#endif
			
		default:
			User::Leave(KErrNotFound);
			}
	}

