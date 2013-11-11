// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name        : lnumeric.CPP
// Part of     : LIBC
// Contains the source for LC_NUMERIC category 
// Version     : 
// This material, including documentation and any related 
// computer programs, is protected by copyright controlled by 
// Nokia Corporation. All rights are reserved. Copying, 
// including reproducing, storing, adapting or translating, any 
// or all of this material requires the prior written consent of 
// Nokia Corporation. This material also contains confidential 
// information which may not be disclosed to others without the 
// prior written consent of Nokia Corporation.
//

#include <limits.h>
#include <sys/cdefs.h>
#include "localeinfo.h"	
#include "lnumeric.h"

#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#include "libc_wsd_defs.h"
#endif
__BEGIN_DECLS
extern int __nlocale_changed;
#ifdef EMULATOR
int *GET_WSD_VAR_NAME(__nlocale_changed, g)();
#define __nlocale_changed (*GET_WSD_VAR_NAME(__nlocale_changed, g)())
#endif //EMULATOR
__END_DECLS

#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
extern unsigned char __clocale_set;
#ifdef EMULATOR
unsigned char *GET_WSD_VAR_NAME(__clocale_set, g)();
#define __clocale_set (*GET_WSD_VAR_NAME(__clocale_set, g)())
#endif //EMULATOR
#endif
#ifndef EMULATOR
int	_numeric_using_locale;
#else //EMULATOR

GET_GLOBAL_VAR_FROM_TLS(_numeric_using_locale, int)
#define _numeric_using_locale (*GET_WSD_VAR_NAME(_numeric_using_locale, g)())
#endif //EMULATOR

#ifndef EMULATOR
static char	numempty[] = { CHAR_MAX, '\0' };
#else //EMULATOR
static const char	numempty[] = { CHAR_MAX, '\0'};
#endif //EMULATOR


static const struct lc_numeric_T _C_numeric_locale = {
	".",     	/* decimal_point */
	"",     	/* thousands_sep */
	numempty	/* grouping */
};


int __numeric_load_locale(const char* localeName)
{
	//Get an instance of locale object
	CLocale* locale = CLocale::GetInstance();	
	//Load numeric(LC_NUMERIC category) information
	if (locale->NumericLoadLocale(localeName) == -1)
	{
		return -1;
	}
	__nlocale_changed = 1;
	_numeric_using_locale = 1;
	
	return 0;
}

struct lc_numeric_T* __get_current_numeric_locale(void)
{
	//Retrieve numeric(LC_NUMERIC category) information
	if(_numeric_using_locale)
	{
		return (CLocale::GetCurrentNumericLocale());			
	}
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
	else if(__clocale_set)
#endif
	//retrieve POSIX locale numeric(LC_NUMERIC category) information
	return ((struct lc_numeric_T *)&_C_numeric_locale);
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
	else
	{
		CLocale *l=CLocale::GetInstance();
		l->NumericLoadLocale(NULL);
		return (CLocale::GetCurrentNumericLocale());	
	}
#endif
}

#ifdef LOCALE_DEBUG
void
numericdebug(void) {
printf(	"decimal_point = %s\n"
	"thousands_sep = %s\n"
	"grouping = %s\n",
	_numeric_locale.decimal_point,
	_numeric_locale.thousands_sep,
	_numeric_locale.grouping
);
}
#endif /* LOCALE_DEBUG */
