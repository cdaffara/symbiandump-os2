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
//#include "globals.h"
#include "xmlsecc_config.h"
#ifndef XMLSEC_NO_X509
#include "xmlsecc_globals.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>

#include <stdapis/libxml2/libxml2_tree.h>
#include <stdapis/libxml2/libxml2_globals.h>

#include "xmlsec_xmlsec.h"
#include "xmlsec_xmltree.h"
#include "xmlsec_keys.h"
#include "xmlsec_keyinfo.h"
#include "xmlsec_keysmngr.h"
#include "xmlsec_x509.h"
#include "xmlsec_base64.h"
#include "xmlsec_errors.h"

#include "xmlsecc_crypto.h"
#include "xmlsecc_evp.h"
#include "xmlsecc_x509.h"

/*************************************************************************
 *
 * X509 utility functions
 *
 ************************************************************************/
static int		xmlSecSymbianCryptoX509DataNodeRead		(xmlSecKeyDataPtr data,
								 xmlNodePtr node,
								 xmlSecKeyInfoCtxPtr keyInfoCtx);
static int		xmlSecSymbianCryptoX509CertificateNodeRead	(xmlSecKeyDataPtr data,
								 xmlNodePtr node,
								 xmlSecKeyInfoCtxPtr keyInfoCtx);
static int		xmlSecSymbianCryptoX509CertificateNodeWrite	(X509* cert,
								 xmlNodePtr node,
								 xmlSecKeyInfoCtxPtr keyInfoCtx);
static int		xmlSecSymbianCryptoX509SubjectNameNodeRead	(xmlSecKeyDataPtr data,
								 xmlNodePtr node,
								 xmlSecKeyInfoCtxPtr keyInfoCtx);
static int		xmlSecSymbianCryptoX509SubjectNameNodeWrite	(X509* cert,
								 xmlNodePtr node,
								 xmlSecKeyInfoCtxPtr keyInfoCtx);
static int		xmlSecSymbianCryptoX509IssuerSerialNodeRead	(xmlSecKeyDataPtr data,
								 xmlNodePtr node,
								 xmlSecKeyInfoCtxPtr keyInfoCtx);
static int		xmlSecSymbianCryptoX509IssuerSerialNodeWrite	(X509* cert,
								 xmlNodePtr node,
								 xmlSecKeyInfoCtxPtr keyInfoCtx);
static int		xmlSecSymbianCryptoX509SKINodeRead		(xmlSecKeyDataPtr data,
								 xmlNodePtr node,
								 xmlSecKeyInfoCtxPtr keyInfoCtx);
static int		xmlSecSymbianCryptoX509SKINodeWrite		(X509* cert,
								 xmlNodePtr node,
								 xmlSecKeyInfoCtxPtr keyInfoCtx);
static int		xmlSecSymbianCryptoX509CRLNodeRead		(xmlSecKeyDataPtr data,
								 xmlNodePtr node,
								 xmlSecKeyInfoCtxPtr keyInfoCtx);
static int		xmlSecSymbianCryptoX509CRLNodeWrite		(X509_CRL* crl,
								 xmlNodePtr node,
								 xmlSecKeyInfoCtxPtr keyInfoCtx);
static int		xmlSecSymbianCryptoKeyDataX509VerifyAndExtractKey(xmlSecKeyDataPtr data, 
								xmlSecKeyPtr key,
								xmlSecKeyInfoCtxPtr keyInfoCtx);
static X509*		xmlSecSymbianCryptoX509CertDerRead		(const xmlSecByte* buf, 
								 xmlSecSize size);
static X509*		xmlSecSymbianCryptoX509CertBase64DerRead	(xmlChar* buf);
static xmlChar*		xmlSecSymbianCryptoX509CertBase64DerWrite	(X509* cert, 
								 int base64LineWrap);
static X509_CRL*	xmlSecSymbianCryptoX509CrlDerRead		(xmlSecByte* buf, 
								 xmlSecSize size);
static X509_CRL*	xmlSecSymbianCryptoX509CrlBase64DerRead	(xmlChar* buf);
static xmlChar*		xmlSecSymbianCryptoX509CrlBase64DerWrite	(X509_CRL* crl, 
								 int base64LineWrap);
//static xmlChar*		xmlSecSymbianCryptoX509NameWrite		(X509_NAME* nm);
#ifdef XMLSEC_FUTURE_SUPPORT
static xmlChar*		xmlSecSymbianCryptoASN1IntegerWrite		(ASN1_INTEGER *asni);
#endif //XMLSEC_FUTURE_SUPPORT
static xmlChar*		xmlSecSymbianCryptoX509SKIWrite		(X509* cert);
static void		xmlSecSymbianCryptoX509CertDebugDump		(X509* cert, 
								 FILE* output);
static void		xmlSecSymbianCryptoX509CertDebugXmlDump	(X509* cert, 
								 FILE* output);
#ifdef XMLSEC_FUTURE_SUPPORT								 
static int		xmlSecSymbianCryptoX509CertGetTime		(ASN1_TIME* t,
								 time_t* res);
#endif //XMLSEC_FUTURE_SUPPORT
/*************************************************************************
*
* Support for SymbianCertStore
*
*************************************************************************/
static int XmlSecCertStoreFlag = 0;

// ---------------------------------------------------------------------------
// Set SymbianCertStore flag.
// ---------------------------------------------------------------------------
//
EXPORT_C 
void xmlSecSetCertStoreFlag()
    {
    XmlSecCertStoreFlag = 1;
    }
    
// ---------------------------------------------------------------------------
// Reset SymbianCertStore  flag.
// ---------------------------------------------------------------------------
//
EXPORT_C 
void xmlSecResetCertStoreFlag()
    {
    XmlSecCertStoreFlag = 0;
    }
// ---------------------------------------------------------------------------
// Check SymbianCertStore  flag.
// ---------------------------------------------------------------------------
//
EXPORT_C 
int xmlSecCheckCertStoreFlag()
    {
    return XmlSecCertStoreFlag;
    }    
/*************************************************************************
 *
 * Internal SymbianCrypto X509 data CTX
 *
 ************************************************************************/
typedef struct _xmlSecSymbianCryptoX509DataCtx		xmlSecSymbianCryptoX509DataCtx,
							*xmlSecSymbianCryptoX509DataCtxPtr;
struct _xmlSecSymbianCryptoX509DataCtx {
    X509*		keyCert;
    STACK_OF(X509)*	certsList;
    STACK_OF(X509_CRL)*	crlsList;
};
/**************************************************************************
 *
 * <dsig:X509Data> processing
 *
 *
 * The X509Data  Element (http://www.w3.org/TR/xmldsig-core/#sec-X509Data)
 *
 * An X509Data element within KeyInfo contains one or more identifiers of keys 
 * or X509 certificates (or certificates' identifiers or a revocation list). 
 * The content of X509Data is:
 *
 *  1. At least one element, from the following set of element types; any of these may appear together or more than once iff (if and only if) each instance describes or is related to the same certificate:
 *  2.
 *    * The X509IssuerSerial element, which contains an X.509 issuer 
 *	distinguished name/serial number pair that SHOULD be compliant 
 *	with RFC2253 [LDAP-DN],
 *    * The X509SubjectName element, which contains an X.509 subject 
 *	distinguished name that SHOULD be compliant with RFC2253 [LDAP-DN],
 *    * The X509SKI element, which contains the base64 encoded plain (i.e. 
 *	non-DER-encoded) value of a X509 V.3 SubjectKeyIdentifier extension.
 *    * The X509Certificate element, which contains a base64-encoded [X509v3] 
 *	certificate, and
 *    * Elements from an external namespace which accompanies/complements any 
 *	of the elements above.
 *    * The X509CRL element, which contains a base64-encoded certificate 
 *	revocation list (CRL) [X509v3].
 *
 * Any X509IssuerSerial, X509SKI, and X509SubjectName elements that appear 
 * MUST refer to the certificate or certificates containing the validation key.
 * All such elements that refer to a particular individual certificate MUST be 
 * grouped inside a single X509Data element and if the certificate to which 
 * they refer appears, it MUST also be in that X509Data element.
 *
 * Any X509IssuerSerial, X509SKI, and X509SubjectName elements that relate to 
 * the same key but different certificates MUST be grouped within a single 
 * KeyInfo but MAY occur in multiple X509Data elements.
 *
 * All certificates appearing in an X509Data element MUST relate to the 
 * validation key by either containing it or being part of a certification 
 * chain that terminates in a certificate containing the validation key.
 *
 * No ordering is implied by the above constraints.
 *
 * Note, there is no direct provision for a PKCS#7 encoded "bag" of 
 * certificates or CRLs. However, a set of certificates and CRLs can occur 
 * within an X509Data element and multiple X509Data elements can occur in a 
 * KeyInfo. Whenever multiple certificates occur in an X509Data element, at 
 * least one such certificate must contain the public key which verifies the 
 * signature.
 *
 * Schema Definition
 *
 *  <element name="X509Data" type="ds:X509DataType"/> 
 *  <complexType name="X509DataType">
 *    <sequence maxOccurs="unbounded">
 *      <choice>
 *        <element name="X509IssuerSerial" type="ds:X509IssuerSerialType"/>
 *        <element name="X509SKI" type="base64Binary"/>
 *        <element name="X509SubjectName" type="string"/>
 *        <element name="X509Certificate" type="base64Binary"/>
 *        <element name="X509CRL" type="base64Binary"/>
 *        <any namespace="##other" processContents="lax"/>
 *      </choice>
 *    </sequence>
 *  </complexType>
 *  <complexType name="X509IssuerSerialType"> 
 *    <sequence> 
 *       <element name="X509IssuerName" type="string"/> 
 *       <element name="X509SerialNumber" type="integer"/> 
 *     </sequence>
 *  </complexType>
 *
 *  DTD
 *
 *    <!ELEMENT X509Data ((X509IssuerSerial | X509SKI | X509SubjectName |
 *                          X509Certificate | X509CRL)+ %X509.ANY;)>
 *    <!ELEMENT X509IssuerSerial (X509IssuerName, X509SerialNumber) >
 *    <!ELEMENT X509IssuerName (#PCDATA) >
 *    <!ELEMENT X509SubjectName (#PCDATA) >
 *    <!ELEMENT X509SerialNumber (#PCDATA) >
 *    <!ELEMENT X509SKI (#PCDATA) >
 *    <!ELEMENT X509Certificate (#PCDATA) >
 *    <!ELEMENT X509CRL (#PCDATA) >
 *
 * -----------------------------------------------------------------------
 *
 * xmlSecSymbianCryptoX509DataCtx is located after xmlSecTransform
 *
 *************************************************************************/
#define xmlSecSymbianCryptoX509DataSize	\
    (sizeof(xmlSecKeyData) + sizeof(xmlSecSymbianCryptoX509DataCtx))	
#define xmlSecSymbianCryptoX509DataGetCtx(data) \
    ((xmlSecSymbianCryptoX509DataCtxPtr)(((xmlSecByte*)(data)) + sizeof(xmlSecKeyData)))

static int		xmlSecSymbianCryptoKeyDataX509Initialize	(xmlSecKeyDataPtr data);
static int		xmlSecSymbianCryptoKeyDataX509Duplicate	(xmlSecKeyDataPtr dst,
								 xmlSecKeyDataPtr src);
static void		xmlSecSymbianCryptoKeyDataX509Finalize	(xmlSecKeyDataPtr data);
static int		xmlSecSymbianCryptoKeyDataX509XmlRead		(xmlSecKeyDataId id,
								 xmlSecKeyPtr key,
								 xmlNodePtr node,
								 xmlSecKeyInfoCtxPtr keyInfoCtx);
static int		xmlSecSymbianCryptoKeyDataX509XmlWrite	(xmlSecKeyDataId id,
								 xmlSecKeyPtr key,
								 xmlNodePtr node,
								 xmlSecKeyInfoCtxPtr keyInfoCtx);
static xmlSecKeyDataType xmlSecSymbianCryptoKeyDataX509GetType	(xmlSecKeyDataPtr data);
static const xmlChar*	xmlSecSymbianCryptoKeyDataX509GetIdentifier	(xmlSecKeyDataPtr data);

static void		xmlSecSymbianCryptoKeyDataX509DebugDump	(xmlSecKeyDataPtr data,
								 FILE* output);
static void		xmlSecSymbianCryptoKeyDataX509DebugXmlDump	(xmlSecKeyDataPtr data,
								 FILE* output);



static xmlSecKeyDataKlass xmlSecSymbianCryptoKeyDataX509Klass = {
    sizeof(xmlSecKeyDataKlass),
    xmlSecSymbianCryptoX509DataSize,

    /* data */
    xmlSecNameX509Data,
    xmlSecKeyDataUsageKeyInfoNode | xmlSecKeyDataUsageRetrievalMethodNodeXml, 
						/* xmlSecKeyDataUsage usage; */
    xmlSecHrefX509Data,				/* const xmlChar* href; */
    xmlSecNodeX509Data,				/* const xmlChar* dataNodeName; */
    xmlSecDSigNs,				/* const xmlChar* dataNodeNs; */
    
    /* constructors/destructor */
    xmlSecSymbianCryptoKeyDataX509Initialize,		/* xmlSecKeyDataInitializeMethod initialize; */
    xmlSecSymbianCryptoKeyDataX509Duplicate,		/* xmlSecKeyDataDuplicateMethod duplicate; */
    xmlSecSymbianCryptoKeyDataX509Finalize,		/* xmlSecKeyDataFinalizeMethod finalize; */
    NULL,					/* xmlSecKeyDataGenerateMethod generate; */

    /* get info */
    xmlSecSymbianCryptoKeyDataX509GetType, 		/* xmlSecKeyDataGetTypeMethod getType; */
    NULL,					/* xmlSecKeyDataGetSizeMethod getSize; */
    xmlSecSymbianCryptoKeyDataX509GetIdentifier,	/* xmlSecKeyDataGetIdentifier getIdentifier; */    

    /* read/write */
    xmlSecSymbianCryptoKeyDataX509XmlRead,		/* xmlSecKeyDataXmlReadMethod xmlRead; */
    xmlSecSymbianCryptoKeyDataX509XmlWrite,		/* xmlSecKeyDataXmlWriteMethod xmlWrite; */
    NULL,					/* xmlSecKeyDataBinReadMethod binRead; */
    NULL,					/* xmlSecKeyDataBinWriteMethod binWrite; */

    /* debug */
    xmlSecSymbianCryptoKeyDataX509DebugDump,		/* xmlSecKeyDataDebugDumpMethod debugDump; */
    xmlSecSymbianCryptoKeyDataX509DebugXmlDump, 	/* xmlSecKeyDataDebugDumpMethod debugXmlDump; */

    /* reserved for the future */
    NULL,					/* void* reserved0; */
    NULL,					/* void* reserved1; */
};

/** 
 * xmlSecSymbianCryptoKeyDataX509GetKlass:
 * 
 * The SymbianCrypto X509 key data klass (http://www.w3.org/TR/xmldsig-core/#sec-X509Data).
 *
 * Returns the X509 data klass.
 */
EXPORT_C
xmlSecKeyDataId 
xmlSecSymbianCryptoKeyDataX509GetKlass(void) {
    return(&xmlSecSymbianCryptoKeyDataX509Klass);
}

/**
 * xmlSecSymbianCryptoKeyDataX509GetKeyCert:
 * @data:		the pointer to X509 key data.
 *
 * Gets the certificate from which the key was extracted. 
 *
 * Returns the key's certificate or NULL if key data was not used for key
 * extraction or an error occurs.
 */
EXPORT_C
X509* 	
xmlSecSymbianCryptoKeyDataX509GetKeyCert(xmlSecKeyDataPtr data) {
    xmlSecSymbianCryptoX509DataCtxPtr ctx;
    
    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataX509Id), NULL);

    ctx = xmlSecSymbianCryptoX509DataGetCtx(data);
    xmlSecAssert2(ctx, NULL);

    return(ctx->keyCert);
}

/**
 * xmlSecSymbianCryptoKeyDataX509AdoptKeyCert:
 * @data:		the pointer to X509 key data.
 * @cert:		the pointer to SymbianCrypto X509 certificate.
 *
 * Sets the key's certificate in @data.
 *
 * Returns 0 on success or a negative value if an error occurs.
 */
EXPORT_C
int
xmlSecSymbianCryptoKeyDataX509AdoptKeyCert(xmlSecKeyDataPtr data, X509* cert) {
    xmlSecSymbianCryptoX509DataCtxPtr ctx;

    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataX509Id), -1);
    xmlSecAssert2(cert, -1);

    ctx = xmlSecSymbianCryptoX509DataGetCtx(data);
    xmlSecAssert2(ctx, -1);
    
    if(ctx->keyCert) {
	X509_free(ctx->keyCert);
    }
    ctx->keyCert = cert;
    return(0);
}

/**
 * xmlSecSymbianCryptoKeyDataX509AdoptCert:
 * @data:		the pointer to X509 key data.
 * @cert:		the pointer to SymbianCrypto X509 certificate.
 *
 * Adds certificate to the X509 key data.
 *
 * Returns 0 on success or a negative value if an error occurs.
 */
EXPORT_C
int 
xmlSecSymbianCryptoKeyDataX509AdoptCert(xmlSecKeyDataPtr data, X509* cert) {
    xmlSecSymbianCryptoX509DataCtxPtr ctx;
    
    
    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataX509Id), -1);
    xmlSecAssert2(cert, -1);

    ctx = xmlSecSymbianCryptoX509DataGetCtx(data);
    xmlSecAssert2(ctx, -1);
    /*
    if(ctx->certsList == NULL) {
	ctx->certsList = sk_X509_new_null();
	if(ctx->certsList == NULL) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
			"sk_X509_new_null",
			XMLSEC_ERRORS_R_CRYPTO_FAILED,
			XMLSEC_ERRORS_NO_MESSAGE);
	    return(-1);	
	}
    }
    
    ret = sk_X509_push(ctx->certsList, cert);
    if(ret < 1) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    "sk_X509_push",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);	
    }
    */
    return(0);
}

/**
 * xmlSecSymbianCryptoKeyDataX509GetCert:
 * @data:		the pointer to X509 key data.
 * @pos:		the desired certificate position.
 * 
 * Gets a certificate from X509 key data.
 *
 * Returns the pointer to certificate or NULL if @pos is larger than the 
 * number of certificates in @data or an error occurs.
 */
EXPORT_C
X509* 
xmlSecSymbianCryptoKeyDataX509GetCert(xmlSecKeyDataPtr data, xmlSecSize pos) {
    xmlSecSymbianCryptoX509DataCtxPtr ctx;

    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataX509Id), NULL);

    ctx = xmlSecSymbianCryptoX509DataGetCtx(data);
    xmlSecAssert2(ctx, NULL);
    xmlSecAssert2(ctx->certsList, NULL);

    return NULL;
}

/**
 * xmlSecSymbianCryptoKeyDataX509GetCertsSize:
 * @data:		the pointer to X509 key data.
 *
 * Gets the number of certificates in @data.
 *
 * Returns te number of certificates in @data.
 */
EXPORT_C
xmlSecSize 	
xmlSecSymbianCryptoKeyDataX509GetCertsSize(xmlSecKeyDataPtr data) {
    xmlSecSymbianCryptoX509DataCtxPtr ctx;

    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataX509Id), 0);

    ctx = xmlSecSymbianCryptoX509DataGetCtx(data);
    xmlSecAssert2(ctx, 0);

   return 0;
}

/**
 * xmlSecSymbianCryptoKeyDataX509AdoptCrl:
 * @data:		the pointer to X509 key data.
 * @crl:		the pointer to SymbianCrypto X509 CRL.
 *
 * Adds CRL to the X509 key data.
 *
 * Returns 0 on success or a negative value if an error occurs.
 */
EXPORT_C
int 
xmlSecSymbianCryptoKeyDataX509AdoptCrl(xmlSecKeyDataPtr data, X509_CRL* crl) {
    xmlSecSymbianCryptoX509DataCtxPtr ctx;
   
    
    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataX509Id), -1);
    xmlSecAssert2(crl, -1);

    ctx = xmlSecSymbianCryptoX509DataGetCtx(data);
    xmlSecAssert2(ctx, -1);
    /*
    if(ctx->crlsList == NULL) {
	ctx->crlsList = sk_X509_CRL_new_null();
	if(ctx->crlsList == NULL) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
			"sk_X509_CRL_new_null",
			XMLSEC_ERRORS_R_CRYPTO_FAILED,
			XMLSEC_ERRORS_NO_MESSAGE);
	    return(-1);	
	}
    }
    
    ret = sk_X509_CRL_push(ctx->crlsList, crl);
    if(ret < 1) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    "sk_X509_CRL_push",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);	
    }
    */
    return(0);
}

/**
 * xmlSecSymbianCryptoKeyDataX509GetCrl:
 * @data:		the pointer to X509 key data.
 * @pos:		the desired CRL position.
 * 
 * Gets a CRL from X509 key data.
 *
 * Returns the pointer to CRL or NULL if @pos is larger than the 
 * number of CRLs in @data or an error occurs.
 */
EXPORT_C
X509_CRL* 
xmlSecSymbianCryptoKeyDataX509GetCrl(xmlSecKeyDataPtr data, xmlSecSize pos) {
    xmlSecSymbianCryptoX509DataCtxPtr ctx;

    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataX509Id), NULL);

    ctx = xmlSecSymbianCryptoX509DataGetCtx(data);
    xmlSecAssert2(ctx, NULL);

    xmlSecAssert2(ctx->crlsList, NULL);

   return NULL;
}

/**
 * xmlSecSymbianCryptoKeyDataX509GetCrlsSize:
 * @data:		the pointer to X509 key data.
 *
 * Gets the number of CRLs in @data.
 *
 * Returns te number of CRLs in @data.
 */
EXPORT_C
xmlSecSize 
xmlSecSymbianCryptoKeyDataX509GetCrlsSize(xmlSecKeyDataPtr data) {
    xmlSecSymbianCryptoX509DataCtxPtr ctx;

    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataX509Id), 0);

    ctx = xmlSecSymbianCryptoX509DataGetCtx(data);
    xmlSecAssert2(ctx, 0);

   return 0;
}

static int	
xmlSecSymbianCryptoKeyDataX509Initialize(xmlSecKeyDataPtr data) {
    xmlSecSymbianCryptoX509DataCtxPtr ctx;

    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataX509Id), -1);

    ctx = xmlSecSymbianCryptoX509DataGetCtx(data);
    xmlSecAssert2(ctx, -1);

    memset(ctx, 0, sizeof(xmlSecSymbianCryptoX509DataCtx));
    return(0);
}

static int
xmlSecSymbianCryptoKeyDataX509Duplicate(xmlSecKeyDataPtr dst, xmlSecKeyDataPtr src) {
    X509* certSrc;
    X509* certDst = NULL;
    X509_CRL* crlSrc;
    X509_CRL* crlDst = NULL;
    xmlSecSize size, pos;
    int ret;

    xmlSecAssert2(xmlSecKeyDataCheckId(dst, xmlSecSymbianCryptoKeyDataX509Id), -1);
    xmlSecAssert2(xmlSecKeyDataCheckId(src, xmlSecSymbianCryptoKeyDataX509Id), -1);
    
    /* copy certsList */
    size = xmlSecSymbianCryptoKeyDataX509GetCertsSize(src);
    for(pos = 0; pos < size; ++pos) {
	certSrc = xmlSecSymbianCryptoKeyDataX509GetCert(src, pos);
	certDst = xmlSecSymbianCryptoKeyDataX509GetCert(dst, pos);
	if(!certSrc) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataGetName(src)),
			"xmlSecSymbianCryptoKeyDataX509GetCert",
			XMLSEC_ERRORS_R_XMLSEC_FAILED,
			"pos=%d", pos);
	    return(-1);
	}
	
	if(!certDst) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataGetName(dst)),
			"X509_dup",
			XMLSEC_ERRORS_R_CRYPTO_FAILED,
			XMLSEC_ERRORS_NO_MESSAGE);
	    return(-1);
	}
	
	ret = xmlSecSymbianCryptoKeyDataX509AdoptCert(dst, certDst);
	if(ret < 0) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataGetName(dst)),
			"xmlSecSymbianCryptoKeyDataX509AdoptCert",
			XMLSEC_ERRORS_R_XMLSEC_FAILED,
			XMLSEC_ERRORS_NO_MESSAGE);
	    return(-1);
	}
    }

    /* copy crls */
    size = xmlSecSymbianCryptoKeyDataX509GetCrlsSize(src);
    for(pos = 0; pos < size; ++pos) {
	crlSrc = xmlSecSymbianCryptoKeyDataX509GetCrl(src, pos);
	crlDst =xmlSecSymbianCryptoKeyDataX509GetCrl(dst, pos);
	if(!crlSrc) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataGetName(src)),
			"xmlSecSymbianCryptoKeyDataX509GetCrl",
			XMLSEC_ERRORS_R_XMLSEC_FAILED,
			"pos=%d", pos);
	    return(-1);
	}
	if(!crlDst) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataGetName(dst)),
			"X509_CRL_dup",
			XMLSEC_ERRORS_R_CRYPTO_FAILED,
			XMLSEC_ERRORS_NO_MESSAGE);
	    return(-1);
	}
	
	ret = xmlSecSymbianCryptoKeyDataX509AdoptCrl(dst, crlDst);
	if(ret < 0) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataGetName(dst)),
			"xmlSecSymbianCryptoKeyDataX509AdoptCrl",
			XMLSEC_ERRORS_R_XMLSEC_FAILED,
			XMLSEC_ERRORS_NO_MESSAGE);
	    return(-1);
	}
    }

    /* copy key cert if exist */
    certSrc = xmlSecSymbianCryptoKeyDataX509GetKeyCert(src);
    if(certSrc) {
	if(!certDst) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataGetName(dst)),
			"X509_dup",
			XMLSEC_ERRORS_R_CRYPTO_FAILED,
			XMLSEC_ERRORS_NO_MESSAGE);
	    return(-1);
	}
	ret = xmlSecSymbianCryptoKeyDataX509AdoptKeyCert(dst, certDst);
	if(ret < 0) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataGetName(dst)),
			"xmlSecSymbianCryptoKeyDataX509AdoptKeyCert",
			XMLSEC_ERRORS_R_XMLSEC_FAILED,
			XMLSEC_ERRORS_NO_MESSAGE);
	    return(-1);
	}
    }
    return(0);
}

static void
xmlSecSymbianCryptoKeyDataX509Finalize(xmlSecKeyDataPtr data) {
    xmlSecSymbianCryptoX509DataCtxPtr ctx;

    xmlSecAssert(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataX509Id));

    ctx = xmlSecSymbianCryptoX509DataGetCtx(data);
    xmlSecAssert(ctx);

    if(ctx->certsList) {
	
    }
    if(ctx->crlsList) {

    }
    if(ctx->keyCert) {
	X509_free(ctx->keyCert);
    }
    memset(ctx, 0, sizeof(xmlSecSymbianCryptoX509DataCtx));
}

static int
xmlSecSymbianCryptoKeyDataX509XmlRead(xmlSecKeyDataId id, xmlSecKeyPtr key,
				xmlNodePtr node, xmlSecKeyInfoCtxPtr keyInfoCtx) {
    xmlSecKeyDataPtr data;
    int ret;
    
    xmlSecAssert2(id == xmlSecSymbianCryptoKeyDataX509Id, -1);
    xmlSecAssert2(key, -1);
    xmlSecAssert2(node, -1);
    xmlSecAssert2(keyInfoCtx, -1);
    
    data = xmlSecKeyEnsureData(key, id);
    if(!data) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    "xmlSecKeyEnsureData",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }
    
    ret = xmlSecSymbianCryptoX509DataNodeRead(data, node, keyInfoCtx);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    "xmlSecSymbianCryptoX509DataNodeRead",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }

    if((keyInfoCtx->flags & XMLSEC_KEYINFO_FLAGS_X509DATA_DONT_VERIFY_CERTS) == 0) {
        ret = xmlSecSymbianCryptoKeyDataX509VerifyAndExtractKey(data, key, keyInfoCtx);
	if(ret < 0) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
		        xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
			"xmlSecSymbianCryptoKeyDataX509VerifyAndExtractKey",
		        XMLSEC_ERRORS_R_XMLSEC_FAILED,
			XMLSEC_ERRORS_NO_MESSAGE);
	    return(-1);
        }
    }
    return(0);
}

static int 
xmlSecSymbianCryptoKeyDataX509XmlWrite(xmlSecKeyDataId id, xmlSecKeyPtr key,
				xmlNodePtr node, xmlSecKeyInfoCtxPtr keyInfoCtx) {
    xmlSecKeyDataPtr data;
    X509* cert;
    X509_CRL* crl;
    xmlSecSize size, pos = 0;
    int content;
    int ret;
    			
    xmlSecAssert2(id == xmlSecSymbianCryptoKeyDataX509Id, -1);
    xmlSecAssert2(key, -1);
    xmlSecAssert2(node, -1);
    xmlSecAssert2(keyInfoCtx, -1);

    content = xmlSecX509DataGetNodeContent (node, 1, keyInfoCtx);
    if (content < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    "xmlSecX509DataGetNodeContent",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "content=%d", content);
	return(-1);
    } else if(content == 0) {
	/* by default we are writing certificates and crls */
	content = XMLSEC_X509DATA_DEFAULT;
    }

    /* get x509 data */
    data = xmlSecKeyGetData(key, id);
    if(!data) {
	/* no x509 data in the key */
	return(0);	
    }

    /* write certs */   
    
//    for(pos = 0; pos < size; ++pos) {
	cert = xmlSecSymbianCryptoKeyDataX509GetKeyCert(data);
	if(!cert) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
			"xmlSecSymbianCryptoKeyDataX509GetCert",
			XMLSEC_ERRORS_R_XMLSEC_FAILED,
			"pos=%d", pos);
	    return(-1);
	}
	
	if((content & XMLSEC_X509DATA_CERTIFICATE_NODE) != 0) {
	    ret = xmlSecSymbianCryptoX509CertificateNodeWrite(cert, node, keyInfoCtx);
	    if(ret < 0) {
		xmlSecError(XMLSEC_ERRORS_HERE,
			    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
			    "xmlSecSymbianCryptoX509CertificateNodeWrite",
			    XMLSEC_ERRORS_R_XMLSEC_FAILED,
			    "pos=%d", pos);
		return(-1);
	    }
	}

	if((content & XMLSEC_X509DATA_SUBJECTNAME_NODE) != 0) {
	    ret = xmlSecSymbianCryptoX509SubjectNameNodeWrite(cert, node, keyInfoCtx);
	    if(ret < 0) {
		xmlSecError(XMLSEC_ERRORS_HERE,
			    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
			    "xmlSecSymbianCryptoX509SubjectNameNodeWrite",
			    XMLSEC_ERRORS_R_XMLSEC_FAILED,
			    "pos=%d", pos);
		return(-1);
	    }
	}

	if((content & XMLSEC_X509DATA_ISSUERSERIAL_NODE) != 0) {
	    ret = xmlSecSymbianCryptoX509IssuerSerialNodeWrite(cert, node, keyInfoCtx);
	    if(ret < 0) {
		xmlSecError(XMLSEC_ERRORS_HERE,
			    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
			    "xmlSecSymbianCryptoX509IssuerSerialNodeWrite",
			    XMLSEC_ERRORS_R_XMLSEC_FAILED,
			    "pos=%d", pos);
		return(-1);
	    }
	}

	if((content & XMLSEC_X509DATA_SKI_NODE) != 0) {
	    ret = xmlSecSymbianCryptoX509SKINodeWrite(cert, node, keyInfoCtx);
	    if(ret < 0) {
		xmlSecError(XMLSEC_ERRORS_HERE,
			    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
			    "xmlSecSymbianCryptoX509SKINodeWrite",
			    XMLSEC_ERRORS_R_XMLSEC_FAILED,
			    "pos=%d", pos);
		return(-1);
	    }
	}

//    }    // for(pos = 0; pos < size; ++pos) 
	
    /* write crls if needed */   
    if((content & XMLSEC_X509DATA_CRL_NODE) != 0) {
	size = xmlSecSymbianCryptoKeyDataX509GetCrlsSize(data);
	for(pos = 0; pos < size; ++pos) {
	    crl = xmlSecSymbianCryptoKeyDataX509GetCrl(data, pos);
	    if(!crl) {
		xmlSecError(XMLSEC_ERRORS_HERE,
			    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
			    "xmlSecSymbianCryptoKeyDataX509GetCrl",
			    XMLSEC_ERRORS_R_XMLSEC_FAILED,
			    "pos=%d", pos);
		return(-1);
	    }
	    
	    ret = xmlSecSymbianCryptoX509CRLNodeWrite(crl, node, keyInfoCtx);
	    if(ret < 0) {
		xmlSecError(XMLSEC_ERRORS_HERE,
			    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
			    "xmlSecSymbianCryptoX509CRLNodeWrite",
			    XMLSEC_ERRORS_R_XMLSEC_FAILED,
			    "pos=%d", pos);
		return(-1);
	    }
	}	
    }
   
    return(0);
}


static xmlSecKeyDataType
xmlSecSymbianCryptoKeyDataX509GetType(xmlSecKeyDataPtr data) {
    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataX509Id), 
                                                                   xmlSecKeyDataTypeUnknown);

    return(xmlSecKeyDataTypeUnknown);
}

static const xmlChar*
xmlSecSymbianCryptoKeyDataX509GetIdentifier(xmlSecKeyDataPtr data) {
    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataX509Id), NULL);
    
    return(NULL);
}

static void 
xmlSecSymbianCryptoKeyDataX509DebugDump(xmlSecKeyDataPtr data, FILE* output) {
    X509* cert;
    xmlSecSize size, pos;

    xmlSecAssert(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataX509Id));
    xmlSecAssert(output);

    fprintf(output, "=== X509 Data:\n");
    cert = xmlSecSymbianCryptoKeyDataX509GetKeyCert(data);
    if(cert) {
	fprintf(output, "==== Key Certificate:\n");
	xmlSecSymbianCryptoX509CertDebugDump(cert, output);
    }
    
    size = xmlSecSymbianCryptoKeyDataX509GetCertsSize(data);
    for(pos = 0; pos < size; ++pos) {
	cert = xmlSecSymbianCryptoKeyDataX509GetCert(data, pos);
	if(!cert) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
			"xmlSecSymbianCryptoKeyDataX509GetCert",
			XMLSEC_ERRORS_R_XMLSEC_FAILED,
			"pos=%d", pos);
	    return;
	}
	fprintf(output, "==== Certificate:\n");
	xmlSecSymbianCryptoX509CertDebugDump(cert, output);
    }
    
    /* we don't print out crls */
}

static void
xmlSecSymbianCryptoKeyDataX509DebugXmlDump(xmlSecKeyDataPtr data, FILE* output) {
    X509* cert;
    xmlSecSize size, pos;

    xmlSecAssert(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataX509Id));
    xmlSecAssert(output);

    fprintf(output, "<X509Data>\n");
    cert = xmlSecSymbianCryptoKeyDataX509GetKeyCert(data);
    if(cert) {
	fprintf(output, "<KeyCertificate>\n");
	xmlSecSymbianCryptoX509CertDebugXmlDump(cert, output);
	fprintf(output, "</KeyCertificate>\n");
    }
    
    size = xmlSecSymbianCryptoKeyDataX509GetCertsSize(data);
    for(pos = 0; pos < size; ++pos) {
	cert = xmlSecSymbianCryptoKeyDataX509GetCert(data, pos);
	if(!cert) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
			"xmlSecSymbianCryptoKeyDataX509GetCert",
			XMLSEC_ERRORS_R_XMLSEC_FAILED,
			"pos=%d", pos);
	    return;
	}
	fprintf(output, "<Certificate>\n");
	xmlSecSymbianCryptoX509CertDebugXmlDump(cert, output);
	fprintf(output, "</Certificate>\n");
    }
    
    /* we don't print out crls */
    fprintf(output, "</X509Data>\n");
}

static int
xmlSecSymbianCryptoX509DataNodeRead(xmlSecKeyDataPtr data, 
                                        xmlNodePtr node, 
                                        xmlSecKeyInfoCtxPtr keyInfoCtx) {
    xmlNodePtr cur; 
    int ret;
        
    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataX509Id), -1);
    xmlSecAssert2(node, -1);
    xmlSecAssert2(keyInfoCtx, -1);
    
    for(cur = xmlSecGetNextElementNode(node->children);
	cur;
	cur = xmlSecGetNextElementNode(cur->next)) {
	
	ret = 0;
	if(xmlSecCheckNodeName(cur, xmlSecNodeX509Certificate, xmlSecDSigNs)) {
	    ret = xmlSecSymbianCryptoX509CertificateNodeRead(data, cur, keyInfoCtx);
	} else if(xmlSecCheckNodeName(cur, xmlSecNodeX509SubjectName, xmlSecDSigNs)) {
	    ret = xmlSecSymbianCryptoX509SubjectNameNodeRead(data, cur, keyInfoCtx);
	} else if(xmlSecCheckNodeName(cur, xmlSecNodeX509IssuerSerial, xmlSecDSigNs)) {
	    ret = xmlSecSymbianCryptoX509IssuerSerialNodeRead(data, cur, keyInfoCtx);
	} else if(xmlSecCheckNodeName(cur, xmlSecNodeX509SKI, xmlSecDSigNs)) {
	    ret = xmlSecSymbianCryptoX509SKINodeRead(data, cur, keyInfoCtx);
	} else if(xmlSecCheckNodeName(cur, xmlSecNodeX509CRL, xmlSecDSigNs)) {
	    ret = xmlSecSymbianCryptoX509CRLNodeRead(data, cur, keyInfoCtx);
	} else if((keyInfoCtx->flags & XMLSEC_KEYINFO_FLAGS_X509DATA_STOP_ON_UNKNOWN_CHILD) != 0) {
	    /* laxi schema validation: ignore unknown nodes */
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
			xmlSecErrorsSafeString(xmlSecNodeGetName(cur)),
			XMLSEC_ERRORS_R_UNEXPECTED_NODE,
			XMLSEC_ERRORS_NO_MESSAGE);
	    return(-1);
	}
	if(ret < 0) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
			xmlSecErrorsSafeString(xmlSecNodeGetName(cur)),
			XMLSEC_ERRORS_R_XMLSEC_FAILED,
			"read node failed");
	    return(-1);  
	}	
    }
    return(0);
}

static int
xmlSecSymbianCryptoX509CertificateNodeRead(xmlSecKeyDataPtr data, 
                                    xmlNodePtr node, 
                                    xmlSecKeyInfoCtxPtr keyInfoCtx) {	
    xmlChar *content;
    X509* cert;
    int ret;

    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataX509Id), -1);
    xmlSecAssert2(node, -1);
    xmlSecAssert2(keyInfoCtx, -1);

    content = xmlNodeGetContent(node);
    if((!content) || (xmlSecIsEmptyString(content) == 1)) {
	if(content) {
	    xmlFree(content);
	}
	if ( OOM_FLAG )
	    return(-1);
	if((keyInfoCtx->flags & XMLSEC_KEYINFO_FLAGS_STOP_ON_EMPTY_NODE) != 0) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
			xmlSecErrorsSafeString(xmlSecNodeGetName(node)),
			XMLSEC_ERRORS_R_INVALID_NODE_CONTENT,
			XMLSEC_ERRORS_NO_MESSAGE);
	    return(-1);
	}
	return(0);
    }

    cert = xmlSecSymbianCryptoX509CertBase64DerRead(content);
    if(!cert) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    "xmlSecSymbianCryptoX509CertBase64DerRead",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	xmlFree(content);
	return(-1);
    }    
    
    ret = xmlSecSymbianCryptoKeyDataX509AdoptKeyCert(data, cert);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    "xmlSecSymbianCryptoKeyDataX509AdoptKeyCert",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	X509_free(cert);
	xmlFree(content);
	return(-1);
    }
     
    xmlFree(content);
    return(0);
}

static int 
xmlSecSymbianCryptoX509CertificateNodeWrite(X509* cert, 
                            xmlNodePtr node, 
                            xmlSecKeyInfoCtxPtr keyInfoCtx) {
    xmlChar* buf;
    xmlNodePtr cur;
    
    xmlSecAssert2(cert, -1);
    xmlSecAssert2(node, -1);
    xmlSecAssert2(keyInfoCtx, -1);
    
    /* set base64 lines size from context */
    buf = xmlSecSymbianCryptoX509CertBase64DerWrite(cert, keyInfoCtx->base64LineSize); 
    if(!buf) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecSymbianCryptoX509CertBase64DerWrite",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }
	
    cur = xmlSecAddChild(node, xmlSecNodeX509Certificate, xmlSecDSigNs);
    if(!cur) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecAddChild",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "node=%s",
		    xmlSecErrorsSafeString(xmlSecNodeX509Certificate));
	xmlFree(buf);
	return(-1);	
    }

    xmlNodeSetContent(cur, xmlSecStringCR);
    if ( OOM_FLAG )
        {
        xmlUnlinkNode(cur);
	    xmlFreeNode(cur);
        xmlFree(buf);
        return(-1);
        }
    xmlNodeSetContent(cur, buf);
    if ( OOM_FLAG )
        {
        xmlUnlinkNode(cur);
	    xmlFreeNode(cur);
        xmlFree(buf);
        return(-1);
        }
    xmlFree(buf);
    return(0);
}

static int		
xmlSecSymbianCryptoX509SubjectNameNodeRead(xmlSecKeyDataPtr data, 
                            xmlNodePtr node, 
                            xmlSecKeyInfoCtxPtr keyInfoCtx) {	
    xmlSecKeyDataStorePtr x509Store;
    xmlChar* subject;
    X509* cert;
    //X509* cert2 = NULL;
    
    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataX509Id), -1);
    xmlSecAssert2(node, -1);
    xmlSecAssert2(keyInfoCtx, -1);
    xmlSecAssert2(keyInfoCtx->keysMngr, -1);

    x509Store = xmlSecKeysMngrGetDataStore(keyInfoCtx->keysMngr, xmlSecSymbianCryptoX509StoreId);
    if(!x509Store) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    "xmlSecKeysMngrGetDataStore",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }

    subject = xmlNodeGetContent(node);
    if((!subject) || (xmlSecIsEmptyString(subject) == 1)) {
	if(subject) {
	    xmlFree(subject);
	}
	if((keyInfoCtx->flags & XMLSEC_KEYINFO_FLAGS_STOP_ON_EMPTY_NODE) != 0) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
		        xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
			xmlSecErrorsSafeString(xmlSecNodeGetName(node)),
			XMLSEC_ERRORS_R_INVALID_NODE_CONTENT,
			XMLSEC_ERRORS_NO_MESSAGE);
	    return(-1);
	}
	return(0);
    }

    cert = xmlSecSymbianCryptoX509StoreFindCert(x509Store, subject, NULL, NULL, NULL, keyInfoCtx);
    if(!cert){
	if((keyInfoCtx->flags & XMLSEC_KEYINFO_FLAGS_X509DATA_STOP_ON_UNKNOWN_CERT) != 0) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
		        xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
			NULL,
			XMLSEC_ERRORS_R_CERT_NOT_FOUND,
			"subject=%s", 
			xmlSecErrorsSafeString(subject));
	    xmlFree(subject);
	    return(-1);
	}
	xmlFree(subject);
	return(0);
    }
    
    //dead error condition
    /*
    if(!cert2) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    "X509_dup",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	xmlFree(subject);
	return(-1);
    }
    
    ret = xmlSecSymbianCryptoKeyDataX509AdoptCert(data, cert2);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    "xmlSecSymbianCryptoKeyDataX509AdoptCert",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	xmlFree(subject);
	return(-1);
    }
    */
    xmlFree(subject);
    return(0);
}

static int
xmlSecSymbianCryptoX509SubjectNameNodeWrite(X509* cert, 
                                xmlNodePtr node, 
                                xmlSecKeyInfoCtxPtr keyInfoCtx ATTRIBUTE_UNUSED) {
    xmlChar* buf = NULL;
    xmlNodePtr cur = NULL;

    xmlSecAssert2(cert, -1);
    xmlSecAssert2(node, -1);

    if(!buf) {
	xmlSecError(XMLSEC_ERRORS_HERE,
	    NULL,
	    "xmlSecSymbianCryptoX509NameWrite(X509_get_subject_name)",
	    XMLSEC_ERRORS_R_XMLSEC_FAILED,
	    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }

    cur = xmlSecAddChild(node, xmlSecNodeX509SubjectName, xmlSecDSigNs);
    if(!cur) {
	xmlSecError(XMLSEC_ERRORS_HERE,
	    NULL,
	    "xmlSecAddChild",
	    XMLSEC_ERRORS_R_XMLSEC_FAILED,
	    "node=%s",
	    xmlSecErrorsSafeString(xmlSecNodeX509SubjectName));
	xmlFree(buf);
	return(-1);
    }
    xmlNodeSetContent(cur, buf);
    xmlFree(buf);
    return(0);
}

static int 
xmlSecSymbianCryptoX509IssuerSerialNodeRead(xmlSecKeyDataPtr data, 
                                xmlNodePtr node, 
                                xmlSecKeyInfoCtxPtr keyInfoCtx) {
    xmlSecKeyDataStorePtr x509Store;
    xmlNodePtr cur;
    xmlChar *issuerName;
    xmlChar *issuerSerial;    
    X509* cert;
    //X509* cert2 = NULL;

    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataX509Id), -1);
    xmlSecAssert2(node, -1);
    xmlSecAssert2(keyInfoCtx, -1);
    xmlSecAssert2(keyInfoCtx->keysMngr, -1);

    x509Store = xmlSecKeysMngrGetDataStore(keyInfoCtx->keysMngr, xmlSecSymbianCryptoX509StoreId);
    if(!x509Store) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    "xmlSecKeysMngrGetDataStore",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }

    cur = xmlSecGetNextElementNode(node->children);
    if(!cur) {
	if((keyInfoCtx->flags & XMLSEC_KEYINFO_FLAGS_STOP_ON_EMPTY_NODE) != 0) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
			xmlSecErrorsSafeString(xmlSecNodeX509IssuerName),
			XMLSEC_ERRORS_R_NODE_NOT_FOUND,
			"node=%s",
			xmlSecErrorsSafeString(xmlSecNodeGetName(cur)));
	    return(-1);
	}
	return(0);
    }
    
    /* the first is required node X509IssuerName */
    if(!xmlSecCheckNodeName(cur, xmlSecNodeX509IssuerName, xmlSecDSigNs)) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    xmlSecErrorsSafeString(xmlSecNodeX509IssuerName),
		    XMLSEC_ERRORS_R_NODE_NOT_FOUND,
		    "node=%s",
		    xmlSecErrorsSafeString(xmlSecNodeGetName(cur)));
	return(-1);
    }    
    issuerName = xmlNodeGetContent(cur);
    if(!issuerName) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    xmlSecErrorsSafeString(xmlSecNodeGetName(cur)),
		    XMLSEC_ERRORS_R_INVALID_NODE_CONTENT,
		    "node=%s",
		    xmlSecErrorsSafeString(xmlSecNodeX509IssuerName));
	return(-1);
    }
    cur = xmlSecGetNextElementNode(cur->next); 

    /* next is required node X509SerialNumber */
    if((!cur) || !xmlSecCheckNodeName(cur, xmlSecNodeX509SerialNumber, xmlSecDSigNs)) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    xmlSecErrorsSafeString(xmlSecNodeGetName(cur)),
		    XMLSEC_ERRORS_R_NODE_NOT_FOUND,
		    "node=%s",
		    xmlSecErrorsSafeString(xmlSecNodeX509SerialNumber));
	xmlFree(issuerName);
	return(-1);
    }    
    issuerSerial = xmlNodeGetContent(cur);
    if(!issuerSerial) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    xmlSecErrorsSafeString(xmlSecNodeX509SerialNumber),
		    XMLSEC_ERRORS_R_INVALID_NODE_CONTENT,
		    "node=%s",
		    xmlSecErrorsSafeString(xmlSecNodeGetName(cur)));
	xmlFree(issuerName);
	return(-1);
    }
    cur = xmlSecGetNextElementNode(cur->next); 

    if(cur) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    xmlSecErrorsSafeString(xmlSecNodeGetName(cur)),
		    XMLSEC_ERRORS_R_UNEXPECTED_NODE,
		    XMLSEC_ERRORS_NO_MESSAGE);
	xmlFree(issuerSerial);
	xmlFree(issuerName);
	return(-1);
    }

    cert = xmlSecSymbianCryptoX509StoreFindCert(x509Store, 
                                                            NULL, 
                                                            issuerName, 
                                                            issuerSerial, 
                                                            NULL, 
                                                            keyInfoCtx);
    if(!cert)
        {
        if((keyInfoCtx->flags & XMLSEC_KEYINFO_FLAGS_X509DATA_STOP_ON_UNKNOWN_CERT) != 0)
            {
            xmlSecError(XMLSEC_ERRORS_HERE,
		        xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
			NULL,
			XMLSEC_ERRORS_R_CERT_NOT_FOUND,
			"issuerName=%s;issuerSerial=%s",
		        xmlSecErrorsSafeString(issuerName), 
			xmlSecErrorsSafeString(issuerSerial));
            xmlFree(issuerSerial);
            xmlFree(issuerName);
            return(-1);
            }
		xmlFree(issuerSerial);
        xmlFree(issuerName);
        return(0);  
        }

    /*
    if(!cert2) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    "X509_dup",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	xmlFree(issuerSerial);
	xmlFree(issuerName);
	return(-1);
    }

    ret = xmlSecSymbianCryptoKeyDataX509AdoptCert(data, cert2);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    "xmlSecSymbianCryptoKeyDataX509AdoptCert",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	xmlFree(issuerSerial);
	xmlFree(issuerName);
	return(-1);
    }
    */
    xmlFree(issuerSerial);
    xmlFree(issuerName);
    return(0);
}

static int
xmlSecSymbianCryptoX509IssuerSerialNodeWrite(X509* cert, 
                                    xmlNodePtr node, 
                                    xmlSecKeyInfoCtxPtr keyInfoCtx ATTRIBUTE_UNUSED) {
    xmlNodePtr cur;
    xmlNodePtr issuerNameNode;
    xmlNodePtr issuerNumberNode;
    xmlChar* buf = NULL;
    
    xmlSecAssert2(cert, -1);
    xmlSecAssert2(node, -1);

    /* create xml nodes */
    cur = xmlSecAddChild(node, xmlSecNodeX509IssuerSerial, xmlSecDSigNs);
    if(!cur) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecAddChild",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "node=%s",
		    xmlSecErrorsSafeString(xmlSecNodeX509IssuerSerial));
	return(-1);
    }

    issuerNameNode = xmlSecAddChild(cur, xmlSecNodeX509IssuerName, xmlSecDSigNs);
    if(!issuerNameNode) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecAddChild",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "node=%s",
		    xmlSecErrorsSafeString(xmlSecNodeX509IssuerName));
	return(-1);
    }

    issuerNumberNode = xmlSecAddChild(cur, xmlSecNodeX509SerialNumber, xmlSecDSigNs);
    if(!issuerNumberNode) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecAddChild",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "node=%s",
		    xmlSecErrorsSafeString(xmlSecNodeX509SerialNumber));
	return(-1);
    }
    /*
    //write data 
    if(!buf) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecSymbianCryptoX509NameWrite(X509_get_issuer_name)",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }
        
    if(!buf) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecSymbianCryptoASN1IntegerWrite(X509_get_serialNumber)",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }
	*/
	xmlNodeSetContent(issuerNameNode, buf);
    xmlNodeSetContent(issuerNumberNode, buf);
    xmlFree(buf);

    return(0);
}


static int 
xmlSecSymbianCryptoX509SKINodeRead(xmlSecKeyDataPtr data, 
                                    xmlNodePtr node, 
                                    xmlSecKeyInfoCtxPtr keyInfoCtx) {
    xmlSecKeyDataStorePtr x509Store;
    xmlChar* ski;
    X509* cert;
    
    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataX509Id), -1);
    xmlSecAssert2(node, -1);
    xmlSecAssert2(keyInfoCtx, -1);
    xmlSecAssert2(keyInfoCtx->keysMngr, -1);

    x509Store = xmlSecKeysMngrGetDataStore(keyInfoCtx->keysMngr, xmlSecSymbianCryptoX509StoreId);
    if(!x509Store) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    "xmlSecKeysMngrGetDataStore",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }
    
    ski = xmlNodeGetContent(node);
    if((!ski) || (xmlSecIsEmptyString(ski) == 1)) {
	if(ski) {
	    xmlFree(ski);
	}
	if((keyInfoCtx->flags & XMLSEC_KEYINFO_FLAGS_STOP_ON_EMPTY_NODE) != 0) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
			xmlSecErrorsSafeString(xmlSecNodeGetName(node)),
			XMLSEC_ERRORS_R_INVALID_NODE_CONTENT,
			"node=%s",
			xmlSecErrorsSafeString(xmlSecNodeX509SKI));
	    return(-1);
	}
	return(0);
    }

    cert = xmlSecSymbianCryptoX509StoreFindCert(x509Store, NULL, NULL, NULL, ski, keyInfoCtx);
    if(!cert){
	if((keyInfoCtx->flags & XMLSEC_KEYINFO_FLAGS_X509DATA_STOP_ON_UNKNOWN_CERT) != 0) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
		        xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
			NULL,
		        XMLSEC_ERRORS_R_CERT_NOT_FOUND,
			"ski=%s", 
			xmlSecErrorsSafeString(ski));
	    xmlFree(ski);
	    return(-1);
	}
	xmlFree(ski);
	return(0);
    }
 
//dead error condition 
/*
    if(!cert2) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    "X509_dup",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	xmlFree(ski);
	return(-1);
    }
 
    ret = xmlSecSymbianCryptoKeyDataX509AdoptCert(data, cert2);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    "xmlSecSymbianCryptoKeyDataX509AdoptCert",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	xmlFree(ski);
	return(-1);
    }
    */
    
    xmlFree(ski);
    return(0);
}

static int
xmlSecSymbianCryptoX509SKINodeWrite(X509* cert, 
                                    xmlNodePtr node, 
                                    xmlSecKeyInfoCtxPtr keyInfoCtx ATTRIBUTE_UNUSED) {
    xmlChar *buf = NULL;
    xmlNodePtr cur = NULL;

    xmlSecAssert2(cert, -1);
    xmlSecAssert2(node, -1);

    buf = xmlSecSymbianCryptoX509SKIWrite(cert);
    if(!buf) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecSymbianCryptoX509SKIWrite",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }

    cur = xmlSecAddChild(node, xmlSecNodeX509SKI, xmlSecDSigNs);
    if(!cur) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecAddChild",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "new_node=%s",
		    xmlSecErrorsSafeString(xmlSecNodeX509SKI));
	xmlFree(buf);
	return(-1);
    }
    xmlNodeSetContent(cur, buf);
    xmlFree(buf);

    return(0);
}

static int 
xmlSecSymbianCryptoX509CRLNodeRead(xmlSecKeyDataPtr data, 
                                xmlNodePtr node, 
                                xmlSecKeyInfoCtxPtr keyInfoCtx) {
    xmlChar *content;
    X509_CRL* crl;
    int ret;

    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataX509Id), -1);
    xmlSecAssert2(node, -1);
    xmlSecAssert2(keyInfoCtx, -1);

    content = xmlNodeGetContent(node);
    if((!content) || (xmlSecIsEmptyString(content) == 1)) {
	if(content) {
	    xmlFree(content);
	}
	if((keyInfoCtx->flags & XMLSEC_KEYINFO_FLAGS_STOP_ON_EMPTY_NODE) != 0) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
			xmlSecErrorsSafeString(xmlSecNodeGetName(node)),
			XMLSEC_ERRORS_R_INVALID_NODE_CONTENT,
			XMLSEC_ERRORS_NO_MESSAGE);
	    return(-1);
	}
	return(0);
    }

    crl = xmlSecSymbianCryptoX509CrlBase64DerRead(content);
    if(!crl) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    "xmlSecSymbianCryptoX509CrlBase64DerRead",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	xmlFree(content);
	return(-1);
    }    
    
    ret = xmlSecSymbianCryptoKeyDataX509AdoptCrl(data, crl);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    "xmlSecSymbianCryptoKeyDataX509AdoptCrl",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	X509_crl_free(crl);
	xmlFree(content);
	return(-1);
    }
     
    xmlFree(content);
    return(0);
}

static int
xmlSecSymbianCryptoX509CRLNodeWrite(X509_CRL* crl, 
                                        xmlNodePtr node, 
                                        xmlSecKeyInfoCtxPtr keyInfoCtx) {
    xmlChar* buf = NULL;
    xmlNodePtr cur = NULL;

    xmlSecAssert2(crl, -1);
    xmlSecAssert2(node, -1);
    xmlSecAssert2(keyInfoCtx, -1);

    /* set base64 lines size from context */
    buf = xmlSecSymbianCryptoX509CrlBase64DerWrite(crl, keyInfoCtx->base64LineSize); 
    if(!buf) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecSymbianCryptoX509CrlBase64DerWrite",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }

    cur = xmlSecAddChild(node, xmlSecNodeX509CRL, xmlSecDSigNs);
    if(!cur) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecAddChild",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    "new_node=%s",
		    xmlSecErrorsSafeString(xmlSecNodeX509CRL));
	xmlFree(buf);
	return(-1);
    }
    xmlNodeSetContent(cur, xmlSecStringCR);
    xmlNodeSetContent(cur, buf);
    xmlFree(buf);

    return(0);
}

static int
xmlSecSymbianCryptoKeyDataX509VerifyAndExtractKey(xmlSecKeyDataPtr data, xmlSecKeyPtr key,
				    xmlSecKeyInfoCtxPtr keyInfoCtx) {
    xmlSecSymbianCryptoX509DataCtxPtr ctx;
    xmlSecKeyDataStorePtr x509Store;
    int ret;
    
    xmlSecAssert2(xmlSecKeyDataCheckId(data, xmlSecSymbianCryptoKeyDataX509Id), -1);
    xmlSecAssert2(key, -1);
    xmlSecAssert2(keyInfoCtx, -1);
    xmlSecAssert2(keyInfoCtx->keysMngr, -1);

    ctx = xmlSecSymbianCryptoX509DataGetCtx(data);
    xmlSecAssert2(ctx, -1);

    x509Store = xmlSecKeysMngrGetDataStore(keyInfoCtx->keysMngr, xmlSecSymbianCryptoX509StoreId);
    if(!x509Store) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
		    "xmlSecKeysMngrGetDataStore",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }

	// do not use list first
    //if((ctx->keyCert == NULL) && (ctx->certsList != NULL) && (xmlSecKeyGetValue(key) == NULL)) {
    if((ctx->keyCert) && (!xmlSecKeyGetValue(key)) ) {
	
	
	ret = xmlSecSymbianCryptoX509StoreKeyCertVerify(x509Store, ctx->keyCert);	
	if(ret == 0) {
	    xmlSecKeyDataPtr keyValue;
	    /*
	    ctx->keyCert = X509_dup(cert);
	    if(ctx->keyCert == NULL) {
		xmlSecError(XMLSEC_ERRORS_HERE,
			    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
			    "X509_dup",
			    XMLSEC_ERRORS_R_CRYPTO_FAILED,
			    XMLSEC_ERRORS_NO_MESSAGE);
		return(-1);
	    }
		*/
	    keyValue = xmlSecSymbianCryptoX509CertGetKey(ctx->keyCert);
	    if(!keyValue) {
		xmlSecError(XMLSEC_ERRORS_HERE,
			    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
			    "xmlSecSymbianCryptoX509CertGetKey",
			    XMLSEC_ERRORS_R_XMLSEC_FAILED,
			    XMLSEC_ERRORS_NO_MESSAGE);
		return(-1);
	    }
	    
	    /* verify that the key matches our expectations */
	    if(xmlSecKeyReqMatchKeyValue(&(keyInfoCtx->keyReq), keyValue) != 1) {
		xmlSecError(XMLSEC_ERRORS_HERE,
			    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
			    "xmlSecKeyReqMatchKeyValue",
			    XMLSEC_ERRORS_R_XMLSEC_FAILED,
			    XMLSEC_ERRORS_NO_MESSAGE);
		xmlSecKeyDataDestroy(keyValue);
		return(-1);
	    }	
	        
	    ret = xmlSecKeySetValue(key, keyValue);
    	    if(ret < 0) {
		xmlSecError(XMLSEC_ERRORS_HERE,
			    xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
			    "xmlSecKeySetValue",
			    XMLSEC_ERRORS_R_XMLSEC_FAILED,
			    XMLSEC_ERRORS_NO_MESSAGE);
		xmlSecKeyDataDestroy(keyValue);
		return(-1);
	    }	    
	    /*
	    if((X509_get_notBefore(ctx->keyCert) != NULL) && (X509_get_notAfter(ctx->keyCert) != NULL)) {
		ret = xmlSecSymbianCryptoX509CertGetTime(X509_get_notBefore(ctx->keyCert), &(key->notValidBefore));
		if(ret < 0) {
		    xmlSecError(XMLSEC_ERRORS_HERE,
			        xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
				"xmlSecSymbianCryptoX509CertGetTime",
			        XMLSEC_ERRORS_R_XMLSEC_FAILED,
				"notValidBefore");
		    return(-1);
		}
		
		ret = xmlSecSymbianCryptoX509CertGetTime(X509_get_notAfter(ctx->keyCert), &(key->notValidAfter));
		if(ret < 0) {
		    xmlSecError(XMLSEC_ERRORS_HERE,
			        xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
				"xmlSecSymbianCryptoX509CertGetTime",
			        XMLSEC_ERRORS_R_XMLSEC_FAILED,
				"notValidAfter");
		    return(-1);
		}
	    } else {
		key->notValidBefore = key->notValidAfter = 0;
	    }
	    */
	    ret = X509_test_validityPeriod(ctx->keyCert);
	    if(ret != 1) { 
		    xmlSecError(XMLSEC_ERRORS_HERE,
			        xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
				"xmlSecSymbianCryptoX509CertGetTime",
			        XMLSEC_ERRORS_R_XMLSEC_FAILED,
				"x509_test_validityPeriod");
		    return(-1);
		}
		else 
		{
			key->notValidBefore = key->notValidAfter = 0;
		}
	    
	    
	} else if((keyInfoCtx->flags & XMLSEC_KEYINFO_FLAGS_X509DATA_STOP_ON_INVALID_CERT) != 0) {
	    xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
			NULL,
			XMLSEC_ERRORS_R_CERT_NOT_FOUND,
			XMLSEC_ERRORS_NO_MESSAGE);
	    return(-1);
	}
	else if (ret != 0)
	{
	     xmlSecError(XMLSEC_ERRORS_HERE,
			xmlSecErrorsSafeString(xmlSecKeyDataGetName(data)),
			NULL,
			XMLSEC_ERRORS_R_CERT_NOT_FOUND,
			XMLSEC_ERRORS_NO_MESSAGE);
	    return(-1);
	}
    }
    return(0);
}

#ifdef HAVE_TIMEGM
extern time_t timegm (struct tm *tm);
#else  /* HAVE_TIMEGM */
#ifdef WIN32
#define timegm(tm)	(mktime(tm) - _timezone)
#else /* WIN32 */
/* Absolutely not the best way but it's the only ANSI compatible way I know.
 * If you system has a native struct tm --> GMT time_t conversion function
 * (like timegm) use it instead.
 */
 /*
static time_t 
my_timegm(struct tm *t) {  
    time_t tl, tb;  
    struct tm *tg;  

    tl = mktime (t);  
    if(tl == -1) {
	t->tm_hour--;
	tl = mktime (t);
	if (tl == -1) {
	    return -1;
	}
	tl += 3600;    
    }  
    tg = gmtime (&tl);  
    tg->tm_isdst = 0;  
    tb = mktime (tg);  
    if (tb == -1) {
	tg->tm_hour--;
	tb = mktime (tg);
	if (tb == -1) {
	    return -1;
	}
	tb += 3600;    
    }  
    return (tl - (tb - tl)); 
}

#define timegm(tm) my_timegm(tm)
*/
#endif /* WIN32 */
#endif /* HAVE_TIMEGM */

#ifdef XMLSEC_FUTURE_SUPPORT
static int
xmlSecSymbianCryptoX509CertGetTime(ASN1_TIME* t, time_t* res) {
    struct tm tm;
    int offset;
    
    xmlSecAssert2(t, -1);
    xmlSecAssert2(res, -1);

    (*res) = 0;
#ifndef XMLSEC_OPENSSL_096
    if(!ASN1_TIME_check(t)) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "ASN1_TIME_check",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }
#endif /* XMLSEC_OPENSSL_096 */
        
    memset(&tm, 0, sizeof(tm));

#define g2(p) (((p)[0]-'0')*10+(p)[1]-'0')
    if(t->type == V_ASN1_UTCTIME) {
	xmlSecAssert2(t->length > 12, -1);

	
	/* this code is copied from OpenSSL asn1/a_utctm.c file */	
	tm.tm_year = g2(t->data);
	if(tm.tm_year < 50) {
	    tm.tm_year += 100;
	}
	tm.tm_mon  = g2(t->data + 2) - 1;
	tm.tm_mday = g2(t->data + 4);
	tm.tm_hour = g2(t->data + 6);
	tm.tm_min  = g2(t->data + 8);
	tm.tm_sec  = g2(t->data + 10);
	if(t->data[12] == 'Z') {
	    offset = 0;
	} else {
	    xmlSecAssert2(t->length > 16, -1);
	    
	    offset = g2(t->data + 13) * 60 + g2(t->data + 15);
	    if(t->data[12] == '-') {
		offset = -offset;
	    }
	}
	tm.tm_isdst = -1;
    } else {
	xmlSecAssert2(t->length > 14, -1);
	
	tm.tm_year = g2(t->data) * 100 + g2(t->data + 2);
	tm.tm_mon  = g2(t->data + 4) - 1;
	tm.tm_mday = g2(t->data + 6);
	tm.tm_hour = g2(t->data + 8);
	tm.tm_min  = g2(t->data + 10);
	tm.tm_sec  = g2(t->data + 12);
	if(t->data[14] == 'Z') {
	    offset = 0;
	} else {
	    xmlSecAssert2(t->length > 18, -1);
	    
	    offset = g2(t->data + 15) * 60 + g2(t->data + 17);
	    if(t->data[14] == '-') {
		offset = -offset;
	    }
	}
	tm.tm_isdst = -1;
    }
#undef g2
    (*res) = timegm(&tm) - offset * 60;
    return(0);
}

#endif	//XMLSEC_FUTURE_SUPPORT
/** 
 * xmlSecSymbianCryptoX509CertGetKey:
 * @cert:		the certificate.
 * 
 * Extracts public key from the @cert.
 *
 * Returns public key value or NULL if an error occurs.
 */
EXPORT_C
xmlSecKeyDataPtr	
xmlSecSymbianCryptoX509CertGetKey(X509* cert) {
    xmlSecKeyDataPtr data;
    EVP_PKEY *pKey = NULL;
    
    xmlSecAssert2(cert, NULL);

    pKey = X509_get_pubkey(cert);
    if(!pKey) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "X509_get_pubkey",
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
    
    return(data);
}

static X509*
xmlSecSymbianCryptoX509CertBase64DerRead(xmlChar* buf) {
    int ret;

    xmlSecAssert2(buf, NULL);
    
    /* usual trick with base64 decoding "in-place" */
    ret = xmlSecBase64Decode(buf, (xmlSecByte*)buf, xmlStrlen(buf)); 
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecBase64Decode",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(NULL);
    }
    
    return(xmlSecSymbianCryptoX509CertDerRead((xmlSecByte*)buf, ret));
}

static X509*
xmlSecSymbianCryptoX509CertDerRead(const xmlSecByte* buf, xmlSecSize size) {
    X509 *cert = NULL;
    BIO *mem = NULL;
    int ret;

    xmlSecAssert2(buf, NULL);
    xmlSecAssert2(size > 0, NULL);
    
    mem = BIO_new();
    if(!mem) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "BIO_new",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    "BIO_s_mem");
	return(NULL);
    }
    
    ret = BIO_write(mem, buf, size);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "BIO_write",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    "size=%d", size);
	BIO_free(mem);
	return(NULL);
    }

    cert = d2i_X509_bio(mem);
    if(!cert) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "d2i_X509_bio",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	BIO_free(mem);
	return(NULL);
    }

    BIO_free(mem);

    return(cert);
}

static xmlChar*
xmlSecSymbianCryptoX509CertBase64DerWrite(X509* cert, int base64LineWrap) {
    xmlChar *res = NULL;
    
   

    xmlSecAssert2(cert, NULL);
    
    res = xmlSecBase64Encode((const xmlSecByte*)cert->der, cert->derlen, base64LineWrap);
    if(!res) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecBase64Encode",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(NULL);
    }    
        
    return(res);
}

static X509_CRL*
xmlSecSymbianCryptoX509CrlBase64DerRead(xmlChar* buf) {
    int ret;

    xmlSecAssert2(buf, NULL);
    
    /* usual trick with base64 decoding "in-place" */
    ret = xmlSecBase64Decode(buf, (xmlSecByte*)buf, xmlStrlen(buf)); 
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecBase64Decode",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(NULL);
    }
    
    return(xmlSecSymbianCryptoX509CrlDerRead((xmlSecByte*)buf, ret));
}

static X509_CRL*
xmlSecSymbianCryptoX509CrlDerRead(xmlSecByte* buf, xmlSecSize size) {
    X509_CRL *crl = NULL;
   
    xmlSecAssert2(buf, NULL);
    xmlSecAssert2(size > 0, NULL);
   
    return(crl);
}

static xmlChar*
xmlSecSymbianCryptoX509CrlBase64DerWrite(X509_CRL* crl, int base64LineWrap) {
    xmlChar *res = NULL;
    
    

    xmlSecAssert2(crl, NULL);
#ifdef XMLSEC_FUTURE_SUPPORT	
    mem = BIO_new(BIO_s_mem());
    if(!mem) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "BIO_new",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    "BIO_s_mem");
	return(NULL);
    }

    i2d_X509_CRL_bio(mem, crl);
    BIO_flush(mem);
        
    size = BIO_get_mem_data(mem, &p);
    if((size <= 0) || (!p)){
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "BIO_get_mem_data",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	BIO_free_all(mem);
	return(NULL);
    }
    
    res = xmlSecBase64Encode(p, size, base64LineWrap);
    if(!res) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecBase64Encode",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	BIO_free_all(mem);
	return(NULL);
    }    

    BIO_free_all(mem);    
    
#endif    
    return(res);
}
/*
static xmlChar*
xmlSecSymbianCryptoX509NameWrite(X509_NAME* nm) {
    xmlChar *res = NULL;
    long size;

    xmlSecAssert2(nm, NULL);
#ifdef XMLSEC_FUTURE_SUPPORT
    mem = BIO_new(BIO_s_mem());
    if(!mem) {
        xmlSecError(XMLSEC_ERRORS_HERE,
        	    NULL,
        	    "BIO_new",
        	    XMLSEC_ERRORS_R_CRYPTO_FAILED,
        	    "BIO_s_mem");
        return(NULL);
    }

    if (X509_NAME_print_ex(mem, nm, 0, XN_FLAG_RFC2253) <=0) {
        xmlSecError(XMLSEC_ERRORS_HERE,
        	    NULL,
        	    "X509_NAME_print_ex",
        	    XMLSEC_ERRORS_R_CRYPTO_FAILED,
        	    XMLSEC_ERRORS_NO_MESSAGE);
        BIO_free_all(mem);
        return(NULL);
    }

    BIO_flush(mem);

    size = BIO_pending(mem);
    res = xmlMalloc(size + 1);
    if(!res) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlMalloc",
		    XMLSEC_ERRORS_R_MALLOC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	BIO_free_all(mem);
	return(NULL);
    }

    size = BIO_read(mem, res, size);
    res[size] = '\0';

    BIO_free_all(mem);
#endif    
    return(res);
}
*/
#ifdef XMLSEC_FUTURE_SUPPORT
static xmlChar*
xmlSecSymbianCryptoASN1IntegerWrite(ASN1_INTEGER *asni) {
    xmlChar *res = NULL;
    BIGNUM *bn;
    char *p;
    
    xmlSecAssert2(asni, NULL);
    bn = ASN1_INTEGER_to_BN(asni, NULL);
    if(!bn) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "ASN1_INTEGER_to_BN",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(NULL);
    }

    p = BN_bn2dec(bn);
    if (!p) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "BN_bn2dec",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	BN_free(bn);
	return(NULL);
    }
    BN_free(bn);
    bn = NULL;

    /* SymbianCrypto and LibXML2 can have different memory callbacks, i.e.
       when data is allocated in SymbianCrypto should be freed with SymbianCrypto
       method, not with LibXML2 method.
     */
    res = xmlCharStrdup(p);
    if(!res) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlCharStrdup",
		    XMLSEC_ERRORS_R_MALLOC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	OPENSSL_free(p);
	return(NULL);
    }
    OPENSSL_free(p);
    p = NULL;   
    return(res);
}
#endif	//XMLSEC_FUTURE_SUPPORT 

static xmlChar*
xmlSecSymbianCryptoX509SKIWrite(X509* cert) {
    xmlChar *res = NULL;
   
#ifdef XMLSEC_FUTURE_SUPPORT    
    X509_EXTENSION *ext;
    ASN1_OCTET_STRING *keyId;

    xmlSecAssert2(cert, NULL);

    index = X509_get_ext_by_NID(cert, NID_subject_key_identifier, -1);
    if (index < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "Certificate without SubjectKeyIdentifier extension",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(NULL);
    }
    
    ext = X509_get_ext(cert, index);
    if (!ext) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "X509_get_ext",
		    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(NULL);
    }

    keyId = X509V3_EXT_d2i(ext);
    if (!keyId) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "X509V3_EXT_d2i",
	    	    XMLSEC_ERRORS_R_CRYPTO_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	M_ASN1_OCTET_STRING_free(keyId);
	return(NULL);
    }

    res = xmlSecBase64Encode(M_ASN1_STRING_data(keyId), M_ASN1_STRING_length(keyId), 0);
    if(!res) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecBase64Encode",
	    	    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	M_ASN1_OCTET_STRING_free(keyId);
	return(NULL);
    }
    M_ASN1_OCTET_STRING_free(keyId);
#endif 		//XMLSEC_FUTURE_SUPPORT    
    return(res);
}

static void 
xmlSecSymbianCryptoX509CertDebugDump(X509* cert, FILE* output) {
    
#ifdef XMLSEC_FUTURE_SUPPORT    
    BIGNUM *bn = NULL;

    xmlSecAssert(cert);
    xmlSecAssert(output);

    fprintf(output, "==== Subject Name: %s\n", 
	X509_NAME_oneline(X509_get_subject_name(cert), buf, sizeof(buf))); 
    fprintf(output, "==== Issuer Name: %s\n", 
	X509_NAME_oneline(X509_get_issuer_name(cert), buf, sizeof(buf))); 
    fprintf(output, "==== Issuer Serial: ");
    bn = ASN1_INTEGER_to_BN(X509_get_serialNumber(cert),NULL);
    if(bn) {
	BN_print_fp(output, bn);
	BN_free(bn);
	fprintf(output, "\n");
    } else {
	fprintf(output, "unknown\n");
    }
#endif    //XMLSEC_FUTURE_SUPPORT    
}


static void 
xmlSecSymbianCryptoX509CertDebugXmlDump(X509* cert, FILE* output) {
  
#ifdef XMLSEC_FUTURE_SUPPORT    
    BIGNUM *bn = NULL;

    xmlSecAssert(cert);
    xmlSecAssert(output);
    
    fprintf(output, "=== X509 Certificate\n");
    fprintf(output, "==== Subject Name: %s\n", 
	 X509_NAME_oneline(X509_get_subject_name(cert), buf, sizeof(buf))); 
    fprintf(output, "==== Issuer Name: %s\n", 
	 X509_NAME_oneline(X509_get_issuer_name(cert), buf, sizeof(buf))); 
    fprintf(output, "==== Issuer Serial: ");
    bn = ASN1_INTEGER_to_BN(X509_get_serialNumber(cert),NULL);
    if(bn) {
	BN_print_fp(output, bn);
	BN_free(bn);
	fprintf(output, "\n");
    } else {
	fprintf(output, "unknown\n");
    }
#endif    //XMLSEC_FUTURE_SUPPORT    
}


/**************************************************************************
 *
 * Raw X509 Certificate processing
 *
 *
 *************************************************************************/
static int		xmlSecSymbianCryptoKeyDataRawX509CertBinRead	(xmlSecKeyDataId id,
								 xmlSecKeyPtr key,
								 const xmlSecByte* buf,
								 xmlSecSize bufSize,
								 xmlSecKeyInfoCtxPtr keyInfoCtx);

static xmlSecKeyDataKlass xmlSecSymbianCryptoKeyDataRawX509CertKlass = {
    sizeof(xmlSecKeyDataKlass),
    sizeof(xmlSecKeyData),

    /* data */
    xmlSecNameRawX509Cert,
    xmlSecKeyDataUsageRetrievalMethodNodeBin, 
						/* xmlSecKeyDataUsage usage; */
    xmlSecHrefRawX509Cert,			/* const xmlChar* href; */
    NULL,					/* const xmlChar* dataNodeName; */
    xmlSecDSigNs,				/* const xmlChar* dataNodeNs; */
    
    /* constructors/destructor */
    NULL,					/* xmlSecKeyDataInitializeMethod initialize; */
    NULL,					/* xmlSecKeyDataDuplicateMethod duplicate; */
    NULL,					/* xmlSecKeyDataFinalizeMethod finalize; */
    NULL,					/* xmlSecKeyDataGenerateMethod generate; */

    /* get info */
    NULL,			 		/* xmlSecKeyDataGetTypeMethod getType; */
    NULL,					/* xmlSecKeyDataGetSizeMethod getSize; */
    NULL,					/* xmlSecKeyDataGetIdentifier getIdentifier; */    

    /* read/write */
    NULL,					/* xmlSecKeyDataXmlReadMethod xmlRead; */
    NULL,					/* xmlSecKeyDataXmlWriteMethod xmlWrite; */
    xmlSecSymbianCryptoKeyDataRawX509CertBinRead,	/* xmlSecKeyDataBinReadMethod binRead; */
    NULL,					/* xmlSecKeyDataBinWriteMethod binWrite; */

    /* debug */
    NULL,					/* xmlSecKeyDataDebugDumpMethod debugDump; */
    NULL,					/* xmlSecKeyDataDebugDumpMethod debugXmlDump; */

    /* reserved for the future */
    NULL,					/* void* reserved0; */
    NULL,					/* void* reserved1; */
};

/**
 * xmlSecSymbianCryptoKeyDataRawX509CertGetKlass:
 * 
 * The raw X509 certificates key data klass.
 *
 * Returns raw X509 certificates key data klass.
 */
EXPORT_C
xmlSecKeyDataId 
xmlSecSymbianCryptoKeyDataRawX509CertGetKlass(void) {
    return(&xmlSecSymbianCryptoKeyDataRawX509CertKlass);
}

static int
xmlSecSymbianCryptoKeyDataRawX509CertBinRead(xmlSecKeyDataId id, xmlSecKeyPtr key,
				    const xmlSecByte* buf, xmlSecSize bufSize,
				    xmlSecKeyInfoCtxPtr keyInfoCtx) {
    xmlSecKeyDataPtr data;
    X509* cert;
    int ret;
    
    xmlSecAssert2(id == xmlSecSymbianCryptoKeyDataRawX509CertId, -1);
    xmlSecAssert2(key, -1);
    xmlSecAssert2(buf, -1);
    xmlSecAssert2(bufSize > 0, -1);
    xmlSecAssert2(keyInfoCtx, -1);

    cert = xmlSecSymbianCryptoX509CertDerRead(buf, bufSize);
    if(!cert) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    NULL,
		    "xmlSecSymbianCryptoX509CertDerRead",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	return(-1);
    }

    data = xmlSecKeyEnsureData(key, xmlSecSymbianCryptoKeyDataX509Id);
    if(!data) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    "xmlSecKeyEnsureData",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	X509_free(cert);
	return(-1);
    }
    
    ret = xmlSecSymbianCryptoKeyDataX509AdoptCert(data, cert);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    "xmlSecSymbianCryptoKeyDataX509AdoptCert",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	X509_free(cert);
	return(-1);
    }

    ret = xmlSecSymbianCryptoKeyDataX509VerifyAndExtractKey(data, key, keyInfoCtx);
    if(ret < 0) {
	xmlSecError(XMLSEC_ERRORS_HERE,
		    xmlSecErrorsSafeString(xmlSecKeyDataKlassGetName(id)),
		    "xmlSecSymbianCryptoKeyDataX509VerifyAndExtractKey",
		    XMLSEC_ERRORS_R_XMLSEC_FAILED,
		    XMLSEC_ERRORS_NO_MESSAGE);
	X509_free(cert);
	return(-1);
    }
    X509_free(cert);
    return(0);
}


#endif /* XMLSEC_NO_X509 */

