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


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#define MYPORT 5000 // the port users will be connecting to

#define BACKLOG 50 // how many pending connections queue will hold

int main(void)
	{
	int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
	struct sockaddr_in my_addr; // my address information
	struct sockaddr_in their_addr; // connector's address information
	socklen_t sin_size;

	int yes=1;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
		{
		exit(1);
		}
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		{
		exit(1);
		}

	my_addr.sin_family = AF_INET; // host byte order
	my_addr.sin_port = htons(MYPORT); // short, network byte order
	my_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
	memset(my_addr.sin_zero, '\0', sizeof my_addr.sin_zero);
	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof my_addr) == -1) 
		{
		exit(1);
		}
	if (listen(sockfd, BACKLOG) == -1) 
		{
		exit(1);
		}

	while(1)
		{
		FILE *fp;
		char *ptr1;
		int SIZE = 100*1024; // happens for only data size > 16KB
		int no_bytes;
		int retVal;

		sin_size = sizeof(their_addr);
		if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1)
			{
			continue;
			}
		int ret = fcntl (new_fd, F_SETFL, O_NONBLOCK);

		fp = fopen("c:\\socketmorethan16k.txt","rb"); // somefile >16K
		ptr1 = (char *)malloc(SIZE);

		if(fp)
			{
			no_bytes = fread(ptr1,1,SIZE,fp);
			fclose(fp);
			retVal = write(new_fd,ptr1,no_bytes);
			if(retVal == -1)
				{
				return -1;
				}
			}
		}

	}
