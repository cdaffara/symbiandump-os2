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

#define LOG_FILE "c:\\logs\\dbus_message_new_error_printf0_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
	if(result)
		assert_failed = 1;
	
	testResultXml("dbus_message_new_error_printf0");
	close_log_file();
}

int main()
{
		char error_name[40];
		char error_msg[40];
		long outgoing_size ;
		const char* errorname;
		FILE *fp;
		long lsize;
		char *buffer;
		size_t result;
		DBusConnection* connection;
		DBusError error;
		DBusMessage* msg;
		DBusMessage* reply;
		
		dbus_error_init(&error);
		
		
		errorname = "dbus.test.error"; 
		
		
		
		connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
	
		if(!connection)
		{
			std_log(LOG_FILENAME_LINE, "Error_name :%s",error.name);
			std_log(LOG_FILENAME_LINE, "Error_msg:%s",error.message);
			create_xml(1);
			return 1; 
		}  
		msg = dbus_message_new_method_call("Test.Method.Call1", "/Test/Method/Object", "test.Method.Call", "dbus_message_new_error_printf0");
		if(msg == NULL) 
			{ 
			std_log(LOG_FILENAME_LINE, "Message error");
			create_xml(1);
			return 1;
			} 
		else 
			std_log(LOG_FILENAME_LINE, "Message created");
			
	  	reply = dbus_connection_send_with_reply_and_block(connection, msg, 10000, &error);
	  	if(!reply)
	  		{	  		
	  		fp = fopen("c:\\method1newerror.txt","rb");   // the file msggetnorply.txt is been written in method1.c
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
					std_log(LOG_FILENAME_LINE, "reading error");
					std_log(LOG_FILENAME_LINE, "SUCCESS");
					create_xml(0)  ;
					return 0;
	  		}	
	  	dbus_message_unref(msg);  
	   
	   dbus_connection_close(connection);
	   dbus_connection_unref(connection);
	   dbus_shutdown();
 	 	 return 0;
			
			
		
}
