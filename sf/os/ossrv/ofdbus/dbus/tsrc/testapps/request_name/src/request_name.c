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


#include<stdio.h>  
#include "test-utils.h"
#include <dbus/dbus.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int main()
{
        DBusConnection* connection;
        DBusError error;
        DBusError error1;
        int result;
        
        const char* fifopath = "C:\\mkfifo2.file";
    	int err;
    	int fd;
    	char* buf1 = NULL;
        
        dbus_error_init(&error);
        dbus_error_init(&error1);
        connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
        result = dbus_bus_request_name(connection, "Test.Method.Call3", DBUS_NAME_FLAG_ALLOW_REPLACEMENT, &error1);
        if(result == -1)
        {
                printf("Error 1\n");
                buf1 = "Error Occured";
        }
        fd = open(fifopath, O_WRONLY);
    	if(fd > 0)
    	{
    		if(!buf1)
    			buf1 = "done";
    		err = write (fd, buf1, 20); 
    		close(fd);
    	}
        while(1)
                sleep(1);

        return 0;
}
