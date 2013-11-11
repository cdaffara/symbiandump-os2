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


// Implementation of sendmsg.c

#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

EXPORT_C ssize_t
sendmsg(int fd, const struct msghdr *message, int flags)
	{
	ssize_t ret = 0;
    size_t msgsize = 0;    
    char *send_buffer;
    char *tempBuffer;
    struct iovec *data_vector = message->msg_iov;
    int vector_index;

	// Calculate the sizes of vector
    for(vector_index = 0; vector_index < message->msg_iovlen; vector_index++)
    	{
    	msgsize = msgsize + data_vector[vector_index].iov_len;
    	}
    
    if (msgsize <= 0)
    	{
    	return 0;
    	}
	
    send_buffer = (char*)malloc(msgsize);
    if (send_buffer == 0) 
		{
		errno = ENOMEM;
		return -1;
		}
    tempBuffer = send_buffer;
    // Copy the data into single buffer
    for (vector_index = 0; vector_index < message->msg_iovlen; vector_index++) 
    	{
		memcpy (tempBuffer, data_vector[vector_index].iov_base, data_vector[vector_index].iov_len);
		tempBuffer = tempBuffer + data_vector[vector_index].iov_len;
    	}    
    ret = sendto (fd, send_buffer, msgsize, flags, message->msg_name, message->msg_namelen);
    free (send_buffer);
    return ret;
	}
