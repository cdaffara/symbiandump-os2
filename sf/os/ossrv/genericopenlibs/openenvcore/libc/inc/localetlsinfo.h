/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : localetlsinfo.h
* Part of     : LIBC
* Contains the class declartion for locale related functions
* Version     : 
* This material, including documentation and any related 
* computer programs, is protected by copyright controlled by 
* Nokia Corporation. All rights are reserved. Copying, 
* including reproducing, storing, adapting or translating, any 
* or all of this material requires the prior written consent of 
* Nokia Corporation. This material also contains confidential 
* information which may not be disclosed to others without the 
* prior written consent of Nokia Corporation.
*
*/



#ifndef _LOCALETLSINFO_H_
#define _LOCALETLSINFO_H_

#include <e32def.h>
#include <e32cmn.h>
#include "sysif.h"

#define LANG_VARIANT_DLL_NAME 10
#ifdef SYMBIAN_DISTINCT_LOCALE_MODEL
#define NEW_LANG_VARIANT_DLL_NAME 30
#endif
class CLocale;
struct lc_monetary_T;
struct lc_time_T;
struct lc_numeric_T;

struct LocaleInfo
{
	CLocale *_s_iNewLocale;
	lc_monetary_T*_s_iMonetary_locale;
	lc_numeric_T * _s_iNumeric_locale;
	lc_time_T * _s_iTime_locale;

	TText8 *_s_numericLocale;
	TText8 *_s_monetaryLocale;
	TText8 *_s_timeLocale;

	TBuf<LANG_VARIANT_DLL_NAME> _s_DllName;
#ifdef SYMBIAN_DISTINCT_LOCALE_MODEL
	TBuf<NEW_LANG_VARIANT_DLL_NAME> _s_ColDllName;
#endif
	/*******************************************************************
	Overloading new and delete operators so that they will
	allocate and deallocare memory from/to the private heap of backend
	********************************************************************/
	inline TAny* operator new(TUint aSize, TAny* aBase) __NO_THROW
		{
		Mem::FillZ(aBase, aSize); return aBase;
		}
		
	inline TAny* operator new(TUint aSize) __NO_THROW
		{
		return Backend()->Alloc(aSize);
		}
		
	inline TAny* operator new(TUint aSize, TLeave)
		{
		TAny* ptr = Backend()->Alloc(aSize);
		if (ptr == NULL)
			{
			User::Leave(KErrNoMemory);
			}
		return ptr;
		}
		
	inline TAny* operator new(TUint aSize, TUint aExtraSize) __NO_THROW
		{
		return Backend()->Alloc(aSize + aExtraSize);
		}
		
	inline TAny* operator new(TUint aSize, TLeave, TUint aExtraSize)
		{
		TAny* ptr = Backend()->Alloc(aSize + aExtraSize);
		if (ptr == NULL)
			{
			User::Leave(KErrNoMemory);
			}
		return ptr;
		}
	
	inline void operator delete(TAny *aPtr) __NO_THROW
		{
		Backend()->Free( aPtr );
		}
};

typedef struct LocaleInfo _localeinfo;

#endif //_LOCALETLSINFO_H_



