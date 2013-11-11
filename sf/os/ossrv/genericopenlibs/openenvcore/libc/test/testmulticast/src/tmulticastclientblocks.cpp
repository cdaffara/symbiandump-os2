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
// Name        : tmulticastclientblocks.cpp
// 
//

#include "tmulticastclient.h"

#define MSGBUFSIZE 512
#define MULTICAST_PORT 5000
#define MULTICAST_GROUP "224.0.10.110"
#define MESSAGE "Hello, World"
const TUint KMsgLength = sizeof(MESSAGE);

void* ThreadSendto(void*)
	{
	int fd;
	struct sockaddr_in addr;
	
    fd = socket(AF_INET,SOCK_DGRAM,0);
    if (fd < 0)
		{
		return (void*)NULL;
		}
    
    memset((void *)&addr,0,sizeof(struct sockaddr_in));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr = inet_addr(MULTICAST_GROUP); 
    addr.sin_port=htons(MULTICAST_PORT);
    
    int counter = 0;
    while(counter<10)
    	{
        if( sendto(fd, MESSAGE, KMsgLength, 0, (struct sockaddr*)&addr, sizeof(struct sockaddr)) < 0)
        	{
        	close(fd);
        	return (void*)NULL;
        	}
        counter++;
    	}
	close(fd);
	return (void*)NULL;
	}

void* ThreadRecvfrom(void* flagvar)
	{
	int fd, err;
	int onOff=1,ttl = 3, counter = 0, nbytes = 0;
	struct sockaddr_in addr;
	char msgbuf[MSGBUFSIZE];
	unsigned int addrlen;
	struct ip_mreq mreq;
	int *flag = (int*)flagvar;
	
	if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0) 
		{
		return (void*)NULL;
	  	}
	
	 err = setsockopt(fd,SOL_SOCKET, SO_REUSEADDR,(char *)&onOff, sizeof( onOff ));
	 if (err != 0)
	 	{
	 	return (void*)NULL;
	 	}

	 memset((void *)&addr,0,sizeof(struct sockaddr_in));
	 addr.sin_family=AF_INET;
	 addr.sin_addr.s_addr = htonl(INADDR_ANY);
	 addr.sin_port=htons(MULTICAST_PORT);
	 	 
	 /* bind to receive address */
	 if (bind(fd,(struct sockaddr *) &addr,sizeof(addr)) < 0) 
	 	{
	 	return (void*)NULL;
	 	} 
	 	
	 /* use setsockopt() to request that the kernel join a multicast group */
	 memset( ( void * )&mreq, 0, sizeof( struct ip_mreq ) );
	 mreq.imr_interface.s_addr = htonl( INADDR_ANY );
	 mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
		 
	 if ((err = setsockopt(fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,(char *)&mreq,sizeof(struct ip_mreq))) < 0) 
	 	{
	 	return (void*)NULL;
	 	}
	 
	 err = setsockopt( fd, IPPROTO_IP,IP_MULTICAST_TTL, &ttl, sizeof( ttl ) );
	 if (err != 0) 
	 	{
	 	return (void*)NULL;
	 	}
	 /* now just enter a read-print loop */
	 while (counter < 10) 
	 	{
	   	addrlen=sizeof(addr);
	   	nbytes = recvfrom(fd, msgbuf, KMsgLength, 0, (struct sockaddr *)&addr, &addrlen);
	   	if ( nbytes < 0 ) 
	   		{
	   		return (void*)NULL;
	   		}
	   	msgbuf[nbytes] = '\0';
	   	counter++;
	 	}
	   	
	 if (setsockopt(fd, IPPROTO_IP, IP_DROP_MEMBERSHIP,(char *)&mreq,sizeof(struct ip_mreq)) != 0 )
	 	{
	 	return (void*)NULL;
	 	}
	 *flag = 1;
	 return (void*)NULL;
	}

TInt CTestMulticastClient::TestClient()
    {
  	struct sockaddr_in addr;
	unsigned int addrlen;
	int fd, nbytes=0;
	int err = 0,ret = KErrNone;
	int onOff=1,ttl = 3;
	int counter  = 0, flag = 0;   
	struct ip_mreq mreq;
	char msgbuf[MSGBUFSIZE];
	
	struct ifreq ifReq;    
	/* Set the default interface */    
	strcpy( ifReq.ifr_name, "Ethernet with Daemon Dynamic IP" );    
	ret = setdefaultif( &ifReq );    
	if( ret == -1 )
		{
		INFO_PRINTF2(_L( "Setting default interface failed with errno = %d"), errno );
		return KErrGeneral;
		}
		
	/* create what looks like an ordinary UDP socket */
	if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0) 
		{
	  	INFO_PRINTF2(_L("Socket creation failed with errno %d"), errno);
	  	return KErrGeneral;
	  	}
	
	err = setsockopt(fd,SOL_SOCKET, SO_REUSEADDR,(char *)&onOff, sizeof( onOff ));
	if (err != 0)
		{
	  	INFO_PRINTF2(_L("setsockopt with SO_REUSEADDR failed with errno %d\n"),errno);
	  	return KErrGeneral;
	 	}

	memset((void *)&addr,0,sizeof(struct sockaddr_in));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port=htons(MULTICAST_PORT);
		 
	/* bind to receive address */
	if (bind(fd,(struct sockaddr *) &addr,sizeof(addr)) < 0) 
	 	{
	  	INFO_PRINTF2(_L("Socket binding failed with errno %d"), errno);
	  	return KErrGeneral;
	 	} 
	 
 	INFO_PRINTF1(_L("socket bound and waiting..."));
 	
	/* use setsockopt() to request that the kernel join a multicast group */
	memset( ( void * )&mreq, 0, sizeof( struct ip_mreq ) );
	mreq.imr_interface.s_addr = htonl( INADDR_ANY );
	mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
		 
	if ((err = setsockopt(fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,(char *)&mreq,sizeof(struct ip_mreq))) < 0) 
		{
	   	INFO_PRINTF2(_L("The new P.I.P.S socket can not support Multicast!!! Errno is %d"),errno);
	   	return KErrGeneral;
	 	}
	INFO_PRINTF1(_L("Now the new P.I.P.S socket has joined the multicast group.."));
 
	ret = setsockopt( fd, IPPROTO_IP,IP_MULTICAST_TTL, &ttl, sizeof( ttl ) );
	if (ret != 0) 
		{
        INFO_PRINTF2(_L("Setting TTL failed with errno %d"), errno);
        return KErrGeneral;
	 	}
	
	pthread_t thread, recvthread;
	pthread_create(&recvthread,NULL,ThreadRecvfrom,&flag);
	pthread_create(&thread,NULL,ThreadSendto,NULL);
	  	
	/* now just enter a read-print loop */
	INFO_PRINTF1(_L("waiting for msg.."));
	while (counter < 10) 
	 	{
	   	addrlen=sizeof(addr);
	   	nbytes = recvfrom(fd, msgbuf, KMsgLength, 0, (struct sockaddr *)&addr, &addrlen);
	   	if ( nbytes < 0 ) 
	   		{
	        INFO_PRINTF2(_L("Recvfrom failed with errno %d"),errno);
	        return KErrGeneral;
	   		}
	   	msgbuf[nbytes] = '\0';
	   	counter++;
	 	}

	if (setsockopt(fd, IPPROTO_IP, IP_DROP_MEMBERSHIP,(char *)&mreq,sizeof(struct ip_mreq)) != 0 )
		{
	 	INFO_PRINTF2(_L("dropping membership from multicast group failed with errno %d"), errno);
	 	return KErrGeneral;
	 	}
	else
	   	INFO_PRINTF1(_L("dropped from multicast group......"));
	INFO_PRINTF2(_L("%d msgs received"), counter);
  	
	pthread_join(recvthread, NULL);
	pthread_join(thread, NULL);
	if(!flag)
		{
		INFO_PRINTF1(_L("ThreadRecvfrom failed\n"));
		return KErrGeneral;
		}
	 
	ret = setdefaultif(NULL);
	if(ret == -1)
		INFO_PRINTF2(_L("Removing default interface failed with errno %d\n"),errno);
	return KErrNone;
    }
