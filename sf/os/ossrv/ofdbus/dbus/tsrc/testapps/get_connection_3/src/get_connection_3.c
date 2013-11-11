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
        
        const char* fifopath = "C:\\mkfifo003.file";
    	int err;
    	int fd;
    	char buf1[80] = "";
        
        dbus_error_init(&error);
        connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
        if(!connection || dbus_error_is_set(&error))
        	strcpy(buf1, error.name);
        
        fd = open(fifopath, O_WRONLY);
    	if(fd > 0)
    	{
    		if(!strcmp(buf1, ""))
    			strcpy(buf1, "done");
    		err = write (fd, buf1, 80);
    		close(fd);
    	}
        return 0;
}
