/*-
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
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)wsetup.c	8.1 (Berkeley) 6/4/93";
#endif /* LIBC_SCCS and not lint */
#include <sys/cdefs.h>
__FBSDID("$FreeBSD: src/lib/libc/stdio/wsetup.c,v 1.9 2004/06/08 05:44:52 das Exp $");

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "local.h"
#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#include "libc_wsd_defs.h"
#endif

#ifdef EMULATOR
int *GET_WSD_VAR_NAME(__sdidinit, g)();
#define __sdidinit (*GET_WSD_VAR_NAME(__sdidinit, g)())
#endif //EMULATOR

/*
 * Various output routines call wsetup to be sure it is safe to write,
 * because either _flags does not include __SWR, or _buf is NULL.
 * _wsetup returns 0 if OK to write; otherwise, it returns EOF and sets errno.
 */
int
__swsetup(fp)
	FILE *fp;
{
	/* make sure stdio is set up */
	if (!__sdidinit)
		__sinit();

	/*
	 * If we are not writing, we had better be reading and writing.
	 */
	if ((fp->_flags & __SWR) == 0) {
		if ((fp->_flags & __SRW) == 0) {
			errno = EBADF;
#ifdef __SYMBIAN32__
			fp->_flags |= __SERR;
#endif			
			return (EOF);
		}
		if (fp->_flags & __SRD) {
			/* clobber any ungetc data */
			if (HASUB(fp))
				FREEUB(fp);
			fp->_flags &= ~(__SRD|__SEOF);
			
			/*   Need to adjust the real-byte offset of the file
			 *   depending on the value of fp->_r 
			 *   before resetting its value.
			 *   In binary-mode: seekBackCount = No. of unread bytes in buffer
			 *   In text-mode: seekBackCount = (No. of unread chars in buffer + No. of newlines present as part of unread chars in buffer)
			 */
			if(fp->_r)
			    {
	            fpos_t offset = 0;
	            
	            /* Count newlines present as part of unread characters
	             * in buffer denoted by ( fp->_r ) only in text-mode
	             */ 
	            if( fp->_extra->fmode == 't')
	                {
	                int newlines = 0, len = 0;
	                char *p = NULL;
	                p = (char*)(fp->_p);
	                while( len++ < fp->_r )
	                    {
	                    if( *p++ == '\n' )
	                        newlines++;
	                    }
	                offset += newlines;
	                }
	            
	            offset += fp->_r;  
			    if( (_sseek(fp, -(fpos_t)offset, SEEK_CUR)) == (-(fpos_t)1) )
			        return (-1);
			    }
			fp->_r = 0;
			fp->_p = fp->_bf._base;
		}
		fp->_flags |= __SWR;
	}

	/*
	 * Make a buffer if necessary, then set _w.
	 */
	if (fp->_bf._base == NULL)
	{
		__smakebuf(fp);
		if(fp->_bf._base == NULL && errno == ENOMEM)
		{
			return (EOF);
		}
	}
	if (fp->_flags & __SLBF) {
		/*
		 * It is line buffered, so make _lbfsize be -_bufsize
		 * for the putc() macro.  We will change _lbfsize back
		 * to 0 whenever we turn off __SWR.
		 */
		fp->_w = 0;
		fp->_lbfsize = -fp->_bf._size;
	} else
		fp->_w = fp->_flags & __SNBF ? 0 : fp->_bf._size;
	return (0);
}
