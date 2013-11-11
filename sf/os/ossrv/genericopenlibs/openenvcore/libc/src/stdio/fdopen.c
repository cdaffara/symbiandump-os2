/*-
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Chris Torek.
 * 
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
 * © * Portions Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)fdopen.c	8.1 (Berkeley) 6/4/93";
#endif /* LIBC_SCCS and not lint */
#include <sys/cdefs.h>
#ifndef __SYMBIAN32__
__FBSDID("$FreeBSD: src/lib/libc/stdio/fdopen.c,v 1.7 2002/03/22 21:53:04 obrien Exp $");
#endif
#include "namespace.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include "un-namespace.h"
#include <fcntl.h>
#include "local.h"
#include "common_def.h"


#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))

#include "libc_wsd_defs.h"
GET_STATIC_VAR_FROM_TLS(nofile, int)
#define nofile (*GET_WSD_VAR_NAME(nofile, s)())

#endif //EMULATOR


EXPORT_C
FILE *
fdopen(int fd, const char *mode)	
{
	FILE *fp;
	FILE *fp1;
#ifndef EMULATOR	
	static int nofile;
#endif //EMULATOR
	int flags, oflags, fdflags, tmp;

	if (nofile == 0)
		nofile = getdtablesize();

	if ((flags = __sflags(mode, &oflags)) == 0)
		return (NULL);

	/* Make sure the mode the user wants is a subset of the actual mode. */
	if ((fdflags = _fcntl(fd, F_GETFL, 0)) < 0)
		return (NULL);
	tmp = fdflags & O_ACCMODE;
	if (tmp != O_RDWR && (tmp != (oflags & O_ACCMODE))) {
		errno = EINVAL;
		return (NULL);
	}
	if ((fp = __sfp()) == NULL)
		return (NULL);
	fp->_flags = flags;
	
	#ifdef __SYMBIAN32__
	if(( fp1 = __sfind(fd)) != NULL)
		{
		fp->_offset = fp1->_offset;
		fp->_r = fp1->_r;
		fp->_w = fp1->_w;
		}
	if(fp->_flags & __SOFF)
		{
		fp->_flags |= __SOFF;
		}
	#endif //__SYMBIAN32__
	/*
	 * If opened for appending, but underlying descriptor does not have
	 * O_APPEND bit set, assert __SAPP so that __swrite() caller
	 * will _sseek() to the end before write.
	 */
	if ((oflags & O_APPEND) && !(fdflags & O_APPEND))
	fp->_flags |= __SAPP;
	fp->_file = fd;
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
	return (fp);
}
