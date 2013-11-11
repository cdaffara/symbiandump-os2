/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Symbian definitions for the core engine data types
*
*/


#ifndef AM_PLATFORM_LIBRARIES_H
#define AM_PLATFORM_LIBRARIES_H

#ifndef __KERNEL_MODE__
#include <e32base.h>
#else
#include <kernel.h>
#endif // __KERNEL__MODE

/**
* The basic types cannot be included from EAPOL because
* they include user-space headers that is prohibited in
* kernel code.
*
* Therefore, we simply copy the basic types for kernel
* code.
*/

/** JPHC++ definitions */
typedef signed char i8_t;
typedef unsigned char u8_t;
typedef short int i16_t;
typedef unsigned short int u16_t;
typedef long int i32_t;
typedef unsigned long int u32_t;

#if defined(__WINSCW__)
	/// This is signed 64-bit integer.
	typedef long long i64_t;
#elif defined(__GNUC__)
	/// This is signed 64-bit integer.
	typedef long long i64_t;
#else
	/// This is signed 64-bit integer.
	typedef __int64 i64_t;	
#endif

#if defined(__WINSCW__)
	/// This is unsigned 64-bit integer.
	typedef unsigned long long u64_t;	
#elif defined(__GNUC__)
	/// This is unsigned 64-bit integer.
	typedef unsigned long long u64_t;	
#else
	/// This is unsigned 64-bit integer. Actually there is not unsigned version.
	typedef __int64 u64_t;
#endif

/** NOC++ definitions */
typedef int int_t;
typedef unsigned int uint_t;
typedef unsigned char text8_t;
typedef unsigned short int text16_t;

typedef int bool_t;
enum _false_t { false_t = 0 };
enum _true_t { true_t = 1 };

#endif // AM_PLATFORM_LIBRARIES_H
