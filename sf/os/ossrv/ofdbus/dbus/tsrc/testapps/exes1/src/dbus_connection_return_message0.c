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

#define LOG_FILE "c:\\logs\\dbus_connection_return_message0_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
	if(result)
		assert_failed = 1;
	
	testResultXml("dbus_connection_return_message0");
	close_log_file();
}

int main()
{
 	
		DBusConnection* connection;
	DBusError error;
	DBusMessage* msg;
	DBusMessage* borrow_message;
	DBusMessage* reply = NULL;
	char error_name[40];
	char error_msg[40];
	
	dbus_error_init(&error);
	
	connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);

	if(!connection)
	{
		std_log(LOG_FILENAME_LINE, "Error_name :%s",error.name);
		std_log(LOG_FILENAME_LINE, "Error_message:%s",error.message);
		create_xml(1);
		return 1;
	}  
	msg = dbus_message_new_method_call("Test.Method.Call1", "/Test/Method/Object", "test.Method.Call", "dbus_connection_return_message0");
	if(msg == NULL)
		{ 
		std_log(LOG_FILENAME_LINE, "message error");
		create_xml(1);
		return 1;
		}
		
	reply = dbus_connection_send_with_reply_and_block(connection, msg, 10000, &error);
	if(!reply)
		return 1;
		std_log(LOG_FILENAME_LINE, "Reply Message is been received");
	
	

	borrow_message =  dbus_connection_borrow_message (connection )   ;
	
	if(borrow_message== NULL)
		{
			std_log(LOG_FILENAME_LINE, "Queue is empty");
			std_log(LOG_FILENAME_LINE, "Testcase failed");
			create_xml(1);
			return 1;
		}
	else
		{
			dbus_connection_return_message(connection,borrow_message);
			std_log(LOG_FILENAME_LINE, "dbus_connection_return_message os executed successfully");
			std_log(LOG_FILENAME_LINE, "Success");
			create_xml(0);
			dbus_message_unref(msg);  
	   
		   dbus_connection_close(connection);
		   dbus_connection_unref(connection);
		   dbus_shutdown();
 
		    return 0;
		}
			
		
}
