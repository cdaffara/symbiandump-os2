/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/


/**
 @file
 @publishedPartner
 @released
*/

#ifndef __ROMLIT_H__
#define __ROMLIT_H__
#include <e32def.h>
#include <u32std.h>
/**
   Special ROM friendly _LIT16 replacement.

   The existing _LIT16 macro/template successfully creates descriptors
   which will be put into ROM, but it is IMPOSSIBLE to initialize
   another "romable" variable with the address of one....

   What happens is that the compiler thinks it needs to call the
   TLitC16 operator& function call (even though it is a cast which
   will be optimized out of existence) to take the address of the
   object created by _LIT16.

   The result is that the DESTINATION variable gets moved from ROM to
   an initialized data area so that the return value of the TLitC16
   operator& function can be written to it at run time.

   The net result is that your code will not be legal in a target DLL
   (unless you enable thread initialized data, which is not
   recommended).

   A solution is to use this macro/class which creates TRomLitC16
   objects which have an identical layout to TPtrC16 objects and can
   generally be used just like all the other descriptor types.

   You use the _ROMLIT16 macro to create a TRomLitC16 object, then
   take its address and store that in a TRomLitC16 ptr. Attempting to
   store its address into other ptr types will compile but encounter
   the same problem as trying to use _LIT16.
   
   In most cases you can just dereference a TRomLitC16 ptr and pass it
   to any functions which can handle a _LIT16 (due to the out cast
   operators). If it doesn't automatically work, apply the function
   operator to the TRomLitC16 which will return a TDescC16 reference
   (for example ((*ptrToTRomLitC16)()') which is 100% compatible.
*/

#ifndef __TText_defined
#define __TText_defined
#ifdef __GCC32__
typedef wchar_t __TText;
#else
typedef TUint16 __TText;
#endif
#endif

// All L"" strings are correctly aligned to an even boundary so can be
// placed directly into the ptr field of a TPtr object.
//
// Note that the RVCT assembler listings are confusing because a
// standalone ALIGN directive's argument is the requested alignment,
// but the ALIGN argument to an AREA directive specifies a power of 2
// for alignment. So the following RVCT generated code ensures
// wchar_t strings are 2 byte aligned:-
//
// "AREA ||.constwstring||, DATA, READONLY, MERGE=2, STRINGS, ALIGN=1"
#define _ROMLIT16(name,s) \
	const static TRomLitC16 name={((EPtrC<<KShiftDesType16) | (sizeof(L##s)/2-1)), L##s}

struct TRomLitC16
	{
	operator const TDesC16 *() const
		{ return reinterpret_cast<const TDesC16*>(this); }
	operator const TDesC16 &() const
		{ return *(TDesC16 *)this; }
	const TDesC16& operator()() const
		{ return *(TDesC16 *)this; }
	operator const __TRefDesC16() const
		{ return *(TDesC16 *)this; }
	TUint iTypeLength;
	const __TText *iPtr;
	};

#endif
