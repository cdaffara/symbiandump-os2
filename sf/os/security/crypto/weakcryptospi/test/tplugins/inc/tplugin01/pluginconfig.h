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
	_ROMLIT16(KAlternativeName, "AlternativeName");
	
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
#endif
	
	/**
	Common Mode Arrays
	*/
	static const TInt32 KDesPaddingModes[]={KPaddingModeNone, KPaddingModeSSLv3, KPaddingModePKCS7};
	static const TInt32 KRSAVerifierPaddingModes[]={KPaddingModeNone, KPaddingModePkcs1_v1_5_Signature};
	static const TInt32 KDesCryptoModes[]={KOperationModeNone, KOperationModeECB, KOperationModeCBC};
	static const TInt32 KHashOperationModes[]={KHashMode, KHmacMode};
	
	
	/**
	Test Plugin 01 - Symmetric Cipher DES 1
	*/
	static const TInt32 KTestPlugin01Des_1 = 0x102831A1;
	static const TUid KTestPlugin01Des_1Uid = {KTestPlugin01Des_1};	
	static const TSymmetricCipherCharacteristics KDes_1 = 
		{
		KSymmetricCipherInterface, 
		KAlgorithmCipherDes, 
		KTestPlugin01Des_1, 
		&KSymbianTestFramework, 
		EFalse, 
		EFalse, 
		6, 
		&KDesDescription, 
		100, 
		90, 
		32, 
		64, 
		KDesPaddingModes, sizeof(KDesPaddingModes)/sizeof(TInt32), 
		KDesCryptoModes, sizeof(KDesCryptoModes)/sizeof(TInt32),
		3
		};	

	/**
Test Plugin 01 - Symmetric Cipher DES 2
	*/
	static const TInt32 KTestPlugin01Des_2 = 0x102831A2;
	static const TUid KTestPlugin01Des_2Uid = {KTestPlugin01Des_2};
	static const TSymmetricCipherCharacteristics KDes_2 = 
		{
		KSymmetricCipherInterface, 
		KAlgorithmCipherDes, 
		KTestPlugin01Des_2, 
		&KSymbianTestFramework, 
		EFalse, 
		ETrue, 
		5, 
		&KDesDescription, 
		100, 
		200, 
		64, 
		64, 
		KDesPaddingModes, sizeof(KDesPaddingModes)/sizeof(TInt32), 
		KDesCryptoModes, sizeof(KDesCryptoModes)/sizeof(TInt32),
		3
		};	

	/**
	Test Plugin 01 - Symmetric Cipher DES 3
	*/
	static const TInt32 KTestPlugin01Des_3 = 0x102831A3;
	static const TUid KTestPlugin01Des_3Uid = {KTestPlugin01Des_3};
	static const TSymmetricCipherCharacteristics KDes_3 = 
		{
		KSymmetricCipherInterface, 
		KAlgorithmCipherDes, 
		KTestPlugin01Des_3, 
		&KSymbianTestFramework, 
		ETrue, 
		EFalse, 
		7, 
		&KDesDescription, 
		100, 
		90, 
		128, 
		128, 
		KDesPaddingModes, sizeof(KDesPaddingModes)/sizeof(TInt32), 
		KDesCryptoModes, sizeof(KDesCryptoModes)/sizeof(TInt32),
		3
		};	

	/**
	Test Plugin 01 - Symmetric Cipher DES 4
	*/
	static const TInt32 KTestPlugin01Des_4 = 0x102831A4;
	static const TUid KTestPlugin01Des_4Uid = {KTestPlugin01Des_4};
	static const TSymmetricCipherCharacteristics KDes_4 = 
		{
		KSymmetricCipherInterface, 
		KAlgorithmCipherDes, 
		KTestPlugin01Des_4, 
		&KSymbianTestFramework, 
		EFalse, 
		EFalse, 
		6, 
		&KDesDescription, 
		50, 
		90, 
		64, 
		64, 
		KDesPaddingModes, sizeof(KDesPaddingModes)/sizeof(TInt32), 
		KDesCryptoModes, sizeof(KDesCryptoModes)/sizeof(TInt32),
		2
		};	

	/**
	Test Plugin 01 - Hash MD2 1
	*/
	static const TInt32 KTestPlugin01Md2_1 = 0x102831A5;
	static const TUid KTestPlugin01Md2_1Uid = {KTestPlugin01Md2_1};
	static const THashCharacteristics KMd2_1 = 
		{
		KHashInterface, 
		KAlgorithmHashMd2, 
		KTestPlugin01Md2_1, 
		&KSymbianTestFramework, 
		EFalse, 
		EFalse, 
		6, 
		&KMd2Description, 
		100, 
		85, 
		192, 
		64,
		KHashOperationModes, sizeof(KHashOperationModes)/sizeof(TInt32)
		};

	/**
	Test Plugin 01 - Hash MD2 2
	*/
	static const TInt32 KTestPlugin01Md2_2 = 0x102831A6;
	static const TUid KTestPlugin01Md2_2Uid = {KTestPlugin01Md2_2};	
	static const THashCharacteristics KMd2_2 = 
		{
		KHashInterface, 
		KAlgorithmHashMd2, 
		KTestPlugin01Md2_2, 
		&KAlternativeName, 
		EFalse, 
		EFalse, 
		6, 
		&KMd2Description, 
		100, 
		90, 
		256, 
		256,
		KHashOperationModes, sizeof(KHashOperationModes)/sizeof(TInt32)
		};
		
	/**
	Test Plugin 01 - RSA Key Generator 1
	*/
	static const TInt32 KTestPlugin01RsaKeyGen_1 = 0x102831A7;
	static const TUid KTestPlugin01RsaKeyGen_1Uid={KTestPlugin01RsaKeyGen_1};
	static const TAsymmetricKeypairGeneratorCharacteristics KRSAKeyPairGenerator_1 = 
		{
		KKeypairGeneratorInterface, 
		KAlgorithmRSAKeyPairGenerator, 
		KTestPlugin01RsaKeyGen_1, 
		&KSymbianTestFramework, 
		EFalse, 
		EFalse, 
		6, 
		&KKeyPairDescription, 
		99, 
		90, 
		2048
		};
		
	/**
	Test Plugin 01 - RSA verifier 1
	*/
	static const TInt32 KTestPlugin01RsaVerifier_1 = 0x102831A8;
	static const TUid KTestPlugin01RsaVerifier_1Uid={KTestPlugin01RsaVerifier_1};
	static const TAsymmetricSignatureCharacteristics KRSAVerifier_1 = 
		{
		KVerifierInterface, 
		KAlgorithmVerifierRsa, 
		KTestPlugin01RsaVerifier_1, 
		&KSymbianTestFramework, 
		EFalse, 
		EFalse, 
		6, 
		&KRsaVerifierDescription, 
		100, 
		90, 
		256, 
		KRSAVerifierPaddingModes, sizeof(KRSAVerifierPaddingModes)/sizeof(KRSAVerifierPaddingModes[0]), 
		2};
		
	/**
	Test Plugin 01 - RSA verifier 2
	*/
	static const TInt32 KTestPlugin01RsaVerifier_2 = 0x102831A9;
	static const TUid KTestPlugin01RsaVerifier_2Uid={KTestPlugin01RsaVerifier_2};
	static const TAsymmetricSignatureCharacteristics KRSAVerifier_2 = 
		{
		KVerifierInterface, 
		KAlgorithmVerifierRsa, 
		KTestPlugin01RsaVerifier_2, 
		&KSymbianTestFramework, 
		EFalse, 
		EFalse, 
		6, 
		&KRsaVerifierDescription, 
		100, 
		90, 
		192, 
		KRSAVerifierPaddingModes, sizeof(KRSAVerifierPaddingModes)/sizeof(KRSAVerifierPaddingModes[0]), 
		3};

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
	/**
	Test Plugin 01 - AES
	*/
	static const TInt32 KTestPlugin01Aes = 0x102831AA;
	static const TUid KTestPlugin01AesUid={KTestPlugin01Aes};
	static const TInt32 KTestPlugin01AesPaddingModes[]={KPaddingModeNone, KPaddingModeSSLv3, KPaddingModePKCS7};
	static const TInt32 KTestPlugin01AesCryptoModes[]={KOperationModeNone, KOperationModeECB, KOperationModeCBC, KOperationModeCTR};
	
	static const TSymmetricCipherCharacteristics KAes_1 = 
		{
		KSymmetricCipherInterface,
		KAlgorithmCipherAes,
		KTestPlugin01Aes,
		&KAlternativeName,
		EFalse,
		EFalse,
		6,
		&KAesDescription,
		100,
		90, 
		256 /* max key bits */,
		128 /* block bits */,
		KTestPlugin01AesPaddingModes,
		sizeof(KTestPlugin01AesPaddingModes)/sizeof(KTestPlugin01AesPaddingModes[0]),
		KTestPlugin01AesCryptoModes, sizeof(KTestPlugin01AesCryptoModes)/sizeof(KTestPlugin01AesCryptoModes[0]),
		3
		};

	/**
	Test Plugin 01 - AesXcbcMac96
	*/
	static const TInt32 KTestPlugin01XcbcMac96 = 0x102831AB;
	static const TUid KTestPlugin01XcbcMac96Uid = {KTestPlugin01XcbcMac96};	
	static const TMacCharacteristics KXcbcMac96Char = 
		{
		KMacInterface, 
		KAlgorithmCipherAesXcbcMac96, 
		KTestPlugin01XcbcMac96, 
		&KAlternativeName, 
		EFalse, 
		EFalse, 
		6, 
		&KAesXcbcMac96Description, 
		100, 
		90, 
		KSymmetricCipherMode, 
		0,
		&KAes_1
		};
	
	/**
	Test Plugin 01 - Mac MD2 1
	*/
	static const TInt32 KTestPlugin01MacMd2_1 = 0x102831AC;
	static const TUid KTestPlugin01MacMd2_1Uid = {KTestPlugin01MacMd2_1};	
	static const TMacCharacteristics KMacMd2_1 = 
		{
		KMacInterface, 
		KAlgorithmHashMd2, 
		KTestPlugin01MacMd2_1, 
		&KAlternativeName, 
		EFalse, 
		EFalse, 
		2, 
		&KHmacMd2Description, 
		20, 
		70, 
		KHmacMode, 
		&KMd2_1,
		0
		};
	
	/**
	Test Plugin 01 - Mac MD2 2
	*/
	static const TInt32 KTestPlugin01MacMd2_2 = 0x102831AD;
	static const TUid KTestPlugin01MacMd2_2Uid = {KTestPlugin01MacMd2_2};	
	static const TMacCharacteristics KMacMd2_2 = 
		{
		KMacInterface, 
		KAlgorithmHashMd2, 
		KTestPlugin01MacMd2_2, 
		&KAlternativeName, 
		EFalse, 
		EFalse, 
		6, 
		&KHmacMd2Description, 
		100, 
		90, 
		KHmacMode, 
		&KMd2_2,
		0
		};
#endif
	
	/**
	Symmetric Cipher implemetation characteristic table
	*/
	static const TSymmetricCipherCharacteristics* const KSymmetricCipherCharacteristics[]=
		{
		&KDes_1, &KDes_2, &KDes_3, &KDes_4
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
, &KAes_1
#endif
		};

	/**
	Hash implemetation characteristic table
	*/
	static const THashCharacteristics* const KHashCharacteristics[] = 
		{
		&KMd2_1, &KMd2_2
		};
		
	/**
	Key pair generator implementation characteristic table
	*/
	static const TAsymmetricKeypairGeneratorCharacteristics* const KKeyPairGeneratorCharacteristics[]=
		{
		&KRSAKeyPairGenerator_1
		};
		
	/**
	Verifier implementation characteristic table
	*/
	static const TAsymmetricSignatureCharacteristics* const KVerifierCharacteristics[]=
		{
		&KRSAVerifier_1, &KRSAVerifier_2
		};

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
	static const TMacCharacteristics* const KMacCharacteristics[]=
		{
		&KXcbcMac96Char, &KMacMd2_1, &KMacMd2_2
		};
#endif
	}

#endif //__CRYPTOAPI_SOFTWAREPLUGINCONFIG_H__
