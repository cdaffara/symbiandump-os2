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
// Name        : tsample.cpp
// 
//

// INCLUDE FILES
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdapis/netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/un.h>
#include <errno.h>
#include <fcntl.h>

#include <test/testexecutestepbase.h>
#include <test/testexecuteserverbase.h>

#define SERVPATH "c:\\tmp.pipe" 
const TUint KMaxLine = 27;

int write1(int sock_fd)
    {
    char line[KMaxLine];    
    int x;
    char character = 'A';
    unsigned int cnt = 0;
    for(; cnt < KMaxLine - 1; cnt++)
        {
        line[cnt] = character;
        character++;
        if (character > 'Z')
            {
            character = 'A';
            }
        }
    line[cnt] = '\0';
    x=write(sock_fd,line,KMaxLine);
    if (x < 0)
        {
	    return KErrGeneral;
        }
        
    return 0;
    }
    
int main()
	{
	int sock_fd,newsock_fd;	
	int error;
	unsigned int t;
	struct sockaddr_un serveraddr,remote;
	TInt ret = 0;

	sock_fd = socket(AF_UNIX,SOCK_STREAM,0);
	if (sock_fd < 0)
        {
        return KErrGeneral;
        }

	memset(&serveraddr, 0x00, sizeof(struct sockaddr_un));    
	serveraddr.sun_family=AF_UNIX;    
	strcpy((char*)&serveraddr.sun_path, SERVPATH);    
	unlink(SERVPATH);
	if ( bind(sock_fd,(struct sockaddr *)&serveraddr,sizeof(serveraddr))  < 0)
		{
		ret = KErrGeneral;
		goto close;
		}

    if(listen(sock_fd,5) < 0)
    	{
    	ret = KErrGeneral;
    	goto close;
     	}

	t=sizeof(remote);
    if((newsock_fd = accept(sock_fd,(struct sockaddr *)&remote,&t)) < 0)
    	{
    	printf("Accept failed errno = %d\n");
    	ret = KErrGeneral;
    	goto close;
    	}

	error = write1(newsock_fd);
	
	if (error < 0)
		{		
		ret = error;
		goto close;
		}
	
close:
	shutdown(sock_fd,SHUT_RDWR);
	close(sock_fd);
	unlink(SERVPATH);

	return ret;	
	}
