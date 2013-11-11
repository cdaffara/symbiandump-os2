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

#define LOG_FILE "c:\\logs\\DBus_Bus_Start_Service_By_Name01_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
	if(result)
		assert_failed = 1;
	
	testResultXml("DBus_Bus_Start_Service_By_Name0");
	close_log_file();
}

int main()
{
 DBusError error;
	DBusConnection* connection;
	dbus_uint32_t result;
	
	dbus_error_init(&error);
	
	connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
	std_log(LOG_FILENAME_LINE, "Conn. created");
	
	if(!CheckConnection(connection, &error))
		return 1;
	
	if(!dbus_bus_start_service_by_name(connection, "Test.Method.Call", 0, &result, &error))
	{
		std_log(LOG_FILENAME_LINE, "Failed to get connection with error::%s\n%s",error.name,error.message);
		create_xml(1);
		
		dbus_error_free(&error);
		return 1;
	}
	
	if(result == DBUS_START_REPLY_SUCCESS)
		std_log(LOG_FILENAME_LINE, "Service started with result DBUS_START_REPLY_SUCCESS");
	else
		if(result == DBUS_START_REPLY_ALREADY_RUNNING)
			std_log(LOG_FILENAME_LINE, "Service started with result DBUS_START_REPLY_ALREADY_RUNNING");
		else
			std_log(LOG_FILENAME_LINE, "Service started with result unknown");
			
	
	dbus_connection_close(connection);
	dbus_connection_unref(connection);
	dbus_shutdown();
	
	std_log(LOG_FILENAME_LINE, "Success");
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
