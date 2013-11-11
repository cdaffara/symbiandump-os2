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

#include <string.h>
#include "xmlsecc_config.h"
#include "xmlsec_xmlsec.h"
#include "xmlsec_xmltree.h"
#include "xmlsec_keys.h"
#include "xmlsec_keyinfo.h"
#include "xmlsec_transforms.h"
#include "xmlsec_errors.h"
#include "xmlsec_error_flag.h"

#include "xmlsecc_crypto.h"
#include "xmlsecc_cryptowrapper.h"
#include "xmlsecc_evp.h"

/**************************************************************************
 *
 * Internal SymbianCrypto EVP key CTX
 *
 *************************************************************************/
typedef struct _xmlSecSymbianCryptoEvpKeyDataCtx	xmlSecSymbianCryptoEvpKeyDataCtx, 
						*xmlSecSymbianCryptoEvpKeyDataCtxPtr;
struct _xmlSecSymbianCryptoEvpKeyDataCtx {
    EVP_PKEY*		pKey;
};
	    
/******************************************************************************
 *
 * EVP key (dsa/rsa)
 *
 * xmlSecSymbianCryptoEvpKeyDataCtx is located after xmlSecTransform
 *
 *****************************************************************************/
#define xmlSecSymbianCryptoEvpKeyDataSize	\
    (sizeof(xmlSecKeyData) + sizeof(xmlSecSymbianCryptoEvpKeyDataCtx))	
#define xmlSecSymbianCryptoEvpKeyDataGetCtx(data) \
    ((xmlSecSymbianCryptoEvpKeyDataCtxPtr)(((xmlSecByte*)(data)) + sizeof(xmlSecKeyData)))

static int		xmlSecSymbianCryptoEvpKeyDataInitialize	(xmlSecKeyDataPtr data);
static int		xmlSecSymbianCryptoEvpKeyDataDuplicate	(xmlSecKeyDataPtr dst,
								 xmlSecKeyDataPtr src);
static void		xmlSecSymbianCryptoEvpKeyDataFinalize		(xmlSecKeyDataPtr data);

/**
 * xmlSecSymbianCryptoEvpKeyDataAdoptEvp:
 * @data:		the pointer to SymbianCrypto EVP key data.
 * @pKey:		the pointer to EVP key.
 *
 * Sets the value of key data.
 *
 * Returns 0 on success or a negative value otherwise.
 */
EXPORT_C
int 
xmlSecSymbianCryptoEvpKeyDataAdoptEvp(xmlSecKeyDataPtr data, EVP_PKEY* pKey) {
    xmlSecSymbianCryptoEvpKeyDataCtxPtr ctx;

    xmlSecAssert2(xmlSecKeyDataIsValid(data), -1);
    xmlSecAssert2(xmlSecKeyDataCheckSize(data, xmlSecSymbianCryptoEvpKeyDataSize), -1);
    xmlSecAssert2(pKey, -1);
    
    ctx = xmlSecSymbianCryptoEvpKeyDataGetCtx(data);
    xmlSecAssert2(ctx, -1);
    
    if(ctx->pKey) {
	sc_pkey_free(ctx->pKey);
    }
    ctx->pKey = pKey;
    return(0);
}

/**
 * xmlSecSymbianCryptoEvpKeyDataGetEvp:
 * @data:		the pointer to SymbianCrypto EVP data.
 *
 * Gets the EVP_PKEY from the key data.
 *
 * Returns pointer to EVP_PKEY or NULL if an error occurs.
 */
EXPORT_C
EVP_PKEY* 
xmlSecSymbianCryptoEvpKeyDataGetEvp(xmlSecKeyDataPtr data) {
    xmlSecSymbianCryptoEvpKeyDataCtxPtr ctx;

    xmlSecAssert2(xmlSecKeyDataIsValid(data), NULL);
    xmlSecAssert2(xmlSecKeyDataCheckSize(data, xmlSecSymbianCryptoEvpKeyDataSize), NULL);

    ctx = xmlSecSymbianCryptoEvpKeyDataGetCtx(data);
    xmlSecAssert2(ctx, NULL);

    return(ctx->pKey);

	//return NULL;    
}

static int
xmlSecSymbianCryptoEvpKeyDataInitialize(xmlSecKeyDataPtr data) {
    xmlSecSymbianCryptoEvpKeyDataCtxPtr ctx;

    xmlSecAssert2(xmlSecKeyDataIsValid(data), -1);
    xmlSecAssert2(xmlSecKeyDataCheckSize(data, xmlSecSymbianCryptoEvpKeyDataSize), -1);

    ctx = xmlSecSymbianCryptoEvpKeyDataGetCtx(data);
    xmlSecAssert2(ctx, -1);

    memset(ctx, 0, sizeof(xmlSecSymbianCryptoEvpKeyDataCtx));

    return(0);
}

static int
xmlSecSymbianCryptoEvpKeyDataDuplicate(xmlSecKeyDataPtr dst, xmlSecKeyDataPtr src) {
    xmlSecSymbianCryptoEvpKeyDataCtxPtr ctxDst;
    xmlSecSymbianCryptoEvpKeyDataCtxPtr ctxSrc;

    xmlSecAssert2(xmlSecKeyDataIsValid(dst), -1);
    xmlSecAssert2(xmlSecKeyDataCheckSize(dst, xmlSecSymbianCryptoEvpKeyDataSize), -1);
    xmlSecAssert2(xmlSecKeyDataIsValid(src), -1);
    xmlSecAssert2(xmlSecKeyDataCheckSize(src, xmlSecSymbianCryptoEvpKeyDataSize), -1);

    ctxDst = xmlSecSymbianCryptoEvpKeyDataGetCtx(dst);
    xmlSecAssert2(ctxDst, -1);
    xmlSecAssert2(!ctxDst->pKey, -1);

    ctxSrc = xmlSecSymbianCryptoEvpKeyDataGetCtx(src);
    xmlSecAssert2(ctxSrc, -1);

    if(ctxSrc->pKey) 
    {
	ctxDst->pKey = xmlSecSymbianCryptoEvpKeyDup(ctxSrc->pKey);
	if(!ctxDst->pKey) 
	    {
	        xmlSecError(XMLSEC_ERRORS_HERE,
	            xmlSecErrorsSafeString(xmlSecKeyDataGetName(dst)),
	            "xmlSecSymbianCryptoEvpKeyDup",
	            XMLSEC_ERRORS_R_XMLSEC_FAILED,
	            XMLSEC_ERRORS_NO_MESSAGE);
	        return(-1);
	    }   
     ctxDst->pKey->duplicate=0;
     ctxSrc->pKey->duplicate=1;
     } 

    return(0);
}

static void
xmlSecSymbianCryptoEvpKeyDataFinalize(xmlSecKeyDataPtr data) {
    xmlSecSymbianCryptoEvpKeyDataCtxPtr ctx;
    
    xmlSecAssert(xmlSecKeyDataIsValid(data));
    xmlSecAssert(xmlSecKeyDataCheckSize(data, xmlSecSymbianCryptoEvpKeyDataSize));

    ctx = xmlSecSymbianCryptoEvpKeyDataGetCtx(data);
    xmlSecAssert(ctx);
    
    if(ctx->pKey) {
	sc_pkey_free(ctx->pKey);
    }
    memset(ctx, 0, sizeof(xmlSecSymbianCryptoEvpKeyDataCtx));
    
}
/****************************************************************************
 *
 * Symbian Keys Store
 *
 ***************************************************************************/
#define xmlSecSymbianKeysStoreSize \
	(sizeof(xmlSecKeyStore)+ sizeof(xmlSecPtrList))
#define xmlSecSymbianKeysStoreGetList(store) \
    ((xmlSecKeyStoreCheckSize((store), xmlSecSymbianKeysStoreSize)) ? \
	(xmlSecPtrListPtr)(((xmlSecByte*)(store)) + sizeof(xmlSecKeyStore)) : \
	(xmlSecPtrListPtr)NULL)


static xmlSecKeyPtr 		xmlSecSymbianKeysStoreFindKey	(xmlSecKeyStorePtr store, 
								 const xmlChar* name, 
								 xmlSecKeyInfoCtxPtr keyInfoCtx);

static xmlSecKeyStoreKlass xmlSecSymbianKeysStoreKlass = {
    sizeof(xmlSecKeyStoreKlass),
    xmlSecSymbianKeysStoreSize,

    /* data */
    BAD_CAST "symbian-keys-store",		/* const xmlChar* name; */ 
        
    /* constructors/destructor */
    NULL,		/* xmlSecKeyStoreInitializeMethod initialize; */
    NULL,		/* xmlSecKeyStoreFinalizeMethod finalize; */
    xmlSecSymbianKeysStoreFindKey,		/* xmlSecKeyStoreFindKeyMethod findKey; */

    /* reserved for the future */
    NULL,					/* void* reserved0; */
    NULL, 					/* void* reserved1; */
};

/**
 * xmlSecSymbianKeysStoreGetKlass:
 * 
 * The Symbian list based keys store klass.
 *
 * Returns simple list based keys store klass.
 */
EXPORT_C
xmlSecKeyStoreId 
xmlSecSymbianKeysStoreGetKlass(void) {
    return(&xmlSecSymbianKeysStoreKlass);
}


static xmlSecKeyPtr 
xmlSecSymbianKeysStoreFindKey(xmlSecKeyStorePtr store, const xmlChar* name, 
			    xmlSecKeyInfoCtxPtr keyInfoCtx) {
  
    xmlSecKeyPtr key=NULL;
   
    xmlSecAssert2(xmlSecKeyStoreCheckId(store, xmlSecSymbianKeysStoreId), NULL);
    xmlSecAssert2(keyInfoCtx, NULL);

    if((!name) || (keyInfoCtx->keyReq.keyId == xmlSecKeyDataIdUnknown)){
	return(NULL);
    }
    if(keyInfoCtx->keyReq.keyId== xmlSecSymbianCryptoKeyDataRsaId) {

    key=xmlSecSymbianCryptoAppKeyLoadSks((char*)name);	
    if(!key) {
            xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecSymbianKeysStoreFindKey",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "name=%s", 
		    xmlSecErrorsSafeString(name));
	    return(NULL);
        }
    
    }
    return(key);
}
/**
 * xmlSecSymbianCryptoAppKeyLoadSks:
 * @keyname:		the key name.
 *
 * Reads key from the symbian keystore.
 *
 * Returns pointer to the key or NULL if an error occurs.
 */
EXPORT_C
xmlSecKeyPtr
xmlSecSymbianCryptoAppKeyLoadSks(char* keyname) {

    xmlSecKeyPtr key = NULL;
    xmlSecKeyDataPtr data;
    EVP_PKEY* pKey = NULL;    

    int ret;

    xmlSecAssert2(keyname, NULL);

    pKey = d2i_PKCS8PrivateKey(keyname);
    if(!pKey) {
		    xmlSecError(XMLSEC_ERRORS_HERE,
				NULL,
				"sc_PrivateKey_read",
				XMLSEC_ERRORS_R_CRYPTO_FAILED,
				XMLSEC_ERRORS_NO_MESSAGE);
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
/******************************************************************************
 *
 * EVP helper functions
 *
 *****************************************************************************/
/**
 * xmlSecSymbianCryptoEvpKeyDup:
 * @pKey:		the pointer to EVP_PKEY.
 *
 * Duplicates @pKey.
 *
 * Returns pointer to newly created EVP_PKEY object or NULL if an error occurs.
 */
EXPORT_C
EVP_PKEY* 
xmlSecSymbianCryptoEvpKeyDup(EVP_PKEY* pKey) {
    
    EVP_PKEY* pKeyNew;

    xmlSecAssert2(pKey, NULL);
   
    pKeyNew = sc_pkey_duplicate(pKey);
    if(!pKeyNew) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "sc_pkey_duplicate",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(NULL);		    	
    }

    return (pKeyNew);
}

/**
 * xmlSecSymbianCryptoEvpKeyAdopt:
 * @pKey:		the pointer to EVP_PKEY.
 *
 * Creates xmlsec key object from SymbianCrypto key object.
 *
 * Returns pointer to newly created xmlsec key or NULL if an error occurs.
 */
EXPORT_C
xmlSecKeyDataPtr
xmlSecSymbianCryptoEvpKeyAdopt(EVP_PKEY *pKey) {
    xmlSecKeyDataPtr data = NULL;
    int ret;
    
    xmlSecAssert2(pKey, NULL);

    switch(pKey->type) {	
#ifndef XMLSEC_NO_RSA    
    case EVP_PKEY_RSA:
	data = xmlSecKeyDataCreate(xmlSecSymbianCryptoKeyDataRsaId);
	if(!data) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			NULL,
			"xmlSecKeyDataCreate",
			XMLSEC_ERRORS_R_XMLSEC_FAILED,
			"xmlSecSymbianCryptoKeyDataRsaId");
	    return(NULL);	    
	}
	break;
#endif /* XMLSEC_NO_RSA */	
#ifndef XMLSEC_NO_DSA	
    case EVP_PKEY_DSA:
	data = xmlSecKeyDataCreate(xmlSecSymbianCryptoKeyDataDsaId);
	if(!data) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			NULL,
			"xmlSecKeyDataCreate",
			XMLSEC_ERRORS_R_XMLSEC_FAILED,
			"xmlSecSymbianCryptoKeyDataDsaId");
	    return(NULL);	    
	}
	break;
#endif /* XMLSEC_NO_DSA */	
    default:	
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    NULL,
		    XMLSEC_ERRORS_R_INVALID_TYPE,
		    "evp key type %d not supported", pKey->type);
	return(NULL);
    }

    xmlSecAssert2(data, NULL);    
    ret = xmlSecSymbianCryptoEvpKeyDataAdoptEvp(data, pKey);
    if(ret < 0) {	
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecSymbianCryptoEvpKeyDataAdoptEvp",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	xmlSecKeyDataDestroy(data);
	return(NULL);	    
    }
    return(data);
}

#ifndef XMLSEC_NO_DSA    
/**************************************************************************
 *
 * <dsig:DSAKeyValue> processing
 *
 *
 * The DSAKeyValue Element (http://www.w3.org/TR/xmldsig-core/#sec-DSAKeyValue)
 *
 * DSA keys and the DSA signature algorithm are specified in [DSS]. 
 * DSA public key values can have the following fields:
 *      
 *   * P - a prime modulus meeting the [DSS] requirements 
 *   * Q - an integer in the range 2**159 < Q < 2**160 which is a prime 
 *         divisor of P-1 
 *   * G - an integer with certain properties with respect to P and Q 
 *   * Y - G**X mod P (where X is part of the private key and not made 
 *	   public) 
 *   * J - (P - 1) / Q 
 *   * seed - a DSA prime generation seed 
 *   * pgenCounter - a DSA prime generation counter
 *
 * Parameter J is available for inclusion solely for efficiency as it is 
 * calculatable from P and Q. Parameters seed and pgenCounter are used in the 
 * DSA prime number generation algorithm specified in [DSS]. As such, they are 
 * optional but must either both be present or both be absent. This prime 
 * generation algorithm is designed to provide assurance that a weak prime is 
 * not being used and it yields a P and Q value. Parameters P, Q, and G can be 
 * public and common to a group of users. They might be known from application 
 * context. As such, they are optional but P and Q must either both appear or 
 * both be absent. If all of P, Q, seed, and pgenCounter are present, 
 * implementations are not required to check if they are consistent and are 
 * free to use either P and Q or seed and pgenCounter. All parameters are 
 * encoded as base64 [MIME] values.
 *     
 * Arbitrary-length integers (e.g. "bignums" such as RSA moduli) are 
 * represented in XML as octet strings as defined by the ds:CryptoBinary type.
 *     
 * Schema Definition:
 *     
 * <element name="DSAKeyValue" type="ds:DSAKeyValueType"/> 
 * <complexType name="DSAKeyValueType"> 
 *   <sequence>
 *     <sequence minOccurs="0">
 *        <element name="P" type="ds:CryptoBinary"/> 
 *        <element name="Q" type="ds:CryptoBinary"/>
 *     </sequence>
 *     <element name="G" type="ds:CryptoBinary" minOccurs="0"/> 
 *     <element name="Y" type="ds:CryptoBinary"/> 
 *     <element name="J" type="ds:CryptoBinary" minOccurs="0"/>
 *     <sequence minOccurs="0">
 *       <element name="Seed" type="ds:CryptoBinary"/> 
 *       <element name="PgenCounter" type="ds:CryptoBinary"/> 
 *     </sequence>
 *   </sequence>
 * </complexType>
 *     
 * DTD Definition:
 *     
 *  <!ELEMENT DSAKeyValue ((P, Q)?, G?, Y, J?, (Seed, PgenCounter)?) > 
 *  <!ELEMENT P (#PCDATA) >
 *  <!ELEMENT Q (#PCDATA) >
 *  <!ELEMENT G (#PCDATA) >
 *  <!ELEMENT Y (#PCDATA) >
 *  <!ELEMENT J (#PCDATA) >
 *  <!ELEMENT Seed (#PCDATA) >
 *  <!ELEMENT PgenCounter (#PCDATA) >
 *
 * ============================================================================
 * 
 * To support reading/writing private keys an X element added (before Y).
 * Note: The current implementation does not support Seed and PgenCounter!
 * by this the P, Q and G are *required*!
 *
 *************************************************************************/
static int		xmlSecSymbianCryptoKeyDataDsaInitialize	(xmlSecKeyDataPtr data);
static int		xmlSecSymbianCryptoKeyDataDsaDuplicate	(xmlSecKeyDataPtr dst,
								 xmlSecKeyDataPtr src);
static void		xmlSecSymbianCryptoKeyDataDsaFinalize		(xmlSecKeyDataPtr data);
static int		xmlSecSymbianCryptoKeyDataDsaXmlRead		(xmlSecKeyDataId id,
								 xmlSecKeyPtr key,
								 xmlNodePtr node,
								 xmlSecKeyInfoCtxPtr keyInfoCtx);
static int		xmlSecSymbianCryptoKeyDataDsaXmlWrite		(xmlSecKeyDataId id,
								 xmlSecKeyPtr key,
								 xmlNodePtr node,
								 xmlSecKeyInfoCtxPtr keyInfoCtx);
static int		xmlSecSymbianCryptoKeyDataDsaGenerate		(xmlSecKeyDataPtr data,
								 xmlSecSize sizeBits,
								 xmlSecKeyDataType type);

static xmlSecKeyDataType xmlSecSymbianCryptoKeyDataDsaGetType		(xmlSecKeyDataPtr data);
static xmlSecSize		xmlSecSymbianCryptoKeyDataDsaGetSize		(xmlSecKeyDataPtr data);
static void		xmlSecSymbianCryptoKeyDataDsaDebugDump	(xmlSecKeyDataPtr data,
								 FILE* output);
static void		xmlSecSymbianCryptoKeyDataDsaDebugXmlDump	(xmlSecKeyDataPtr data,
								 FILE* output);

static xmlSecKeyDataKlass xmlSecSymbianCryptoKeyDataDsaKlass = {
    sizeof(xmlSecKeyDataKlass),
    xmlSecSymbianCryptoEvpKeyDataSize,

    /* data */
    xmlSecNameDSAKeyValue,
    xmlSecKeyDataUsageKeyValueNode | xmlSecKeyDataUsageRetrievalMethodNodeXml, 
						/* xmlSecKeyDataUsage usage; */
    xmlSecHrefDSAKeyValue,			/* const xmlChar* href; */
    xmlSecNodeDSAKeyValue,			/* const xmlChar* dataNodeName; */
    xmlSecDSigNs,				/* const xmlChar* dataNodeNs; */
    
    /* constructors/destructor */
    xmlSecSymbianCryptoKeyDataDsaInitialize,		/* xmlSecKeyDataInitializeMethod initialize; */
    xmlSecSymbianCryptoKeyDataDsaDuplicate,		/* xmlSecKeyDataDuplicateMethod duplicate; */
    xmlSecSymbianCryptoKeyDataDsaFinalize,		/* xmlSecKeyDataFinalizeMethod finalize; */
    xmlSecSymbianCryptoKeyDataDsaGenerate,		/* xmlSecKeyDataGenerateMethod generate; */
    
    /* get info */
    xmlSecSymbianCryptoKeyDataDsaGetType, 		/* xmlSecKeyDataGetTypeMethod getType; */
    xmlSecSymbianCryptoKeyDataDsaGetSize,		/* xmlSecKeyDataGetSizeMethod getSize; */
    NULL,					/* xmlSecKeyDataGetIdentifier getIdentifier; */    

    /* read/write */
    xmlSecSymbianCryptoKeyDataDsaXmlRead,		/* xmlSecKeyDataXmlReadMethod xmlRead; */
    xmlSecSymbianCryptoKeyDataDsaXmlWrite,		/* xmlSecKeyDataXmlWriteMethod xmlWrite; */
    NULL,					/* xmlSecKeyDataBinReadMethod binRead; */
    NULL,					/* xmlSecKeyDataBinWriteMethod binWrite; */

    /* debug */
    xmlSecSymbianCryptoKeyDataDsaDebugDump,		/* xmlSecKeyDataDebugDumpMethod debugDump; */
    xmlSecSymbianCryptoKeyDataDsaDebugXmlDump, 	/* xmlSecKeyDataDebugDumpMethod debugXmlDump; */

    /* reserved for the future */
    NULL,					/* void* reserved0; */
    NULL,					/* void* reserved1; */
};

/**
 * xmlSecSymbianCryptoKeyDataDsaGetKlass:
 * 
 * The DSA key data klass.
 *
 * Returns pointer to DSA key data klass.
 */
xmlSecKeyDataId 
xmlSecSymbianCryptoKeyDataDsaGetKlass(void) {
    return(&xmlSecSymbianCryptoKeyDataDsaKlass);
}

/**
 * xmlSecSymbianCryptoKeyDataDsaAdoptDsa:
 * @data:		the pointer to DSA key data.
 * @dsa:		the pointer to SymbianCrypto DSA key.
 *
 * Sets the value of DSA key data.
 *
 * Returns 0 on success or a negative value otherwise.
 */ 
int
xmlSecSymbianCryptoKeyDataDsaAdoptDsa(xmlSecKeyDataPtr data, DSA* dsa) {
    EVP_PKEY* pKey = NULL;
    int ret;
    
    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataDsaId), -1);
    
    /* construct new EVP_PKEY */
    if(dsa) {
	pKey = EVP_PKEY_new();
	if(!pKey) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
			"EVP_PKEY_new",
			XMLSEC_ERRORS_R_CRYPTO_FAILED,
			XMLSEC_ERRORS_NO_MESSAGE);
	    return(-1);
	}
	
	ret = EVP_PKEY_assign_DSA(pKey, dsa);
	if(ret != 1) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
			"EVP_PKEY_assign_DSA",
			XMLSEC_ERRORS_R_CRYPTO_FAILED,
			XMLSEC_ERRORS_NO_MESSAGE);
	    return(-1);
	}	
    }
    
    ret = xmlSecSymbianCryptoKeyDataDsaAdoptEvp(data, pKey);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    "xmlSecSymbianCryptoKeyDataDsaAdoptEvp",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	if(pKey) {
	    EVP_PKEY_free(pKey);
	}
	return(-1);
    }
    return(0);    
}

/**
 * xmlSecSymbianCryptoKeyDataDsaGetDsa:
 * @data:		the pointer to DSA key data.
 *
 * Gets the SymbianCrypto DSA key from DSA key data.
 *
 * Returns pointer to SymbianCrypto DSA key or NULL if an error occurs.
 */
DSA* 
xmlSecSymbianCryptoKeyDataDsaGetDsa(xmlSecKeyDataPtr data) {
    EVP_PKEY* pKey;
    
    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataDsaId), NULL);
    
    pKey = xmlSecSymbianCryptoKeyDataDsaGetEvp(data);
    xmlSecAssert2((!pKey) || (pKey->type == EVP_PKEY_DSA), NULL);
    
    return((pKey) ? pKey->pkey.dsa : (DSA*)NULL);
}

/** 
 * xmlSecSymbianCryptoKeyDataDsaAdoptEvp:
 * @data:		the pointer to DSA key data.
 * @pKey:		the pointer to SymbianCrypto EVP key.
 *
 * Sets the DSA key data value to SymbianCrypto EVP key.
 *
 * Returns 0 on success or a negative value otherwise.
 */
int 
xmlSecSymbianCryptoKeyDataDsaAdoptEvp(xmlSecKeyDataPtr data, EVP_PKEY* pKey) {
    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataDsaId), -1);
    xmlSecAssert2(pKey, -1);
    xmlSecAssert2(pKey->type == EVP_PKEY_DSA, -1);
    
    return(xmlSecSymbianCryptoEvpKeyDataAdoptEvp(data, pKey));
}

/**
 * xmlSecSymbianCryptoKeyDataDsaGetEvp:
 * @data:		the pointer to DSA key data.
 *
 * Gets the SymbianCrypto EVP key from DSA key data.
 *
 * Returns pointer to SymbianCrypto EVP key or NULL if an error occurs.
 */
EVP_PKEY* 
xmlSecSymbianCryptoKeyDataDsaGetEvp(xmlSecKeyDataPtr data) {
    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataDsaId), NULL);

    return(xmlSecSymbianCryptoEvpKeyDataGetEvp(data));
}

static int
xmlSecSymbianCryptoKeyDataDsaInitialize(xmlSecKeyDataPtr data) {
    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataDsaId), -1);

    return(xmlSecSymbianCryptoEvpKeyDataInitialize(data));
}

static int
xmlSecSymbianCryptoKeyDataDsaDuplicate(xmlSecKeyDataPtr dst, xmlSecKeyDataPtr src) {
    xmlSecAssert2(xmlSecKeyDataCheckId(dst, xmlSecSymbianCryptoKeyDataDsaId), -1);
    xmlSecAssert2(xmlSecKeyDataCheckId(src, xmlSecSymbianCryptoKeyDataDsaId), -1);

    return(xmlSecSymbianCryptoEvpKeyDataDuplicate(dst, src));
}

static void
xmlSecSymbianCryptoKeyDataDsaFinalize(xmlSecKeyDataPtr data) {
    xmlSecAssert(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataDsaId));
    
    xmlSecSymbianCryptoEvpKeyDataFinalize(data);
}

static int
xmlSecSymbianCryptoKeyDataDsaXmlRead(xmlSecKeyDataId id, xmlSecKeyPtr key,
				    xmlNodePtr node, xmlSecKeyInfoCtxPtr keyInfoCtx) {
    xmlSecKeyDataPtr data;
    xmlNodePtr cur;
    DSA *dsa;
    int ret;

    xmlSecAssert2(id == xmlSecSymbianCryptoKeyDataDsaId, -1);
    xmlSecAssert2(key, -1);
    xmlSecAssert2(node, -1);
    xmlSecAssert2(keyInfoCtx, -1);

    if(xmlSecKeyGetValue(key)) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    NULL,
		    XMLSEC_ERRORS_R_INVALID_KEY_DATA,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);	
    }

    dsa = DSA_new();
    if(!dsa) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    "DSA_new",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }
    
    cur = xmlSecGetNextElementNode(node->children);

    /* first is P node. It is REQUIRED because we do not support Seed and PgenCounter*/
    if((!cur) || (!xmlSecCheckNodeName(cur,  xmlSecNodeDSAP, xmlSecDSigNs))) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    xmlSecErrorsSafeString(xmlSecNodeGetName(cur)),
		    XMLSEC_ERRORS_R_INVALID_NODE,
		    "node=%s", 
		    xmlSecErrorsSafeString(xmlSecNodeDSAP));
	DSA_free(dsa);	
	return(-1);
    }
    if(!xmlSecSymbianCryptoNodeGetBNValue(cur, &(dsa->p))) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    "xmlSecSymbianCryptoNodeGetBNValue",		    
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "node=%s", 
		    xmlSecErrorsSafeString(xmlSecNodeDSAP));
	DSA_free(dsa);
	return(-1);
    }
    cur = xmlSecGetNextElementNode(cur->next);

    /* next is Q node. It is REQUIRED because we do not support Seed and PgenCounter*/
    if((!cur) || (!xmlSecCheckNodeName(cur, xmlSecNodeDSAQ, xmlSecDSigNs))) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    xmlSecErrorsSafeString(xmlSecNodeGetName(cur)),
		    XMLSEC_ERRORS_R_INVALID_NODE,
		    "node=%s", 
		    xmlSecErrorsSafeString(xmlSecNodeDSAQ));
	DSA_free(dsa);
	return(-1);
    }
    if(!xmlSecSymbianCryptoNodeGetBNValue(cur, &(dsa->q))) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    "xmlSecSymbianCryptoNodeGetBNValue",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "node=%s", 
		    xmlSecErrorsSafeString(xmlSecNodeDSAQ));
	DSA_free(dsa);
	return(-1);
    }
    cur = xmlSecGetNextElementNode(cur->next);

    /* next is G node. It is REQUIRED because we do not support Seed and PgenCounter*/
    if((!cur) || (!xmlSecCheckNodeName(cur, xmlSecNodeDSAG, xmlSecDSigNs))) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    xmlSecErrorsSafeString(xmlSecNodeGetName(cur)),
		    XMLSEC_ERRORS_R_INVALID_NODE,
		    "node=%s", 
		    xmlSecErrorsSafeString(xmlSecNodeDSAG));
	DSA_free(dsa);
	return(-1);
    }
    if(!xmlSecSymbianCryptoNodeGetBNValue(cur, &(dsa->g))) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    "xmlSecSymbianCryptoNodeGetBNValue",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "node=%s", 
		    xmlSecErrorsSafeString(xmlSecNodeDSAG));
	DSA_free(dsa);
	return(-1);
    }
    cur = xmlSecGetNextElementNode(cur->next);

    if((cur) && (xmlSecCheckNodeName(cur, xmlSecNodeDSAX, xmlSecNs))) {
        /* next is X node. It is REQUIRED for private key but
	 * we are not sure exactly what do we read */
	if(!xmlSecSymbianCryptoNodeGetBNValue(cur, &(dsa->priv_key))) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
			"xmlSecSymbianCryptoNodeGetBNValue",
		        XMLSEC_ERRORS_R_XMLSEC_FAILED,
			"node=%s", 
			xmlSecErrorsSafeString(xmlSecNodeDSAX));
	    DSA_free(dsa);
	    return(-1);
	}
	cur = xmlSecGetNextElementNode(cur->next);
    }

    /* next is Y node. */
    if((!cur) || (!xmlSecCheckNodeName(cur, xmlSecNodeDSAY, xmlSecDSigNs))) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    xmlSecErrorsSafeString(xmlSecNodeGetName(cur)),
		    XMLSEC_ERRORS_R_INVALID_NODE,
		    "node=%s", 
		    xmlSecErrorsSafeString(xmlSecNodeDSAY));
	DSA_free(dsa);
	return(-1);
    }
    if(!xmlSecSymbianCryptoNodeGetBNValue(cur, &(dsa->pub_key))) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    "xmlSecSymbianCryptoNodeGetBNValue",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "node=%s", xmlSecErrorsSafeString(xmlSecNodeDSAY));
	DSA_free(dsa);
	return(-1);
    }
    cur = xmlSecGetNextElementNode(cur->next);

    if((cur) && (xmlSecCheckNodeName(cur, xmlSecNodeDSAJ, xmlSecDSigNs))) {
	cur = xmlSecGetNextElementNode(cur->next);  
    }
    
    if((cur) && (xmlSecCheckNodeName(cur, xmlSecNodeDSASeed, xmlSecDSigNs))) {
	cur = xmlSecGetNextElementNode(cur->next);  
    }

    if((cur) && (xmlSecCheckNodeName(cur, xmlSecNodeDSAPgenCounter, xmlSecDSigNs))) {
	cur = xmlSecGetNextElementNode(cur->next);  
    }

    if(cur) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    xmlSecErrorsSafeString(xmlSecNodeGetName(cur)),
		    XMLSEC_ERRORS_R_UNEXPECTED_NODE,
		    XMLSEC_ERRORS_NO_MESSAGE);
	DSA_free(dsa);
	return(-1);
    }

    data = xmlSecKeyDataCreate(id);
    if(!data) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    "xmlSecKeyDataCreate",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	DSA_free(dsa);
	return(-1);
    }

    ret = xmlSecSymbianCryptoKeyDataDsaAdoptDsa(data, dsa);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    "xmlSecSymbianCryptoKeyDataDsaAdoptDsa",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	xmlSecKeyDataDestroy(data);
	DSA_free(dsa);
	return(-1);
    }

    ret = xmlSecKeySetValue(key, data);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    "xmlSecKeySetValue",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	xmlSecKeyDataDestroy(data);
	return(-1);	
    }

    return(0);
}

static int 
xmlSecSymbianCryptoKeyDataDsaXmlWrite(xmlSecKeyDataId id, xmlSecKeyPtr key,
				xmlNodePtr node, xmlSecKeyInfoCtxPtr keyInfoCtx) {
    xmlNodePtr cur;
    DSA* dsa;
    int ret;
    
    xmlSecAssert2(id == xmlSecSymbianCryptoKeyDataDsaId, -1);
    xmlSecAssert2(key, -1);
    xmlSecAssert2(xmlSecKeyDataCheckId(xmlSecKeyGetValue(key), 
                                xmlSecSymbianCryptoKeyDataDsaId), -1);
    xmlSecAssert2(node, -1);
    xmlSecAssert2(keyInfoCtx, -1);

    dsa = xmlSecSymbianCryptoKeyDataDsaGetDsa(xmlSecKeyGetValue(key));
    xmlSecAssert2(dsa, -1);
    
    if(((xmlSecKeyDataTypePublic | xmlSecKeyDataTypePrivate) & keyInfoCtx->keyReq.keyType) == 0) {
	/* we can have only private key or public key */
	return(0);
    }    
    
    /* first is P node */
    xmlSecAssert2(dsa->p, -1);
    cur = xmlSecAddChild(node, xmlSecNodeDSAP, xmlSecDSigNs);
    if(!cur) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    "xmlSecAddChild",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "node=%s", 
		    xmlSecErrorsSafeString(xmlSecNodeDSAP));
	return(-1);	
    }
    ret = xmlSecSymbianCryptoNodeSetBNValue(cur, dsa->p, 1);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    "xmlSecSymbianCryptoNodeSetBNValue",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "node=%s", 
		    xmlSecErrorsSafeString(xmlSecNodeDSAP));
	return(-1);
    }    

    /* next is Q node. */
    xmlSecAssert2(dsa->q, -1);
    cur = xmlSecAddChild(node, xmlSecNodeDSAQ, xmlSecDSigNs);
    if(!cur) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    "xmlSecAddChild",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "node=%s", 
		    xmlSecErrorsSafeString(xmlSecNodeDSAQ));
	return(-1);	
    }
    ret = xmlSecSymbianCryptoNodeSetBNValue(cur, dsa->q, 1);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    "xmlSecSymbianCryptoNodeSetBNValue",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "node=%s", 
		    xmlSecErrorsSafeString(xmlSecNodeDSAQ));
	return(-1);
    }

    /* next is G node. */
    xmlSecAssert2(dsa->g, -1);
    cur = xmlSecAddChild(node, xmlSecNodeDSAG, xmlSecDSigNs);
    if(!cur) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    "xmlSecAddChild",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "node=%s", 
		    xmlSecErrorsSafeString(xmlSecNodeDSAG));
	return(-1);	
    }
    ret = xmlSecSymbianCryptoNodeSetBNValue(cur, dsa->g, 1);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    "xmlSecSymbianCryptoNodeSetBNValue",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "node=%s", 
		    xmlSecErrorsSafeString(xmlSecNodeDSAG));
	return(-1);
    }

    /* next is X node: write it ONLY for private keys and ONLY if it is requested */
    if(((keyInfoCtx->keyReq.keyType & xmlSecKeyDataTypePrivate) != 0) && (dsa->priv_key)) {
	cur = xmlSecAddChild(node, xmlSecNodeDSAX, xmlSecNs);
	if(!cur) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
			"xmlSecAddChild",
			XMLSEC_ERRORS_R_XMLSEC_FAILED,
			"node=%s", 
			xmlSecErrorsSafeString(xmlSecNodeDSAX));
	    return(-1);	
	}
	ret = xmlSecSymbianCryptoNodeSetBNValue(cur, dsa->priv_key, 1);
	if(ret < 0) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
			"xmlSecSymbianCryptoNodeSetBNValue",
			XMLSEC_ERRORS_R_XMLSEC_FAILED,
			"node=%s", 
			xmlSecErrorsSafeString(xmlSecNodeDSAX));
	    return(-1);
	}
    }

    /* next is Y node. */
    xmlSecAssert2(dsa->pub_key, -1);
    cur = xmlSecAddChild(node, xmlSecNodeDSAY, xmlSecDSigNs);
    if(!cur) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    "xmlSecAddChild",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "node=%s", 
		    xmlSecErrorsSafeString(xmlSecNodeDSAY));
	return(-1);	
    }
    ret = xmlSecSymbianCryptoNodeSetBNValue(cur, dsa->pub_key, 1);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    "xmlSecSymbianCryptoNodeSetBNValue",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "node=%s", 
		    xmlSecErrorsSafeString(xmlSecNodeDSAY));
	return(-1);
    }
    return(0);
}

static int
xmlSecSymbianCryptoKeyDataDsaGenerate(xmlSecKeyDataPtr data, 
                            xmlSecSize sizeBits, 
                            xmlSecKeyDataType type ATTRIBUTE_UNUSED) {
    DSA* dsa;
    int counter_ret;
    unsigned long h_ret;
    int ret;
    
    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataDsaId), -1);
    xmlSecAssert2(sizeBits > 0, -1);

    dsa = DSA_generate_parameters(sizeBits, NULL, 0, &counter_ret, &h_ret, NULL, NULL); 
    if(!dsa) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    "DSA_generate_parameters",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    "size=%d", sizeBits);
	return(-1);    
    }

    ret = DSA_generate_key(dsa);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    "DSA_generate_key",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	DSA_free(dsa);
	return(-1);    
    }

    ret = xmlSecSymbianCryptoKeyDataDsaAdoptDsa(data, dsa);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    "xmlSecSymbianCryptoKeyDataDsaAdoptDsa",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	DSA_free(dsa);
	return(-1);
    }

    return(0);
}

static xmlSecKeyDataType
xmlSecSymbianCryptoKeyDataDsaGetType(xmlSecKeyDataPtr data) {
    DSA* dsa;
    
    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataDsaId), 
                                xmlSecKeyDataTypeUnknown);
    
    dsa = xmlSecSymbianCryptoKeyDataDsaGetDsa(data);
    if((dsa) && (dsa->p) && (dsa->q) && 
       (dsa->g) && (dsa->pub_key)) {
       
        if(dsa->priv_key) {
	    return(xmlSecKeyDataTypePrivate | xmlSecKeyDataTypePublic);
	} else if(dsa->engine) {
	    /**
	     *
	     * We assume here that engine *always* has private key.
	     * This might be incorrect but it seems that there is no
	     * way to ask engine if given key is private or not.
	     */
	    return(xmlSecKeyDataTypePrivate | xmlSecKeyDataTypePublic);
	} else {
	    return(xmlSecKeyDataTypePublic);
	}
    }

    return(xmlSecKeyDataTypeUnknown);
}

static xmlSecSize 
xmlSecSymbianCryptoKeyDataDsaGetSize(xmlSecKeyDataPtr data) {
    DSA* dsa;

    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataDsaId), 0);

    dsa = xmlSecSymbianCryptoKeyDataDsaGetDsa(data);
    if((dsa) && (dsa->p)) {
	return(BN_num_bits(dsa->p));
    }    
    return(0);
}

static void 
xmlSecSymbianCryptoKeyDataDsaDebugDump(xmlSecKeyDataPtr data, FILE* output) {
    xmlSecAssert(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataDsaId));
    xmlSecAssert(output);
    
    fprintf(output, "=== dsa key: size = %d\n", 
	    xmlSecSymbianCryptoKeyDataDsaGetSize(data));
}

static void
xmlSecSymbianCryptoKeyDataDsaDebugXmlDump(xmlSecKeyDataPtr data, FILE* output) {
    xmlSecAssert(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataDsaId));
    xmlSecAssert(output);
        
    fprintf(output, "<DSAKeyValue size=\"%d\" />\n", 
	    xmlSecSymbianCryptoKeyDataDsaGetSize(data));
}

#endif /* XMLSEC_NO_DSA */

#ifndef XMLSEC_NO_RSA
/**************************************************************************
 *
 * <dsig:RSAKeyValue> processing
 *
 * http://www.w3.org/TR/xmldsig-core/#sec-RSAKeyValue
 * The RSAKeyValue Element
 *
 * RSA key values have two fields: Modulus and Exponent.
 *
 * <RSAKeyValue>
 *   <Modulus>xA7SEU+e0yQH5rm9kbCDN9o3aPIo7HbP7tX6WOocLZAtNfyxSZDU16ksL6W
 *     jubafOqNEpcwR3RdFsT7bCqnXPBe5ELh5u4VEy19MzxkXRgrMvavzyBpVRgBUwUlV
 *   	  5foK5hhmbktQhyNdy/6LpQRhDUDsTvK+g9Ucj47es9AQJ3U=
 *   </Modulus>
 *   <Exponent>AQAB</Exponent>
 * </RSAKeyValue>
 *
 * Arbitrary-length integers (e.g. "bignums" such as RSA moduli) are 
 * represented in XML as octet strings as defined by the ds:CryptoBinary type.
 *
 * Schema Definition:
 * 
 * <element name="RSAKeyValue" type="ds:RSAKeyValueType"/>
 * <complexType name="RSAKeyValueType">
 *   <sequence>
 *     <element name="Modulus" type="ds:CryptoBinary"/> 
 *     <element name="Exponent" type="ds:CryptoBinary"/>
 *   </sequence>
 * </complexType>
 *
 * DTD Definition:
 * 
 * <!ELEMENT RSAKeyValue (Modulus, Exponent) > 
 * <!ELEMENT Modulus (#PCDATA) >
 * <!ELEMENT Exponent (#PCDATA) >
 *
 * ============================================================================
 * 
 * To support reading/writing private keys an PrivateExponent element is added
 * to the end
 *
 *************************************************************************/

static int		xmlSecSymbianCryptoKeyDataRsaInitialize	(xmlSecKeyDataPtr data);
static int		xmlSecSymbianCryptoKeyDataRsaDuplicate	(xmlSecKeyDataPtr dst,
								 xmlSecKeyDataPtr src);
static void		xmlSecSymbianCryptoKeyDataRsaFinalize		(xmlSecKeyDataPtr data);
static int		xmlSecSymbianCryptoKeyDataRsaXmlRead		(xmlSecKeyDataId id,
								 xmlSecKeyPtr key,
								 xmlNodePtr node,
								 xmlSecKeyInfoCtxPtr keyInfoCtx);
static int		xmlSecSymbianCryptoKeyDataRsaXmlWrite		(xmlSecKeyDataId id,
								 xmlSecKeyPtr key,
								 xmlNodePtr node,
								 xmlSecKeyInfoCtxPtr keyInfoCtx);
static int		xmlSecSymbianCryptoKeyDataRsaGenerate		(xmlSecKeyDataPtr data,
							    	 xmlSecSize sizeBits,
								 xmlSecKeyDataType type);

static xmlSecKeyDataType xmlSecSymbianCryptoKeyDataRsaGetType		(xmlSecKeyDataPtr data);
static xmlSecSize		xmlSecSymbianCryptoKeyDataRsaGetSize		(xmlSecKeyDataPtr data);
static void		xmlSecSymbianCryptoKeyDataRsaDebugDump	(xmlSecKeyDataPtr data,
								 FILE* output);
static void		xmlSecSymbianCryptoKeyDataRsaDebugXmlDump	(xmlSecKeyDataPtr data,
								 FILE* output);
static xmlSecKeyDataKlass xmlSecSymbianCryptoKeyDataRsaKlass = {
    sizeof(xmlSecKeyDataKlass),
    xmlSecSymbianCryptoEvpKeyDataSize,

    /* data */
    xmlSecNameRSAKeyValue,
    xmlSecKeyDataUsageKeyValueNode | xmlSecKeyDataUsageRetrievalMethodNodeXml, 
						/* xmlSecKeyDataUsage usage; */
    xmlSecHrefRSAKeyValue,			/* const xmlChar* href; */
    xmlSecNodeRSAKeyValue,			/* const xmlChar* dataNodeName; */
    xmlSecDSigNs,				/* const xmlChar* dataNodeNs; */
    
    /* constructors/destructor */
    xmlSecSymbianCryptoKeyDataRsaInitialize,		/* xmlSecKeyDataInitializeMethod initialize; */
    xmlSecSymbianCryptoKeyDataRsaDuplicate,		/* xmlSecKeyDataDuplicateMethod duplicate; */
    xmlSecSymbianCryptoKeyDataRsaFinalize,		/* xmlSecKeyDataFinalizeMethod finalize; */
    xmlSecSymbianCryptoKeyDataRsaGenerate,		/* xmlSecKeyDataGenerateMethod generate; */
    
    /* get info */
    xmlSecSymbianCryptoKeyDataRsaGetType, 		/* xmlSecKeyDataGetTypeMethod getType; */
    xmlSecSymbianCryptoKeyDataRsaGetSize,		/* xmlSecKeyDataGetSizeMethod getSize; */
    NULL,					/* xmlSecKeyDataGetIdentifier getIdentifier; */    

    /* read/write */
    xmlSecSymbianCryptoKeyDataRsaXmlRead,		/* xmlSecKeyDataXmlReadMethod xmlRead; */
    xmlSecSymbianCryptoKeyDataRsaXmlWrite,		/* xmlSecKeyDataXmlWriteMethod xmlWrite; */
    NULL,					/* xmlSecKeyDataBinReadMethod binRead; */
    NULL,					/* xmlSecKeyDataBinWriteMethod binWrite; */

    /* debug */
    xmlSecSymbianCryptoKeyDataRsaDebugDump,		/* xmlSecKeyDataDebugDumpMethod debugDump; */
    xmlSecSymbianCryptoKeyDataRsaDebugXmlDump, 	/* xmlSecKeyDataDebugDumpMethod debugXmlDump; */

    /* reserved for the future */
    NULL,					/* void* reserved0; */
    NULL,					/* void* reserved1; */
};

/** 
 * xmlSecSymbianCryptoKeyDataRsaGetKlass:
 *
 * The SymbianCrypto RSA key data klass.
 *
 * Returns pointer to SymbianCrypto RSA key data klass.
 */
EXPORT_C
xmlSecKeyDataId 
xmlSecSymbianCryptoKeyDataRsaGetKlass(void) {
    return(&xmlSecSymbianCryptoKeyDataRsaKlass);
}

/**
 * xmlSecSymbianCryptoKeyDataRsaAdoptRsa:
 * @data:		the pointer to RSA key data.
 * @rsa:		the pointer to SymbianCrypto RSA key.
 *
 * Sets the value of RSA key data.
 *
 * Returns 0 on success or a negative value otherwise.
 */ 
// not needed for current functionality
#ifdef DEBUG 
int
xmlSecSymbianCryptoKeyDataRsaAdoptRsa(xmlSecKeyDataPtr data, RSA* rsa) {
    EVP_PKEY* pKey = NULL;
    int ret;
    
    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataRsaId), -1);
    
    /* construct new EVP_PKEY */
    
	/*
	ret = EVP_PKEY_assign_RSA(pKey, rsa);
	if(ret != 1) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
			"EVP_PKEY_assign_RSA",
			XMLSEC_ERRORS_R_CRYPTO_FAILED,
			XMLSEC_ERRORS_NO_MESSAGE);
	    return(-1);
	}	
    }
   */
    ret = xmlSecSymbianCryptoKeyDataRsaAdoptEvp(data, pKey);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    "xmlSecSymbianCryptoKeyDataRsaAdoptEvp",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	if(pKey) {
	    EVP_PKEY_free(pKey);
	}
	return(-1);
    }
    
    return(0);    
}
#endif

/**
 * xmlSecSymbianCryptoKeyDataRsaGetRsa:
 * @data:		the pointer to RSA key data.
 *
 * Gets the SymbianCrypto RSA key from RSA key data.
 *
 * Returns pointer to SymbianCrypto RSA key or NULL if an error occurs.
 */
RSA* 
xmlSecSymbianCryptoKeyDataRsaGetRsa(xmlSecKeyDataPtr data) {
   
    
    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataRsaId), NULL);
/*    
    pKey = xmlSecSymbianCryptoKeyDataRsaGetEvpTest(data);
    xmlSecAssert2((pKey == NULL) || (pKey->type == EVP_PKEY_RSA), NULL);
    
    return((pKey != NULL) ? pKey->pkey.rsa : (RSA*)NULL);
*/    
    return NULL;
}

/** 
 * xmlSecSymbianCryptoKeyDataRsaAdoptEvp:
 * @data:		the pointer to RSA key data.
 * @pKey:		the pointer to SymbianCrypto EVP key.
 *
 * Sets the RSA key data value to SymbianCrypto EVP key.
 *
 * Returns 0 on success or a negative value otherwise.
 */
int 
xmlSecSymbianCryptoKeyDataRsaAdoptEvp(xmlSecKeyDataPtr data, EVP_PKEY* pKey) {
    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataRsaId), -1);
    xmlSecAssert2(pKey, -1);
    xmlSecAssert2(pKey->type == EVP_PKEY_RSA, -1);
    
    return(xmlSecSymbianCryptoEvpKeyDataAdoptEvp(data, pKey));
}

/**
 * xmlSecSymbianCryptoKeyDataRsaGetEvp:
 * @data:		the pointer to RSA key data.
 *
 * Gets the SymbianCrypto EVP key from RSA key data.
 *
 * Returns pointer to SymbianCrypto EVP key or NULL if an error occurs.
 */
EVP_PKEY* 
xmlSecSymbianCryptoKeyDataRsaGetEvpTest(xmlSecKeyDataPtr data) {
    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataRsaId), NULL);

    return(xmlSecSymbianCryptoEvpKeyDataGetEvp(data));
}

static int
xmlSecSymbianCryptoKeyDataRsaInitialize(xmlSecKeyDataPtr data) {
    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataRsaId), -1);

    return(xmlSecSymbianCryptoEvpKeyDataInitialize(data));
}

static int
xmlSecSymbianCryptoKeyDataRsaDuplicate(xmlSecKeyDataPtr dst, xmlSecKeyDataPtr src) {
    xmlSecAssert2(xmlSecKeyDataCheckId(dst, xmlSecSymbianCryptoKeyDataRsaId), -1);
    xmlSecAssert2(xmlSecKeyDataCheckId(src, xmlSecSymbianCryptoKeyDataRsaId), -1);

    return(xmlSecSymbianCryptoEvpKeyDataDuplicate(dst, src));
}

static void
xmlSecSymbianCryptoKeyDataRsaFinalize(xmlSecKeyDataPtr data) {
    xmlSecAssert(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataRsaId));

    xmlSecSymbianCryptoEvpKeyDataFinalize(data);
}

static int
xmlSecSymbianCryptoKeyDataRsaXmlRead(xmlSecKeyDataId id, xmlSecKeyPtr key,
				    xmlNodePtr node, xmlSecKeyInfoCtxPtr keyInfoCtx) {
    xmlSecKeyDataPtr data;
    xmlNodePtr cur;
    //RSA *rsa=NULL;
    int ret;

    xmlSecAssert2(id == xmlSecSymbianCryptoKeyDataRsaId, -1);
    xmlSecAssert2(key, -1);
    xmlSecAssert2(node, -1);
    xmlSecAssert2(keyInfoCtx, -1);

    if(xmlSecKeyGetValue(key)) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    NULL,		    
		    XMLSEC_ERRORS_R_INVALID_KEY_DATA,
		    "key already has a value");
	return(-1);	
    }

	/*
    if(!rsa) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    "RSA_new",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }
	*/
    cur = xmlSecGetNextElementNode(node->children);
    
    /* first is Modulus node. It is REQUIRED because we do not support Seed and PgenCounter*/
    if((!cur) || (!xmlSecCheckNodeName(cur,  xmlSecNodeRSAModulus, xmlSecDSigNs))) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    xmlSecErrorsSafeString(xmlSecNodeGetName(cur)),
		    XMLSEC_ERRORS_R_INVALID_NODE,
		    "node=%s", 
		    xmlSecErrorsSafeString(xmlSecNodeRSAModulus));	
	return(-1);
    }
    /*
    if(xmlSecSymbianCryptoNodeGetBNValue(cur, &(rsa->n)) == NULL) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    "xmlSecSymbianCryptoNodeGetBNValue",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "node=%s", 
		    xmlSecErrorsSafeString(xmlSecNodeRSAModulus));
	RSA_free(rsa);
	return(-1);
    }
    */
    cur = xmlSecGetNextElementNode(cur->next);

    /* next is Exponent node. It is REQUIRED because we do not support Seed and PgenCounter*/
    if((!cur) || (!xmlSecCheckNodeName(cur, xmlSecNodeRSAExponent, xmlSecDSigNs))) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    xmlSecErrorsSafeString(xmlSecNodeGetName(cur)),
		    XMLSEC_ERRORS_R_INVALID_NODE,
		    "node=%s", 
		    xmlSecErrorsSafeString(xmlSecNodeRSAExponent));
	return(-1);
    }
    /*
    if(xmlSecSymbianCryptoNodeGetBNValue(cur, &(rsa->e)) == NULL) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    "xmlSecSymbianCryptoNodeGetBNValue",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "node=%s", 
		    xmlSecErrorsSafeString(xmlSecNodeRSAExponent));
	RSA_free(rsa);
	return(-1);
    }
    */
    cur = xmlSecGetNextElementNode(cur->next);

    if((cur) && (xmlSecCheckNodeName(cur, xmlSecNodeRSAPrivateExponent, xmlSecNs))) {
        /* next is X node. It is REQUIRED for private key but
	 * we are not sure exactly what do we read */
	 /*
	if(xmlSecSymbianCryptoNodeGetBNValue(cur, &(rsa->d)) == NULL) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
			"xmlSecSymbianCryptoNodeGetBNValue",
			XMLSEC_ERRORS_R_XMLSEC_FAILED,
			"node=%s", 
			xmlSecErrorsSafeString(xmlSecNodeRSAPrivateExponent));
	    RSA_free(rsa);
	    return(-1);
	}
	*/
	cur = xmlSecGetNextElementNode(cur->next);
    }

    if(cur) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    xmlSecErrorsSafeString(xmlSecNodeGetName(cur)),
		    XMLSEC_ERRORS_R_INVALID_NODE,
		    "no nodes expected");
	return(-1);
    }

    data = xmlSecKeyDataCreate(id);
    if(!data) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    "xmlSecKeyDataCreate",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }
/*
    ret = xmlSecSymbianCryptoKeyDataRsaAdoptRsa(data, rsa);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    "xmlSecSymbianCryptoKeyDataRsaAdoptRsa",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	xmlSecKeyDataDestroy(data);
	//RSA_free(rsa);
	return(-1);
    }
*/
    ret = xmlSecKeySetValue(key, data);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    "xmlSecKeySetValue",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	xmlSecKeyDataDestroy(data);
	return(-1);	
    }

    return(0);
}

static int 
xmlSecSymbianCryptoKeyDataRsaXmlWrite(xmlSecKeyDataId id, xmlSecKeyPtr key,
			    xmlNodePtr node, xmlSecKeyInfoCtxPtr keyInfoCtx) {
    xmlNodePtr cur;
    RSA* rsa;
   
    
    xmlSecAssert2(id == xmlSecSymbianCryptoKeyDataRsaId, -1);
    xmlSecAssert2(key, -1);
    xmlSecAssert2(xmlSecKeyDataCheckId(xmlSecKeyGetValue(key), 
                                xmlSecSymbianCryptoKeyDataRsaId), -1);
    xmlSecAssert2(node, -1);
    xmlSecAssert2(keyInfoCtx, -1);

    rsa = xmlSecSymbianCryptoKeyDataRsaGetRsa(xmlSecKeyGetValue(key));
    xmlSecAssert2(rsa, -1);
    
    if(((xmlSecKeyDataTypePublic | xmlSecKeyDataTypePrivate) & keyInfoCtx->keyReq.keyType) == 0) {
	/* we can have only private key or public key */
	return(0);
    }    

    /* first is Modulus node */
    cur = xmlSecAddChild(node, xmlSecNodeRSAModulus, xmlSecDSigNs);
    if(!cur) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    "xmlSecAddChild",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "node=%s", 
		    xmlSecErrorsSafeString(xmlSecNodeRSAModulus));
	return(-1);	
    }
    /*
    ret = xmlSecSymbianCryptoNodeSetBNValue(cur, rsa->n, 1);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    "xmlSecSymbianCryptoNodeSetBNValue",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "node=%s", 
		    xmlSecErrorsSafeString(xmlSecNodeRSAModulus));
	return(-1);
    }    
    */

    /* next is Exponent node. */
    cur = xmlSecAddChild(node, xmlSecNodeRSAExponent, xmlSecDSigNs);
    if(!cur) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    "xmlSecAddChild",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "node=%s", 
		    xmlSecErrorsSafeString(xmlSecNodeRSAExponent));
	return(-1);	
    }
    /*
    ret = xmlSecSymbianCryptoNodeSetBNValue(cur, rsa->e, 1);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    "xmlSecSymbianCryptoNodeSetBNValue",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "node=%s", 
		    xmlSecErrorsSafeString(xmlSecNodeRSAExponent));
	return(-1);
    }
	*/
    /* next is PrivateExponent node: write it ONLY for private keys and ONLY if it is requested */
    /*
    if(((keyInfoCtx->keyReq.keyType & xmlSecKeyDataTypePrivate) != 0) && (rsa->d != NULL)) {
	cur = xmlSecAddChild(node, xmlSecNodeRSAPrivateExponent, xmlSecNs);
	if(cur == NULL) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
			"xmlSecAddChild",
			XMLSEC_ERRORS_R_XMLSEC_FAILED,
		        "node=%s", 
			xmlSecErrorsSafeString(xmlSecNodeRSAPrivateExponent));
	    return(-1);	
	}
	ret = xmlSecSymbianCryptoNodeSetBNValue(cur, rsa->d, 1);
	if(ret < 0) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
			"xmlSecSymbianCryptoNodeSetBNValue",
			XMLSEC_ERRORS_R_XMLSEC_FAILED,
		        "node=%s", 
			xmlSecErrorsSafeString(xmlSecNodeRSAPrivateExponent));
	    return(-1);
	}
    }
    */
    return(0);
}

// this routine is modified to fit the interface of evpwrapper.cpp
static int
xmlSecSymbianCryptoKeyDataRsaGenerate(xmlSecKeyDataPtr data, 
                            xmlSecSize sizeBits, 
                            xmlSecKeyDataType type ATTRIBUTE_UNUSED) {
   
    int ret;
    EVP_PKEY *pKey;
    
    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataRsaId), -1);
    xmlSecAssert2(sizeBits > 0, -1);

	// Construct new EVP_PKEY
	pKey = sc_pkey_new(EVP_PKEY_RSA, NULL);
    if(!pKey) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    "sc_pkey_key",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    "sizeBits=%d", sizeBits);
	return(-1);  
    }
	
	// Check if there is existing key first
	ret = sc_pkey_load(pKey);
	if (ret < -1) {	//KErrNotFound = -1
	xmlSecSetErrorFlag( ret );
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    "sc_load_key",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    "sizeBits=%d", sizeBits);
	sc_pkey_free(pKey);
	return(-1);  
    }	
		
	// Generate key if key is not found		
	if (ret == -1)	// KErrNotFound
	{
		TInt ret2=sc_pkey_generate(pKey, sizeBits);
		if ( ret2 < -1 )
		{
		    xmlSecSetErrorFlag( ret2 );
		    xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    "sc_generate_key",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    "sizeBits=%d", sizeBits);
			sc_pkey_free(pKey);
	        return(-1);      
		}
	}		
	if (!pKey->load) 
		{
	
		xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    "sc_generate_key",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    "sizeBits=%d", sizeBits);
		
		sc_pkey_free(pKey);
		return(-1);  
		}	

    ret = xmlSecSymbianCryptoKeyDataRsaAdoptEvp(data, pKey);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    "xmlSecSymbianCryptoKeyDataRsaAdoptEvp",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	if(pKey) {
	    sc_pkey_free(pKey);
	}
	return(-1);
    }


/*
    ret = xmlSecSymbianCryptoKeyDataRsaAdoptRsa(data, rsa);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    "xmlSecSymbianCryptoKeyDataRsaAdoptRsa",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	RSA_free(rsa);
	return(-1);
    }
*/
    sc_pkey_free(pKey);
    return(0);
}

static xmlSecKeyDataType
xmlSecSymbianCryptoKeyDataRsaGetType(xmlSecKeyDataPtr data) {
    EVP_PKEY* pkey;
    
    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataRsaId), 
                                xmlSecKeyDataTypeUnknown);
    
    pkey = xmlSecSymbianCryptoEvpKeyDataGetEvp(data);
    
    if (pkey->load)
    {
    	return(xmlSecKeyDataTypePrivate | xmlSecKeyDataTypePublic);
    }
    /*
    if((rsa != NULL) && (rsa->n != NULL) && (rsa->e != NULL)) {
	if(rsa->d != NULL) {
	    return(xmlSecKeyDataTypePrivate | xmlSecKeyDataTypePublic);
	} else if(rsa->engine != NULL) {
	*/
	    /**
	     *
	     * We assume here that engine *always* has private key.
	     * This might be incorrect but it seems that there is no
	     * way to ask engine if given key is private or not.
	     */
/*	     
	    return(xmlSecKeyDataTypePrivate | xmlSecKeyDataTypePublic);
	} else {
	    return(xmlSecKeyDataTypePublic);
	}
    }
	*/
    return(xmlSecKeyDataTypeUnknown);
}

static xmlSecSize 
xmlSecSymbianCryptoKeyDataRsaGetSize(xmlSecKeyDataPtr data) {
    EVP_PKEY* pkey;

    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataRsaId), 0);

	pkey = xmlSecSymbianCryptoEvpKeyDataGetEvp(data);
	if (pkey)
	{
		return pkey->bitsize;
	}
    /*
    if((rsa != NULL) && (rsa->n != NULL)) {
	return(BN_num_bits(rsa->n));
    }  
    */  
    return(0);
}

static void 
xmlSecSymbianCryptoKeyDataRsaDebugDump(xmlSecKeyDataPtr data, FILE* output) {
    xmlSecAssert(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataRsaId));
    xmlSecAssert(output);
    
    fprintf(output, "=== rsa key: size = %d\n", 
	    xmlSecSymbianCryptoKeyDataRsaGetSize(data));
}

static void
xmlSecSymbianCryptoKeyDataRsaDebugXmlDump(xmlSecKeyDataPtr data, FILE* output) {
    xmlSecAssert(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataRsaId));
    xmlSecAssert(output);
        
    fprintf(output, "<RSAKeyValue size=\"%d\" />\n", 
	    xmlSecSymbianCryptoKeyDataRsaGetSize(data));
}
    
#endif /* XMLSEC_NO_RSA */





