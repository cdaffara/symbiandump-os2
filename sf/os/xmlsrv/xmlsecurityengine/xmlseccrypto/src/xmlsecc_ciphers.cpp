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
#include "xmlsec_keys.h"
#include "xmlsec_transforms.h"
#include "xmlsec_errors.h"

#include "xmlsecc_crypto.h"
#include "xmlsecc_cryptowrapper.h"		// replace gnutils/gnutls.h

/**************************************************************************
 *
 * Internal SymbianCrypto Block cipher CTX
 *
 *****************************************************************************/
xmlSecTransformGetDataTypeMethod const xmlSecCrpytoGetDataTypeMethod  = xmlSecTransformDefaultGetDataType;
xmlSecTransformPushBinMethod const xmlSecCryptoPushBinMethod = xmlSecTransformDefaultPushBin;
xmlSecTransformPopBinMethod const xmlSecCryptoPopBinMethod = xmlSecTransformDefaultPopBin;

typedef struct _xmlSecSymbianCryptoBlockCipherCtx		xmlSecSymbianCryptoBlockCipherCtx,
							*xmlSecSymbianCryptoBlockCipherCtxPtr;
struct _xmlSecSymbianCryptoBlockCipherCtx {
    int			cipher;
    int			mode;
    ScCipherHd	cipherCtx;
    //sc_cipher_hd_t	cipherCtx;
    xmlSecKeyDataId	keyId;
    int			keyInitialized;
    int			ctxInitialized;
};

static int 	xmlSecSymbianCryptoBlockCipherCtxInit		(xmlSecSymbianCryptoBlockCipherCtxPtr ctx,
							 xmlSecBufferPtr in,
							 xmlSecBufferPtr out,
							 int encrypt,
							 const xmlChar* cipherName,
							 xmlSecTransformCtxPtr transformCtx);
static int 	xmlSecSymbianCryptoBlockCipherCtxUpdate	(xmlSecSymbianCryptoBlockCipherCtxPtr ctx,
							 xmlSecBufferPtr in,
							 xmlSecBufferPtr out,
							 int encrypt,
							 const xmlChar* cipherName,
							 xmlSecTransformCtxPtr transformCtx);
static int 	xmlSecSymbianCryptoBlockCipherCtxFinal		(xmlSecSymbianCryptoBlockCipherCtxPtr ctx,
							 xmlSecBufferPtr in,
							 xmlSecBufferPtr out,
							 int encrypt,
							 const xmlChar* cipherName,
							 xmlSecTransformCtxPtr transformCtx);
static int 
xmlSecSymbianCryptoBlockCipherCtxInit(xmlSecSymbianCryptoBlockCipherCtxPtr ctx,
				xmlSecBufferPtr in, xmlSecBufferPtr out,
				int encrypt,
				const xmlChar* cipherName,
				xmlSecTransformCtxPtr transformCtx) {
    int blockLen;
    int ret;

    xmlSecAssert2(ctx, -1);
    xmlSecAssert2(ctx->cipher != 0, -1);
    xmlSecAssert2(ctx->cipherCtx, -1);
    xmlSecAssert2(ctx->keyInitialized != 0, -1);
    xmlSecAssert2(ctx->ctxInitialized == 0, -1);
    xmlSecAssert2(in, -1);
    xmlSecAssert2(out, -1);
    xmlSecAssert2(transformCtx, -1);

    /* iv len == block len */
    blockLen = sc_cipher_get_algo_blklen(ctx->cipher);
    xmlSecAssert2(blockLen > 0, -1);    
    set_ctx_blocksize(ctx->cipherCtx, blockLen);		//set blocksize
    
    if(encrypt) {
	xmlSecByte* iv;
    	xmlSecSize outSize;
	
	/* allocate space for IV */	
	outSize = xmlSecBufferGetSize(out);
	ret = xmlSecBufferSetSize(out, outSize + blockLen);
	if(ret < 0) {
	    xmlSecError(XMLSEC_ERRORS_HERE, 
			xmlSecErrorsSafeString(cipherName),
			"xmlSecBufferSetSize",
			XMLSEC_ERRORS_R_XMLSEC_FAILED,
			"size=%d", outSize + blockLen);
	    return(-1);
	}
	iv = xmlSecBufferGetData(out) + outSize;
	
	/* generate and use random iv */
	ret = sc_randomize(iv, blockLen, SC_STRONG_RANDOM);
	if( ret != 0 ) {
	    xmlSecError(XMLSEC_ERRORS_HERE, 
			xmlSecErrorsSafeString(cipherName),
			"sc_randomize",
			XMLSEC_ERRORS_R_MALLOC_FAILED,
			"ret=%d", ret);
	    return( -1 );
	}	
	ret = sc_cipher_setiv(ctx->cipherCtx, iv, blockLen);
	if(ret != 0) {
	    xmlSecError(XMLSEC_ERRORS_HERE, 
			xmlSecErrorsSafeString(cipherName),
			"sc_cipher_setiv",
			XMLSEC_ERRORS_R_CRYPTO_FAILED,
			"ret=%d", ret);
	    return(-1);
	}
    } else {
	/* if we don't have enough data, exit and hope that 
	 * we'll have iv next time */
	if(xmlSecBufferGetSize(in) < (xmlSecSize)blockLen) {
	    return(0);
	}
	xmlSecAssert2(xmlSecBufferGetData(in), -1);

	/* set iv */
	ret = sc_cipher_setiv(ctx->cipherCtx, xmlSecBufferGetData(in), blockLen);
	if(ret != 0) {
	    xmlSecError(XMLSEC_ERRORS_HERE, 
			xmlSecErrorsSafeString(cipherName),
			"sc_cipher_setiv",
			XMLSEC_ERRORS_R_CRYPTO_FAILED,
			"ret=%d", ret);
	    return(-1);
	}
	
	/* and remove from input */
	ret = xmlSecBufferRemoveHead(in, blockLen);
	if(ret < 0) {
	    xmlSecError(XMLSEC_ERRORS_HERE, 
			xmlSecErrorsSafeString(cipherName),
			"xmlSecBufferRemoveHead",
			XMLSEC_ERRORS_R_XMLSEC_FAILED,
			"size=%d", blockLen);
	    return(-1);
	}
    }

    ctx->ctxInitialized = 1;
    return(0);
}

static int 
xmlSecSymbianCryptoBlockCipherCtxUpdate(xmlSecSymbianCryptoBlockCipherCtxPtr ctx,
				  xmlSecBufferPtr in, xmlSecBufferPtr out,
				  int encrypt,
				  const xmlChar* cipherName,
				  xmlSecTransformCtxPtr transformCtx) {
    xmlSecSize inSize, inBlocks, outSize;
    int blockLen;
    xmlSecByte* outBuf;
    int ret;
    
    xmlSecAssert2(ctx, -1);
    xmlSecAssert2(ctx->cipher != 0, -1);
    xmlSecAssert2(ctx->cipherCtx, -1);
    xmlSecAssert2(ctx->ctxInitialized != 0, -1);
    xmlSecAssert2(in, -1);
    xmlSecAssert2(out, -1);
    xmlSecAssert2(transformCtx, -1);

    blockLen = sc_cipher_get_algo_blklen(ctx->cipher);
    xmlSecAssert2(blockLen > 0, -1);

    inSize = xmlSecBufferGetSize(in);
    outSize = xmlSecBufferGetSize(out);
    
    if(inSize < (xmlSecSize)blockLen) {
	return(0);
    }

    if(encrypt) {
        inBlocks = inSize / ((xmlSecSize)blockLen);
    } else {
	/* we want to have the last block in the input buffer 
	 * for padding check */
        inBlocks = (inSize - 1) / ((xmlSecSize)blockLen);
    }
    inSize = inBlocks * ((xmlSecSize)blockLen);

    /* we write out the input size plus may be one block */
    ret = xmlSecBufferSetMaxSize(out, outSize + inSize + blockLen);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE, 
		    xmlSecErrorsSafeString(cipherName),
		    "xmlSecBufferSetMaxSize",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "size=%d", outSize + inSize + blockLen);
	return(-1);
    }
    outBuf = xmlSecBufferGetData(out) + outSize;
    
    if(encrypt) {
	ret = sc_cipher_encrypt(ctx->cipherCtx, outBuf, inSize + blockLen,
				xmlSecBufferGetData(in), inSize, NOTLAST);
	if(ret != 0) {
	    xmlSecError(XMLSEC_ERRORS_HERE, 
			xmlSecErrorsSafeString(cipherName),
			"sc_cipher_encrypt",
			XMLSEC_ERRORS_R_CRYPTO_FAILED,
			"ret=%d", ret);
	    return(-1);
	}
    } else {
	ret = sc_cipher_decrypt(ctx->cipherCtx, outBuf, inSize + blockLen,
				xmlSecBufferGetData(in), inSize, NULL, NOTLAST);
	if(ret != 0) {
	    xmlSecError(XMLSEC_ERRORS_HERE, 
			xmlSecErrorsSafeString(cipherName),
			"sc_cipher_decrypt",
			XMLSEC_ERRORS_R_CRYPTO_FAILED,
			"ret=%d", ret);
	    return(-1);
	}
    }

    /* set correct output buffer size */
    ret = xmlSecBufferSetSize(out, outSize + inSize);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE, 
		    xmlSecErrorsSafeString(cipherName),
		    "xmlSecBufferSetSize",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "size=%d", outSize + inSize);
	return(-1);
    }
        
    /* remove the processed block from input */
    ret = xmlSecBufferRemoveHead(in, inSize);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE, 
		    xmlSecErrorsSafeString(cipherName),
		    "xmlSecBufferRemoveHead",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "size=%d", inSize);
	return(-1);
    }
    return(0);
}

static int 
xmlSecSymbianCryptoBlockCipherCtxFinal(xmlSecSymbianCryptoBlockCipherCtxPtr ctx,
				 xmlSecBufferPtr in,
				 xmlSecBufferPtr out,
				 int encrypt,
				 const xmlChar* cipherName,
				 xmlSecTransformCtxPtr transformCtx) {
    xmlSecSize inSize, outSize;
    int blockLen, outLen = 0;
    //xmlSecByte* inBuf;
    xmlSecByte* outBuf;
    int ret;
    
    xmlSecAssert2(ctx, -1);
    xmlSecAssert2(ctx->cipher != 0, -1);
    xmlSecAssert2(ctx->cipherCtx, -1);
    xmlSecAssert2(ctx->ctxInitialized != 0, -1);
    xmlSecAssert2(in, -1);
    xmlSecAssert2(out, -1);
    xmlSecAssert2(transformCtx, -1);

    blockLen = sc_cipher_get_algo_blklen(ctx->cipher);
    xmlSecAssert2(blockLen > 0, -1);

    inSize = xmlSecBufferGetSize(in);
    outSize = xmlSecBufferGetSize(out);

    if(encrypt != 0) {
        xmlSecAssert2(inSize < (xmlSecSize)blockLen, -1);        
    
	/* create padding */
        ret = xmlSecBufferSetMaxSize(in, blockLen);
	if(ret < 0) {
	    xmlSecError(XMLSEC_ERRORS_HERE, 
			xmlSecErrorsSafeString(cipherName),
			"xmlSecBufferSetMaxSize",
			XMLSEC_ERRORS_R_XMLSEC_FAILED,
			"size=%d", blockLen);
	    return(-1);
	}
	//inBuf = xmlSecBufferGetData(in);

    } else {
	if(inSize != (xmlSecSize)blockLen) {
	    xmlSecError(XMLSEC_ERRORS_HERE, 
			xmlSecErrorsSafeString(cipherName),
			NULL,
			XMLSEC_ERRORS_R_INVALID_DATA,
			"data=%d;block=%d", inSize, blockLen);
	    return(-1);
	}
    }
   
    /* process last block */
    ret = xmlSecBufferSetMaxSize(out, outSize + 2 * blockLen);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE, 
		    xmlSecErrorsSafeString(cipherName),
		    "xmlSecBufferSetMaxSize",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "size=%d", outSize + 2 * blockLen);
	return(-1);
    }
    outBuf = xmlSecBufferGetData(out) + outSize;

    if(encrypt) {
	ret = sc_cipher_encrypt(ctx->cipherCtx, outBuf, inSize + blockLen,
				xmlSecBufferGetData(in), inSize, ISLAST);
	if(ret != 0) {
	    xmlSecError(XMLSEC_ERRORS_HERE, 
			xmlSecErrorsSafeString(cipherName),
			"sc_cipher_encrypt",
			XMLSEC_ERRORS_R_CRYPTO_FAILED,
			"ret=%d", ret);
	    return(-1);
	}
	
	if((xmlSecSize)blockLen >= (inSize + 1)) 
	{
		// Symbian should have done the padding,
		//we should re-align inSize so that higher layer won't break
		inSize = blockLen;	
	}
	
    } else {
	ret = sc_cipher_decrypt(ctx->cipherCtx, outBuf, inSize + blockLen,
				xmlSecBufferGetData(in), inSize, &outLen, ISLAST);
	if(ret != 0) {
	    xmlSecError(XMLSEC_ERRORS_HERE, 
			xmlSecErrorsSafeString(cipherName),
			"sc_cipher_decrypt",
			XMLSEC_ERRORS_R_CRYPTO_FAILED,
			"ret=%d", ret);
	    return(-1);
	}	
    }

    if(encrypt == 0) {
	/* check padding */
	if(inSize < outBuf[blockLen - 1]) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(cipherName),
			NULL,
			XMLSEC_ERRORS_R_INVALID_DATA,
			"padding=%d;buffer=%d",
			outBuf[blockLen - 1], inSize);
	    return(-1);	
	}
	// outLen is explicitely written and returned from sc_cipher_decrypt()
	// outLen = inSize - outBuf[blockLen - 1];
    } else {
	outLen = inSize;
    }

    /* set correct output buffer size */
    ret = xmlSecBufferSetSize(out, outSize + outLen);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE, 
		    xmlSecErrorsSafeString(cipherName),
		    "xmlSecBufferSetSize",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "size=%d", outSize + outLen);
	return(-1);
    }
        
    /* remove the processed block from input */
    ret = xmlSecBufferRemoveHead(in, inSize);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE, 
		    xmlSecErrorsSafeString(cipherName),
		    "xmlSecBufferRemoveHead",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "size=%d", inSize);
	return(-1);
    }
    

    return(0);
}


/******************************************************************************
 *
 *  Block Cipher transforms
 *
 * xmlSecSymbianCryptoBlockCipherCtx block is located after xmlSecTransform structure
 * 
 *****************************************************************************/
#define xmlSecSymbianCryptoBlockCipherSize	\
    (sizeof(xmlSecTransform) + sizeof(xmlSecSymbianCryptoBlockCipherCtx))
#define xmlSecSymbianCryptoBlockCipherGetCtx(transform) \
    ((xmlSecSymbianCryptoBlockCipherCtxPtr)(((xmlSecByte*)(transform)) + sizeof(xmlSecTransform)))

static int	xmlSecSymbianCryptoBlockCipherInitialize	(xmlSecTransformPtr transform);
static void	xmlSecSymbianCryptoBlockCipherFinalize		(xmlSecTransformPtr transform);
static int  	xmlSecSymbianCryptoBlockCipherSetKeyReq	(xmlSecTransformPtr transform, 
							 xmlSecKeyReqPtr keyReq);
static int	xmlSecSymbianCryptoBlockCipherSetKey		(xmlSecTransformPtr transform,
							 xmlSecKeyPtr key);
static int	xmlSecSymbianCryptoBlockCipherExecute		(xmlSecTransformPtr transform,
							 int last,
							 xmlSecTransformCtxPtr transformCtx);
static int	xmlSecSymbianCryptoBlockCipherCheckId		(xmlSecTransformPtr transform);
							 


static int
xmlSecSymbianCryptoBlockCipherCheckId(xmlSecTransformPtr transform) {
#ifndef XMLSEC_NO_DES
    if(xmlSecTransformCheckId(transform, xmlSecSymbianCryptoTransformDes3CbcId)) {
	return(1);
    }
#endif /* XMLSEC_NO_DES */

#ifndef XMLSEC_NO_AES
    if(xmlSecTransformCheckId(transform, xmlSecSymbianCryptoTransformAes128CbcId) ||
       xmlSecTransformCheckId(transform, xmlSecSymbianCryptoTransformAes192CbcId) ||
       xmlSecTransformCheckId(transform, xmlSecSymbianCryptoTransformAes256CbcId)) {
       
       return(1);
    }
#endif /* XMLSEC_NO_AES */
    
    return(0);
}

static int 
xmlSecSymbianCryptoBlockCipherInitialize(xmlSecTransformPtr transform) {
    xmlSecSymbianCryptoBlockCipherCtxPtr ctx;
#ifndef XMLSEC_GNUTLS_OLD
    //gpg_err_code_t ret;
    //sc_error_t ret;
    int ret;
#endif /* XMLSEC_GNUTLS_OLD */
    
    xmlSecAssert2(xmlSecSymbianCryptoBlockCipherCheckId(transform), -1);
    xmlSecAssert2(xmlSecTransformCheckSize(transform, xmlSecSymbianCryptoBlockCipherSize), -1);

    ctx = xmlSecSymbianCryptoBlockCipherGetCtx(transform);
    xmlSecAssert2(ctx, -1);
    
    memset(ctx, 0, sizeof(xmlSecSymbianCryptoBlockCipherCtx));

#ifndef XMLSEC_NO_DES
    if(transform->id == xmlSecSymbianCryptoTransformDes3CbcId) {
	ctx->cipher 	= SC_CIPHER_3DES;
	ctx->mode	= SC_CIPHER_MODE_CBC;
	ctx->keyId 	= xmlSecSymbianCryptoKeyDataDesId;
    } else 
#endif /* XMLSEC_NO_DES */

#ifndef XMLSEC_NO_AES
    if(transform->id == xmlSecSymbianCryptoTransformAes128CbcId) {
	ctx->cipher 	= SC_CIPHER_AES128;	
	ctx->mode	= SC_CIPHER_MODE_CBC;
	ctx->keyId 	= xmlSecSymbianCryptoKeyDataAesId;
    } else if(transform->id == xmlSecSymbianCryptoTransformAes192CbcId) {
	ctx->cipher 	= SC_CIPHER_AES192;	
	ctx->mode	= SC_CIPHER_MODE_CBC;
	ctx->keyId 	= xmlSecSymbianCryptoKeyDataAesId;
    } else if(transform->id == xmlSecSymbianCryptoTransformAes256CbcId) {
	ctx->cipher 	= SC_CIPHER_AES256;	
	ctx->mode	= SC_CIPHER_MODE_CBC;
	ctx->keyId 	= xmlSecSymbianCryptoKeyDataAesId;
    } else 
#endif /* XMLSEC_NO_AES */

    if(1) {
	xmlSecError(XMLSEC_ERRORS_HERE, 
		    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
		    NULL,
		    XMLSEC_ERRORS_R_INVALID_TRANSFORM,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }        

#ifndef XMLSEC_GNUTLS_OLD
    ret = sc_cipher_open(&ctx->cipherCtx, ctx->cipher, ctx->mode, SC_CIPHER_SECURE); /* we are paranoid */
    if(ret != 0/*GPG_ERR_NO_ERROR*/) {
#else /* XMLSEC_GNUTLS_OLD */ 
    ctx->cipherCtx = sc_cipher_open(ctx->cipher, ctx->mode, SC_CIPHER_SECURE); /* we are paranoid */
    if(!ctx->cipherCtx) {
#endif /* XMLSEC_GNUTLS_OLD */
	xmlSecError(XMLSEC_ERRORS_HERE, 
		    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
		    "sc_cipher_open",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }
    return(0);
}

static void 
xmlSecSymbianCryptoBlockCipherFinalize(xmlSecTransformPtr transform) {
    xmlSecSymbianCryptoBlockCipherCtxPtr ctx;

    xmlSecAssert(xmlSecSymbianCryptoBlockCipherCheckId(transform));
    xmlSecAssert(xmlSecTransformCheckSize(transform, xmlSecSymbianCryptoBlockCipherSize));

    ctx = xmlSecSymbianCryptoBlockCipherGetCtx(transform);
    xmlSecAssert(ctx);

    if(ctx->cipherCtx) {
	sc_cipher_close(ctx->cipherCtx);
    }
    
    memset(ctx, 0, sizeof(xmlSecSymbianCryptoBlockCipherCtx));
}

static int  
xmlSecSymbianCryptoBlockCipherSetKeyReq(xmlSecTransformPtr transform,  xmlSecKeyReqPtr keyReq) {
    xmlSecSymbianCryptoBlockCipherCtxPtr ctx;

    xmlSecAssert2(xmlSecSymbianCryptoBlockCipherCheckId(transform), -1);
    xmlSecAssert2((transform->operation == xmlSecTransformOperationEncrypt) 
                                || (transform->operation == xmlSecTransformOperationDecrypt), -1);
    xmlSecAssert2(xmlSecTransformCheckSize(transform, xmlSecSymbianCryptoBlockCipherSize), -1);
    xmlSecAssert2(keyReq, -1);

    ctx = xmlSecSymbianCryptoBlockCipherGetCtx(transform);
    xmlSecAssert2(ctx, -1);
    xmlSecAssert2(ctx->cipher != 0, -1);
    xmlSecAssert2(ctx->keyId, -1);

    keyReq->keyId 	= ctx->keyId;
    keyReq->keyType 	= xmlSecKeyDataTypeSymmetric;
    if(transform->operation == xmlSecTransformOperationEncrypt) {
	keyReq->keyUsage = xmlSecKeyUsageEncrypt;
    } else {
	keyReq->keyUsage = xmlSecKeyUsageDecrypt;
    }

    keyReq->keyBitsSize = 8 * sc_cipher_get_algo_keylen(ctx->cipher);
    return(0);
}

static int
xmlSecSymbianCryptoBlockCipherSetKey(xmlSecTransformPtr transform, xmlSecKeyPtr key) {
    xmlSecSymbianCryptoBlockCipherCtxPtr ctx;
    xmlSecBufferPtr buffer;
    xmlSecSize keySize;
    int ret;
    
    xmlSecAssert2(xmlSecSymbianCryptoBlockCipherCheckId(transform), -1);
    xmlSecAssert2((transform->operation == xmlSecTransformOperationEncrypt) 
                                || (transform->operation == xmlSecTransformOperationDecrypt), -1);
    xmlSecAssert2(xmlSecTransformCheckSize(transform, xmlSecSymbianCryptoBlockCipherSize), -1);
    xmlSecAssert2(key, -1);

    ctx = xmlSecSymbianCryptoBlockCipherGetCtx(transform);
    xmlSecAssert2(ctx, -1);
    xmlSecAssert2(ctx->cipherCtx, -1);
    xmlSecAssert2(ctx->cipher != 0, -1);
    xmlSecAssert2(ctx->keyInitialized == 0, -1);
    xmlSecAssert2(ctx->keyId, -1);
    xmlSecAssert2(xmlSecKeyCheckId(key, ctx->keyId), -1);

    keySize = sc_cipher_get_algo_keylen(ctx->cipher);
    xmlSecAssert2(keySize > 0, -1);

    buffer = xmlSecKeyDataBinaryValueGetBuffer(xmlSecKeyGetValue(key));
    xmlSecAssert2(buffer, -1);

    if(xmlSecBufferGetSize(buffer) < keySize) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
		    NULL,
		    XMLSEC_ERRORS_R_INVALID_KEY_DATA_SIZE,
		    "keySize=%d;expected=%d",
		    xmlSecBufferGetSize(buffer), keySize);
	return(-1);
    }
    
    xmlSecAssert2(xmlSecBufferGetData(buffer), -1);
    ret = sc_cipher_setkey(ctx->cipherCtx, xmlSecBufferGetData(buffer), keySize);
    if(ret != 0) {
	xmlSecError(XMLSEC_ERRORS_HERE, 
		    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
		    "sc_cipher_setkey",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    "ret=%d", ret);
	return(-1);
    }
    
    ctx->keyInitialized = 1;
    return(0);
}

static int 
xmlSecSymbianCryptoBlockCipherExecute(xmlSecTransformPtr transform, 
                                            int last, 
                                            xmlSecTransformCtxPtr transformCtx) {
    xmlSecSymbianCryptoBlockCipherCtxPtr ctx;
    xmlSecBufferPtr in, out;
    int ret;
    
    xmlSecAssert2(xmlSecSymbianCryptoBlockCipherCheckId(transform), -1);
    xmlSecAssert2((transform->operation == xmlSecTransformOperationEncrypt) 
                                || (transform->operation == xmlSecTransformOperationDecrypt), -1);
    xmlSecAssert2(xmlSecTransformCheckSize(transform, xmlSecSymbianCryptoBlockCipherSize), -1);
    xmlSecAssert2(transformCtx, -1);

    in = &(transform->inBuf);
    out = &(transform->outBuf);

    ctx = xmlSecSymbianCryptoBlockCipherGetCtx(transform);
    xmlSecAssert2(ctx, -1);

    if(transform->status == xmlSecTransformStatusNone) {
	transform->status = xmlSecTransformStatusWorking;
    }

    if(transform->status == xmlSecTransformStatusWorking) {
	if(ctx->ctxInitialized == 0) {
    	    ret = xmlSecSymbianCryptoBlockCipherCtxInit(ctx, in, out, 
			(transform->operation == xmlSecTransformOperationEncrypt) ? 1 : 0,
			xmlSecTransformGetName(transform), transformCtx);
	    if(ret < 0) {
		xmlSecError(XMLSEC_ERRORS_HERE, 
			    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
			    "xmlSecSymbianCryptoBlockCipherCtxInit",
			    XMLSEC_ERRORS_R_XMLSEC_FAILED,
			    XMLSEC_ERRORS_NO_MESSAGE);
		return(-1);
	    }
	}
	if((ctx->ctxInitialized == 0) && (last != 0)) {
	    xmlSecError(XMLSEC_ERRORS_HERE, 
			xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
			NULL,
			XMLSEC_ERRORS_R_INVALID_DATA,
			"not enough data to initialize transform");
	    return(-1);
	}
	if(ctx->ctxInitialized != 0) {
	    ret = xmlSecSymbianCryptoBlockCipherCtxUpdate(ctx, in, out, 
			(transform->operation == xmlSecTransformOperationEncrypt) ? 1 : 0,
			xmlSecTransformGetName(transform), transformCtx);
	    if(ret < 0) {
		xmlSecError(XMLSEC_ERRORS_HERE, 
			    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
			    "xmlSecSymbianCryptoBlockCipherCtxUpdate",
			    XMLSEC_ERRORS_R_XMLSEC_FAILED,
			    XMLSEC_ERRORS_NO_MESSAGE);
		return(-1);
	    }
	}
	
	if(last) {
	    ret = xmlSecSymbianCryptoBlockCipherCtxFinal(ctx, in, out, 
			(transform->operation == xmlSecTransformOperationEncrypt) ? 1 : 0,
			xmlSecTransformGetName(transform), transformCtx);
	    if(ret < 0) {
		xmlSecError(XMLSEC_ERRORS_HERE, 
			    xmlSecErrorsSafeString(xmlSecTransformGetName(transform)),
			    "xmlSecSymbianCryptoBlockCipherCtxFinal",
			    XMLSEC_ERRORS_R_XMLSEC_FAILED,
			    XMLSEC_ERRORS_NO_MESSAGE);
		return(-1);
	    }
	    transform->status = xmlSecTransformStatusFinished;
	} 
    } else if(transform->status == xmlSecTransformStatusFinished) {
	/* the only way we can get here is if there is no input */
	xmlSecAssert2(xmlSecBufferGetSize(in) == 0, -1);
    } else if(transform->status == xmlSecTransformStatusNone) {
	/* the only way we can get here is if there is no enough data in the input */
	xmlSecAssert2(last == 0, -1);
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


#ifndef XMLSEC_NO_AES
/*********************************************************************
 *
 * AES CBC cipher transforms
 *
 ********************************************************************/
static xmlSecTransformKlass xmlSecSymbianCryptoAes128CbcKlass = {
    /* klass/object sizes */
    sizeof(xmlSecTransformKlass),		/* xmlSecSize klassSize */
    xmlSecSymbianCryptoBlockCipherSize,		/* xmlSecSize objSize */

    xmlSecNameAes128Cbc,			/* const xmlChar* name; */
    xmlSecHrefAes128Cbc,			/* const xmlChar* href; */
    xmlSecTransformUsageEncryptionMethod,	/* xmlSecAlgorithmUsage usage; */

    xmlSecSymbianCryptoBlockCipherInitialize, 		/* xmlSecTransformInitializeMethod initialize; */
    xmlSecSymbianCryptoBlockCipherFinalize,		/* xmlSecTransformFinalizeMethod finalize; */
    NULL,					/* xmlSecTransformNodeReadMethod readNode; */
    NULL,					/* xmlSecTransformNodeWriteMethod writeNode; */
    xmlSecSymbianCryptoBlockCipherSetKeyReq,		/* xmlSecTransformSetKeyMethod setKeyReq; */
    xmlSecSymbianCryptoBlockCipherSetKey,		/* xmlSecTransformSetKeyMethod setKey; */
    NULL,					/* xmlSecTransformValidateMethod validate; */
    xmlSecCrpytoGetDataTypeMethod,		/* xmlSecTransformGetDataTypeMethod getDataType; */
    xmlSecCryptoPushBinMethod,		/* xmlSecTransformPushBinMethod pushBin; */
    xmlSecCryptoPopBinMethod,		/* xmlSecTransformPopBinMethod popBin; */
    NULL,					/* xmlSecTransformPushXmlMethod pushXml; */
    NULL,					/* xmlSecTransformPopXmlMethod popXml; */
    xmlSecSymbianCryptoBlockCipherExecute,		/* xmlSecTransformExecuteMethod execute; */

    NULL,					/* void* reserved0; */
    NULL,					/* void* reserved1; */
};

/**
 * xmlSecSymbianCryptoTransformAes128CbcGetKlass:
 * 
 * AES 128 CBC encryption transform klass.
 * 
 * Returns pointer to AES 128 CBC encryption transform.
 */
EXPORT_C 
xmlSecTransformId 
xmlSecSymbianCryptoTransformAes128CbcGetKlass(void) {
    return(&xmlSecSymbianCryptoAes128CbcKlass);
}

static xmlSecTransformKlass xmlSecSymbianCryptoAes192CbcKlass = {
    /* klass/object sizes */
    sizeof(xmlSecTransformKlass),		/* xmlSecSize klassSize */
    xmlSecSymbianCryptoBlockCipherSize,		/* xmlSecSize objSize */

    xmlSecNameAes192Cbc,			/* const xmlChar* name; */
    xmlSecHrefAes192Cbc,			/* const xmlChar* href; */
    xmlSecTransformUsageEncryptionMethod,	/* xmlSecAlgorithmUsage usage; */

    xmlSecSymbianCryptoBlockCipherInitialize, 		/* xmlSecTransformInitializeMethod initialize; */
    xmlSecSymbianCryptoBlockCipherFinalize,		/* xmlSecTransformFinalizeMethod finalize; */
    NULL,					/* xmlSecTransformNodeReadMethod readNode; */
    NULL,					/* xmlSecTransformNodeWriteMethod writeNode; */
    xmlSecSymbianCryptoBlockCipherSetKeyReq,		/* xmlSecTransformSetKeyMethod setKeyReq; */
    xmlSecSymbianCryptoBlockCipherSetKey,		/* xmlSecTransformSetKeyMethod setKey; */
    NULL,					/* xmlSecTransformValidateMethod validate; */
    xmlSecCrpytoGetDataTypeMethod,		/* xmlSecTransformGetDataTypeMethod getDataType; */
    xmlSecCryptoPushBinMethod,		/* xmlSecTransformPushBinMethod pushBin; */
    xmlSecCryptoPopBinMethod,		/* xmlSecTransformPopBinMethod popBin; */
    NULL,					/* xmlSecTransformPushXmlMethod pushXml; */
    NULL,					/* xmlSecTransformPopXmlMethod popXml; */
    xmlSecSymbianCryptoBlockCipherExecute,		/* xmlSecTransformExecuteMethod execute; */
    
    NULL,					/* void* reserved0; */
    NULL,					/* void* reserved1; */
};

/**
 * xmlSecSymbianCryptoTransformAes192CbcGetKlass:
 * 
 * AES 192 CBC encryption transform klass.
 * 
 * Returns pointer to AES 192 CBC encryption transform.
 */
EXPORT_C 
xmlSecTransformId 
xmlSecSymbianCryptoTransformAes192CbcGetKlass(void) {
    return(&xmlSecSymbianCryptoAes192CbcKlass);
}

static xmlSecTransformKlass xmlSecSymbianCryptoAes256CbcKlass = {
    /* klass/object sizes */
    sizeof(xmlSecTransformKlass),		/* xmlSecSize klassSize */
    xmlSecSymbianCryptoBlockCipherSize,		/* xmlSecSize objSize */

    xmlSecNameAes256Cbc,			/* const xmlChar* name; */
    xmlSecHrefAes256Cbc,			/* const xmlChar* href; */
    xmlSecTransformUsageEncryptionMethod,	/* xmlSecAlgorithmUsage usage; */

    xmlSecSymbianCryptoBlockCipherInitialize, 		/* xmlSecTransformInitializeMethod initialize; */
    xmlSecSymbianCryptoBlockCipherFinalize,		/* xmlSecTransformFinalizeMethod finalize; */
    NULL,					/* xmlSecTransformNodeReadMethod readNode; */
    NULL,					/* xmlSecTransformNodeWriteMethod writeNode; */
    xmlSecSymbianCryptoBlockCipherSetKeyReq,		/* xmlSecTransformSetKeyMethod setKeyReq; */
    xmlSecSymbianCryptoBlockCipherSetKey,		/* xmlSecTransformSetKeyMethod setKey; */
    NULL,					/* xmlSecTransformValidateMethod validate; */
    xmlSecCrpytoGetDataTypeMethod,		/* xmlSecTransformGetDataTypeMethod getDataType; */
    xmlSecCryptoPushBinMethod,		/* xmlSecTransformPushBinMethod pushBin; */
    xmlSecCryptoPopBinMethod,		/* xmlSecTransformPopBinMethod popBin; */
    NULL,					/* xmlSecTransformPushXmlMethod pushXml; */
    NULL,					/* xmlSecTransformPopXmlMethod popXml; */
    xmlSecSymbianCryptoBlockCipherExecute,		/* xmlSecTransformExecuteMethod execute; */
    
    NULL,					/* void* reserved0; */
    NULL,					/* void* reserved1; */
};

/**
 * xmlSecSymbianCryptoTransformAes256CbcGetKlass:
 * 
 * AES 256 CBC encryption transform klass.
 * 
 * Returns pointer to AES 256 CBC encryption transform.
 */
EXPORT_C 
xmlSecTransformId 
xmlSecSymbianCryptoTransformAes256CbcGetKlass(void) {
    return(&xmlSecSymbianCryptoAes256CbcKlass);
}

#endif /* XMLSEC_NO_AES */

#ifndef XMLSEC_NO_DES
static xmlSecTransformKlass xmlSecSymbianCryptoDes3CbcKlass = {
    /* klass/object sizes */
    sizeof(xmlSecTransformKlass),		/* xmlSecSize klassSize */
    xmlSecSymbianCryptoBlockCipherSize,		/* xmlSecSize objSize */

    xmlSecNameDes3Cbc,				/* const xmlChar* name; */
    xmlSecHrefDes3Cbc, 				/* const xmlChar* href; */
    xmlSecTransformUsageEncryptionMethod,	/* xmlSecAlgorithmUsage usage; */

    xmlSecSymbianCryptoBlockCipherInitialize, 		/* xmlSecTransformInitializeMethod initialize; */
    xmlSecSymbianCryptoBlockCipherFinalize,		/* xmlSecTransformFinalizeMethod finalize; */
    NULL,					/* xmlSecTransformNodeReadMethod readNode; */
    NULL,					/* xmlSecTransformNodeWriteMethod writeNode; */
    xmlSecSymbianCryptoBlockCipherSetKeyReq,		/* xmlSecTransformSetKeyMethod setKeyReq; */
    xmlSecSymbianCryptoBlockCipherSetKey,		/* xmlSecTransformSetKeyMethod setKey; */
    NULL,					/* xmlSecTransformValidateMethod validate; */
    xmlSecCrpytoGetDataTypeMethod,		/* xmlSecTransformGetDataTypeMethod getDataType; */
    xmlSecCryptoPushBinMethod,		/* xmlSecTransformPushBinMethod pushBin; */
    xmlSecCryptoPopBinMethod,		/* xmlSecTransformPopBinMethod popBin; */
    NULL,					/* xmlSecTransformPushXmlMethod pushXml; */
    NULL,					/* xmlSecTransformPopXmlMethod popXml; */
    xmlSecSymbianCryptoBlockCipherExecute,		/* xmlSecTransformExecuteMethod execute; */
    
    NULL,					/* void* reserved0; */
    NULL,					/* void* reserved1; */
};

/** 
 * xmlSecSymbianCryptoTransformDes3CbcGetKlass:
 *
 * Triple DES CBC encryption transform klass.
 * 
 * Returns pointer to Triple DES encryption transform.
 */
EXPORT_C
xmlSecTransformId 
xmlSecSymbianCryptoTransformDes3CbcGetKlass(void) {
    return(&xmlSecSymbianCryptoDes3CbcKlass);
}
#endif /* XMLSEC_NO_DES */


