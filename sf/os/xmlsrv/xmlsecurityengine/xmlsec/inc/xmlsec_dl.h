/** 
 * XML Security Library (http://www.aleksey.com/xmlsec).
 *
 *
 * This is free software; see Copyright file in the source
 * distribution for preciese wording.
 * 
 * Copyright (C) 2002-2003 Aleksey Sanin <aleksey@aleksey.com>
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */
#ifndef __XMLSEC_DL_H__
#define __XMLSEC_DL_H__    

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */ 

#include "xmlsec_config.h"

typedef struct _xmlSecCryptoDLFunctions 	xmlSecCryptoDLFunctions,
						*xmlSecCryptoDLFunctionsPtr;

XMLSEC_EXPORT int 				xmlSecCryptoDLFunctionsRegisterKeyDataAndTransforms
									    (xmlSecCryptoDLFunctionsPtr functions);
										
#ifndef XMLSEC_NO_CRYPTO_DYNAMIC_LOADING

#include <stdapis/libxml2/libxml2_tree.h>
#include <stdapis/libxml2/libxml2_xmlio.h>

#include "xmlsec_xmlsec.h"
#include "xmlsec_keysdata.h"
#include "xmlsec_keys.h"
#include "xmlsec_keysmngr.h"
#include "xmlsec_transforms.h"

/**
 * Dynamic load functions
 */
XMLSEC_EXPORT int				xmlSecCryptoDLInit		(void);
XMLSEC_EXPORT int				xmlSecCryptoDLShutdown		(void);

XMLSEC_EXPORT int				xmlSecCryptoDLLoadLibrary	(const xmlChar* crypto);
XMLSEC_EXPORT xmlSecCryptoDLFunctionsPtr	xmlSecCryptoDLGetLibraryFunctions(const xmlChar* crypto);
XMLSEC_EXPORT int				xmlSecCryptoDLUnloadLibrary	(const xmlChar* crypto);

XMLSEC_EXPORT int 				xmlSecCryptoDLSetFunctions	(xmlSecCryptoDLFunctionsPtr functions);
XMLSEC_EXPORT xmlSecCryptoDLFunctionsPtr 	xmlSecCryptoDLGetFunctions	(void);
					
#endif /* XMLSEC_NO_CRYPTO_DYNAMIC_LOADING */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __XMLSEC_APP_H__ */

