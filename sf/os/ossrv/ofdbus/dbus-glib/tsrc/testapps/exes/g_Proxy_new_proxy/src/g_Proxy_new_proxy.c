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


#define LOG_FILE "c:\\logs\\g_Proxy_new_proxy_log1.txt" 
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
	{
	if(result)
		assert_failed = 1;
	
	testResultXml("g_Proxy_new_proxy");
    close_log_file();
	}



int main()
{

		DBusGConnection* connection;
		DBusGProxy* proxy;
		DBusGProxy* New_proxy;
		GError* error = NULL;
		
		char* path = "/Test/Proxy/Path";
		
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
		proxy = dbus_g_proxy_new_for_name(connection, "Test.Glib.Proxy.Path1", path, "T   est.Proxy.Interface");
				
		New_proxy = dbus_g_proxy_new_from_proxy (proxy,"Test.Proxy.Interface",path);
		
		if(New_proxy==NULL)
			{
				std_log(LOG_FILENAME_LINE,"FAIL");
			create_xml(1);
			return 1;
			}
		std_log(LOG_FILENAME_LINE,"New proxy is been created from the existing proxy");
		
	g_object_unref(New_proxy);
	g_object_unref(proxy);
std_log(LOG_FILENAME_LINE,"Success");
			create_xml(0);
		    return 0;
		    
	}