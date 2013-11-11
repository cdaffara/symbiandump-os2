/** 
 * XMLSec library
 *
 * This is free software; see Copyright file in the source
 * distribution for preciese wording.
 * 
 * Copyright (C) 2002-2003 Aleksey Sanin <aleksey@aleksey.com>
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */
#ifndef __XMLSEC_SYMBIANCRYPTO_APP_H__
#define __XMLSEC_SYMBIANCRYPTO_APP_H__    

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */ 
#include "xmlsecc_config.h"
#include "xmlsec_xmlsec.h"
#include "xmlsec_keys.h"
#include "xmlsec_keysmngr.h"
#include "xmlsec_transforms.h"

#include "xmlsecc_bio.h"

/**
 * Init/shutdown
 */
XMLSEC_CRYPTO_EXPORT int	xmlSecSymbianCryptoAppInit			(const char* config);
XMLSEC_CRYPTO_EXPORT int	xmlSecSymbianCryptoAppShutdown			(void);

/** 
 * Keys Manager
 */
XMLSEC_CRYPTO_EXPORT int	xmlSecSymbianCryptoAppDefaultKeysMngrInit	(xmlSecKeysMngrPtr mngr);
XMLSEC_CRYPTO_EXPORT int 	xmlSecSymbianCryptoAppDefaultKeysMngrAdoptKey	(xmlSecKeysMngrPtr mngr,
									 xmlSecKeyPtr key);
XMLSEC_CRYPTO_EXPORT int 	xmlSecSymbianCryptoAppDefaultKeysMngrLoad	(xmlSecKeysMngrPtr mngr,
									 const char* uri);
XMLSEC_CRYPTO_EXPORT int 	xmlSecSymbianCryptoAppDefaultKeysMngrSave	(xmlSecKeysMngrPtr mngr,
									 const char* filename,
									 xmlSecKeyDataType type);
#ifndef XMLSEC_NO_X509
XMLSEC_CRYPTO_EXPORT int	xmlSecSymbianCryptoAppKeysMngrCertLoad		(xmlSecKeysMngrPtr mngr, 
									 const char *filename, 
									 xmlSecKeyDataFormat format, 
									 xmlSecKeyDataType type);
XMLSEC_CRYPTO_EXPORT int	xmlSecSymbianCryptoAppKeysMngrCertLoadMemory	(xmlSecKeysMngrPtr mngr,
									 const xmlSecByte* data,
									 xmlSecSize dataSize,
									 xmlSecKeyDataFormat format,
									 xmlSecKeyDataType type);
XMLSEC_CRYPTO_EXPORT int	xmlSecSymbianCryptoAppKeysMngrCertLoadBIO (xmlSecKeysMngrPtr mngr, 
                                                                    BIO* bio, 
                                                                    xmlSecKeyDataFormat format,
                                                                    xmlSecKeyDataType type);									 
#endif /* XMLSEC_NO_X509 */


/** 
 * Keys
 */
XMLSEC_CRYPTO_EXPORT xmlSecKeyPtr xmlSecSymbianCryptoAppKeyLoadWithName		(const char *filename, 
									 xmlSecKeyDataFormat format,
									 const char *keyname,
									 const char *pwd,
									 void *pwdCallback,
									 void* pwdCallbackCtx);									 
XMLSEC_CRYPTO_EXPORT xmlSecKeyPtr xmlSecSymbianCryptoAppKeyLoadMemoryWithName (
                                                            const xmlSecByte* data,
									 xmlSecSize dataSize,
									 xmlSecKeyDataFormat format,
									 const char *keyname,
									 const char *pwd,
									 void* pwdCallback,
									 void* pwdCallbackCtx);		
XMLSEC_CRYPTO_EXPORT xmlSecKeyPtr xmlSecSymbianCryptoAppKeyLoadBIO		(BIO* bio, 
									 xmlSecKeyDataFormat format,
									 const char *pwd,
									 void* pwdCallback,
									 void* pwdCallbackCtx);											 						 
#ifndef XMLSEC_NO_X509
XMLSEC_CRYPTO_EXPORT xmlSecKeyPtr xmlSecSymbianCryptoAppPkcs12Load		(const char *filename, 
									 const char *pwd,
		    							 void* pwdCallback, 
									 void* pwdCallbackCtx);
XMLSEC_CRYPTO_EXPORT xmlSecKeyPtr xmlSecSymbianCryptoAppPkcs12LoadMemory	(const xmlSecByte* data,
									 xmlSecSize dataSize,
									 const char *pwd,
									 void* pwdCallback,
									 void* pwdCallbackCtx);
XMLSEC_CRYPTO_EXPORT int	xmlSecSymbianCryptoAppKeyCertLoad		(xmlSecKeyPtr key,
									 const char* filename,
									 xmlSecKeyDataFormat format);
XMLSEC_CRYPTO_EXPORT int	xmlSecSymbianCryptoAppKeyCertLoadMemory	(xmlSecKeyPtr key,
									 const xmlSecByte* data,
									 xmlSecSize dataSize,
									 xmlSecKeyDataFormat format);
XMLSEC_CRYPTO_EXPORT int	xmlSecSymbianCryptoAppKeyCertLoadBIO		(xmlSecKeyPtr key,
									 BIO* bio,
									 xmlSecKeyDataFormat format);
#endif /* XMLSEC_NO_X509 */
XMLSEC_CRYPTO_EXPORT void*	xmlSecSymbianCryptoAppGetDefaultPwdCallback	(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __XMLSEC_SYMBIANCRYPTO_APP_H__ */

