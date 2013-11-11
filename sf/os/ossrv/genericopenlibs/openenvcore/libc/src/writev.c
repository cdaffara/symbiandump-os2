/*-
 *  Copyright (c) 1995, 1996, 1997, 1998, 1999 Kungliga Tekniska Högskolan
 * (Royal Institute of Technology, Stockholm, Sweden).
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * © Portions Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 

 */

#ifndef __SYMBIAN32__
#ifdef HAVE_CONFIG_H
#include <config.h>
RCSID("$Id: writev.c,v 1.1.2.1.2.4 2006/11/23 12:00:53 sivanand Exp $");
#endif
#include "roken.h"
#endif

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/syslimits.h>
#include <stdio.h>
#include <wchar.h>


ssize_t
_writev(int d, const struct iovec *iov, int iovcnt)
{
    ssize_t ret;
    size_t tot = 0;
    int VecIter;
    char *buf, *p;
    
    if(iovcnt > IOV_MAX) 
    {
      errno = EINVAL ;
      return -1 ;
    }
	if(iovcnt < 0 ) 
    {
    	errno = EINVAL ;
    	return -1 ;
    }
    
    for(VecIter = 0; VecIter < iovcnt; ++VecIter) 
    {
       if(iov[VecIter].iov_base == NULL) 
       {
        if(VecIter != 0 ) 
        {
          break ; //Break From the for loop	
        }
    	errno = EFAULT  ;
    	return -1 ;
       }	
    	
       if((int)iov[VecIter].iov_len >= 0 ) 
       {
          tot += iov[VecIter].iov_len;
       }
    	 
       else 
       {
    	  errno = EINVAL ;
    	  return -1 ;	
       }
       
    }
    buf = malloc(tot);
    if (tot != 0 && buf == NULL) {
        errno = ENOMEM;
        return -1;
    }
    p = buf;
    for (VecIter = 0; VecIter < iovcnt; ++VecIter) {
        
        if(iov[VecIter].iov_base == NULL) 
        	{
        	 break ; //Break from the for loop
        	}
        memcpy (p, iov[VecIter].iov_base, iov[VecIter].iov_len);
        p += iov[VecIter].iov_len;
    }
    ret = write (d, buf, tot);
    free (buf);
    return ret;
}

#ifdef __SYMBIAN_COMPILE_UNUSED__
ssize_t
_wwritev(int d, const struct iovec *iov, int iovcnt)
{
    ssize_t ret;
    size_t tot = 0;
    int VecIter;
    FILE *fp ;
   
    wchar_t *buf, *p;
    
    if(iovcnt > IOV_MAX) 
    {
      errno = EINVAL ;
      return -1 ;
    }
	if(iovcnt < 0 ) 
    {
    	errno = EINVAL ;
    	return -1 ;
    }
    
    for(VecIter = 0; VecIter < iovcnt; ++VecIter) 
    {
       if(iov[VecIter].iov_base == NULL) 
       {
        if(VecIter != 0 ) 
        {
          break ; //Break From the for loop	
        }
    	errno = EFAULT  ;
    	return -1 ;
       }	
    	
       if((int)iov[VecIter].iov_len >= 0 ) 
       {
          tot += iov[VecIter].iov_len;
       }
    	 
       else 
       {
    	  errno = EINVAL ;
    	  return -1 ;	
       }
       
    }
    buf = (wchar_t *)malloc(tot*sizeof(wchar_t));
    if (tot != 0 && buf == NULL) {
        errno = ENOMEM;
        return -1;
    }
    p = buf;
    for (VecIter = 0; VecIter < iovcnt; ++VecIter) 
    {
        
        if(iov[VecIter].iov_base == NULL) 
        	{
        	 break ; //Break from the for loop
        	}
        wmemcpy (p, iov[VecIter].iov_base, iov[VecIter].iov_len);
        p += iov[VecIter].iov_len;
    }
   	fp = fdopen(d,"a+"); 
    ret = fwprintf(fp,buf);
    //ret = write (d, buf, tot);
    free (buf);
    return ret;
}

#endif //__SYMBIAN_COMPILE_UNUSED__

EXPORT_C 
writev(int fd, const struct iovec *iov, int iovcnt)
{
	return _writev(fd , iov , iovcnt) ;
}


