/** 
 * XML Security Library (http://www.aleksey.com/xmlsec).
 *
 * This is free software; see Copyright file in the source
 * distribution for preciese wording.
 * 
 * Copyright (C) 2002-2003 Aleksey Sanin <aleksey@aleksey.com>
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */
#ifndef __XMLSEC_X509_H__
#define __XMLSEC_X509_H__    
#include "xmlsec_config.h"
#ifndef XMLSEC_NO_X509
	
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */ 
#include <stdio.h>		

#include <stdapis/libxml2/libxml2_tree.h>
#include <stdapis/libxml2/libxml2_parser.h>

#include "xmlsec_xmlsec.h"
#include "xmlsec_buffer.h"
#include "xmlsec_list.h"
#include "xmlsec_keys.h"
#include "xmlsec_keysmngr.h"
#include "xmlsec_keyinfo.h"
#include "xmlsec_transforms.h"

/**
 * XMLSEC_X509DATA_CERTIFICATE_NODE:
 *
 * <dsig:X509Certificate/> node found or would be written back.
 */
#define XMLSEC_X509DATA_CERTIFICATE_NODE			0x00000001
/**
 * XMLSEC_X509DATA_SUBJECTNAME_NODE:
 *
 * <dsig:X509SubjectName/> node found or would be written back.
 */
#define XMLSEC_X509DATA_SUBJECTNAME_NODE			0x00000002
/**
 * XMLSEC_X509DATA_ISSUERSERIAL_NODE:
 *
 * <dsig:X509IssuerSerial/> node found or would be written back.
 */
#define XMLSEC_X509DATA_ISSUERSERIAL_NODE			0x00000004
/**
 * XMLSEC_X509DATA_SKI_NODE:
 *
 * <dsig:/X509SKI> node found or would be written back.
 */
#define XMLSEC_X509DATA_SKI_NODE				0x00000008
/**
 * XMLSEC_X509DATA_CRL_NODE:
 *
 * <dsig:X509CRL/> node found or would be written back.
 */
#define XMLSEC_X509DATA_CRL_NODE				0x00000010
/**
 * XMLSEC_X509DATA_DEFAULT:
 *
 * Default set of nodes to write in case of empty
 * <dsig:X509Data/> node template.
 */
#define XMLSEC_X509DATA_DEFAULT	\
	(XMLSEC_X509DATA_CERTIFICATE_NODE | XMLSEC_X509DATA_CRL_NODE)
	    
XMLSEC_EXPORT int		xmlSecX509DataGetNodeContent 	(xmlNodePtr node, 
								 int deleteChildren,
								 xmlSecKeyInfoCtxPtr keyInfoCtx);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XMLSEC_NO_X509 */

#endif /* __XMLSEC_X509_H__ */

