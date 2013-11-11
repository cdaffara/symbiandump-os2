/** 
 * XMLSec library
 *
 * This is free software; see Copyright file in the source
 * distribution for preciese wording.
 * 
 * Copyright (C) 2002-2003 Aleksey Sanin <aleksey@aleksey.com>
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */
#ifndef __XMLSEC_SYMBIANCRYPTO_X509_H__
#define __XMLSEC_SYMBIANCRYPTO_X509_H__    

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */ 

#include "xmlsecc_config.h"
#ifndef XMLSEC_NO_X509

//#include <openssl/x509.h>

#include "xmlsec_xmlsec.h"
#include "xmlsec_keys.h"
#include "xmlsec_transforms.h"

#include "xmlsecc_x509wrapper.h"

/**
 * XMLSEC_STACK_OF_X509:
 *
 * Macro. To make docbook happy.
 */
#define XMLSEC_STACK_OF_X509		STACK_OF(X509)

/**
 * XMLSEC_STACK_OF_X509_CRL:
 *
 * Macro. To make docbook happy.
 */
#define XMLSEC_STACK_OF_X509_CRL	STACK_OF(X509_CRL)


/**
 * xmlSecSymbianCryptoKeyDataX509Id:
 * 
 * The SymbianCrypto X509 data klass.
 */
#define xmlSecSymbianCryptoKeyDataX509Id \
	xmlSecSymbianCryptoKeyDataX509GetKlass()
XMLSEC_CRYPTO_EXPORT xmlSecKeyDataId 	xmlSecSymbianCryptoKeyDataX509GetKlass(void);
XMLSEC_CRYPTO_EXPORT X509* 		xmlSecSymbianCryptoKeyDataX509GetKeyCert(xmlSecKeyDataPtr data);
XMLSEC_CRYPTO_EXPORT int         xmlSecSymbianCryptoKeyDataX509AdoptKeyCert(xmlSecKeyDataPtr data, 
                                                                                X509* cert);

XMLSEC_CRYPTO_EXPORT int 		xmlSecSymbianCryptoKeyDataX509AdoptCert(xmlSecKeyDataPtr data,
									 X509* cert);
XMLSEC_CRYPTO_EXPORT X509* 		xmlSecSymbianCryptoKeyDataX509GetCert	(xmlSecKeyDataPtr data,
									 xmlSecSize pos);
XMLSEC_CRYPTO_EXPORT xmlSecSize		xmlSecSymbianCryptoKeyDataX509GetCertsSize(xmlSecKeyDataPtr data);

XMLSEC_CRYPTO_EXPORT int 		xmlSecSymbianCryptoKeyDataX509AdoptCrl(xmlSecKeyDataPtr data,
									 X509_CRL* crl);
XMLSEC_CRYPTO_EXPORT X509_CRL*		xmlSecSymbianCryptoKeyDataX509GetCrl	(xmlSecKeyDataPtr data,
									 xmlSecSize pos);
XMLSEC_CRYPTO_EXPORT xmlSecSize		xmlSecSymbianCryptoKeyDataX509GetCrlsSize(xmlSecKeyDataPtr data);

XMLSEC_CRYPTO_EXPORT xmlSecKeyDataPtr	xmlSecSymbianCryptoX509CertGetKey	(X509* cert);

/**
 * xmlSecSymbianCryptoKeyDataRawX509CertId:
 * 
 * The SymbianCrypto raw X509 certificate klass.
 */
#define xmlSecSymbianCryptoKeyDataRawX509CertId \
	xmlSecSymbianCryptoKeyDataRawX509CertGetKlass()
XMLSEC_CRYPTO_EXPORT xmlSecKeyDataId 	xmlSecSymbianCryptoKeyDataRawX509CertGetKlass(void);

/**
 * xmlSecSymbianCryptoX509StoreId:
 * 
 * The SymbianCrypto X509 store klass.
 */
#define xmlSecSymbianCryptoX509StoreId \
	xmlSecSymbianCryptoX509StoreGetKlass()
XMLSEC_CRYPTO_EXPORT xmlSecKeyDataStoreId xmlSecSymbianCryptoX509StoreGetKlass(void);
XMLSEC_CRYPTO_EXPORT X509* 		xmlSecSymbianCryptoX509StoreFindCert	(xmlSecKeyDataStorePtr store,
									 xmlChar *subjectName,
									 xmlChar *issuerName, 
									 xmlChar *issuerSerial,
									 xmlChar *ski,
									 xmlSecKeyInfoCtx* keyInfoCtx);
XMLSEC_CRYPTO_EXPORT int 		xmlSecSymbianCryptoX509StoreKeyCertVerify(xmlSecKeyDataStorePtr store, 
									 X509* cert);									 
XMLSEC_CRYPTO_EXPORT X509* 		xmlSecSymbianCryptoX509StoreVerify	(xmlSecKeyDataStorePtr store,
									 XMLSEC_STACK_OF_X509* certs,
									 XMLSEC_STACK_OF_X509_CRL* crls,
									 xmlSecKeyInfoCtx* keyInfoCtx);
XMLSEC_CRYPTO_EXPORT int		xmlSecSymbianCryptoX509StoreAdoptCert	(xmlSecKeyDataStorePtr store,
									 X509* cert,
									 xmlSecKeyDataType type);
XMLSEC_CRYPTO_EXPORT int		xmlSecSymbianCryptoX509StoreAddCertsPath(xmlSecKeyDataStorePtr store,
									 const char* path);
									 


XMLSEC_CRYPTO_EXPORT void xmlSecSetCertStoreFlag();
XMLSEC_CRYPTO_EXPORT void xmlSecResetCertStoreFlag();	
XMLSEC_CRYPTO_EXPORT int xmlSecCheckCertStoreFlag();								 
									 
#endif /* XMLSEC_NO_X509 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __XMLSEC_XMLSEC_SYMBIANCRYPTO_X509_H__ */
