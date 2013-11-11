/*
 * Copyright (c) 1988, 1993
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
 * * Portions Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)tempnam.c	8.1 (Berkeley) 6/4/93";
#endif /* LIBC_SCCS and not lint */
#include <sys/cdefs.h>
#ifndef __SYMBIAN32__
__FBSDID("$FreeBSD: src/lib/libc/stdio/tempnam.c,v 1.10 2002/03/22 21:53:04 obrien Exp $");
#endif
#include <sys/param.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <paths.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifndef __SYMBIAN32__
__warn_references(tempnam,
    "warning: tempnam() possibly used unsafely; consider using mkstemp()");
#endif

#ifndef __SYMBIAN32__
extern char *_mktemp(char *);
#else
extern char *_mktemp(char *, int);
#endif//

EXPORT_C char *
tempnam(const char *dir, const char *pfx)	
{
	int sverrno;
	char *f, *name;

	if (!(name = malloc(MAXPATHLEN)))
		return(NULL);

	if (!pfx)
		pfx = "tmp.";
#ifndef __SYMBIAN32__
	//this section of code is moved below
	//As per standards the check should be on user specified
	//directory first
	if (issetugid() == 0 && (f = getenv("TMPDIR"))) {
		(void)snprintf(name, MAXPATHLEN, "%s%s%sXXXXXX", f,
		    *(f + strlen(f) - 1) == '/'? "": "/", pfx);
		if ((f = _mktemp(name)))
			return(f);
	}
#endif //__SYMBIAN32__
	if ((f = (char *)dir)) {
		#ifndef __SYMBIAN32__
		(void)snprintf(name, MAXPATHLEN, "%s%s%sXXXXXX", f,
		    *(f + strlen(f) - 1) == '/'? "": "/", pfx);
			if ((f = _mktemp(name)))
		#else
		(void)snprintf(name, MAXPATHLEN, "%s%s%sXXXXXX", f,
		    *(f + strlen(f) - 1) == '\\'? "": "\\", pfx);
		    if((f = _mktemp(name, 0)))
		#endif //__SYMBIAN32__
			return(f);
		}

	f = P_tmpdir;
#ifndef __SYMBIAN32__
	(void)snprintf(name, MAXPATHLEN, "%s%sXXXXXX", f, pfx);
	if ((f = _mktemp(name)))
#else
	(void)snprintf(name, MAXPATHLEN, "%s%s%sXXXXXX", f,
		    *(f + strlen(f) - 1) == '\\'? "": "\\", pfx);
	if((f = _mktemp(name, O_TMPFILE)))
#endif //__SYMBIAN32__
	return(f);
		
#ifdef __SYMBIAN32__
	if (issetugid() == 0 && (f = getenv("TMPDIR"))) {
		(void)snprintf(name, MAXPATHLEN, "%s%s%sXXXXXX", f,
		    *(f + strlen(f) - 1) == '/'? "": "/", pfx);
		#ifndef __SYMBIAN32__
			if ((f = _mktemp(name)))
		#else
			if((f = _mktemp(name, O_TMPFILE)))
		#endif	//__SYMBIAN32__
			return(f);
		
	}
#endif //__SYMBIAN32__

	f = _PATH_TMP;
	(void)snprintf(name, MAXPATHLEN, "%s%sXXXXXX", f, pfx);
		
		#ifndef __SYMBIAN32__
				if ((f = _mktemp(name)))
		#else
				if((f = _mktemp(name, O_TMPFILE)))
		#endif	//__SYMBIAN32__
				return(f);
		

	sverrno = errno;
	free(name);
	errno = sverrno;
	return(NULL);
}
