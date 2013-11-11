/** 
 * XMLSec library
 *
 * This is free software; see Copyright file in the source
 * distribution for preciese wording.
 * 
 * Copyright (C) 2002-2003 Aleksey Sanin <aleksey@aleksey.com>
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */
#include "xmlsecc_config.h"
#ifndef XMLSEC_NO_HMAC
#include "xmlsecc_globals.h"

#include <string.h>

#include <stdapis/libxml2/libxml2_globals.h>
#include <e32err.h>         //definition of KErrNoMemory

#include "xmlsec_xmlsec.h"
#include "xmlsec_xmltree.h"
#include "xmlsec_keys.h"
#include "xmlsec_transforms.h"
#include "xmlsec_errors.h"
#include "xmlsec_error_flag.h"

#include "xmlsecc_app.h"
#include "xmlsecc_crypto.h"
#include "xmlsecc_cryptowrapper.h"

#define XMLSEC_SYMBIANCRYPTO_MAX_HMAC_SIZE		128

/**************************************************************************
 *
 * Internal SYMBIANCRYPTO HMAC CTX
 *
 *****************************************************************************/
typedef struct _xmlSecSymbianCryptoHmacCtx 
                                      xmlSecSymbianCryptoHmacCtx, *xmlSecSymbianCryptoHmacCtxPtr;
struct _xmlSecSymbianCryptoHmacCtx {
    int			digest;
    ScMDHd		digestCtx;
    xmlSecByte	 	dgst[XMLSEC_SYMBIANCRYPTO_MAX_HMAC_SIZE];
    xmlSecSize		dgstSize;	/* dgst size in bits */
};	    

xmlSecTransformGetDataTypeMethod const xmlSecCrpytoGetDataTypeMethod  = xmlSecTransformDefaultGetDataType;
xmlSecTransformPushBinMethod const xmlSecCryptoPushBinMethod = xmlSecTransformDefaultPushBin;
xmlSecTransformPopBinMethod const xmlSecCryptoPopBinMethod = xmlSecTransformDefaultPopBin;

/******************************************************************************
 *
 * HMAC transforms
 *
 * xmlSecSymbianCryptoHmacCtx is located after xmlSecTransform
 *
 *****************************************************************************/
#define xmlSecSymbianCryptoHmacGetCtx(transform) \
    ((xmlSecSymbianCryptoHmacCtxPtr)(((xmlSecByte*)(transform)) + sizeof(xmlSecTransform)))
#define xmlSecSymbianCryptoHmacSize	\
    (sizeof(xmlSecTransform) + sizeof(xmlSecSymbianCryptoHmacCtx))
#define xmlSecSymbianCryptoHmacCheckId(transform) \
    (xmlSecTransformCheckId((transform), xmlSecSymbianCryptoTransformHmacSha1Id) || \
     xmlSecTransformCheckId((transform), xmlSecSymbianCryptoTransformHmacMd5Id) || \
     xmlSecTransformCheckId((transform), xmlSecSymbianCryptoTransformHmacRipemd160Id))

static int 	xmlSecSymbianCryptoHmacInitialize		(xmlSecTransformPtr transform);
static void 	xmlSecSymbianCryptoHmacFinalize		(xmlSecTransformPtr transform);
static int 	xmlSecSymbianCryptoHmacNodeRead		(xmlSecTransformPtr transform,
							 xmlNodePtr node,
							 xmlSecTransformCtxPtr transformCtx);
static int  	xmlSecSymbianCryptoHmacSetKeyReq		(xmlSecTransformPtr transform, 
							 xmlSecKeyReqPtr keyReq);
static int  	xmlSecSymbianCryptoHmacSetKey			(xmlSecTransformPtr transform, 
							 xmlSecKeyPtr key);
static int	xmlSecSymbianCryptoHmacVerify			(xmlSecTransformPtr transform, 
							 const xmlSecByte* data, 
							 xmlSecSize dataSize,
							 xmlSecTransformCtxPtr transformCtx);
static int 	xmlSecSymbianCryptoHmacExecute			(xmlSecTransformPtr transform, 
							 int last, 
							 xmlSecTransformCtxPtr transformCtx);

static int 
xmlSecSymbianCryptoHmacInitialize(xmlSecTransformPtr transform) {
    xmlSecSymbianCryptoHmacCtxPtr ctx;
#ifndef XMLSEC_GNUTLS_OLD
    //gpg_err_code_t ret;
#endif /* XMLSEC_GNUTLS_OLD */
	int ret;

    xmlSecAssert2(xmlSecSymbianCryptoHmacCheckId(transform), -1);
    xmlSecAssert2(xmlSecTransformCheckSize(transform, xmlSecSymbianCryptoHmacSize), -1);

    ctx = xmlSecSymbianCryptoHmacGetCtx(transform);
    xmlSecAssert2(ctx, -1);
    
    memset(ctx, 0, sizeof(xmlSecSymbianCryptoHmacCtx));
    if(xmlSecTransformCheckId(transform, xmlSecSymbianCryptoTransformHmacSha1Id)) {
        ctx->digest = SC_MD_SHA1;
    } else if(xmlSecTransformCheckId(transform, xmlSecSymbianCryptoTransformHmacMd5Id)) {
        ctx->digest = SC_MD_MD5;
    } else if(xmlSecTransformCheckId(transform, xmlSecSymbianCryptoTransformHmacRipemd160Id)) {
        ctx->digest = SC_MD_RMD160;
    } else {
	xmlSecError(XMLSEC_ERRORS_HERE, 
		    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
		    NULL,
		    XMLSEC_ERRORS_R_INVALID_TRANSFORM,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }
    
    ret = sc_md_open(&ctx->digestCtx, ctx->digest, SC_MD_FLAG_HMAC | SC_MD_FLAG_SECURE); /* we are paranoid */
    if(ret != 0 /*GPG_ERR_NO_ERROR*/) {
        if ( ret != KErrNone )
            {
            xmlSecSetErrorFlag( ret );
            }
		xmlSecError(XMLSEC_ERRORS_HERE, 
			    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
			    "gcry_md_open",
			    XMLSEC_ERRORS_R_CRYPTO_FAILED,
			    XMLSEC_ERRORS_NO_MESSAGE);
		return(-1);
    }
    
    return(0);
}

static void 
xmlSecSymbianCryptoHmacFinalize(xmlSecTransformPtr transform) {
    xmlSecSymbianCryptoHmacCtxPtr ctx;

    xmlSecAssert(xmlSecSymbianCryptoHmacCheckId(transform));    
    xmlSecAssert(xmlSecTransformCheckSize(transform, xmlSecSymbianCryptoHmacSize));

    ctx = xmlSecSymbianCryptoHmacGetCtx(transform);
    xmlSecAssert(ctx);

    if(ctx->digestCtx) {
	sc_md_close(ctx->digestCtx);
    }
    memset(ctx, 0, sizeof(xmlSecSymbianCryptoHmacCtx));
}

/**
 * xmlSecSymbianCryptoHmacNodeRead:
 *
 * HMAC (http://www.w3.org/TR/xmldsig-core/#sec-HMAC):
 *
 * The HMAC algorithm (RFC2104 [HMAC]) takes the truncation length in bits 
 * as a parameter; if the parameter is not specified then all the bits of the 
 * hash are output. An example of an HMAC SignatureMethod element:  
 * <SignatureMethod Algorithm="http://www.w3.org/2000/09/xmldsig#hmac-sha1">
 *   <HMACOutputLength>128</HMACOutputLength>
 * </SignatureMethod>
 * 
 * Schema Definition:
 * 
 * <simpleType name="HMACOutputLengthType">
 *   <restriction base="integer"/>
 * </simpleType>
 *     
 * DTD:
 *     
 * <!ELEMENT HMACOutputLength (#PCDATA)>
 */
static int
xmlSecSymbianCryptoHmacNodeRead(xmlSecTransformPtr transform, 
                                                xmlNodePtr node, 
                                                xmlSecTransformCtxPtr transformCtx) {
    xmlSecSymbianCryptoHmacCtxPtr ctx;
    xmlNodePtr cur;

    xmlSecAssert2(xmlSecSymbianCryptoHmacCheckId(transform), -1);
    xmlSecAssert2(xmlSecTransformCheckSize(transform, xmlSecSymbianCryptoHmacSize), -1);
    xmlSecAssert2(node, -1);
    xmlSecAssert2(transformCtx, -1);

    ctx = xmlSecSymbianCryptoHmacGetCtx(transform);
    xmlSecAssert2(ctx, -1);

    cur = xmlSecGetNextElementNode(node->children); 
    if((cur) && xmlSecCheckNodeName(cur, xmlSecNodeHMACOutputLength, xmlSecDSigNs)) {  
	xmlChar *content;
	
	content = xmlNodeGetContent(cur);
	if(content) {
	    ctx->dgstSize = atoi((char*)content);	    
	    xmlFree(content);
	}
	cur = xmlSecGetNextElementNode(cur->next);
    }
    
    if(cur) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
		    xmlSecNodeGetName(cur),
		    XMLSEC_ERRORS_R_INVALID_NODE,
		    "no nodes expected");
	return(-1);
    }
    return(0); 
}


static int  
xmlSecSymbianCryptoHmacSetKeyReq(xmlSecTransformPtr transform,  xmlSecKeyReqPtr keyReq) {
    xmlSecSymbianCryptoHmacCtxPtr ctx;

    xmlSecAssert2(xmlSecSymbianCryptoHmacCheckId(transform), -1);
    xmlSecAssert2((transform->operation == xmlSecTransformOperationSign) 
                                || (transform->operation == xmlSecTransformOperationVerify), -1);
    xmlSecAssert2(keyReq, -1);
    xmlSecAssert2(xmlSecTransformCheckSize(transform, xmlSecSymbianCryptoHmacSize), -1);

    ctx = xmlSecSymbianCryptoHmacGetCtx(transform);
    xmlSecAssert2(ctx, -1);

    keyReq->keyId  = xmlSecSymbianCryptoKeyDataHmacId;
    keyReq->keyType= xmlSecKeyDataTypeSymmetric;
    if(transform->operation == xmlSecTransformOperationSign) {
	keyReq->keyUsage = xmlSecKeyUsageSign;
    } else {
	keyReq->keyUsage = xmlSecKeyUsageVerify;
    }
    
    return(0);
}

static int
xmlSecSymbianCryptoHmacSetKey(xmlSecTransformPtr transform, xmlSecKeyPtr key) {
    xmlSecSymbianCryptoHmacCtxPtr ctx;
    xmlSecKeyDataPtr value;
    xmlSecBufferPtr buffer;    
    int ret;
    
    xmlSecAssert2(xmlSecSymbianCryptoHmacCheckId(transform), -1);
    xmlSecAssert2((transform->operation == xmlSecTransformOperationSign) 
                                || (transform->operation == xmlSecTransformOperationVerify), -1);
    xmlSecAssert2(xmlSecTransformCheckSize(transform, xmlSecSymbianCryptoHmacSize), -1);
    xmlSecAssert2(key, -1);

    ctx = xmlSecSymbianCryptoHmacGetCtx(transform);
    xmlSecAssert2(ctx, -1);
    xmlSecAssert2(ctx->digestCtx, -1);
    
    value = xmlSecKeyGetValue(key);
    xmlSecAssert2(xmlSecKeyDataCheckId(value, xmlSecSymbianCryptoKeyDataHmacId), -1);

    buffer = xmlSecKeyDataBinaryValueGetBuffer(value);
    xmlSecAssert2(buffer, -1);

    if(xmlSecBufferGetSize(buffer) == 0) {
	xmlSecError(XMLSEC_ERRORS_HERE, 
		    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
		    NULL,
		    XMLSEC_ERRORS_R_INVALID_KEY_DATA_SIZE,
		    "key is empty");
	return(-1);    
    }    
    ret = sc_md_setkey(ctx->digestCtx, xmlSecBufferGetData(buffer), 
			xmlSecBufferGetSize(buffer));			
    if(ret != 0) {
	xmlSecError(XMLSEC_ERRORS_HERE, 
		    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
		    "gcry_md_setkey",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    "ret=%d", ret);
	return(-1);
    }
    return(0);
}

static int
xmlSecSymbianCryptoHmacVerify(xmlSecTransformPtr transform, 
			const xmlSecByte* data, xmlSecSize dataSize,
			xmlSecTransformCtxPtr transformCtx) {
    static xmlSecByte last_byte_masks[] = 	
		{ 0xFF, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };

    xmlSecSymbianCryptoHmacCtxPtr ctx;
    xmlSecByte mask;
        
    xmlSecAssert2(xmlSecTransformIsValid(transform), -1);
    xmlSecAssert2(xmlSecTransformCheckSize(transform, xmlSecSymbianCryptoHmacSize), -1);
    xmlSecAssert2(transform->operation == xmlSecTransformOperationVerify, -1);
    xmlSecAssert2(transform->status == xmlSecTransformStatusFinished, -1);
    xmlSecAssert2(data, -1);
    xmlSecAssert2(transformCtx, -1);

    ctx = xmlSecSymbianCryptoHmacGetCtx(transform);
    xmlSecAssert2(ctx, -1);
    xmlSecAssert2(ctx->digestCtx, -1);
    xmlSecAssert2(ctx->dgstSize > 0, -1);
    
    /* compare the digest size in bytes */
    if(dataSize != ((ctx->dgstSize + 7) / 8)){
	xmlSecError(XMLSEC_ERRORS_HERE, 
		    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
		    NULL,
		    XMLSEC_ERRORS_R_INVALID_SIZE,
		    "data=%d;dgst=%d",
		    dataSize, ((ctx->dgstSize + 7) / 8));
	transform->status = xmlSecTransformStatusFail;
	return(0);
    }

    /* we check the last byte separatelly */
    xmlSecAssert2(dataSize > 0, -1);
    mask = last_byte_masks[ctx->dgstSize % 8];
    if((ctx->dgst[dataSize - 1] & mask) != (data[dataSize - 1]  & mask)) {
	xmlSecError(XMLSEC_ERRORS_HERE, 
		    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
		    NULL,
		    XMLSEC_ERRORS_R_DATA_NOT_MATCH,
		    "data and digest do not match (last byte)");
	transform->status = xmlSecTransformStatusFail;
	return(0);
    }

    /* now check the rest of the digest */
    if((dataSize > 1) && (memcmp(ctx->dgst, data, dataSize - 1) != 0)) {
	xmlSecError(XMLSEC_ERRORS_HERE, 
		    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
		    NULL,
		    XMLSEC_ERRORS_R_DATA_NOT_MATCH,
		    "data and digest do not match");
	transform->status = xmlSecTransformStatusFail;
	return(0);
    }
    
    transform->status = xmlSecTransformStatusOk;
    return(0);
}

static int 
xmlSecSymbianCryptoHmacExecute(xmlSecTransformPtr transform, 
                                            int last, 
                                            xmlSecTransformCtxPtr transformCtx) {
    xmlSecSymbianCryptoHmacCtxPtr ctx;
    xmlSecBufferPtr in, out;
    const xmlSecByte* dgst;
    xmlSecSize dgstSize;
    int ret;
    
    xmlSecAssert2(xmlSecSymbianCryptoHmacCheckId(transform), -1);
    xmlSecAssert2((transform->operation == xmlSecTransformOperationSign) 
                                || (transform->operation == xmlSecTransformOperationVerify), -1);
    xmlSecAssert2(xmlSecTransformCheckSize(transform, xmlSecSymbianCryptoHmacSize), -1);
    xmlSecAssert2(transformCtx, -1);

    ctx = xmlSecSymbianCryptoHmacGetCtx(transform);
    xmlSecAssert2(ctx, -1);
    xmlSecAssert2(ctx->digestCtx, -1);

    in = &(transform->inBuf);
    out = &(transform->outBuf);

    if(transform->status == xmlSecTransformStatusNone) {
	transform->status = xmlSecTransformStatusWorking;
    }
    
    if(transform->status == xmlSecTransformStatusWorking) {
	xmlSecSize inSize;

	inSize = xmlSecBufferGetSize(in);
	if(inSize > 0) {
	    sc_md_write(ctx->digestCtx, xmlSecBufferGetData(in), inSize);
	    
	    ret = xmlSecBufferRemoveHead(in, inSize);
	    if(ret < 0) {
		xmlSecError(XMLSEC_ERRORS_HERE, 
			    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
			    "xmlSecBufferRemoveHead",
			    XMLSEC_ERRORS_R_XMLSEC_FAILED,
			    "size=%d", inSize);
		return(-1);
	    }
	}
	if(last) {	    
	    /* get the final digest */
	    dgstSize = sc_md_get_algo_dlen(ctx->digestCtx);
	    sc_md_final(ctx->digestCtx);	    
	    dgst = sc_md_read(ctx->digestCtx, ctx->digest);
	    if(!dgst) {
		xmlSecError(XMLSEC_ERRORS_HERE, 
			    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
			    "gcry_md_read",
			    XMLSEC_ERRORS_R_CRYPTO_FAILED,
			    XMLSEC_ERRORS_NO_MESSAGE);
	        return(-1);
	    }
	    
	    /* copy it to our internal buffer */
	    //dgstSize = sc_md_get_algo_dlen(ctx->digest);
	    xmlSecAssert2(dgstSize > 0, -1);
	    xmlSecAssert2(dgstSize <= sizeof(ctx->dgst), -1);
	    memcpy(ctx->dgst, dgst, dgstSize);

	    /* check/set the result digest size */
	    if(ctx->dgstSize == 0) {
		ctx->dgstSize = dgstSize * 8; /* no dgst size specified, use all we have */
	    } else if(ctx->dgstSize <= 8 * dgstSize) {
		dgstSize = ((ctx->dgstSize + 7) / 8); /* we need to truncate result digest */
	    } else {
		xmlSecError(XMLSEC_ERRORS_HERE, 
			    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
			    NULL,
			    XMLSEC_ERRORS_R_INVALID_SIZE,
			    "result-bits=%d;required-bits=%d",
			    8 * dgstSize, ctx->dgstSize);
		return(-1);
	    }

	    if(transform->operation == xmlSecTransformOperationSign) {
		ret = xmlSecBufferAppend(out, ctx->dgst, dgstSize);
		if(ret < 0) {
		    xmlSecError(XMLSEC_ERRORS_HERE, 
				xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
				"xmlSecBufferAppend",
				XMLSEC_ERRORS_R_XMLSEC_FAILED,
				"size=%d", dgstSize);
		    return(-1);
		}
	    }
	    transform->status = xmlSecTransformStatusFinished;
	}
    } else if(transform->status == xmlSecTransformStatusFinished) {
	/* the only way we can get here is if there is no input */
	xmlSecAssert2(xmlSecBufferGetSize(&(transform->inBuf)) == 0, -1);
    } else {
	xmlSecError(XMLSEC_ERRORS_HERE, 
		    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
		    NULL,
		    XMLSEC_ERRORS_R_INVALID_STATUS,
		    "size=%d", transform->status);
	return(-1);
    }
    
    return(0);
}

/** 
 * HMAC SHA1
 */
static xmlSecTransformKlass xmlSecSymbianCryptoHmacSha1Klass = {
    /* klass/object sizes */
    sizeof(xmlSecTransformKlass),		/* xmlSecSize klassSize */
    xmlSecSymbianCryptoHmacSize,			/* xmlSecSize objSize */

    xmlSecNameHmacSha1,				/* const xmlChar* name; */
    xmlSecHrefHmacSha1, 			/* const xmlChar *href; */
    xmlSecTransformUsageSignatureMethod,	/* xmlSecTransformUsage usage; */
    
    xmlSecSymbianCryptoHmacInitialize,			/* xmlSecTransformInitializeMethod initialize; */
    xmlSecSymbianCryptoHmacFinalize,			/* xmlSecTransformFinalizeMethod finalize; */
    xmlSecSymbianCryptoHmacNodeRead,			/* xmlSecTransformNodeReadMethod readNode; */
    NULL,					/* xmlSecTransformNodeWriteMethod writeNode; */
    xmlSecSymbianCryptoHmacSetKeyReq,			/* xmlSecTransformSetKeyReqMethod setKeyReq; */
    xmlSecSymbianCryptoHmacSetKey,			/* xmlSecTransformSetKeyMethod setKey; */
    xmlSecSymbianCryptoHmacVerify,			/* xmlSecTransformValidateMethod validate; */
    xmlSecCrpytoGetDataTypeMethod,		/* xmlSecTransformGetDataTypeMethod getDataType; */
    xmlSecCryptoPushBinMethod,		/* xmlSecTransformPushBinMethod pushBin; */
    xmlSecCryptoPopBinMethod,		/* xmlSecTransformPopBinMethod popBin; */
    NULL,					/* xmlSecTransformPushXmlMethod pushXml; */
    NULL,					/* xmlSecTransformPopXmlMethod popXml; */
    xmlSecSymbianCryptoHmacExecute,			/* xmlSecTransformExecuteMethod execute; */
    
    NULL,					/* void* reserved0; */
    NULL,					/* void* reserved1; */
};

/** 
 * xmlSecSymbianCryptoTransformHmacSha1GetKlass:
 *
 * The HMAC-SHA1 transform klass.
 *
 * Returns the HMAC-SHA1 transform klass.
 */
EXPORT_C
xmlSecTransformId 
xmlSecSymbianCryptoTransformHmacSha1GetKlass(void) {
    return(&xmlSecSymbianCryptoHmacSha1Klass);
}

/** 
 * HMAC Ripemd160
 */
static xmlSecTransformKlass xmlSecSymbianCryptoHmacRipemd160Klass = {
    /* klass/object sizes */
    sizeof(xmlSecTransformKlass),		/* xmlSecSize klassSize */
    xmlSecSymbianCryptoHmacSize,			/* xmlSecSize objSize */

    xmlSecNameHmacRipemd160,			/* const xmlChar* name; */
    xmlSecHrefHmacRipemd160, 			/* const xmlChar* href; */
    xmlSecTransformUsageSignatureMethod,	/* xmlSecTransformUsage usage; */
    
    xmlSecSymbianCryptoHmacInitialize,			/* xmlSecTransformInitializeMethod initialize; */
    xmlSecSymbianCryptoHmacFinalize,			/* xmlSecTransformFinalizeMethod finalize; */
    xmlSecSymbianCryptoHmacNodeRead,			/* xmlSecTransformNodeReadMethod readNode; */
    NULL,					/* xmlSecTransformNodeWriteMethod writeNode; */
    xmlSecSymbianCryptoHmacSetKeyReq,			/* xmlSecTransformSetKeyReqMethod setKeyReq; */
    xmlSecSymbianCryptoHmacSetKey,			/* xmlSecTransformSetKeyMethod setKey; */
    xmlSecSymbianCryptoHmacVerify,			/* xmlSecTransformValidateMethod validate; */
    xmlSecCrpytoGetDataTypeMethod,		/* xmlSecTransformGetDataTypeMethod getDataType; */
    xmlSecCryptoPushBinMethod,		/* xmlSecTransformPushBinMethod pushBin; */
    xmlSecCryptoPopBinMethod,		/* xmlSecTransformPopBinMethod popBin; */
    NULL,					/* xmlSecTransformPushXmlMethod pushXml; */
    NULL,					/* xmlSecTransformPopXmlMethod popXml; */
    xmlSecSymbianCryptoHmacExecute,			/* xmlSecTransformExecuteMethod execute; */
    
    NULL,					/* void* reserved0; */
    NULL,					/* void* reserved1; */
};

/** 
 * xmlSecSymbianCryptoTransformHmacRipemd160GetKlass:
 *
 * The HMAC-RIPEMD160 transform klass.
 *
 * Returns the HMAC-RIPEMD160 transform klass.
 */
EXPORT_C
xmlSecTransformId 
xmlSecSymbianCryptoTransformHmacRipemd160GetKlass(void) {
    return(&xmlSecSymbianCryptoHmacRipemd160Klass);
}

/** 
 * HMAC Md5
 */
static xmlSecTransformKlass xmlSecSymbianCryptoHmacMd5Klass = {
    /* klass/object sizes */
    sizeof(xmlSecTransformKlass),		/* xmlSecSize klassSize */
    xmlSecSymbianCryptoHmacSize,			/* xmlSecSize objSize */

    xmlSecNameHmacMd5,				/* const xmlChar* name; */
    xmlSecHrefHmacMd5, 				/* const xmlChar* href; */
    xmlSecTransformUsageSignatureMethod,	/* xmlSecTransformUsage usage; */
    
    xmlSecSymbianCryptoHmacInitialize,			/* xmlSecTransformInitializeMethod initialize; */
    xmlSecSymbianCryptoHmacFinalize,			/* xmlSecTransformFinalizeMethod finalize; */
    xmlSecSymbianCryptoHmacNodeRead,			/* xmlSecTransformNodeReadMethod readNode; */
    NULL,					/* xmlSecTransformNodeWriteMethod writeNode; */
    xmlSecSymbianCryptoHmacSetKeyReq,			/* xmlSecTransformSetKeyReqMethod setKeyReq; */
    xmlSecSymbianCryptoHmacSetKey,			/* xmlSecTransformSetKeyMethod setKey; */
    xmlSecSymbianCryptoHmacVerify,			/* xmlSecTransformValidateMethod validate; */
    xmlSecCrpytoGetDataTypeMethod,		/* xmlSecTransformGetDataTypeMethod getDataType; */
    xmlSecCryptoPushBinMethod,		/* xmlSecTransformPushBinMethod pushBin; */
    xmlSecCryptoPopBinMethod,		/* xmlSecTransformPopBinMethod popBin; */
    NULL,					/* xmlSecTransformPushXmlMethod pushXml; */
    NULL,					/* xmlSecTransformPopXmlMethod popXml; */
    xmlSecSymbianCryptoHmacExecute,			/* xmlSecTransformExecuteMethod execute; */
    
    NULL,					/* void* reserved0; */
    NULL,					/* void* reserved1; */
};

/** 
 * xmlSecSymbianCryptoTransformHmacMd5GetKlass:
 *
 * The HMAC-MD5 transform klass.
 *
 * Returns the HMAC-MD5 transform klass.
 */
EXPORT_C
xmlSecTransformId 
xmlSecSymbianCryptoTransformHmacMd5GetKlass(void) {
    return(&xmlSecSymbianCryptoHmacMd5Klass);
}

#endif /* XMLSEC_NO_HMAC */
