/*-
 * © Portions copyright (c) 2006 Nokia Corporation.  All rights reserved.
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
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD: src/lib/libc/locale/wcsnrtombs.c,v 1.3 2005/02/12 08:45:12 stefanf Exp $");

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#ifndef __SYMBIAN32__
#include "mblocal.h"
#endif //__SYMBIAN32__

#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#include "libc_wsd_defs.h"
#endif 

#ifdef EMULATOR

GET_STATIC_VAR_FROM_TLS(wcsnrtombs_mbs, mbstate_t)
#define mbs (*GET_WSD_VAR_NAME(wcsnrtombs_mbs, s)())
#endif //EMULATOR

#ifdef __SYMBIAN32__
size_t __wcsnrtombs_std(char * __restrict dst, const wchar_t ** __restrict src,
    size_t nwc, size_t len, mbstate_t * __restrict ps);
#endif //__SYMBIAN32__

EXPORT_C size_t
wcsnrtombs(char * __restrict dst, const wchar_t ** __restrict src, size_t nwc,
    size_t len, mbstate_t * __restrict ps)
{
#ifndef EMULATOR
	static mbstate_t mbs;
#endif //EMULATOR	

	if (ps == NULL)
		ps = &mbs;
#ifndef __SYMBIAN32__	
	return (__wcsnrtombs(dst, src, nwc, len, ps));
#else //__SYMBIAN32__
	return (__wcsnrtombs_std(dst, src, nwc, len, ps));
#endif //__SYMBIAN32__
}

size_t
__wcsnrtombs_std(char * __restrict dst, const wchar_t ** __restrict src,
    size_t nwc, size_t len, mbstate_t * __restrict ps)
{
	mbstate_t mbsbak;
	char buf[MB_LEN_MAX];
	const wchar_t *s;
	size_t nbytes;
	size_t nb;

#ifdef __SYMBIAN32__
	if(src == NULL)
	{
		return (0);
	}
#endif //__SYMBIAN32__

	s = *src;
	nbytes = 0;

	if (dst == NULL) {
		while (nwc-- > 0) {
#ifndef __SYMBIAN32__			
			if ((nb = __wcrtomb(buf, *s, ps)) == (size_t)-1)
#else //__SYMBIAN32__
			if ((nb = wcrtomb(buf, *s, ps)) == (size_t)-1)
#endif //__SYMBIAN32__
				/* Invalid character - wcrtomb() sets errno. */
				return ((size_t)-1);
			else if (*s == L'\0')
				{
				*src = NULL;
				return (nbytes + nb - 1);
				}
			s++;
			nbytes += nb;
		}
		return (nbytes);
	}

	while (len > 0 && nwc-- > 0) {
		if (len > (size_t)MB_CUR_MAX) {
			/* Enough space to translate in-place. */
#ifndef __SYMBIAN32__			
			if ((nb = __wcrtomb(dst, *s, ps)) == (size_t)-1) {
#else //__SYMBIAN32__
			if ((nb = wcrtomb(dst, *s, ps)) == (size_t)-1) {
#endif //__SYMBIAN32__
				*src = s;
				return ((size_t)-1);
			}
		} else {
			/*
			 * May not be enough space; use temp. buffer.
			 *
			 * We need to save a copy of the conversion state
			 * here so we can restore it if the multibyte
			 * character is too long for the buffer.
			 */
			mbsbak = *ps;
#ifndef __SYMBIAN32__			
			if ((nb = __wcrtomb(buf, *s, ps)) == (size_t)-1) {
#else //__SYMBIAN32__
			if ((nb = wcrtomb(buf, *s, ps)) == (size_t)-1) {
#endif //__SYMBIAN32__
				*src = s;
				return ((size_t)-1);
			}
			if (nb > (int)len) {
				/* MB sequence for character won't fit. */
				*ps = mbsbak;
				break;
			}
			memcpy(dst, buf, nb);
		}
		if (*s == L'\0') {
			*src = NULL;
			return (nbytes + nb - 1);
		}
		s++;
		dst += nb;
		len -= nb;
		nbytes += nb;
	}
	*src = s;
	return (nbytes);
}
