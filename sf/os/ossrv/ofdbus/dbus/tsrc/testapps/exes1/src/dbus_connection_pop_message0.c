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

#define LOG_FILE "c:\\logs\\dbus_connection_pop_message0_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
	if(result)
		assert_failed = 1;
	
	testResultXml("dbus_connection_pop_message0");
	close_log_file();
}

int main()
{
 	
		DBusConnection* connection;
		DBusError error; 
		DBusMessage* msg;
		DBusMessage*pop_message;
		DBusMessage* reply = NULL;
		char error_name[40];
		char error_msg[40];
		dbus_int32_t no = 5;
		
		dbus_error_init(&error);
		
		connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
	
		if(!connection)
		{
			std_log(LOG_FILENAME_LINE, "Error_name",error.name);
			std_log(LOG_FILENAME_LINE, "Error_message",error.message);
			create_xml(1);
			return 1;
		}  
		msg = dbus_message_new_method_call("Test.Method.Call1", "/Test/Method/Object", "test.Method.Call", "dbus_connection_pop_message0");
		if(msg == NULL)
			{ 
			std_log(LOG_FILENAME_LINE, "Message error");
			create_xml(1);
			return 1;
			}
		
		reply = dbus_connection_send_with_reply_and_block(connection, msg, 10000, &error);
		if(!reply)
			return 1;
		dbus_message_get_args(reply, &error, DBUS_TYPE_INT32, &no, DBUS_TYPE_INVALID);
	
		pop_message =  dbus_connection_pop_message (connection )   ;
		
		std_log(LOG_FILENAME_LINE, "pop message is been execyted successfully");
		std_log(LOG_FILENAME_LINE, "Checking for the message");
		
		pop_message = dbus_connection_borrow_message (connection );
		if(pop_message == NULL)
			{
				std_log(LOG_FILENAME_LINE, "Queue is empty as the pop message is removed the message from the Queue");
				std_log(LOG_FILENAME_LINE, "SUCCESS");
				create_xml(0);
				
				dbus_message_unref(msg);  
				   
				   dbus_connection_close(connection);
				   dbus_connection_unref(connection);
				   dbus_shutdown();
 
				
			    return 0;  
			
				
			}
		else
			{
				std_log(LOG_FILENAME_LINE, "Failed");
		
				create_xml(1);
				return 1;	
				
			}
		
}
