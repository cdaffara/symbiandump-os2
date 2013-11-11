/*-
 * Copyright (c) 1990, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Chris Torek.
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
 * * Portions Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)tmpnam.c	8.3 (Berkeley) 3/28/94";
#endif /* LIBC_SCCS and not lint */
#include <sys/cdefs.h>
__FBSDID("$FreeBSD: src/lib/libc/stdio/tmpnam.c,v 1.5 2002/03/22 21:53:04 obrien Exp $");

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#ifdef __SYMBIAN32__
#include <fcntl.h>
#endif//__SYMBIAN32__
#ifndef __SYMBIAN32__
__warn_references(tmpnam,
    "warning: tmpnam() possibly used unsafely; consider using mkstemp()");
#endif //__SYMBIAN32__

#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#include "libc_wsd_defs.h"
#endif

#ifdef EMULATOR

GET_STATIC_ARRAY_FROM_TLS(buf, char)
GET_STATIC_VAR_FROM_TLS(tmpcount, u_long)

#define buf (GET_WSD_VAR_NAME(buf, s)())
#define tmpcount (*GET_WSD_VAR_NAME(tmpcount, s)())
#endif //EMULATOR

#ifdef __SYMBIAN32__
extern char *_mktemp(char *, int);
#else//__SYMBIAN32__
extern char *_mktemp(char *);
#endif//__SYMBIAN32__

EXPORT_C
char *
tmpnam(s)
	char *s;
{
#ifndef EMULATOR	
	static u_long tmpcount;
	static char buf[L_tmpnam];
#endif //EMULATOR

if (s == NULL)
s = buf;
#ifndef __SYMBIAN32__
	(void)snprintf(s, L_tmpnam, "%stmp.%lu.XXXXXX", P_tmpdir, tmpcount);
#else
	(void)snprintf(s, L_tmpnam, "%s\\tmp.%lu.XXXXXX", P_tmpdir, tmpcount);
#endif//__SYMBIAN32__ 
++tmpcount;
#ifdef __SYMBIAN32__
	return (_mktemp(s, O_TMPFILE));
#else//__SYMBIAN32__
	return (_mktemp(s));
#endif//__SYMBIAN32__	
}
