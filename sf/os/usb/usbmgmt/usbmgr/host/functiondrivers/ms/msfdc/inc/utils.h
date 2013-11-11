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
* Utilities for server. (These depend on the logger.)
*
*/

/**
 @file
 @internalComponent
*/

#ifndef UTILS_H
#define UTILS_H

#include <e32base.h>
#include <ecom/ecom.h>
#include <usb/usblogger.h>

// In debug, using checking forms of CleanupStack::Pop. In release builds, 
// use the non-checking form to save a little bit of ROM.
#ifdef _DEBUG
#define CLEANUPSTACK_POP1(a)		CleanupStack::Pop(a);
#define CLEANUPSTACK_POP2(a, b) 	CleanupStack::Pop(a, b);
#else
#define CLEANUPSTACK_POP1(a)		CleanupStack::Pop();
#define CLEANUPSTACK_POP2(a, b) 	CleanupStack::Pop(2);
#endif // _DEBUG

// What we want for internal programming errors in a server is a set of macros 
// which, to save effort all round, use __LINE__ as the panic code, and a 
// file-specific panic category. To make this non-standard pattern as helpful 
// to users as possible, we append ' line#' to the category. That means we 
// first have to check that the category is 10 characters long or less, so 
// that the whole thing is legible to users when it appears on the screen.
template <TBool> struct ASSERTION_FAILURE;
TEMPLATE_SPECIALIZATION struct ASSERTION_FAILURE<ETrue>{};
template <TInt> struct __assertion_test;
#define COMPILE_ASSERT( B ) void __compile_assert(::__assertion_test<sizeof(::ASSERTION_FAILURE<(B)>)>)

// We want a 10-character string (but allow for the NULL terminator).
#define PANICCATEGORY(aaa) COMPILE_ASSERT(sizeof(L##aaa)/2 <= 11); _LIT(KPanicCat, aaa) 

// A handy panic-self macro- the category is KPanicCat with " line#" appended; 
// the code is the line number. 
#define PANIC_LINENUM \
	{ \
	_LIT(KLnNo, " line#"); \
	TBuf<KMaxExitCategoryName> cat = KPanicCat(); \
	cat.Append(KLnNo()); \
	_USB_PANIC(cat, __LINE__); \
	}

// A handy assertion macro that panics with a locally-defined panic category 
// and the line number.
#define ASSERT_ALWAYS(a) \
	{ \
	if ( !(a) ) \
		{ \
		PANIC_LINENUM; \
		} \
	}

#ifdef _DEBUG
#define ASSERT_DEBUG(a) ASSERT_ALWAYS(a)
#define DEBUG_PANIC_LINENUM PANIC_LINENUM
#else
#define ASSERT_DEBUG(a)
#define DEBUG_PANIC_LINENUM
#endif // _DEBUG

// Undefine the e32def.h-defined ASSERT macro to make sure no-one uses it 
// under the mistaken impression that it's useful. Use our informative one 
// above instead!
#undef ASSERT

/**
Cleanup stack item to remove a given TUint from an RArray.
*/
struct TArrayRemove
	{
	TArrayRemove(RArray<TUint>& aDeviceIds, TUint aDeviceId);
	~TArrayRemove();
	
	RArray<TUint>& iDeviceIds;
	const TUint iDeviceId;
	};
void CleanupRemovePushL(TArrayRemove& aArrayRemove);
void Remove(TAny* aArrayRemove);

#endif // UTILS_H
