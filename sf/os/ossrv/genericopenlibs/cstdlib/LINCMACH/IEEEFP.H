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

#ifndef __IEEE_BIG_ENDIAN
#ifndef __IEEE_LITTLE_ENDIAN

/**
EPOC safety net - Check various EPOC defines to ensure that
we get an appropriate endianness choice
*/
#if (defined(__WINS__) || defined(__MX86__) || defined(__X86GCC__)) && !defined(__i386__)
#define __i386__
#elif defined(__MARM__) && !defined(__arm__)
#define __arm__
#endif

#ifdef __arm__
	#ifdef __EABI__
	/**
	EABI supports the VFP specification, where the ordering of the words
	matches the endianness.
	*/
	#define __IEEE_LITTLE_ENDIAN
#else
	/** 
	Pre-VFP, ARM used the big endian ordering of the words, regardless of
	the endianess. 
	*/
	#define __IEEE_BIG_ENDIAN
	#endif
#endif

#ifdef __hppa__

#define __IEEE_BIG_ENDIAN
#endif

#ifdef __sparc__

#define __IEEE_BIG_ENDIAN
#endif

#if defined(__m68k__) || defined(__mc68000__)

#define __IEEE_BIG_ENDIAN
#endif

#if defined (__H8300__) || defined (__H8300H__)

#define __IEEE_BIG_ENDIAN
#define __SMALL_BITFIELDS
#define _DOUBLE_IS_32BITS
#endif

#ifdef __H8500__

#define __IEEE_BIG_ENDIAN
#define __SMALL_BITFIELDS
#define _DOUBLE_IS_32BITS
#endif

#ifdef __sh__
#ifdef __LITTLE_ENDIAN__

#define __IEEE_LITTLE_ENDIAN
#else
#define __IEEE_BIG_ENDIAN
#endif
#ifdef __SH3E__

#define _DOUBLE_IS_32BITS
#endif
#endif

#ifdef _AM29K

#define __IEEE_BIG_ENDIAN
#endif

/* Added "&& !defined(__arm__)" to avoid the duplicate definition of the
   __ieee_[float|double]_shape_type structures in stdlib\LINC\IEEEFP.h 
   conditionally compiled using the __IEEE_LITTLE_ENDIAN && __IEEE_BIG_ENDIAN 
   in GCCXML builds. This is a work around as we can't get gccxml to stop 
   defining __i386__ as it's defined in the "<built-in>" header file of the 
   compiler.
*/
#if defined(__i386__) && !defined(__arm__)

#define __IEEE_LITTLE_ENDIAN
#endif

#ifdef __i960__

#define __IEEE_LITTLE_ENDIAN
#endif

#ifdef __MIPSEL__

#define __IEEE_LITTLE_ENDIAN
#endif
#ifdef __MIPSEB__

#define __IEEE_BIG_ENDIAN
#endif

/**
necv70 was __IEEE_LITTLE_ENDIAN. 
*/

#ifdef __W65__

#define __IEEE_LITTLE_ENDIAN
#define __SMALL_BITFIELDS
#define _DOUBLE_IS_32BITS
#endif

#if defined(__Z8001__) || defined(__Z8002__)

#define __IEEE_BIG_ENDIAN
#endif

#ifdef __m88k__

#define __IEEE_BIG_ENDIAN
#endif

#ifdef __v800

#define __IEEE_LITTLE_ENDIAN
#endif

#ifdef __PPC__
#ifdef _BIG_ENDIAN

#define __IEEE_BIG_ENDIAN
#else

#define __IEEE_LITTLE_ENDIAN
#endif
#endif

#ifdef __mcore__

#define __IEEE_LITTLE_ENDIAN	/* always little-endian M*Core for EPOC */
#endif

#ifndef __IEEE_BIG_ENDIAN
#ifndef __IEEE_LITTLE_ENDIAN
#error Endianess not declared!!
#endif /* not __IEEE_LITTLE_ENDIAN */
#endif /* not __IEEE_BIG_ENDIAN */

#endif /* not __IEEE_LITTLE_ENDIAN */
#endif /* not __IEEE_BIG_ENDIAN */

