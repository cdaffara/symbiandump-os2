/*
 * Copyright (c) 2001 Alexey Zelkin <phantom@FreeBSD.org>
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * * Portions Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */
 
  
#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)localeconv.c	8.1 (Berkeley) 6/4/93";
#endif /* LIBC_SCCS and not lint */
#include <sys/cdefs.h>
__FBSDID("$FreeBSD: src/lib/libc/locale/localeconv.c,v 1.13 2003/06/26 10:46:16 phantom Exp $");

#include <locale.h>

#include "lmonetary.h"
#include "lnumeric.h"
#include <_ansi.h>

#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#include "libc_wsd_defs.h"
#endif

#ifdef EMULATOR

GET_STATIC_VAR_FROM_TLS(lret, struct lconv)
#define lret (*GET_WSD_VAR_NAME(lret, s)())
#endif //EMULATOR

/* 
 * The localeconv() function constructs a struct lconv from the current
 * monetary and numeric locales.
 *
 * Because localeconv() may be called many times (especially by library
 * routines like printf() & strtod()), the approprate members of the 
 * lconv structure are computed only when the monetary or numeric 
 * locale has been changed.
 */

#ifndef EMULATOR

int __mlocale_changed = 1;
int __nlocale_changed = 1;
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
unsigned char __clocale_set = 0;
unsigned char __applocale_changed = 0;
#endif
#else //EMULATOR

GET_GLOBAL_VAR_FROM_TLS(__mlocale_changed, int)
GET_GLOBAL_VAR_FROM_TLS(__nlocale_changed, int)
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
GET_GLOBAL_VAR_FROM_TLS(__clocale_set, unsigned char)
GET_GLOBAL_VAR_FROM_TLS(__applocale_changed, unsigned char)
#endif

#define __mlocale_changed (*GET_WSD_VAR_NAME(__mlocale_changed, g)())
#define __nlocale_changed (*GET_WSD_VAR_NAME(__nlocale_changed, g)())
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
#define __clocale_set (*GET_WSD_VAR_NAME(__clocale_set, g)())
#define __applocale_changed (*GET_WSD_VAR_NAME(__applocale_changed, g)())
#endif
#endif //EMULATOR


/*
 * Return the current locale conversion.
 */
EXPORT_C struct lconv *
localeconv()
{
#ifdef __SYMBIAN32__
    static struct lconv lret;
#else //__SYMBIAN32__
    static struct lconv ret;
#endif //__SYMBIAN32__

#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
    if (__mlocale_changed || !(__applocale_changed)) {
#else
    if (__mlocale_changed) {
#endif
	/* LC_MONETARY part */
        struct lc_monetary_T * mptr; 
	
#ifndef __SYMBIAN32__

#define M_ASSIGN_STR(NAME) (ret.NAME = (char*)mptr->NAME)
#define M_ASSIGN_CHAR(NAME) (ret.NAME = mptr->NAME[0])
#else //__SYMBIAN32__

#define M_ASSIGN_STR(NAME) (lret.NAME = (char*)mptr->NAME)
#define M_ASSIGN_CHAR(NAME) (lret.NAME = mptr->NAME[0])
#endif //__SYMBIAN32__

	mptr = __get_current_monetary_locale();
	M_ASSIGN_STR(int_curr_symbol);
	M_ASSIGN_STR(currency_symbol);
	M_ASSIGN_STR(mon_decimal_point);
	M_ASSIGN_STR(mon_thousands_sep);
	M_ASSIGN_STR(mon_grouping);
	M_ASSIGN_STR(positive_sign);
	M_ASSIGN_STR(negative_sign);
	M_ASSIGN_CHAR(int_frac_digits);
	M_ASSIGN_CHAR(frac_digits);
	M_ASSIGN_CHAR(p_cs_precedes);
	M_ASSIGN_CHAR(p_sep_by_space);
	M_ASSIGN_CHAR(n_cs_precedes);
	M_ASSIGN_CHAR(n_sep_by_space);
	M_ASSIGN_CHAR(p_sign_posn);
	M_ASSIGN_CHAR(n_sign_posn);
	M_ASSIGN_CHAR(int_p_cs_precedes);
	M_ASSIGN_CHAR(int_n_cs_precedes);
	M_ASSIGN_CHAR(int_p_sep_by_space);
	M_ASSIGN_CHAR(int_n_sep_by_space);
	M_ASSIGN_CHAR(int_p_sign_posn);
	M_ASSIGN_CHAR(int_n_sign_posn);
	__mlocale_changed = 0;
    }

    if (__nlocale_changed) {
	/* LC_NUMERIC part */
        struct lc_numeric_T * nptr; 

#ifndef __SYMBIAN32__

#define N_ASSIGN_STR(NAME) (ret.NAME = (char*)nptr->NAME)
#else //__SYMBIAN32__

#define N_ASSIGN_STR(NAME) (lret.NAME = (char*)nptr->NAME)
#endif //__SYMBIAN32__

	nptr = __get_current_numeric_locale();
	N_ASSIGN_STR(decimal_point);
	N_ASSIGN_STR(thousands_sep);
	N_ASSIGN_STR(grouping);
	__nlocale_changed = 0;
    }
#ifndef __SYMBIAN32__

    return (&ret);
#else //__SYMBIAN32__

    return (&lret);
#endif //__SYMBIAN32__
}
