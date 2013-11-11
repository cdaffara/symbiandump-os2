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
#include <dbus/dbus.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include<unistd.h>

int var_arg_ret_val = -3;

#define LOG_FILE "c:\\logs\\dbus_message_get_no_reply_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
	{
	if(result)
		assert_failed = 1;
	
	testResultXml("dbus_message_get_no_reply");
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
		DBusMessage* msg;
		DBusMessage* reply = NULL;
		dbus_bool_t noreply = TRUE;
		char error_name[40];
		char error_msg[40];
		FILE *fp;
		long lsize;
		char *buffer;
		size_t result;
		dbus_error_init(&error);
				
				
		connection = dbus_bus_get(DBUS_BUS_SESSION, &error); 
		if(!connection)
		{
			sprintf(error_name, "Error_name : %s", error.name);
			std_log(LOG_FILENAME_LINE, "Connection is not been established");
			sprintf(error_msg, "Error_msg : %s", error.message);
			create_xml(1);
			return 1;
		}  
		msg = dbus_message_new_method_call("Test.Method.Call1", "/Test/Method/Object", "test.Method.Call", "dbus_message_get_no_reply0");
		if(msg == NULL)
			{ 
				std_log(LOG_FILENAME_LINE, "Message error");
			create_xml(1);
			return 1;
			}
		dbus_message_set_no_reply( msg,noreply);	 
		dbus_connection_send(connection, msg, NULL);
		std_log(LOG_FILENAME_LINE, "Message sent");
		sleep (5);
		fp = fopen("c:\\msggetnorply.txt","rb");   // the file msggetnorply.txt is been written in method1.c
		if(fp == NULL)
		std_log(LOG_FILENAME_LINE, "File doesnt exists");
			
			fseek(fp,0,SEEK_END); 
			lsize = ftell(fp);
			rewind(fp);								// Sets the position indicator associated with stream to the beginning of the file.
			
			buffer = (char*) malloc (sizeof(char)*lsize);
			if(buffer == NULL)
			std_log(LOG_FILENAME_LINE, "Memory error"); 
				
				
			result = fread(buffer,1,lsize,fp);	
			if (result != lsize) 
			std_log(LOG_FILENAME_LINE, "Reading error");
				std_log(LOG_FILENAME_LINE, "%s",buffer);
				std_log(LOG_FILENAME_LINE, "Success");
				create_xml(0);			 
	      return 0;
  } 