// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name        : getnameinfo_private.CPP
// Part of     : LIBC
// Contains the source for the implemantation of part of getnameinfo API
// Version     : 
//



#include <e32std.h>
#include <wchar.h>
#include <_ansi.h>
#include "sysif.h"
#include "fdesc.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/errno.h>
#include "lposix.h"
#include "sysreent.h"
#include <sys/socket.h>
#include <stdapis/netinet/in.h>
#include "netdb_r.h"
#include <string.h>
#include <es_sock.h>
#include <string.h>
#include "getnameinfo_private.h" 
#include <stdapis/arpa/inet.h>


#ifdef __cplusplus
extern "C" {
#endif

struct hostent* gethostaddrdetails(const void *src, int len, int af, int *errp,const char *addr)
{
char hostbuf[100];
#ifdef INET6
	struct in6_addr addrbuf;
#else
	struct in_addr addrbuf;
#endif

	*errp = HOST_NOT_FOUND;

	switch (af) 
		{
	case AF_INET:
		if (len != sizeof(struct in_addr)) 
			{
			*errp = NO_RECOVERY;
			return NULL;
			}
		if ((long)src & ~(sizeof(struct in_addr) - 1)) 
			{
			memcpy(&addrbuf, src, len);
			src = &addrbuf;
			}
		if (((struct in_addr *)src)->s_addr == 0)
			return NULL;
		break;
#ifdef INET6
	case AF_INET6:
		if (len != sizeof(struct in6_addr)) 
			{
			*errp = NO_RECOVERY;
			return NULL;
			}
		if ((long)src & ~(sizeof(struct in6_addr) / 2 - 1)) 
			{	/*XXX*/
			memcpy(&addrbuf, src, len);
			src = &addrbuf;
			}
		if (IN6_IS_ADDR_UNSPECIFIED((struct in6_addr *)src))
			return NULL;
		if (IN6_IS_ADDR_V4MAPPED((struct in6_addr *)src)
		||  IN6_IS_ADDR_V4COMPAT((struct in6_addr *)src)) 
			{
			src = (char *)src +
			    (sizeof(struct in6_addr) - sizeof(struct in_addr));
			af = AF_INET;
			len = sizeof(struct in_addr);
			}
		break;
#endif
	default:
		*errp = NO_RECOVERY;
		return NULL;
		}

        unsigned long  test_addr;
        if (inet_ntop(af, addr, hostbuf, sizeof(hostbuf)) != 0) 
        test_addr=inet_addr(hostbuf);
        
       if (af == AF_INET6)
       {
       	       return(gethostbyaddr((const char *)&addrbuf,sizeof(addrbuf),af));
       }
       else
       return(gethostbyaddr((const char *)(&test_addr),sizeof(test_addr),af));
        
        
     }

#ifdef __cplusplus
}
#endif
