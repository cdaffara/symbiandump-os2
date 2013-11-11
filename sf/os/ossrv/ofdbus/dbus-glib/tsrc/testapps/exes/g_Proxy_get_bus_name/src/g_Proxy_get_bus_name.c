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

#include <dbus/dbus-glib-bindings.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <unistd.h>


#define LOG_FILE "c:\\logs\\g_Proxy_get_bus_name_log1.txt" 
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
	{
	if(result)
		assert_failed = 1;
	
	testResultXml("g_Proxy_get_bus_name");
    close_log_file();
	}



int main()
{

		DBusGConnection* connection;
		
		DBusGProxy* proxy;
		GError* error = NULL;
		const char*     proxy_get_name;
		
		char* path = "/Test/Proxy/Path";
		
		const char* name_bus = "Test.Glib.Proxy.Path1";
		int compare =0;
		g_type_init();
		 
		connection = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
		if (connection == NULL)
			{
			  g_printerr ("Failed to open connection to bus: %s\n",
			              error->message);
			  g_error_free (error);
			 
			  create_xml(1);
			  return 1;
			}
		
		proxy = dbus_g_proxy_new_for_name(connection, "Test.Glib.Proxy.Path1", path, "Test.Proxy.Interface");
		
		proxy_get_name = dbus_g_proxy_get_bus_name (proxy);
		if(proxy_get_name != NULL)
		{
			std_log(LOG_FILENAME_LINE,"%s",proxy_get_name);
			
		}
		
			compare = strcmp(name_bus,proxy_get_name);
		
		if(compare == 0)
			{
				std_log(LOG_FILENAME_LINE,"%s", proxy_get_name);
			std_log(LOG_FILENAME_LINE,"success");
			
			g_object_unref(proxy);
			create_xml(0);
		    return 0;
			}
			std_log(LOG_FILENAME_LINE,"FAIL");
		 
		g_object_unref(proxy);
		create_xml(1);
		return 1;
				
	}