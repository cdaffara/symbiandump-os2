/** 
 * XMLSec library
 *
 * This is free software; see Copyright file in the source
 * distribution for preciese wording.
 * 
 * Copyright (C) 2002-2003 Aleksey Sanin <aleksey@aleksey.com>
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */
#ifndef __XMLSEC_SYMBIANCRYPTO_SYMBOLS_H__
#define __XMLSEC_SYMBIANCRYPTO_SYMBOLS_H__    

#include "xmlsecc_config.h"

#if !defined(IN_XMLSEC) && defined(XMLSEC_CRYPTO_DYNAMIC_LOADING)
#error To disable dynamic loading of xmlsec-crypto libraries undefine XMLSEC_CRYPTO_DYNAMIC_LOADING
#endif /* !defined(IN_XMLSEC) && defined(XMLSEC_CRYPTO_DYNAMIC_LOADING) */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */ 

/**
 * Defines for writing simple code
 */
#ifdef XMLSEC_CRYPTO_SYMBIANCRYPTO

/**  
 * Crypto Init/shutdown
 */
#define xmlSecCryptoInit			xmlSecSymbianCryptoInit
#define xmlSecCryptoShutdown			xmlSecSymbianCryptoShutdown

#define xmlSecCryptoKeysMngrInit		xmlSecSymbianCryptoKeysMngrInit

/**
 * Key data ids
 */
#define xmlSecKeyDataAesId			xmlSecSymbianCryptoKeyDataAesId
#define xmlSecKeyDataDesId			xmlSecSymbianCryptoKeyDataDesId
#define xmlSecKeyDataDsaId			xmlSecSymbianCryptoKeyDataDsaId
#define xmlSecKeyDataHmacId			xmlSecSymbianCryptoKeyDataHmacId
#define xmlSecKeyDataRsaId			xmlSecSymbianCryptoKeyDataRsaId
#define xmlSecKeyDataX509Id			xmlSecSymbianCryptoKeyDataX509Id
#define xmlSecKeyDataRawX509CertId		xmlSecSymbianCryptoKeyDataRawX509CertId

/**
 * Key data store ids
 */
#define xmlSecX509StoreId			xmlSecSymbianCryptoX509StoreId

/**
 * Crypto transforms ids
 */
#define xmlSecTransformAes128CbcId		xmlSecSymbianCryptoTransformAes128CbcId
#define xmlSecTransformAes192CbcId		xmlSecSymbianCryptoTransformAes192CbcId
#define xmlSecTransformAes256CbcId		xmlSecSymbianCryptoTransformAes256CbcId
#define xmlSecTransformKWAes128Id		xmlSecSymbianCryptoTransformKWAes128Id
#define xmlSecTransformKWAes192Id		xmlSecSymbianCryptoTransformKWAes192Id
#define xmlSecTransformKWAes256Id		xmlSecSymbianCryptoTransformKWAes256Id
#define xmlSecTransformDes3CbcId		xmlSecSymbianCryptoTransformDes3CbcId
#define xmlSecTransformKWDes3Id			xmlSecSymbianCryptoTransformKWDes3Id
#define xmlSecTransformDsaSha1Id		xmlSecSymbianCryptoTransformDsaSha1Id
#define xmlSecTransformHmacMd5Id		xmlSecSymbianCryptoTransformHmacMd5Id
#define xmlSecTransformHmacRipemd160Id		xmlSecSymbianCryptoTransformHmacRipemd160Id
#define xmlSecTransformHmacSha1Id		xmlSecSymbianCryptoTransformHmacSha1Id
#define xmlSecTransformRipemd160Id		xmlSecSymbianCryptoTransformRipemd160Id
#define xmlSecTransformRsaSha1Id		xmlSecSymbianCryptoTransformRsaSha1Id
#define xmlSecTransformRsaPkcs1Id		xmlSecSymbianCryptoTransformRsaPkcs1Id
#define xmlSecTransformRsaOaepId		xmlSecSymbianCryptoTransformRsaOaepId
#define xmlSecTransformSha1Id			xmlSecSymbianCryptoTransformSha1Id

/**
 * High level routines form xmlsec command line utility
 */ 
#define xmlSecCryptoAppInit			xmlSecSymbianCryptoAppInit
#define xmlSecCryptoAppShutdown			xmlSecSymbianCryptoAppShutdown
#define xmlSecCryptoAppDefaultKeysMngrInit	xmlSecSymbianCryptoAppDefaultKeysMngrInit
#define xmlSecCryptoAppDefaultKeysMngrAdoptKey	xmlSecSymbianCryptoAppDefaultKeysMngrAdoptKey
#define xmlSecCryptoAppDefaultKeysMngrLoad	xmlSecSymbianCryptoAppDefaultKeysMngrLoad
#define xmlSecCryptoAppDefaultKeysMngrSave	xmlSecSymbianCryptoAppDefaultKeysMngrSave
#define xmlSecCryptoAppKeysMngrCertLoad		xmlSecSymbianCryptoAppKeysMngrCertLoad
#define xmlSecCryptoAppKeysMngrCertLoadMemory	xmlSecSymbianCryptoAppKeysMngrCertLoadMemory
#define xmlSecCryptoAppKeyLoadWithName	xmlSecSymbianCryptoAppKeyLoadWithName
#define xmlSecCryptoAppPkcs12Load		xmlSecSymbianCryptoAppPkcs12Load
#define xmlSecCryptoAppKeyCertLoad		xmlSecSymbianCryptoAppKeyCertLoad
#define xmlSecCryptoAppKeyLoadMemoryWithName xmlSecSymbianCryptoAppKeyLoadMemoryWithName
#define xmlSecCryptoAppPkcs12LoadMemory		xmlSecSymbianCryptoAppPkcs12LoadMemory
#define xmlSecCryptoAppKeyCertLoadMemory	xmlSecSymbianCryptoAppKeyCertLoadMemory
#define xmlSecCryptoAppGetDefaultPwdCallback	xmlSecSymbianCryptoAppGetDefaultPwdCallback

#endif /* XMLSEC_CRYPTO_SYMBIANCRYPTO */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __XMLSEC_SYMBIANCRYPTO_CRYPTO_H__ */

#define __XMLSEC_SYMBIANCRYPTO_CRYPTO_H__    
