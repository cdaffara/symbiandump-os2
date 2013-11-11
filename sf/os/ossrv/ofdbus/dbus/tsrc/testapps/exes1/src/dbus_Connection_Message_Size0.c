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

#define LOG_FILE "c:\\logs\\Dbus_Connection_Message_Size01_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
	if(result)
		assert_failed = 1;
	
	testResultXml("Dbus_Connection_Message_Size0");
	close_log_file();
}

int main()
{
    
	long set_size = 123456;
	long get_size = 0;
	DBusConnection* connection;
	DBusError error;
	
	dbus_error_init(&error);
	
	connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
	if(!CheckConnection(connection, &error))
		return 1;
	
	get_size = dbus_connection_get_max_message_size(connection);
	std_log(LOG_FILENAME_LINE, "Max Message size is %ld",get_size);
		
	dbus_connection_set_max_message_size(connection, set_size);
	get_size = dbus_connection_get_max_message_size(connection);
	if(set_size != get_size)
		{
			std_log(LOG_FILENAME_LINE, "Failed in dbus_connection_max_message_size");
			create_xml(1);
			return 1;
		}
	
	get_size = dbus_connection_get_max_received_size(connection);
	std_log(LOG_FILENAME_LINE, "Max received size is %ld",get_size);
	
	
	dbus_connection_set_max_received_size(connection, set_size);
	get_size = dbus_connection_get_max_received_size(connection);	
	if(set_size != get_size)
		{
			std_log(LOG_FILENAME_LINE, "Failed in dbus_connection_max_received_size");
			create_xml(1);
			return 1;
		}

	dbus_connection_close(connection);
	dbus_connection_unref(connection);
	dbus_shutdown();
	std_log(LOG_FILENAME_LINE, "SUCCESS");
	create_xml(0); 
	return 0;
}

CheckConnection(DBusConnection* conn, DBusError* error)
	{
		if(!conn)
		{
			std_log(LOG_FILENAME_LINE, "Failed to get connection with error :: %s\n%s", error->name, error->message);
			    	
	    	dbus_error_free(error);
	    	return FALSE;
		}
		
		return TRUE;
	}
