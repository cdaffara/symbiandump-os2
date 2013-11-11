/** 
 * XMLSec library
 *
 * This is free software; see Copyright file in the source
 * distribution for preciese wording.
 * 
 * Copyright (C) 2002-2003 Aleksey Sanin <aleksey@aleksey.com>
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */
#ifndef __XMLSEC_SYMBIANCRYPTO_EVP_H__
#define __XMLSEC_SYMBIANCRYPTO_EVP_H__    

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */ 
#include "xmlsecc_config.h"
#include "xmlsec_xmlsec.h"
#include "xmlsec_keys.h"
#include "xmlsec_transforms.h"
#include "xmlsec_keysmngr.h"

// replace <openssl/evp.h>
#include "xmlsecc_evpwrapper.h"

XMLSEC_CRYPTO_EXPORT int 		xmlSecSymbianCryptoEvpKeyDataAdoptEvp	(xmlSecKeyDataPtr data, 
									 EVP_PKEY* pKey);
XMLSEC_CRYPTO_EXPORT EVP_PKEY* 		xmlSecSymbianCryptoEvpKeyDataGetEvp	(xmlSecKeyDataPtr data);

/******************************************************************************
 *
 * EVP helper functions
 *
 *****************************************************************************/
XMLSEC_CRYPTO_EXPORT EVP_PKEY*		xmlSecSymbianCryptoEvpKeyDup		(EVP_PKEY* pKey);
XMLSEC_CRYPTO_EXPORT xmlSecKeyDataPtr 	xmlSecSymbianCryptoEvpKeyAdopt	(EVP_PKEY *pKey);

/****************************************************************************
 *
 * Symbian Keys Store
 *
 ***************************************************************************/
/**
 * xmlSecSymbianKeysStoreId:
 *
 * A symbian keys store klass id.
 */
#define xmlSecSymbianKeysStoreId		xmlSecSymbianKeysStoreGetKlass()

XMLSEC_CRYPTO_EXPORT xmlSecKeyStoreId		xmlSecSymbianKeysStoreGetKlass	(void);

XMLSEC_CRYPTO_EXPORT xmlSecKeyPtr xmlSecSymbianCryptoAppKeyLoadSks(char* keyname);	


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __XMLSEC_SYMBIANCRYPTO_EVP_H__ */


