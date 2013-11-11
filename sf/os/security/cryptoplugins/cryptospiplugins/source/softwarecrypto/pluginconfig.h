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
*
*/


/**
 @file
 @@internalComponent
 @released
*/

#ifndef __CRYPTOAPI_SOFTWAREPLUGINCONFIG_H__
#define __CRYPTOAPI_SOFTWAREPLUGINCONFIG_H__

#include <cryptospi/cryptoparams.h>
#include <e32cmn.h>
#include <cryptospi/romlit.h>
#include <cryptospi/cryptospidef.h>

namespace SoftwareCrypto
	{
	using namespace CryptoSpi;

	/**
	Creator Name
	*/
	_ROMLIT16(KSymbian, "Symbian");
	
	/**
	implementation Name
	*/
	_ROMLIT16(KMd2Description, "Md2");
	_ROMLIT16(KMd4Description, "Md4");
	_ROMLIT16(KMd5Description, "Md5");
	_ROMLIT16(KSha1Description, "Sha1");
	_ROMLIT16(KSha224Description, "Sha224");
	_ROMLIT16(KSha256Description, "Sha256");
	_ROMLIT16(KSha384Description, "Sha384");
	_ROMLIT16(KSha512Description, "Sha512");

	_ROMLIT16(KRandomDescription, "Random");
	_ROMLIT16(KDesDescription, "Des");
	_ROMLIT16(K3DesDescription, "3Des");
	_ROMLIT16(KAesDescription, "Aes");
	_ROMLIT16(KRC2Description, "RC2");
	_ROMLIT16(KARC4Description, "ARC4");
	_ROMLIT16(KRsaCipherDescription, "Rsa");
	_ROMLIT16(KRsaSignerDescription, "RsaSigner");
	_ROMLIT16(KDsaSignerDescription, "DsaSigner");
	_ROMLIT16(KRsaVerifierDescription, "RsaVerifier");
	_ROMLIT16(KDsaVerifierDescription, "DsaVerifier");
	_ROMLIT16(KDHDescription, "DH");
	_ROMLIT16(KSymmetricKeyGeneratorDescription, "Symmetric Key Generator");
	_ROMLIT16(KRsaKeyPairDescription, "Rsa Key Pair Generator");
	_ROMLIT16(KDsaKeyPairDescription, "Dsa Key Pair Generator");
	_ROMLIT16(KDhKeyPairDescription, "DH Key Pair Generator");
	/**
	For each crypto algorithm implemented in this plugin define
		a plugin-wide unique id to identify it
		a UID that contains the above id
		a table of characteristics
	*/	

	
	/**
	Md2 Characteristics
	*/
	static const TInt32 KCryptoPluginMd2 = 0x102831D0;	
	static const TUid KCryptoPluginMd2Uid={KCryptoPluginMd2};
	static const TInt32 KMd2OperationModes[]={KHashMode, KHmacMode};
	static const THashCharacteristics KMd2_1 = {KHashInterface, KAlgorithmHashMd2, KCryptoPluginMd2, &KSymbian, EFalse, EFalse, 6, &KMd2Description, 100, 90, 128, 128, KMd2OperationModes, sizeof(KMd2OperationModes)/sizeof(KMd2OperationModes[0])};
	
	/**
	Md5 Characteristics
	*/
	static const TInt32 KCryptoPluginMd5 = 0x102831D1;	
	static const TUid KCryptoPluginMd5Uid={KCryptoPluginMd5};
	static const TInt32 KMd5OperationModes[]={KHashMode, KHmacMode};
	static const THashCharacteristics KMd5_1 = {KHashInterface, KAlgorithmHashMd5, KCryptoPluginMd5, &KSymbian, EFalse, EFalse, 6, &KMd5Description, 100, 90, 512, 128, KMd5OperationModes, sizeof(KMd5OperationModes)/sizeof(KMd5OperationModes[0])};	

	/**
	Sha1 Characteristics
	*/
	static const TInt32 KCryptoPluginSha1 = 0x102831D2;
	static const TUid KCryptoPluginSha1Uid={KCryptoPluginSha1};
	static const TInt32 KSha1OperationModes[]={KHashMode, KHmacMode};
	static const THashCharacteristics KSha1_1 = {KHashInterface, KAlgorithmHashSha1, KCryptoPluginSha1, &KSymbian, EFalse, EFalse, 6, &KSha1Description, 100, 90, 512, 160, KSha1OperationModes, sizeof(KSha1OperationModes)/sizeof(KSha1OperationModes[0])};	
	
	/**
	Md4 Characteristics
	*/
	static const TInt32 KCryptoPluginMd4 = 0x2000B340;	
	static const TUid KCryptoPluginMd4Uid={KCryptoPluginMd4};
	static const TInt32 KMd4OperationModes[]={KHashMode, KHmacMode};
	static const THashCharacteristics KMd4_1 = {KHashInterface, KAlgorithmHashMd4, KCryptoPluginMd4, &KSymbian, EFalse, EFalse, 6, &KMd4Description, 100, 90, 512, 128, KMd4OperationModes, sizeof(KMd4OperationModes)/sizeof(KMd4OperationModes[0])};	
	
	/**
	Sha224 Characteristics
	*/
	static const TInt32 KCryptoPluginSha224 = 0x2000E25C;
	static const TUid KCryptoPluginSha224Uid={KCryptoPluginSha224};
	static const TInt32 KSha224OperationModes[]={KHashMode, KHmacMode};
	static const THashCharacteristics KSha224_1 = {KHashInterface, KAlgorithmHashSha224, KCryptoPluginSha224, &KSymbian, EFalse, EFalse, 6, &KSha224Description, 100, 90, 512, 224, KSha224OperationModes, sizeof(KSha224OperationModes)/sizeof(KSha224OperationModes[0])};	

	/**
	Sha256 Characteristics
	*/
	static const TInt32 KCryptoPluginSha256 = 0x2000E25D;
	static const TUid KCryptoPluginSha256Uid={KCryptoPluginSha256};
	static const TInt32 KSha256OperationModes[]={KHashMode, KHmacMode};
	static const THashCharacteristics KSha256_1 = {KHashInterface, KAlgorithmHashSha256, KCryptoPluginSha256, &KSymbian, EFalse, EFalse, 6, &KSha256Description, 100, 90, 512, 256, KSha256OperationModes, sizeof(KSha256OperationModes)/sizeof(KSha256OperationModes[0])};	

	/**
	Sha384 Characteristics
	*/
	static const TInt32 KCryptoPluginSha384 = 0x2000E25E;
	static const TUid KCryptoPluginSha384Uid={KCryptoPluginSha384};
	static const TInt32 KSha384OperationModes[]={KHashMode, KHmacMode};
	static const THashCharacteristics KSha384_1 = {KHashInterface, KAlgorithmHashSha384, KCryptoPluginSha384, &KSymbian, EFalse, EFalse, 6, &KSha384Description, 100, 90, 1024, 384, KSha384OperationModes, sizeof(KSha384OperationModes)/sizeof(KSha384OperationModes[0])};	

	/**
	Sha512 Characteristics
	*/
	static const TInt32 KCryptoPluginSha512 = 0x2000E25F;
	static const TUid KCryptoPluginSha512Uid={KCryptoPluginSha512};
	static const TInt32 KSha512OperationModes[]={KHashMode, KHmacMode};
	static const THashCharacteristics KSha512_1 = {KHashInterface, KAlgorithmHashSha512, KCryptoPluginSha512, &KSymbian, EFalse, EFalse, 6, &KSha512Description, 100, 90, 1024, 512, KSha512OperationModes, sizeof(KSha512OperationModes)/sizeof(KSha512OperationModes[0])};	

	
	/**
	Random Characteristics
	*/
	static const TInt32 KCryptoPluginRandom = 0x102831D3;
	static const TUid KCryptoPluginRandomUid={KCryptoPluginRandom};
	static const TRandomCharacteristics KRandom_1 = {KRandomInterface, KAlgorithmRandom, KCryptoPluginRandom, &KSymbian, EFalse, EFalse, 6, &KRandomDescription, 100, 90, 3};	


	
	/**
	DES Characteristics
	*/
	static const TInt32 KCryptoPluginDes = 0x102831D4;
	static const TUid KCryptoPluginDesUid={KCryptoPluginDes};
	static const TInt32 KDesPaddingModes[]={KPaddingModeNone, KPaddingModeSSLv3, KPaddingModePKCS7};
	static const TInt32 KDesCryptoModes[]={KOperationModeNone, KOperationModeECB, KOperationModeCBC, KOperationModeCTR};
	
	static const TSymmetricCipherCharacteristics KDes_1 = {KSymmetricCipherInterface, KAlgorithmCipherDes, KCryptoPluginDes, &KSymbian, EFalse, EFalse, 6, &KDesDescription, 100, 90, 
	64 /* max key bits */, 64 /* block bits */, KDesPaddingModes, sizeof(KDesPaddingModes)/sizeof(KDesPaddingModes[0]), KDesCryptoModes, sizeof(KDesCryptoModes)/sizeof(KDesCryptoModes[0]), 3};

	/**
	3DES-EDE Characteristics
	*/
	static const TInt32 KCryptoPlugin3Des = 0x102831D5;
	static const TUid KCryptoPlugin3DesUid={KCryptoPlugin3Des};
	static const TInt32 K3DesPaddingModes[]={KPaddingModeNone, KPaddingModeSSLv3, KPaddingModePKCS7};
	static const TInt32 K3DesCryptoModes[]={KOperationModeNone, KOperationModeECB, KOperationModeCBC, KOperationModeCTR};
	
	static const TSymmetricCipherCharacteristics K3Des_1 = {KSymmetricCipherInterface, KAlgorithmCipher3Des, KCryptoPlugin3Des, &KSymbian, EFalse, EFalse, 6, &K3DesDescription, 100, 90, 
	192 /* max key bits */, 64 /* block bits */, K3DesPaddingModes, sizeof(K3DesPaddingModes)/sizeof(K3DesPaddingModes[0]), K3DesCryptoModes, sizeof(K3DesCryptoModes)/sizeof(K3DesCryptoModes[0]), 3};

	/**
	AES Characteristics
	*/
	static const TInt32 KCryptoPluginAes = 0x102831D6;
	static const TUid KCryptoPluginAesUid={KCryptoPluginAes};
	static const TInt32 KAesPaddingModes[]={KPaddingModeNone, KPaddingModeSSLv3, KPaddingModePKCS7};
	static const TInt32 KAesCryptoModes[]={KOperationModeNone, KOperationModeECB, KOperationModeCBC, KOperationModeCTR};
	
	static const TSymmetricCipherCharacteristics KAes_1 = {KSymmetricCipherInterface, KAlgorithmCipherAes, KCryptoPluginAes, &KSymbian, EFalse, EFalse, 6, &KAesDescription, 100, 90, 
	256 /* max key bits */, 128 /* block bits */, KAesPaddingModes, sizeof(KAesPaddingModes)/sizeof(KAesPaddingModes[0]), KAesCryptoModes, sizeof(KAesCryptoModes)/sizeof(KAesCryptoModes[0]), 3};


	/**
	RC2 Characteristics
	*/
	static const TInt32 KCryptoPluginRc2 = 0x102831D7;
	static const TUid KCryptoPluginRc2Uid={KCryptoPluginRc2};
	static const TInt32 KRc2PaddingModes[]={KPaddingModeNone, KPaddingModeSSLv3, KPaddingModePKCS7};
	static const TInt32 KRc2CryptoModes[]={KOperationModeNone, KOperationModeECB, KOperationModeCBC, KOperationModeCTR};
	
	static const TSymmetricCipherCharacteristics KRc2_1 = {KSymmetricCipherInterface, KAlgorithmCipherRc2, KCryptoPluginRc2, &KSymbian, EFalse, EFalse, 6, &KRC2Description, 100, 90, 
	1024 /* max key bits */, 64 /* block bits */, KRc2PaddingModes, sizeof(KRc2PaddingModes)/sizeof(KRc2PaddingModes[0]), KRc2CryptoModes, sizeof(KRc2CryptoModes)/sizeof(KRc2CryptoModes[0]), 3};


	/**
	ARC4 Characteristics
	*/
	static const TInt32 KCryptoPluginArc4 = 0x102831D8;
	static const TUid KCryptoPluginArc4Uid={KCryptoPluginArc4};
	static const TInt32 KArc4PaddingModes[]={KPaddingModeNone};
	static const TInt32 KArc4CryptoModes[]={KOperationModeNone};
	
	static const TSymmetricCipherCharacteristics KArc4_1 = {KSymmetricCipherInterface, KAlgorithmCipherArc4, KCryptoPluginArc4, &KSymbian, EFalse, EFalse, 6, &KARC4Description, 100, 90, 
	2048 /* max key bits */, 8 /* block bits */, KArc4PaddingModes, sizeof(KArc4PaddingModes)/sizeof(KArc4PaddingModes[0]), KArc4CryptoModes, sizeof(KArc4CryptoModes)/sizeof(KArc4CryptoModes[0]), 3};


	/**
	RSA Cipher Characteristics
	*/
	static const TInt32 KCryptoPluginRsaCipher = 0x102831D9;
	static const TUid KCryptoPluginRsaCipherUid={KCryptoPluginRsaCipher};
	static const TInt32 KRSACipherPaddingModes[]={KPaddingModeNone, KPaddingModePkcs1_v1_5_Encryption};
	
	static const TAsymmetricCipherCharacteristics KRSA_1 = {KAsymmetricCipherInterface, KAlgorithmCipherRsa, KCryptoPluginRsaCipher, &KSymbian, EFalse, EFalse, 6, &KRsaCipherDescription, 100, 90, 
	256, KRSACipherPaddingModes, sizeof(KRSACipherPaddingModes)/sizeof(KRSACipherPaddingModes[0]), 3};


	/**
	RSA Signer Characteristics
	*/
	static const TInt32 KCryptoPluginRsaSigner = 0x102831DA;
	static const TUid KCryptoPluginRsaSignerUid={KCryptoPluginRsaSigner};
	static const TInt32 KRSASignerPaddingModes[]={KPaddingModeNone, KPaddingModePkcs1_v1_5_Signature};
	
	static const TAsymmetricSignatureCharacteristics KRSASigner_1 = {KSignerInterface, KAlgorithmSignerRsa, KCryptoPluginRsaSigner, &KSymbian, EFalse, EFalse, 6, &KRsaSignerDescription, 100, 90, 
	256, KRSASignerPaddingModes, sizeof(KRSASignerPaddingModes)/sizeof(KRSASignerPaddingModes[0]), 3};

	/**
	DSA Signer Characteristics
	*/
	static const TInt32 KCryptoPluginDsaSigner = 0x102831DB;
	static const TUid KCryptoPluginDsaSignerUid={KCryptoPluginDsaSigner};
	static const TInt32 KDSASignerPaddingModes[]={KPaddingModeNone};
	
	static const TAsymmetricSignatureCharacteristics KDSASigner_1 = {KSignerInterface, KAlgorithmSignerDsa, KCryptoPluginDsaSigner, &KSymbian, EFalse, EFalse, 6, &KDsaSignerDescription, 100, 90, 
	256, KDSASignerPaddingModes, sizeof(KDSASignerPaddingModes)/sizeof(KDSASignerPaddingModes[0]), 3};


	/**
	RSA verifier Characteristics
	*/
	static const TInt32 KCryptoPluginRsaVerifier = 0x102831DC;
	static const TUid KCryptoPluginRsaVerifierUid={KCryptoPluginRsaVerifier};
	static const TInt32 KRSAVerifierPaddingModes[]={KPaddingModeNone, KPaddingModePkcs1_v1_5_Signature};
	
	static const TAsymmetricSignatureCharacteristics KRSAVerifier_1 = {KVerifierInterface, KAlgorithmVerifierRsa, KCryptoPluginRsaVerifier, &KSymbian, EFalse, EFalse, 6, &KRsaVerifierDescription, 100, 90, 
	256, KRSAVerifierPaddingModes, sizeof(KRSAVerifierPaddingModes)/sizeof(KRSAVerifierPaddingModes[0]), 3};

	/**
	DSA verifier Characteristics
	*/
	static const TInt32 KCryptoPluginDsaVerifier = 0x102831DD;
	static const TUid KCryptoPluginDsaVerifierUid={KCryptoPluginDsaVerifier};
	static const TInt32 KDSAVerifierPaddingModes[]={KPaddingModeNone};
	
	static const TAsymmetricSignatureCharacteristics KDSAVerifier_1 = {KVerifierInterface, KAlgorithmVerifierDsa, KCryptoPluginDsaVerifier, &KSymbian, EFalse, EFalse, 6, &KDsaVerifierDescription, 100, 90, 
	256, KDSAVerifierPaddingModes, sizeof(KDSAVerifierPaddingModes)/sizeof(KDSAVerifierPaddingModes[0]), 3};

	/**
	DH Key Agreement Characteristics
	*/
	static const TInt32 KCryptoPluginDhKeyAgreement = 0x102831DE;	// used to identify the CKeyAgreementImpl derived class within the plugin to use
	static const TUid KCryptoPluginDhKeyAgreementUid={KCryptoPluginDhKeyAgreement};	// returned by the CKeyAgreementImpl derived class
	static const TKeyAgreementCharacteristics KDH_1 = {KKeyAgreementInterface, KAlgorithmKeyAgreementDH, KCryptoPluginDhKeyAgreement, &KSymbian, EFalse, EFalse, 6, &KDHDescription, 100, 90};

	/**
	DH Key Pair Generator Characteristics
	*/
	static const TInt32 KCryptoPluginDhKeyPairGen = 0x102831DF;
	static const TUid KCryptoPluginDhKeyPairGenUid={KCryptoPluginDhKeyPairGen};
	static const TAsymmetricKeypairGeneratorCharacteristics KDHKeyPairGenerator_1 = {KKeypairGeneratorInterface, KAlgorithmDHKeyPairGenerator, KCryptoPluginDhKeyPairGen, &KSymbian, EFalse, EFalse, 6, &KDhKeyPairDescription, 100, 90, 1024};

	/**
	RSA Key Pair Generator Characteristics
	*/
	static const TInt32 KCryptoPluginRsaKeyPairGen = 0x102831E0;
	static const TUid KCryptoPluginRsaKeyPairGenUid={KCryptoPluginRsaKeyPairGen};
	static const TAsymmetricKeypairGeneratorCharacteristics KRSAKeyPairGenerator_1 = {KKeypairGeneratorInterface, KAlgorithmRSAKeyPairGenerator, KCryptoPluginRsaKeyPairGen, &KSymbian, EFalse, EFalse, 6, &KRsaKeyPairDescription, 100, 90, 1024};

	/**
	DSA Key Pair Generator Characteristics
	*/
	static const TInt32 KCryptoPluginDsaKeyPairGen = 0x102831E1;
	static const TUid KCryptoPluginDsaKeyPairGenUid={KCryptoPluginDsaKeyPairGen};
	static const TAsymmetricKeypairGeneratorCharacteristics KDSAKeyPairGenerator_1 = {KKeypairGeneratorInterface, KAlgorithmDSAKeyPairGenerator, KCryptoPluginDsaKeyPairGen, &KSymbian, EFalse, EFalse, 6, &KDsaKeyPairDescription, 100, 90, 1024};


/******************************MAC INTERFACE PLUG-INS' CHARACTERISTICS***************************/	
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT	
	_ROMLIT16(KHmacMd2Description, "HmacMd2");
	_ROMLIT16(KHmacMd4Description, "HmacMd4");
	_ROMLIT16(KHmacMd5Description, "HmacMd5");
	_ROMLIT16(KHmacSha1Description, "HmacSha1");
	_ROMLIT16(KHmacSha224Description, "HmacSha224");
	_ROMLIT16(KHmacSha256Description, "HmacSha256");
	_ROMLIT16(KHmacSha384Description, "HmacSha384");
	_ROMLIT16(KHmacSha512Description, "HmacSha512");
	_ROMLIT16(KMacAesXcbcMac96Description, "AesXcbcMac96");
	_ROMLIT16(KMacAesXcbcPrf128Description, "AesXcbcPrf128");
	/**
	 * AES-XCBC-MAC-96 MAC Plugin Characteristics
	 */		            
	static const TInt32 KCryptoPluginMacAesXcbcMac96 = 0x2001EDA8;	
	static const TUid KCryptoPluginMacAesXcbcMac96Uid= {KCryptoPluginMacAesXcbcMac96};
	static const TMacCharacteristics KMac_1 = {
                                               KMacInterface, 
                                               KAlgorithmCipherAesXcbcMac96, 
			                                   KCryptoPluginMacAesXcbcMac96, 
			                                   &KSymbian, 
			                                   EFalse, EFalse, 6, 
			                                   &KMacAesXcbcMac96Description, 
			                                   100, 90, KSymmetricCipherMode,
			                                   NULL,&KAes_1
											   };
	/**
	 * AES-XCBC-PRF-128 MAC Plugin Characteristics
	 */		
	static const TInt32 KCryptoPluginMacAesXcbcPrf128  = 0x2001EDA9;	
	static const TUid KCryptoPluginMacAesXcbcPrf128Uid = {KCryptoPluginMacAesXcbcPrf128};
	static const TMacCharacteristics KMac_2 = {
                                               KMacInterface, 
                                               KAlgorithmCipherAesXcbcPrf128, 
			                                   KCryptoPluginMacAesXcbcPrf128, 
			                                   &KSymbian, 
			                                   EFalse, EFalse, 6, 
			                                   &KMacAesXcbcPrf128Description, 
			                                   100, 90, KSymmetricCipherMode,
			                                   NULL,&KAes_1
											   };
	/**
	 * HASH-MD2 MAC Plugin Characteristics
	 */		
	static const TInt32 KCryptoPluginMacHashMd2  = 0x2001EDA0;	
	static const TUid KCryptoPluginMacHashMd2Uid = {KCryptoPluginMacHashMd2};
	static const TMacCharacteristics KMac_3 = {
                                               KMacInterface, 
										       KAlgorithmHashMd2, 
				                               KCryptoPluginMacHashMd2, 
				                               &KSymbian, 
				                               EFalse, EFalse, 6, 
				                               &KHmacMd2Description, 
				                               100, 90, KHmacMode,
				                               &KMd2_1, NULL 
						                       };
	/**
	 * HASH-MD4 MAC Plugin Characteristics
	 */		
	static const TInt32 KCryptoPluginMacHashMd4  = 0x2001EDA1;	
	static const TUid KCryptoPluginMacHashMd4Uid = {KCryptoPluginMacHashMd4};
	static const TMacCharacteristics KMac_4 = {
                                               KMacInterface, 
										       KAlgorithmHashMd4, 
										       KCryptoPluginMacHashMd4, 
				                               &KSymbian, 
				                               EFalse, EFalse, 6, 
				                               &KHmacMd4Description, 
				                               100, 90, KHmacMode,
				                               &KMd4_1, NULL 
						                       };
	/**
	 * HASH-MD5 MAC Plugin Characteristics
	 */		
	static const TInt32 KCryptoPluginMacHashMd5  = 0x2001EDA2;	
	static const TUid KCryptoPluginMacHashMd5Uid = {KCryptoPluginMacHashMd5};
	static const TMacCharacteristics KMac_5 = {
                                               KMacInterface, 
										       KAlgorithmHashMd5, 
										       KCryptoPluginMacHashMd5, 
				                               &KSymbian, 
				                               EFalse, EFalse, 6, 
				                               &KHmacMd5Description, 
				                               100, 90, KHmacMode,
				                               &KMd5_1, NULL 
						                       };
	/**
	 * HASH-SHA1 MAC Plugin Characteristics
	 */		
	static const TInt32 KCryptoPluginMacHashSha1  = 0x2001EDA3;	
	static const TUid KCryptoPluginMacHashSha1Uid = {KCryptoPluginMacHashSha1};
	static const TMacCharacteristics KMac_6 = {
                                               KMacInterface, 
										       KAlgorithmHashSha1, 
										       KCryptoPluginMacHashSha1, 
				                               &KSymbian, 
				                               EFalse, EFalse, 6, 
				                               &KHmacSha1Description, 
				                               100, 90, KHmacMode,
				                               &KSha1_1, NULL 
						                       };
	/**
	 * HASH-SHA224 MAC Plugin Characteristics
	 */		
	static const TInt32 KCryptoPluginMacHashSha224  = 0x2001EDA4;	
	static const TUid KCryptoPluginMacHashSha224Uid = {KCryptoPluginMacHashSha224};
	static const TMacCharacteristics KMac_7 = {
                                               KMacInterface, 
										       KAlgorithmHashSha224, 
										       KCryptoPluginMacHashSha224, 
				                               &KSymbian, 
				                               EFalse, EFalse, 6, 
				                               &KHmacSha224Description, 
				                               100, 90, KHmacMode,
				                               &KSha224_1, NULL 
						                       };
	/**
	 * HASH-SHA256 MAC Plugin Characteristics
	 */		
	static const TInt32 KCryptoPluginMacHashSha256  = 0x2001EDA5;	
	static const TUid KCryptoPluginMacHashSha256Uid = {KCryptoPluginMacHashSha256};
	static const TMacCharacteristics KMac_8 = {
                                               KMacInterface, 
										       KAlgorithmHashSha256, 
										       KCryptoPluginMacHashSha256, 
				                               &KSymbian, 
				                               EFalse, EFalse, 6, 
				                               &KHmacSha256Description, 
				                               100, 90, KHmacMode,
				                               &KSha256_1, NULL 
						                       };
	/**
	 * HASH-SHA384 MAC Plugin Characteristics
	 */		
	static const TInt32 KCryptoPluginMacHashSha384  = 0x2001EDA6;	
	static const TUid KCryptoPluginMacHashSha384Uid = {KCryptoPluginMacHashSha384};
	static const TMacCharacteristics KMac_9 = {
                                               KMacInterface, 
										       KAlgorithmHashSha384, 
										       KCryptoPluginMacHashSha384, 
				                               &KSymbian, 
				                               EFalse, EFalse, 6, 
				                               &KHmacSha384Description, 
				                               100, 90, KHmacMode,
				                               &KSha384_1, NULL 
						                       };
	/**
	 * HASH-SHA512 MAC Plugin Characteristics
	 */		
	static const TInt32 KCryptoPluginMacHashSha512  = 0x2001EDA7;	
	static const TUid KCryptoPluginMacHashSha512Uid = {KCryptoPluginMacHashSha512};
	static const TMacCharacteristics KMac_10 = {
                                               KMacInterface, 
										       KAlgorithmHashSha512, 
										       KCryptoPluginMacHashSha512, 
				                               &KSymbian, 
				                               EFalse, EFalse, 6, 
				                               &KHmacSha512Description, 
				                               100, 90, KHmacMode,
				                               &KSha512_1, NULL 
						                       };
	/**
	 * Mac implementation characteristic table
	 */
	static const TMacCharacteristics* const KMacCharacteristics[] = {
												&KMac_1, &KMac_2, &KMac_3, 
												&KMac_4, &KMac_5, &KMac_6,
												&KMac_7, &KMac_8, &KMac_9,
												&KMac_10
												};	
	
#endif
/***********************************************************************************************/

	/**
	For each crypto algorithm type implemented in this plugin list the characteristics
	*/
	
	/**
	Hash implementation characteristic table
	*/
	static const THashCharacteristics* const KHashCharacteristics[] = 
		{
		&KMd2_1, &KMd5_1, &KSha1_1, &KMd4_1, &KSha224_1, &KSha256_1, &KSha384_1, &KSha512_1
		};

	/**
	Random implementation characteristic table
	*/
	
	static const TRandomCharacteristics* const KRandomCharacteristics[]=
		{
		&KRandom_1	
		};
		
	/**
	Symmetric Cipher implementation characteristic table
	*/
	static const TSymmetricCipherCharacteristics* const KSymmetricCipherCharacteristics[]=
		{
		&KDes_1, &K3Des_1, &KAes_1, &KRc2_1, &KArc4_1
		};


	/**
	Asymmetric Cipher implementation characteristic table
	*/
	static const TAsymmetricCipherCharacteristics* const KAsymmetricCipherCharacteristics[]=
		{
		&KRSA_1	
		};

	/**
	Signer implementation characteristic table
	*/
	static const TAsymmetricSignatureCharacteristics* const KSignerCharacteristics[]=
		{
		&KRSASigner_1, &KDSASigner_1
		};


	/**
	Verifier implementation characteristic table
	*/
	static const TAsymmetricSignatureCharacteristics* const KVerifierCharacteristics[]=
		{
		&KRSAVerifier_1, &KDSAVerifier_1
		};

	/**
	Key Agreement implementation characteristic table
	*/
	static const TKeyAgreementCharacteristics* const KKeyAgreementCharacteristics[]=
		{
		&KDH_1
		};

	/**
	Key pair generator implementation characteristic table
	*/
	static const TAsymmetricKeypairGeneratorCharacteristics* const KKeyPairGeneratorCharacteristics[]=
		{
		&KDHKeyPairGenerator_1, &KRSAKeyPairGenerator_1, &KDSAKeyPairGenerator_1
		};
	}

#endif //__CRYPTOAPI_SOFTWAREPLUGINCONFIG_H__
