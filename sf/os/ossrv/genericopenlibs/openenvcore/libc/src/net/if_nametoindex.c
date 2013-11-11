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



#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <stdapis/net/if.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

/*
 *    If the specified interface name does not exist, the return value is
 *    0, and errno is set to ENXIO.  If there was a system error (such as
 *    running out of memory), the return value is 0 and errno is set to the
 *    proper value (e.g., ENOMEM).
 */

EXPORT_C unsigned int if_nametoindex(const char *ifname)
	{
	struct if_nameindex *ifi = NULL;
	struct if_nameindex *ifa;
	unsigned int ni;
	
	ifi = if_nameindex();
	if (ifi == NULL)
		{
		errno = ENXIO;
		return 0;
		}
	ni = 0;
	for (ifa = ifi; ifa->if_index != 0; ifa++ )
		{
		if ( strcmp(ifa->if_name, ifname ) == 0)
			{
			ni = ifa->if_index;
			break;
			}
		}
	
	if_freenameindex(ifi);
	if (!ni)
		{
		errno = ENXIO;
		}
	return(ni);
	}
	
