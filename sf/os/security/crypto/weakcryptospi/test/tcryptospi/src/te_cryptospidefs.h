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
* This file define all the common values thoughout your test project
*
*/




/**
 @file
 @internalTechnology
*/
#if (!defined __TE_CRYPTOSPI_SUITEDEFS_H__)
#define __TE_CRYPTOSPI_SUITEDEFS_H__

#include <cryptospi/romlit.h>
#include <cryptospi/cryptospidef.h>

namespace CryptoSpi
	{

	const TInt KKeyExponent = 65537;

	//tplugin01 implementation definitions
	const TInt32 KTestPlugin01Des_1 = 0x102831A1;
	const TInt32 KTestPlugin01Des_2 = 0x102831A2;
	const TInt32 KTestPlugin01Des_3 = 0x102831A3;
	const TInt32 KTestPlugin01Des_4 = 0x102831A4;
	const TInt32 KTestPlugin01Md2_1 = 0x102831A5;
	const TInt32 KTestPlugin01Md2_2 = 0x102831A6;
	const TInt32 KTestPlugin01RsaKeyGen_1 = 0x102831A7;
	const TInt32 KTestPlugin01RsaVerifier_1 = 0x102831A8;
	const TInt32 KTestPlugin01RsaVerifier_2 = 0x102831A9;
	
	//tplugin02 implementation definitions
	const TInt32 KTestPlugin02Random_1 = 0x102831B0;
	const TInt32 KTestPlugin02Md5_1 = 0x102831B1;
	const TInt32 KTestPlugin02Des_1 = 0x102831B2;
	const TInt32 KTestPlugin02Des_2 = 0x102831B3;
	const TInt32 KTestPlugin02Rsa_1 = 0x102831B4;
	const TInt32 KTestPlugin02Rsa_2 = 0x102831B5;
	const TInt32 KTestPlugin02RsaKeyGen_1 = 0x102831B6;
	const TInt32 KTestPlugin02DHKeyGen_1 = 0x102831B7;
	const TInt32 KTestPlugin02RsaSigner_1 = 0x102831B8;
	const TInt32 KTestPlugin02DHKeyAgree_1 = 0x102831B9;
	const TInt32 KTestPlugin02Random_2 = 0x102831BA;
	
	//softwarecrypto implementation definitions
	const TInt32 KCryptoPluginMd2 = 0x102831D0;
	const TInt32 KCryptoPluginMd5 = 0x102831D1;
	const TInt32 KCryptoPluginSha1 = 0x102831D2;
	const TInt32 KCryptoPluginSha224 = 0x2000E25C;
	const TInt32 KCryptoPluginSha256 = 0x2000E25D;
	const TInt32 KCryptoPluginSha384 = 0x2000E25E;
	const TInt32 KCryptoPluginSha512 = 0x2000E25F;
	const TInt32 KCryptoPluginRandom = 0x102831D3;
	const TInt32 KCryptoPluginDes = 0x102831D4;
	const TInt32 KCryptoPlugin3Des = 0x102831D5;
	const TInt32 KCryptoPluginAes = 0x102831D6;
	const TInt32 KCryptoPluginRc2 = 0x102831D7;
	const TInt32 KCryptoPluginArc4 = 0x102831D8;
	const TInt32 KCryptoPluginRsaCipher = 0x102831D9;
	const TInt32 KCryptoPluginRsaSigner = 0x102831DA;
	const TInt32 KCryptoPluginDsaSigner = 0x102831DB;
	const TInt32 KCryptoPluginRsaVerifier = 0x102831DC;
	const TInt32 KCryptoPluginDsaVerifier = 0x102831DD;
	const TInt32 KCryptoPluginDhKeyAgreement = 0x102831DE;
	const TInt32 KCryptoPluginDhKeyPairGen = 0x102831DF;
	const TInt32 KCryptoPluginRsaKeyPairGen = 0x102831E0;
	const TInt32 KCryptoPluginDsaKeyPairGen = 0x102831E1;
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
	const TInt32 KTestPlugin01Aes = 0x102831AA;
	const TInt32 KTestPlugin01XcbcMac96 = 0x102831AB;
	const TInt32 KTestPlugin01MacMd2_1 = 0x102831AC;
	const TInt32 KTestPlugin01MacMd2_2 = 0x102831AD;
	const TInt32 KTestPlugin02Aes = 0x102831BB;
	const TInt32 KTestPlugin02XcbcMac96 = 0x102831BC;
	const TInt32 KTestPlugin02XcbcPrf128 = 0x102831BD;
#endif

	//creator name definitions
	_ROMLIT16(KSymbian, "Symbian");
	_ROMLIT16(KSymbianTestFramework, "SymbianTestFramework");
	_ROMLIT16(KTestFramework, "TestFramework");
	_ROMLIT16(KAlternativeName, "AlternativeName");

	//implementation name definitions
	_ROMLIT16(KMd2Description, "Md2");
	_ROMLIT16(KMd5Description, "Md5");
	_ROMLIT16(KSha1Description, "Sha1");
	_ROMLIT16(KSha224Description, "Sha224");
	_ROMLIT16(KSha256Description, "Sha256");
	_ROMLIT16(KSha384Description, "Sha384");
	_ROMLIT16(KSha512Description, "Sha512");
	_ROMLIT16(KHmacMd2Description, "HmacMd2");
	_ROMLIT16(KHmacMd5Description, "HmacMd5");
	_ROMLIT16(KHmacSha1Description, "HmacSha1");
	_ROMLIT16(KHmacSha224Description, "HmacSha224");
	_ROMLIT16(KHmacSha256Description, "HmacSha256");
	_ROMLIT16(KHmacSha384Description, "HmacSha384");
	_ROMLIT16(KHmacSha512Description, "HmacSha512");
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
	
	//operation mode definitions
	const TInt32 KHashOperationModes[]={KHashMode, KHmacMode};
	const TInt32 KDesCryptoModes[]={KOperationModeNone, KOperationModeECB, KOperationModeCBC};
	const TInt32 K3DesCryptoModes[]={KOperationModeNone, KOperationModeECB, KOperationModeCBC};
	const TInt32 KAesCryptoModes[]={KOperationModeNone, KOperationModeECB, KOperationModeCBC};
	const TInt32 KRc2CryptoModes[]={KOperationModeNone, KOperationModeECB, KOperationModeCBC};
	const TInt32 KArc4CryptoModes[]={KOperationModeNone};

	//padding mode definitions
	const TInt32 KDesPaddingModes[]={KPaddingModeNone, KPaddingModeSSLv3, KPaddingModePKCS7};
	const TInt32 K3DesPaddingModes[]={KPaddingModeNone, KPaddingModeSSLv3, KPaddingModePKCS7};
	const TInt32 KAesPaddingModes[]={KPaddingModeNone, KPaddingModeSSLv3, KPaddingModePKCS7};
	const TInt32 KRc2PaddingModes[]={KPaddingModeNone, KPaddingModeSSLv3, KPaddingModePKCS7};
	const TInt32 KArc4PaddingModes[]={KPaddingModeNone};
	const TInt32 KRSACipherPaddingModes[]={KPaddingModeNone, KPaddingModePkcs1_v1_5_Encryption};
	const TInt32 KRSASignerPaddingModes[]={KPaddingModeNone, KPaddingModePkcs1_v1_5_Signature};
	const TInt32 KDSASignerPaddingModes[]={KPaddingModeNone};
	const TInt32 KRSAVerifierPaddingModes[]={KPaddingModeNone, KPaddingModePkcs1_v1_5_Signature};
	const TInt32 KDSAVerifierPaddingModes[]={KPaddingModeNone};
	
	//Extended Characteristic Uids
	const TInt32 KExtendCharType1 = 0x102ABCD1;
	const TUid KExtendCharType1Uid ={KExtendCharType1};

	const TInt32 KExtendCharType2 = 0x102ABCD2;
	const TUid KExtendCharType2Uid ={KExtendCharType2};

	const TInt32 KExtendCharType3 = 0x102ABCD3;
	const TUid KExtendCharType3Uid ={KExtendCharType3};	

	//***********************************************************************
	//INI Configuration Parameter Definitons
	//***********************************************************************

	//Test Case Initialisation Parameters
	_LIT(KConfigInterfaceType,"InterfaceType");
	_LIT(KConfigEncryptKey,"EncryptKey");
	_LIT(KConfigInvalidKey, "InvalidKey");
	_LIT(KConfigEncryptKeyType,"KeyType");
	_LIT(KConfigTestCaseId,"TestCaseId");
	_LIT(KConfigAlgorithmUid,"AlgorithmUid");
	_LIT(KConfigCryptoMode,"CryptoMode");
	_LIT(KConfigOperationMode,"OperationMode");
	_LIT(KConfigPaddingMode,"PaddingMode");
	_LIT(KConfigAlgorithmParams,"AlgorithmParams");
	_LIT(KConfigSourcePath,"SourcePath");
	_LIT(KConfigEncryptedPath,"EncryptedPath");
	_LIT(KConfigIVPath,"IVPath");
	
	_LIT(KConfigInvalidPaddingMode,"InvalidPaddingMode");
	_LIT(KConfigInvalidOperationMode,"InvalidOperationMode");

	//Source File Parameters
	const TInt KDataReadBlocks = 20;

	//Symmetric/Asymmetric Parameters
	_LIT(KConfigExEncryptedData,"Encrypted");

	//*****ASYMMETRIC ENCRYPT DATA CHECK PARAMS - TEMP*****
	_LIT(KConfigKeyModulous,"Modulous");
	_LIT(KConfigKeyExponent,"Exponent");
	//*****ASYMMETRIC ENCRYPT DATA CHECK PARAMS - TEMP*****

	//Hash/Hmac Specific Parameters
	_LIT(KConfigExHashHmacValue,"ExHashValue");
	_LIT(KConfigExSecondHashHmacValue,"ExSecondHashValue");
	_LIT(KConfigSecondOperationMode,"SecondOperationMode");
	_LIT(KConfigThreadName,"WorkerThreadName");

	//Key Exchange/Generation & Signer/Verifier Parameters
	_LIT(KConfigExchangeTestType,"TestType");
	_LIT(KConfigExchangeKey,"Key");
	_LIT(KConfigExchangePaddingMode,"PaddingMode");
	_LIT(KConfigExchangePaddingMode1,"PaddingMode1");
	_LIT(KConfigExchangePaddingMode2,"PaddingMode2");
	_LIT(KConfigExchangePaddingMode3,"PaddingMode3");
	_LIT(KConfigExchangeKeyTypeVariant, "Keytype");
	_LIT(KConfigSignVerifyType, "Type");
	_LIT(KConfigSignVerifyData, "Data");
	_LIT(KConfigSignVerifySigR, "SignatureR");
	_LIT(KConfigSignVerifySigS, "SignatureS");

	//Common Definitions
	_LIT(KNullString, "NULL");
	_LIT(KEmptyString, "");
	_LIT8(KFileNewLine,"\r\n");

	//Plugin Load
	_LIT(KConfigRuleSelectorToggle, "RuleSelectorToggle");

	//********************CHARACTERISTICS**********************

	//Selection Rule Input Parameters
	_LIT(KConfigInterfaceScopeBase,"InterfaceScopeRule");
	_LIT(KConfigAlgorithmScopeBase,"AlgorithmScopeRule");
	_LIT(KConfigCharUidBase, "CharacterUidRule");
	_LIT(KConfigCompareOperatorBase, "CompareOperatorRule");
	_LIT(KConfigCharValueBase, "CharacterValueRule");
	_LIT(KConfigCharValueTypeBase, "CharacterValueTypeRule");
	_LIT(KConfigIsOptionalBase, "IsOptionaRule");

	const TInt KMaxConfigParamStringSize = 30;	//Max String Size for Dynamic INI file parameters

	//Expected Common Characteristics
	_LIT(KConfigExInterfaceUid, "ExInterfaceUid");
	_LIT(KConfigExAlgorithmUid, "ExAlgorithmUid");
	_LIT(KConfigExImplementationUid, "ExImplementationUid");
	_LIT(KConfigExCreatorName, "ExCreatorName");
	_LIT(KConfigExFIPSApproved, "ExFIPSApproved");
	_LIT(KConfigExHardwareSupport, "ExHardwareSupport");
	_LIT(KConfigExMaxConcurrency, "ExMaxConcurrency");
	_LIT(KConfigExAlgorithmName, "ExAlgorithmName");
	_LIT(KConfigExLatency, "ExLatency");
	_LIT(KConfigExThroughput, "ExThroughput");

	//Expected Cipher Characteristics
	_LIT(KConfigExMaxKeyLength, "ExMaxKeyLength");
	_LIT(KConfigExKeySupportMode, "ExKeySupportMode");
	_LIT(KConfigExPaddingModes, "ExPaddingModes");
	_LIT(KConfigExPaddingModeNum, "ExPaddingModeNum");
	_LIT(KConfigExBlockSize, "ExBlockSize");
	_LIT(KConfigExOperationModes, "ExOperationModes");  
	_LIT(KConfigExOperationModeNum, "ExOperationModeNum"); 

	//Expected Hash/Hmac Specific Characteristics
	_LIT(KConfigExOutputSize, "ExOutputSize");
		
	//Expected Random Specific Characteristics
	_LIT(KConfigExBlockingMode, "ExBlockingMode"); 

	//Expected Extended Characteristics
	_LIT(KConfigExExtendConcurrency, "ExExtendConcurrency");
	_LIT(KConfigExExtendExclusiveUse, "ExExtendExclusiveUse");
	_LIT(KConfigExExtendTestChar1, "ExExtendTestChar1");
	_LIT(KConfigExExtendTestChar2, "ExExtendTestChar2");
	_LIT(KConfigExExtendTestChar3, "ExExtendTestChar3");

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
	_LIT(KConfigEncryptKeyPath,"EncryptKeyPath");

	//Mac Specific Parameters
	_LIT(KConfigExMacValue,"ExMacValue");

	//Expected Mac Specific Characteristics
	_LIT(KConfigExMacMode, "ExMacMode");
#endif
	
	enum TReturnValueType
		{
		ERuleValueInteger,
		ERuleValueDes8,
		ERuleValueDes16,
		ERuleValueBoolean		
		};
			
	// For test step panics
	_LIT(KTe_CryptoSpiPanic,"Te_CryptoSpi");
	
	}
#endif
