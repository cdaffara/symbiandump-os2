/*-
 * 
 * Copyright (c) 2002-2004 Tim J. Robbins.
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
 * * Portions Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD: src/lib/libc/locale/mbtowc.c,v 1.11 2004/07/29 06:18:40 tjr Exp $");

#include <stdlib.h>
#include <wchar.h>
#ifndef __SYMBIAN32__
#include "mblocal.h"
#endif //__SYMBIAN32__

#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#include "libc_wsd_defs.h"
#endif 

#ifdef EMULATOR

GET_STATIC_VAR_FROM_TLS(mbtowc_mbs, mbstate_t)
#define mbs (*GET_WSD_VAR_NAME(mbtowc_mbs, s)())
#endif //EMULATOR

EXPORT_C int
mbtowc(wchar_t * __restrict pwc, const char * __restrict s, size_t n)
{
	static const mbstate_t initial = {0,{0}};
#ifndef EMULATOR	
	static mbstate_t mbs;
#endif //EMULATOR	
	size_t rval;

	if (s == NULL) {
		/* No support for state dependent encodings. */
		mbs = initial;
		return (0);
	}
#ifndef __SYMBIAN32__	
	rval = __mbrtowc(pwc, s, n, &mbs);
#else //__SYMBIAN32__
	rval = (size_t) mbrtowc(pwc, s, n, &mbs);
#endif //__SYMBIAN32__

	if (rval == (size_t)-1 || rval == (size_t)-2)
		return (-1);
	return ((int)rval);
}
