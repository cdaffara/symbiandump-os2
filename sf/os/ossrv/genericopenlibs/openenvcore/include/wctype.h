/*-
 * Copyright (c)1999 Citrus Project,
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	citrus Id: wctype.h,v 1.4 2000/12/21 01:50:21 itojun Exp
 *	$NetBSD: wctype.h,v 1.3 2000/12/22 14:16:16 itojun Exp $
 * $FreeBSD: src/include/wctype.h,v 1.13 2004/08/12 10:29:14 tjr Exp $
 *
 * © Portions copyright (c) 2006 Nokia Corporation.  All rights reserved.
 */

#ifndef _WCTYPE_H_
#define	_WCTYPE_H_

#include <sys/cdefs.h>
#include <sys/_types.h>

#include <_ctype.h>

#ifdef __SYMBIAN32__
#include <_ansi.h>
#endif// __SYMBIAN32__

#ifndef _WCTRANS_T
typedef	int	wctrans_t;
#define	_WCTRANS_T
#endif

#ifndef _WCTYPE_T
typedef	unsigned long	wctype_t;
#define	_WCTYPE_T
#endif

#ifndef _WINT_T_DECLARED
typedef	__wint_t	wint_t;
#define	_WINT_T_DECLARED
#endif

#ifndef WEOF
#define	WEOF	((wint_t)-1)
#endif

#ifdef __SYMBIAN32__
#ifdef __cplusplus
extern "C" {
#endif
#endif // __SYMBIAN32__

__BEGIN_DECLS
IMPORT_C int	iswalnum(wint_t);
IMPORT_C int	iswalpha(wint_t);
IMPORT_C int	iswblank(wint_t);
IMPORT_C int	iswcntrl(wint_t);
IMPORT_C int	iswctype(wint_t, wctype_t);
IMPORT_C int	iswdigit(wint_t);
IMPORT_C int	iswgraph(wint_t);
IMPORT_C int	iswlower(wint_t);
IMPORT_C int	iswprint(wint_t);
IMPORT_C int	iswpunct(wint_t);
IMPORT_C int	iswspace(wint_t);
IMPORT_C int	iswupper(wint_t);
IMPORT_C int	iswxdigit(wint_t);
IMPORT_C wint_t	towctrans(wint_t, wctrans_t);
IMPORT_C wint_t	towlower(wint_t);
IMPORT_C wint_t	towupper(wint_t);
IMPORT_C wctrans_t
	wctrans(const char *);
IMPORT_C wctype_t
	wctype(const char *);
__END_DECLS

#ifdef __SYMBIAN32__
#ifdef __cplusplus
}
#endif
#endif // __SYMBIAN32__

#endif		/* _WCTYPE_H_ */
