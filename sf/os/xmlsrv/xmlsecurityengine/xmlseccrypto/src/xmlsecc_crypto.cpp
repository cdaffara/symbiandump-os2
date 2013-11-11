/** 
 * XMLSec library
 *
 * This is free software; see Copyright file in the source
 * distribution for preciese wording.
 * 
 * Copyright (C) 2002-2003 Aleksey Sanin <aleksey@aleksey.com>
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */
#include "xmlsecc_globals.h"

#include <e32def.h>
#include <string.h>
#include <random.h>
#include "xmlsecc_config.h"
#include "xmlsec_xmlsec.h"
#include "xmlsec_keys.h"
#include "xmlsec_transforms.h"
#include "xmlsec_errors.h"
#include "xmlsec_dl.h"
#include "xmlsec_private.h"

#include "xmlsecc_app.h"
#include "xmlsecc_crypto.h"
#include "xmlsecc_cryptowrapper.h"
#include "xmlsecc_evpwrapper.h"
#include "xmlsecc_x509.h"

static xmlSecCryptoDLFunctionsPtr gXmlSecSymbianCryptoFunctions = NULL;

/**
 * xmlSecCryptoGetFunctions_symbiancrypto:
 *
 * Gets the pointer to xmlsec-symbiancrypto functions table.
 *
 * Returns the xmlsec-symbiancrypto functions table or NULL if an error occurs.
 */
xmlSecCryptoDLFunctionsPtr
xmlSecCryptoGetFunctions_symbiancrypto(void) {
    static xmlSecCryptoDLFunctions functions;
    
    if(gXmlSecSymbianCryptoFunctions) {
	return(gXmlSecSymbianCryptoFunctions);
    }

    memset(&functions, 0, sizeof(functions));
    gXmlSecSymbianCryptoFunctions = &functions;

    /**  
     * Crypto Init/shutdown
     */
    gXmlSecSymbianCryptoFunctions->cryptoInit 			= xmlSecSymbianCryptoInit;
    gXmlSecSymbianCryptoFunctions->cryptoShutdown 		= xmlSecSymbianCryptoShutdown;
    gXmlSecSymbianCryptoFunctions->cryptoKeysMngrInit 	= xmlSecSymbianCryptoKeysMngrInit;

    /**
     * Key data ids
     */
#ifndef XMLSEC_NO_AES    
    gXmlSecSymbianCryptoFunctions->keyDataAesGetKlass	= xmlSecSymbianCryptoKeyDataAesGetKlass;
#endif /* XMLSEC_NO_AES */

#ifndef XMLSEC_NO_DES    
    gXmlSecSymbianCryptoFunctions->keyDataDesGetKlass 	= xmlSecSymbianCryptoKeyDataDesGetKlass;
#endif /* XMLSEC_NO_DES */

#ifndef XMLSEC_NO_HMAC  
    gXmlSecSymbianCryptoFunctions->keyDataHmacGetKlass 	= xmlSecSymbianCryptoKeyDataHmacGetKlass;
#endif /* XMLSEC_NO_HMAC */   

#ifndef XMLSEC_NO_RSA
	gXmlSecSymbianCryptoFunctions->keyDataRsaGetKlass 	= xmlSecSymbianCryptoKeyDataRsaGetKlass;
#endif /* XMLSEC_NO_RSA */ 

#ifndef XMLSEC_NO_X509
    gXmlSecSymbianCryptoFunctions->keyDataX509GetKlass 	 = xmlSecSymbianCryptoKeyDataX509GetKlass;
    gXmlSecSymbianCryptoFunctions->keyDataRawX509CertGetKlass 	
                                              = xmlSecSymbianCryptoKeyDataRawX509CertGetKlass;
#endif /* XMLSEC_NO_X509 */

    /**
     * Key data store ids
     */
#ifndef XMLSEC_NO_X509
    gXmlSecSymbianCryptoFunctions->x509StoreGetKlass 	= xmlSecSymbianCryptoX509StoreGetKlass;
#endif /* XMLSEC_NO_X509 */

    /**
     * Crypto transforms ids
     */
#ifndef XMLSEC_NO_AES    
    gXmlSecSymbianCryptoFunctions->transformAes128CbcGetKlass 	
                                           = xmlSecSymbianCryptoTransformAes128CbcGetKlass;
    gXmlSecSymbianCryptoFunctions->transformAes192CbcGetKlass 	
                                           = xmlSecSymbianCryptoTransformAes192CbcGetKlass;
    gXmlSecSymbianCryptoFunctions->transformAes256CbcGetKlass 	
                                           = xmlSecSymbianCryptoTransformAes256CbcGetKlass;
#endif /* XMLSEC_NO_AES */

#ifndef XMLSEC_NO_DES    
    gXmlSecSymbianCryptoFunctions->transformDes3CbcGetKlass 
                                            = xmlSecSymbianCryptoTransformDes3CbcGetKlass;
#endif /* XMLSEC_NO_DES */

#ifndef XMLSEC_NO_HMAC
    gXmlSecSymbianCryptoFunctions->transformHmacSha1GetKlass 		
                                            = xmlSecSymbianCryptoTransformHmacSha1GetKlass;
    gXmlSecSymbianCryptoFunctions->transformHmacRipemd160GetKlass 	
                                            = xmlSecSymbianCryptoTransformHmacRipemd160GetKlass;
    gXmlSecSymbianCryptoFunctions->transformHmacMd5GetKlass 		
                                            = xmlSecSymbianCryptoTransformHmacMd5GetKlass;
#endif /* XMLSEC_NO_HMAC */

#ifndef XMLSEC_NO_SHA1    
    gXmlSecSymbianCryptoFunctions->transformSha1GetKlass 
                                            = xmlSecSymbianCryptoTransformSha1GetKlass;
#endif /* XMLSEC_NO_SHA1 */


#ifndef XMLSEC_NO_RSA
    gXmlSecSymbianCryptoFunctions->transformRsaSha1GetKlass 		
                                            = xmlSecSymbianCryptoTransformRsaSha1GetKlass;
#endif /* XMLSEC_NO_RSA */




    /**
     * High level routines form xmlsec command line utility
     */ 
    gXmlSecSymbianCryptoFunctions->cryptoAppInit 			
                                                = xmlSecSymbianCryptoAppInit;
    gXmlSecSymbianCryptoFunctions->cryptoAppShutdown 			
                                                = xmlSecSymbianCryptoAppShutdown;
    gXmlSecSymbianCryptoFunctions->cryptoAppDefaultKeysMngrInit 	
                                                = xmlSecSymbianCryptoAppDefaultKeysMngrInit;
    gXmlSecSymbianCryptoFunctions->cryptoAppDefaultKeysMngrAdoptKey 	
                                                = xmlSecSymbianCryptoAppDefaultKeysMngrAdoptKey;
    gXmlSecSymbianCryptoFunctions->cryptoAppDefaultKeysMngrLoad 	
                                                = xmlSecSymbianCryptoAppDefaultKeysMngrLoad;
    gXmlSecSymbianCryptoFunctions->cryptoAppDefaultKeysMngrSave 	
                                                = xmlSecSymbianCryptoAppDefaultKeysMngrSave;
#ifndef XMLSEC_NO_X509
    gXmlSecSymbianCryptoFunctions->cryptoAppKeysMngrCertLoad 		
                                                = xmlSecSymbianCryptoAppKeysMngrCertLoad;
    gXmlSecSymbianCryptoFunctions->cryptoAppPkcs12Load  		
                                                = xmlSecSymbianCryptoAppPkcs12Load; 
    gXmlSecSymbianCryptoFunctions->cryptoAppKeyCertLoad 		
                                                = xmlSecSymbianCryptoAppKeyCertLoad;
#endif /* XMLSEC_NO_X509 */
    gXmlSecSymbianCryptoFunctions->cryptoAppKeyLoadWithName		
                                                = xmlSecSymbianCryptoAppKeyLoadWithName;     
    gXmlSecSymbianCryptoFunctions->cryptoAppDefaultPwdCallback		
                                           = (void*)xmlSecSymbianCryptoAppGetDefaultPwdCallback();

    return(gXmlSecSymbianCryptoFunctions);
}


/**
 * xmlSecSymbianCryptoInit:
 * 
 * XMLSec library specific crypto engine initialization. 
 *
 * Returns 0 on success or a negative value otherwise.
 */
EXPORT_C
int 
xmlSecSymbianCryptoInit (void)  {
    /* Check loaded xmlsec library version */
    if(xmlSecCheckVersionExact() != 1) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecCheckVersionExact",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }

    if (sc_pkey_init() != 0)
    {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "sc_pkey_init",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    
    }

    /* register our klasses */
    if(xmlSecCryptoDLFunctionsRegisterKeyDataAndTransforms(
                                                xmlSecCryptoGetFunctions_symbiancrypto()) < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecCryptoDLFunctionsRegisterKeyDataAndTransforms",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }
    
    return(0);
}

/**
 * xmlSecSymbianCryptoShutdown:
 * 
 * XMLSec library specific crypto engine shutdown. 
 *
 * Returns 0 on success or a negative value otherwise.
 */
EXPORT_C
int 
xmlSecSymbianCryptoShutdown(void) {
	sc_pkey_shutdown();
    return(0);
}

/**
 * xmlSecSymbianCryptoKeysMngrInit:
 * @mngr:		the pointer to keys manager.
 *
 * Adds SymbianCrypto specific key data stores in keys manager.
 *
 * Returns 0 on success or a negative value otherwise.
 */
EXPORT_C
int
xmlSecSymbianCryptoKeysMngrInit(xmlSecKeysMngrPtr mngr) {
    int ret;
    
    xmlSecAssert2(mngr, -1);

    /* Implementation from OpenSSL */
#ifndef XMLSEC_NO_X509
    /* create x509 store if needed */
    if(!xmlSecKeysMngrGetDataStore(mngr, xmlSecSymbianCryptoX509StoreId)) {
	xmlSecKeyDataStorePtr x509Store;

    x509Store = xmlSecKeyDataStoreCreate(xmlSecSymbianCryptoX509StoreId);
	if(!x509Store) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			NULL,
			"xmlSecKeyDataStoreCreate",
			XMLSEC_ERRORS_R_XMLSEC_FAILED,
			"xmlSecOpenSSLX509StoreId");
	    return(-1);   
	}
    
    ret = xmlSecKeysMngrAdoptDataStore(mngr, x509Store);
    if(ret < 0) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			NULL,
			"xmlSecKeysMngrAdoptDataStore",
			XMLSEC_ERRORS_R_XMLSEC_FAILED,
			XMLSEC_ERRORS_NO_MESSAGE);
	    xmlSecKeyDataStoreDestroy(x509Store);
	    return(-1); 
	}
    }
#endif /* XMLSEC_NO_X509 */        
    return(0);
}

/**
 * xmlSecSymbianCryptoGenerateRandom:
 * @buffer:		the destination buffer.
 * @size:		the numer of bytes to generate.
 *
 * Generates @size random bytes and puts result in @buffer.
 *
 * Returns 0 on success or a negative value otherwise.
 */
EXPORT_C
int
xmlSecSymbianCryptoGenerateRandom(xmlSecBufferPtr buffer, xmlSecSize size) {	
    int ret;
    
    xmlSecAssert2(buffer, -1);
    xmlSecAssert2(size > 0, -1);

    ret = xmlSecBufferSetSize(buffer, size);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE, 
		    NULL,
		    "xmlSecBufferSetSize",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "size=%d", size);
	return(-1);
    }
        
    /* get random data */
    sc_randomize(xmlSecBufferGetData(buffer), size, SC_STRONG_RANDOM); 
    /*
    CSystemRandom* rand=CSystemRandom::NewLC();
    unsigned char* bytes=xmlSecBufferGetData(buffer);
    TPtr8 ptr(bytes, xmlSecBufferGetMaxSize(buffer));
    rand->GenerateBytesL(ptr);
    CleanupStack::PopAndDestroy();
    */
    return(0);
}
