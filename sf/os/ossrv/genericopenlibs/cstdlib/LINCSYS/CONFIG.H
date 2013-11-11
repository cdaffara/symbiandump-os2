/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



/**
 @file
 @publishedAll
 @released
*/

#if defined(__H8300__) || defined(__H8500__) || defined (__H8300H__) ||  defined(__W65__)
#define _FLOAT_ARG float
#define __SMALL_BITFIELDS
#define _DOUBLE_IS_32BITS
#define __IEEE_BIG_ENDIAN
/** 
???  This conditional is true for the h8500 and the w65, defining H8300
in those cases probably isn't the right thing to do.  
*/
#define H8300 1
#endif

#ifdef __W65__
#define _DOUBLE_IS_32BITS
#define __SMALL_BITFIELDS
#define __IEEE_BIG_ENDIAN
#undef INT_MAX
#undef UINT_MAX
#define INT_MAX 32767
#define UINT_MAX 65535

#endif

/** 
16 bit integer machines 
*/
#if defined(__Z8001__) || defined(__Z8002__) || defined(__H8300__) || defined(__H8500__) || defined(__W65__) || defined (__H8300H__)
#undef INT_MAX
#undef UINT_MAX
#define INT_MAX 32767
#define UINT_MAX 65535
#endif

#ifdef ___AM29K__
#define _FLOAT_RET double
#endif

#ifdef __i386__
#ifndef __unix__
/** 
in other words, go32 
*/
#define _FLOAT_RET double
#endif
#endif

#ifdef __m68k__
/** 
This is defined in machine/ieeefp.h; need to check is it redundant here? 
*/
#define __IEEE_BIG_ENDIAN
#endif

#if defined(__EPOC32__)
#ifndef __STDC__
#define __STDC__
#endif	// __STDC__
#define REENTRANT_SYSCALLS_PROVIDED
#define HAVE_GETTIMEOFDAY
#define SIMULATED_SIGNALS
int _execve (const char *path, char * const argv[], char * const envp[]);
int _vfork();
#endif

#if INT_MAX == 32767
typedef long int __int32_t;
typedef unsigned long int __uint32_t;
#else
typedef int __int32_t;
typedef unsigned int __uint32_t;
#endif

