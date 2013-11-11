#include <sys/types.h>
#include <utime.h>
#include <sys/_timeval.h>
#include <sys/time.h>
/*
 * Copyright (c) 1999-2004 Damien Miller <djm@mindrot.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * * Portions Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 */


//#ifndef HAVE_UTIMES
     
EXPORT_C int utimes(const char *filename,  const struct timeval *tvp)
{
	struct utimbuf ub  ;
	
	if(!tvp)   //NULL CHECK
		{
		return utime(filename ,(struct utimbuf *) tvp) ;  
		}
	   

	ub.actime = tvp[0].tv_sec;
	ub.modtime = tvp[1].tv_sec;
	
	return (utime(filename, &ub));
}//#endif
