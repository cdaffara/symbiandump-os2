/** 
 * XML Security Library (http://www.aleksey.com/xmlsec).
 *
 * Memory buffer transform
 *
 * This is free software; see Copyright file in the source
 * distribution for preciese wording.
 * 
 * Copyright (C) 2002-2003 Aleksey Sanin <aleksey@aleksey.com>
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */
#ifndef __XMLSEC_MEMBUF_H__
#define __XMLSEC_MEMBUF_H__    

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */ 

#include <stdapis/libxml2/libxml2_tree.h>
#include "xmlsec_config.h"
#include "xmlsec_xmlsec.h"
#include "xmlsec_buffer.h"
#include "xmlsec_transforms.h"

/********************************************************************
 *
 * Memory Buffer transform 
 *
 *******************************************************************/
/**
 * xmlSecTransformMemBufId:
 * 
 * The Memory Buffer transform klass.
 */
#define xmlSecTransformMemBufId \
	xmlSecTransformMemBufGetKlass()
XMLSEC_EXPORT xmlSecTransformId	xmlSecTransformMemBufGetKlass		(void);
XMLSEC_EXPORT xmlSecBufferPtr	xmlSecTransformMemBufGetBuffer		(xmlSecTransformPtr transform);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __XMLSEC_MEMBUF_H__ */

