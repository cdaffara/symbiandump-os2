/*
 * © Portions copyright (c) 2006 Nokia Corporation.  All rights reserved.
 * Copyright (c) 1983, 1993
 *	The Regents of the University of California.  All rights reserved.
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
static char sccsid[] = "@(#)getservbyport.c	8.1 (Berkeley) 6/4/93";
#endif /* LIBC_SCCS and not lint */
#include <sys/cdefs.h>
__FBSDID("$FreeBSD: src/lib/libc/net/getservbyport.c,v 1.7 2005/04/18 18:34:58 ume Exp $");

#include <netdb.h>
#include <string.h>
#include "netdb_private.h"
#ifdef __SYMBIAN32__
#include "reent.h"
#include "netdb_r.h"

#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#include "libc_wsd_defs.h"
#endif 

extern int num_line;

#ifdef EMULATOR
long int *GET_WSD_VAR_NAME(num_line, g)();
#define num_line (*GET_WSD_VAR_NAME(num_line, g)())
#endif //EMULATOR

#endif //__SYMBIAN32__

int
getservbyport_r(int port, const char *proto, struct servent *se,
    struct servent_data *sed)
{
	int error;

#ifdef YP
	sed->yp_port = port;
	sed->yp_proto = (char *)proto;
#endif

	setservent_r(sed->stayopen, sed);
	while ((error = getservent_r(se, sed)) == 0) {
		if (se->s_port != port)
			continue;
		if (proto == 0 || strcmp(se->s_proto, proto) == 0)
			break;
	}
	if (!sed->stayopen)
		endservent_r(sed);

#ifdef YP
	sed->yp_port = 0;
	sed->yp_proto = NULL;
#endif

#ifdef __SYMBIAN32__
       if(sed->fp)
       	{
       	fclose(sed->fp);
       	}
#endif//__SYMBIAN32__
	
	return (error);
}



EXPORT_C struct servent *
getservbyport(int port, const char *proto)
{
	struct servdata *sd;

#ifndef __SYMBIAN32__
	if ((sd = __servdata_init()) == NULL)
		return (NULL);
#else
    sd= reentfunction(_REENT);
    sd->serv.s_name=0;
    sd->serv.s_port=0;
    sd->serv.s_proto=0;
    sd->serv.s_aliases=0;
    sd->data.fp=0;      
    num_line=0;
#endif//__SYMBIAN32__
    if (getservbyport_r(port, proto, &sd->serv, &sd->data) != 0)

#ifdef __SYMBIAN32__
        {
        num_line=0;
#endif//__SYMBIAN32__

	    return (NULL);

#ifdef __SYMBIAN32__
         }    
     num_line=0;
#endif//__SYMBIAN32__

	return (&sd->serv);
}
