/** 
 *
 * XMLSec library
 * 
 * DES Algorithm support
 * 
 * This is free software; see Copyright file in the source
 * distribution for preciese wording.
 * 
 * Copyright (C) 2002-2003 Aleksey Sanin <aleksey@aleksey.com>
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */
#include "xmlsecc_globals.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "xmlsecc_config.h"
#include "xmlsec_xmlsec.h"
#include "xmlsec_xmltree.h"
#include "xmlsec_keys.h"
#include "xmlsec_keyinfo.h"
#include "xmlsec_transforms.h"
#include "xmlsec_errors.h"

#include "xmlsecc_crypto.h"

/*****************************************************************************
 * 
 * Symmetic (binary) keys - just a wrapper for xmlSecKeyDataBinary
 *
 ****************************************************************************/
static int	xmlSecSymbianCryptoSymKeyDataInitialize	(xmlSecKeyDataPtr data);
static int	xmlSecSymbianCryptoSymKeyDataDuplicate		(xmlSecKeyDataPtr dst,
							 xmlSecKeyDataPtr src);
static void	xmlSecSymbianCryptoSymKeyDataFinalize		(xmlSecKeyDataPtr data);
static int	xmlSecSymbianCryptoSymKeyDataXmlRead		(xmlSecKeyDataId id,
							 xmlSecKeyPtr key,
							 xmlNodePtr node,
							 xmlSecKeyInfoCtxPtr keyInfoCtx);
static int	xmlSecSymbianCryptoSymKeyDataXmlWrite		(xmlSecKeyDataId id,
							 xmlSecKeyPtr key,
							 xmlNodePtr node,
							 xmlSecKeyInfoCtxPtr keyInfoCtx);
static int	xmlSecSymbianCryptoSymKeyDataBinRead		(xmlSecKeyDataId id,
							 xmlSecKeyPtr key,
							 const xmlSecByte* buf,
							 xmlSecSize bufSize,
							 xmlSecKeyInfoCtxPtr keyInfoCtx);
static int	xmlSecSymbianCryptoSymKeyDataBinWrite		(xmlSecKeyDataId id,
							 xmlSecKeyPtr key,
							 xmlSecByte** buf,
							 xmlSecSize* bufSize,
							 xmlSecKeyInfoCtxPtr keyInfoCtx);
static int	xmlSecSymbianCryptoSymKeyDataGenerate		(xmlSecKeyDataPtr data,
							 xmlSecSize sizeBits,
							 xmlSecKeyDataType type);

static xmlSecKeyDataType xmlSecSymbianCryptoSymKeyDataGetType	(xmlSecKeyDataPtr data);
static xmlSecSize	 xmlSecSymbianCryptoSymKeyDataGetSize	(xmlSecKeyDataPtr data);
static void	xmlSecSymbianCryptoSymKeyDataDebugDump		(xmlSecKeyDataPtr data,
							 FILE* output);
static void	xmlSecSymbianCryptoSymKeyDataDebugXmlDump	(xmlSecKeyDataPtr data,
							 FILE* output);
static int	xmlSecSymbianCryptoSymKeyDataKlassCheck	(xmlSecKeyDataKlass* klass);

#define xmlSecSymbianCryptoSymKeyDataCheckId(data) \
    (xmlSecKeyDataIsValid((data)) && \
     xmlSecSymbianCryptoSymKeyDataKlassCheck((data)->id))

static int
xmlSecSymbianCryptoSymKeyDataInitialize(xmlSecKeyDataPtr data) {
    xmlSecAssert2(xmlSecSymbianCryptoSymKeyDataCheckId(data), -1);
    
    return(xmlSecKeyDataBinaryValueInitialize(data));
}

static int
xmlSecSymbianCryptoSymKeyDataDuplicate(xmlSecKeyDataPtr dst, xmlSecKeyDataPtr src) {
    xmlSecAssert2(xmlSecSymbianCryptoSymKeyDataCheckId(dst), -1);
    xmlSecAssert2(xmlSecSymbianCryptoSymKeyDataCheckId(src), -1);
    xmlSecAssert2(dst->id == src->id, -1);
        
    return(xmlSecKeyDataBinaryValueDuplicate(dst, src));
}

static void
xmlSecSymbianCryptoSymKeyDataFinalize(xmlSecKeyDataPtr data) {
    xmlSecAssert(xmlSecSymbianCryptoSymKeyDataCheckId(data));
    
    xmlSecKeyDataBinaryValueFinalize(data);
}

static int
xmlSecSymbianCryptoSymKeyDataXmlRead(xmlSecKeyDataId id, xmlSecKeyPtr key,
			       xmlNodePtr node, xmlSecKeyInfoCtxPtr keyInfoCtx) {
    xmlSecAssert2(xmlSecSymbianCryptoSymKeyDataKlassCheck(id), -1);
    
    return(xmlSecKeyDataBinaryValueXmlRead(id, key, node, keyInfoCtx));
}

static int 
xmlSecSymbianCryptoSymKeyDataXmlWrite(xmlSecKeyDataId id, xmlSecKeyPtr key,
				    xmlNodePtr node, xmlSecKeyInfoCtxPtr keyInfoCtx) {
    xmlSecAssert2(xmlSecSymbianCryptoSymKeyDataKlassCheck(id), -1);
    
    return(xmlSecKeyDataBinaryValueXmlWrite(id, key, node, keyInfoCtx));
}

static int
xmlSecSymbianCryptoSymKeyDataBinRead(xmlSecKeyDataId id, xmlSecKeyPtr key,
				    const xmlSecByte* buf, xmlSecSize bufSize,
				    xmlSecKeyInfoCtxPtr keyInfoCtx) {
    xmlSecAssert2(xmlSecSymbianCryptoSymKeyDataKlassCheck(id), -1);
    
    return(xmlSecKeyDataBinaryValueBinRead(id, key, buf, bufSize, keyInfoCtx));
}

static int
xmlSecSymbianCryptoSymKeyDataBinWrite(xmlSecKeyDataId id, xmlSecKeyPtr key,
				    xmlSecByte** buf, xmlSecSize* bufSize,
				    xmlSecKeyInfoCtxPtr keyInfoCtx) {
    xmlSecAssert2(xmlSecSymbianCryptoSymKeyDataKlassCheck(id), -1);
    
    return(xmlSecKeyDataBinaryValueBinWrite(id, key, buf, bufSize, keyInfoCtx));
}

static int
xmlSecSymbianCryptoSymKeyDataGenerate(xmlSecKeyDataPtr data, 
                                        xmlSecSize sizeBits, 
                                        xmlSecKeyDataType type ATTRIBUTE_UNUSED) {
    xmlSecBufferPtr buffer;

    xmlSecAssert2(xmlSecSymbianCryptoSymKeyDataCheckId(data), -1);
    xmlSecAssert2(sizeBits > 0, -1);

    buffer = xmlSecKeyDataBinaryValueGetBuffer(data);
    xmlSecAssert2(buffer, -1);
    
    return(xmlSecSymbianCryptoGenerateRandom(buffer, (sizeBits + 7) / 8));
}

static xmlSecKeyDataType
xmlSecSymbianCryptoSymKeyDataGetType(xmlSecKeyDataPtr data) {
    xmlSecBufferPtr buffer;

    xmlSecAssert2(xmlSecSymbianCryptoSymKeyDataCheckId(data), xmlSecKeyDataTypeUnknown);

    buffer = xmlSecKeyDataBinaryValueGetBuffer(data);
    xmlSecAssert2(buffer, xmlSecKeyDataTypeUnknown);

    return((xmlSecBufferGetSize(buffer) > 0) ? 
                    xmlSecKeyDataTypeSymmetric : xmlSecKeyDataTypeUnknown);
}

static xmlSecSize 
xmlSecSymbianCryptoSymKeyDataGetSize(xmlSecKeyDataPtr data) {
    xmlSecAssert2(xmlSecSymbianCryptoSymKeyDataCheckId(data), 0);
    
    return(xmlSecKeyDataBinaryValueGetSize(data));
}

static void 
xmlSecSymbianCryptoSymKeyDataDebugDump(xmlSecKeyDataPtr data, FILE* output) {
    xmlSecAssert(xmlSecSymbianCryptoSymKeyDataCheckId(data));
    
    xmlSecKeyDataBinaryValueDebugDump(data, output);    
}

static void
xmlSecSymbianCryptoSymKeyDataDebugXmlDump(xmlSecKeyDataPtr data, FILE* output) {
    xmlSecAssert(xmlSecSymbianCryptoSymKeyDataCheckId(data));
    
    xmlSecKeyDataBinaryValueDebugXmlDump(data, output);    
}

static int 
xmlSecSymbianCryptoSymKeyDataKlassCheck(xmlSecKeyDataKlass* klass) {    
#ifndef XMLSEC_NO_DES
    if(klass == xmlSecSymbianCryptoKeyDataDesId) {
	return(1);
    }
#endif /* XMLSEC_NO_DES */

#ifndef XMLSEC_NO_AES
    if(klass == xmlSecSymbianCryptoKeyDataAesId) {
	return(1);
    }
#endif /* XMLSEC_NO_AES */

#ifndef XMLSEC_NO_HMAC
    if(klass == xmlSecSymbianCryptoKeyDataHmacId) {
	return(1);
    }
#endif /* XMLSEC_NO_HMAC */

    return(0);
}

#ifndef XMLSEC_NO_AES
/**************************************************************************
 *
 * <xmlsec:AESKeyValue> processing
 *
 *************************************************************************/
static xmlSecKeyDataKlass xmlSecSymbianCryptoKeyDataAesKlass = {
    sizeof(xmlSecKeyDataKlass),
    xmlSecKeyDataBinarySize,

    /* data */
    xmlSecNameAESKeyValue,
    xmlSecKeyDataUsageKeyValueNode | xmlSecKeyDataUsageRetrievalMethodNodeXml, 
						/* xmlSecKeyDataUsage usage; */
    xmlSecHrefAESKeyValue,			/* const xmlChar* href; */
    xmlSecNodeAESKeyValue,			/* const xmlChar* dataNodeName; */
    xmlSecNs,					/* const xmlChar* dataNodeNs; */
    
    /* constructors/destructor */
    xmlSecSymbianCryptoSymKeyDataInitialize,		/* xmlSecKeyDataInitializeMethod initialize; */
    xmlSecSymbianCryptoSymKeyDataDuplicate,		/* xmlSecKeyDataDuplicateMethod duplicate; */
    xmlSecSymbianCryptoSymKeyDataFinalize,		/* xmlSecKeyDataFinalizeMethod finalize; */
    xmlSecSymbianCryptoSymKeyDataGenerate,		/* xmlSecKeyDataGenerateMethod generate; */
    
    /* get info */
    xmlSecSymbianCryptoSymKeyDataGetType, 		/* xmlSecKeyDataGetTypeMethod getType; */
    xmlSecSymbianCryptoSymKeyDataGetSize,		/* xmlSecKeyDataGetSizeMethod getSize; */
    NULL,					/* xmlSecKeyDataGetIdentifier getIdentifier; */

    /* read/write */
    xmlSecSymbianCryptoSymKeyDataXmlRead,		/* xmlSecKeyDataXmlReadMethod xmlRead; */
    xmlSecSymbianCryptoSymKeyDataXmlWrite,		/* xmlSecKeyDataXmlWriteMethod xmlWrite; */
    xmlSecSymbianCryptoSymKeyDataBinRead,		/* xmlSecKeyDataBinReadMethod binRead; */
    xmlSecSymbianCryptoSymKeyDataBinWrite,		/* xmlSecKeyDataBinWriteMethod binWrite; */

    /* debug */
    xmlSecSymbianCryptoSymKeyDataDebugDump,		/* xmlSecKeyDataDebugDumpMethod debugDump; */
    xmlSecSymbianCryptoSymKeyDataDebugXmlDump, 	/* xmlSecKeyDataDebugDumpMethod debugXmlDump; */

    /* reserved for the future */
    NULL,					/* void* reserved0; */
    NULL,					/* void* reserved1; */
};

/** 
 * xmlSecSymbianCryptoKeyDataAesGetKlass:
 * 
 * The AES key data klass.
 *
 * Returns AES key data klass.
 */
EXPORT_C
xmlSecKeyDataId 
xmlSecSymbianCryptoKeyDataAesGetKlass(void) {
    return(&xmlSecSymbianCryptoKeyDataAesKlass);
}

/**
 * xmlSecSymbianCryptoKeyDataAesSet:
 * @data:		the pointer to AES key data.
 * @buf:		the pointer to key value.
 * @bufSize:		the key value size (in bytes).
 *
 * Sets the value of AES key data.
 *
 * Returns 0 on success or a negative value if an error occurs.
 */
EXPORT_C
int
xmlSecSymbianCryptoKeyDataAesSet(xmlSecKeyDataPtr data, 
                                                const xmlSecByte* buf, xmlSecSize bufSize) {
    xmlSecBufferPtr buffer;
    
    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataAesId), -1);
    xmlSecAssert2(buf, -1);
    xmlSecAssert2(bufSize > 0, -1);
    
    buffer = xmlSecKeyDataBinaryValueGetBuffer(data);
    xmlSecAssert2(buffer, -1);
    
    return(xmlSecBufferSetData(buffer, buf, bufSize));
}
#endif /* XMLSEC_NO_AES */

#ifndef XMLSEC_NO_DES
/**************************************************************************
 *
 * <xmlsec:DESKeyValue> processing
 *
 *************************************************************************/
static xmlSecKeyDataKlass xmlSecSymbianCryptoKeyDataDesKlass = {
    sizeof(xmlSecKeyDataKlass),
    xmlSecKeyDataBinarySize,

    /* data */
    xmlSecNameDESKeyValue,
    xmlSecKeyDataUsageKeyValueNode | xmlSecKeyDataUsageRetrievalMethodNodeXml, 
						/* xmlSecKeyDataUsage usage; */
    xmlSecHrefDESKeyValue,			/* const xmlChar* href; */
    xmlSecNodeDESKeyValue,			/* const xmlChar* dataNodeName; */
    xmlSecNs,					/* const xmlChar* dataNodeNs; */
    
    /* constructors/destructor */
    xmlSecSymbianCryptoSymKeyDataInitialize,		/* xmlSecKeyDataInitializeMethod initialize; */
    xmlSecSymbianCryptoSymKeyDataDuplicate,		/* xmlSecKeyDataDuplicateMethod duplicate; */
    xmlSecSymbianCryptoSymKeyDataFinalize,		/* xmlSecKeyDataFinalizeMethod finalize; */
    xmlSecSymbianCryptoSymKeyDataGenerate,		/* xmlSecKeyDataGenerateMethod generate; */
    
    /* get info */
    xmlSecSymbianCryptoSymKeyDataGetType, 		/* xmlSecKeyDataGetTypeMethod getType; */
    xmlSecSymbianCryptoSymKeyDataGetSize,		/* xmlSecKeyDataGetSizeMethod getSize; */
    NULL,					/* xmlSecKeyDataGetIdentifier getIdentifier; */

    /* read/write */
    xmlSecSymbianCryptoSymKeyDataXmlRead,		/* xmlSecKeyDataXmlReadMethod xmlRead; */
    xmlSecSymbianCryptoSymKeyDataXmlWrite,		/* xmlSecKeyDataXmlWriteMethod xmlWrite; */
    xmlSecSymbianCryptoSymKeyDataBinRead,		/* xmlSecKeyDataBinReadMethod binRead; */
    xmlSecSymbianCryptoSymKeyDataBinWrite,		/* xmlSecKeyDataBinWriteMethod binWrite; */

    /* debug */
    xmlSecSymbianCryptoSymKeyDataDebugDump,		/* xmlSecKeyDataDebugDumpMethod debugDump; */
    xmlSecSymbianCryptoSymKeyDataDebugXmlDump, 	/* xmlSecKeyDataDebugDumpMethod debugXmlDump; */

    /* reserved for the future */
    NULL,					/* void* reserved0; */
    NULL,					/* void* reserved1; */
};

/** 
 * xmlSecSymbianCryptoKeyDataDesGetKlass:
 * 
 * The DES key data klass.
 *
 * Returns DES key data klass.
 */
EXPORT_C
xmlSecKeyDataId 
xmlSecSymbianCryptoKeyDataDesGetKlass(void) {
    return(&xmlSecSymbianCryptoKeyDataDesKlass);
}

/**
 * xmlSecSymbianCryptoKeyDataDesSet:
 * @data:		the pointer to DES key data.
 * @buf:		the pointer to key value.
 * @bufSize:		the key value size (in bytes).
 *
 * Sets the value of DES key data.
 *
 * Returns 0 on success or a negative value if an error occurs.
 */
EXPORT_C
int
xmlSecSymbianCryptoKeyDataDesSet(xmlSecKeyDataPtr data, 
                                                const xmlSecByte* buf, 
                                                xmlSecSize bufSize) {
    xmlSecBufferPtr buffer;
    
    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataDesId), -1);
    xmlSecAssert2(buf, -1);
    xmlSecAssert2(bufSize > 0, -1);
    
    buffer = xmlSecKeyDataBinaryValueGetBuffer(data);
    xmlSecAssert2(buffer, -1);
    
    return(xmlSecBufferSetData(buffer, buf, bufSize));
}

#endif /* XMLSEC_NO_DES */

#ifndef XMLSEC_NO_HMAC
/**************************************************************************
 *
 * <xmlsec:HMACKeyValue> processing
 *
 *************************************************************************/
static xmlSecKeyDataKlass xmlSecSymbianCryptoKeyDataHmacKlass = {
    sizeof(xmlSecKeyDataKlass),
    xmlSecKeyDataBinarySize,

    /* data */
    xmlSecNameHMACKeyValue,
    xmlSecKeyDataUsageKeyValueNode | xmlSecKeyDataUsageRetrievalMethodNodeXml, 
						/* xmlSecKeyDataUsage usage; */
    xmlSecHrefHMACKeyValue,			/* const xmlChar* href; */
    xmlSecNodeHMACKeyValue,			/* const xmlChar* dataNodeName; */
    xmlSecNs,					/* const xmlChar* dataNodeNs; */
    
    /* constructors/destructor */
    xmlSecSymbianCryptoSymKeyDataInitialize,		/* xmlSecKeyDataInitializeMethod initialize; */
    xmlSecSymbianCryptoSymKeyDataDuplicate,		/* xmlSecKeyDataDuplicateMethod duplicate; */
    xmlSecSymbianCryptoSymKeyDataFinalize,		/* xmlSecKeyDataFinalizeMethod finalize; */
    xmlSecSymbianCryptoSymKeyDataGenerate,		/* xmlSecKeyDataGenerateMethod generate; */
    
    /* get info */
    xmlSecSymbianCryptoSymKeyDataGetType, 		/* xmlSecKeyDataGetTypeMethod getType; */
    xmlSecSymbianCryptoSymKeyDataGetSize,		/* xmlSecKeyDataGetSizeMethod getSize; */
    NULL,					/* xmlSecKeyDataGetIdentifier getIdentifier; */

    /* read/write */
    xmlSecSymbianCryptoSymKeyDataXmlRead,		/* xmlSecKeyDataXmlReadMethod xmlRead; */
    xmlSecSymbianCryptoSymKeyDataXmlWrite,		/* xmlSecKeyDataXmlWriteMethod xmlWrite; */
    xmlSecSymbianCryptoSymKeyDataBinRead,		/* xmlSecKeyDataBinReadMethod binRead; */
    xmlSecSymbianCryptoSymKeyDataBinWrite,		/* xmlSecKeyDataBinWriteMethod binWrite; */

    /* debug */
    xmlSecSymbianCryptoSymKeyDataDebugDump,		/* xmlSecKeyDataDebugDumpMethod debugDump; */
    xmlSecSymbianCryptoSymKeyDataDebugXmlDump, 	/* xmlSecKeyDataDebugDumpMethod debugXmlDump; */

    /* reserved for the future */
    NULL,					/* void* reserved0; */
    NULL,					/* void* reserved1; */
};

/** 
 * xmlSecSymbianCryptoKeyDataHmacGetKlass:
 * 
 * The HMAC key data klass.
 *
 * Returns HMAC key data klass.
 */
EXPORT_C
xmlSecKeyDataId 
xmlSecSymbianCryptoKeyDataHmacGetKlass(void) {
    return(&xmlSecSymbianCryptoKeyDataHmacKlass);
}

/**
 * xmlSecSymbianCryptoKeyDataHmacSet:
 * @data:		the pointer to HMAC key data.
 * @buf:		the pointer to key value.
 * @bufSize:		the key value size (in bytes).
 *
 * Sets the value of HMAC key data.
 *
 * Returns 0 on success or a negative value if an error occurs.
 */
EXPORT_C
int
xmlSecSymbianCryptoKeyDataHmacSet(xmlSecKeyDataPtr data, 
                                                const xmlSecByte* buf, 
                                                xmlSecSize bufSize) {
    xmlSecBufferPtr buffer;
    
    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataHmacId), -1);
    xmlSecAssert2(buf, -1);
    xmlSecAssert2(bufSize > 0, -1);
    
    buffer = xmlSecKeyDataBinaryValueGetBuffer(data);
    xmlSecAssert2(buffer, -1);
    
    return(xmlSecBufferSetData(buffer, buf, bufSize));
}

#endif /* XMLSEC_NO_HMAC */
