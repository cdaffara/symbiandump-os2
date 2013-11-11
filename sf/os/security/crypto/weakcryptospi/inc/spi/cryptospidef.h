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
* crypto spi data definition
*
*/


/**
 @file
 @publishedPartner
 @released
*/
 
#ifndef __CRYPTOAPI_CRYPTOSPIDEF_H__
#define __CRYPTOAPI_CRYPTOSPIDEF_H__

#include <e32base.h>

namespace CryptoSpi
	{
	/**
	Definition of Cryptography interface UID value
	*/
	const TInt32 KHashInterface = 0x10283161;
	const TInt32 KRandomInterface = 0x10283162;
	const TInt32 KSymmetricCipherInterface = 0x10283163;
	const TInt32 KAsymmetricCipherInterface = 0x10283164;
	const TInt32 KSignerInterface = 0x10283165;
	const TInt32 KVerifierInterface = 0x10283166;
	const TInt32 KKeyAgreementInterface = 0x10283167;
	const TInt32 KKeyGeneratorInterface = 0x10283168;
	const TInt32 KKeypairGeneratorInterface = 0x10283169;
	const TInt32 KAsyncHashInterface = 0x1028316a;
	const TInt32 KAsyncRandomInterface = 0x1028316b;
	const TInt32 KAsyncSymmetricCipherInterface = 0x1028316c;
	const TInt32 KAsyncAsymmetricCipherInterface = 0x1028316d;
	const TInt32 KAsyncSignerInterface = 0x1028316e;
	const TInt32 KAsyncVerifierInterface = 0x1028316f;
	const TInt32 KAsyncKeyAgreementInterface = 0x10283170;
	const TInt32 KAsyncKeyGeneratorInterface = 0x10283171;
	const TInt32 KAsyncKeypairGeneratorInterface = 0x10283172;


/******************************MAC INTERFACE UIDS**********************************/

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
	/**
	 * Mac Interface Uid
	 */
	const TInt32 KMacInterface = 0x2001ED9A;
	/**
	 * Asynchronous Mac Interface Uid
	 */
	const TInt32 KAsyncMacInterface = 0x2001ED9B;
	/**
	 * Definition of Mac mode Uid value
	 */
	const TInt32 KSymmetricCipherMode = 0x2001ED9C;
	/**
	 * Definition of Mac algorithm UID value
	 */
	const TInt32 KAlgorithmCipherAesXcbcMac96 = 0x2001ED9D; 
	const TInt32 KAlgorithmCipherAesXcbcPrf128 = 0x2001ED9E;
	/**
     * Mac algorithm Uids
	 */	
	const TUid KMacInterfaceUid={KMacInterface};
	const TUid KAsyncMacInterfaceUid={KAsyncMacInterface};
	const TUid KSymmetricCipherModeUid = {KSymmetricCipherMode};
	const TUid KAesXcbcMac96Uid = {KAlgorithmCipherAesXcbcMac96};
	const TUid KAesXcbcPrf128Uid = {KAlgorithmCipherAesXcbcPrf128};
	
#endif
	
/********************************************************************************/
	
	
	/**
	Hash Interface Uid
	*/
	const TUid KHashInterfaceUid={KHashInterface};

	
	
	/**
	Random Interface Uid
	*/
	const TUid KRandomInterfaceUid={KRandomInterface};

	/**
	Symmetric Cipher Interface Uid
	*/
	const TUid KSymmetricCipherInterfaceUid={KSymmetricCipherInterface};

	/**
	Asymmetric Cipher Interface Uid
	*/
	const TUid KAsymmetricCipherInterfaceUid={KAsymmetricCipherInterface};

	/**
	Signer Interface Uid
	*/
	const TUid KSignerInterfaceUid={KSignerInterface};

	/**
	Verifier Interface Uid
	*/
	const TUid KVerifierInterfaceUid={KVerifierInterface};

	/**
	Key Agreement Interface Uid
	*/
	const TUid KKeyAgreementInterfaceUid={KKeyAgreementInterface};

	/**
	Key generator Interface Uid
	*/
	const TUid KKeyGeneratorInterfaceUid={KKeyGeneratorInterface};

	/**
	Key generator Interface Uid
	*/
	const TUid KKeypairGeneratorInterfaceUid={KKeypairGeneratorInterface};

	/**
	Asynchronous Hash Interface Uid
	*/
	const TUid KAsyncHashInterfaceUid={KAsyncHashInterface};

	/**
	Asynchronous Random Interface Uid
	*/
	const TUid KAsyncRandomInterfaceUid={KAsyncRandomInterface};

	/**
	Asynchronous Symmetric Cipher Interface Uid
	*/
	const TUid KAsyncSymmetricCipherInterfaceUid={KAsyncSymmetricCipherInterface};

	/**
	Asynchronous Asymmetric Cipher Interface Uid
	*/
	const TUid KAsyncAsymmetricCipherInterfaceUid={KAsyncAsymmetricCipherInterface};

	/**
	Asynchronous Signer Interface Uid
	*/
	const TUid KAsyncSignerInterfaceUid={KAsyncSignerInterface};

	/**
	Asynchronous Verifier Interface Uid
	*/
	const TUid KAsyncVerifierInterfaceUid={KAsyncVerifierInterface};

	/**
	Asynchronous Key Agreement Interface Uid
	*/
	const TUid KAsyncKeyAgreementInterfaceUid={KAsyncKeyAgreementInterface};

	/**
	Asynchronous Key generator Interface Uid
	*/
	const TUid KAsyncKeyGeneratorInterfaceUid={KAsyncKeyGeneratorInterface};

	/**
	Asynchronous Key generator Interface Uid
	*/
	const TUid KAsyncKeypairGeneratorInterfaceUid={KAsyncKeypairGeneratorInterface};

	// Need to initialise directly by element to avoid ARMV5 compiler generating
	// an array construction table which causes writable global data to be placed
	// in the binary (writable global data is not permitted by the movable memory
	// model which e.g., the H2 reference board has to use).
	const TUid KInterfacesUids[]=
		{
		{KHashInterface}, 
		{KRandomInterface}, 
		{KSymmetricCipherInterface}, 
		{KAsymmetricCipherInterface},
		{KSignerInterface},
		{KVerifierInterface},
		{KKeyAgreementInterface},
		{KKeyGeneratorInterface},
		{KKeypairGeneratorInterface},
		{KAsyncHashInterface},
		{KAsyncRandomInterface},
		{KAsyncSymmetricCipherInterface},
		{KAsyncAsymmetricCipherInterface},
		{KAsyncSignerInterface},
		{KAsyncVerifierInterface},
		{KAsyncKeyAgreementInterface},
		{KAsyncKeyGeneratorInterface},
		{KAsyncKeypairGeneratorInterface},
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT		
		{KMacInterface},
		{KAsyncMacInterface},
#endif		
		};

	/**
	Definition of Hash algorithm UID value
	*/
	const TInt32 KAlgorithmHashMd2 = 0x10283173;
	const TInt32 KAlgorithmHashMd4 = 0x2000B341;
	const TInt32 KAlgorithmHashMd5 = 0x10283174;
	const TInt32 KAlgorithmHashSha1 = 0x10283175;
	const TInt32 KAlgorithmHashSha224 = 0x2000E258;
	const TInt32 KAlgorithmHashSha256 = 0x2000E259;
	const TInt32 KAlgorithmHashSha384 = 0x2000E25A;
	const TInt32 KAlgorithmHashSha512 = 0x2000E25B;

	/**
	Md2 algorithm Uid
	*/	
	const TUid KMd2Uid={KAlgorithmHashMd2};

	/**
	Md4 algorithm Uid
	*/	
	const TUid KMd4Uid={KAlgorithmHashMd4};

	/**
	Md5 algorithm Uid
	*/	
	const TUid KMd5Uid={KAlgorithmHashMd5};

	/**
	Sha1 algorithm Uid
	*/	
	const TUid KSha1Uid={KAlgorithmHashSha1};

	/**
	Sha224 algorithm Uid
	*/	
	const TUid KSha224Uid={KAlgorithmHashSha224};
	
	/**
	Sha256 algorithm Uid
	*/	
	const TUid KSha256Uid={KAlgorithmHashSha256};
	
	/**
	Sha384 algorithm Uid
	*/	
	const TUid KSha384Uid={KAlgorithmHashSha384};
	
	/**
	Sha512 algorithm Uid
	*/	
	const TUid KSha512Uid={KAlgorithmHashSha512};

	/**
	Definition of hash mode Uid value
	*/
	const TInt32 KHashMode = 0x10283176;
	const TInt32 KHmacMode = 0x10283177;
		
	/**
	Hash mode Uid
	*/
	const TUid KHashModeUid = {KHashMode};

	/**
	Hmac mode Uid
	*/
	const TUid KHmacModeUid = {KHmacMode};

	/**
	Definition of Random algorithm UID value
	*/ 
	const TInt32 KAlgorithmRandom = 0x10283178;

	/**
	Random algorithm Uid
	*/	
	const TUid KRandomUid={KAlgorithmRandom};

	/**
	Definition of Symmetric Cipher algorithm UID value
	*/ 
	const TInt32 KAlgorithmCipherDes = 0x10283179;
	const TInt32 KAlgorithmCipher3Des = 0x1028317a;
	const TInt32 KAlgorithmCipherRc2 = 0x1028317b;
	const TInt32 KAlgorithmCipherArc4 = 0x1028317c;
	const TInt32 KAlgorithmCipherAes = 0x1028317d;
	const TInt32 KAlgorithmCipherMisty1 = 0x102831be;
	const TInt32 KAlgorithmCipherMisty2 = 0x102831bf;
	const TInt32 KAlgorithmCipherKasumi = 0x102831c0;

	/**
	Des algorithm Uid
	*/
	const TUid KDesUid={KAlgorithmCipherDes};

	/**
	3Des algorithm Uid
	*/
	const TUid K3DesUid={KAlgorithmCipher3Des};

	/**
	RC2 algorithm Uid
	*/
	const TUid KRc2Uid={KAlgorithmCipherRc2};

	/**
	ARC4 algorithm Uid
	*/
	const TUid KArc4Uid={KAlgorithmCipherArc4};

	/**
	AES algorithm Uid
	*/
	const TUid KAesUid={KAlgorithmCipherAes};

	/**
	Misty1 (RFC 2294) algorithm Uid
	*/
	const TUid KMisty1Uid={KAlgorithmCipherMisty1};

	/**
	Misty2 algorithm Uid
	*/
	const TUid KMisty2Uid={KAlgorithmCipherMisty2};
	
	/**
	Kasumi algorithm Uid
	*/
	const TUid KKasumiUid={KAlgorithmCipherKasumi};	

	/**
	Definition of Asymmetric Cipher algorithm UID value
	*/ 
	const TInt32 KAlgorithmCipherRsa = 0x1028317e;

	/**
	Rsa Cipher algorithm Uid
	*/
	const TUid KRsaCipherUid={KAlgorithmCipherRsa};

	/**
	Definition of Signer algorithm UID value
	*/ 
	const TInt32 KAlgorithmSignerRsa = 0x1028317f;
	const TInt32 KAlgorithmSignerDsa = 0x10283180;

	/**
	Rsa Signer algorithm Uid
	*/
	const TUid KRsaSignerUid={KAlgorithmSignerRsa};

	/**
	Dsa Signer algorithm Uid
	*/
	const TUid KDsaSignerUid={KAlgorithmSignerDsa};

	/**
	Definition of Verifier algorithm UID value
	*/
	const TInt32 KAlgorithmVerifierRsa = 0x10283181;
	const TInt32 KAlgorithmVerifierDsa = 0x10283182;

	/**
	Rsa Veifier algorithm Uid
	*/
	const TUid KRsaVerifierUid={KAlgorithmVerifierRsa};

	/**
	Dsa Signer algorithm Uid
	*/
	const TUid KDsaVerifierUid={KAlgorithmVerifierDsa};


	/**
	Definition of Key Agreement algorithm UID value
	*/
	const TInt32 KAlgorithmKeyAgreementDH = 0x10283183;

	/**
	DH Agreement algorithm Uid
	*/
	const TUid KDHAgreementUid={KAlgorithmKeyAgreementDH};

	/**
	Definition of Key Pair Generator algorithm UID value
	*/
	const TInt32 KAlgorithmDHKeyPairGenerator = 0x10283184;
	const TInt32 KAlgorithmRSAKeyPairGenerator = 0x10283185;
	const TInt32 KAlgorithmDSAKeyPairGenerator = 0x10283186;
	/**
	DH Key Pair Generator Algorithm Uid
	*/
	const TUid KDHKeyPairGeneratorUid={KAlgorithmDHKeyPairGenerator};

	/**
	RSA Key Pair Generator Algorithm Uid
	*/
	const TUid KRSAKeyPairGeneratorUid={KAlgorithmRSAKeyPairGenerator};

	/**
	DSA Key Pair Generator Algorithm Uid
	*/
	const TUid KDSAKeyPairGeneratorUid={KAlgorithmDSAKeyPairGenerator};

	/**
	This fake interface uid is used to allow the sub-class of CPadding
	to be determined via Extension_ because true RTTI is not supported.		
	*/
	const TInt32 KPaddingInterface = 0x10283187;
	const TUid KPaddingInterfaceUid = {KPaddingInterface};		

	/**
	Definition of Padding mode UID value
	*/
	const TInt32 KPaddingModeNone = 0x10283188;
	const TInt32 KPaddingModeSSLv3 = 0x10283189;
	const TInt32 KPaddingModePKCS7 = 0x1028318a;
	const TInt32 KPaddingModePkcs1_v1_5_Encryption = 0x1028318b;
	const TInt32 KPaddingModePkcs1_v1_5_Signature = 0x1028318c;
	
	/**
	Padding Mode None Uid
	*/
	const TUid KPaddingModeNoneUid = {KPaddingModeNone};
	
	/**
	SSLv3 Padding Mode Uid
	*/	
	const TUid KPaddingModeSSLv3Uid = {KPaddingModeSSLv3};

	/**
	PKCS7 Padding Mode Uid
	*/		
	const TUid KPaddingModePKCS7Uid = {KPaddingModePKCS7};
	
	/**
	Pkcs1 v1.5 Encryption Padding mode Uid
	*/
	const TUid KPaddingModePkcs1_v1_5_EncryptionUid = {KPaddingModePkcs1_v1_5_Encryption};
	
	/**
	Pkcs1 v1.5 Signature Padding mode Uid
	*/	
	const TUid KPaddingModePkcs1_v1_5_SignatureUid = {KPaddingModePkcs1_v1_5_Signature};	
	
	/**
	Definition of Cryptography mode UID value
	*/
	const TInt32 KCryptoModeEncrypt = 0x1028318d;
	const TInt32 KCryptoModeDecrypt = 0x1028318e;
	const TInt32 KCryptoModeNone = 0x1028318f;
	
	/**
	Crypto Encrypt Mode Uid
	*/
	const TUid KCryptoModeEncryptUid = {KCryptoModeEncrypt};
	
	/**
	Crypto Decrypt Mode Uid
	*/
	const TUid KCryptoModeDecryptUid = {KCryptoModeDecrypt};
	
	/**
	Crypto None Mode Uid
	*/
	const TUid KCryptoModeNoneUid = {KCryptoModeNone};

	/**
	Definition of Symmetric operation mode Uid value
	*/
	const TInt32 KOperationModeNone = 0x10283190;
	const TInt32 KOperationModeECB = 0x10283191;
	const TInt32 KOperationModeCBC = 0x10283192;
	const TInt32 KOperationModeOFB = 0x10283193;
	const TInt32 KOperationModeCFB = 0x10283194;
	const TInt32 KOperationModeCTR = 0x10283195;
	
	/**
	None Mode Uid
	*/
	const TUid KOperationModeNoneUid = {KOperationModeNone};
	
	/**
	ECB Mode Uid
	*/
	const TUid KOperationModeECBUid = {KOperationModeECB};
	
	/**
	CBC Mode Uid
	*/	
	const TUid KOperationModeCBCUid = {KOperationModeCBC};
	
	/**
	OFB Mode Uid
	*/	
	const TUid KOperationModeOFBUid = {KOperationModeOFB};
	
	/**
	CFB Mode Uid
	*/	
	const TUid KOperationModeCFBUid = {KOperationModeCFB};
	
	/**
	CTR Mode Uid
	*/	
	const TUid KOperationModeCTRUid = {KOperationModeCTR};

 	/**
 	Definition of the Hmac key parameter Uid value
 	*/
 	const TInt32 KHmacKeyParameter = 0x102831ba;	
 	const TUid KHmacKeyParameterUid={KHmacKeyParameter};

	/**
	Definition of the Symmetric key parameter Uid value
	*/
	const TInt32 KSymmetricKeyParameter = 0x10283196;	
	const TUid KSymmetricKeyParameterUid={KSymmetricKeyParameter};
	
	/**
	Definition of the Dsa key parameter Uid value
	*/	
	const TInt32 KDsaKeyParameterP = 0x10283197;
	const TInt32 KDsaKeyParameterQ = 0x10283198;
	const TInt32 KDsaKeyParameterG = 0x10283199;
	const TInt32 KDsaKeyParameterY = 0x1028319a;
	const TInt32 KDsaKeyParameterX = 0x1028319b;
	const TInt32 KDsaKeyGenerationCounter = 0x1028319c;
	const TInt32 KDsaKeyGenerationSeed = 0x1028319d;

	const TUid KDsaKeyParameterPUid={KDsaKeyParameterP};
	const TUid KDsaKeyParameterQUid={KDsaKeyParameterQ};
	const TUid KDsaKeyParameterGUid={KDsaKeyParameterG};
	const TUid KDsaKeyParameterYUid={KDsaKeyParameterY};
	const TUid KDsaKeyParameterXUid={KDsaKeyParameterX};
	const TUid KDsaKeyGenerationCounterUid={KDsaKeyGenerationCounter};
	const TUid KDsaKeyGenerationSeedUid={KDsaKeyGenerationSeed};

	/**
	Definition of the Diffie-Hellman parameter Uid value
	*/
	const TInt32 KDhKeyParameterN = 0x1028319e;
	const TInt32 KDhKeyParameterG = 0x1028319f;
	const TInt32 KDhKeyParameterX = 0x102831a0;
	const TInt32 KDhKeyParameterx = 0x102831a1;

	const TUid KDhKeyParameterNUid = {KDhKeyParameterN};
	const TUid KDhKeyParameterGUid = {KDhKeyParameterG};
	const TUid KDhKeyParameterXUid = {KDhKeyParameterX};
	const TUid KDhKeyParameterxUid = {KDhKeyParameterx};

	/**
	Definition of the RSA cipher parameter Uid value
	*/
	const TInt32 KRsaKeyParameterN = 0x102831a2;
	const TInt32 KRsaKeyParameterE = 0x102831a3;
	const TInt32 KRsaKeyParameterD = 0x102831a4;
	const TInt32 KRsaKeyParameterP = 0x102831a5;
	const TInt32 KRsaKeyParameterQ = 0x102831a6;
	const TInt32 KRsaKeyParameterQInv = 0x102831a7;
	const TInt32 KRsaKeyParameterDP = 0x102831a8;
	const TInt32 KRsaKeyParameterDQ = 0x102831a9;

	const TUid KRsaKeyParameterNUid = {KRsaKeyParameterN};
	const TUid KRsaKeyParameterEUid = {KRsaKeyParameterE};
	const TUid KRsaKeyParameterDUid = {KRsaKeyParameterD};
	const TUid KRsaKeyParameterPUid = {KRsaKeyParameterP};
	const TUid KRsaKeyParameterQUid = {KRsaKeyParameterQ};
	const TUid KRsaKeyParameterQInvUid = {KRsaKeyParameterQInv};
	const TUid KRsaKeyParameterDPUid = {KRsaKeyParameterDP};
	const TUid KRsaKeyParameterDQUid = {KRsaKeyParameterDQ};

	/**
	Definition of the DSA Signature Parameter Uid value
	*/
	const TInt32 KDsaSignatureParameterR = 0x102831aa;
	const TInt32 KDsaSignatureParameterS = 0x102831ab;
	
	const TUid KDsaSignatureParameterRUid = {KDsaSignatureParameterR};
	const TUid KDsaSignatureParameterSUid = {KDsaSignatureParameterS};

	/**
	Definition of the RSA Signature Parameter Uid value
	*/	
	const TInt32 KRsaSignatureParameterS = 0x102831ac;
	const TUid KRsaSignatureParameterSUid = {KRsaSignatureParameterS};

	/**
	Definition of the key type uid
	*/
	const TInt32 KRsaKeyType = 0x102831ad;
	const TUid KRsaKeyTypeUid= {KRsaKeyType};

	/**
	Definition of Symmetric and Asymmetric Key Type Uid value
	*/
	const TInt32 KHmacKey = 0x102831bb;
	const TInt32 KSymmetricKey = 0x102831ae;	
	const TInt32 KDsaPublicKey = 0x102831af;
	const TInt32 KDsaPrivateKey = 0x102831b0;
	const TInt32 KRsaPublicKey = 0x102831b1;
	const TInt32 KRsaPrivateKeyStandard = 0x102831b2;
	const TInt32 KRsaPrivateKeyCRT = 0x102831b3;
	const TInt32 KDHPublicKey = 0x102831b4;
	const TInt32 KDHPrivateKey = 0x102831b5;
	const TInt32 KDHAgreedKey = 0x102831b6;

	const TUid KHmacKeyUid={KHmacKey};
	const TUid KSymmetricKeyUid={KSymmetricKey};
	const TUid KDsaPublicKeyUid={KDsaPublicKey};
	const TUid KDsaPrivateKeyUid={KDsaPrivateKey};
	const TUid KRsaPublicKeyUid={KRsaPublicKey};
	const TUid KRsaPrivateKeyStandardUid={KRsaPrivateKeyStandard};
	const TUid KRsaPrivateKeyCRTUid={KRsaPrivateKeyCRT};
	const TUid KDHPublicKeyUid={KDHPublicKey};
	const TUid KDHPrivateKeyUid={KDHPrivateKey};
	const TUid KDHAgreedKeyUid={KDHAgreedKey};

	const TUid KRC2EffectiveKeyLenBits = { 0x102831bc };
	const TUid KARC4DiscardBytes = { 0x102831bd };

	/**
	Definition of Key Attribute value
	*/
	const TInt32 KNonEmbeddedKey = 0x102831b7;
	const TInt32 KExtractableKey = 0x102831b8;
	const TInt32 KNonExtractableKey = 0x102831b9;

	const TUid KNonEmbeddedKeyUid={KNonEmbeddedKey};
	const TUid KExtractableKeyUid={KExtractableKey};
	const TUid KNonExtractableKeyUid={KNonExtractableKey};

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
	/**
	 * The UID below is not specific to ECC. This should be used 
	 * as a CKey key param, whenever the user of CryptoSPI just has
	 * the handle and not the actual key with him. The SPI plugin 
	 * should search for this ID and invoke the right apis for 
	 * getting access to keys.
	 */
    const TInt32 KPassedHandleToKey = 0x20025161;
    const TUid KPassedHandleToKeyUid = {KPassedHandleToKey};

#ifdef SYMBIAN_ENABLE_SDP_ECC
    /**
    UIDs related to Ecc (which is Asymmetric Cipher)
    */
    const TInt32 KAlgorithmCipherEcc = 0x20024482;
    const TInt32 KAlgorithmSignerEcc = 0x20024483;
    const TInt32 KAlgorithmVerifierEcc = 0x20024486;
    const TInt32 KAlgorithmECCKeyPairGenerator = 0x20024487;

    const TUid KEccCipherUid = {KAlgorithmCipherEcc};
    const TUid KEccSignerUid = {KAlgorithmSignerEcc};
    const TUid KEccVerifierUid = {KAlgorithmVerifierEcc};
    const TUid KEccKeyPairGeneratorUid = {KAlgorithmECCKeyPairGenerator};

    /**
    UIDs related to ECC keys
    */
    const TInt32 KEccKeyType = 0x20025160;
    const TInt32 KEccPublicKey = 0x20024484;
    const TInt32 KEccPrivateKey = 0x20024485;

    const TUid KEccKeyTypeUid = {KEccKeyType};
    const TUid KEccPublicKeyUid = {KEccPublicKey};
    const TUid KEccPrivateKeyUid = {KEccPrivateKey};

    /**
    UIDs related to ECC key params
    */
    const TInt32 KEccFiniteFieldFP = 0x20025151;
    const TInt32 KEccFiniteFieldF2M = 0x20025152;
    const TInt32 KEccKeyGenerationSeed = 0x20025153;
    const TInt32 KEccKeyParameterP = 0x20025154;
    const TInt32 KEccKeyParameterA = 0x20025155;
    const TInt32 KEccKeyParameterB = 0x20025156;
    const TInt32 KEccKeyParameterG = 0x20025157;
    const TInt32 KEccKeyParameterN = 0x20025158;
    const TInt32 KEccKeyParameterH = 0x20025159;
    const TInt32 KEccKeyParameterM = 0x2002515A;
    const TInt32 KEccKeyParameterF = 0x2002515B;
    const TInt32 KEccKeyParameterD = 0x2002515C;
    const TInt32 KEccKeyParameterQ = 0x2002515D;
    const TInt32 KEccSignatureParameterR = 0x2002515E;
    const TInt32 KEccSignatureParameterS = 0x2002515F;

    const TUid KEccFiniteFieldFPUid = {KEccFiniteFieldFP};
    const TUid KEccFiniteFieldF2MUid = {KEccFiniteFieldF2M};
    const TUid KEccKeyGenerationSeedUid = {KEccKeyGenerationSeed};
    const TUid KEccKeyParameterPUid = {KEccKeyParameterP};
    const TUid KEccKeyParameterAUid = {KEccKeyParameterA};
    const TUid KEccKeyParameterBUid = {KEccKeyParameterB};
    const TUid KEccKeyParameterGUid = {KEccKeyParameterG};
    const TUid KEccKeyParameterNUid = {KEccKeyParameterN};
    const TUid KEccKeyParameterHUid = {KEccKeyParameterH};
    const TUid KEccKeyParameterMUid = {KEccKeyParameterM};
    const TUid KEccKeyParameterFUid = {KEccKeyParameterF};
    const TUid KEccKeyParameterDUid = {KEccKeyParameterD};
    const TUid KEccKeyParameterQUid = {KEccKeyParameterQ};
    const TUid KEccSignatureParameterRUid = {KEccSignatureParameterR};
    const TUid KEccSignatureParameterSUid = {KEccSignatureParameterS};
#endif //SYMBIAN_ENABLE_SDP_ECC
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
	}


#endif //__CRYPTOAPI_CRYPTOSPIDEF_H__
