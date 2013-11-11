/** 
 * XMLSec library
 *
 * This is free software; see Copyright file in the source
 * distribution for preciese wording.
 * 
 * Copyright (C) 2002-2003 Aleksey Sanin <aleksey@aleksey.com>
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */
#ifndef __XMLSEC_SYMBIANCRYPTO_CRYPTO_H__
#define __XMLSEC_SYMBIANCRYPTO_CRYPTO_H__    

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */ 
#include "xmlsecc_config.h"
#include "xmlsec_xmlsec.h"
#include "xmlsec_keys.h"
#include "xmlsec_transforms.h"
#include "xmlsec_dl.h"

XMLSEC_CRYPTO_EXPORT xmlSecCryptoDLFunctionsPtr	xmlSecCryptoGetFunctions_gnutls(void);

/**
 * Init shutdown
 */
XMLSEC_CRYPTO_EXPORT int		xmlSecSymbianCryptoInit		(void);
XMLSEC_CRYPTO_EXPORT int		xmlSecSymbianCryptoShutdown		(void);

XMLSEC_CRYPTO_EXPORT int		xmlSecSymbianCryptoKeysMngrInit	(xmlSecKeysMngrPtr mngr);
XMLSEC_CRYPTO_EXPORT int		xmlSecSymbianCryptoGenerateRandom	(xmlSecBufferPtr buffer,
									 xmlSecSize size);

/********************************************************************
 *
 * AES transforms
 *
 *******************************************************************/
#ifndef XMLSEC_NO_AES
/**
 * xmlSecSymbianCryptoKeyDataAesId:
 * 
 * The AES key data klass.
 */
#define xmlSecSymbianCryptoKeyDataAesId \
	xmlSecSymbianCryptoKeyDataAesGetKlass()
XMLSEC_CRYPTO_EXPORT xmlSecKeyDataId 	xmlSecSymbianCryptoKeyDataAesGetKlass	(void);
XMLSEC_CRYPTO_EXPORT int		xmlSecSymbianCryptoKeyDataAesSet	(xmlSecKeyDataPtr data,
									 const xmlSecByte* buf,
									 xmlSecSize bufSize);
/**
 * xmlSecSymbianCryptoTransformAes128CbcId:
 * 
 * The AES128 CBC cipher transform klass.
 */
#define xmlSecSymbianCryptoTransformAes128CbcId \
	xmlSecSymbianCryptoTransformAes128CbcGetKlass()
XMLSEC_CRYPTO_EXPORT xmlSecTransformId	xmlSecSymbianCryptoTransformAes128CbcGetKlass(void);

/**
 * xmlSecSymbianCryptoTransformAes192CbcId:
 * 
 * The AES192 CBC cipher transform klass.
 */
#define xmlSecSymbianCryptoTransformAes192CbcId \
	xmlSecSymbianCryptoTransformAes192CbcGetKlass()
XMLSEC_CRYPTO_EXPORT xmlSecTransformId	xmlSecSymbianCryptoTransformAes192CbcGetKlass(void);

/**
 * xmlSecSymbianCryptoTransformAes256CbcId:
 * 
 * The AES256 CBC cipher transform klass.
 */
#define xmlSecSymbianCryptoTransformAes256CbcId \
	xmlSecSymbianCryptoTransformAes256CbcGetKlass()
XMLSEC_CRYPTO_EXPORT xmlSecTransformId	xmlSecSymbianCryptoTransformAes256CbcGetKlass(void);

#endif /* XMLSEC_NO_AES */

/********************************************************************
 *
 * DES transforms
 *
 *******************************************************************/
#ifndef XMLSEC_NO_DES
/**
 * xmlSecSymbianCryptoKeyDataDesId:
 * 
 * The DES key data klass.
 */
#define xmlSecSymbianCryptoKeyDataDesId \
	xmlSecSymbianCryptoKeyDataDesGetKlass()
XMLSEC_CRYPTO_EXPORT xmlSecKeyDataId 	xmlSecSymbianCryptoKeyDataDesGetKlass	(void);
XMLSEC_CRYPTO_EXPORT int		xmlSecSymbianCryptoKeyDataDesSet	(xmlSecKeyDataPtr data,
									 const xmlSecByte* buf,
									 xmlSecSize bufSize);

/**
 * xmlSecSymbianCryptoTransformDes3CbcId:
 * 
 * The DES3 CBC cipher transform klass.
 */
#define xmlSecSymbianCryptoTransformDes3CbcId \
	xmlSecSymbianCryptoTransformDes3CbcGetKlass()
XMLSEC_CRYPTO_EXPORT xmlSecTransformId xmlSecSymbianCryptoTransformDes3CbcGetKlass(void);

#endif /* XMLSEC_NO_DES */


/********************************************************************
 *
 * HMAC transforms
 *
 *******************************************************************/
#ifndef XMLSEC_NO_HMAC
/** 
 * xmlSecSymbianCryptoKeyDataHmacId:
 * 
 * The HMAC key klass.
 */
#define xmlSecSymbianCryptoKeyDataHmacId \
	xmlSecSymbianCryptoKeyDataHmacGetKlass()
XMLSEC_CRYPTO_EXPORT xmlSecKeyDataId	xmlSecSymbianCryptoKeyDataHmacGetKlass	(void);
XMLSEC_CRYPTO_EXPORT int		xmlSecSymbianCryptoKeyDataHmacSet	(xmlSecKeyDataPtr data,
									 const xmlSecByte* buf,
									 xmlSecSize bufSize);
/**
 * xmlSecSymbianCryptoTransformHmacMd5Id:
 * 
 * The HMAC with MD5 signature transform klass.
 */
#define xmlSecSymbianCryptoTransformHmacMd5Id \
	xmlSecSymbianCryptoTransformHmacMd5GetKlass()
XMLSEC_CRYPTO_EXPORT xmlSecTransformId xmlSecSymbianCryptoTransformHmacMd5GetKlass(void);
#endif /* XMLSEC_NO_HMAC */

/**
 * xmlSecSymbianCryptoTransformHmacRipemd160Id:
 * 
 * The HMAC with RipeMD160 signature transform klass.
 */
#define xmlSecSymbianCryptoTransformHmacRipemd160Id \
	xmlSecSymbianCryptoTransformHmacRipemd160GetKlass()
XMLSEC_CRYPTO_EXPORT xmlSecTransformId xmlSecSymbianCryptoTransformHmacRipemd160GetKlass(void);

/**
 * xmlSecSymbianCryptoTransformHmacSha1Id:
 * 
 * The HMAC with SHA1 signature transform klass.
 */
#define xmlSecSymbianCryptoTransformHmacSha1Id \
	xmlSecSymbianCryptoTransformHmacSha1GetKlass()
XMLSEC_CRYPTO_EXPORT xmlSecTransformId xmlSecSymbianCryptoTransformHmacSha1GetKlass(void);



/********************************************************************
 *
 * SHA1 transform
 *
 *******************************************************************/
#ifndef XMLSEC_NO_SHA1
/**
 * xmlSecSymbianCryptoTransformSha1Id:
 * 
 * The SHA1 digest transform klass.
 */
#define xmlSecSymbianCryptoTransformSha1Id \
	xmlSecSymbianCryptoTransformSha1GetKlass()
XMLSEC_CRYPTO_EXPORT xmlSecTransformId xmlSecSymbianCryptoTransformSha1GetKlass(void);
#endif /* XMLSEC_NO_SHA1 */


/********************************************************************
 *
 * RSA transforms
 *
 *******************************************************************/
#ifndef XMLSEC_NO_RSA

/**
 * xmlSecSymbianCryptoKeyDataRsaId:
 * 
 * The RSA key klass.
 */
#define xmlSecSymbianCryptoKeyDataRsaId \
	xmlSecSymbianCryptoKeyDataRsaGetKlass()
XMLSEC_CRYPTO_EXPORT xmlSecKeyDataId xmlSecSymbianCryptoKeyDataRsaGetKlass(void);


/**
 * xmlSecSymbianCryptoTransformRsaSha1Id:
 * 
 * The RSA-SHA1 signature transform klass.
 */

#define xmlSecSymbianCryptoTransformRsaSha1Id	\
	xmlSecSymbianCryptoTransformRsaSha1GetKlass()
XMLSEC_CRYPTO_EXPORT xmlSecTransformId xmlSecSymbianCryptoTransformRsaSha1GetKlass(void);

/**
 * xmlSecSymbianCryptoTransformRsaPkcs1Id:
 * 
 * The RSA PKCS1 key transport transform klass.
 */
#define xmlSecSymbianCryptoTransformRsaPkcs1Id \
	xmlSecSymbianCryptoTransformRsaPkcs1GetKlass()
XMLSEC_CRYPTO_EXPORT xmlSecTransformId xmlSecSymbianCryptoTransformRsaPkcs1GetKlass(void);

/**
 * xmlSecSymbianCryptoTransformRsaOaepId:
 * 
 * The RSA PKCS1 key transport transform klass.
 */
/*
#define xmlSecSymbianCryptoTransformRsaOaepId \
	xmlSecSymbianCryptoTransformRsaOaepGetKlass()
XMLSEC_CRYPTO_EXPORT xmlSecTransformId xmlSecSymbianCryptoTransformRsaOaepGetKlass(void);
*/
#endif /* XMLSEC_NO_RSA */


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __XMLSEC_SYMBIANCRYPTO_CRYPTO_H__ */

#define __XMLSEC_SYMBIANCRYPTO_CRYPTO_H__    
