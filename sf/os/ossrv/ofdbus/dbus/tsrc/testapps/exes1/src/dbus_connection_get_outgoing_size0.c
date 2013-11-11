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

#define LOG_FILE "c:\\logs\\dbus_connection_get_outgoing_size0_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
	if(result)
		assert_failed = 1;
	
	testResultXml("dbus_connection_get_outgoing_size0");
	close_log_file();
}

int main()
{
		char error_name[40];
		char error_msg[40];
		long outgoing_size = 0 ;
		


		DBusConnection* connection;
		DBusError error;
		DBusMessage* msg;
		DBusMessage* reply = NULL; 
		dbus_error_init(&error);
				connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
	
		if(!connection)
		{
			std_log(LOG_FILENAME_LINE, "ERROR_name:%s",error.name);
			std_log(LOG_FILENAME_LINE, "Error_msg:%s",error.message);
			create_xml(1);
			return 1;
		}  
		msg = dbus_message_new_method_call("Test.Method.Call1", "/Test/Method/Object", "test.Method.Call", "dbus_connection_get_outgoing_size0");
		if(msg == NULL)
			{ 
			std_log(LOG_FILENAME_LINE, "Message error");
			create_xml(1);
			return 1;
			}
		outgoing_size = dbus_connection_get_outgoing_size( connection ) ;
		std_log(LOG_FILENAME_LINE, "outgoing message size before sent %d",outgoing_size);
		
		dbus_connection_send(connection, msg, NULL);
	
		std_log(LOG_FILENAME_LINE, "Message sent");
		
 		outgoing_size = dbus_connection_get_outgoing_size( connection ) ;
		std_log(LOG_FILENAME_LINE, "outgoing message size  %d",outgoing_size);
		create_xml(0);
		
		dbus_message_unref(msg);  
	   
	   dbus_connection_close(connection);
	   dbus_connection_unref(connection);
	   dbus_shutdown();
	
		std_log(LOG_FILENAME_LINE, "success");
		  return 0;  
      
			
			
		
}
