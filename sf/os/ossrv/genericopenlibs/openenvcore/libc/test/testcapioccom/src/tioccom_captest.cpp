
// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : tioccom_captest.cpp
// 
//

 
#include "tioccom.h"
#include <sys/types.h>
#include <sys/ioccom.h>
#include <e32err.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdapis/netinet/in.h>
#include <sys/ioctl.h>
#include <stdapis/net/if.h>

#define MAX_LEN  20

// ============================ MEMBER FUNCTIONS ===============================


/*
-------------------------------------------------------------------------------
Function Name		: CTestioccom::ioctlsocket
API Tested			: ioctlsocket
-------------------------------------------------------------------------------
*/

TInt CTestioccom::ioctlsocket()
	{
	INFO_PRINTF1(_L("ioctl"));
    int i;
    struct ifreq ifr;
    int sockfd;
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);      
    strncpy(ifr.ifr_name, "Ethernet with Daemon Dynamic IP", sizeof(ifr.ifr_name));
    i= ioctl(sockfd, SIOCGIFINDEX, &ifr); 
                
    if  (i < 0 && errno == EACCES) 
		{
	    INFO_PRINTF1(_L("Error in ioctl API"));
	    INFO_PRINTF2(_L("errno=%d\n"),errno);
        return errno;
		}
	return KErrNone;
	}
	
