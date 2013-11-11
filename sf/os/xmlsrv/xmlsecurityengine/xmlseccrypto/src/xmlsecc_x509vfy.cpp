/** 
 * XMLSec library
 *
 * X509 support
 *
 *
 * This is free software; see Copyright file in the source
 * distribution for preciese wording.
 * 
 * Copyright (C) 2002-2003 Aleksey Sanin <aleksey@aleksey.com>
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */
#include "xmlsecc_config.h"
#ifndef XMLSEC_NO_X509
#include "xmlsecc_globals.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <stdapis/libxml2/libxml2_tree.h>

#include "xmlsec_xmlsec.h"
#include "xmlsec_xmltree.h"
#include "xmlsec_keys.h"
#include "xmlsec_keyinfo.h"
#include "xmlsec_keysmngr.h"
#include "xmlsec_base64.h"
#include "xmlsec_errors.h"

#include "xmlsecc_crypto.h"
#include "xmlsecc_x509wrapper.h"
#include "xmlsecc_x509.h"
#include "xmlsec_error_flag.h"

#define XMLSEC_OPENSSL_097

/**************************************************************************
 *
 * Internal SymbianCrypto X509 store CTX
 *
 *************************************************************************/
typedef struct _xmlSecSymbianCryptoX509StoreCtx		xmlSecSymbianCryptoX509StoreCtx, 
							*xmlSecSymbianCryptoX509StoreCtxPtr;
struct _xmlSecSymbianCryptoX509StoreCtx {
    X509_STORE* 	xst;
    STACK_OF(X509)* 	untrusted;
    STACK_OF(X509_CRL)* crls;

#if !defined(XMLSEC_OPENSSL_096) && !defined(XMLSEC_OPENSSL_097)
    X509_VERIFY_PARAM * vpm;	    
#endif /* !defined(XMLSEC_OPENSSL_096) && !defined(XMLSEC_OPENSSL_097) */
};	    

/****************************************************************************
 *
 * xmlSecSymbianCryptoKeyDataStoreX509Id:
 *
 * xmlSecSymbianCryptoX509StoreCtx is located after xmlSecTransform
 *
 ***************************************************************************/
#define xmlSecSymbianCryptoX509StoreGetCtx(store) \
    ((xmlSecSymbianCryptoX509StoreCtxPtr)(((xmlSecByte*)(store)) + \
				    sizeof(xmlSecKeyDataStoreKlass)))
#define xmlSecSymbianCryptoX509StoreSize	\
    (sizeof(xmlSecKeyDataStoreKlass) + sizeof(xmlSecSymbianCryptoX509StoreCtx))
 
static int		xmlSecSymbianCryptoX509StoreInitialize	(xmlSecKeyDataStorePtr store);
static void		xmlSecSymbianCryptoX509StoreFinalize		(xmlSecKeyDataStorePtr store);

static xmlSecKeyDataStoreKlass xmlSecSymbianCryptoX509StoreKlass = {
    sizeof(xmlSecKeyDataStoreKlass),
    xmlSecSymbianCryptoX509StoreSize,

    /* data */
    xmlSecNameX509Store,			/* const xmlChar* name; */ 
        
    /* constructors/destructor */
    xmlSecSymbianCryptoX509StoreInitialize,		/* xmlSecKeyDataStoreInitializeMethod initialize; */
    xmlSecSymbianCryptoX509StoreFinalize,		/* xmlSecKeyDataStoreFinalizeMethod finalize; */

    /* reserved for the future */
    NULL,					/* void* reserved0; */
    NULL,					/* void* reserved1; */
};

/*static int		xmlSecSymbianCryptoX509VerifyCrl			(X509_STORE* xst, 
									 X509_CRL *crl );*/
static X509*		xmlSecSymbianCryptoX509FindCert			(STACK_OF(X509) *certs,
									 xmlChar *subjectName,
									 xmlChar *issuerName, 
									 xmlChar *issuerSerial,
									 xmlChar *ski);
/*static X509*		xmlSecSymbianCryptoX509FindNextChainCert		(STACK_OF(X509) *chain, 
		    							 X509 *cert);
static int		xmlSecSymbianCryptoX509VerifyCertAgainstCrls		(STACK_OF(X509_CRL) *crls, 
								         X509* cert);
static X509_NAME*	xmlSecSymbianCryptoX509NameRead			(xmlSecByte *str, 
									 int len);
static int 		xmlSecSymbianCryptoX509NameStringRead			(xmlSecByte **str, 
									 int *strLen, 
									 xmlSecByte *res, 
									 int resLen, 
									 xmlSecByte delim, 
									 int ingoreTrailingSpaces);
static int		xmlSecSymbianCryptoX509NamesCompare			(X509_NAME *a,
									 X509_NAME *b);
static int 		xmlSecSymbianCryptoX509_NAME_cmp			(const X509_NAME *a, 
									 const X509_NAME *b);

static int 		xmlSecSymbianCryptoX509_NAME_ENTRY_cmp		(const X509_NAME_ENTRY **a, 
									 const X509_NAME_ENTRY **b);
*/
/** 
 * xmlSecSymbianCryptoX509StoreGetKlass:
 * 
 * The SymbianCrypto X509 certificates key data store klass.
 *
 * Returns pointer to SymbianCrypto X509 certificates key data store klass.
 */
EXPORT_C
xmlSecKeyDataStoreId 
xmlSecSymbianCryptoX509StoreGetKlass(void) {
    return(&xmlSecSymbianCryptoX509StoreKlass);
}

/**
 * xmlSecSymbianCryptoX509StoreFindCert:
 * @store:		the pointer to X509 key data store klass.
 * @subjectName:	the desired certificate name.
 * @issuerName:		the desired certificate issuer name.
 * @issuerSerial:	the desired certificate issuer serial number.
 * @ski:		the desired certificate SKI.
 * @keyInfoCtx:		the pointer to <dsig:KeyInfo/> element processing context.
 *
 * Searches @store for a certificate that matches given criteria.
 *
 * Returns pointer to found certificate or NULL if certificate is not found
 * or an error occurs.
 */
EXPORT_C
X509* 
xmlSecSymbianCryptoX509StoreFindCert(xmlSecKeyDataStorePtr store, xmlChar *subjectName,
				xmlChar *issuerName, xmlChar *issuerSerial,
				xmlChar *ski, xmlSecKeyInfoCtx* keyInfoCtx) {
    xmlSecSymbianCryptoX509StoreCtxPtr ctx;
    X509* res = NULL;
    
    xmlSecAssert2(xmlSecKeyDataStoreCheckId(store, xmlSecSymbianCryptoX509StoreId), NULL);
    xmlSecAssert2(keyInfoCtx, NULL);

    ctx = xmlSecSymbianCryptoX509StoreGetCtx(store);
    xmlSecAssert2(ctx, NULL);

    if((!res) && (ctx->untrusted)) {
        res = xmlSecSymbianCryptoX509FindCert(ctx->untrusted, 
                                                            subjectName, 
                                                            issuerName, 
                                                            issuerSerial, 
                                                            ski);
    }
    return(res);
}


/**
 * xmlSecSymbianCryptoX509StoreVerify:
 * @store:		the pointer to X509 key data store klass.
 * @cert:		the untrusted key cert.
 *
 * Verifies @certs list.
 *
 * Returns Result of the verification
 */
EXPORT_C 
int 	
xmlSecSymbianCryptoX509StoreKeyCertVerify(xmlSecKeyDataStorePtr store, X509* cert) {
    xmlSecSymbianCryptoX509StoreCtxPtr ctx;
    
    
    
    int err = 0;
    
    

    xmlSecAssert2(xmlSecKeyDataStoreCheckId(store, xmlSecSymbianCryptoX509StoreId), NULL);
    xmlSecAssert2(cert, NULL);

    ctx = xmlSecSymbianCryptoX509StoreGetCtx(store);
    xmlSecAssert2(ctx, NULL);
    xmlSecAssert2(ctx->xst, NULL);
    
 if(!xmlSecCheckCertStoreFlag()) //SymbianCertStore Flag == FALSE
    {
    err = X509_STORE_certchain_init (ctx->xst, cert);
    if(err != 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataStoreGetName(store)),
		    "X509_STORE_certchain_init",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	//goto done;
	    return -1;
        }       
    err = X509_STORE_certchain_validate(ctx->xst);
	if(err != 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataStoreGetName(store)),
		    "X509_STORE_certchain_init",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	//goto done;
	    return -1;
        }	   

        return X509_STORE_certchain_getValidateResult(ctx->xst);    
    }
else    //SymbianCertStore Flag ==TRUE
    {  
        err = X509_STORE_certchain_init_fromCertStore(ctx->xst, cert);
        if(err != 0) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataStoreGetName(store)),
		    "X509_STORE_certchain_init",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	//goto done;
	    return -1;
        }
        err = X509_STORE_certchain_validate(ctx->xst);
        if(err != 0) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataStoreGetName(store)),
		    "X509_STORE_certchain_init",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	    //goto done;
	    return -1;
        }	          
        return X509_STORE_certchain_getValidateResult(ctx->xst);            
    } //else
}
			   
/**
 * xmlSecSymbianCryptoX509StoreVerify:
 * @store:		the pointer to X509 key data store klass.
 * @certs:		the untrusted certificates stack.
 * @crls:		the crls stack.
 * @keyInfoCtx:		the pointer to <dsig:KeyInfo/> element processing context.
 *
 * Verifies @certs list.
 *
 * Returns pointer to the first verified certificate from @certs.
 */
EXPORT_C 
X509* 	
xmlSecSymbianCryptoX509StoreVerify(xmlSecKeyDataStorePtr store, XMLSEC_STACK_OF_X509* certs,
			     XMLSEC_STACK_OF_X509_CRL* crls, xmlSecKeyInfoCtx* keyInfoCtx) {
    xmlSecSymbianCryptoX509StoreCtxPtr ctx;
    
    X509* res = NULL;
    X509* cert = NULL;
   
    
    int err = 0;

    int ret;

    xmlSecAssert2(xmlSecKeyDataStoreCheckId(store, xmlSecSymbianCryptoX509StoreId), NULL);
    xmlSecAssert2(certs, NULL);
    xmlSecAssert2(keyInfoCtx, NULL);

    ctx = xmlSecSymbianCryptoX509StoreGetCtx(store);
    xmlSecAssert2(ctx, NULL);
    xmlSecAssert2(ctx->xst, NULL);
    
    err = X509_STORE_certchain_init (ctx->xst, certs);
    if(err != 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataStoreGetName(store)),
		    "X509_STORE_certchain_init",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
    }
    
    
    err = X509_STORE_certchain_validate(ctx->xst);
	if(err != 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataStoreGetName(store)),
		    "X509_STORE_certchain_init",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
    }	   
    
    ret = X509_STORE_certchain_getValidateResult(ctx->xst);
	if(ret == 1) {
		res = cert;
	}    
  
#ifdef XMLSEC_FUTURE_SUPPORT  
    /* dup certs */
    /*
    certs2 = sk_X509_dup(certs);
    if(certs2 == NULL) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataStoreGetName(store)),
		    "sk_X509_dup",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	goto done;
    }
	*/
    /* add untrusted certs from the store */
#ifndef XMLSEC_NO_X509_UNTRUST    
    if(ctx->untrusted) {
	for(i = 0; i < sk_X509_num(ctx->untrusted); ++i) { 
	    ret = sk_X509_push(certs2, sk_X509_value(ctx->untrusted, i));
	    if(ret < 1) {
		xmlSecError(XMLSEC_ERRORS_HERE,
			    xmlSecErrorsSafeString(xmlSecKeyDataStoreGetName(store)),
			    "sk_X509_push",
			    XMLSEC_ERRORS_R_CRYPTO_FAILED,
			    XMLSEC_ERRORS_NO_MESSAGE);
		goto done;
	    }
	}
    }
#endif //XMLSEC_NO_X509_UNTRUST

#ifndef XMLSEC_NO_X509_CRL 
    /* dup crls but remove all non-verified */   
    if(crls) {
	crls2 = sk_X509_CRL_dup(crls);
	if(!crls2) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataStoreGetName(store)),
			"sk_X509_CRL_dup",
			XMLSEC_ERRORS_R_CRYPTO_FAILED,
			XMLSEC_ERRORS_NO_MESSAGE);
	    goto done;
	}

	for(i = 0; i < sk_X509_CRL_num(crls2); ) { 
	    ret = xmlSecSymbianCryptoX509VerifyCrl(ctx->xst, sk_X509_CRL_value(crls2, i));
	    if(ret == 1) {
		++i;
	    } else if(ret == 0) {
		sk_X509_CRL_delete(crls2, i);
	    } else {
		xmlSecError(XMLSEC_ERRORS_HERE,
			    xmlSecErrorsSafeString(xmlSecKeyDataStoreGetName(store)),
			    "xmlSecSymbianCryptoX509VerifyCrl",
			    XMLSEC_ERRORS_R_XMLSEC_FAILED,
			    XMLSEC_ERRORS_NO_MESSAGE);
		goto done;
	    }
	}	
    }
    
    /* remove all revoked certs */
    for(i = 0; i < sk_X509_num(certs2);) { 
	cert = sk_X509_value(certs2, i);

	if(crls2) {
	    ret = xmlSecSymbianCryptoX509VerifyCertAgainstCrls(crls2, cert);
	    if(ret == 0) {
		sk_X509_delete(certs2, i);
		continue;
	    } else if(ret != 1) {
		xmlSecError(XMLSEC_ERRORS_HERE,
			    xmlSecErrorsSafeString(xmlSecKeyDataStoreGetName(store)),
			    "xmlSecSymbianCryptoX509VerifyCertAgainstCrls",
			    XMLSEC_ERRORS_R_XMLSEC_FAILED,
			    XMLSEC_ERRORS_NO_MESSAGE);
		goto done;
	    }
	}	    	    

	if(ctx->crls) {
	    ret = xmlSecSymbianCryptoX509VerifyCertAgainstCrls(ctx->crls, cert);
	    if(ret == 0) {
		sk_X509_delete(certs2, i);
		continue;
	    } else if(ret != 1) {
		xmlSecError(XMLSEC_ERRORS_HERE,
			    xmlSecErrorsSafeString(xmlSecKeyDataStoreGetName(store)),
			    "xmlSecSymbianCryptoX509VerifyCertAgainstCrls",
			    XMLSEC_ERRORS_R_XMLSEC_FAILED,
			    XMLSEC_ERRORS_NO_MESSAGE);
		goto done;
	    }
	}
	++i;
    }	

#endif //XMLSEC_NO_X509_CRL

    /* get one cert after another and try to verify */
    for(i = 0; i < sk_X509_num(certs2); ++i) { 
	cert = sk_X509_value(certs2, i);
	if(!xmlSecSymbianCryptoX509FindNextChainCert(certs2, cert)) {
	    X509_STORE_CTX xsc; 

#if !defined(XMLSEC_OPENSSL_096) && !defined(XMLSEC_OPENSSL_097)
	    X509_VERIFY_PARAM * vpm = NULL;	    
	    unsigned long vpm_flags = 0;

	    vpm = X509_VERIFY_PARAM_new();
	    if(!vpm) {
		xmlSecError(XMLSEC_ERRORS_HERE,
			    xmlSecErrorsSafeString(xmlSecKeyDataStoreGetName(store)),
			    "X509_VERIFY_PARAM_new",
		    	    XMLSEC_ERRORS_R_CRYPTO_FAILED,
			    XMLSEC_ERRORS_NO_MESSAGE);
	        goto done;
	    }
	    vpm_flags = vpm->flags;
/*
	    vpm_flags &= (~X509_V_FLAG_X509_STRICT);
*/
	    vpm_flags &= (~X509_V_FLAG_CRL_CHECK);

	    X509_VERIFY_PARAM_set_depth(vpm, 9);  
	    X509_VERIFY_PARAM_set_flags(vpm, vpm_flags);
#endif /* !defined(XMLSEC_OPENSSL_096) && !defined(XMLSEC_OPENSSL_097) */
    

	    X509_STORE_CTX_init (&xsc, ctx->xst, cert, certs2);

	    if(keyInfoCtx->certsVerificationTime > 0) {	
#if !defined(XMLSEC_OPENSSL_096) && !defined(XMLSEC_OPENSSL_097)
		vpm_flags |= X509_V_FLAG_USE_CHECK_TIME;
	        X509_VERIFY_PARAM_set_time(vpm, keyInfoCtx->certsVerificationTime);
#endif /* !defined(XMLSEC_OPENSSL_096) && !defined(XMLSEC_OPENSSL_097) */
		X509_STORE_CTX_set_time(&xsc, 0, keyInfoCtx->certsVerificationTime);
	    }

#if !defined(XMLSEC_OPENSSL_096) && !defined(XMLSEC_OPENSSL_097)
	    X509_STORE_CTX_set0_param(&xsc, vpm);	    
#endif /* !defined(XMLSEC_OPENSSL_096) && !defined(XMLSEC_OPENSSL_097) */

	    
	    ret 	= X509_verify_cert(&xsc); 
	    err_cert 	= X509_STORE_CTX_get_current_cert(&xsc);
	    err	 	= X509_STORE_CTX_get_error(&xsc);
	    depth	= X509_STORE_CTX_get_error_depth(&xsc);
	    
	    X509_STORE_CTX_cleanup (&xsc);  
	    
	    if(ret == 1) {
		res = cert;
		goto done;
	    } else if(ret < 0) {
		const char* err_msg;
		
		buf[0] = '\0';
	        X509_NAME_oneline(X509_get_subject_name(err_cert), buf, sizeof buf);
		err_msg = X509_verify_cert_error_string(err);
		xmlSecError(XMLSEC_ERRORS_HERE,
			    xmlSecErrorsSafeString(xmlSecKeyDataStoreGetName(store)),
			    "X509_verify_cert",
			    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    	    "subj=%s;err=%d;msg=%s", 
			    xmlSecErrorsSafeString(buf),
			    err, 
			    xmlSecErrorsSafeString(err_msg));
		goto done;
	    } else if(ret == 0) {
		const char* err_msg;
		
		buf[0] = '\0';
	        X509_NAME_oneline(X509_get_subject_name(err_cert), buf, sizeof buf);
		err_msg = X509_verify_cert_error_string(err);
		xmlSecError(XMLSEC_ERRORS_HERE,
			    xmlSecErrorsSafeString(xmlSecKeyDataStoreGetName(store)),
			    "X509_verify_cert",
			    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    	    "subj=%s;err=%d;msg=%s", 
			    xmlSecErrorsSafeString(buf),
			    err, 
			    xmlSecErrorsSafeString(err_msg));
	    }
	}
    }

    /* if we came here then we found nothing. do we have any error? */
    if((err != 0) && (err_cert)) {
	const char* err_msg;

	err_msg = X509_verify_cert_error_string(err);
	switch (err) {
	case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT:
	    X509_NAME_oneline(X509_get_issuer_name(err_cert), buf, sizeof buf);
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataStoreGetName(store)),
			NULL,
			XMLSEC_ERRORS_R_CERT_ISSUER_FAILED,
		        "err=%d;msg=%s;issuer=%s", 
			err, 
			xmlSecErrorsSafeString(err_msg),
			xmlSecErrorsSafeString(buf));
	    break;
	case X509_V_ERR_CERT_NOT_YET_VALID:
	case X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD:
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataStoreGetName(store)),
			NULL,
			XMLSEC_ERRORS_R_CERT_NOT_YET_VALID,
			"err=%d;msg=%s", err,
			xmlSecErrorsSafeString(err_msg));
	    break;
	case X509_V_ERR_CERT_HAS_EXPIRED:
	case X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD:
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataStoreGetName(store)),
			NULL,
			XMLSEC_ERRORS_R_CERT_HAS_EXPIRED,
			"err=%d;msg=%s", err,
			xmlSecErrorsSafeString(err_msg));
	    break;
	default:			
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataStoreGetName(store)),
			NULL,
			XMLSEC_ERRORS_R_CERT_VERIFY_FAILED,
			"err=%d;msg=%s", err,
			xmlSecErrorsSafeString(err_msg));
	}		    
    }
    
done:    

    if(certs2) {
	X509_free(certs2);
    }
    if(crls2) {
	sk_X509_CRL_free(crls2);
    }
    
#endif //XMLSEC_FUTURE_SUPPORT   
    return(res);
}

/**
 * xmlSecSymbianCryptoX509StoreAdoptCert:
 * @store:		the pointer to X509 key data store klass.
 * @cert:		the pointer to SymbianCrypto X509 certificate.
 * @type:		the certificate type (trusted/untrusted).
 *
 * Adds trusted (root) or untrusted certificate to the store.
 *
 * Returns 0 on success or a negative value if an error occurs.
 */
EXPORT_C
int 
xmlSecSymbianCryptoX509StoreAdoptCert(xmlSecKeyDataStorePtr store, 
                                                    X509* cert, 
                                                    xmlSecKeyDataType type) {
    xmlSecSymbianCryptoX509StoreCtxPtr ctx;
    int ret = -1;
    
    xmlSecAssert2(xmlSecKeyDataStoreCheckId(store, xmlSecSymbianCryptoX509StoreId), -1);
    xmlSecAssert2(cert, -1);

    ctx = xmlSecSymbianCryptoX509StoreGetCtx(store);
    xmlSecAssert2(ctx, -1);

    if((type & xmlSecKeyDataTypeTrusted) != 0) {
        xmlSecAssert2(ctx->xst, -1);

        ret = X509_STORE_add_cert(ctx->xst, cert);
        if(ret != 0) {
            xmlSecError(XMLSEC_ERRORS_HERE,
                        xmlSecErrorsSafeString(xmlSecKeyDataStoreGetName(store)),
                        "X509_STORE_add_cert",
                        XMLSEC_ERRORS_R_CRYPTO_FAILED,
                        XMLSEC_ERRORS_NO_MESSAGE);
            xmlSecSetErrorFlag( ret );
            return(-1);
        }
        /* add cert increments the reference */
        X509_free(cert);
    } else {
	xmlSecAssert2(ctx->untrusted, -1);

	if(ret < 1) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataStoreGetName(store)),
			"sk_X509_push",
			XMLSEC_ERRORS_R_CRYPTO_FAILED,
			XMLSEC_ERRORS_NO_MESSAGE);
	    return(-1);
	}
    }
    return(0);
}

/**
 * xmlSecSymbianCryptoX509StoreAddCertsPath:
 * @store: the pointer to SymbianCrypto x509 store.
 * @path: the path to the certs dir.
 *
 * Adds all certs in the @path to the list of trusted certs
 * in @store.
 *
 * Returns 0 on success or a negative value otherwise.
 */
EXPORT_C
int 
xmlSecSymbianCryptoX509StoreAddCertsPath(xmlSecKeyDataStorePtr store, const char *path) {
    xmlSecSymbianCryptoX509StoreCtxPtr ctx;

    xmlSecAssert2(xmlSecKeyDataStoreCheckId(store, xmlSecSymbianCryptoX509StoreId), -1);
    xmlSecAssert2(path, -1);

    ctx = xmlSecSymbianCryptoX509StoreGetCtx(store);
    xmlSecAssert2(ctx, -1);
    xmlSecAssert2(ctx->xst, -1);
    /*
    lookup = X509_STORE_add_lookup(ctx->xst, X509_LOOKUP_hash_dir());
    if(lookup == NULL) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataStoreGetName(store)),
		    "X509_STORE_add_lookup",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }    
    X509_LOOKUP_add_dir(lookup, path, X509_FILETYPE_DEFAULT);
    */
    return(0);
}

static int
xmlSecSymbianCryptoX509StoreInitialize(xmlSecKeyDataStorePtr store) {
    
    
    xmlSecSymbianCryptoX509StoreCtxPtr ctx;
    xmlSecAssert2(xmlSecKeyDataStoreCheckId(store, xmlSecSymbianCryptoX509StoreId), -1);

    ctx = xmlSecSymbianCryptoX509StoreGetCtx(store);
    xmlSecAssert2(ctx, -1);

    memset(ctx, 0, sizeof(xmlSecSymbianCryptoX509StoreCtx));

    ctx->xst = X509_STORE_new();
    if(!ctx->xst) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataStoreGetName(store)),
		    "X509_STORE_new",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }
#ifdef XMLSEC_FUTURE_SUPPORT    
    if(!X509_STORE_set_default_paths(ctx->xst)) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataStoreGetName(store)),
		    "X509_STORE_set_default_paths",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }
    
    path = xmlSecSymbianCryptoGetDefaultTrustedCertsFolder();
    if(path) {
	X509_LOOKUP *lookup = NULL;
	
	lookup = X509_STORE_add_lookup(ctx->xst, X509_LOOKUP_hash_dir());
        if(!lookup) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataStoreGetName(store)),
		    "X509_STORE_add_lookup",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	    return(-1);
	}    
	X509_LOOKUP_add_dir(lookup, (char*)path, X509_FILETYPE_DEFAULT);
    }

    ctx->untrusted = sk_X509_new_null();
    if(!ctx->untrusted) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataStoreGetName(store)),
		    "sk_X509_new_null",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }    

    ctx->crls = sk_X509_CRL_new_null();
    if(!ctx->crls) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataStoreGetName(store)),
		    "sk_X509_CRL_new_null",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }    
    
#if !defined(XMLSEC_OPENSSL_096) && !defined(XMLSEC_OPENSSL_097)
    ctx->vpm = X509_VERIFY_PARAM_new();
    if(!ctx->vpm) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataStoreGetName(store)),
		    "X509_VERIFY_PARAM_new",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }    
    X509_VERIFY_PARAM_set_depth(ctx->vpm, 9); /* the default cert verification path in openssl */	
    X509_STORE_set1_param(ctx->xst, ctx->vpm);
    
#else  /* !defined(XMLSEC_OPENSSL_096) && !defined(XMLSEC_OPENSSL_097) */
    ctx->xst->depth = 9; /* the default cert verification path in openssl */	
#endif /* !defined(XMLSEC_OPENSSL_096) && !defined(XMLSEC_OPENSSL_097) */

#endif	//XMLSEC_FUTURE_SUPPORT
    return(0);    
}

static void
xmlSecSymbianCryptoX509StoreFinalize(xmlSecKeyDataStorePtr store) {
    xmlSecSymbianCryptoX509StoreCtxPtr ctx;
    xmlSecAssert(xmlSecKeyDataStoreCheckId(store, xmlSecSymbianCryptoX509StoreId));

    ctx = xmlSecSymbianCryptoX509StoreGetCtx(store);
    xmlSecAssert(ctx);
    
    if(ctx->xst) {
	X509_STORE_free(ctx->xst);
    }
    if(ctx->untrusted) {
    }
    if(ctx->crls) {
    }
#if !defined(XMLSEC_OPENSSL_096) && !defined(XMLSEC_OPENSSL_097)
    if(ctx->vpm) {
	X509_VERIFY_PARAM_free(ctx->vpm);
    }
#endif /* !defined(XMLSEC_OPENSSL_096) && !defined(XMLSEC_OPENSSL_097) */
    memset(ctx, 0, sizeof(xmlSecSymbianCryptoX509StoreCtx));
}


/*****************************************************************************
 *
 * Low-level x509 functions
 *
 *****************************************************************************/
/*static int
xmlSecSymbianCryptoX509VerifyCrl(X509_STORE* xst, X509_CRL *crl ) {

    EVP_PKEY *pkey;
    int ret = 0;  
#ifdef XMLSEC_FUTURE_SUPPORT   
    xmlSecAssert2(xst, -1);
    xmlSecAssert2(crl, -1);
 
    X509_STORE_CTX_init(&xsc, xst, NULL, NULL);
    ret = X509_STORE_get_by_subject(&xsc, X509_LU_X509, 
				    X509_CRL_get_issuer(crl), &xobj);
    if(ret <= 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "X509_STORE_get_by_subject",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }
    pkey = X509_get_pubkey(xobj.data.x509);
    X509_OBJECT_free_contents(&xobj);
    if(!pkey) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "X509_get_pubkey",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }
    ret = X509_CRL_verify(crl, pkey);
    EVP_PKEY_free(pkey);    
    if(ret != 1) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "X509_CRL_verify",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
    }
    X509_STORE_CTX_cleanup (&xsc);  
#endif		//XMLSEC_FUTURE_SUPPORT    
    return((ret == 1) ? 1 : 0);
}
*/

/**
 * xmlSecSymbianCryptoX509FindCert:
 */
static X509*		
xmlSecSymbianCryptoX509FindCert(STACK_OF(X509) *certs, xmlChar *subjectName,
			xmlChar *issuerName, xmlChar *issuerSerial,
			xmlChar *ski) {

    

    xmlSecAssert2(certs, NULL);
#ifdef XMLSEC_FUTURE_SUPPORT    
    /* may be this is not the fastest way to search certs */
    if(subjectName) {
	X509_NAME *nm;
	X509_NAME *subj;

	nm = xmlSecSymbianCryptoX509NameRead(subjectName, xmlStrlen(subjectName));
	if(!nm) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			NULL,
			"xmlSecSymbianCryptoX509NameRead",
			XMLSEC_ERRORS_R_XMLSEC_FAILED,
			"subject=%s", 
			xmlSecErrorsSafeString(subjectName));
	    return(NULL);    
	}

	for(i = 0; i < certs->num; ++i) {
	    cert = ((X509**)(certs->data))[i];
	    subj = X509_get_subject_name(cert);
	    if(xmlSecSymbianCryptoX509NamesCompare(nm, subj) == 0) {
		X509_NAME_free(nm);
		return(cert);
	    }	    
	}
	X509_NAME_free(nm);
    } else if((issuerName) && (issuerSerial)) {
	X509_NAME *nm;
	X509_NAME *issuer;
	BIGNUM *bn;
	ASN1_INTEGER *serial;

	nm = xmlSecSymbianCryptoX509NameRead(issuerName, xmlStrlen(issuerName));
	if(!nm) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			NULL,
			"xmlSecSymbianCryptoX509NameRead",
			XMLSEC_ERRORS_R_XMLSEC_FAILED,
			"issuer=%s", 
			xmlSecErrorsSafeString(issuerName));
	    return(NULL);    
	}
		
	bn = BN_new();
	if(!bn) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			NULL,
			"BN_new",
			XMLSEC_ERRORS_R_CRYPTO_FAILED,
			XMLSEC_ERRORS_NO_MESSAGE);
	    X509_NAME_free(nm);
	    return(NULL);    
	}
	if(BN_dec2bn(&bn, (char*)issuerSerial) == 0) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			NULL,
			"BN_dec2bn",
			XMLSEC_ERRORS_R_CRYPTO_FAILED,
			XMLSEC_ERRORS_NO_MESSAGE);
	    BN_free(bn);
	    X509_NAME_free(nm);
	    return(NULL);    
	}
	
	serial = BN_to_ASN1_INTEGER(bn, NULL);
	if(!serial) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			NULL,
			"BN_to_ASN1_INTEGER",
			XMLSEC_ERRORS_R_CRYPTO_FAILED,
			XMLSEC_ERRORS_NO_MESSAGE);
	    BN_free(bn);
	    X509_NAME_free(nm);
	    return(NULL);    
	}
	BN_free(bn); 


	for(i = 0; i < certs->num; ++i) {
	    cert = ((X509**)(certs->data))[i];
	    if(ASN1_INTEGER_cmp(X509_get_serialNumber(cert), serial) != 0) {
		continue;
	    } 
	    issuer = X509_get_issuer_name(cert);
	    if(xmlSecSymbianCryptoX509NamesCompare(nm, issuer) == 0) {
		ASN1_INTEGER_free(serial);
		X509_NAME_free(nm);
		return(cert);
	    }	    
	}

        X509_NAME_free(nm);
	ASN1_INTEGER_free(serial);
    } else if(ski) {
	int len;
	int index;
	X509_EXTENSION *ext;
	ASN1_OCTET_STRING *keyId;
	
	/* our usual trick with base64 decode */
	len = xmlSecBase64Decode(ski, (xmlSecByte*)ski, xmlStrlen(ski));
	if(len < 0) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			NULL,
			"xmlSecBase64Decode",
			XMLSEC_ERRORS_R_XMLSEC_FAILED,
			"ski=%s", 
			xmlSecErrorsSafeString(ski));
	    return(NULL);    	
	}
	for(i = 0; i < certs->num; ++i) {
	    cert = ((X509**)(certs->data))[i];
	    index = X509_get_ext_by_NID(cert, NID_subject_key_identifier, -1); 
	    if(index >= 0) {
	         ext = X509_get_ext(cert, index);
	         if(ext){
		    keyId = X509V3_EXT_d2i(ext);
		    if((keyId) && (keyId->length == len) && 
				    (memcmp(keyId->data, ski, len) == 0)) {
		        M_ASN1_OCTET_STRING_free(keyId);
		        return(cert);
		        }
		M_ASN1_OCTET_STRING_free(keyId);
	        }
	    }
	}	
    }
#endif		//XMLSEC_FUTURE_SUPPORT
    return(NULL);
}

/** 
 * xmlSecSymbianCryptoX509FindNextChainCert:
 */
 /*
static X509*
xmlSecSymbianCryptoX509FindNextChainCert(STACK_OF(X509) *chain, X509 *cert) {
 
  

    xmlSecAssert2(chain, NULL);
    xmlSecAssert2(cert, NULL);
 
    certSubjHash = X509_subject_name_hash(cert);
    for(i = 0; i < sk_X509_num(chain); ++i) {
	if((sk_X509_value(chain, i) != cert) && 
	   (X509_issuer_name_hash(sk_X509_value(chain, i)) == certSubjHash)) {

	    return(sk_X509_value(chain, i));
	}
    }
  
    return(NULL);
}
*/
/**
 * xmlSecSymbianCryptoX509VerifyCertAgainstCrls:
 */
 /*
static int
xmlSecSymbianCryptoX509VerifyCertAgainstCrls(STACK_OF(X509_CRL) *crls, X509* cert) {
   
   
#ifdef XMLSEC_FUTURE_SUPPORT    
    X509_REVOKED *revoked;
    int i, n;
    int ret;  

    xmlSecAssert2(crls, -1);
    xmlSecAssert2(cert, -1);
    
    
     * Try to retrieve a CRL corresponding to the issuer of
     * the current certificate 
      
    n = sk_X509_CRL_num(crls);
    for(i = 0; i < n; i++) {
	crl = sk_X509_CRL_value(crls, i);     
	issuer = X509_CRL_get_issuer(crl);
	if(xmlSecSymbianCryptoX509NamesCompare(X509_CRL_get_issuer(crl), issuer) == 0) { 
	    break;
	}
    }
    if((i >= n) || (!crl)){
	// no crls for this issuer 
	return(1);
    }

    
     // Check date of CRL to make sure it's not expired 
     
    ret = X509_cmp_current_time(X509_CRL_get_nextUpdate(crl));
    if (ret == 0) {
	//crl expired 
	return(1);
    }
    
     
     // Check if the current certificate is revoked by this CRL
     
    n = sk_num(X509_CRL_get_REVOKED(crl));
    for (i = 0; i < n; i++) {
        revoked = (X509_REVOKED *)sk_value(X509_CRL_get_REVOKED(crl), i);
        if (ASN1_INTEGER_cmp(revoked->serialNumber, X509_get_serialNumber(cert)) == 0) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			NULL,
			NULL,
			XMLSEC_ERRORS_R_CERT_REVOKED,
			XMLSEC_ERRORS_NO_MESSAGE);
	    return(0);
        }
    }
#endif		//XMLSEC_FUTURE_SUPPORT    
    return(1);    
}
*/


/**
 * xmlSecSymbianCryptoX509NameRead:
 */  
 /*     
static X509_NAME *
xmlSecSymbianCryptoX509NameRead(xmlSecByte *str, int len) {
   
    
    
    X509_NAME *nm = NULL;

    xmlSecAssert2(str, NULL);
    
#ifdef XMLSEC_FUTURE_SUPPORT    
    nm = X509_NAME_new();
    if(!nm) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "X509_NAME_new",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(NULL);
    }
    
    while(len > 0) {
	//skip spaces after comma or semicolon 
	while((len > 0) && isspace(*str)) {
	    ++str; --len;
	}

	nameLen = xmlSecSymbianCryptoX509NameStringRead(&str, &len, name, sizeof(name), '=', 0);	
	if(nameLen < 0) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			NULL,
			"xmlSecSymbianCryptoX509NameStringRead",
			XMLSEC_ERRORS_R_XMLSEC_FAILED,
			XMLSEC_ERRORS_NO_MESSAGE);
	    X509_NAME_free(nm);
	    return(NULL);
	}
	name[nameLen] = '\0';
	if(len > 0) {
	    ++str; --len;
	    if((*str) == '\"') {
		++str; --len;
		valueLen = xmlSecSymbianCryptoX509NameStringRead(&str, &len, 
					value, sizeof(value), '"', 1);	
		if(valueLen < 0) {
		    xmlSecError(XMLSEC_ERRORS_HERE,
				NULL,
				"xmlSecSymbianCryptoX509NameStringRead",
				XMLSEC_ERRORS_R_XMLSEC_FAILED,
				XMLSEC_ERRORS_NO_MESSAGE);
		    X509_NAME_free(nm);
		    return(NULL);
    		}
		
		//skip quote 
		if((len <= 0) || ((*str) != '\"')) {
		    xmlSecError(XMLSEC_ERRORS_HERE,
				NULL,
				NULL,
				XMLSEC_ERRORS_R_INVALID_DATA,
				"quote is expected:%s",
				xmlSecErrorsSafeString(str));
		    X509_NAME_free(nm);
		    return(NULL);
		}
                ++str; --len;

		//skip spaces before comma or semicolon 
		while((len > 0) && isspace(*str)) {
		    ++str; --len;
		}
		if((len > 0) && ((*str) != ',')) {
		    xmlSecError(XMLSEC_ERRORS_HERE,
				NULL,
				NULL,
				XMLSEC_ERRORS_R_INVALID_DATA,
				"comma is expected:%s",
				xmlSecErrorsSafeString(str));
		    X509_NAME_free(nm);
		    return(NULL);
		}
		if(len > 0) {
		    ++str; --len;
		}
		type = MBSTRING_ASC;
	    } else if((*str) == '#') {
		//Not implemented currently
		xmlSecError(XMLSEC_ERRORS_HERE,
			    NULL,
			    NULL,
			    XMLSEC_ERRORS_R_INVALID_DATA,
			    "reading octect values is not implemented yet");
    	        X509_NAME_free(nm);
		return(NULL);
	    } else {
		valueLen = xmlSecSymbianCryptoX509NameStringRead(&str, &len, 
					value, sizeof(value), ',', 1);	
		if(valueLen < 0) {
		    xmlSecError(XMLSEC_ERRORS_HERE,
				NULL,
				"xmlSecSymbianCryptoX509NameStringRead",
				XMLSEC_ERRORS_R_XMLSEC_FAILED,
				XMLSEC_ERRORS_NO_MESSAGE);
    	    	    X509_NAME_free(nm);
		    return(NULL);
    		}
		type = MBSTRING_ASC;
	    } 			
	} else {
	    valueLen = 0;
	}
	value[valueLen] = '\0';
	if(len > 0) {
	    ++str; --len;
	}	
	X509_NAME_add_entry_by_txt(nm, (char*)name, type, value, valueLen, -1, 0);
    }
#endif	//XMLSEC_FUTURE_SUPPORT    
    return(nm);
}

*/

/**
 * xmlSecSymbianCryptoX509NameStringRead:
 */
 /*
static int 
xmlSecSymbianCryptoX509NameStringRead(xmlSecByte **str, int *strLen, 
			xmlSecByte *res, int resLen,
			xmlSecByte delim, int ingoreTrailingSpaces) {
    xmlSecByte *p, *q, *nonSpace; 

    xmlSecAssert2(str, -1);
    xmlSecAssert2(strLen, -1);
    xmlSecAssert2(res, -1);
    
    p = (*str);
    nonSpace = q = res;
    while(((p - (*str)) < (*strLen)) && ((*p) != delim) && ((q - res) < resLen)) { 
	if((*p) != '\\') {
	    if(ingoreTrailingSpaces && !isspace(*p)) nonSpace = q;	
	    *(q++) = *(p++);
	} else {
	    ++p;
	    nonSpace = q;    
	    if(xmlSecIsHex((*p))) {
		if((p - (*str) + 1) >= (*strLen)) {
		    xmlSecError(XMLSEC_ERRORS_HERE,
				NULL,
				NULL,
				XMLSEC_ERRORS_R_INVALID_DATA,
				"two hex digits expected");
	    	    return(-1);
		}
		*(q++) = xmlSecGetHex(p[0]) * 16 + xmlSecGetHex(p[1]);
		p += 2;
	    } else {
		if(((++p) - (*str)) >= (*strLen)) {
		    xmlSecError(XMLSEC_ERRORS_HERE,
				NULL,
				NULL,
				XMLSEC_ERRORS_R_INVALID_DATA,
				"escaped symbol missed");
		    return(-1);
		}
		*(q++) = *(p++); 
	    }
	}	    
    }
    if(((p - (*str)) < (*strLen)) && ((*p) != delim)) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    NULL,
		    XMLSEC_ERRORS_R_INVALID_SIZE,
		    "buffer is too small");
	return(-1);
    }
    (*strLen) -= (p - (*str));
    (*str) = p;
    return((ingoreTrailingSpaces) ? nonSpace - res + 1 : q - res);
}
*/

/*
static
int xmlSecSymbianCryptoX509_NAME_cmp(const X509_NAME *a, const X509_NAME *b) {
  
    
    const X509_NAME_ENTRY *na,*nb;

    xmlSecAssert2(a != NULL, -1);
    xmlSecAssert2(b != NULL, 1);
	
    if (sk_X509_NAME_ENTRY_num(a->entries) != sk_X509_NAME_ENTRY_num(b->entries)) {
	return sk_X509_NAME_ENTRY_num(a->entries) - sk_X509_NAME_ENTRY_num(b->entries);
    }
	
    for (i=sk_X509_NAME_ENTRY_num(a->entries)-1; i>=0; i--) {
	na=sk_X509_NAME_ENTRY_value(a->entries,i);
	nb=sk_X509_NAME_ENTRY_value(b->entries,i);
	
	ret = xmlSecSymbianCryptoX509_NAME_ENTRY_cmp(&na, &nb);
	if(ret != 0) {
	    return(ret);
	}
    }	

    return(0);
}
*/


/** 
 * xmlSecSymbianCryptoX509NamesCompare:
 *
 * we have to sort X509_NAME entries to get correct results.
 * This is ugly but SymbianCrypto does not support it
 */
 /*
static int		
xmlSecSymbianCryptoX509NamesCompare(X509_NAME *a, X509_NAME *b) {
    X509_NAME *a1 = NULL;
    X509_NAME *b1 = NULL;
    int ret = -1;
    
    xmlSecAssert2(a, -1);    
    xmlSecAssert2(b, 1);    
#ifdef XMLSEC_FUTURE_SUPPORT    
    a1 = X509_NAME_dup(a);
    if(!a1) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "X509_NAME_dup",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
        return(-1);
    }
    b1 = X509_NAME_dup(b);
    if(!b1) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "X509_NAME_dup",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
        return(1);
    }
        
    //sort both
    sk_X509_NAME_ENTRY_set_cmp_func(a1->entries, xmlSecSymbianCryptoX509_NAME_ENTRY_cmp);
    sk_X509_NAME_ENTRY_sort(a1->entries);
    sk_X509_NAME_ENTRY_set_cmp_func(b1->entries, xmlSecSymbianCryptoX509_NAME_ENTRY_cmp);
    sk_X509_NAME_ENTRY_sort(b1->entries);

     //actually compare
    ret = xmlSecSymbianCryptoX509_NAME_cmp(a1, b1);
    
    //cleanup
    X509_NAME_free(a1);
    X509_NAME_free(b1);
#endif		//XMLSEC_FUTURE_SUPPORT    
    return(ret);
}
		*/	

/**
 * xmlSecSymbianCryptoX509_NAME_ENTRY_cmp:
 */
#ifdef XMLSEC_FUTURE_SUPPORT
static int 
xmlSecSymbianCryptoX509_NAME_ENTRY_cmp(const X509_NAME_ENTRY **a, const X509_NAME_ENTRY **b) {
    int ret;
    
    xmlSecAssert2(a, -1);
    xmlSecAssert2(b, 1);
    xmlSecAssert2((*a), -1);
    xmlSecAssert2((*b), 1);

    /* first compare values */    
    if((!((*a)->value)) && ((*b)->value)) {
	return(-1);
    } else if(((*a)->value) && (!((*b)->value))) {
	return(1);
    } else if((!((*a)->value)) && (!((*b)->value))) {
	return(0);
    }	
    
    ret = (*a)->value->length - (*b)->value->length;
    if(ret != 0) {
	return(ret);
    }
		
    ret = memcmp((*a)->value->data, (*b)->value->data, (*a)->value->length);
    if(ret != 0) {
	return(ret);
    }

    /* next compare names */
    return(OBJ_cmp((*a)->object, (*b)->object));
}
#endif	//XMLSEC_FUTURE_SUPPORT


#endif /* XMLSEC_NO_X509 */


