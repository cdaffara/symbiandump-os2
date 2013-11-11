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
 * * Portions Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)tmpfile.c	8.1 (Berkeley) 6/4/93";
#endif /* LIBC_SCCS and not lint */
#include <sys/cdefs.h>
__FBSDID("$FreeBSD: src/lib/libc/stdio/tmpfile.c,v 1.9 2003/02/06 01:08:19 mtm Exp $");

#include "namespace.h"
#include <sys/types.h>

#ifndef __SYMBIAN32__
#include <signal.h>
#endif //__SYMBIAN32__

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <paths.h>
#include "un-namespace.h"
#include <unistd.h>
#include <fcntl.h>
#include "common_def.h"
#ifdef __SYMBIAN32__
#include <sys/stat.h>
#include "sysreent.h"
#endif //__SYMBIAN32__

#ifdef __SYMBIAN32__
	extern int	_mktmpfile(char *, int);
#endif	

EXPORT_C
FILE *
tmpfile()
{
	#ifndef __SYMBIAN32__
	sigset_t set, oset;
	#endif //__SYMBIAN32__
	
	FILE *fp;
	int fd, sverrno;
#define	TRAILER	"tmp.XXXXXX"
	char *buf;
	const char *tmpdir;		
	tmpdir = NULL;

	if (issetugid() == 0)
		tmpdir = getenv("TMPDIR");
	
	if (tmpdir == NULL)
		tmpdir = _PATH_TMP;
#ifdef __SYMBIAN32__
	(void)asprintf(&buf, "%s%s%s", tmpdir,
	    (tmpdir[strlen(tmpdir) - 1] == '\\') ? "" : "\\", TRAILER);
#else
	(void)asprintf(&buf, "%s%s%s", tmpdir,
	    (tmpdir[strlen(tmpdir) - 1] == '/') ? "" : "/", TRAILER);
#endif//__SYMBIAN32__	    
	if (buf == NULL)
		return (NULL);
	
	#ifndef __SYMBIAN32__
	sigfillset(&set);
	(void)_sigprocmask(SIG_BLOCK, &set, &oset);
	#endif //__SYMBIAN32__

#ifndef __SYMBIAN32__
	fd = mkstemp(buf);
#else
	fd = _mktmpfile(buf, O_TMPFILE);
#endif
	
	if (fd != -1)
		(void)unlink(buf);
	free(buf);
	#ifndef __SYMBIAN32__
	(void)_sigprocmask(SIG_SETMASK, &oset, NULL);
	#endif //__SYMBIAN32__
	
	if (fd == -1)
		return (NULL);

	if ((fp = fdopen(fd, "w+")) == NULL) {
		sverrno = errno;
		(void)_close(fd);
		errno = sverrno;
		return (NULL);
	}
	return (fp);
}
