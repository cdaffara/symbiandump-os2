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
static char sccsid[] = "@(#)fopen.c	8.1 (Berkeley) 6/4/93";
#endif /* LIBC_SCCS and not lint */
#include <sys/cdefs.h>
#ifndef __SYMBIAN32__
__FBSDID("$FreeBSD: src/lib/libc/stdio/fopen.c,v 1.10 2002/10/12 16:13:37 mike Exp $");
#endif
#include "namespace.h"
#include <sys/types.h>
#ifndef __SYMBIAN32__
#include <sys/stat.h>
#else
#define	DEFFILEMODE	1 //(S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)
#endif
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <wchar.h>
#include "un-namespace.h"
#include <fcntl.h>
#include "local.h"
#include "common_def.h"

EXPORT_C FILE *
fopen(const char * __restrict file,	const char * __restrict mode)	
{
	FILE *fp;
	int f;
	int flags, oflags;

	if ((flags = __sflags(mode, &oflags)) == 0)
		return (NULL);
	if ((fp = __sfp()) == NULL)
		return (NULL);
	if ((f = open(file, oflags, DEFFILEMODE)) < 0) {
		fp->_flags = 0;			/* release */
		return (NULL);
	}
	fp->_file = f;
	fp->_flags = flags;
	fp->_cookie = fp;
	fp->_read = __sread;
	fp->_write = __swrite;
	fp->_seek = __sseek;
	fp->_close = __sclose;

#ifdef __SYMBIAN32__    
    if (oflags & O_TEXT) {
        fp->_extra->fmode = 't';
        /* In text mode, create a translation buffer */
        fp->_extra->nl_tbuf = BackendAlloc(BUFSIZ);
        if(!fp->_extra->nl_tbuf) {
            errno = ENOMEM;
            fp->_flags = 0;     /* release */
            return (NULL);
            }
    }
#endif
	
	/*
	 * When opening in append mode, even though we use O_APPEND,
	 * we need to seek to the end so that ftell() gets the right
	 * answer.  If the user then alters the seek pointer, or
	 * the file extends, this will fail, but there is not much
	 * we can do about this.  (We could set __SAPP and check in
	 * fseek and ftell.)
	 */
	if (oflags & O_APPEND)
	{
		(void)_sseek(fp, (fpos_t)0, SEEK_END);
#ifdef __SYMBIAN32__
		fp->_flags |= __SAPP;
#endif
	}
	return (fp);
}


EXPORT_C FILE *
wfopen(const wchar_t * __restrict file,	const wchar_t * __restrict mode)	
{
	FILE *fp;
	int f;
	int flags, oflags;
	char destmode[4];
	size_t siz;

	destmode[0] = '\0';

	siz = wcstombs(destmode, mode, 4);

	if ((flags = __sflags(destmode, &oflags)) == 0)
		return (NULL);
	if ((fp = __sfp()) == NULL)
		return (NULL);
	if ((f = wopen(file, oflags, DEFFILEMODE)) < 0) {
		fp->_flags = 0;			/* release */
		return (NULL);
	}
	fp->_file = f;
	fp->_flags = flags;
	fp->_cookie = fp;
	fp->_read = __sread;
	fp->_write = __swrite;
	fp->_seek = __sseek;
	fp->_close = __sclose;
#ifdef __SYMBIAN32__    
    if (oflags & O_TEXT) {
        fp->_extra->fmode = 't';
        /* In text mode, create a translation buffer */
        fp->_extra->nl_tbuf = BackendAlloc(BUFSIZ);
        if(!fp->_extra->nl_tbuf) {
            errno = ENOMEM;
            fp->_flags = 0;     /* release */
            return (NULL);
            }
    }
#endif
	/*
	 * When opening in append mode, even though we use O_APPEND,
	 * we need to seek to the end so that ftell() gets the right
	 * answer.  If the user then alters the seek pointer, or
	 * the file extends, this will fail, but there is not much
	 * we can do about this.  (We could set __SAPP and check in
	 * fseek and ftell.)
	 */
	if (oflags & O_APPEND)
	{
		(void)_sseek(fp, (fpos_t)0, SEEK_END);
#ifdef __SYMBIAN32__
		fp->_flags |= __SAPP;
#endif
	}
	return (fp);
}

