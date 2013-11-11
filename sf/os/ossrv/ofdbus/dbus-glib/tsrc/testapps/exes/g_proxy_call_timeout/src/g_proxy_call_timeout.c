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


#define LOG_FILE "c:\\logs\\g_proxy_call_timeout_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
	{
	if(result)
		assert_failed = 1;
	
	testResultXml("g_proxy_call_timeout");
    close_log_file();
	}


int main()
{
	DBusGConnection *connection;
	GError *error;
	DBusGProxy *proxy;
	
	gint res;
	
	g_type_init ();
	
	error = NULL;
	connection = dbus_g_bus_get (DBUS_BUS_SESSION,
	                           &error);
	if (connection == NULL)
	{
		std_log(LOG_FILENAME_LINE,"Failed to open connection to bus: %s\n",
	              error->message);
	  g_error_free (error);
	  create_xml(1);
	  exit (1);
	}
	
	proxy = dbus_g_proxy_new_for_name (connection,
	        "com.example.DBusGlibObject1",
	        "/com/example/DBusGlibObject1",
	        "com.example.DBusGlibObject1");
	
	if(!dbus_g_proxy_call_with_timeout(proxy, "Simple_method", 10000, &error, G_TYPE_INT, 5, G_TYPE_INVALID, G_TYPE_INT, &res, G_TYPE_INVALID))
	{
		std_log(LOG_FILENAME_LINE, "Error :: %s", error->message);
		create_xml(1);
		return 1;
	}
	
	std_log(LOG_FILENAME_LINE, "Return value = %d", res);
	if(res != 9090)
		{
		std_log(LOG_FILENAME_LINE, "return value is not matching.");
		create_xml(1);
		return 1;
		}
	
	dbus_g_connection_unref(connection);
	std_log(LOG_FILENAME_LINE, "Test Successful");
	create_xml(0);
	return 0;
}

