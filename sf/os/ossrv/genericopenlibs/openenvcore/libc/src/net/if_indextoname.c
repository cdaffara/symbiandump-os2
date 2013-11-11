/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/




#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdapis/net/if.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

/*
 * From RFC 2533:
 *
 * The second function maps an interface index into its corresponding
 * name.
 *
 *    #include <stdapis/net/if.h>
 *
 *    char  *if_indextoname(unsigned int ifindex, char *ifname);
 *
 * The ifname argument must point to a buffer of at least IF_NAMESIZE
 * bytes into which the interface name corresponding to the specified
 * index is returned.  (IF_NAMESIZE is also defined in <net/if.h> and
 * its value includes a terminating null byte at the end of the
 * interface name.) This pointer is also the return value of the
 * function.  If there is no interface corresponding to the specified
 * index, NULL is returned, and errno is set to ENXIO, if there was a
 * system error (such as running out of memory), if_indextoname returns
 * NULL and errno would be set to the proper value (e.g., ENOMEM).
 */

EXPORT_C char *
if_indextoname(unsigned int ifindex, char *ifname)
	{
	struct if_nameindex *ifi = NULL;
	struct if_nameindex *ifa;
	int found = 0;
	
	ifi = if_nameindex();
	if (ifi == NULL)
		{
		errno = ENXIO;
		return NULL;
		}
	for (ifa = ifi; ifa->if_index != 0; ifa++ )
		{
		if (ifindex == ifa->if_index)
			{
			strncpy(ifname, ifa->if_name, IFNAMSIZ);
			found = 1;
			break;
			}
		}
	if (found == 0) 
		{
		ifname = NULL;
		errno = ENXIO;
		}
	if_freenameindex(ifi);
	return(ifname);
	}

