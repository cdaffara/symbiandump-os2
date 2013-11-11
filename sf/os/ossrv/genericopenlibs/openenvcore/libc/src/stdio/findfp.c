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
 * © * Portions Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)findfp.c	8.2 (Berkeley) 1/4/94";
#endif /* LIBC_SCCS and not lint */
#include <sys/cdefs.h>
__FBSDID("$FreeBSD: src/lib/libc/stdio/findfp.c,v 1.29 2004/05/22 15:19:41 tjr Exp $");

#include <sys/param.h>
#ifndef __SYMBIAN32__
#include <machine/atomic.h>
#endif //__SYMBIAN32__
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sysreent.h"

#ifdef __SYMBIAN32__
#include <errno.h>
#endif

#ifndef __SYMBIAN32__
#include <spinlock.h>
#endif //__SYMBIAN32__

#include "libc_private.h"
#include "local.h"
#include "glue.h"

#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#include "libc_wsd_defs.h"
#endif

#ifndef EMULATOR

int	__sdidinit;
#else //EMULATOR

GET_GLOBAL_VAR_FROM_TLS(__sdidinit, int)
#define __sdidinit (*GET_WSD_VAR_NAME(__sdidinit, g)())

#define __cleanup (GetGlobals()->_s___cleanup)
#endif //EMULATOR

#define	NDYNAMIC 10		/* add ten more whenever necessary */

#define	std(flags, file) \
  	{0,0,0,flags,file,{0},0,__sF+file,__sclose,__sread,__sseek,__swrite, \
	 {0}, __sFX + file}
  /*	 p r w flags file _bf z  cookie      close    read    seek    write */
  /*     _ub _extra */
				/* the usual - (stdin + stdout + stderr) */
#ifndef EMULATOR  
static FILE usual[OPEN_MAX - 3];
static struct __sFILEX usual_extra[OPEN_MAX - 3];
static struct glue uglue = { NULL, OPEN_MAX, usual };

static struct __sFILEX __sFX[3] = {{NULL, PTHREAD_MUTEX_INITIALIZER, 0, 0, 0, 0 }, {NULL, PTHREAD_MUTEX_INITIALIZER, 0, 0, 0, 0}, {NULL, PTHREAD_MUTEX_INITIALIZER, 0, 0, 0, 0}};
#else //EMULATOR

GET_STATIC_ARRAY_FROM_TLS(usual, FILE)
GET_STATIC_ARRAY_FROM_TLS(usual_extra,struct  __sFILEX)
GET_STATIC_VAR_FROM_TLS(uglue, struct glue)
GET_STATIC_ARRAY_FROM_TLS(__sFX, struct __sFILEX)

#define usual (GET_WSD_VAR_NAME(usual, s)())
#define usual_extra (GET_WSD_VAR_NAME(usual_extra, s)())
#define uglue (*GET_WSD_VAR_NAME(uglue, s)())
#define __sFX (GET_WSD_VAR_NAME(__sFX, g)())
#endif //EMULATOR
/*
 * We can't make this 'static' until 6.0-current due to binary
 * compatibility concerns.  This also means we cannot change the
 * sizeof(FILE) until that time either and must continue to use the
 * __sFILEX stuff to add to FILE.
 */
#ifndef EMULATOR 

FILE __sF[3] = {
	std(__SRD, STDIN_FILENO),
	std(__SWR, STDOUT_FILENO),
	std(__SWR|__SNBF, STDERR_FILENO)
};
#else //EMULATOR

GET_GLOBAL_ARRAY_FROM_TLS(__sF, FILE)
#define __sF (GET_WSD_VAR_NAME(__sF, g)())
#endif //EMULATOR
/*
 * The following kludge is done to ensure enough binary compatibility
 * with future versions of libc.  Or rather it allows us to work with
 * libraries that have been built with a newer libc that defines these
 * symbols and expects libc to provide them.  We only have need to support
 * i386 and alpha because they are the only "old" systems we have deployed.
 */
#ifndef EMULATOR 

FILE *__stdinp = &__sF[0];
FILE *__stdoutp = &__sF[1];
FILE *__stderrp = &__sF[2];
#else //EMULATOR

#undef __stdinp
#undef __stdoutp
#undef __stderrp

GET_GLOBAL_VAR_FROM_TLS(__stdinp, FILE *)
GET_GLOBAL_VAR_FROM_TLS(__stdoutp, FILE *)
GET_GLOBAL_VAR_FROM_TLS(__stderrp, FILE *)

#define __stdinp   (*GET_WSD_VAR_NAME(__stdinp, g)())
#define __stdoutp (*GET_WSD_VAR_NAME(__stdoutp, g)())
#define __stderrp (*GET_WSD_VAR_NAME(__stderrp, g)())
#endif //EMULATOR

#ifndef EMULATOR

struct glue __sglue = { &uglue, 3, __sF };
static struct glue *lastglue = &uglue;
#else //EMULATOR

GET_GLOBAL_VAR_FROM_TLS(__sglue, struct glue)
GET_STATIC_VAR_FROM_TLS(lastglue, struct glue *)

#define __sglue (*GET_WSD_VAR_NAME(__sglue, g)())
#define lastglue (*GET_WSD_VAR_NAME(lastglue, s)())
#endif //EMULATOR

static struct glue *	moreglue(int);

#ifndef __SYMBIAN32__
static spinlock_t thread_lock = _SPINLOCK_INITIALIZER;
#define THREAD_LOCK()	if (__isthreaded) _SPINLOCK(&thread_lock)
#define THREAD_UNLOCK()	if (__isthreaded) _SPINUNLOCK(&thread_lock)
#endif //__SYMBIAN32__

#ifdef __SYMBIAN32__
#define THREAD_LOCK()
#define THREAD_UNLOCK()
#endif //__SYMBIAN32__


#if NOT_YET
#define	SET_GLUE_PTR(ptr, val)	atomic_set_rel_ptr(&(ptr), (uintptr_t)(val))
#else
#define	SET_GLUE_PTR(ptr, val)	ptr = val
#endif

#ifdef EMULATOR

GET_STATIC_VAR_FROM_TLS(empty, FILE)
GET_STATIC_VAR_FROM_TLS(emptyx, struct __sFILEX)

#define empty (*GET_WSD_VAR_NAME(empty, s)())
#define emptyx (*GET_WSD_VAR_NAME(emptyx, s)())
#endif //EMULATOR

#ifdef EMULATOR
GET_GLOBAL_VAR_FROM_TLS(tmpdirptr, char *)
#else 
char *__tmpdirptr;
#endif
static struct glue *
moreglue(n)
	int n;
{
	struct glue *g;
#ifndef EMULATOR
	static FILE empty;
	static struct __sFILEX emptyx;
#endif //EMUALTOR
	FILE *p;
	struct __sFILEX *fx;

	#ifdef __SYMBIAN32__
	g = (struct glue *)BackendAlloc(sizeof(*g) + ALIGNBYTES + n * sizeof(FILE) +
	    n * sizeof(struct __sFILEX));
	#else
	g = (struct glue *)malloc(sizeof(*g) + ALIGNBYTES + n * sizeof(FILE) +
	    n * sizeof(struct __sFILEX));
	#endif

	if (g == NULL)
		return (NULL);
	p = (FILE *)ALIGN(g + 1);
	fx = (struct __sFILEX *)&p[n];
	g->next = NULL;
	g->niobs = n;
	g->iobs = p;
	while (--n >= 0) {
		*p = empty;
		p->_extra = fx;
		*p->_extra = emptyx;
		p++, fx++;
	}
	return (g);
}

/*
 * Find a free FILE for fopen et al.
 */
FILE *
__sfp()
{
	FILE	*fp;
	int	n;
	struct glue *g;

	if (!__sdidinit)
		__sinit();
	/*
	 * The list must be locked because a FILE may be updated.
	 */
	THREAD_LOCK();
	for (g = __sglue.next; g != NULL; g = g->next) {
		#ifdef __SYMBIAN32__
		for (fp = g->iobs, n = g->niobs; --n >= 3; fp++)
		#else
		for (fp = g->iobs, n = g->niobs; --n >= 0; fp++)
		#endif
			if (fp->_flags == 0)
				goto found;
	}

	THREAD_UNLOCK();	/* don't hold lock while BackendAlloc()ing. */

	#ifdef __SYMBIAN32__
	//for checking for maximum numbers of file structures that are supported.
	if(n==2) 
		{
			errno=EMFILE;
			return NULL;
		}
	#endif
	
	if ((g = moreglue(NDYNAMIC)) == NULL)
		return (NULL);
	THREAD_LOCK();		/* reacquire the lock */
	SET_GLUE_PTR(lastglue->next, g); /* atomically append glue to list */
	lastglue = g;		/* not atomic; only accessed when locked */
	fp = g->iobs;
found:
	fp->_flags = 1;		/* reserve this slot; caller sets real flags */
	THREAD_UNLOCK();
	fp->_p = NULL;		/* no current pointer */
	fp->_w = 0;		/* nothing to read or write */
	fp->_r = 0;
	fp->_bf._base = NULL;	/* no buffer */
	fp->_bf._size = 0;
	fp->_lbfsize = 0;	/* not line buffered */
	fp->_file = -1;		/* no file */
/*	fp->_cookie = <any>; */	/* caller sets cookie, _read/_write etc */
	fp->_ub._base = NULL;	/* no ungetc buffer */
	fp->_ub._size = 0;
	fp->_lb._base = NULL;	/* no line buffer */
	fp->_lb._size = 0;
/*	fp->_lock = NULL; */	/* once set always set (reused) */
#ifndef __SYMBIAN32__
	fp->_extra->orientation = 0;
	memset(&fp->_extra->mbstate, 0, sizeof(mbstate_t));
#else	
	INITEXTRA(fp)
#endif
	return (fp);
}

#ifndef __SYMBIAN32__
/*
 * XXX.  Force immediate allocation of internal memory.  Not used by stdio,
 * but documented historically for certain applications.  Bad applications.
 */
__warn_references(f_prealloc, 
	"warning: this program uses f_prealloc(), which is not recommended.");

void
f_prealloc()
{
	struct glue *g;
	int n;

	n = getdtablesize() - OPEN_MAX + 20;		/* 20 for slop. */
	/*
	 * It should be safe to walk the list without locking it;
	 * new nodes are only added to the end and none are ever
	 * removed.
	 */
	for (g = &__sglue; (n -= g->niobs) > 0 && g->next; g = g->next)
		/* void */;
	if ((n > 0) && ((g = moreglue(n)) != NULL)) {
		THREAD_LOCK();
		SET_GLUE_PTR(lastglue->next, g);
		lastglue = g;
		THREAD_UNLOCK();
	}
}
#endif //__SYMBIAN32__
/*
 * exit() calls _cleanup() through *__cleanup, set whenever we
 * open or buffer a file.  This chicanery is done so that programs
 * that do not use stdio need not link it all in.
 *
 * The name `_cleanup' is, alas, fairly well known outside stdio.
 */
typedef int (*functionPtr)(FILE *);
void
_cleanup()
{
	(void) _fwalk(__sflush);		/* `cheating' */
	(void) _fwalk((functionPtr)__sclose);
}

/*
 * __sinit() is called whenever stdio's internal variables must be set up.
 */
void
__sinit()
{
	int	i;

	THREAD_LOCK();
	if (__sdidinit == 0) {
		/* Set _extra for the usual suspects. */
		for (i = 0; i < OPEN_MAX - 3; i++)
			usual[i]._extra = &usual_extra[i];

		/* Make sure we clean up on exit. */
		__cleanup = _cleanup;		/* conservative */
		__sdidinit = 1;
	}
	THREAD_UNLOCK();
}

#ifdef __SYMBIAN32__
/**
Function interface for stdin
*/

EXPORT_C FILE *__stdin (void)
{
	return ( __stdinp);
}
void GetTmpDirName(void);
EXPORT_C char *tmpdirname(void)
	{
	if(!__tmpdirptr)
		{
		GetTmpDirName();
		}
	
	return ( __tmpdirptr);
}

/**
Function interface for stdout
*/
EXPORT_C FILE *__stdout (void)
{
	return (__stdoutp);
}

/**
Function interface for stderr
**/
EXPORT_C FILE *__stderr (void)
{
	return (__stderrp);
}
#endif //__SYMBIAN32__

#ifdef __SYMBIAN32__
/*
 *It searches the glue to find the file stream attached with the descriptor 
 *and if found returns the file stream else NULL is returned.
 */

FILE * __sfind(int fd)
	{
	FILE *fp;
	int	n;
	struct glue *g;
	if (!__sdidinit)
		__sinit();
		
   /*
	* The list must be locked because a FILE may be updated.
	*/

	THREAD_LOCK();
	for (g = __sglue.next; g != NULL; g = g->next) {
	#ifdef __SYMBIAN32__
	for (fp = g->iobs, n = g->niobs; --n >= 3; fp++)
	#else
	for (fp = g->iobs, n = g->niobs; --n >= 0; fp++)
	#endif
	if (fp->_file == fd)
		{
		THREAD_UNLOCK();
		return (fp);
		}
	}
	THREAD_UNLOCK();
	return NULL;
	}
#endif //__SYMBIAN32__
