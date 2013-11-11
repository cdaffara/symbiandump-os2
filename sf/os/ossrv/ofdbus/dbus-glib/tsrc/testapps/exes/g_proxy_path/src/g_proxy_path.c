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


#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include "dbus_glib1-bindings.h"
#include <dbus/dbus-glib-bindings.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#define LOG_FILE "c:\\logs\\g_proxy_path_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
	{
	if(result)
		assert_failed = 1;
	
	testResultXml("g_proxy_path");
    close_log_file();
	}

int main()
{
	DBusGConnection* connection;
	DBusGProxy* proxy;
	GError* error = NULL;
	
	char* path = "/Test/Proxy/Path";
	
	g_type_init();
	
	connection = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
	if (connection == NULL)
		{
			std_log(LOG_FILENAME_LINE, "Failed to open connection to bus: %s\n",
		              error->message);
		  g_error_free (error);
		  create_xml(1);
		  return 1;
		}
	
	proxy = dbus_g_proxy_new_for_name(connection, "Test.Glib.Proxy.Path", path, "Test.Proxy.Interface");
	
	if(strcmp(path, dbus_g_proxy_get_path(proxy)))
		{
		std_log(LOG_FILENAME_LINE, "Fail to check interface %s", path);
		create_xml(1);
		return 1;
		}
	
	std_log(LOG_FILENAME_LINE, "Test Successful");
	create_xml(0);
	return 0;
}

