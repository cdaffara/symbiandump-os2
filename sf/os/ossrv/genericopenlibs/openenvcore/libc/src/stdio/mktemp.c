/*
 * 
 * Copyright (c) 1987, 1993
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
static char sccsid[] = "@(#)mktemp.c	8.1 (Berkeley) 6/4/93";
#endif /* LIBC_SCCS and not lint */
#include <sys/cdefs.h>
__FBSDID("$FreeBSD: src/lib/libc/stdio/mktemp.c,v 1.28 2003/02/16 17:29:10 nectar Exp $");

#include "namespace.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "un-namespace.h"
#include <fcntl.h>
#include "common_def.h"


#ifndef __SYMBIAN32__
	char *_mktemp(char *);
	static int _gettemp(char *, int *, int, int);
#else
	// _mktempfile accepts an extra parameter for flag 
	// flag will be O_TMPFILE for a call by tmpfile and 0 for tempnam and mkstemp
	char *_mktemp(char *, int);
	static int _gettemp(char *, int *, int, int, int);
	int	_mktmpfile(char *, int);
#endif


static const unsigned char padchar[] =
"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

#ifdef __SYMBIAN_COMPILE_UNUSED__
int
mkstemps(path, slen)
	char *path;
	int slen;
{
	int fd;

	return (_gettemp(path, &fd, 0, slen) ? fd : -1);
}
#endif

EXPORT_C
int
mkstemp(path)
	char *path;
{
	int fd;
	#ifndef __SYMBIAN32__
		return (_gettemp(path, &fd, 0, 0) ? fd : -1);
	#else
		return (_gettemp(path, &fd, 0, 0, 0) ? fd : -1);
	#endif	
}

#ifdef __SYMBIAN_COMPILE_UNUSED__
char *
mkdtemp(path)
	char *path;
{
	return (_gettemp(path, (int *)NULL, 1, 0) ? path : (char *)NULL);
}
#endif


#ifndef __SYMBIAN32__
	char *
	_mktemp(path)
		char *path;
#else
	char *
	_mktemp(path, flag)
		char *path;
		int flag;
#endif	//__SYMBIAN32__
{	
	#ifndef __SYMBIAN32__	
		return (_gettemp(path, (int *)NULL, 0, 0) ? path : (char *)NULL);
	#else
		return (_gettemp(path, (int *)NULL, 0, 0, flag) ? path : (char *)NULL);
	#endif
}

#ifndef __SYMBIAN32__
__warn_references(mktemp,
    "warning: mktemp() possibly used unsafely; consider using mkstemp()");
#endif //__SYMBIAN32__

#ifdef __SYMBIAN_COMPILE_UNUSED__
char *
mktemp(path)
	char *path;
{
	return (_mktemp(path));
}
#endif


#ifdef __SYMBIAN32__
	int
	_mktmpfile(buf, flag)
	char *buf;
	int flag;
		{
		int fd;
		return (_gettemp(buf, &fd, 0, 0, flag) ? fd : -1);
		}

#endif //__SYMBIAN32__


#ifdef __SYMBIAN32__
static int
_gettemp(path, doopen, domkdir, slen, sflag)
	char *path;
	int *doopen;
	int domkdir;
	int slen;
	int sflag;
#else
_gettemp(path, doopen, domkdir, slen)
	char *path;
	int *doopen;
	int domkdir;
	int slen;
#endif //__SYMBIAN32__
{
	char *start, *trv, *suffp;
	char *pad;
	struct stat sbuf;
	int rval;
	uint32_t rand;

	#ifdef __SYMBIAN32__
	int count;
	size_t len = strlen(path);
	#endif //__SYMBIAN32__
	
	if (doopen != NULL && domkdir) {
		errno = EINVAL;
		return (0);
	}
	
#ifdef __SYMBIAN32__	
	// A single mkdir call introduced below instead of two separate 
	// calls from tempnam.c and tmpfile.c
	if (sflag)
		{		
		char *f = P_tmpdir;
				
		if (mkdir(f, 0666) == -1)
			{
			if (errno != EEXIST)
				{
				return -1;		//return from here	
				}
			}
		}
#endif//__SYMBIAN32__

		
	for (trv = path; *trv != '\0'; ++trv)
		;
	trv -= slen;
	suffp = trv;
	--trv;
	
	#ifdef __SYMBIAN32__
	if (trv < path || len < 6 || strcmp(&path[len - 6], "XXXXXX")) {
	#else
	if (trv < path) {
	#endif //__SYMBIAN32__
		errno = EINVAL;
		return (0);
	}
			
	/* Fill space with random characters */
	#ifndef __SYMBIAN32__
	while (trv >= path && *trv == 'X') {
	#else
	count = 0;
	while (trv >= path && count < 6) {
	#endif //__SYMBIAN32__
		rand = arc4random() % (sizeof(padchar) - 1);
		*trv-- = padchar[rand];
		#ifdef __SYMBIAN32__	
		count++;
		#endif //__SYMBIAN32__
	}
	start = trv + 1;

	

	
	/*
	 * check the target directory.
	 */
	if (doopen != NULL || domkdir) {
		for (; trv > path; --trv) {
			if (*trv == '/') {
				*trv = '\0';
				rval = stat(path, &sbuf);
				*trv = '/';
				if (rval != 0)
					return (0);
				if (!S_ISDIR(sbuf.st_mode)) {
					errno = ENOTDIR;
					return (0);
				}
				break;
			}
		}
	}

	for (;;) {
		if (doopen) {
		#ifndef __SYMBIAN32__
			if ((*doopen =
			    _open(path, O_CREAT|O_EXCL|O_RDWR, 0600)) >= 0)
		#else
			if ((*doopen =
			    _open(path, O_CREAT|O_EXCL|O_RDWR|sflag, 0600)) >= 0)
		#endif//__SYMBIAN32__
				return (1);
			if (errno != EEXIST)
				return (0);
		} else if (domkdir) {
			if (mkdir(path, 0700) == 0)
				return (1);
			if (errno != EEXIST)
				return (0);
		} else if (lstat(path, &sbuf))
			return (errno == ENOENT);

		/* If we have a collision, cycle through the space of filenames */
		for (trv = start;;) {
			if (*trv == '\0' || trv == suffp)
				return (0);
			
			#ifdef __SYMBIAN32__
			pad = strchr((const char *)padchar, *trv);
			#else
			pad = strchr(padchar, *trv);
			#endif //__SYMBIAN32__
			
			if (pad == NULL || *++pad == '\0')
				*trv++ = padchar[0];
			else {
				*trv++ = *pad;
				break;
			}
		}
	}
	/*NOTREACHED*/
}
