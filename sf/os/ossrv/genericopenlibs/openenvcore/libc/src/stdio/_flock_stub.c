/*
 *
 * Copyright (c) 1998 John Birrell <jb@cimlogic.com.au>.
 * All rights reserved.
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
 * 4. Neither the name of the author nor the names of any co-contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY JOHN BIRRELL AND CONTRIBUTORS ``AS IS'' AND
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
 * * Portions Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */

/*
 * POSIX stdio FILE locking functions. These assume that the locking
 * is only required at FILE structure level, not at file descriptor
 * level too.
 *
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD: src/lib/libc/stdio/_flock_stub.c,v 1.14 2004/03/09 04:51:58 jb Exp $");

#include "namespace.h"

#ifdef __SYMBIAN32__ 
/* undefine flockfile and funlockfile here so that they don't get 
   exported from IMPORT_C in stdio.h
*/
#undef flockfile
#undef funlockfile
#endif //__SYMBIAN32__ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "un-namespace.h"

#include "local.h"

#ifdef __SYMBIAN32__ 
/* declarations required as we 'undef'ed the definitions 
   that came from namespace.h
*/
void _flockfile(FILE *fp);
void _funlockfile(FILE *fp);
int _ftrylockfile(FILE *fp);
#endif //__SYMBIAN32__ 

/*
 * Weak symbols for externally visible functions in this file:
 */

#ifndef __SYMBIAN32__
__weak_reference(_flockfile, flockfile);
__weak_reference(_flockfile_debug_stub, _flockfile_debug);
__weak_reference(_ftrylockfile, ftrylockfile);
__weak_reference(_funlockfile, funlockfile);
#endif //__SYMBIAN32__

/*
 * We need to retain binary compatibility for a while.  So pretend
 * that _lock is part of FILE * even though it is dereferenced off
 * _extra now.  When we stop encoding the size of FILE into binaries
 * this can be changed in stdio.h.  This will reduce the amount of
 * code that has to change in the future (just remove this comment
 * and #define).
 */
#define _lock _extra

#ifdef __SYMBIAN32__

EXPORT_C
void flockfile(FILE *fp)
{
	_flockfile(fp);
}

EXPORT_C
void funlockfile(FILE *fp)
{
	_funlockfile(fp);
}

EXPORT_C
int ftrylockfile(FILE *fp)
{
	return _ftrylockfile(fp);
}

#endif //__SYMBIAN32__

void
_flockfile(FILE *fp)
{
	pthread_t curthread = _pthread_self();

	if (fp->_lock->fl_owner == curthread)
		fp->_lock->fl_count++;
	else {
		/*
		 * Make sure this mutex is treated as a private
		 * internal mutex:
		 */
		_pthread_mutex_lock(&fp->_lock->fl_mutex);
		fp->_lock->fl_owner = curthread;
		fp->_lock->fl_count = 1;
	}
}

/*
 * This can be overriden by the threads library if it is linked in.
 */
void
_flockfile_debug_stub(FILE *fp/*, char *fname, int lineno*/)
{
	_flockfile(fp);
}

int
_ftrylockfile(FILE *fp)
{
	pthread_t curthread = _pthread_self();
	int	ret = 0;

	if (fp->_lock->fl_owner == curthread)
		fp->_lock->fl_count++;
	/*
	 * Make sure this mutex is treated as a private
	 * internal mutex:
	 */
	else if (_pthread_mutex_trylock(&fp->_lock->fl_mutex) == 0) {
		fp->_lock->fl_owner = curthread;
		fp->_lock->fl_count = 1;
	}
	else
		ret = -1;
	return (ret);
}

void 
_funlockfile(FILE *fp)
{
	pthread_t	curthread = _pthread_self();

	/*
	 * Check if this file is owned by the current thread:
	 */
	if (fp->_lock->fl_owner == curthread) {
		/*
		 * Check if this thread has locked the FILE
		 * more than once:
		 */
		if (fp->_lock->fl_count > 1)
			/*
			 * Decrement the count of the number of
			 * times the running thread has locked this
			 * file:
			 */
			fp->_lock->fl_count--;
		else {
			/*
			 * The running thread will release the
			 * lock now:
			 */
			fp->_lock->fl_count = 0;
#ifndef __SYMBIAN32__			
			fp->_lock->fl_owner = NULL;
#else //__SYMBIAN32__
			fp->_lock->fl_owner = 0;
#endif //__SYMBIAN32__
			_pthread_mutex_unlock(&fp->_lock->fl_mutex);
		}
	}
}
