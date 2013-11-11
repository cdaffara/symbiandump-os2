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
#include "cryptospi/cryptospidef.h"
#include "pluginconfigcommon.h"

namespace SoftwareCrypto
	{
	using namespace CryptoSpi;
	using namespace PluginCommon;

	/**
	Creator Name
	*/
	_ROMLIT16(KTestFramework, "TestFramework");
	
	/**
	Implemetation Name
	*/
	_ROMLIT16(KMd2Description, "Md2");
	_ROMLIT16(KMd5Description, "Md5");
	_ROMLIT16(KSha1Description, "Sha1");
	_ROMLIT16(KHmacMd2Description, "HmacMd2");
	_ROMLIT16(KHmacMd5Description, "HmacMd5");
	_ROMLIT16(KHmacSha1Description, "HmacSha1");
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
	_ROMLIT16(KKeyPairDescription, "Key Pair Generator");

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
	_ROMLIT16(KAesXcbcMac96Description, "AesXcbcMac96");
	_ROMLIT16(KAesXcbcPrf128Description, "AesXcbcPrf128");
#endif

	/**
	Common Mode Arrays
	*/
	static const TInt32 KDesPaddingModes[]={KPaddingModeNone, KPaddingModeSSLv3, KPaddingModePKCS7};
	static const TInt32 KDesCryptoModes[]={KOperationModeNone, KOperationModeECB, KOperationModeCBC};
	static const TInt32 KHashOperationModes[]={KHashMode, KHmacMode};
	static const TInt32 KRSACipherPaddingModes[]={KPaddingModeNone, KPaddingModePkcs1_v1_5_Encryption};
	static const TInt32 KRSASignerPaddingModes[]={KPaddingModeNone, KPaddingModePkcs1_v1_5_Signature};

	/**
	Test Plugin 02 - Random 1
	*/
	static const TInt32 KTestPlugin02Random_1 = 0x102831B0;
	static const TUid KTestPlugin02Random_1Uid = {KTestPlugin02Random_1};
	static const TRandomCharacteristics KRandom_1 = 
		{
		KRandomInterface, 
		KAlgorithmRandom, 
		KTestPlugin02Random_1, 
		&KSymbianTestFramework, 
		EFalse, 
		EFalse, 
		6, 
		&KRandomDescription, 
		100, 
		90, 
		3
		};	

	/**
	Test Plugin 02 - Hash MD5 1
	*/
	static const TInt32 KTestPlugin02Md5_1 = 0x102831B1;
	static const TUid KTestPlugin02Md5_1Uid = {KTestPlugin02Md5_1};
	static const THashCharacteristics KMd5_1 = 
		{
		KHashInterface, 
		KAlgorithmHashMd5, 
		KTestPlugin02Md5_1, 
		&KTestFramework, 
		EFalse, 
		EFalse, 
		6, 
		&KHmacMd5Description, 
		100, 
		1024, 
		512, 
		128,
		KHashOperationModes, sizeof(KHashOperationModes)/sizeof(TInt32)
		};

	/**
	Test Plugin 02 - Symmetric Cipher DES 1
	*/
	
	static const TInt32 KTestPlugin02Des_1 = 0x102831B2;
	static const TUid KTestPlugin02Des_1Uid = {KTestPlugin02Des_1};
	static const TSymmetricCipherCharacteristics KDes_1 = 
		{
		KSymmetricCipherInterface, 
		KAlgorithmCipherDes, 
		KTestPlugin02Des_1, 
		&KSymbianTestFramework, 
		EFalse, 
		EFalse, 
		6, 
		&KDesDescription, 
		100, 
		90, 
		64, 
		64, 
		KDesPaddingModes, sizeof(KDesPaddingModes)/sizeof(TInt32), 
		KDesCryptoModes, sizeof(KDesCryptoModes)/sizeof(TInt32),
		4
		};	

	/**
	Test Plugin 02 - Symmetric Cipher DES 2
	*/
	static const TInt32 KTestPlugin02Des_2 = 0x102831B3;
	static const TUid KTestPlugin02Des_2Uid = {KTestPlugin02Des_2};
	static const TSymmetricCipherCharacteristics KDes_2 = 
		{
		KSymmetricCipherInterface, 
		KAlgorithmCipherDes, 
		KTestPlugin02Des_2, 
		&KSymbianTestFramework, 
		EFalse, 
		EFalse, 
		6, 
		&KDesDescription, 
		101, 
		90, 
		64, 
		32, 
		KDesPaddingModes, sizeof(KDesPaddingModes)/sizeof(TInt32), 
		KDesCryptoModes, sizeof(KDesCryptoModes)/sizeof(TInt32),
		3
		};	

	/**
	Test Plugin 02 - Asymmetric Cipher RSA 1
	*/
	static const TInt32 KTestPlugin02Rsa_1 = 0x102831B4;
	static const TUid KTestPlugin02Rsa_1Uid = {KTestPlugin02Rsa_1};
	static const TAsymmetricCipherCharacteristics KRSA_1 = 
		{
		KAsymmetricCipherInterface, 
		KAlgorithmCipherRsa, 
		KTestPlugin02Rsa_1, 
		&KSymbianTestFramework, 
		EFalse, 
		EFalse, 
		6, 
		&KRsaCipherDescription, 
		100, 
		90, 
		256,
		KRSACipherPaddingModes, sizeof(KRSACipherPaddingModes)/sizeof(TInt32),
		3
		};	

	/**
	Test Plugin 02 - Asymmetric Cipher RSA 2
	*/
	static const TInt32 KTestPlugin02Rsa_2 = 0x102831B5;
	static const TUid KTestPlugin02Rsa_2Uid = {KTestPlugin02Rsa_2};
	static const TAsymmetricCipherCharacteristics KRSA_2 = 
		{
		KAsymmetricCipherInterface, 
		KAlgorithmCipherRsa, 
		KTestPlugin02Rsa_2, 
		&KSymbianTestFramework, 
		EFalse, 
		EFalse, 
		6, 
		&KRsaCipherDescription, 
		100, 
		90, 
		512,
		KRSACipherPaddingModes, sizeof(KRSACipherPaddingModes)/sizeof(TInt32),
		2
		};
		
	/**
	Test Plugin 02 - RSA Key Generator 1
	*/
	static const TInt32 KTestPlugin02RsaKeyGen_1 = 0x102831B6;
	static const TUid KTestPlugin02RsaKeyGen_1Uid={KTestPlugin02RsaKeyGen_1};
	static const TAsymmetricKeypairGeneratorCharacteristics KRSAKeyPairGenerator_1 = 
		{
		KKeypairGeneratorInterface, 
		KAlgorithmRSAKeyPairGenerator, 
		KTestPlugin02RsaKeyGen_1, 
		&KSymbianTestFramework, 
		EFalse, 
		EFalse, 
		6, 
		&KKeyPairDescription, 
		100, 
		90, 
		1024
		};
		
	/**
	Test Plugin 02 - DH Key Pair Generator
	*/
	static const TInt32 KTestPlugin02DHKeyGen_1 = 0x102831B7;
	static const TUid KTestPlugin02DHKeyGen_1Uid={KTestPlugin02DHKeyGen_1};
	static const TAsymmetricKeypairGeneratorCharacteristics KDHKeyPairGenerator_1 = 
		{
		KKeypairGeneratorInterface, 
		KAlgorithmDHKeyPairGenerator, 
		KTestPlugin02DHKeyGen_1, 
		&KSymbianTestFramework, 
		EFalse, 
		EFalse, 
		6, 
		&KKeyPairDescription, 
		100, 
		90, 
		1024
		};
		
	/**
	Test Plugin 02 - RSA Signer 1
	*/
	static const TInt32 KTestPlugin02RsaSigner_1 = 0x102831B8;
	static const TUid KTestPlugin02RsaSigner_1Uid={KTestPlugin02RsaSigner_1};
	static const TAsymmetricSignatureCharacteristics KRSASigner_1 = 
		{
		KSignerInterface, 
		KAlgorithmSignerRsa, 
		KTestPlugin02RsaSigner_1, 
		&KSymbianTestFramework, 
		EFalse, 
		EFalse, 
		6, 
		&KRsaSignerDescription, 
		100, 
		90, 
		256, 
		KRSASignerPaddingModes, sizeof(KRSASignerPaddingModes)/sizeof(TInt32),
		3
		};
	
	/**
	Test Plugin 02 - DH Key Agreement 1
	*/
	static const TInt32 KTestPlugin02DHKeyAgree_1 = 0x102831B9;	
	static const TUid KTestPlugin02DHKeyAgree_1Uid={KTestPlugin02DHKeyAgree_1};
	static const TKeyAgreementCharacteristics KDH_1 = 
		{
		KKeyAgreementInterface, 
		KAlgorithmKeyAgreementDH, 
		KTestPlugin02DHKeyAgree_1, 
		&KSymbianTestFramework, 
		EFalse, 
		EFalse, 
		6, 
		&KDHDescription, 
		100, 
		90
		};
		
	/**
	Test Plugin 02 - Random 2
	*/
	static const TInt32 KTestPlugin02Random_2 = 0x102831BA;
	static const TUid KTestPlugin02Random_2Uid = {KTestPlugin02Random_2};
	static const TRandomCharacteristics KRandom_2 = 
		{
		KRandomInterface, 
		KAlgorithmRandom, 
		KTestPlugin02Random_2, 
		&KSymbianTestFramework, 
		EFalse, 
		EFalse, 
		6, 
		&KRandomDescription, 
		100, 
		90, 
		2
		};
 
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
	/**
	Test Plugin 02 - AES
	*/
	static const TInt32 KTestPlugin02Aes = 0x102831BB;
	static const TUid KTestPlugin02AesUid={KTestPlugin02Aes};
	static const TInt32 KTestPlugin02AesPaddingModes[]={KPaddingModeNone, KPaddingModeSSLv3, KPaddingModePKCS7};
	static const TInt32 KTestPlugin02AesCryptoModes[]={KOperationModeNone, KOperationModeECB, KOperationModeCBC, KOperationModeCTR};
	
	static const TSymmetricCipherCharacteristics KAes_1 = 
		{
		KSymmetricCipherInterface,
		KAlgorithmCipherAes,
		KTestPlugin02Aes,
		&KSymbianTestFramework,
		EFalse,
		EFalse,
		6,
		&KAesDescription,
		100,
		90, 
		256 /* max key bits */,
		128 /* block bits */,
		KTestPlugin02AesPaddingModes,
		sizeof(KTestPlugin02AesPaddingModes)/sizeof(KTestPlugin02AesPaddingModes[0]),
		KTestPlugin02AesCryptoModes, sizeof(KTestPlugin02AesCryptoModes)/sizeof(KTestPlugin02AesCryptoModes[0]),
		3
		};


	/**
	Test Plugin 02 - AesXcbcMac96
	*/
	static const TInt32 KTestPlugin02XcbcMac96 = 0x102831BC;
	static const TUid KTestPlugin02XcbcMac96Uid = {KTestPlugin02XcbcMac96};	
	static const TMacCharacteristics KXcbcMac96Char = 
		{
		KMacInterface, 
		KAlgorithmCipherAesXcbcMac96, 
		KTestPlugin02XcbcMac96, 
		&KSymbianTestFramework, 
		EFalse, 
		EFalse, 
		6, 
		&KAesXcbcMac96Description, 
		20, 
		200, 
		KSymmetricCipherMode, 
		0,
		&KAes_1
		};

	/**
	Test Plugin 02 - AesXcbcPrf128
	*/
	static const TInt32 KTestPlugin02XcbcPrf128 = 0x102831BD;
	static const TUid KTestPlugin02XcbcMacPrf128Uid = {KTestPlugin02XcbcPrf128};	
	static const TMacCharacteristics KXcbcPrf128Char = 
		{
		KMacInterface, 
		KAlgorithmCipherAesXcbcPrf128, 
		KTestPlugin02XcbcPrf128, 
		&KSymbianTestFramework, 
		EFalse, 
		EFalse, 
		5, 
		&KAesXcbcPrf128Description, 
		20, 
		90, 
		KSymmetricCipherMode, 
		0,
		&KAes_1
		};
#endif

	/**
	Random implemetation characteristic table
	*/
	static const TRandomCharacteristics* const KRandomCharacteristics[]=
		{
		&KRandom_1, &KRandom_2	
		};	

	/**
	Hash implemetation characteristic table
	*/
	static const THashCharacteristics* const KHashCharacteristics[]=
		{
		&KMd5_1	
		};

	/**
	Symmetric Cipher implemetation characteristic table
	*/
	static const TSymmetricCipherCharacteristics* const KSymmetricCipherCharacteristics[]=
		{
		&KDes_1, &KDes_2
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
, &KAes_1
#endif
		};	
	
	/**
	Asymmetric Cipher implemetation characteristic table
	*/
	static const TAsymmetricCipherCharacteristics* const KAsymmetricCipherCharacteristics[]=
		{
		&KRSA_1, &KRSA_2	
		};
		
	/**
	Key pair generator implementation characteristic table
	*/
	static const TAsymmetricKeypairGeneratorCharacteristics* const KKeyPairGeneratorCharacteristics[]=
		{
		&KRSAKeyPairGenerator_1, &KDHKeyPairGenerator_1
		};
		
	/**
	Signer implementation characteristic table
	*/
	static const TAsymmetricSignatureCharacteristics* const KSignerCharacteristics[]=
		{
		&KRSASigner_1
		};
			
	/**
	Key Agreement implementation characteristic table
	*/
	static const TKeyAgreementCharacteristics* const KKeyAgreementCharacteristics[]=
		{
		&KDH_1
		};
	
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
	static const TMacCharacteristics* const KMacCharacteristics[]=
		{
		&KXcbcMac96Char, &KXcbcPrf128Char	
		};	
#endif
	}

#endif //__CRYPTOAPI_SOFTWAREPLUGINCONFIG_H__
