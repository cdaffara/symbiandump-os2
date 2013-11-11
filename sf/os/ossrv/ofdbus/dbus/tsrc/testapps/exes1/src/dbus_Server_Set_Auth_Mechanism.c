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

#define LOG_FILE "c:\\logs\\Dbus_Server_Set_Auth_Mechanism1_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
	if(result)
		assert_failed = 1;
	
	testResultXml("Dbus_Server_Set_Auth_Mechanism");
	close_log_file();
}

int main()
{
    
	DBusServer* server;
	DBusError error;
    DBusConnection* connection; 
    dbus_bool_t connection1;
    dbus_bool_t server_set;
    
    char* address;
    char* check_connection;
	
	dbus_error_init(&error);
	server = dbus_server_listen("tcp:host=localhost,port=8085", &error);
	if(!server)
	{
		std_log(LOG_FILENAME_LINE, "Error :: %s\n%s", error.name, error.message);
		std_log(LOG_FILENAME_LINE, "FAIL");
		create_xml(1);
		return 1;
	}
	
	address = dbus_server_get_address(server);
	check_connection=((address));
	
	if(check_connection == NULL)
		{
			std_log(LOG_FILENAME_LINE, "FAILED");
			create_xml(1);
			return 1;
		}
		
		connection = dbus_connection_open("tcp:host=localhost,port=8085", &error);
	
	if(!CheckConnection(connection, &error))
		return 1;  
	
	std_log(LOG_FILENAME_LINE, "check_connection::%s",check_connection);
	
	
	
	connection1=dbus_server_get_is_connected(server);
	if(connection1)
		std_log(LOG_FILENAME_LINE, "Server running");
	else
			std_log(LOG_FILENAME_LINE, "No connection");
	server_set = dbus_server_set_auth_mechanisms(server,NULL);
	if(server_set == TRUE)
		{
			std_log(LOG_FILENAME_LINE, "Server authentication success");
			std_log(LOG_FILENAME_LINE, "SUCCESS");
		}
	else
			std_log(LOG_FILENAME_LINE, "No memory,server authentication failed");
			
	
	dbus_server_disconnect(server);
	connection1=dbus_server_get_is_connected(server);
	if(connection1) {
		std_log(LOG_FILENAME_LINE, "server still running");
		std_log(LOG_FILENAME_LINE, "test failed");
		create_xml(1);   
		return 1;
	}
		else {
			std_log(LOG_FILENAME_LINE, "Connection is been closed");
		    std_log(LOG_FILENAME_LINE, "No connection to the server exists");
			std_log(LOG_FILENAME_LINE, "Success"); 
			create_xml(0);
			return 0;
			
		}
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

