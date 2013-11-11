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

#define LOG_FILE "c:\\logs\\method_call_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
	{
	if(result)
		assert_failed = 1;
	
	testResultXml("method_call");
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
	DBusError error; 
	DBusConnection* connection;
	DBusMessage* msg;
	DBusMessageIter args;
	DBusMessageIter append_args;
	//~!@#$%^&*()_  . hjgfyh34348#$!%^4567
	char* str = "DBus Testing";
	char* str1;
	int i;  
	       
	DBusPendingCall* pending; 
	std_log(LOG_FILENAME_LINE, "Test start");
	    
	dbus_error_init(&error); 
	 
	std_log(LOG_FILENAME_LINE, "creating connection");	
	
	connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
	
	if(dbus_error_is_set(&error))
		return handle_error(&error);
	 
	std_log(LOG_FILENAME_LINE, "connection created successfully");
	msg = dbus_message_new_method_call("Test.Method.Call", "/Test/Method/Object", NULL, "method");
	
	if(msg == NULL)
		{
		std_log(LOG_FILENAME_LINE, "message error");
		create_xml(1);
		return 1;
		}
	std_log(LOG_FILENAME_LINE, "message created successfully");
	
	dbus_message_iter_init_append(msg, &append_args);
	for(i=0;i<255;i++)
	{
		if(!dbus_message_iter_append_basic(&append_args, DBUS_TYPE_STRING, &str))
		{
			std_log(LOG_FILENAME_LINE, "Unable to append arguments. msg no. %d");
			create_xml(1);
			return 1; 
		} 
	}	   
	
	// send message and get a handle for a reply
	if (!dbus_connection_send_with_reply (connection, msg, &pending, -1)) { // -1 is default timeout
			std_log(LOG_FILENAME_LINE, "message send error");
			create_xml(1);
			exit(1);
	} 
	if (NULL == pending) { 
		std_log(LOG_FILENAME_LINE, "pending is null");
		create_xml(1);
		exit(1); 
	} 
	dbus_connection_flush(connection);
	
	// free message
	dbus_message_unref(msg);
	
	// block until we recieve a reply
	dbus_pending_call_block(pending);
	
	// get the reply message
	msg = dbus_pending_call_steal_reply(pending);
	if (NULL == msg) {
		std_log(LOG_FILENAME_LINE, "Reply error");	   		
		create_xml(1);
		return 1;
	} 
	// free the pending message handle
	dbus_pending_call_unref(pending);
	 
	dbus_error_init (&error);
	dbus_message_iter_init(msg, &args);
	i=1; 
	do{
		dbus_message_iter_get_basic(&args, &str1);
		std_log(LOG_FILENAME_LINE, "%d th Reply = %s", i, str1);
		 
		if(strcmp(str, str1)) 
			   {
			   std_log(LOG_FILENAME_LINE, "Invalid reply error");
			   create_xml(1);
			   return 1;
			   }		  
		i++;
	}while(dbus_message_iter_next(&args)); 
	
	// free reply and close connection
	dbus_message_unref(msg);  
	
	dbus_connection_unref(connection);
	
	std_log(LOG_FILENAME_LINE, "Test Successful");
	create_xml(0);
	return 0;
}
