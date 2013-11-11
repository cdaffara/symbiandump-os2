/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file gets the ICU Layout Engine compiling on Symbian OS.
*
*/




#include <e32std.h>

#ifndef __SYMBIANPLATFORM_H_
#define __SYMBIANPLATFORM_H_
#endif

/* This enables Rdebug shaper memory logging */
//#define SHAPER_MEMORY_DEBUG

#define LE_ARRAY_COPY(dst, src, count) (Mem::Copy(dst, src, (count) * sizeof (src)[0]))

#ifdef SHAPER_MEMORY_DEBUG

/* test versions for debugging shaper heap memory problems */
#define LE_NEW_ARRAY(type, count) reinterpret_cast<type*>(NewArray(sizeof(type), count))
#define LE_GROW_ARRAY(array, newSize) GrowArray(array, (newSize) * sizeof (array)[0])
#define LE_DELETE_ARRAY(array) UMemory::FreeArray((void*)array) 

#else

#define LE_NEW_ARRAY(type, count) (  reinterpret_cast<type*>(User::Alloc( sizeof(type)*(count) ))  )
#define LE_GROW_ARRAY(array, newSize) ( User::ReAlloc(array, (newSize) * sizeof (array)[0]) )
#define LE_DELETE_ARRAY(array) ( User::Free((void*)array) )
#endif

#define U_HAVE_NAMESPACE 1
#define U_UTF8_IMPL 1

typedef long long int64_t;

#define U_IS_BIG_ENDIAN 0

/*===========================================================================*/
/* Generic data types                                                        */
/*===========================================================================*/

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
#if defined(_LP64)
typedef signed int  int32_t;
typedef unsigned int  uint32_t;
#else
typedef signed long int32_t;
typedef unsigned long uint32_t;
#endif

#define U_HAVE_WCHAR_H      0
#define U_SIZEOF_WCHAR_T    2

#define U_HAVE_WCSCPY       0

#define U_EXPORT
#define U_EXPORT2
#define U_IMPORT

#define U_MAKE  "Make"

