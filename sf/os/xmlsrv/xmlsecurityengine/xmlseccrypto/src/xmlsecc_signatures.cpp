/** 
 * XMLSec library
 *
 * This is free software; see Copyright file in the source
 * distribution for preciese wording.
 * 
 * Copyright (C) 2002-2003 Aleksey Sanin <aleksey@aleksey.com>
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */
//#include "globals.h"
#include "xmlsecc_globals.h"

#include <string.h>
#include <e32err.h>
#include "xmlsecc_config.h"
#include "xmlsec_xmlsec.h"
#include "xmlsec_keys.h"
#include "xmlsec_transforms.h"
#include "xmlsec_errors.h"
#include "xmlsec_error_flag.h"

#include "xmlsecc_crypto.h"
#include "xmlsecc_evp.h"
#include "xmlsecc_cryptowrapper.h"

#ifndef XMLSEC_NO_DSA
static const TInt KXmlSecSymbianCryptoDsaSignatureSize(40);
static const EVP_MD *xmlSecSymbianCryptoDsaSha1Evp			(void);
#endif /* XMLSEC_NO_DSA */


/**************************************************************************
 *
 * Internal SymbianCrypto evp signatures ctx
 *
 *****************************************************************************/
typedef struct _xmlSecSymbianCryptoEvpSignatureCtx	xmlSecSymbianCryptoEvpSignatureCtx, 
						*xmlSecSymbianCryptoEvpSignatureCtxPtr;
struct _xmlSecSymbianCryptoEvpSignatureCtx {
    int					digest;
    EVP_MD_CTX		digestCtx;
    xmlSecKeyDataId	keyId;
    EVP_PKEY* 		pKey;
};	    

xmlSecTransformGetDataTypeMethod const xmlSecCrpytoGetDataTypeMethod  = xmlSecTransformDefaultGetDataType;
xmlSecTransformPushBinMethod const xmlSecCryptoPushBinMethod = xmlSecTransformDefaultPushBin;
xmlSecTransformPopBinMethod const xmlSecCryptoPopBinMethod = xmlSecTransformDefaultPopBin;

/******************************************************************************
 *
 * EVP Signature transforms
 *
 * xmlSecSymbianCryptoEvpSignatureCtx is located after xmlSecTransform
 *
 *****************************************************************************/
#define xmlSecSymbianCryptoEvpSignatureSize	\
    (sizeof(xmlSecTransform) + sizeof(xmlSecSymbianCryptoEvpSignatureCtx))
#define xmlSecSymbianCryptoEvpSignatureGetCtx(transform) \
    ((xmlSecSymbianCryptoEvpSignatureCtxPtr)(((xmlSecByte*)(transform)) + sizeof(xmlSecTransform)))

static int	xmlSecSymbianCryptoEvpSignatureCheckId		(xmlSecTransformPtr transform);
static int	xmlSecSymbianCryptoEvpSignatureInitialize		(xmlSecTransformPtr transform);
static void	xmlSecSymbianCryptoEvpSignatureFinalize		(xmlSecTransformPtr transform);
static int  	xmlSecSymbianCryptoEvpSignatureSetKeyReq		(xmlSecTransformPtr transform, 
								 xmlSecKeyReqPtr keyReq);
static int	xmlSecSymbianCryptoEvpSignatureSetKey			(xmlSecTransformPtr transform,
								 xmlSecKeyPtr key);
static int  	xmlSecSymbianCryptoEvpSignatureVerify			(xmlSecTransformPtr transform, 
								 const xmlSecByte* data,
								 xmlSecSize dataSize,
								 xmlSecTransformCtxPtr transformCtx);
static int	xmlSecSymbianCryptoEvpSignatureExecute		(xmlSecTransformPtr transform, 
								 int last,
								 xmlSecTransformCtxPtr transformCtx);

static int
xmlSecSymbianCryptoEvpSignatureCheckId(xmlSecTransformPtr transform) {
#ifndef XMLSEC_NO_DSA

#ifndef XMLSEC_NO_SHA1
    if(xmlSecTransformCheckId(transform, xmlSecSymbianCryptoTransformDsaSha1Id)) {
	return(1);
    } else
#endif /* XMLSEC_NO_SHA1 */

#endif /* XMLSEC_NO_DSA */

#ifndef XMLSEC_NO_RSA

#ifndef XMLSEC_NO_MD5
    if(xmlSecTransformCheckId(transform, xmlSecSymbianCryptoTransformRsaMd5Id)) {
	return(1);
    } else 
#endif /* XMLSEC_NO_MD5 */

#ifndef XMLSEC_NO_RIPEMD160
    if(xmlSecTransformCheckId(transform, xmlSecSymbianCryptoTransformRsaRipemd160Id)) {
	return(1);
    } else 
#endif /* XMLSEC_NO_RIPEMD160 */

#ifndef XMLSEC_NO_SHA1
    if(xmlSecTransformCheckId(transform, xmlSecSymbianCryptoTransformRsaSha1Id)) {
	return(1);
    } else 
#endif /* XMLSEC_NO_SHA1 */

#ifndef XMLSEC_NO_SHA224
    if(xmlSecTransformCheckId(transform, xmlSecSymbianCryptoTransformRsaSha224Id)) {
	return(1);
    } else 
#endif /* XMLSEC_NO_SHA224 */

#ifndef XMLSEC_NO_SHA256
    if(xmlSecTransformCheckId(transform, xmlSecSymbianCryptoTransformRsaSha256Id)) {
	return(1);
    } else 
#endif /* XMLSEC_NO_SHA256 */

#ifndef XMLSEC_NO_SHA384
    if(xmlSecTransformCheckId(transform, xmlSecSymbianCryptoTransformRsaSha384Id)) {
	return(1);
    } else 
#endif /* XMLSEC_NO_SHA384 */

#ifndef XMLSEC_NO_SHA512
    if(xmlSecTransformCheckId(transform, xmlSecSymbianCryptoTransformRsaSha512Id)) {
	return(1);
    } else 
#endif /* XMLSEC_NO_SHA512 */

#endif /* XMLSEC_NO_RSA */
    
    {
	return(0);
    }
    
  //  return(0);
}

static int 
xmlSecSymbianCryptoEvpSignatureInitialize(xmlSecTransformPtr transform) {
    xmlSecSymbianCryptoEvpSignatureCtxPtr ctx;
    xmlSecAssert2(xmlSecSymbianCryptoEvpSignatureCheckId(transform), -1);
    xmlSecAssert2(xmlSecTransformCheckSize(transform, xmlSecSymbianCryptoEvpSignatureSize), -1);

    ctx = xmlSecSymbianCryptoEvpSignatureGetCtx(transform);
    xmlSecAssert2(ctx, -1);

    memset(ctx, 0, sizeof(xmlSecSymbianCryptoEvpSignatureCtx));    

#ifndef XMLSEC_NO_DSA

#ifndef XMLSEC_NO_SHA1
    if(xmlSecTransformCheckId(transform, xmlSecSymbianCryptoTransformDsaSha1Id)) {
	ctx->digest	= xmlSecSymbianCryptoDsaSha1Evp();
	ctx->keyId	= xmlSecSymbianCryptoKeyDataDsaId;
    } else 
#endif /* XMLSEC_NO_SHA1 */

#endif /* XMLSEC_NO_DSA */

#ifndef XMLSEC_NO_RSA

#ifndef XMLSEC_NO_MD5
    if(xmlSecTransformCheckId(transform, xmlSecSymbianCryptoTransformRsaMd5Id)) {
	ctx->digest	= EVP_md5();
	ctx->keyId	= xmlSecSymbianCryptoKeyDataRsaId;
    } else 
#endif /* XMLSEC_NO_MD5 */

#ifndef XMLSEC_NO_RIPEMD160
    if(xmlSecTransformCheckId(transform, xmlSecSymbianCryptoTransformRsaRipemd160Id)) {
	ctx->digest	= EVP_ripemd160();
	ctx->keyId	= xmlSecSymbianCryptoKeyDataRsaId;
    } else 
#endif /* XMLSEC_NO_RIPEMD160 */

#ifndef XMLSEC_NO_SHA1
    if(xmlSecTransformCheckId(transform, xmlSecSymbianCryptoTransformRsaSha1Id)) {
//	ctx->digest	= EVP_sha1();
    ctx->digest = SC_MD_SHA1;
	ctx->keyId	= xmlSecSymbianCryptoKeyDataRsaId;
    } else 
#endif /* XMLSEC_NO_SHA1 */

#ifndef XMLSEC_NO_SHA224
    if(xmlSecTransformCheckId(transform, xmlSecSymbianCryptoTransformRsaSha224Id)) {
	ctx->digest	= EVP_sha224();
	ctx->keyId	= xmlSecSymbianCryptoKeyDataRsaId;
    } else 
#endif /* XMLSEC_NO_SHA224 */

#ifndef XMLSEC_NO_SHA256
    if(xmlSecTransformCheckId(transform, xmlSecSymbianCryptoTransformRsaSha256Id)) {
	ctx->digest	= EVP_sha256();
	ctx->keyId	= xmlSecSymbianCryptoKeyDataRsaId;
    } else 
#endif /* XMLSEC_NO_SHA256 */

#ifndef XMLSEC_NO_SHA384
    if(xmlSecTransformCheckId(transform, xmlSecSymbianCryptoTransformRsaSha384Id)) {
	ctx->digest	= EVP_sha384();
	ctx->keyId	= xmlSecSymbianCryptoKeyDataRsaId;
    } else 
#endif /* XMLSEC_NO_SHA384 */

#ifndef XMLSEC_NO_SHA512
    if(xmlSecTransformCheckId(transform, xmlSecSymbianCryptoTransformRsaSha512Id)) {
	ctx->digest	= EVP_sha512();
	ctx->keyId	= xmlSecSymbianCryptoKeyDataRsaId;
    } else 
#endif /* XMLSEC_NO_SHA512 */

#endif /* XMLSEC_NO_RSA */

    if(1) {
	xmlSecError(XMLSEC_ERRORS_HERE, 
		    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
		    NULL,
		    XMLSEC_ERRORS_R_INVALID_TRANSFORM,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }

    return(0);
}

static void 
xmlSecSymbianCryptoEvpSignatureFinalize(xmlSecTransformPtr transform) {
    xmlSecSymbianCryptoEvpSignatureCtxPtr ctx;

    xmlSecAssert(xmlSecSymbianCryptoEvpSignatureCheckId(transform));
    xmlSecAssert(xmlSecTransformCheckSize(transform, xmlSecSymbianCryptoEvpSignatureSize));

    ctx = xmlSecSymbianCryptoEvpSignatureGetCtx(transform);
    xmlSecAssert(ctx);

    
    if (ctx->pKey) {
	sc_pkey_free(ctx->pKey);
    }
    
    if (ctx->digestCtx)
    {
    	sc_md_close(ctx->digestCtx);
    }

    memset(ctx, 0, sizeof(xmlSecSymbianCryptoEvpSignatureCtx));    
}

static int 
xmlSecSymbianCryptoEvpSignatureSetKey(xmlSecTransformPtr transform, xmlSecKeyPtr key) {
    xmlSecSymbianCryptoEvpSignatureCtxPtr ctx;
    xmlSecKeyDataPtr value;
    EVP_PKEY* pKey;

    xmlSecAssert2(xmlSecSymbianCryptoEvpSignatureCheckId(transform), -1);
    xmlSecAssert2((transform->operation == xmlSecTransformOperationSign) 
                                || (transform->operation == xmlSecTransformOperationVerify), -1);
    xmlSecAssert2(xmlSecTransformCheckSize(transform, xmlSecSymbianCryptoEvpSignatureSize), -1);
    xmlSecAssert2(key, -1);

    ctx = xmlSecSymbianCryptoEvpSignatureGetCtx(transform);
    xmlSecAssert2(ctx, -1);
    xmlSecAssert2(ctx->digest, -1);
    xmlSecAssert2(ctx->keyId, -1);
    xmlSecAssert2(xmlSecKeyCheckId(key, ctx->keyId), -1);

    value = xmlSecKeyGetValue(key);
    xmlSecAssert2(value, -1);
    
    pKey = xmlSecSymbianCryptoEvpKeyDataGetEvp(value);
    if (!pKey) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
		    "xmlSecSymbianCryptoEvpKeyDataGetEvp",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }
    
    if (ctx->pKey) {
	sc_pkey_free(ctx->pKey);
    }

    ctx->pKey = xmlSecSymbianCryptoEvpKeyDup(pKey);	
    if (!ctx->pKey) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
		    "xmlSecSymbianCryptoEvpKeyDup",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }

    return(0);
}

static int  
xmlSecSymbianCryptoEvpSignatureSetKeyReq(xmlSecTransformPtr transform,  xmlSecKeyReqPtr keyReq) {
    xmlSecSymbianCryptoEvpSignatureCtxPtr ctx;

    xmlSecAssert2(xmlSecSymbianCryptoEvpSignatureCheckId(transform), -1);
    xmlSecAssert2((transform->operation == xmlSecTransformOperationSign) 
                                || (transform->operation == xmlSecTransformOperationVerify), -1);
    xmlSecAssert2(xmlSecTransformCheckSize(transform, xmlSecSymbianCryptoEvpSignatureSize), -1);
    xmlSecAssert2(keyReq, -1);

    ctx = xmlSecSymbianCryptoEvpSignatureGetCtx(transform);
    xmlSecAssert2(ctx, -1);
    xmlSecAssert2(ctx->keyId, -1);

    keyReq->keyId        = ctx->keyId;
    if (transform->operation == xmlSecTransformOperationSign) {
        keyReq->keyType  = xmlSecKeyDataTypePrivate;
	keyReq->keyUsage = xmlSecKeyUsageSign;
    } else {
        keyReq->keyType  = xmlSecKeyDataTypePublic;
	keyReq->keyUsage = xmlSecKeyUsageVerify;
    }
    return(0);
}


static int
xmlSecSymbianCryptoEvpSignatureVerify(xmlSecTransformPtr transform, 
			const xmlSecByte* data, xmlSecSize dataSize,
			xmlSecTransformCtxPtr transformCtx) {
    xmlSecSymbianCryptoEvpSignatureCtxPtr ctx;
    int ret = -1;
    
    xmlSecAssert2(xmlSecSymbianCryptoEvpSignatureCheckId(transform), -1);
    xmlSecAssert2(transform->operation == xmlSecTransformOperationVerify, -1);
    xmlSecAssert2(xmlSecTransformCheckSize(transform, xmlSecSymbianCryptoEvpSignatureSize), -1);
    xmlSecAssert2(transform->status == xmlSecTransformStatusFinished, -1);
    xmlSecAssert2(data, -1);
    xmlSecAssert2(transformCtx, -1);

    ctx = xmlSecSymbianCryptoEvpSignatureGetCtx(transform);
    xmlSecAssert2(ctx, -1);

    ret = sc_verify_final(ctx->digestCtx, (xmlSecByte*)data, dataSize, ctx->pKey);
    if (ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE, 
		    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
		    "EVP_VerifyFinal",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    } else if (ret != 1) {
	xmlSecError(XMLSEC_ERRORS_HERE, 
		    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
		    "EVP_VerifyFinal",
		    XMLSEC_ERRORS_R_DATA_NOT_MATCH,
		    "signature do not match");
	transform->status = xmlSecTransformStatusFail;
	return(0);
    }
        
    transform->status = xmlSecTransformStatusOk;
    return(0);
}

static int 
xmlSecSymbianCryptoEvpSignatureExecute(xmlSecTransformPtr transform, 
                                int last, 
                                xmlSecTransformCtxPtr transformCtx) {
    xmlSecSymbianCryptoEvpSignatureCtxPtr ctx;
    xmlSecBufferPtr in, out;
    xmlSecSize inSize, outSize;
    int ret = -1;
    
    xmlSecAssert2(xmlSecSymbianCryptoEvpSignatureCheckId(transform), -1);
    xmlSecAssert2((transform->operation == xmlSecTransformOperationSign) 
                                || (transform->operation == xmlSecTransformOperationVerify), -1);
    xmlSecAssert2(xmlSecTransformCheckSize(transform, xmlSecSymbianCryptoEvpSignatureSize), -1);
    xmlSecAssert2(transformCtx, -1);

    ctx = xmlSecSymbianCryptoEvpSignatureGetCtx(transform);
    xmlSecAssert2(ctx, -1);

    in = &(transform->inBuf);
    out = &(transform->outBuf);
    inSize = xmlSecBufferGetSize(in);
    outSize = xmlSecBufferGetSize(out);    
    
    ctx = xmlSecSymbianCryptoEvpSignatureGetCtx(transform);
    xmlSecAssert2(ctx, -1);
    xmlSecAssert2(ctx->digest, -1);
    xmlSecAssert2(ctx->pKey, -1);

    if (transform->status == xmlSecTransformStatusNone) {
	xmlSecAssert2(outSize == 0, -1);
	
	if (transform->operation == xmlSecTransformOperationSign) {	
		// Initialization 
	    ret = sc_sign_init(&(ctx->digestCtx), ctx->digest);
	    if(ret != 0) {
	        xmlSecSetErrorFlag( ret );
		xmlSecError(XMLSEC_ERRORS_HERE, 
			    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
			    "EVP_SignInit",
			    XMLSEC_ERRORS_R_CRYPTO_FAILED,
			    XMLSEC_ERRORS_NO_MESSAGE);
		return(-1);
	    }

	} else {
	 	 ret = sc_verify_init(&(ctx->digestCtx), ctx->digest);
		 if (ret != 0) {
	        xmlSecSetErrorFlag( ret );
		 xmlSecError(XMLSEC_ERRORS_HERE, 
				    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
				    "EVP_SignInit",
				    XMLSEC_ERRORS_R_CRYPTO_FAILED,
				    XMLSEC_ERRORS_NO_MESSAGE);
		 return(-1);
		 }
	}
	transform->status = xmlSecTransformStatusWorking;
    }
    
    if ((transform->status == xmlSecTransformStatusWorking) && (inSize > 0)) {
	xmlSecAssert2(outSize == 0, -1);

	if (transform->operation == xmlSecTransformOperationSign) {

	    sc_sign_update(ctx->digestCtx, xmlSecBufferGetData(in), inSize);

	} else {

	    sc_verify_update(ctx->digestCtx, xmlSecBufferGetData(in), inSize);

	}
	    
	ret = xmlSecBufferRemoveHead(in, inSize);
	if (ret < 0) {
	    xmlSecError(XMLSEC_ERRORS_HERE, 
			xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
			"xmlSecBufferRemoveHead",
			XMLSEC_ERRORS_R_XMLSEC_FAILED,
			XMLSEC_ERRORS_NO_MESSAGE);
	    return(-1);
	}
    }

    if ((transform->status == xmlSecTransformStatusWorking) && (last != 0)) {
	xmlSecAssert2(outSize == 0, -1);
	if (transform->operation == xmlSecTransformOperationSign) {
	    outSize = sc_pkey_size(ctx->pKey);
#ifndef XMLSEC_NO_DSA 
	    if (outSize < XMLSEC_OPENSSL_DSA_SIGNATURE_SIZE) {
		outSize = XMLSEC_OPENSSL_DSA_SIGNATURE_SIZE;
	    }
#endif /* XMLSEC_NO_DSA */ 

	    ret = xmlSecBufferSetMaxSize(out, outSize);
	    if (ret < 0) {
		xmlSecError(XMLSEC_ERRORS_HERE, 
			    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
			    "xmlSecBufferSetMaxSize",
			    XMLSEC_ERRORS_R_XMLSEC_FAILED,
			    "size=%d", outSize);
		return(-1);
	    }
	
	    ret = sc_sign_final(ctx->digestCtx, xmlSecBufferGetData(out), &outSize, ctx->pKey);
	    if (ret < 0) {
		xmlSecError(XMLSEC_ERRORS_HERE, 
			    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
			    "EVP_SignFinal",
			    XMLSEC_ERRORS_R_CRYPTO_FAILED,
			    XMLSEC_ERRORS_NO_MESSAGE);
		return(-1);
	    }
		
	    ret = xmlSecBufferSetSize(out, outSize);
	    if (ret < 0) {
		xmlSecError(XMLSEC_ERRORS_HERE, 
			    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
			    "xmlSecBufferSetSize",
			    XMLSEC_ERRORS_R_XMLSEC_FAILED,
			    "size=%d", outSize);
		return(-1);
	    }
	}
	transform->status = xmlSecTransformStatusFinished;
    }
    
    if ((transform->status == xmlSecTransformStatusWorking) 
        || (transform->status == xmlSecTransformStatusFinished)) {
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

#ifndef XMLSEC_NO_DSA

#ifndef XMLSEC_NO_SHA1
/****************************************************************************
 *
 * DSA-SHA1 signature transform
 *
 ***************************************************************************/
static xmlSecTransformKlass xmlSecSymbianCryptoDsaSha1Klass = {
    /* klass/object sizes */
    sizeof(xmlSecTransformKlass),		/* xmlSecSize klassSize */
    xmlSecSymbianCryptoEvpSignatureSize,		/* xmlSecSize objSize */

    xmlSecNameDsaSha1,				/* const xmlChar* name; */
    xmlSecHrefDsaSha1, 				/* const xmlChar* href; */
    xmlSecTransformUsageSignatureMethod,	/* xmlSecTransformUsage usage; */
    
    xmlSecSymbianCryptoEvpSignatureInitialize,	/* xmlSecTransformInitializeMethod initialize; */
    xmlSecSymbianCryptoEvpSignatureFinalize,		/* xmlSecTransformFinalizeMethod finalize; */
    NULL,					/* xmlSecTransformNodeReadMethod readNode; */
    NULL,					/* xmlSecTransformNodeWriteMethod writeNode; */
    xmlSecSymbianCryptoEvpSignatureSetKeyReq,		/* xmlSecTransformSetKeyReqMethod setKeyReq; */
    xmlSecSymbianCryptoEvpSignatureSetKey,		/* xmlSecTransformSetKeyMethod setKey; */
    xmlSecSymbianCryptoEvpSignatureVerify,		/* xmlSecTransformVerifyMethod verify; */
    xmlSecCrpytoGetDataTypeMethod,		/* xmlSecTransformGetDataTypeMethod getDataType; */
    xmlSecCryptoPushBinMethod,		/* xmlSecTransformPushBinMethod pushBin; */
    xmlSecCryptoPopBinMethod,		/* xmlSecTransformPopBinMethod popBin; */
    NULL,					/* xmlSecTransformPushXmlMethod pushXml; */
    NULL,					/* xmlSecTransformPopXmlMethod popXml; */
    xmlSecSymbianCryptoEvpSignatureExecute,		/* xmlSecTransformExecuteMethod execute; */
    
    NULL,					/* void* reserved0; */
    NULL,					/* void* reserved1; */
};

/**
 * xmlSecSymbianCryptoTransformDsaSha1GetKlass:
 * 
 * The DSA-SHA1 signature transform klass.
 *
 * Returns DSA-SHA1 signature transform klass.
 */
xmlSecTransformId 
xmlSecSymbianCryptoTransformDsaSha1GetKlass(void) {
    return(&xmlSecSymbianCryptoDsaSha1Klass);
}

/****************************************************************************
 *
 * DSA-SHA1 EVP
 *
 * XMLDSig specifies dsa signature packing not supported by SymbianCrypto so 
 * we created our own EVP_MD.
 *
 * http://www.w3.org/TR/xmldsig-core/#sec-SignatureAlg:
 * 
 * The output of the DSA algorithm consists of a pair of integers 
 * usually referred by the pair (r, s). The signature value consists of 
 * the base64 encoding of the concatenation of two octet-streams that 
 * respectively result from the octet-encoding of the values r and s in 
 * that order. Integer to octet-stream conversion must be done according 
 * to the I2OSP operation defined in the RFC 2437 [PKCS1] specification 
 * with a l parameter equal to 20. For example, the SignatureValue element 
 * for a DSA signature (r, s) with values specified in hexadecimal:
 *
 *  r = 8BAC1AB6 6410435C B7181F95 B16AB97C 92B341C0 
 *  s = 41E2345F 1F56DF24 58F426D1 55B4BA2D B6DCD8C8
 *       
 * from the example in Appendix 5 of the DSS standard would be
 *        
 * <SignatureValue>i6watmQQQ1y3GB+VsWq5fJKzQcBB4jRfH1bfJFj0JtFVtLotttzYyA==</SignatureValue>
 *
 ***************************************************************************/
#ifndef XMLSEC_OPENSSL_096
static int 
xmlSecSymbianCryptoDsaSha1EvpInit(EVP_MD_CTX *ctx)
{ 
    return SHA1_Init(ctx->md_data); 
}

static int 
xmlSecSymbianCryptoDsaSha1EvpUpdate(EVP_MD_CTX *ctx,const void *data,unsigned long count)
{ 
    return SHA1_Update(ctx->md_data,data,count); 
}

static int 
xmlSecSymbianCryptoDsaSha1EvpFinal(EVP_MD_CTX *ctx,xmlSecByte *md)
{ 
    return SHA1_Final(md,ctx->md_data); 
}
#endif /* XMLSEC_OPENSSL_096 */

static int 	
xmlSecSymbianCryptoDsaSha1EvpSign(int type ATTRIBUTE_UNUSED, 
			const xmlSecByte *dgst, int dlen,
			xmlSecByte *sig, unsigned int *siglen, DSA *dsa) {
    DSA_SIG *s;
    int rSize, sSize;

    s = DSA_do_sign(dgst, dlen, dsa);
    if(!s) {
	*siglen=0;
	return(0);
    }

    rSize = BN_num_bytes(s->r);
    sSize = BN_num_bytes(s->s);
    if((rSize > (KXmlSecSymbianCryptoDsaSignatureSize / 2)) ||
       (sSize > (KXmlSecSymbianCryptoDsaSignatureSize / 2))) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    NULL,
		    XMLSEC_ERRORS_R_INVALID_SIZE,
		    "size(r)=%d or size(s)=%d > %d", 
		    rSize, sSize, KXmlSecSymbianCryptoDsaSignatureSize / 2);
	DSA_SIG_free(s);
	return(0);
    }	

    memset(sig, 0, KXmlSecSymbianCryptoDsaSignatureSize);
    BN_bn2bin(s->r, sig + (KXmlSecSymbianCryptoDsaSignatureSize / 2) - rSize);
    BN_bn2bin(s->s, sig + KXmlSecSymbianCryptoDsaSignatureSize - sSize);
    *siglen = KXmlSecSymbianCryptoDsaSignatureSize;

    DSA_SIG_free(s);
    return(1);    
}

static int 
xmlSecSymbianCryptoDsaSha1EvpVerify(int type ATTRIBUTE_UNUSED, 
			const xmlSecByte *dgst, int dgst_len,
			const xmlSecByte *sigbuf, int siglen, DSA *dsa) {
    DSA_SIG *s;    
    int ret = -1;

    s = DSA_SIG_new();
    if (!s) {
	return(ret);
    }

    if(siglen != KXmlSecSymbianCryptoDsaSignatureSize) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    NULL,
		    XMLSEC_ERRORS_R_INVALID_SIZE,
		    "invalid length %d (%d expected)",
		    siglen, KXmlSecSymbianCryptoDsaSignatureSize);
	goto err;
    }

    s->r = BN_bin2bn(sigbuf, KXmlSecSymbianCryptoDsaSignatureSize / 2, NULL);
    s->s = BN_bin2bn(sigbuf + (KXmlSecSymbianCryptoDsaSignatureSize / 2), 
		       KXmlSecSymbianCryptoDsaSignatureSize / 2, NULL);
    if((!s->r) || (!s->s)) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "BN_bin2bn",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	goto err;
    }

    ret = DSA_do_verify(dgst, dgst_len, s, dsa);

err:
    DSA_SIG_free(s);
    return(ret);
}

static const EVP_MD xmlSecSymbianCryptoDsaMdEvp = {
    NID_dsaWithSHA,
    NID_dsaWithSHA,
    SHA_DIGEST_LENGTH,
#ifndef XMLSEC_OPENSSL_096
    0,
    xmlSecSymbianCryptoDsaSha1EvpInit,
    xmlSecSymbianCryptoDsaSha1EvpUpdate,
    xmlSecSymbianCryptoDsaSha1EvpFinal,
    NULL,
    NULL,
#else /* XMLSEC_OPENSSL_096 */
    SHA1_Init,
    SHA1_Update,
    SHA1_Final,
#endif /* XMLSEC_OPENSSL_096 */
    xmlSecSymbianCryptoDsaSha1EvpSign,
    xmlSecSymbianCryptoDsaSha1EvpVerify, 
    {EVP_PKEY_DSA,EVP_PKEY_DSA2,EVP_PKEY_DSA3,EVP_PKEY_DSA4,0},
    SHA_CBLOCK,
    sizeof(EVP_MD *)+sizeof(SHA_CTX),
};

static const EVP_MD *xmlSecSymbianCryptoDsaSha1Evp(void)
{
    return(&xmlSecSymbianCryptoDsaMdEvp);
}
#endif /* XMLSEC_NO_SHA1 */

#endif /* XMLSEC_NO_DSA */

#ifndef XMLSEC_NO_RSA

#ifndef XMLSEC_NO_MD5
/****************************************************************************
 *
 * RSA-MD5 signature transform
 *
 ***************************************************************************/
static xmlSecTransformKlass xmlSecSymbianCryptoRsaMd5Klass = {
    /* klass/object sizes */
    sizeof(xmlSecTransformKlass),		/* xmlSecSize klassSize */
    xmlSecSymbianCryptoEvpSignatureSize,		/* xmlSecSize objSize */

    xmlSecNameRsaMd5,				/* const xmlChar* name; */
    xmlSecHrefRsaMd5, 				/* const xmlChar* href; */
    xmlSecTransformUsageSignatureMethod,	/* xmlSecTransformUsage usage; */
    
    xmlSecSymbianCryptoEvpSignatureInitialize,	/* xmlSecTransformInitializeMethod initialize; */
    xmlSecSymbianCryptoEvpSignatureFinalize,		/* xmlSecTransformFinalizeMethod finalize; */
    NULL,					/* xmlSecTransformNodeReadMethod readNode; */
    NULL,					/* xmlSecTransformNodeWriteMethod writeNode; */
    xmlSecSymbianCryptoEvpSignatureSetKeyReq,		/* xmlSecTransformSetKeyReqMethod setKeyReq; */
    xmlSecSymbianCryptoEvpSignatureSetKey,		/* xmlSecTransformSetKeyMethod setKey; */
    xmlSecSymbianCryptoEvpSignatureVerify,		/* xmlSecTransformVerifyMethod verify; */
    xmlSecCrpytoGetDataTypeMethod,		/* xmlSecTransformGetDataTypeMethod getDataType; */
    xmlSecCryptoPushBinMethod,		/* xmlSecTransformPushBinMethod pushBin; */
    xmlSecCryptoPopBinMethod,		/* xmlSecTransformPopBinMethod popBin; */
    NULL,					/* xmlSecTransformPushXmlMethod pushXml; */
    NULL,					/* xmlSecTransformPopXmlMethod popXml; */
    xmlSecSymbianCryptoEvpSignatureExecute,		/* xmlSecTransformExecuteMethod execute; */
    
    NULL,					/* void* reserved0; */
    NULL,					/* void* reserved1; */
};

/**
 * xmlSecSymbianCryptoTransformRsaMd5GetKlass:
 * 
 * The RSA-MD5 signature transform klass.
 *
 * Returns RSA-MD5 signature transform klass.
 */
xmlSecTransformId 
xmlSecSymbianCryptoTransformRsaMd5GetKlass(void) {
    return(&xmlSecSymbianCryptoRsaMd5Klass);
}

#endif /* XMLSEC_NO_MD5 */

#ifndef XMLSEC_NO_RIPEMD160
/****************************************************************************
 *
 * RSA-RIPEMD160 signature transform
 *
 ***************************************************************************/
static xmlSecTransformKlass xmlSecSymbianCryptoRsaRipemd160Klass = {
    /* klass/object sizes */
    sizeof(xmlSecTransformKlass),		/* xmlSecSize klassSize */
    xmlSecSymbianCryptoEvpSignatureSize,		/* xmlSecSize objSize */

    xmlSecNameRsaRipemd160,				/* const xmlChar* name; */
    xmlSecHrefRsaRipemd160, 				/* const xmlChar* href; */
    xmlSecTransformUsageSignatureMethod,	/* xmlSecTransformUsage usage; */
    
    xmlSecSymbianCryptoEvpSignatureInitialize,	/* xmlSecTransformInitializeMethod initialize; */
    xmlSecSymbianCryptoEvpSignatureFinalize,		/* xmlSecTransformFinalizeMethod finalize; */
    NULL,					/* xmlSecTransformNodeReadMethod readNode; */
    NULL,					/* xmlSecTransformNodeWriteMethod writeNode; */
    xmlSecSymbianCryptoEvpSignatureSetKeyReq,		/* xmlSecTransformSetKeyReqMethod setKeyReq; */
    xmlSecSymbianCryptoEvpSignatureSetKey,		/* xmlSecTransformSetKeyMethod setKey; */
    xmlSecSymbianCryptoEvpSignatureVerify,		/* xmlSecTransformVerifyMethod verify; */
    xmlSecCrpytoGetDataTypeMethod,		/* xmlSecTransformGetDataTypeMethod getDataType; */
    xmlSecCryptoPushBinMethod,		/* xmlSecTransformPushBinMethod pushBin; */
    xmlSecCryptoPopBinMethod,		/* xmlSecTransformPopBinMethod popBin; */
    NULL,					/* xmlSecTransformPushXmlMethod pushXml; */
    NULL,					/* xmlSecTransformPopXmlMethod popXml; */
    xmlSecSymbianCryptoEvpSignatureExecute,		/* xmlSecTransformExecuteMethod execute; */
    
    NULL,					/* void* reserved0; */
    NULL,					/* void* reserved1; */
};

/**
 * xmlSecSymbianCryptoTransformRsaRipemd160GetKlass:
 * 
 * The RSA-RIPEMD160 signature transform klass.
 *
 * Returns RSA-RIPEMD160 signature transform klass.
 */
xmlSecTransformId 
xmlSecSymbianCryptoTransformRsaRipemd160GetKlass(void) {
    return(&xmlSecSymbianCryptoRsaRipemd160Klass);
}

#endif /* XMLSEC_NO_RIPEMD160 */

#ifndef XMLSEC_NO_SHA1
/****************************************************************************
 *
 * RSA-SHA1 signature transform
 *
 ***************************************************************************/
static xmlSecTransformKlass xmlSecSymbianCryptoRsaSha1Klass = {
    /* klass/object sizes */
    sizeof(xmlSecTransformKlass),		/* xmlSecSize klassSize */
    xmlSecSymbianCryptoEvpSignatureSize,		/* xmlSecSize objSize */

    xmlSecNameRsaSha1,				/* const xmlChar* name; */
    xmlSecHrefRsaSha1, 				/* const xmlChar* href; */
    xmlSecTransformUsageSignatureMethod,	/* xmlSecTransformUsage usage; */
    
    xmlSecSymbianCryptoEvpSignatureInitialize,	/* xmlSecTransformInitializeMethod initialize; */
    xmlSecSymbianCryptoEvpSignatureFinalize,		/* xmlSecTransformFinalizeMethod finalize; */
    NULL,					/* xmlSecTransformNodeReadMethod readNode; */
    NULL,					/* xmlSecTransformNodeWriteMethod writeNode; */
    xmlSecSymbianCryptoEvpSignatureSetKeyReq,		/* xmlSecTransformSetKeyReqMethod setKeyReq; */
    xmlSecSymbianCryptoEvpSignatureSetKey,		/* xmlSecTransformSetKeyMethod setKey; */
    xmlSecSymbianCryptoEvpSignatureVerify,		/* xmlSecTransformVerifyMethod verify; */
    xmlSecCrpytoGetDataTypeMethod,		/* xmlSecTransformGetDataTypeMethod getDataType; */
    xmlSecCryptoPushBinMethod,		/* xmlSecTransformPushBinMethod pushBin; */
    xmlSecCryptoPopBinMethod,		/* xmlSecTransformPopBinMethod popBin; */
    NULL,					/* xmlSecTransformPushXmlMethod pushXml; */
    NULL,					/* xmlSecTransformPopXmlMethod popXml; */
    xmlSecSymbianCryptoEvpSignatureExecute,		/* xmlSecTransformExecuteMethod execute; */
    
    NULL,					/* void* reserved0; */
    NULL,					/* void* reserved1; */
};

/**
 * xmlSecSymbianCryptoTransformRsaSha1GetKlass:
 * 
 * The RSA-SHA1 signature transform klass.
 *
 * Returns RSA-SHA1 signature transform klass.
 */
EXPORT_C
xmlSecTransformId 
xmlSecSymbianCryptoTransformRsaSha1GetKlass(void) {
    return(&xmlSecSymbianCryptoRsaSha1Klass);
}

#endif /* XMLSEC_NO_SHA1 */

#ifndef XMLSEC_NO_SHA224
/****************************************************************************
 *
 * RSA-SHA224 signature transform
 *
 ***************************************************************************/
static xmlSecTransformKlass xmlSecSymbianCryptoRsaSha224Klass = {
    /* klass/object sizes */
    sizeof(xmlSecTransformKlass),		/* xmlSecSize klassSize */
    xmlSecSymbianCryptoEvpSignatureSize,		/* xmlSecSize objSize */

    xmlSecNameRsaSha224,				/* const xmlChar* name; */
    xmlSecHrefRsaSha224, 				/* const xmlChar* href; */
    xmlSecTransformUsageSignatureMethod,	/* xmlSecTransformUsage usage; */
    
    xmlSecSymbianCryptoEvpSignatureInitialize,	/* xmlSecTransformInitializeMethod initialize; */
    xmlSecSymbianCryptoEvpSignatureFinalize,		/* xmlSecTransformFinalizeMethod finalize; */
    NULL,					/* xmlSecTransformNodeReadMethod readNode; */
    NULL,					/* xmlSecTransformNodeWriteMethod writeNode; */
    xmlSecSymbianCryptoEvpSignatureSetKeyReq,		/* xmlSecTransformSetKeyReqMethod setKeyReq; */
    xmlSecSymbianCryptoEvpSignatureSetKey,		/* xmlSecTransformSetKeyMethod setKey; */
    xmlSecSymbianCryptoEvpSignatureVerify,		/* xmlSecTransformVerifyMethod verify; */
    xmlSecCrpytoGetDataTypeMethod,		/* xmlSecTransformGetDataTypeMethod getDataType; */
    xmlSecCryptoPushBinMethod,		/* xmlSecTransformPushBinMethod pushBin; */
    xmlSecCryptoPopBinMethod,		/* xmlSecTransformPopBinMethod popBin; */
    NULL,					/* xmlSecTransformPushXmlMethod pushXml; */
    NULL,					/* xmlSecTransformPopXmlMethod popXml; */
    xmlSecSymbianCryptoEvpSignatureExecute,		/* xmlSecTransformExecuteMethod execute; */
    
    NULL,					/* void* reserved0; */
    NULL,					/* void* reserved1; */
};

/**
 * xmlSecSymbianCryptoTransformRsaSha224GetKlass:
 * 
 * The RSA-SHA224 signature transform klass.
 *
 * Returns RSA-SHA224 signature transform klass.
 */
xmlSecTransformId 
xmlSecSymbianCryptoTransformRsaSha224GetKlass(void) {
    return(&xmlSecSymbianCryptoRsaSha224Klass);
}

#endif /* XMLSEC_NO_SHA224 */

#ifndef XMLSEC_NO_SHA256
/****************************************************************************
 *
 * RSA-SHA256 signature transform
 *
 ***************************************************************************/
static xmlSecTransformKlass xmlSecSymbianCryptoRsaSha256Klass = {
    /* klass/object sizes */
    sizeof(xmlSecTransformKlass),		/* xmlSecSize klassSize */
    xmlSecSymbianCryptoEvpSignatureSize,		/* xmlSecSize objSize */

    xmlSecNameRsaSha256,				/* const xmlChar* name; */
    xmlSecHrefRsaSha256, 				/* const xmlChar* href; */
    xmlSecTransformUsageSignatureMethod,	/* xmlSecTransformUsage usage; */
    
    xmlSecSymbianCryptoEvpSignatureInitialize,	/* xmlSecTransformInitializeMethod initialize; */
    xmlSecSymbianCryptoEvpSignatureFinalize,		/* xmlSecTransformFinalizeMethod finalize; */
    NULL,					/* xmlSecTransformNodeReadMethod readNode; */
    NULL,					/* xmlSecTransformNodeWriteMethod writeNode; */
    xmlSecSymbianCryptoEvpSignatureSetKeyReq,		/* xmlSecTransformSetKeyReqMethod setKeyReq; */
    xmlSecSymbianCryptoEvpSignatureSetKey,		/* xmlSecTransformSetKeyMethod setKey; */
    xmlSecSymbianCryptoEvpSignatureVerify,		/* xmlSecTransformVerifyMethod verify; */
    xmlSecCrpytoGetDataTypeMethod,		/* xmlSecTransformGetDataTypeMethod getDataType; */
    xmlSecCryptoPushBinMethod,		/* xmlSecTransformPushBinMethod pushBin; */
    xmlSecCryptoPopBinMethod,		/* xmlSecTransformPopBinMethod popBin; */
    NULL,					/* xmlSecTransformPushXmlMethod pushXml; */
    NULL,					/* xmlSecTransformPopXmlMethod popXml; */
    xmlSecSymbianCryptoEvpSignatureExecute,		/* xmlSecTransformExecuteMethod execute; */
    
    NULL,					/* void* reserved0; */
    NULL,					/* void* reserved1; */
};

/**
 * xmlSecSymbianCryptoTransformRsaSha256GetKlass:
 * 
 * The RSA-SHA256 signature transform klass.
 *
 * Returns RSA-SHA256 signature transform klass.
 */
xmlSecTransformId 
xmlSecSymbianCryptoTransformRsaSha256GetKlass(void) {
    return(&xmlSecSymbianCryptoRsaSha256Klass);
}

#endif /* XMLSEC_NO_SHA256 */

#ifndef XMLSEC_NO_SHA384
/****************************************************************************
 *
 * RSA-SHA384 signature transform
 *
 ***************************************************************************/
static xmlSecTransformKlass xmlSecSymbianCryptoRsaSha384Klass = {
    /* klass/object sizes */
    sizeof(xmlSecTransformKlass),		/* xmlSecSize klassSize */
    xmlSecSymbianCryptoEvpSignatureSize,		/* xmlSecSize objSize */

    xmlSecNameRsaSha384,				/* const xmlChar* name; */
    xmlSecHrefRsaSha384, 				/* const xmlChar* href; */
    xmlSecTransformUsageSignatureMethod,	/* xmlSecTransformUsage usage; */
    
    xmlSecSymbianCryptoEvpSignatureInitialize,	/* xmlSecTransformInitializeMethod initialize; */
    xmlSecSymbianCryptoEvpSignatureFinalize,		/* xmlSecTransformFinalizeMethod finalize; */
    NULL,					/* xmlSecTransformNodeReadMethod readNode; */
    NULL,					/* xmlSecTransformNodeWriteMethod writeNode; */
    xmlSecSymbianCryptoEvpSignatureSetKeyReq,		/* xmlSecTransformSetKeyReqMethod setKeyReq; */
    xmlSecSymbianCryptoEvpSignatureSetKey,		/* xmlSecTransformSetKeyMethod setKey; */
    xmlSecSymbianCryptoEvpSignatureVerify,		/* xmlSecTransformVerifyMethod verify; */
    xmlSecCrpytoGetDataTypeMethod,		/* xmlSecTransformGetDataTypeMethod getDataType; */
    xmlSecCryptoPushBinMethod,		/* xmlSecTransformPushBinMethod pushBin; */
    xmlSecCryptoPopBinMethod,		/* xmlSecTransformPopBinMethod popBin; */
    NULL,					/* xmlSecTransformPushXmlMethod pushXml; */
    NULL,					/* xmlSecTransformPopXmlMethod popXml; */
    xmlSecSymbianCryptoEvpSignatureExecute,		/* xmlSecTransformExecuteMethod execute; */
    
    NULL,					/* void* reserved0; */
    NULL,					/* void* reserved1; */
};

/**
 * xmlSecSymbianCryptoTransformRsaSha384GetKlass:
 * 
 * The RSA-SHA384 signature transform klass.
 *
 * Returns RSA-SHA384 signature transform klass.
 */
xmlSecTransformId 
xmlSecSymbianCryptoTransformRsaSha384GetKlass(void) {
    return(&xmlSecSymbianCryptoRsaSha384Klass);
}

#endif /* XMLSEC_NO_SHA384 */

#ifndef XMLSEC_NO_SHA512
/****************************************************************************
 *
 * RSA-SHA512 signature transform
 *
 ***************************************************************************/
static xmlSecTransformKlass xmlSecSymbianCryptoRsaSha512Klass = {
    /* klass/object sizes */
    sizeof(xmlSecTransformKlass),		/* xmlSecSize klassSize */
    xmlSecSymbianCryptoEvpSignatureSize,		/* xmlSecSize objSize */

    xmlSecNameRsaSha512,				/* const xmlChar* name; */
    xmlSecHrefRsaSha512, 				/* const xmlChar* href; */
    xmlSecTransformUsageSignatureMethod,	/* xmlSecTransformUsage usage; */
    
    xmlSecSymbianCryptoEvpSignatureInitialize,	/* xmlSecTransformInitializeMethod initialize; */
    xmlSecSymbianCryptoEvpSignatureFinalize,		/* xmlSecTransformFinalizeMethod finalize; */
    NULL,					/* xmlSecTransformNodeReadMethod readNode; */
    NULL,					/* xmlSecTransformNodeWriteMethod writeNode; */
    xmlSecSymbianCryptoEvpSignatureSetKeyReq,		/* xmlSecTransformSetKeyReqMethod setKeyReq; */
    xmlSecSymbianCryptoEvpSignatureSetKey,		/* xmlSecTransformSetKeyMethod setKey; */
    xmlSecSymbianCryptoEvpSignatureVerify,		/* xmlSecTransformVerifyMethod verify; */
    xmlSecCrpytoGetDataTypeMethod,		/* xmlSecTransformGetDataTypeMethod getDataType; */
    xmlSecCryptoPushBinMethod,		/* xmlSecTransformPushBinMethod pushBin; */
    xmlSecCryptoPopBinMethod,		/* xmlSecTransformPopBinMethod popBin; */
    NULL,					/* xmlSecTransformPushXmlMethod pushXml; */
    NULL,					/* xmlSecTransformPopXmlMethod popXml; */
    xmlSecSymbianCryptoEvpSignatureExecute,		/* xmlSecTransformExecuteMethod execute; */
    
    NULL,					/* void* reserved0; */
    NULL,					/* void* reserved1; */
};

/**
 * xmlSecSymbianCryptoTransformRsaSha512GetKlass:
 * 
 * The RSA-SHA512 signature transform klass.
 *
 * Returns RSA-SHA512 signature transform klass.
 */
xmlSecTransformId 
xmlSecSymbianCryptoTransformRsaSha512GetKlass(void) {
    return(&xmlSecSymbianCryptoRsaSha512Klass);
}

#endif /* XMLSEC_NO_SHA512 */

#endif /* XMLSEC_NO_RSA */



