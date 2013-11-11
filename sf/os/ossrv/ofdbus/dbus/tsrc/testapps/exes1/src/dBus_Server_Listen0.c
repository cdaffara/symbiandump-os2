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

#define LOG_FILE "c:\\logs\\DBus_Server_Listen01_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
	if(result)
		assert_failed = 1;
	
	testResultXml("DBus_Server_Listen0");
	close_log_file();
}

int main()
{
 DBusServer* server;
	DBusConnection* connection;
	DBusError error;
	char* address;
		
	dbus_error_init(&error);
	server = dbus_server_listen("tcp:host=localhost,port=8085", &error);
	
	if(!server)
	{
		std_log(LOG_FILENAME_LINE, "Error :: %s\n%s",error.name,error.message);
		std_log(LOG_FILENAME_LINE, "Fail");
		create_xml(1);
		return 1;
	}
	
	address = dbus_server_get_address(server);
	std_log(LOG_FILENAME_LINE, "Address :; %s",address);
	
	
	connection = dbus_connection_open_private("tcp:host=localhost,port=8085", &error);
	if(!CheckConnection(connection, &error))
		return 1;
	

	dbus_connection_close(connection);
	dbus_connection_unref(connection);
	dbus_shutdown();
	
	dbus_server_disconnect(server);	
	
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

