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
* Description:  Contains the source for all the system calls in libc
 *     
 *
*/

 
#include <limits.h>
#include <stdlib.h>
#include <sys/cdefs.h>
#include "localeinfo.h"
#include "lmonetary.h"

#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#include "libc_wsd_defs.h"
#endif

// Muzibour : Include #include <sys/cdefs.h> and then use the macro __BEGIN_DECLS
__BEGIN_DECLS
extern int __mlocale_changed;
#ifdef EMULATOR
int *GET_WSD_VAR_NAME(__mlocale_changed, g)();
#define __mlocale_changed (*GET_WSD_VAR_NAME(__mlocale_changed, g)())
#endif //EMULATOR
__END_DECLS
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
__BEGIN_DECLS
extern unsigned char __clocale_set;
#ifdef EMULATOR
unsigned char *GET_WSD_VAR_NAME(__clocale_set, g)();
#define __clocale_set (*GET_WSD_VAR_NAME(__clocale_set, g)())
#endif //EMULATOR
__END_DECLS
#endif
#ifndef EMULATOR
int	_monetary_using_locale;
#else //EMULATOR

GET_GLOBAL_VAR_FROM_TLS(_monetary_using_locale, int)
#define	_monetary_using_locale (*GET_WSD_VAR_NAME(_monetary_using_locale, g)())
#endif //EMULATOR

#ifndef EMULATOR
static char	empty[] = "";
static char	numempty[] = { CHAR_MAX, '\0'};
#else //EMULATOR
static const char	empty[] = "";
static const char	numempty[] = { CHAR_MAX, '\0'};

#endif //EMULATOR

static const struct lc_monetary_T _C_monetary_locale = {
	empty,		/* int_curr_symbol */
	empty,		/* currency_symbol */
	empty,		/* mon_decimal_point */
	empty,		/* mon_thousands_sep */
	numempty,	/* mon_grouping */
	empty,		/* positive_sign */
	empty,		/* negative_sign */
	numempty,	/* int_frac_digits */
	numempty,	/* frac_digits */
	numempty,	/* p_cs_precedes */
	numempty,	/* p_sep_by_space */
	numempty,	/* n_cs_precedes */
	numempty,	/* n_sep_by_space */
	numempty,	/* p_sign_posn */
	numempty,	/* n_sign_posn */
	numempty,	/* int_p_cs_precedes */
	numempty,	/* int_n_cs_precedes */
	numempty,	/* int_p_sep_by_space */
	numempty,	/* int_n_sep_by_space */
	numempty,	/* int_p_sign_posn */
	numempty	/* int_n_sign_posn */
};



int
__monetary_load_locale(const char* localeName)
{
	//Get an instance of locale object
	CLocale* locale = CLocale::GetInstance();	
	//Load the monetary(LC_MONETARY category) information
	TInt ret = KErrNone;
	TRAPD(result,ret = locale->MonetaryLoadLocaleL(localeName));
	if ((result != KErrNone) || (ret == -1))
	{
		return -1;
	}
	__mlocale_changed = 1;
	_monetary_using_locale = 1;
	
	return 0;
}

struct lc_monetary_T *
__get_current_monetary_locale(void)
{
	//Retreive the monetary(LC_MONETARY category) information
	if(_monetary_using_locale)
	{
		return (CLocale::GetCurrentMonetaryLocale());			
	}
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
	else if(__clocale_set)
#endif
	//return POSIX C locale monetary(LC_MONETARY category) information
	return ((struct lc_monetary_T *)&_C_monetary_locale);
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
	else
	{
		CLocale *l=CLocale::GetInstance();
		TRAPD(result,l->MonetaryLoadLocaleL(NULL));
		if (result != KErrNone)
		{
			return NULL;
		}
		return (CLocale::GetCurrentMonetaryLocale());
	}
#endif
}

#ifdef LOCALE_DEBUG
void
monetdebug() {
printf(	"int_curr_symbol = %s\n"
	"currency_symbol = %s\n"
	"mon_decimal_point = %s\n"
	"mon_thousands_sep = %s\n"
	"mon_grouping = %s\n"
	"positive_sign = %s\n"
	"negative_sign = %s\n"
	"int_frac_digits = %d\n"
	"frac_digits = %d\n"
	"p_cs_precedes = %d\n"
	"p_sep_by_space = %d\n"
	"n_cs_precedes = %d\n"
	"n_sep_by_space = %d\n"
	"p_sign_posn = %d\n"
	"n_sign_posn = %d\n",
	"int_p_cs_precedes = %d\n"
	"int_p_sep_by_space = %d\n"
	"int_n_cs_precedes = %d\n"
	"int_n_sep_by_space = %d\n"
	"int_p_sign_posn = %d\n"
	"int_n_sign_posn = %d\n",
	_monetary_locale.int_curr_symbol,
	_monetary_locale.currency_symbol,
	_monetary_locale.mon_decimal_point,
	_monetary_locale.mon_thousands_sep,
	_monetary_locale.mon_grouping,
	_monetary_locale.positive_sign,
	_monetary_locale.negative_sign,
	_monetary_locale.int_frac_digits[0],
	_monetary_locale.frac_digits[0],
	_monetary_locale.p_cs_precedes[0],
	_monetary_locale.p_sep_by_space[0],
	_monetary_locale.n_cs_precedes[0],
	_monetary_locale.n_sep_by_space[0],
	_monetary_locale.p_sign_posn[0],
	_monetary_locale.n_sign_posn[0],
	_monetary_locale.int_p_cs_precedes[0],
	_monetary_locale.int_p_sep_by_space[0],
	_monetary_locale.int_n_cs_precedes[0],
	_monetary_locale.int_n_sep_by_space[0],
	_monetary_locale.int_p_sign_posn[0],
	_monetary_locale.int_n_sign_posn[0]
);
}
#endif /* LOCALE_DEBUG */
