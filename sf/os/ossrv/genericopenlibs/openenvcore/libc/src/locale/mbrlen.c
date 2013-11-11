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
__FBSDID("$FreeBSD: src/lib/libc/locale/mbrlen.c,v 1.4 2004/05/12 14:26:54 tjr Exp $");

#include <wchar.h>
#ifndef __SYMBIAN32__
#include "mblocal.h"
#endif //__SYMBIAN32__

#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#include "libc_wsd_defs.h"
#endif 

#ifdef EMULATOR

GET_STATIC_VAR_FROM_TLS(mbrlen_mbs, mbstate_t)
#define mbs (*GET_WSD_VAR_NAME(mbrlen_mbs, s)())
#endif //EMULATOR

EXPORT_C size_t
mbrlen(const char * __restrict s, size_t n, mbstate_t * __restrict ps)
{
#ifndef EMULATOR	
	static mbstate_t mbs;
#endif //EMULATOR	

	if (ps == NULL)
		ps = &mbs;
#ifndef __SYMBIAN32__	
	return (__mbrtowc(NULL, s, n, ps));
#else //__SYMBIAN32__
	return (mbrtowc(NULL, s, n, ps));
#endif //__SYMBIAN32__
}
