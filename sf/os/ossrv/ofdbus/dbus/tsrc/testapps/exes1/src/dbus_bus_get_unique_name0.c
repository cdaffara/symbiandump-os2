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

#define LOG_FILE "c:\\logs\\dbus_bus_get_unique_name0_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
	if(result)
		assert_failed = 1;
	
	testResultXml("dbus_bus_get_unique_name0");
	close_log_file();
}

int CheckConnection(DBusConnection* conn, DBusError* error)
{
	if(!conn)
	{
		std_log(LOG_FILENAME_LINE, "Failed to get connection with error :: %s\n%s", error->name, error->message);
	
		dbus_error_free(error);
		return FALSE;
	}
	
	return TRUE;
}

int main()
{
   	DBusError error;
	DBusConnection* connection;
	char* name;
	char str[30];
				
	dbus_error_init(&error);
	
	connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
	if(!CheckConnection(connection, &error))
	{
		create_xml(1);
		return 1;
	}
	
	name = (char*)dbus_bus_get_unique_name(connection);
	
	if(!name)
	{
		create_xml(1);
		return 1;
	}
	
	strcpy(str, name);
	
	std_log(LOG_FILENAME_LINE, "Unique Name :: %s", str);
	
	dbus_connection_close(connection);
	dbus_connection_unref(connection);
	dbus_shutdown();
	
	std_log(LOG_FILENAME_LINE, "Test Successful");
	create_xml(0);
	return 0;
}
