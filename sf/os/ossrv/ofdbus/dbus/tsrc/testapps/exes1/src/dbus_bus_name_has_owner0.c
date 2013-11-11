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

#define LOG_FILE "c:\\logs\\dbus_bus_name_has_owner0_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

int CheckConnection(DBusConnection* conn, DBusError* error)
	{
		if(!conn)
		{
			std_log(LOG_FILENAME_LINE,"Failed to get connection with error :: %s\n%s", error->name, error->message);
	    	
	    	dbus_error_free(error);
	    	return FALSE;
		}
		
		return TRUE;
	}

void create_xml(int result)
{
	if(result)
		assert_failed = 1;
	
	testResultXml("dbus_bus_name_has_owner0");
	close_log_file();
}

int main()
{

	
	DBusConnection* connection;
	DBusError error;
	
	dbus_error_init(&error);
	connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
	if(!CheckConnection(connection, &error))
		return 1;
	
	if(dbus_bus_request_name(connection, "Test.Request.Name", DBUS_NAME_FLAG_ALLOW_REPLACEMENT, &error) == -1)
	{
		std_log(LOG_FILENAME_LINE,"Error :: %s\nMessage :: %s", error.name, error.message);
		create_xml(1);
		return 1;
	}
	
	if(!dbus_bus_name_has_owner(connection, "Test.Request.Name", &error))
	{
		std_log(LOG_FILENAME_LINE,"Fail to check owner after requesting name.");
		create_xml(1);
		return 1;
	}
	
	if(dbus_bus_release_name(connection, "Test.Request.Name", &error) == -1)
	{
		std_log(LOG_FILENAME_LINE,"Error :: %s\nMessage :: %s", error.name, error.message);
		create_xml(1);
		return 1;
	}
	
	if(dbus_bus_name_has_owner(connection, "Test.Request.Name", &error))
	{
		std_log(LOG_FILENAME_LINE,"owner is there after releasing name.");
		create_xml(1);
		return 1;
	}
	
	dbus_connection_close(connection);
	dbus_connection_unref(connection);
	dbus_shutdown();
	
	std_log(LOG_FILENAME_LINE,"Test Successful");
	create_xml(0);
	return 0;

}
