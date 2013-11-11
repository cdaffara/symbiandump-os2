/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Name        : stl_symbian.h
* Part of     : standard c++ library. (config file)
* 
*
*/




#ifndef _STL_SYMBIAN_H_
#define _STL_SYMBIAN_H_

#ifndef _STLP_HAS_NO_NEW_C_HEADERS
#define _STLP_HAS_NO_NEW_C_HEADERS
#endif //_STLP_HAS_NO_NEW_C_HEADERS

#define _STLP_NATIVE_HEADER
#define _STLP_NATIVE_C_HEADER(x)	<stdapis/x>
#define _STLP_NATIVE_CPP_RUNTIME_HEADER(header)	<stdapis/stlportv5/header>
#define _STLP_USE_DEFAULT_FILE_OFFSET

#if defined _STLP_USE_DECLSPEC
#undef _STLP_USE_DECLSPEC
#endif

#if defined _STLP_EXPORT_DECLSPEC
#undef _STLP_EXPORT_DECLSPEC
#endif

#if defined _STLP_IMPORT_DECLSPEC
#undef _STLP_IMPORT_DECLSPEC
#endif

#define _STLP_USE_DECLSPEC
#define _STLP_EXPORT_DECLSPEC	EXPORT_C
#define _STLP_IMPORT_DECLSPEC	IMPORT_C

/* No support for Class declspec */
#if defined _STLP_CLASS_EXPORT_DECLSPEC
#undef _STLP_CLASS_EXPORT_DECLSPEC 
#endif

#if defined _STLP_CLASS_IMPORT_DECLSPEC
#undef _STLP_CLASS_IMPORT_DECLSPEC
#endif

#define _STLP_CLASS_EXPORT_DECLSPEC 
#define _STLP_CLASS_IMPORT_DECLSPEC 


#define _STLP_NO_OWN_NAMESPACE	1
#define _STLP_USE_NAMESPACES	1

#if !defined (_STLP_EXPOSE_GLOBALS_IMPLEMENTATION)
#  define _STLP_EXPOSE_GLOBALS_IMPLEMENTATION 1
#endif // _STLP_EXPOSE_GLOBALS_IMPLEMENTATION

#define _STLP_NO_VENDOR_MATH_L

#define _REENTRANT
#define _STLP_PTHREADS
#define _STLP_STDIO_FILE_H

#ifdef __EPOC32__
#define __SYMBIAN32__NO_STATIC_IMPORTS__
#else

#endif

#endif //_STL_SYMBIAN_H_
 
