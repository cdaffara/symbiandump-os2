/*-
 * © Portions copyright (c) 2006 Nokia Corporation.  All rights reserved.
 * Copyright (c) 1995, 1996, 1997, 1998, 1999 Kungliga Tekniska Högskolan
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
 */

#ifndef __SYMBIAN32__
#ifdef HAVE_CONFIG_H
#include <config.h>
RCSID("$Id: readv.c,v 1.1.2.1.2.4 2006/11/23 11:59:59 sivanand Exp $");
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

#define min(X , Y)   ((X) < (Y) ? (X) :(Y) )


EXPORT_C ssize_t
readv(int d, const struct iovec *iov, int iovcnt)
{
    ssize_t ret, nb;
    size_t tot = 0;
    int VecIter = 0 ;
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

    for(VecIter = 0; VecIter  < iovcnt ; ++VecIter)
    	{
    	 if(iov[VecIter].iov_base == NULL) 
    	 { 
    	    if(VecIter != 0) 
    	    {
    	     break ;	 //Break from the for loop 
    	    }
    	 	errno = EFAULT ;
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
    
    nb = ret = read (d, buf, tot);
    p = buf;
    
    while (nb > 0) {
        ssize_t cnt = min(nb, iov->iov_len);
        memcpy (iov->iov_base, p, cnt);
        p += cnt;
        nb -= cnt;
        iov += 1 ;      //Newly added
    }
    free(buf);
    return ret;
}

