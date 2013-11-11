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
#include <cryptospi/keypair.h>
#include "pluginconfig.h" 
#include "md5impl.h"
#include "hmacimpl.h"
#include <cryptospi/keys.h>
#include "desimpl.h"
#include "desextendimpl.h"
#include "rsaimpl.h"
#include "rsakeypairgenimpl.h"
#include "randomimpl.h"
#include "rsasignerimpl.h"
#include "dhimpl.h"
#include "dhkeypairgenimpl.h"
#include <cryptospi/cryptospidef.h>	

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
#include "rijndaelimpl.h"
#include "macimpl.h"

#endif

using namespace SoftwareCrypto;

EXPORT_C const TCharacteristics** CCryptoPluginEntry::Enumerate(TUid aInterface, TInt& aNumPlugins)
	{
	const TCharacteristics** ptr(0);
	switch (aInterface.iUid)
		{
	case KSymmetricCipherInterface:
		{
		aNumPlugins=sizeof(KSymmetricCipherCharacteristics)/sizeof(TSymmetricCipherCharacteristics*);
		ptr = (const TCharacteristics**) &KSymmetricCipherCharacteristics[0];			
		}
		break;

	case KAsymmetricCipherInterface:
		{
		aNumPlugins=sizeof(KAsymmetricCipherCharacteristics)/sizeof(TAsymmetricCipherCharacteristics*);
		ptr = (const TCharacteristics**) &KAsymmetricCipherCharacteristics[0];			
		}
		break;
		
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

	case KRandomInterface:
		{
		aNumPlugins=sizeof(KRandomCharacteristics)/sizeof(TRandomCharacteristics*);
		ptr= (const TCharacteristics**) &KRandomCharacteristics[0];			
		}
		break;
		
	case KKeypairGeneratorInterface:
		{
		aNumPlugins=sizeof(KKeyPairGeneratorCharacteristics)/sizeof(TAsymmetricKeypairGeneratorCharacteristics*);
		ptr = (const TCharacteristics**) &KKeyPairGeneratorCharacteristics[0];			
		}
		break;
		
	case KSignerInterface:
		{
		aNumPlugins=sizeof(KSignerCharacteristics)/sizeof(TAsymmetricSignatureCharacteristics*);
		ptr = (const TCharacteristics**) &KSignerCharacteristics[0];
		}
		break;
		
	case KKeyAgreementInterface:
		{
		aNumPlugins=sizeof(KKeyAgreementCharacteristics)/sizeof(TKeyAgreementCharacteristics*);
		ptr = (const TCharacteristics**) &KKeyAgreementCharacteristics[0];
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
		case KTestPlugin02Md5_1:
			{
			aExt = CMD5Impl::CreateExtendedCharacteristicsL();	
			}
		    break;
	   
		case KTestPlugin02Random_1:
		case KTestPlugin02Random_2:
			{
			aExt = CRandomImpl::CreateExtendedCharacteristicsL();	
			}
		   	break;

		case KTestPlugin02Des_1:
			{
			aExt = CDesExtendImpl::CreateExtendedCharacteristicsL();	
			}
		   	break;
	   
		case KTestPlugin02Des_2:
			{
			aExt = CDesImpl::CreateExtendedCharacteristicsL();	
			}
		   	break;
	
		case KTestPlugin02Rsa_1:   
		case KTestPlugin02Rsa_2:
			{
			aExt = CRSAImpl::CreateExtendedCharacteristicsL();	
			}
		    break;
 
	 	case KTestPlugin02RsaKeyGen_1:
		 	{
		 	aExt = CRSAKeyPairGenImpl::CreateExtendedCharacteristicsL();	
		 	}
		   	break;
	   
		case KTestPlugin02DHKeyGen_1:
			{
			aExt = CDHKeyPairGenImpl::CreateExtendedCharacteristicsL();	
			}
		   	break;
	   
		case KTestPlugin02RsaSigner_1:
			{
			aExt = CRSASignerImpl::CreateExtendedCharacteristicsL();	
			}
		   	break;
	   
		case KTestPlugin02DHKeyAgree_1:
			{
			aExt = CDHKeyPairGenImpl::CreateExtendedCharacteristicsL();	
			}
		  	break;
			
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
		case KTestPlugin02XcbcMac96:
		case KTestPlugin02XcbcPrf128:
		case KTestPlugin02Aes:
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

	
EXPORT_C void CCryptoPluginEntry::CreateAsymmetricCipherL(MAsymmetricCipher*& aPlugin,
														TUid aImplementationId,
														const CKey& aKey,
														TUid aCryptoMode,
														TUid aPaddingMode,
														const CCryptoParams* /*aAlgorithmParams*/)
	{
	switch (aImplementationId.iUid)
		{

		case KTestPlugin02Rsa_1:
		case KTestPlugin02Rsa_2:
			{
			aPlugin = CRSAImpl::NewL(aImplementationId, aKey, aCryptoMode, aPaddingMode);
			}
			break;
		
		default:
			User::Leave(KErrNotFound);
		}	
	}
	
														
EXPORT_C void CCryptoPluginEntry::CreateAsymmetricSignerL(MSigner*& aPlugin,
														TUid aImplementationId,
														const CKey& aKey,
														TUid aPaddingMode,
														const CCryptoParams* /*aAlgorithmParams*/)
	{
	switch (aImplementationId.iUid)
		{
		case KTestPlugin02RsaSigner_1:
			{
			aPlugin = CRSASignerImpl::NewL(aImplementationId, aKey, aPaddingMode);
			}	
			break;

		default:
			User::Leave(KErrNotFound);
			}
	}
														
																								
EXPORT_C void CCryptoPluginEntry::CreateAsymmetricVerifierL(MVerifier*& /*aPlugin*/,
															TUid /*aImplementationId*/,
															const CKey& /*aKey*/,
															TUid /*aPaddingMode*/,
															const CCryptoParams* /*aAlgorithmParams*/)
	{
	User::Leave(KErrNotFound);
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


EXPORT_C void CCryptoPluginEntry::CreateKeyAgreementL(MKeyAgreement*& aPlugin,
														TUid aImplementationId,
														const CKey& aPrivateKey,
														const CCryptoParams* aAlgorithmParams)
	{
	// create requested key agreement implementation in this plugin
	switch (aImplementationId.iUid)
		{
		case KTestPlugin02DHKeyAgree_1:
			{
			aPlugin = CDHImpl::NewL(aPrivateKey, aAlgorithmParams);
			}
			break;

		default:
			User::Leave(KErrNotFound);
		}	
	}
	
														
EXPORT_C void CCryptoPluginEntry::CreateKeyPairGeneratorL(MKeyPairGenerator*& aPlugin,
														TUid aImplementationId,
														const CCryptoParams* /*aAlgorithmParams*/)
	{
	//create requested key pair generator implementation in this plugin
	switch (aImplementationId.iUid)
		{
			
		case KTestPlugin02RsaKeyGen_1:
			{
			aPlugin = CRSAKeyPairGenImpl::NewL(aImplementationId);
			}
			break;
			
		case KTestPlugin02DHKeyGen_1:
			{
			aPlugin = CDHKeyPairGenImpl::NewL(aImplementationId);
			}
			break;
			
		default:
			User::Leave(KErrNotFound);
			}	
	}
	
														
EXPORT_C void CCryptoPluginEntry::CreateRandomL(MRandom*& aPlugin,
												TUid aImplementationId,
												const CCryptoParams* /*aAlgorithmParams*/)
	{
	
	switch (aImplementationId.iUid)
		{
		
		case KTestPlugin02Random_1:
		case KTestPlugin02Random_2:
			{
			aPlugin=CRandomImpl::NewL(aImplementationId);
			}	
			break;

		default:
			User::Leave(KErrNotFound);
		}
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
		case KTestPlugin02Des_1:
			{
			aPlugin=CDesExtendImpl::NewL(aImplementationId, aKey, aCryptoMode, aOperationMode, aPadding);
			}	
			break;
		
		case KTestPlugin02Des_2:
			{
			aPlugin=CDesImpl::NewL(aImplementationId, aKey, aCryptoMode, aOperationMode, aPadding);
			}	
			break;

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
		case KTestPlugin02Aes:
			{
			aPlugin=CRijndaelImpl::NewL(aKey, aCryptoMode, aOperationMode, aPadding, KTestPlugin02AesUid);
			}
			break;
#endif

		default:
			User::Leave(KErrNotFound);
		}
	
	}

