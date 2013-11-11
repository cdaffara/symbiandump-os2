/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <sys/sockio.h>
#include <stdapis/net/if.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

/*
 * Return All Interface Names and Indexes
 *
 *    The if_nameindex structure holds the information about a single
 *    interface and is defined as a result of including the <net/if.h>
 *    header.
 *
 *       struct if_nameindex {
 *         unsigned int   if_index;
 *         char          *if_name;
 *       };
 *
 *    The final function returns an array of if_nameindex structures, one
 *    structure per interface.
 *
 *       struct if_nameindex  *if_nameindex(void);
 *
 *    The end of the array of structures is indicated by a structure with
 *    an if_index of 0 and an if_name of NULL.  The function returns a NULL
 *    pointer upon an error, and would set errno to the appropriate value.
 *
 *    The memory used for this array of structures along with the interface
 *    names pointed to by the if_name members is obtained dynamically.
 *    This memory is freed by the next function.
 *
 * Free Memory
 *
 *    The following function frees the dynamic memory that was allocated by
 *    if_nameindex().
 *
 *        #include <stdapis/net/if.h>
 *
 *        void  if_freenameindex(struct if_nameindex *ptr);
 *
 *    The argument to this function must be a pointer that was returned by
 *    if_nameindex().
 */

int getifinfo(struct if_nameindex **pif);

EXPORT_C struct if_nameindex *if_nameindex(void)
	{
	struct if_nameindex *ifni;

	// Memory allocation for ifni is in getifaddrs() function
	if (getifinfo(&ifni) < 0)
		{
		errno = ENXIO;
		return(NULL);
		}
	return ifni;
	}

EXPORT_C void if_freenameindex(struct if_nameindex *ptr)
	{	
	free(ptr);
	}

int getifinfo(struct if_nameindex **pif)
	{
	char *buf;
	struct ifconf ifc;
	struct ifreq *ifr;	
	int ret;
	int index;
	struct if_nameindex *data;
	char *names;
	int sock;
    int namelength = 0;
	int count = 0;//IAP count
	
	if(pif == NULL)	
		{
		return (-1);
		}
	*pif = NULL;
	
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
		return (-1);
		}
	//for calculating the number of access points
	ret =  ioctl(sock, SIOCGIFNUM, &count);
	if (ret < 0)
	    {
	    close(sock);
	    return (-1);
	    }
	//allocate only if ioctl()is successfull
	buf = (char *)malloc(count*sizeof(struct ifreq));
	if (buf == NULL)
		{
		close(sock);
		return (-1);
		}
	ifc.ifc_buf = buf;
	ifc.ifc_len = count*sizeof(struct ifreq);
	//this ioctl obtains the ifnet(access point) details
	ret =  ioctl(sock, SIOCGIFCONF, (char *)&ifc);
	close(sock);
	if (ret < 0)
		{
		free(buf);
		return (-1);
		}
	ifr = ifc.ifc_req;
	if (ifr == NULL || count == 0)
		{
		free(buf);
		return (-1);
		}
		
	// Calulate the memory requirement of interface names		
	for (index = 0; index < count; index++)
		{
		namelength += strlen(ifr->ifr_name);
		namelength++; // +1 for null character
		ifr++;
		}
    /*
     * Next, allocate a chunk of memory, use the first part
     * for the array of structures, and the last part for
     * the strings.
     */
	*pif = (struct if_nameindex*)malloc( ( (count + 1) * sizeof(struct if_nameindex) ) + namelength);
	if (*pif == NULL)
		{
		free(buf);
		return (-1);
		}
	data = *pif;
	names = (char*)(*pif + count + 1);
	
	// Traverse the ifc, copy index and name
	ifr = ifc.ifc_req;
	for (index = 0; index < count; index++)
		{
		data->if_index = ifr->ifr_index;
		data->if_name = names;
		strcpy(data->if_name, ifr->ifr_name);
		names = names + strlen(ifr->ifr_name) + 1; // +1 for null character
		ifr++;
		data++;
		}
	// Finally, don't forget to terminate the array.	
	data->if_index = 0;
	data->if_name = NULL;
	free(buf);
	return (0);
	}
