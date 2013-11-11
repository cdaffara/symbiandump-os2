/*-
 * © Portions copyright (c) 2006 Nokia Corporation.  All rights reserved.
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
 * © Portions copyright (c) 2005-2006  Nokia Corporation.  All rights reserved.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)refill.c	8.1 (Berkeley) 6/4/93";
#endif /* LIBC_SCCS and not lint */
#include <sys/cdefs.h>
__FBSDID("$FreeBSD: src/lib/libc/stdio/refill.c,v 1.18 2002/08/13 09:30:41 tjr Exp $");

#include "namespace.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "un-namespace.h"

#include "libc_private.h"
#include "local.h"
#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#include "libc_wsd_defs.h"
#endif

#ifdef EMULATOR
int *GET_WSD_VAR_NAME(__sdidinit, g)();
#define __sdidinit (*GET_WSD_VAR_NAME(__sdidinit, g)())
#endif //EMULATOR

static int lflush(FILE *);

static int
lflush(FILE *fp)
{
	int	ret = 0;

	if ((fp->_flags & (__SLBF|__SWR)) == (__SLBF|__SWR)) {
		FLOCKFILE(fp);
		ret = __sflush(fp);
		FUNLOCKFILE(fp);
	}
	return (ret);
}
/* Validation checks on the FILE
 * Avoid duplication of the code
 */
 int validate(FILE *fp)
	{
	/* make sure stdio is set up */
	if (!__sdidinit)
		__sinit();
	
	ORIENT(fp, -1);
	
	fp->_r = 0;		/* largely a convenience for callers */
	
	/* SysV does not make this test; take it out for compatibility */
	if (fp->_flags & __SEOF)
		return (EOF);
	
	/* if not already reading, have to be reading and writing */
	if ((fp->_flags & __SRD) == 0) {
		if ((fp->_flags & __SRW) == 0) {
			errno = EBADF;
			fp->_flags |= __SERR;
			return (EOF);
		}
		/* switch to reading */
		if (fp->_flags & __SWR) {
			if (__sflush(fp))
				return (EOF);
			fp->_flags &= ~__SWR;
			fp->_w = 0;
			fp->_lbfsize = 0;
		}
		fp->_flags |= __SRD;
	} else {
		/*
		 * We were reading.  If there is an ungetc buffer,
		 * we must have been reading from that.  Drop it,
		 * restoring the previous buffer (if any).  If there
		 * is anything in that buffer, return.
		 */
		if (HASUB(fp)) {
			FREEUB(fp);
			if ((fp->_r = fp->_ur) != 0) {
				fp->_p = fp->_extra->_up;
				return (0);
			}
		}
	}
	return 1;
}

/*
 * Refill a stdio buffer.
 * Return EOF on eof or error, 0 otherwise.
 */

int
__srefill(FILE *fp)
{
	int i = 0;
	int ret = validate(fp);
	if(ret != 1)
		return (ret);
		if (fp->_bf._base == NULL)
	{
		__smakebuf(fp);
		if(fp->_bf._base == NULL && errno == ENOMEM)
		{
			return (EOF);
		}
	}
	/*
	 * Before reading from a line buffered or unbuffered file,
	 * flush all line buffered output files, per the ANSI C
	 * standard.
	 */
	if (fp->_flags & (__SLBF|__SNBF)) {
		/* Ignore this file in _fwalk to avoid potential deadlock. */
		fp->_flags |= __SIGN;
		(void) _fwalk(lflush);
		fp->_flags &= ~__SIGN;

		/* Now flush this file without locking it. */
		if ((fp->_flags & (__SLBF|__SWR)) == (__SLBF|__SWR))
			__sflush(fp);
	}
	fp->_p = fp->_bf._base;

#ifdef __SYMBIAN32__
	if (fp->_file == 0)
	{
		int offset = 0;
		while(1)
		{
				i = 0;
				offset = _sread(fp, (char *)fp->_p, fp->_bf._size);	
				fp->_r += offset; 
				if(offset < 0)
					goto noinput;
				
				if(offset > 0)
				{				  
					//Check for back-space
					if(*fp->_p == 0x08)						
					{	
						// Check for back - space at first place
						if(fp->_r == 1)
						
							fp->_r -=1;					
						else		
							{
							fp->_r -=2;					
							fp->_p -=1;
							}
						continue;
					}
					
					//Check for new-line
					for (i = 0; i < offset; ++i)
						{
						if (*fp->_p == '\n')
							{
							break;
							}
						fp->_p++;
						}
					
					if (i != offset)
						{
						fp->_r -= (offset - i);	
						break;
						}
				}
				else
					break;
		}		
		fp->_p -= fp->_r;
		fp->_r += (offset - i);
	}	
	else
	{
		fp->_r = _sread(fp, (char *)fp->_p, fp->_bf._size);	
	}	
#else
	fp->_r = _sread(fp, (char *)fp->_p, fp->_bf._size);
#endif
noinput:
	fp->_flags &= ~__SMOD;	/* buffer contents are again pristine */
	if (fp->_r <= 0) {
		if (fp->_r == 0)
			fp->_flags |= __SEOF;
		else {
			fp->_r = 0;
			fp->_flags |= __SERR;
		}
		return (EOF);
	}
	return (0);
}

#ifdef __SYMBIAN32__
int __read_long(FILE *fp,char *p, size_t *resid)
{
		int w;
		int ret = validate(fp);
		if(ret != 1)
			return (ret);
		w = _sread(fp,p,*resid);
		p+=w;
		fp->_p = fp->_bf._base;
		if (w < *resid) 
		{
			if (w < 0)
				fp->_flags |= __SERR;
			else 
			{
				fp->_flags |= __SEOF;
			}
		*resid-=w;
		return (EOF);
		}	
		*resid-=w;
	return 0;
	}
#endif //if __SYMBIAN32__
