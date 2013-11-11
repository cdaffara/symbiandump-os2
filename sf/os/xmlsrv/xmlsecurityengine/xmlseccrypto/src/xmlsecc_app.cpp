/** 
 * XMLSec library
 *
 * This is free software; see Copyright file in the source
 * distribution for preciese wording.
 * 
 * Copyright (C) 2002-2003 Aleksey Sanin <aleksey@aleksey.com>
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */
#include <string.h>

#include "xmlsecc_globals.h"

#include "xmlsecc_config.h"
#include "xmlsec_xmlsec.h"
#include "xmlsec_keys.h"
#include "xmlsec_transforms.h"
#include "xmlsec_errors.h"

#include "xmlsecc_app.h"
#include "xmlsecc_crypto.h"
#include "xmlsecc_evp.h"

#ifndef XMLSEC_NO_X509   
#include "xmlsecc_x509.h"
#include "xmlsecc_x509wrapper.h"
#include "xmlsecc_bio.h"
#endif /* XMLSEC_NO_X509 */ 

/**
 * xmlSecSymbianCryptoAppInit:
 * @config:		the path to SymbianCrypto configuration (unused).
 * 
 * General crypto engine initialization. This function is used
 * by XMLSec command line utility and called before 
 * @xmlSecInit function.
 *
 * Returns 0 on success or a negative value otherwise.
 */
EXPORT_C
int
xmlSecSymbianCryptoAppInit(const char* config ATTRIBUTE_UNUSED) {
    int ret(0);
    if( ret ) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "symbiancrypto_init",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    "ret=%d", ret);
	return(-1);
    }
    return(0);
}

/**
 * xmlSecSymbianCryptoAppShutdown:
 * 
 * General crypto engine shutdown. This function is used
 * by XMLSec command line utility and called after 
 * @xmlSecShutdown function.
 *
 * Returns 0 on success or a negative value otherwise.
 */
EXPORT_C
int
xmlSecSymbianCryptoAppShutdown(void) {
    return(0);
}

/**
 * xmlSecSymbianCryptoAppKeyLoadWithName:
 * @filename:		the key filename.
 * @format:		the key file format.
 * @keyname:    the assigned name of key
 * @pwd:		the key file password.
 * @pwdCallback:	the key password callback.
 * @pwdCallbackCtx:	the user context for password callback.
 *
 * Reads key from the a file (not implemented yet).
 *
 * Returns pointer to the key or NULL if an error occurs.
 */
EXPORT_C
xmlSecKeyPtr
xmlSecSymbianCryptoAppKeyLoadWithName(const char* filename, xmlSecKeyDataFormat format,
			const char* keyname,
			const char* pwd,
			void* pwdCallback,
			void* pwdCallbackCtx) {			
    xmlSecKeyPtr key = NULL;
    
    BIO* bio;
    
    			
    xmlSecAssert2(filename, NULL);
    xmlSecAssert2(format != xmlSecKeyDataFormatUnknown, NULL);
     
    bio = BIO_new_file(filename, (const char*)"rb", keyname);
    if(!bio) {
	return(NULL);    
    }	
    
    key = xmlSecSymbianCryptoAppKeyLoadBIO (bio, format, pwd, pwdCallback, pwdCallbackCtx);
    BIO_free(bio);
    if(!key) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecOpenSSLAppKeyLoadBIO",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "filename=%s", 
		    xmlSecErrorsSafeString(filename));
	return(NULL);
    }
    
    if(xmlSecKeySetName(key, (const unsigned char*)keyname) < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecKeySetName",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "filename=%s", 
		    xmlSecErrorsSafeString(filename));
	xmlSecKeyDestroy(key);
	return(NULL);
    }
        
    return(key);
}

/**
 * xmlSecSymbianCryptoAppKeyLoadMemory:
 * @data:		the binary key data.
 * @dataSize:		the size of binary key.
 * @format:		the key file format.
 * @pwd:		the key file password.
 * @pwdCallback:	the key password callback.
 * @pwdCallbackCtx:	the user context for password callback.
 *
 * Reads key from the memory buffer (not implemented yet).
 *
 * Returns pointer to the key or NULL if an error occurs.
 */
EXPORT_C
xmlSecKeyPtr
xmlSecSymbianCryptoAppKeyLoadMemoryWithName(const xmlSecByte* data, xmlSecSize dataSize, 
			xmlSecKeyDataFormat format, const char* keyname, const char* pwd, 
			void* pwdCallback, void* pwdCallbackCtx) {
    
    xmlSecKeyPtr key = NULL;
    
    BIO* bio;
    
    bio = BIO_new_buffer((const char*)data,dataSize,keyname);
    if(!bio) {
	return(NULL);    
    }
    		
    key = xmlSecSymbianCryptoAppKeyLoadBIO(bio, format, pwd, pwdCallback, pwdCallbackCtx);
    BIO_free(bio);
    if(!key) {
	    return(NULL);
    }    
            
    if(xmlSecKeySetName(key, (const unsigned char*)keyname) < 0) {
        xmlSecKeyDestroy(key);
	    return(NULL);
    }
    return(key);
}

/**
 * xmlSecOpenSSLAppKeyLoadBIO:
 * @bio:		the key BIO.
 * @format:		the key file format.
 * @pwd:		the key file password.
 * @pwdCallback:	the key password callback.
 * @pwdCallbackCtx:	the user context for password callback.
 *
 * Reads key from the an OpenSSL BIO object.
 *
 * Returns pointer to the key or NULL if an error occurs.
 */
EXPORT_C
xmlSecKeyPtr
xmlSecSymbianCryptoAppKeyLoadBIO(BIO* bio, xmlSecKeyDataFormat format,
			const char* pwd, void* pwdCallback, 
			void* pwdCallbackCtx) {

    xmlSecKeyPtr key = NULL;
    xmlSecKeyDataPtr data;
    EVP_PKEY* pKey = NULL;    
    int ret(-1);

    xmlSecAssert2(bio, NULL);
    xmlSecAssert2(format != xmlSecKeyDataFormatUnknown, NULL);

    switch (format)
    {
		case xmlSecKeyDataFormatDer:	
        /* try to read private key first */           
	    if(!pKey) {
       	    // go to start of the file and try to read public key 
		    pKey = d2i_PUBKEY_bio(bio);
		    if(!pKey) {
			xmlSecError(XMLSEC_ERRORS_HERE,
				    NULL,
				    "d2i_PrivateKey_bio and d2i_PUBKEY_bio",
				    XMLSEC_ERRORS_R_CRYPTO_FAILED,
				    XMLSEC_ERRORS_NO_MESSAGE);
			return(NULL);
		    }
		}
				
		break;
    	case xmlSecKeyDataFormatPkcs8Der:
        /* try to read private key first */    
		pKey = d2i_PKCS8PrivateKey_bio(bio, pwdCallback, pwdCallbackCtx);
	    if(!pKey) {
		    xmlSecError(XMLSEC_ERRORS_HERE,
				NULL,
				"sc_PrivateKey_read",
				XMLSEC_ERRORS_R_CRYPTO_FAILED,
				XMLSEC_ERRORS_NO_MESSAGE);
		    return(NULL);
		}    					    		  
    	break;
    	default:
			xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    NULL,
		    XMLSEC_ERRORS_R_INVALID_FORMAT,
		    "format=%d", format); 
			return(NULL);    	
    } 

	data = xmlSecSymbianCryptoEvpKeyAdopt(pKey);
    if(!data) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecSymbianCryptoEvpKeyAdopt",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	sc_pkey_free(pKey);
	return(NULL);	    
    }    

    key = xmlSecKeyCreate();
    if(!key) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecKeyCreate",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	xmlSecKeyDataDestroy(data);
	return(NULL);
    }
    
    ret = xmlSecKeySetValue(key, data);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecKeySetValue",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "data=%s",
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)));
	xmlSecKeyDestroy(key);
	xmlSecKeyDataDestroy(data);
	return(NULL);
    } 
    
    return(key);
}


#ifndef XMLSEC_NO_X509
static X509*		xmlSecSymbianCryptoAppCertLoadBIO		(BIO* bio,
								 xmlSecKeyDataFormat format);

/**
 * xmlSecSymbianCryptoAppKeyCertLoad:
 * @key:		the pointer to key.
 * @filename:		the certificate filename.
 * @format:		the certificate file format.
 *
 * Reads the certificate from $@filename and adds it to key
 * (not implemented yet).
 * 
 * Returns 0 on success or a negative value otherwise.
 */
EXPORT_C
int		
xmlSecSymbianCryptoAppKeyCertLoad(xmlSecKeyPtr key, const char* filename, 
			  xmlSecKeyDataFormat format) {
	BIO* bio(NULL);
	int ret(-1);
	
    xmlSecAssert2(key, -1);
    xmlSecAssert2(filename, -1);
    xmlSecAssert2(format != xmlSecKeyDataFormatUnknown, -1);
    
    /* Implementation from OpenSSL */
    bio = BIO_new_file(filename, "rb", NULL);
    if(!bio) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "BIO_new_file",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    "filename=%s", 
		    xmlSecErrorsSafeString(filename));
	return(-1);    
    }

    ret = xmlSecSymbianCryptoAppKeyCertLoadBIO (key, bio, format);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecSymbianCryptoAppKeyCertLoadBIO",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "filename=%s", 
		    xmlSecErrorsSafeString(filename));
	BIO_free(bio);
	return(-1);
    }
    
    BIO_free(bio);
    return(0);
    
}

/**
 * xmlSecSymbianCryptoAppKeyCertLoadBIO:
 * @key:		the pointer to key.
 * @bio:		the certificate bio.
 * @format:		the certificate file format.
 *
 * Reads the certificate from memory buffer and adds it to key.
 * 
 * Returns 0 on success or a negative value otherwise.
 */
EXPORT_C
int		
xmlSecSymbianCryptoAppKeyCertLoadBIO(xmlSecKeyPtr key, BIO* bio, xmlSecKeyDataFormat format) {

    xmlSecKeyDataFormat certFormat;
    xmlSecKeyDataPtr data;
    X509* cert(NULL);
    int ret(-1);
    
    xmlSecAssert2(key, -1);
    xmlSecAssert2(bio, -1);
    xmlSecAssert2(format != xmlSecKeyDataFormatUnknown, -1);
    
    data = xmlSecKeyEnsureData(key, xmlSecSymbianCryptoKeyDataX509Id);
    if(!data) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecKeyEnsureData",		    
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "transform=%s",
		    xmlSecErrorsSafeString(xmlSecTransformKlassGetName(xmlSecSymbianCryptoKeyDataX509Id)));
	return(-1);
    }

    /* adjust cert format */
    switch(format) {
    case xmlSecKeyDataFormatPkcs8Pem:
	certFormat = xmlSecKeyDataFormatPem;
	break;
    case xmlSecKeyDataFormatPkcs8Der:
	certFormat = xmlSecKeyDataFormatDer;
	break;
    default:
	certFormat = format;
    }

    cert = xmlSecSymbianCryptoAppCertLoadBIO(bio, certFormat);
    if(!cert) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecSymbianCryptoAppCertLoad", 
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);    
    }    	
    
    ret = xmlSecSymbianCryptoKeyDataX509AdoptKeyCert(data, cert);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecSymbianCryptoKeyDataX509AdoptCert",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "data=%s",
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)));
	X509_free(cert);
	return(-1);    
    }
    
    return(0);        
}


/**
 * xmlSecSymbianCryptoAppKeyCertLoadMemory:
 * @key:		the pointer to key.
 * @data:		the certificate binary data.
 * @dataSize:		the certificate binary data size.
 * @format:		the certificate file format.
 *
 * Reads the certificate from memory buffer and adds it to key (not implemented yet).
 * 
 * Returns 0 on success or a negative value otherwise.
 */
EXPORT_C
int		
xmlSecSymbianCryptoAppKeyCertLoadMemory(xmlSecKeyPtr key, 
                                    const xmlSecByte* data, 
                                    xmlSecSize dataSize, 
			        xmlSecKeyDataFormat format) {
    xmlSecAssert2(key, -1);
    xmlSecAssert2(data, -1);
    xmlSecAssert2(format != xmlSecKeyDataFormatUnknown, -1);
    int ret(-1);
    BIO* bio(NULL);
    
    bio = BIO_new_buffer((const char*)data,dataSize,NULL);
    if(!bio) {
	    return(-1);    
    }
    
    ret = xmlSecSymbianCryptoAppKeyCertLoadBIO (key, bio, format);
    BIO_free(bio);
    if(ret < 0) {
	    return(-1);
    }
    return(0);
}

/**
 * xmlSecSymbianCryptoAppPEMReadPrivateKey:
 * @filename:		the PEM key filename.
 * @pwd:		the PEM file password.
 * @pwdCallback:	the password callback.
 * @pwdCallbackCtx:	the user context for password callback.
 *
 * Reads key and all associated certificates from the PKCS12 file
 * (not implemented yet).
 * For uniformity, call xmlSecSymbianCryptoAppKeyLoad instead of this function. Pass
 * in format=xmlSecKeyDataFormatPkcs12.
 *
 * Returns pointer to the key or NULL if an error occurs.
 */
xmlSecKeyPtr	
xmlSecSymbianCryptoAppPEMReadPrivateKey(const char* filename, 
			  const char* pwd ATTRIBUTE_UNUSED,
		          void* pwdCallback ATTRIBUTE_UNUSED, 
			  void* pwdCallbackCtx ATTRIBUTE_UNUSED) {
    xmlSecAssert2(filename, NULL);

    /* Unimplemented - for futher use */
    xmlSecError(XMLSEC_ERRORS_HERE,
		NULL,
		"xmlSecSymbianCryptoAppPEMReadPrivateKey",
		XMLSEC_ERRORS_R_NOT_IMPLEMENTED,
		XMLSEC_ERRORS_NO_MESSAGE);
    return(NULL); 
}


/**
 * xmlSecSymbianCryptoAppPkcs12Load:
 * @filename:		the PKCS12 key filename.
 * @pwd:		the PKCS12 file password.
 * @pwdCallback:	the password callback.
 * @pwdCallbackCtx:	the user context for password callback.
 *
 * Reads key and all associated certificates from the PKCS12 file
 * (not implemented yet).
 * For uniformity, call xmlSecSymbianCryptoAppKeyLoad instead of this function. Pass
 * in format=xmlSecKeyDataFormatPkcs12.
 *
 * Returns pointer to the key or NULL if an error occurs.
 */
EXPORT_C
xmlSecKeyPtr	
xmlSecSymbianCryptoAppPkcs12Load(const char* filename, 
			  const char* pwd ATTRIBUTE_UNUSED,
		          void* pwdCallback ATTRIBUTE_UNUSED, 
			  void* pwdCallbackCtx ATTRIBUTE_UNUSED) {
    xmlSecAssert2(filename, NULL);

    /* Unimplemented - for futher use */
    xmlSecError(XMLSEC_ERRORS_HERE,
		NULL,
		"xmlSecSymbianCryptoAppPkcs12Load",
		XMLSEC_ERRORS_R_NOT_IMPLEMENTED,
		XMLSEC_ERRORS_NO_MESSAGE);
    return(NULL); 
}

/**
 * xmlSecSymbianCryptoAppPkcs12LoadMemory:
 * @data:		the PKCS12 binary data.
 * @dataSize:		the PKCS12 binary data size.
 * @pwd:		the PKCS12 file password.
 * @pwdCallback:	the password callback.
 * @pwdCallbackCtx:	the user context for password callback.
 *
 * Reads key and all associated certificates from the PKCS12 data in memory buffer.
 * For uniformity, call xmlSecSymbianCryptoAppKeyLoadMemory instead of this function. Pass
 * in format=xmlSecKeyDataFormatPkcs12 (not implemented yet).
 *
 * Returns pointer to the key or NULL if an error occurs.
 */
EXPORT_C
xmlSecKeyPtr	
xmlSecSymbianCryptoAppPkcs12LoadMemory(const xmlSecByte* data, xmlSecSize dataSize, 
			   const char* pwd, void* pwdCallback, 
			   void* pwdCallbackCtx) {
    xmlSecAssert2(data, NULL);
    /* Unimplemented - for futher use */
    xmlSecError(XMLSEC_ERRORS_HERE,
		NULL,
		"xmlSecSymbianCryptoAppPkcs12LoadMemory",
		XMLSEC_ERRORS_R_NOT_IMPLEMENTED,
		XMLSEC_ERRORS_NO_MESSAGE);
    return(NULL); 
}

/**
 * xmlSecSymbianCryptoAppKeysMngrCertLoad:
 * @mngr: 		the keys manager.
 * @filename: 		the certificate file.
 * @format:		the certificate file format.
 * @type: 		the flag that indicates is the certificate in @filename
 *    			trusted or not.
 * 
 * Reads cert from @filename and adds to the list of trusted or known
 * untrusted certs in @store (not implemented yet).
 *
 * Returns 0 on success or a negative value otherwise.
 */
EXPORT_C
int
xmlSecSymbianCryptoAppKeysMngrCertLoad(xmlSecKeysMngrPtr mngr, const char *filename, 
				xmlSecKeyDataFormat format, 
				xmlSecKeyDataType type ATTRIBUTE_UNUSED) {
	BIO* bio(NULL);
	int ret(-1);
					
    xmlSecAssert2(mngr, -1);
    xmlSecAssert2(filename, -1);
    xmlSecAssert2(format != xmlSecKeyDataFormatUnknown, -1);

    /* Implementation from OpenSSL */
    bio = BIO_new_file(filename, "rb", NULL);
    if(!bio) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "BIO_new_file",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    "filename=%s", 
		    xmlSecErrorsSafeString(filename));
	return(-1);    
    }

    ret = xmlSecSymbianCryptoAppKeysMngrCertLoadBIO(mngr, bio, format, type);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecSymbianCryptoAppKeysMngrCertLoadBIO",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "filename=%s", 
		    xmlSecErrorsSafeString(filename));
	BIO_free(bio);
	return(-1);
    }
    
    BIO_free(bio);    
    return(0);
}

/**
 * xmlSecSymbianCryptoAppKeysMngrCertLoadMemory:
 * @mngr: 		the keys manager.
 * @data:		the certificate binary data.
 * @dataSize:		the certificate binary data size.
 * @format:		the certificate file format.
 * @type: 		the flag that indicates is the certificate trusted or not.
 * 
 * Reads cert from binary buffer @data and adds to the list of trusted or known
 * untrusted certs in @store (not implemented yet).
 *
 * Returns 0 on success or a negative value otherwise.
 */
EXPORT_C
int
xmlSecSymbianCryptoAppKeysMngrCertLoadMemory(xmlSecKeysMngrPtr mngr, const xmlSecByte* data,
				    xmlSecSize dataSize, xmlSecKeyDataFormat format, 
				    xmlSecKeyDataType type) {
    xmlSecAssert2(mngr, -1);
    xmlSecAssert2(data, -1);
    xmlSecAssert2(format != xmlSecKeyDataFormatUnknown, -1);

	BIO* bio(NULL);
	int ret(-1);

    bio = BIO_new_buffer((const char*)data,dataSize, NULL);
    if(!bio) {
    	return(-1);    
    }

    ret = xmlSecSymbianCryptoAppKeysMngrCertLoadBIO(mngr, bio, format, type);
    BIO_free(bio);
    if(ret < 0) {
    	return(-1);
    }
    return(0);
}

static X509*	
xmlSecSymbianCryptoAppCertLoadBIO(BIO* bio, xmlSecKeyDataFormat format) {
    X509 *cert;
    
    xmlSecAssert2(bio, NULL);
    xmlSecAssert2(format != xmlSecKeyDataFormatUnknown, NULL);

    switch(format) {
    case xmlSecKeyDataFormatDer:
    case xmlSecKeyDataFormatCertDer:
	cert = d2i_X509_bio(bio);
	if(!cert) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			NULL,
			"d2i_X509_bio",
			XMLSEC_ERRORS_R_CRYPTO_FAILED,
			XMLSEC_ERRORS_NO_MESSAGE);
	    return(NULL);    
	}
	break;
    default:
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    NULL,
		    XMLSEC_ERRORS_R_INVALID_FORMAT,
		    "format=%d", format); 
	return(NULL);
    }
        	
    return(cert);
}


/**
 * xmlSecSymbianCryptoAppKeysMngrCertLoadBIO:
 * @mngr: 		the keys manager.
 * @bio: 		the certificate BIO.
 * @format:		the certificate file format.
 * @type: 		the flag that indicates is the certificate trusted or not.
 * 
 * Reads cert from an SymbianCrypto BIO object and adds to the list of trusted or known
 * untrusted certs in @store.
 *
 * Returns 0 on success or a negative value otherwise.
 */
EXPORT_C
int
xmlSecSymbianCryptoAppKeysMngrCertLoadBIO(xmlSecKeysMngrPtr mngr, BIO* bio, 
				    xmlSecKeyDataFormat format, xmlSecKeyDataType type) {
    xmlSecKeyDataStorePtr x509Store;
    X509* cert(NULL);
    int ret(-1);

    xmlSecAssert2(mngr, -1);
    xmlSecAssert2(bio, -1);
    xmlSecAssert2(format != xmlSecKeyDataFormatUnknown, -1);
    
    x509Store = xmlSecKeysMngrGetDataStore(mngr, xmlSecSymbianCryptoX509StoreId);
    if(!x509Store) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecKeysMngrGetDataStore",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "xmlSecSymbianCryptoX509StoreId");
	return(-1);
    }

    cert = xmlSecSymbianCryptoAppCertLoadBIO(bio, format);
    if(!cert) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecSymbianCryptoAppCertLoadBIO",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);    
    }    	
    
    ret = xmlSecSymbianCryptoX509StoreAdoptCert(x509Store, cert, type);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecSymbianCryptoX509StoreAdoptCert",		    
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	X509_free(cert);
	return(-1);    
    }
    
    return(0);
}

#endif /* XMLSEC_NO_X509 */

/**
 * xmlSecSymbianCryptoAppDefaultKeysMngrInit:
 * @mngr: 		the pointer to keys manager.
 *
 * Initializes @mngr with simple keys store #xmlSecSimpleKeysStoreId
 * and a default SymbianCrypto crypto key data stores.
 *
 * Returns 0 on success or a negative value otherwise.
 */
EXPORT_C 
int
xmlSecSymbianCryptoAppDefaultKeysMngrInit(xmlSecKeysMngrPtr mngr) {
    int ret(-1);
    
    xmlSecAssert2(mngr, -1);

    /* create simple keys store if needed */        
    if(!xmlSecKeysMngrGetKeysStore(mngr)) {
	xmlSecKeyStorePtr keysStore;

        keysStore = xmlSecKeyStoreCreate(xmlSecSymbianKeysStoreId);
	if(!keysStore) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			NULL,
			"xmlSecKeyStoreCreate",
			XMLSEC_ERRORS_R_XMLSEC_FAILED,
			"xmlSecSimpleKeysStoreId");
	    return(-1);
	}
	
	ret = xmlSecKeysMngrAdoptKeysStore(mngr, keysStore);
	if(ret < 0) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			NULL,
			"xmlSecKeysMngrAdoptKeysStore",
			XMLSEC_ERRORS_R_XMLSEC_FAILED,
			XMLSEC_ERRORS_NO_MESSAGE);
	    xmlSecKeyStoreDestroy(keysStore);
	    return(-1);        
	}
    }

    ret = xmlSecSymbianCryptoKeysMngrInit(mngr);    
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecSymbianCryptoKeysMngrInit",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1); 
    }
    
    mngr->getKey = xmlSecKeysMngrGetKey;
    return(0);
}

/**
 * xmlSecSymbianCryptoAppDefaultKeysMngrAdoptKey:
 * @mngr: 		the pointer to keys manager.
 * @key:		the pointer to key.
 *
 * Adds @key to the keys manager @mngr created with #xmlSecSymbianCryptoAppDefaultKeysMngrInit
 * function.
 *  
 * Returns 0 on success or a negative value otherwise.
 */
EXPORT_C 
int 
xmlSecSymbianCryptoAppDefaultKeysMngrAdoptKey(xmlSecKeysMngrPtr mngr, xmlSecKeyPtr key) {
    xmlSecKeyStorePtr store;
    int ret(-1);
    
    xmlSecAssert2(mngr, -1);
    xmlSecAssert2(key, -1);
    
    store = xmlSecKeysMngrGetKeysStore(mngr);
    if(!store) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecKeysMngrGetKeysStore",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }
    
    ret = xmlSecSimpleKeysStoreAdoptKey(store, key);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecSimpleKeysStoreAdoptKey",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }
    
    return(0);
}

/**
 * xmlSecSymbianCryptoAppDefaultKeysMngrLoad:
 * @mngr: 		the pointer to keys manager.
 * @uri:		the uri.
 *
 * Loads XML keys file from @uri to the keys manager @mngr created 
 * with #xmlSecSymbianCryptoAppDefaultKeysMngrInit function.
 *  
 * Returns 0 on success or a negative value otherwise.
 */
EXPORT_C 
int 
xmlSecSymbianCryptoAppDefaultKeysMngrLoad(xmlSecKeysMngrPtr mngr, const char* uri) {
    xmlSecKeyStorePtr store;
    int ret(-1);
    
    xmlSecAssert2(mngr, -1);
    xmlSecAssert2(uri, -1);
    
    store = xmlSecKeysMngrGetKeysStore(mngr);
    if(!store) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecKeysMngrGetKeysStore",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }
    
    ret = xmlSecSimpleKeysStoreLoad(store, uri, mngr);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecSimpleKeysStoreLoad",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "uri=%s", xmlSecErrorsSafeString(uri));
	return(-1);
    }
    
    return(0);
}

/**
 * xmlSecSymbianCryptoAppDefaultKeysMngrSave:
 * @mngr: 		the pointer to keys manager.
 * @filename:		the destination filename.
 * @type:		the type of keys to save (public/private/symmetric).
 *
 * Saves keys from @mngr to  XML keys file.
 *  
 * Returns 0 on success or a negative value otherwise.
 */
EXPORT_C 
int 
xmlSecSymbianCryptoAppDefaultKeysMngrSave(xmlSecKeysMngrPtr mngr, 
                                                    const char* filename, 
                                                    xmlSecKeyDataType type) {
    xmlSecKeyStorePtr store;
    int ret(-1);
    
    xmlSecAssert2(mngr, -1);
    xmlSecAssert2(filename, -1);
    
    store = xmlSecKeysMngrGetKeysStore(mngr);
    if(!store) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecKeysMngrGetKeysStore",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }
    
    ret = xmlSecSimpleKeysStoreSave(store, filename, type);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecSimpleKeysStoreSave",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "filename=%s", 
		    xmlSecErrorsSafeString(filename));
	return(-1);
    }
    
    return(0);
}

/**
 * xmlSecSymbianCryptoAppGetDefaultPwdCallback:
 *
 * Gets default password callback.
 *
 * Returns default password callback.
 */
EXPORT_C
void*
xmlSecSymbianCryptoAppGetDefaultPwdCallback(void) {
    return(NULL);
}

