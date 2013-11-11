/*-
 * 
 * Copyright (c) 1990, 1993
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
 * * Portions Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)gets.c	8.1 (Berkeley) 6/4/93";
#endif /* LIBC_SCCS and not lint */
#include <sys/cdefs.h>
#ifndef __SYMBIAN32__ 
__FBSDID("$FreeBSD: src/lib/libc/stdio/gets.c,v 1.16 2003/01/30 23:32:53 tjr Exp $");
#endif

#include "namespace.h"
#include <stdio.h>
#include <sys/cdefs.h>
#include "un-namespace.h"
#include <unistd.h>
#include "libc_private.h"
#include "local.h"
#include "common_def.h"

#ifndef __SYMBIAN32__ 
__warn_references(gets, "warning: this program uses gets(), which is unsafe.");
#endif

#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#include "libc_wsd_defs.h"
#endif

#ifdef EMULATOR
GET_STATIC_VAR_FROM_TLS(warned, int)
GET_STATIC_ARRAY_FROM_TLS(w, char)

#define warned (*GET_WSD_VAR_NAME(warned, s)())
#define w (GET_WSD_VAR_NAME(w, s)())
#define WARNING_STR_SIZE 53
#endif //EMULATOR

EXPORT_C
char *
gets(buf)
	char *buf;
{
	int c;
	char *s;
#ifndef EMULATOR
	static int warned;
	static char w[] =
	    "warning: this program uses gets(), which is unsafe.\n";
#endif //EMULATOR
	FLOCKFILE(stdin);
	ORIENT(stdin, -1);
	if (!warned) {
#ifndef EMULATOR		
		(void) _write(STDERR_FILENO, w, sizeof(w) - 1);
#else		
		(void) _write(STDERR_FILENO, w, WARNING_STR_SIZE);
#endif		
		warned = 1;
	}
	for (s = buf; (c = __sgetc(stdin)) != '\n';)
		if (c == EOF)
			if (s == buf) {
				FUNLOCKFILE(stdin);
				return (NULL);
			} else
				break;
		else
			*s++ = (char) c;
	*s = 0;
	FUNLOCKFILE(stdin);
	return (buf);
}
