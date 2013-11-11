/** 
 * XML Security Library (http://www.aleksey.com/xmlsec).
 *
 * Export macro declarations for Win32 platform.
 *
 * This is free software; see Copyright file in the source
 * distribution for preciese wording.
 * 
 * Copyright (C) 2002-2003 Aleksey Sanin <aleksey@aleksey.com>
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */
#ifndef __XMLSEC_EXPORTS_H__
#define __XMLSEC_EXPORTS_H__    

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */ 

#include "xmlsec_config.h"

/* Now, the export orgy begins. The following we must do for the 
   Windows platform with MSVC compiler. */

#if !defined XMLSEC_EXPORT
#  if defined _MSC_VER
     /* if we compile libxmlsec itself: */
#    if defined(IN_XMLSEC)
#      if !defined(XMLSEC_STATIC)
#        define XMLSEC_EXPORT __declspec(dllexport) 
#      else
#        define XMLSEC_EXPORT extern
#      endif
     /* if a client program includes this file: */
#    else
#      if !defined(XMLSEC_STATIC)
#        define XMLSEC_EXPORT __declspec(dllimport) 
#      else
#        define XMLSEC_EXPORT 
#      endif
#    endif
   /* This holds on all other platforms/compilers, which are easier to
      handle in regard to this. */
#  else
#    define XMLSEC_EXPORT
#  endif
#endif

#if !defined XMLSEC_CRYPTO_EXPORT
#  if defined _MSC_VER
     /* if we compile libxmlsec itself: */
#    if defined(IN_XMLSEC_CRYPTO)
#      if !defined(XMLSEC_STATIC)
#        define XMLSEC_CRYPTO_EXPORT __declspec(dllexport) 
#      else
#        define XMLSEC_CRYPTO_EXPORT extern
#      endif
     /* if a client program includes this file: */
#    else
#      if !defined(XMLSEC_STATIC)
#        define XMLSEC_CRYPTO_EXPORT __declspec(dllimport) 
#      else
#        define XMLSEC_CRYPTO_EXPORT 
#      endif
#    endif
   /* This holds on all other platforms/compilers, which are easier to
      handle in regard to this. */
#  else
#    define XMLSEC_CRYPTO_EXPORT
#  endif
#endif

#if !defined XMLSEC_EXPORT_VAR
#  if defined _MSC_VER
     /* if we compile libxmlsec itself: */
#    if defined(IN_XMLSEC)
#      if !defined(XMLSEC_STATIC)
#        define XMLSEC_EXPORT_VAR __declspec(dllexport) extern
#      else
#        define XMLSEC_EXPORT_VAR extern
#      endif
     /* if we compile libxmlsec-crypto itself: */
#    elif defined(IN_XMLSEC_CRYPTO)
#        define XMLSEC_EXPORT_VAR extern
     /* if a client program includes this file: */
#    else
#      if !defined(XMLSEC_STATIC)
#        define XMLSEC_EXPORT_VAR __declspec(dllimport) extern
#      else
#        define XMLSEC_EXPORT_VAR extern
#      endif
#    endif
   /* This holds on all other platforms/compilers, which are easier to
      handle in regard to this. */
#  else
#    define XMLSEC_EXPORT_VAR extern
#  endif
#endif

/* Added for Symbian WINSCW/ARM */
#ifdef __SYMBIAN32__
#  undef  XMLSEC_EXPORT
#  if defined(IN_XMLSEC1)
#    define XMLSEC_EXPORT EXPORT_C
#  else
#    define XMLSEC_EXPORT IMPORT_C
#  endif
#  undef  XMLSEC_EXPORT_VAR
//Venus: variables will not exported through XMLSEC_EXPORT_VAR, use XMLSEC_EXPORT_VAR2 instead
#  if defined(__MARM_ARMV5__)
#    define XMLSEC_EXPORT_VAR static 	
#  else
#    define XMLSEC_EXPORT_VAR
#  endif
#  if defined(IN_XMLSEC) || defined(__MARM_ARMV5__)
#    define XMLSEC_EXPORT_VAR2 __declspec(dllexport) extern	
#  else
#    define XMLSEC_EXPORT_VAR2 __declspec(dllimport) extern	
#  endif
#  define XMLSEC_GLOBAL_CONST xmlSecGetGlobalConsts()->
#endif

#ifdef __SYMBIAN32__
#  undef  XMLSEC_CRYPTO_EXPORT
#  if defined(IN_XMLSEC_CRYPTO1)
#    define XMLSEC_CRYPTO_EXPORT EXPORT_C
#  else
#    define XMLSEC_CRYPTO_EXPORT IMPORT_C
#  endif
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __XMLSEC_EXPORTS_H__ */


