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
static char sccsid[] = "@(#)stdio.c	8.1 (Berkeley) 6/4/93";
#endif /* LIBC_SCCS and not lint */
#include <sys/cdefs.h>
#ifndef __SYMBIAN32__
__FBSDID("$FreeBSD: src/lib/libc/stdio/stdio.c,v 1.24 2003/01/07 06:17:13 tjr Exp $");
#endif
#include "namespace.h"
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include "common_def.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "un-namespace.h"
#include "local.h"
#include "sysreent.h"



/*
 * Small standard I/O/seek/close functions.
 */
#ifndef __SYMBIAN32__
int
__sread(cookie, buf, n)
	void *cookie;
	char *buf;
	int n;
{
	FILE *fp = cookie;

	return(_read(fp->_file, buf, (size_t)n));
}

int
__swrite(cookie, buf, n)
	void *cookie;
	char const *buf;
	int n;
{
	FILE *fp = cookie;

	return (_write(fp->_file, buf, (size_t)n));
}

fpos_t
__sseek(cookie, offset, whence)
	void *cookie;
	fpos_t offset;
	int whence;
{
	FILE *fp = cookie;

	return (lseek(fp->_file, (off_t)offset, whence));
}

int
__sclose(cookie)
	void *cookie;
{

	return (_close(((FILE *)cookie)->_file));
}
#else //__SYMBIAN32__
int
__sread (void *cookie, char *buf, int n)
{
  register FILE *fp = (FILE *) cookie;
  int retn = 0, realBytesRead = 0;
  /* In case of text mode, converting all \r\n patterns to \n after every RFile->Read() 
   * using trans-buffer fp->_extra->nl_tbuf before writing into the Input buffer "buf". 
   * fp->_extra->nl_tbuf is allocated during fopen(),fdopen() and freed as part of 
   * fclose() and is exclusively for text mode.
   */
  if (fp->_extra->fmode == 't')
      {
      char *dst = buf;
      char *src = fp->_extra->nl_tbuf;
      int i, j = 0, ret;
      do 
          {
          i = 0;
          ret = _read_r(&errno, fp->_file, src, BUFSIZ);
          if (ret > 0) 
              {
              if (j > 0 && dst[j-1] == '\r' && src[i] == '\n') 
                  {
                  --j;
                  }
              
              while (i < ret && j < n) 
                  {
                  if (src[i] == '\r') 
                      {
                      if (i + 1 == ret) 
                          {
                          dst[j++] = src[i];
                          break;
                          }

                      if (src[i+1] == '\n') 
                          {
                          ++i;
                          }
                      } 
                  dst[j++] = src[i++];
                  }
              }
          realBytesRead += i;
          } while (j < n && ret == BUFSIZ);
      //newlines = i - j;  // gives total no. of \r\n patterns. 
      
      if (i < ret)
          {
          ret = _lseek_r (&errno, fp->_file, (off_t) (i-ret), SEEK_CUR); 
          }
      retn = j;
      }
  else
      {
      retn = _read_r(&errno, fp->_file, buf, n);
      }
  // updating fp->_offset field if __SOFF is set in both text,binary modes.
  if (retn > 0) {
        if (fp->_flags & __SOFF) {
            if(fp->_extra->fmode == 't')
                { // text-mode
                if (fp->_offset <= OFF_MAX - realBytesRead)
                    fp->_offset += realBytesRead;
                else
                    fp->_flags &= ~__SOFF;
                }
            else
                { // binary-mode
                if (fp->_offset <= OFF_MAX - retn)
                    fp->_offset += retn;
                else
                    fp->_flags &= ~__SOFF;
                }
        }
    } else if (retn < 0)        
        fp->_flags &= ~__SOFF;
  
  return retn;
}

int
__swrite (void *cookie, char const *buf, int n)
{
  register FILE *fp = (FILE *) cookie;
  int ret = 0, wrote = 0;
  if (fp->_flags & __SAPP)
    (void) _lseek_r (&errno, fp->_file, (off_t) 0, SEEK_END);
  fp->_flags &= ~__SOFF;	/* in case O_APPEND mode is set */
  
  /* In case of text mode, converting all newlines of Input buffer to \r\n patterns 
   * using fp->_extra->nl_tbuf buffer before actually calling RFile->Write().
   * fp->_extra->nl_tbuf is allocated during fopen(),fdopen() and freed as part 
   * fclose() and is exclusively for text mode.
   */
  if (fp->_extra->fmode == 't')
      {
      int newlines = 0, i = 0, j, bytes;
      char *src = (char *)buf;
      char *dst = (char *)fp->_extra->nl_tbuf;
      
      do 
          {
          bytes = 0;
          j = 0;
          while (i < n && j < (BUFSIZ-1)) 
              {
              if (src[i] == '\n') 
                  {
                  ++newlines;
                  dst[j++] = '\r';
                  ++bytes;
                  }
              dst[j++] = src[i++];
              ++bytes;
              }
          wrote += _write_r(&errno, fp->_file, dst, bytes);
          } while (i < n);
      
      ret = (wrote - newlines);
      }
  else
      {
      ret = _write_r (&errno, fp->_file, buf, n);
      }
  
  /* __SOFF removed even on success in case O_APPEND mode is set. */
    if (ret >= 0) {
        if( fp->_extra->fmode == 't' )
            { // text-mode
            if ((fp->_flags & (__SAPP|__SOFF)) == (__SAPP|__SOFF) &&
                    fp->_offset <= OFF_MAX - wrote)
                fp->_offset += wrote;
            else
                fp->_flags &= ~__SOFF;
            }
        else
            { // binary-mode
            if ((fp->_flags & (__SAPP|__SOFF)) == (__SAPP|__SOFF) &&
                fp->_offset <= OFF_MAX - ret)
                fp->_offset += ret;
            else
                fp->_flags &= ~__SOFF;
            }

    } else if (ret < 0)
        fp->_flags &= ~__SOFF;
    
    return ret;
}

fpos_t
__sseek (void* cookie,fpos_t offset,int whence)
{
  register FILE *fp = (FILE *) cookie;
  register off_t ret;
  ret = _lseek_r (&errno, fp->_file, (off_t) offset, whence);
  if (ret == -1L)
    fp->_flags &= ~__SOFF;
  else
    {
      fp->_flags |= __SOFF;
      fp->_offset = ret;
    }
  return ret;
}

int
__sclose (void* cookie)
{
  FILE *fp = (FILE *) cookie;
  return _close_r (&errno, fp->_file);
}
#endif //__SYMBIAN32__



/*
 * Higher level wrappers.
 */
int
_sread(fp, buf, n)
	FILE *fp;
	char *buf;
	int n;
{
    return (*fp->_read)(fp->_cookie, buf, n);
}

int
_swrite(fp, buf, n)
	FILE *fp;
	char const *buf;
	int n;
{
	int serrno;

	if (fp->_flags & __SAPP) {
		serrno = errno;
		if (_sseek(fp, (fpos_t)0, SEEK_END) == -1 &&
		    (fp->_flags & __SOPT))
			return (-1);
		errno = serrno;
	}
	return (*fp->_write)(fp->_cookie, buf, n);
}

fpos_t
_sseek(fp, offset, whence)
	FILE *fp;
	fpos_t offset;
	int whence;
{
	fpos_t ret;
	int serrno, errret;

	serrno = errno;
	errno = 0;
	ret = (*fp->_seek)(fp->_cookie, offset, whence);
	errret = errno;
	if (errno == 0)
		errno = serrno;
	/*
	 * Disallow negative seeks per POSIX.
	 * It is needed here to help upper level caller
	 * in the cases it can't detect.
	 */
	if (ret < 0) {
		if (errret == 0) {
			if (offset != 0 || whence != SEEK_CUR) {
				if (HASUB(fp))
					FREEUB(fp);
				fp->_p = fp->_bf._base;
				fp->_r = 0;
				fp->_flags &= ~__SEOF;
			}
			fp->_flags |= __SERR;
			errno = EINVAL;
		} else if (errret == ESPIPE)
			fp->_flags &= ~__SAPP;
		fp->_flags &= ~__SOFF;
		ret = -1;
	} else if (fp->_flags & __SOPT) {
		fp->_flags |= __SOFF;
		fp->_offset = ret;
	}
	return (ret);
}
