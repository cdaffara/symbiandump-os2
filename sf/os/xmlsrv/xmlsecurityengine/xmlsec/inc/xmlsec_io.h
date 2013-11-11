/** 
 * XML Security Library (http://www.aleksey.com/xmlsec).
 *
 * Input uri transform and utility functions.
 *
 * This is free software; see Copyright file in the source
 * distribution for preciese wording.
 * 
 * Copyright (C) 2002-2003 Aleksey Sanin <aleksey@aleksey.com>
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */
#ifndef __XMLSEC_IO_H__
#define __XMLSEC_IO_H__    

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */ 

#include <stdapis/libxml2/libxml2_tree.h>
#include <stdapis/libxml2/libxml2_xmlio.h>
#include "xmlsec_config.h"
#include "xmlsec_xmlsec.h"
#include "xmlsec_transforms.h"

XMLSEC_EXPORT int	xmlSecIOInit				(void);
XMLSEC_EXPORT void	xmlSecIOShutdown			(void);
XMLSEC_EXPORT void	xmlSecIOCleanupCallbacks		(void);
XMLSEC_EXPORT int	xmlSecIORegisterDefaultCallbacks 	(void);
XMLSEC_EXPORT int     	xmlSecIORegisterCallbacks		(xmlInputMatchCallback matchFunc,
								 xmlInputOpenCallback openFunc,
								 xmlInputReadCallback readFunc,
								 xmlInputCloseCallback closeFunc);

/********************************************************************
 *
 * Input URI transform 
 *
 *******************************************************************/
/**
 * xmlSecTransformInputURIId:
 * 
 * The Input URI transform id.
 */
#define xmlSecTransformInputURIId \
	xmlSecTransformInputURIGetKlass()
XMLSEC_EXPORT xmlSecTransformId	xmlSecTransformInputURIGetKlass	(void);
XMLSEC_EXPORT int 	xmlSecTransformInputURIOpen		(xmlSecTransformPtr transform,
								 const xmlChar* uri);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __XMLSEC_IO_H__ */

