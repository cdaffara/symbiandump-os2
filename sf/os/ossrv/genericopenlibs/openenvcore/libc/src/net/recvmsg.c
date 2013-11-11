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


// Implementation of recvmsg.c

#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

EXPORT_C int
recvmsg(int fd, struct msghdr *message, int flags)
	{
    ssize_t ret = 0;
	ssize_t readSize;
    size_t msgsize = 0;
    char *rcv_buffer;
	char *tempBuffer;
    struct iovec *data_vector = message->msg_iov;
    int vector_index;
	ssize_t cnt;

    for(vector_index = 0; vector_index < message->msg_iovlen; vector_index++)
		{
		msgsize = msgsize + data_vector[vector_index].iov_len;
		}

	if (msgsize <= 0)
    	{
    	return 0;
    	}

    rcv_buffer = (char*)malloc(msgsize);
    if (rcv_buffer == 0) 
		{
		errno = ENOMEM;
		return -1;
		}

    readSize = ret = recvfrom (fd, rcv_buffer, msgsize, flags, message->msg_name, &message->msg_namelen);
    tempBuffer = rcv_buffer;
    while (readSize > 0) 
		{
		if (readSize < data_vector->iov_len)
			{
			cnt = readSize;
			}
		else
			{
			cnt = data_vector->iov_len;
			}
		memcpy (data_vector->iov_base, tempBuffer, cnt);
		tempBuffer += cnt;
		readSize -= cnt;
		data_vector++;
		}
    free(rcv_buffer);
    return ret;
	}
