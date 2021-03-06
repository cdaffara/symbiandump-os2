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
#include <dbus/dbus.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

#define LOG_FILE "c:\\logs\\req_name_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
	{
	if(result)
		assert_failed = 1;
	
	testResultXml("req_name");
    close_log_file();
	}

int handle_error(DBusError* error)
	{
	std_log(LOG_FILENAME_LINE,"%s", error->name);
	std_log(LOG_FILENAME_LINE,"%s", error->message);
	dbus_error_free(error);
	create_xml(1);
	return 1; 
	} 

int main()
{
	DBusConnection* connection;
	DBusError error;
	DBusError error1;
	int result;
	int fd;
	const char* fifopath = "C:\\mkfifo2.file";
	int err;
	
	FILE* fp;
	char buf[20];
	
	dbus_error_init(&error);
	dbus_error_init(&error1);
	connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
	if(!connection || dbus_error_is_set(&error))
		return handle_error(&error);
	
	result = dbus_bus_request_name(connection, "Test.Method.Call", DBUS_NAME_FLAG_ALLOW_REPLACEMENT, &error1);
	if(result == -1)
		std_log(LOG_FILENAME_LINE, "Error Set :: %s\n%s", error1.name,error1.message);
	if(result != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER )
	{
		std_log(LOG_FILENAME_LINE, "Test Fail");
		create_xml(1);
		return 1;
	}
		
	result = dbus_bus_request_name(connection, "Test.Method.Call2", DBUS_NAME_FLAG_REPLACE_EXISTING, &error1);
	if(result == -1)
		std_log(LOG_FILENAME_LINE, "Error Set :: %s\n%s", error1.name,error1.message);
	if(result != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER )
	{
		std_log(LOG_FILENAME_LINE, "Test Fail");
		return 1;
	}
	
	result = dbus_bus_request_name(connection, "Test.Method.Call2", DBUS_NAME_FLAG_REPLACE_EXISTING, &error1);
	if(result == -1)
		std_log(LOG_FILENAME_LINE, "Error Set :: %s\n%s", error1.name,error1.message);
	if(result != DBUS_REQUEST_NAME_REPLY_ALREADY_OWNER )
	{
		std_log(LOG_FILENAME_LINE, "Test Fail");
		create_xml(1);
		return 1;
	}
	
	//for FIFO
	
	err = mkfifo (fifopath, 0666);
	   if(err != 0)
	   {
	       // probably file already exists, delete the file
	       unlink(fifopath); 
	       // try once more..
	       err = mkfifo (fifopath, 0666);
	       if(err != 0)
	       {
	       		std_log(LOG_FILENAME_LINE, "Failed to make FIFO");
	       		create_xml(1);
	            return 1;
	       }
	   }
	
	#if defined(__WINSCW__) || defined(__WINS__)
	fp = popen("z:\\sys\\bin\\request_name.exe","r");
	#else
	fp = popen("c:\\sys\\bin\\request_name.exe","r");
	#endif
	if(!fp)
		{
		std_log(LOG_FILENAME_LINE, "fp is NULL.");
		create_xml(1);
		return 1;
		}	
	fd = open(fifopath, O_RDONLY);
	if(fd > 0)
	{
		err = read (fd, buf, 20); 
		if(!strcmp("Error Occured", buf))
			{
			std_log(LOG_FILENAME_LINE, "Error while requesting name in request_name.exe.");
			create_xml(1);
			return 1;
			}
		close(fd);
	}
	else
		{
		std_log(LOG_FILENAME_LINE, "Error in FIFO open().");
		create_xml(1);
		return 1;
		}
	unlink(fifopath);
	if(strcmp("done", buf))
		{
		create_xml(1);
		return 1;
		}
	
	result = dbus_bus_request_name(connection, "Test.Method.Call3", DBUS_NAME_FLAG_DO_NOT_QUEUE , &error1);
	if(result == -1)
		std_log(LOG_FILENAME_LINE, "Error Set :: %s\n%s", error1.name,error1.message);
	if(result != DBUS_REQUEST_NAME_REPLY_EXISTS )
	{
		std_log(LOG_FILENAME_LINE, "Test Fail");
		create_xml(1);
		return 1;
	}	
	
	dbus_connection_unref(connection);
	std_log(LOG_FILENAME_LINE, "Test Successful");
	create_xml(0);
	return 0;
}
