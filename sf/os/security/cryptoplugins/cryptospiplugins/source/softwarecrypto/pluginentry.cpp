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
* plugin entry implementation
* plugin entry implementation
*
*/


/**
 @file
*/

#include "pluginentry.h"
#include "keypair.h"
#include "pluginconfig.h"
#include "md2impl.h"
#include "md5impl.h"
#include "md4impl.h"
#include "sha1impl.h"
#include "hmacimpl.h"
#include "keys.h"
#include "3desimpl.h"
#include "desimpl.h"
#include "rc2impl.h"
#include "arc4impl.h"
#include "randomimpl.h"
#include "rijndaelimpl.h"
#include "randomimpl.h"
#include "rsaimpl.h"
#include "dhimpl.h"
#include "dhkeypairgenimpl.h"
#include "rsakeypairgenimpl.h"
#include "dsakeypairgenimpl.h"
#include "dsasignerimpl.h"
#include "dsaverifyimpl.h"
#include "rsasignerimpl.h"
#include "rsaverifyimpl.h"
#include <cryptospi/cryptospidef.h>

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT	
	#include "macimpl.h"
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

	case KAsymmetricCipherInterface:
		{
		aNumPlugins=sizeof(KAsymmetricCipherCharacteristics)/sizeof(TAsymmetricCipherCharacteristics*);
		ptr = (const TCharacteristics**) &KAsymmetricCipherCharacteristics[0];
		}
		break;

	case KSignerInterface:
		{
		aNumPlugins=sizeof(KSignerCharacteristics)/sizeof(TAsymmetricSignatureCharacteristics*);
		ptr = (const TCharacteristics**) &KSignerCharacteristics[0];
		}
		break;

	case KVerifierInterface:
		{
		aNumPlugins=sizeof(KVerifierCharacteristics)/sizeof(TAsymmetricSignatureCharacteristics*);
		ptr = (const TCharacteristics**) &KVerifierCharacteristics[0];
		}
		break;

	case KKeyAgreementInterface:
		{
		aNumPlugins=sizeof(KKeyAgreementCharacteristics)/sizeof(TKeyAgreementCharacteristics*);
		ptr = (const TCharacteristics**) &KKeyAgreementCharacteristics[0];
		}
		break;

	case KKeypairGeneratorInterface:
		{
		aNumPlugins=sizeof(KKeyPairGeneratorCharacteristics)/sizeof(TAsymmetricKeypairGeneratorCharacteristics*);
		ptr = (const TCharacteristics**) &KKeyPairGeneratorCharacteristics[0];
		}
		break;

	case KKeyGeneratorInterface:
		break;
		
	default:;
		}
		
	return ptr;
	}

EXPORT_C void CCryptoPluginEntry::GetExtendedCharacteristicsL(TUid aImplementationUid, CExtendedCharacteristics*& aExt)
	{
	switch (aImplementationUid.iUid)
		{
		
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT	
	case KCryptoPluginMacHashMd2:   
#endif
	
	case KCryptoPluginMd2:
	   aExt = CMD2Impl::CreateExtendedCharacteristicsL();
	   break;
	   
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT	
	case KCryptoPluginMacHashMd5:   
#endif
	
	case KCryptoPluginMd5:
	   aExt = CMD5Impl::CreateExtendedCharacteristicsL();
	   break;
	   
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT	
	case KCryptoPluginMacHashSha1:   
#endif
	
	case KCryptoPluginSha1:
	   aExt = CSHA1Impl::CreateExtendedCharacteristicsL();
	   break;
	   
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT	
	case KCryptoPluginMacHashMd4:   
#endif
	
	case KCryptoPluginMd4:
	   aExt = CMD4Impl::CreateExtendedCharacteristicsL();
	   break;
	case KCryptoPluginRandom:
	   aExt = CRandomImpl::CreateExtendedCharacteristicsL();
	   break;
	case KCryptoPluginDes:
	   aExt = CDesImpl::CreateExtendedCharacteristicsL();
	   break;
	case KCryptoPlugin3Des:
	   aExt = C3DesImpl::CreateExtendedCharacteristicsL();
	   break;
	case KCryptoPluginRc2:
	   aExt = CRc2Impl::CreateExtendedCharacteristicsL();
	   break;
	case KCryptoPluginArc4:
	   aExt = CArc4Impl::CreateExtendedCharacteristicsL();
	   break;
	   
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT	
	case KCryptoPluginMacAesXcbcMac96:   
	case KCryptoPluginMacAesXcbcPrf128:
#endif	
	
	case KCryptoPluginAes:
	   aExt = CRijndaelImpl::CreateExtendedCharacteristicsL();
	   break;
	case KCryptoPluginRsaCipher:
	   aExt = CRSAImpl::CreateExtendedCharacteristicsL();
	   break;
	case KCryptoPluginRsaSigner:
	   aExt = CRSASignerImpl::CreateExtendedCharacteristicsL();
	   break;
	case KCryptoPluginDsaSigner:
	   aExt = CDSASignerImpl::CreateExtendedCharacteristicsL();
	   break;
	case KCryptoPluginRsaVerifier:
	   aExt = CRSAVerifierImpl::CreateExtendedCharacteristicsL();
	   break;
	case KCryptoPluginDsaVerifier:
	   aExt = CDSAVerifierImpl::CreateExtendedCharacteristicsL();
	   break;
	case KCryptoPluginDhKeyAgreement:
	   aExt = CDHImpl::CreateExtendedCharacteristicsL();
	   break;
	case KCryptoPluginDhKeyPairGen:
	   aExt = CDHKeyPairGenImpl::CreateExtendedCharacteristicsL();
	   break;
	case KCryptoPluginRsaKeyPairGen:
	   aExt = CRSAKeyPairGenImpl::CreateExtendedCharacteristicsL();
	   break;
	case KCryptoPluginDsaKeyPairGen:
	   aExt = CDSAKeyPairGenImpl::CreateExtendedCharacteristicsL();
	   break;
	default:
		User::Leave(KErrNotSupported);
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
	case KCryptoPluginRsaCipher:
		{
		aPlugin = CRSAImpl::NewL(aKey, aCryptoMode, aPaddingMode);
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
	case KCryptoPluginRsaSigner:
		{
		aPlugin = CRSASignerImpl::NewL(aKey, aPaddingMode);
		}	
		break;

	case KCryptoPluginDsaSigner:
		{
		aPlugin = CDSASignerImpl::NewL(aKey);
		}	
		break;

	default:
		User::Leave(KErrNotFound);
		}
	}
	
EXPORT_C void CCryptoPluginEntry::CreateAsymmetricVerifierL(MVerifier*& aPlugin,
															TUid aImplementationId,
															const CKey& aKey,
															TUid aPaddingMode,
															const CCryptoParams* /*aAlgorithmParams*/)
	{
	switch (aImplementationId.iUid)
		{
	case KCryptoPluginRsaVerifier:
		aPlugin = CRSAVerifierImpl::NewL(aKey, aPaddingMode);
		break;
	
	case KCryptoPluginDsaVerifier:
		{
		aPlugin = CDSAVerifierImpl::NewL(aKey);
		}	
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

EXPORT_C void CCryptoPluginEntry::CreateKeyAgreementL(MKeyAgreement*& aPlugin,
														TUid aImplementationId,
														const CKey& aPrivateKey,
														const CCryptoParams* aAlgorithmParams)
	{
	// create requested key agreement implementation in this plugin
	switch (aImplementationId.iUid)
		{
	case KCryptoPluginDhKeyAgreement:
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
	// create requested key pair generator implementation in this plugin
	switch (aImplementationId.iUid)
		{
	case KCryptoPluginDhKeyPairGen:
		{
		aPlugin = CDHKeyPairGenImpl::NewL();
		}
		break;
		
	case KCryptoPluginRsaKeyPairGen:
		{
		aPlugin = CRSAKeyPairGenImpl::NewL();
		}
		break;
		
	case KCryptoPluginDsaKeyPairGen:
		{
		aPlugin = CDSAKeyPairGenImpl::NewL();
		}
		break;
		
	default:
		User::Leave(KErrNotFound);
		}
	}
	
EXPORT_C void CCryptoPluginEntry::CreateRandomL(MRandom*& aPlugin,
												TUid /*aImplementationId*/,
												const CCryptoParams* /*aAlgorithmParams*/)
	{
	aPlugin=CRandomImpl::NewL();
	}	

EXPORT_C void CCryptoPluginEntry::CreateSymmetricCipherL(MSymmetricCipher*& aPlugin,
														TUid aImplementationId,
														const CKey& aKey,
														TUid aCryptoMode,
														TUid aOperationMode,
														TUid aPadding,
														const CCryptoParams* aAlgorithmParams)
	{
	switch (aImplementationId.iUid)
		{
	case KCryptoPluginAes:
		{
		aPlugin=CRijndaelImpl::NewL(
			aKey, aCryptoMode, aOperationMode, aPadding);
		}	
		break;
	case KCryptoPluginDes:
		{
		aPlugin=CDesImpl::NewL(
			aKey, aCryptoMode, aOperationMode, aPadding);
		}	
		break;
	case KCryptoPlugin3Des:
		{
		aPlugin=C3DesImpl::NewL(
			aKey, aCryptoMode, aOperationMode, aPadding);
		}	
		break;
	case KCryptoPluginRc2:
		{
		TInt effectiveKeyLenBits(CRc2Impl::KDefaultEffectiveKeyLenBits);
			
		if (aAlgorithmParams && aAlgorithmParams->IsPresent(KRC2EffectiveKeyLenBits))
			{
			effectiveKeyLenBits = aAlgorithmParams->GetTIntL(KRC2EffectiveKeyLenBits);
			if (effectiveKeyLenBits < 0)
				{
				User::Leave(KErrArgument);
				}
			}
		aPlugin=CRc2Impl::NewL(
			aKey, aCryptoMode, aOperationMode, aPadding, effectiveKeyLenBits);
		}	
		break;				
	
	case KCryptoPluginArc4:
		{
		TInt discardBytes(CArc4Impl::KDefaultDiscardBytes);
		if (aAlgorithmParams && aAlgorithmParams->IsPresent(KARC4DiscardBytes))
			{
			discardBytes = aAlgorithmParams->GetTIntL(KARC4DiscardBytes);
			if (discardBytes < 0)
				{
				User::Leave(KErrArgument);
				}
			}		
		aPlugin=CArc4Impl::NewL(aKey, discardBytes);
		}
		break;
		
	default:
		User::Leave(KErrNotFound);
		}
	}

