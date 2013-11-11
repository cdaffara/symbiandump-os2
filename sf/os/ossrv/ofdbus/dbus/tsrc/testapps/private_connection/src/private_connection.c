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
#include<ctype.h>
#include <pthread.h> 
#include <fcntl.h>
#include <string.h>
#include<unistd.h>  
 

int main()
{
	DBusError error;
	DBusConnection* connection;
	FILE* fp;
	char buf[40] = "Error Occured";
	const char* fifopath = "C:\\mkfifo.file";
	int fd;
	int err;
	
	dbus_error_init(&error);
	connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
	fp = fopen("C:\\conn.txt", "a+");
	if(dbus_error_is_set(&error))
	{	
		fputs(buf, fp);
		fclose(fp);
	}
	else
		{
		strcpy(buf, "Success occured");
		strcat(buf, "\n");
		fputs(buf, fp);
		fclose(fp);
		}
	fd = open(fifopath, O_WRONLY);
	if(fd > 0)
	{
		err = write (fd, buf, 20);  
		close(fd);
	}
	
	
	while(1)
		{
		sleep(1);
		}
	
	dbus_connection_close(connection);
	dbus_connection_unref(connection);
	dbus_shutdown();
}
