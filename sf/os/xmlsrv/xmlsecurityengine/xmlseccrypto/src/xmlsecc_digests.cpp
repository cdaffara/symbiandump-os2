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
#include <e32err.h>
#include "xmlsecc_config.h"
#include "xmlsec_xmlsec.h"
#include "xmlsec_keys.h"
#include "xmlsec_transforms.h"
#include "xmlsec_errors.h"
#include "xmlsec_error_flag.h"

#include "xmlsecc_app.h"
#include "xmlsecc_crypto.h"
#include "xmlsecc_cryptowrapper.h"		// replace gnutils/gnutls.h

//#define XMLSEC_SYMBIAN_MAX_DIGEST_SIZE		32

/**************************************************************************
 *
 * Internal SymbianCrypto Digest CTX
 *
 *****************************************************************************/
typedef struct _xmlSecSymbianCryptoDigestCtx 
                               xmlSecSymbianCryptoDigestCtx, *xmlSecSymbianCryptoDigestCtxPtr;
struct _xmlSecSymbianCryptoDigestCtx {
    int			digest;
    ScMDHd		digestCtx;
    xmlSecByte	 	dgst[XMLSEC_SYMBIAN_MAX_DIGEST_SIZE];
    xmlSecSize		dgstSize;	/* dgst size in bytes */
};	    

xmlSecTransformGetDataTypeMethod const xmlSecCrpytoGetDataTypeMethod  = xmlSecTransformDefaultGetDataType;
xmlSecTransformPushBinMethod const xmlSecCryptoPushBinMethod = xmlSecTransformDefaultPushBin;
xmlSecTransformPopBinMethod const xmlSecCryptoPopBinMethod = xmlSecTransformDefaultPopBin;

/******************************************************************************
 *
 * Digest transforms
 *
 * xmlSecSymbianCryptoDigestCtx is located after xmlSecTransform
 *
 *****************************************************************************/
#define xmlSecSymbianCryptoDigestSize	\
    (sizeof(xmlSecTransform) + sizeof(xmlSecSymbianCryptoDigestCtx))	
#define xmlSecSymbianCryptoDigestGetCtx(transform) \
    ((xmlSecSymbianCryptoDigestCtxPtr)(((xmlSecByte*)(transform)) + sizeof(xmlSecTransform)))

static int 	xmlSecSymbianCryptoDigestInitialize		(xmlSecTransformPtr transform);
static void 	xmlSecSymbianCryptoDigestFinalize		(xmlSecTransformPtr transform);
static int	xmlSecSymbianCryptoDigestVerify		(xmlSecTransformPtr transform, 
							 const xmlSecByte* data, 
							 xmlSecSize dataSize,
							 xmlSecTransformCtxPtr transformCtx);
static int 	xmlSecSymbianCryptoDigestExecute		(xmlSecTransformPtr transform, 
							 int last, 
							 xmlSecTransformCtxPtr transformCtx);
static int	xmlSecSymbianCryptoDigestCheckId		(xmlSecTransformPtr transform);

static int
xmlSecSymbianCryptoDigestCheckId(xmlSecTransformPtr transform) {

#ifndef XMLSEC_NO_SHA1
    if(xmlSecTransformCheckId(transform, xmlSecSymbianCryptoTransformSha1Id)) {
	return(1);
    }
#endif /* XMLSEC_NO_SHA1 */    

    return(0);
}

static int 
xmlSecSymbianCryptoDigestInitialize(xmlSecTransformPtr transform) {
    xmlSecSymbianCryptoDigestCtxPtr ctx;
#ifndef XMLSEC_GNUTLS_OLD
    TInt ret;
#endif /* XMLSEC_GNUTLS_OLD */

    xmlSecAssert2(xmlSecSymbianCryptoDigestCheckId(transform), -1);
    xmlSecAssert2(xmlSecTransformCheckSize(transform, xmlSecSymbianCryptoDigestSize), -1);

    ctx = xmlSecSymbianCryptoDigestGetCtx(transform);
    xmlSecAssert2(ctx, -1);

    /* initialize context */
    memset(ctx, 0, sizeof(xmlSecSymbianCryptoDigestCtx));

#ifndef XMLSEC_NO_SHA1
    if(xmlSecTransformCheckId(transform, xmlSecSymbianCryptoTransformSha1Id)) {
	ctx->digest = SC_MD_SHA1;
    } else
#endif /* XMLSEC_NO_SHA1 */    	

    if(1) {
	xmlSecError(XMLSEC_ERRORS_HERE, 
		    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
		    NULL,
		    XMLSEC_ERRORS_R_INVALID_TRANSFORM,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }
    
#ifndef XMLSEC_GNUTLS_OLD
    ret = sc_md_open(&ctx->digestCtx, ctx->digest, SC_MD_FLAG_SECURE); /* we are paranoid */
    if(ret != KErrNone) {
#else /* XMLSEC_GNUTLS_OLD */
    ctx->digestCtx = sc_md_open(ctx->digest, SC_MD_FLAG_SECURE); /* we are paranoid */
    if(!ctx->digestCtx) {
#endif /* XMLSEC_GNUTLS_OLD */
    if ( ret != KErrNone )
        {
        xmlSecSetErrorFlag( ret );
        }
	xmlSecError(XMLSEC_ERRORS_HERE, 
		    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
		    "sc_md_open",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }
    return(0);
}

static void 
xmlSecSymbianCryptoDigestFinalize(xmlSecTransformPtr transform) {
    xmlSecSymbianCryptoDigestCtxPtr ctx;

    xmlSecAssert(xmlSecSymbianCryptoDigestCheckId(transform));
    xmlSecAssert(xmlSecTransformCheckSize(transform, xmlSecSymbianCryptoDigestSize));

    ctx = xmlSecSymbianCryptoDigestGetCtx(transform);
    xmlSecAssert(ctx);
    
    if(ctx->digestCtx) {
	sc_md_close(ctx->digestCtx);
    }
    memset(ctx, 0, sizeof(xmlSecSymbianCryptoDigestCtx));
}

static int
xmlSecSymbianCryptoDigestVerify(xmlSecTransformPtr transform, 
			const xmlSecByte* data, xmlSecSize dataSize,
			xmlSecTransformCtxPtr transformCtx) {
    xmlSecSymbianCryptoDigestCtxPtr ctx;
    
    xmlSecAssert2(xmlSecSymbianCryptoDigestCheckId(transform), -1);
    xmlSecAssert2(transform->operation == xmlSecTransformOperationVerify, -1);
    xmlSecAssert2(xmlSecTransformCheckSize(transform, xmlSecSymbianCryptoDigestSize), -1);
    xmlSecAssert2(transform->status == xmlSecTransformStatusFinished, -1);
    xmlSecAssert2(data, -1);
    xmlSecAssert2(transformCtx, -1);

    ctx = xmlSecSymbianCryptoDigestGetCtx(transform);
    xmlSecAssert2(ctx, -1);
    xmlSecAssert2(ctx->dgstSize > 0, -1);
    
    if(dataSize != ctx->dgstSize) {
	xmlSecError(XMLSEC_ERRORS_HERE, 
		    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
		    NULL,
		    XMLSEC_ERRORS_R_INVALID_DATA,
		    "data and digest sizes are different (data=%d, dgst=%d)", 
		    dataSize, ctx->dgstSize);
	transform->status = xmlSecTransformStatusFail;
	return(0);
    }
    
    if(memcmp(ctx->dgst, data, dataSize) != 0) {
	xmlSecError(XMLSEC_ERRORS_HERE, 
		    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
		    NULL,
		    XMLSEC_ERRORS_R_INVALID_DATA,
		    "data and digest do not match");
	transform->status = xmlSecTransformStatusFail;
	return(0);
    }
    
    transform->status = xmlSecTransformStatusOk;
    return(0);
}

static int 
xmlSecSymbianCryptoDigestExecute(xmlSecTransformPtr transform, 
                                        int last, 
                                        xmlSecTransformCtxPtr transformCtx) {
    xmlSecSymbianCryptoDigestCtxPtr ctx;
    xmlSecBufferPtr in, out;
    int ret;
    
    xmlSecAssert2(xmlSecSymbianCryptoDigestCheckId(transform), -1);
    xmlSecAssert2((transform->operation == xmlSecTransformOperationSign) 
                                || (transform->operation == xmlSecTransformOperationVerify), -1);
    xmlSecAssert2(transformCtx, -1);
    xmlSecAssert2(xmlSecTransformCheckSize(transform, xmlSecSymbianCryptoDigestSize), -1);

    ctx = xmlSecSymbianCryptoDigestGetCtx(transform);
    xmlSecAssert2(ctx, -1);
    xmlSecAssert2(ctx->digest != SC_MD_NONE, -1);
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
	    const xmlSecByte* buf;
	    
	    /* get the final digest */
	    sc_md_final(ctx->digestCtx);
	    
	    /* get length before sc_md_read */
	    ctx->dgstSize = sc_md_get_algo_dlen(ctx->digestCtx);
	    xmlSecAssert2(ctx->dgstSize > 0, -1);
	    xmlSecAssert2(ctx->dgstSize <= sizeof(ctx->dgst), -1);
	    
	    buf = sc_md_read(ctx->digestCtx, ctx->digest);
	    if(!buf) {
		xmlSecError(XMLSEC_ERRORS_HERE, 
			    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
			    "sc_md_read",
			    XMLSEC_ERRORS_R_CRYPTO_FAILED,
			    XMLSEC_ERRORS_NO_MESSAGE);
	        return(-1);
	    }
	    
	    /* copy it to our internal buffer */
	    memcpy(ctx->dgst, buf, ctx->dgstSize);

	    /* and to the output if needed */
	    if(transform->operation == xmlSecTransformOperationSign) {
		ret = xmlSecBufferAppend(out, ctx->dgst, ctx->dgstSize);
		if(ret < 0) {
		    xmlSecError(XMLSEC_ERRORS_HERE, 
				xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
				"xmlSecBufferAppend",
				XMLSEC_ERRORS_R_XMLSEC_FAILED,
				"size=%d", ctx->dgstSize);
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
		    "status=%d", transform->status);
	return(-1);
    }
    
    return(0);
}

#ifndef XMLSEC_NO_SHA1
/******************************************************************************
 *
 * SHA1 Digest transforms
 *
 *****************************************************************************/
static xmlSecTransformKlass xmlSecSymbianCryptoSha1Klass = {
    /* klass/object sizes */
    sizeof(xmlSecTransformKlass),		/* xmlSecSize klassSize */
    xmlSecSymbianCryptoDigestSize,			/* xmlSecSize objSize */

    /* data */
    xmlSecNameSha1,						/* const xmlChar* name; */
    xmlSecHrefSha1, 						/* const xmlChar* href; */
    xmlSecTransformUsageDigestMethod,		/* xmlSecTransformUsage usage; */
    
    /* methods */
    xmlSecSymbianCryptoDigestInitialize,		/* xmlSecTransformInitializeMethod initialize; */
    xmlSecSymbianCryptoDigestFinalize,			/* xmlSecTransformFinalizeMethod finalize; */
    NULL,					/* xmlSecTransformNodeReadMethod readNode; */
    NULL,					/* xmlSecTransformNodeWriteMethod writeNode; */
    NULL,					/* xmlSecTransformSetKeyReqMethod setKeyReq; */
    NULL,					/* xmlSecTransformSetKeyMethod setKey; */
    xmlSecSymbianCryptoDigestVerify,			/* xmlSecTransformVerifyMethod verify; */
    xmlSecCrpytoGetDataTypeMethod,		/* xmlSecTransformGetDataTypeMethod getDataType; */
    xmlSecCryptoPushBinMethod,		/* xmlSecTransformPushBinMethod pushBin; */
    xmlSecCryptoPopBinMethod,		/* xmlSecTransformPopBinMethod popBin; */
    NULL,					/* xmlSecTransformPushXmlMethod pushXml; */
    NULL,					/* xmlSecTransformPopXmlMethod popXml; */
    xmlSecSymbianCryptoDigestExecute,			/* xmlSecTransformExecuteMethod execute; */
    
    NULL,					/* void* reserved0; */
    NULL,					/* void* reserved1; */
};

/** 
 * xmlSecSymbianCryptoTransformSha1GetKlass:
 *
 * SHA-1 digest transform klass.
 *
 * Returns pointer to SHA-1 digest transform klass.
 */
EXPORT_C
xmlSecTransformId 
xmlSecSymbianCryptoTransformSha1GetKlass(void) {
    return(&xmlSecSymbianCryptoSha1Klass);
}
#endif /* XMLSEC_NO_SHA1 */




