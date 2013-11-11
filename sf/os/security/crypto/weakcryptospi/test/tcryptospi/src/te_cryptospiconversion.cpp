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
*
*/


/**
 @file
 @internalTechnology
*/

#include "te_cryptospiconversion.h"
#include "te_cryptospidefs.h"
#include <cryptospi/cryptohashapi.h>
#include <cryptospi/cryptorandomapi.h>
#include <cryptospi/cryptosymmetriccipherapi.h>
#include <cryptospi/cryptoasymmetriccipherapi.h>
#include <cryptospi/cryptosignatureapi.h>
#include <cryptospi/cryptokeypairgeneratorapi.h>
#include <cryptospi/cryptokeyagreementapi.h>
#include <cryptospi/ruleselector.h>

using namespace CryptoSpi;

//Returns the Uid representation for the stored string value
TVariantPtrC::operator TUid()
	{
	if (*this==_L("KAlgorithmHashMd2"))	return KMd2Uid;	
	else if (*this==_L("KAlgorithmHashMd5")) return KMd5Uid;
	else if (*this==_L("KAlgorithmHashSha1")) return KSha1Uid;
	else if (*this==_L("KAlgorithmHashMd4")) return KMd4Uid;
	else if (*this==_L("KAlgorithmHashSha224")) return KSha224Uid;
	else if (*this==_L("KAlgorithmHashSha256")) return KSha256Uid;
	else if (*this==_L("KAlgorithmHashSha384")) return KSha384Uid;
	else if (*this==_L("KAlgorithmHashSha512")) return KSha512Uid;
	else if (*this==_L("KAlgorithmRandom")) return KRandomUid;	
	else if (*this==_L("KAlgorithmCipherDes")) return KDesUid;	
	else if (*this==_L("KAlgorithmCipher3Des")) return K3DesUid;
	else if (*this==_L("KAlgorithmCipherRc2")) return KRc2Uid;
	else if (*this==_L("KAlgorithmCipherArc4")) return KArc4Uid;
	else if (*this==_L("KAlgorithmCipherAes")) return KAesUid;
	else if (*this==_L("KAlgorithmCipherMisty1")) return KMisty1Uid;
	else if (*this==_L("KAlgorithmCipherMisty2")) return KMisty2Uid;
	else if (*this==_L("KAlgorithmCipherKasumi")) return KKasumiUid;
	else if (*this==_L("KAlgorithmCipherRsa")) return KRsaCipherUid;
	else if (*this==_L("KAlgorithmSignerRsa")) return KRsaSignerUid;
	else if (*this==_L("KAlgorithmSignerDsa")) return KDsaSignerUid; 
	else if (*this==_L("KAlgorithmVerifierRsa")) return KRsaVerifierUid;
	else if (*this==_L("KAlgorithmVerifierDsa")) return KDsaVerifierUid;
	else if (*this==_L("KAlgorithmKeyAgreementDH")) return KDHAgreementUid;
	else if (*this==_L("KAlgorithmRSAKeyPairGenerator")) return KRSAKeyPairGeneratorUid;
	else if (*this==_L("KAlgorithmDSAKeyPairGenerator")) return KDSAKeyPairGeneratorUid;
	else if (*this==_L("KAlgorithmDHKeyPairGenerator")) return KDHKeyPairGeneratorUid;

	else if (*this==_L("KHashInterface")) return KHashInterfaceUid;
	else if (*this==_L("KRandomInterface")) return KRandomInterfaceUid;
	else if (*this==_L("KSymmetricCipherInterface")) return KSymmetricCipherInterfaceUid;
	else if (*this==_L("KAsymmetricCipherInterface")) return KAsymmetricCipherInterfaceUid;
	else if (*this==_L("KSignerInterface")) return KSignerInterfaceUid;
	else if (*this==_L("KVerifierInterface")) return KVerifierInterfaceUid;
	else if (*this==_L("KKeyAgreementInterface")) return KKeyAgreementInterfaceUid;
	else if (*this==_L("KKeyGeneratorInterface")) return KKeyGeneratorInterfaceUid;
	else if (*this==_L("KKeypairGeneratorInterface")) return KKeypairGeneratorInterfaceUid;
	else if (*this==_L("KAsyncHashInterface")) return KAsyncHashInterfaceUid;
	else if (*this==_L("KAsyncRandomInterface")) return KAsyncRandomInterfaceUid;
	else if (*this==_L("KAsyncSymmetricCipherInterface")) return KAsyncSymmetricCipherInterfaceUid;
	else if (*this==_L("KAsyncAsymmetricCipherInterface")) return KAsyncAsymmetricCipherInterfaceUid;
	else if (*this==_L("KAsyncSignerInterface")) return KAsyncSignerInterfaceUid;
	else if (*this==_L("KAsyncVerifierInterface")) return KAsyncVerifierInterfaceUid;
	else if (*this==_L("KAsyncKeyAgreementInterface")) return KAsyncKeyAgreementInterfaceUid;
	else if (*this==_L("KAsyncKeyGeneratorInterface")) return KAsyncKeyGeneratorInterfaceUid;
	else if (*this==_L("KAsyncKeypairGeneratorInterface")) return KAsyncKeypairGeneratorInterfaceUid;
	
	else if (*this==_L("KCryptoModeEncrypt")) return KCryptoModeEncryptUid;
	else if (*this==_L("KCryptoModeDecrypt")) return KCryptoModeDecryptUid;
	
	else if (*this==_L("KOperationModeNone")) return KOperationModeNoneUid;	
	else if (*this==_L("KOperationModeECB")) return KOperationModeECBUid;
	else if (*this==_L("KOperationModeCBC")) return KOperationModeCBCUid;
	else if (*this==_L("KOperationModeOFB")) return KOperationModeOFBUid;
	else if (*this==_L("KOperationModeCFB")) return KOperationModeCFBUid;
	else if (*this==_L("KOperationModeCTR")) return KOperationModeCTRUid;
	else if (*this==_L("KHashMode")) return KHashModeUid;
	else if (*this==_L("KHmacMode")) return KHmacModeUid;
	
	else if (*this==_L("KPaddingInterface")) return KPaddingInterfaceUid;	
	else if (*this==_L("KPaddingModeNone")) return KPaddingModeNoneUid;
	else if (*this==_L("KPaddingModeSSLv3")) return KPaddingModeSSLv3Uid;
	else if (*this==_L("KPaddingModePKCS7")) return KPaddingModePKCS7Uid;
	else if (*this==_L("KPaddingModePkcs1_v1_5_Encryption")) return KPaddingModePkcs1_v1_5_EncryptionUid;
	else if (*this==_L("KPaddingModePkcs1_v1_5_Signature")) return KPaddingModePkcs1_v1_5_SignatureUid;
	
	else if (*this==_L("KSymmetricKey")) return	KSymmetricKeyUid;
	else if (*this==_L("KSymmetricKeyParameter")) return KSymmetricKeyParameterUid;
	else if (*this==_L("KRsaKeyParameterN")) return KRsaKeyParameterNUid;
	else if (*this==_L("KHmacKey")) return	KHmacKeyUid;
	else if (*this==_L("KHmacKeyParameter")) return KHmacKeyParameterUid;
			
	else if (*this==_L("KRsaSigner")) return KRsaSignerUid;
	else if (*this==_L("KDsaSigner")) return KDsaSignerUid;
	
	else if (*this==_L("KNone")) return KNoneUid;
	else if (*this==_L("KInterfaceType")) return KInterfaceTypeUid;
	else if (*this==_L("KAlgorithmType")) return KAlgorithmTypeUid;
	else if (*this==_L("KImplementationType")) return KImplementationTypeUid;
	else if (*this==_L("KCreatorNameType")) return KCreatorNameTypeUid;
	else if (*this==_L("KIsFIPSApprovedType")) return KIsFIPSApprovedTypeUid;
	else if (*this==_L("KIsHardwareSupportedType")) return KIsHardwareSupportedTypeUid;
	else if (*this==_L("KMaxConcurrencySupportedType")) return KMaxConcurrencySupportedTypeUid;
	else if (*this==_L("KLatencyType")) return KLatencyTypeUid;
	else if (*this==_L("KThroughputType")) return KThroughputTypeUid;
	else if (*this==_L("KBlockSizeType")) return KBlockSizeTypeUid;
	else if (*this==_L("KOutputSizeType")) return KOutputSizeTypeUid;
	else if (*this==_L("KSupportedOperationModesType")) return KSupportedOperationModesTypeUid;
	else if (*this==_L("KBlockingModeType")) return KBlockingModeTypeUid;
	else if (*this==_L("KMaximumKeyLengthType")) return KMaximumKeyLengthTypeUid;
	else if (*this==_L("KSupportedPaddingModesType")) return KSupportedPaddingModesTypeUid;
	else if (*this==_L("KKeySupportModeType")) return KKeySupportModeTypeUid;
	else if (*this==_L("KConcurrencyType")) return KConcurrencyTypeUid;
	else if (*this==_L("KExclusiveUseType")) return KExclusiveUseTypeUid;
	
	else if (*this==_L("KExtendCharType1")) return KExtendCharType1Uid;
	else if (*this==_L("KExtendCharType2")) return KExtendCharType2Uid;
	else if (*this==_L("KExtendCharType3")) return KExtendCharType3Uid;
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
	else if (*this==_L("KAlgorithmCipherAesXcbcMac96")) return KAesXcbcMac96Uid;
	else if (*this==_L("KAlgorithmCipherAesXcbcPrf128")) return KAesXcbcPrf128Uid;
	else if (*this==_L("KMacInterface")) return KMacInterfaceUid;
	else if (*this==_L("KMacModeType")) return KMacModeTypeUid;
#endif
#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
    else if (*this==_L("KAlgorithmCipherEcc")) return KEccCipherUid;
    else if (*this==_L("KAlgorithmSignerEcc")) return KEccSignerUid;
#endif
	else
		{
		User::Panic(*this,KErrNotFound);
		return TUid::Null();	
		}		
	}	

//Returns the TInt representation for the stored string value	
TVariantPtrC::operator TInt()
	{
	
	if (*this==_L("KAlgorithmHashMd2")) return KAlgorithmHashMd2;	
	else if (*this==_L("KAlgorithmHashMd5")) return KAlgorithmHashMd5;
	else if (*this==_L("KAlgorithmHashSha1")) return KAlgorithmHashSha1;
	else if (*this==_L("KAlgorithmHashSha224")) return KAlgorithmHashSha224;
	else if (*this==_L("KAlgorithmHashSha256")) return KAlgorithmHashSha256;
	else if (*this==_L("KAlgorithmHashSha384")) return KAlgorithmHashSha384;
	else if (*this==_L("KAlgorithmHashSha512")) return KAlgorithmHashSha512;
	else if (*this==_L("KAlgorithmRandom")) return KAlgorithmRandom;	
	else if (*this==_L("KAlgorithmCipherDes")) return KAlgorithmCipherDes;	
	else if (*this==_L("KAlgorithmCipher3Des")) return KAlgorithmCipher3Des;
	else if (*this==_L("KAlgorithmCipherRc2")) return KAlgorithmCipherRc2;
	else if (*this==_L("KAlgorithmCipherArc4")) return KAlgorithmCipherArc4;
	else if (*this==_L("KAlgorithmCipherAes")) return KAlgorithmCipherAes;
	else if (*this==_L("KAlgorithmCipherRsa")) return KAlgorithmCipherRsa;
	else if (*this==_L("KAlgorithmSignerRsa")) return KAlgorithmSignerRsa;
	else if (*this==_L("KAlgorithmSignerDsa")) return KAlgorithmSignerDsa; 
	else if (*this==_L("KAlgorithmVerifierRsa")) return KAlgorithmVerifierRsa;
	else if (*this==_L("KAlgorithmVerifierDsa")) return KAlgorithmVerifierDsa;
	else if (*this==_L("KAlgorithmKeyAgreementDH")) return KAlgorithmKeyAgreementDH;
	else if (*this==_L("KAlgorithmRSAKeyPairGenerator")) return KAlgorithmRSAKeyPairGenerator;
	else if (*this==_L("KAlgorithmDSAKeyPairGenerator")) return KAlgorithmDSAKeyPairGenerator;
	else if (*this==_L("KAlgorithmDHKeyPairGenerator")) return KAlgorithmDHKeyPairGenerator;
	
	else if (*this==_L("KHashInterface")) return KHashInterface;
	else if (*this==_L("KRandomInterface")) return KRandomInterface;
	else if (*this==_L("KSymmetricCipherInterface")) return KSymmetricCipherInterface;
	else if (*this==_L("KAsymmetricCipherInterface")) return KAsymmetricCipherInterface;
	else if (*this==_L("KSignerInterface")) return KSignerInterface;
	else if (*this==_L("KVerifierInterface")) return KVerifierInterface;
	else if (*this==_L("KKeyAgreementInterface")) return KKeyAgreementInterface;
	else if (*this==_L("KKeyGeneratorInterface")) return KKeyGeneratorInterface;
	else if (*this==_L("KKeypairGeneratorInterface")) return KKeypairGeneratorInterface;
	else if (*this==_L("KAsyncHashInterface")) return KAsyncHashInterface;
	else if (*this==_L("KAsyncRandomInterface")) return KAsyncRandomInterface;
	else if (*this==_L("KAsyncSymmetricCipherInterface")) return KAsyncSymmetricCipherInterface;
	else if (*this==_L("KAsyncAsymmetricCipherInterface")) return KAsyncAsymmetricCipherInterface;
	else if (*this==_L("KAsyncSignerInterface")) return KAsyncSignerInterface;
	else if (*this==_L("KAsyncVerifierInterface")) return KAsyncVerifierInterface;
	else if (*this==_L("KAsyncKeyAgreementInterface")) return KAsyncKeyAgreementInterface;
	else if (*this==_L("KAsyncKeyGeneratorInterface")) return KAsyncKeyGeneratorInterface;
	else if (*this==_L("KAsyncKeypairGeneratorInterface")) return KAsyncKeypairGeneratorInterface;
	
	else if (*this==_L("KCryptoModeEncrypt")) return KCryptoModeEncrypt;
	else if (*this==_L("KCryptoModeDecrypt")) return KCryptoModeDecrypt;
	
	else if (*this==_L("KOperationModeNone")) return KOperationModeNone;	
	else if (*this==_L("KOperationModeECB")) return KOperationModeECB;
	else if (*this==_L("KOperationModeCBC")) return KOperationModeCBC;
	else if (*this==_L("KOperationModeOFB")) return KOperationModeOFB;
	else if (*this==_L("KOperationModeCFB")) return KOperationModeCFB;
	else if (*this==_L("KOperationModeCTR")) return KOperationModeCTR;
	else if (*this==_L("KHashMode")) return KHashMode;
	else if (*this==_L("KHmacMode")) return KHmacMode;
	
	else if (*this==_L("KPaddingInterface")) return KPaddingInterface;	
	else if (*this==_L("KPaddingModeNone")) return KPaddingModeNone;
	else if (*this==_L("KPaddingModeSSLv3")) return KPaddingModeSSLv3;
	else if (*this==_L("KPaddingModePKCS7")) return KPaddingModePKCS7;
	else if (*this==_L("KPaddingModePkcs1_v1_5_Encryption")) return KPaddingModePkcs1_v1_5_Encryption;
	else if (*this==_L("KPaddingModePkcs1_v1_5_Signature")) return KPaddingModePkcs1_v1_5_Signature;
	
	else if (*this==_L("KTestPlugin01Des_1")) return KTestPlugin01Des_1;	
	else if (*this==_L("KTestPlugin01Des_2")) return KTestPlugin01Des_2;
	else if (*this==_L("KTestPlugin01Des_3")) return KTestPlugin01Des_3;
	else if (*this==_L("KTestPlugin01Des_4")) return KTestPlugin01Des_4;
	else if (*this==_L("KTestPlugin01Md2_1")) return KTestPlugin01Md2_1;
	else if (*this==_L("KTestPlugin01Md2_2")) return KTestPlugin01Md2_2;
	else if (*this==_L("KTestPlugin01RsaKeyGen_1")) return KTestPlugin01RsaKeyGen_1;
	else if (*this==_L("KTestPlugin01RsaVerifier_1")) return KTestPlugin01RsaVerifier_1;
	else if (*this==_L("KTestPlugin01RsaVerifier_2")) return KTestPlugin01RsaVerifier_2;
	
	else if (*this==_L("KTestPlugin02Random_1")) return KTestPlugin02Random_1;	
	else if (*this==_L("KTestPlugin02Md5_1")) return KTestPlugin02Md5_1;
	else if (*this==_L("KTestPlugin02Des_1")) return KTestPlugin02Des_1;
	else if (*this==_L("KTestPlugin02Des_2")) return KTestPlugin02Des_2;
	else if (*this==_L("KTestPlugin02Rsa_1")) return KTestPlugin02Rsa_1;
	else if (*this==_L("KTestPlugin02Rsa_2")) return KTestPlugin02Rsa_2;
	else if (*this==_L("KTestPlugin02RsaKeyGen_1")) return KTestPlugin02RsaKeyGen_1;
	else if (*this==_L("KTestPlugin02DHKeyGen_1")) return KTestPlugin02DHKeyGen_1;
	else if (*this==_L("KTestPlugin02RsaSigner_1")) return KTestPlugin02RsaSigner_1;
	else if (*this==_L("KTestPlugin02DHKeyAgree_1")) return KTestPlugin02DHKeyAgree_1;
	else if (*this==_L("KTestPlugin02Random_2")) return KTestPlugin02Random_2;
	
	else if (*this==_L("KCryptoPluginMd2")) return KCryptoPluginMd2;	
	else if (*this==_L("KCryptoPluginMd5")) return KCryptoPluginMd5;
	else if (*this==_L("KCryptoPluginSha1")) return KCryptoPluginSha1;
	else if (*this==_L("KCryptoPluginSha224")) return KCryptoPluginSha224;
	else if (*this==_L("KCryptoPluginSha256")) return KCryptoPluginSha256;
	else if (*this==_L("KCryptoPluginSha384")) return KCryptoPluginSha384;
	else if (*this==_L("KCryptoPluginSha512")) return KCryptoPluginSha512;
	else if (*this==_L("KCryptoPluginRandom")) return KCryptoPluginRandom;
	else if (*this==_L("KCryptoPluginDes")) return KCryptoPluginDes;
	else if (*this==_L("KCryptoPlugin3Des")) return KCryptoPlugin3Des;
	else if (*this==_L("KCryptoPluginAes")) return KCryptoPluginAes;
	else if (*this==_L("KCryptoPluginRc2")) return KCryptoPluginRc2;
	else if (*this==_L("KCryptoPluginArc4")) return KCryptoPluginArc4;	
	else if (*this==_L("KCryptoPluginRsaCipher")) return KCryptoPluginRsaCipher;
	else if (*this==_L("KCryptoPluginRsaSigner")) return KCryptoPluginRsaSigner;
	else if (*this==_L("KCryptoPluginDsaSigner")) return KCryptoPluginDsaSigner;
	else if (*this==_L("KCryptoPluginRsaVerifier")) return KCryptoPluginRsaVerifier;
	else if (*this==_L("KCryptoPluginDsaVerifier")) return KCryptoPluginDsaVerifier;
	else if (*this==_L("KCryptoPluginDhKeyAgreement")) return KCryptoPluginDhKeyAgreement;
	else if (*this==_L("KCryptoPluginDhKeyPairGen")) return KCryptoPluginDhKeyPairGen;
	else if (*this==_L("KCryptoPluginRsaKeyPairGen")) return KCryptoPluginRsaKeyPairGen;
	else if (*this==_L("KCryptoPluginDsaKeyPairGen")) return KCryptoPluginDsaKeyPairGen;
	
	else if (*this==_L("KInterfaceType")) return KInterfaceType;
	else if (*this==_L("KAlgorithmType")) return KAlgorithmType;
	else if (*this==_L("KImplementationType")) return KImplementationType;
	else if (*this==_L("KCreatorNameType")) return KCreatorNameType;
	else if (*this==_L("KIsFIPSApprovedType")) return KIsFIPSApprovedType;
	else if (*this==_L("KIsHardwareSupportedType")) return KIsHardwareSupportedType;
	else if (*this==_L("KMaxConcurrencySupportedType")) return KMaxConcurrencySupportedType;
	else if (*this==_L("KLatencyType")) return KLatencyType;
	else if (*this==_L("KThroughputType")) return KThroughputType;
	else if (*this==_L("KBlockSizeType")) return KBlockSizeType;
	else if (*this==_L("KOutputSizeType")) return KOutputSizeType;
	else if (*this==_L("KSupportedOperationModesType")) return KSupportedOperationModesType;
	else if (*this==_L("KBlockingModeType")) return KBlockingModeType;
	else if (*this==_L("KMaximumKeyLengthType")) return KMaximumKeyLengthType;
	else if (*this==_L("KSupportedPaddingModesType")) return KSupportedPaddingModesType;
	else if (*this==_L("KKeySupportModeType")) return KKeySupportModeType;
	else if (*this==_L("KConcurrencyType")) return KConcurrencyType;
	else if (*this==_L("KExclusiveUseType")) return KExclusiveUseType;
	
	else if (*this==_L("KExtendCharType1")) return KExtendCharType1;
	else if (*this==_L("KExtendCharType2")) return KExtendCharType2;
	else if (*this==_L("KExtendCharType3")) return KExtendCharType3;
	
	//************************** CUSTOM INTEGERS *******************************
	
	else if (*this==_L("EOpEqual")) return EOpEqual;
	else if (*this==_L("EOpNotEqual")) return EOpNotEqual;
	else if (*this==_L("EOpGreaterThan")) return EOpGreaterThan;
	else if (*this==_L("EOpLessThan")) return EOpLessThan;
	else if (*this==_L("EOpGreaterThanOrEqual")) return EOpGreaterThanOrEqual;
	else if (*this==_L("EOpLessThanOrEqual")) return EOpLessThanOrEqual;
	else if (*this==_L("EOpAscendingSort")) return EOpAscendingSort;
	else if (*this==_L("EOpDescendingSort")) return EOpDescendingSort;	
	
	else if (*this==_L("ERuleValueInteger")) return ERuleValueInteger;
	else if (*this==_L("ERuleValueDes8")) return ERuleValueDes8;
	else if (*this==_L("ERuleValueDes16")) return ERuleValueDes16;
	else if (*this==_L("ERuleValueBoolean")) return ERuleValueBoolean;
	
	else if (*this==_L("KMaxTInt")) return KMaxTInt;
	else if (*this==_L("NULL")) return NULL;
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
	else if (*this==_L("KAlgorithmHashMd4")) return KAlgorithmHashMd4;
	else if (*this==_L("KAlgorithmCipherAesXcbcMac96")) return KAlgorithmCipherAesXcbcMac96;
	else if (*this==_L("KAlgorithmCipherAesXcbcPrf128")) return KAlgorithmCipherAesXcbcPrf128;
	else if (*this==_L("KMacInterface")) return KMacInterface;
	else if (*this==_L("KSymmetricCipherMode")) return KSymmetricCipherMode;
	else if (*this==_L("KTestPlugin01Aes")) return KTestPlugin01Aes;
	else if (*this==_L("KTestPlugin01XcbcMac96")) return KTestPlugin01XcbcMac96;
	else if (*this==_L("KTestPlugin01MacMd2_1")) return KTestPlugin01MacMd2_1;
	else if (*this==_L("KTestPlugin01MacMd2_2")) return KTestPlugin01MacMd2_2;
	else if (*this==_L("KTestPlugin02Aes")) return KTestPlugin02Aes;
	else if (*this==_L("KTestPlugin02XcbcMac96")) return KTestPlugin02XcbcMac96;
	else if (*this==_L("KTestPlugin02XcbcPrf128")) return KTestPlugin02XcbcPrf128;
#endif	
	else
		{
		User::Panic(*this,KErrNotFound);
		return 0;	
		}	
	}
	
TVariantPtrC::operator const TInt32*() 
	{
	
	if (*this==_L("KHashOperationModes")) return KHashOperationModes;
	else if (*this==_L("KDesCryptoModes")) return KDesCryptoModes;
	else if (*this==_L("K3DesCryptoModes")) return K3DesCryptoModes;
	else if (*this==_L("KAesCryptoModes")) return KAesCryptoModes;
	else if (*this==_L("KRc2CryptoModes")) return KRc2CryptoModes;
	else if (*this==_L("KArc4CryptoModes")) return KArc4CryptoModes;

	else if (*this==_L("KDesPaddingModes")) return KDesPaddingModes;
	else if (*this==_L("K3DesPaddingModes")) return K3DesPaddingModes;
	else if (*this==_L("KAesPaddingModes")) return KAesPaddingModes;
	else if (*this==_L("KRc2PaddingModes")) return KRc2PaddingModes;
	else if (*this==_L("KArc4PaddingModes")) return KArc4PaddingModes;
	else if (*this==_L("KRSACipherPaddingModes")) return KRSACipherPaddingModes;
	else if (*this==_L("KRSASignerPaddingModes")) return KRSASignerPaddingModes;
	else if (*this==_L("KDSASignerPaddingModes")) return KDSASignerPaddingModes;
	else if (*this==_L("KRSAVerifierPaddingModes")) return KRSAVerifierPaddingModes;
	else if (*this==_L("KDSAVerifierPaddingModes")) return KDSAVerifierPaddingModes;
	
	else
		{
		User::Panic(*this,KErrNotFound);
		return 0;	
		}
	}
	
TVariantPtrC::operator TRomLitC16() 
	{
	
	if (*this==_L("KSymbian")) return KSymbian;	
	else if (*this==_L("KSymbianTestFramework")) return KSymbianTestFramework;
	else if (*this==_L("KTestFramework")) return KTestFramework;
	else if (*this==_L("KAlternativeName")) return KAlternativeName;
	
	else if (*this==_L("KMd2Description")) return KMd2Description;	
	else if (*this==_L("KMd5Description")) return KMd5Description;
	else if (*this==_L("KSha1Description")) return KSha1Description;
	else if (*this==_L("KSha224Description")) return KSha224Description;
	else if (*this==_L("KSha256Description")) return KSha256Description;
	else if (*this==_L("KSha384Description")) return KSha384Description;
	else if (*this==_L("KSha512Description")) return KSha512Description;
	else if (*this==_L("KHmacMd2Description")) return KHmacMd2Description;
	else if (*this==_L("KHmacMd5Description")) return KHmacMd5Description;	
	else if (*this==_L("KHmacSha1Description")) return KHmacSha1Description;
	else if (*this==_L("KHmacSha224Description")) return KHmacSha224Description;
	else if (*this==_L("KHmacSha256Description")) return KHmacSha256Description;
	else if (*this==_L("KHmacSha384Description")) return KHmacSha384Description;
	else if (*this==_L("KHmacSha512Description")) return KHmacSha512Description;
	else if (*this==_L("KRandomDescription")) return KRandomDescription;	
	else if (*this==_L("KDesDescription")) return KDesDescription;
	else if (*this==_L("K3DesDescription")) return K3DesDescription;	
	else if (*this==_L("KAesDescription")) return KAesDescription;
	else if (*this==_L("KRC2Description")) return KRC2Description;
	else if (*this==_L("KARC4Description")) return KARC4Description;	
	else if (*this==_L("KRsaCipherDescription")) return KRsaCipherDescription;
	else if (*this==_L("KRsaSignerDescription")) return KRsaSignerDescription;	
	else if (*this==_L("KDsaSignerDescription")) return KDsaSignerDescription;
	else if (*this==_L("KRsaVerifierDescription")) return KRsaVerifierDescription;	
	else if (*this==_L("KDsaVerifierDescription")) return KDsaVerifierDescription;
	else if (*this==_L("KDHDescription")) return KDHDescription;	
	else if (*this==_L("KSymmetricKeyGeneratorDescription")) return KSymmetricKeyGeneratorDescription;
	else if (*this==_L("KKeyPairDescription")) return KKeyPairDescription;
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
	else if (*this==_L("KAesXcbcMac96Description")) return KAesXcbcMac96Description;
	else if (*this==_L("KAesXcbcPrf128Description")) return KAesXcbcPrf128Description;
#endif
	else
		{
		User::Panic(*this,KErrNotFound);
		return TRomLitC16();	
		}
	}
	
HBufC* TVariantPtrC::HexStringLC()
	{
	HBufC* parsedString = HBufC::NewLC(this->Length()*2);
	TBuf<2> pair;
	_LIT(KFormat2,"%02x"); 
						    
   	for(TInt i = 0; i<this->Length() ; i++)
    	{
		TInt num  = (*this)[i];
    	pair.Format(KFormat2,num);
    	parsedString->Des().Append(pair);
    	}
	    
	return parsedString;
	}

