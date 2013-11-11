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

#define LOG_FILE "c:\\logs\\dbus_test_names_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
	if(result)
		assert_failed = 1;
	
	testResultXml("dbus_test_names");
	close_log_file();
}

void TestName(DBusConnection *connection, const char *name, int expectedSuccess, int* result)
{
  	DBusError error;
  	dbus_error_init (&error);

  	(void) dbus_bus_request_name (connection, name, 0, &error);
  	if (dbus_error_is_set (&error))
	{
    	if (expectedSuccess)
    	{
    		std_log(LOG_FILENAME_LINE, "Error acquiring name '%s': %s\n", name, error.message);
    		*result = 1;
    	}
    	else
   		{
   			std_log(LOG_FILENAME_LINE, "Expected Error acquiring name '%s': %s\n", name, error.message);
   			*result = 1;
   		}
    	dbus_error_free (&error);
	}
  	else 
	{
    	if (!expectedSuccess)
    	{
    		std_log(LOG_FILENAME_LINE, "Unexpected Success acquiring name '%s'\n", name);
    		*result = 1;
    	}
    	else
    		std_log(LOG_FILENAME_LINE, "Successfully acquired name '%s'\n", name);
	}
}

int main()
{
        DBusError error;
	DBusConnection *connection;
	int result = 0;
	
	dbus_error_init (&error);
	connection = dbus_bus_get_private (DBUS_BUS_SESSION, &error);
	if (connection == NULL)
	  {
	  	std_log(LOG_FILENAME_LINE, "*** Failed to open connection to system bus: %s\n%s",  error.name, error.message);
		dbus_error_free (&error);
		create_xml(1);
		return 1;
	  }
	
	TestName(connection, "org.freedesktop.DBus.Test", TRUE, &result);
	TestName(connection, "org.freedesktop.DBus.Test-2", TRUE, &result);
	TestName(connection, "org.freedesktop.DBus.Test_2", TRUE, &result);
	#if 0
	TestName(connection, "Test_2", TRUE, &result);
	#endif
	if(result)
	{
		create_xml(1);
		return 1;
	}
	
	dbus_connection_close(connection);
	dbus_connection_unref(connection);
	dbus_shutdown();
	
	std_log(LOG_FILENAME_LINE, "Test Successful");
	create_xml(0);
	return 0;
}
