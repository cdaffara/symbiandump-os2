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


#define LOG_FILE "c:\\logs\\g_variant_send_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
	{
	if(result)
		assert_failed = 1;
	
	testResultXml("g_variant_send");
    close_log_file();
	}

int main()
{
	DBusGConnection *connection;
	GError *error;
	DBusGProxy *proxy;
	
	GValue val = {0, };
	GValue val1 = {0, };
	
	char* in  = "Hello World...!!!";
	const char* out;
	
	g_type_init ();
	
	error = NULL;
	
	connection = dbus_g_bus_get (DBUS_BUS_SESSION,
	                           &error);
	
	if (connection == NULL)
	{
		std_log(LOG_FILENAME_LINE, "Failed to open connection to bus: %s\n",
	              error->message);
	  g_error_free (error);
	  create_xml(1);
	  exit (1);
	}
	
	proxy = dbus_g_proxy_new_for_name (connection,
	                                 "com.example.DBusGlibObject1",
	                                 "/com/example/DBusGlibObject1",
	                                 "com.example.DBusGlibObject1");
	
	g_value_init (&val, G_TYPE_STRING);
	g_value_set_string (&val, in);
	
	error = NULL;
	
	if (!com_example_DBusGlibObject1_variant_send_method(proxy, &val, &val1, &error))
	  {
	  	std_log(LOG_FILENAME_LINE, "Error :: %s", error->message);
	  	create_xml(1);
	    return 1;
	  }
	
	out = g_value_get_string(&val1);
	
	if(strcmp(in, out))
		{
		std_log(LOG_FILENAME_LINE, "Strings are mismatching");
		create_xml(1);
		return 1;
		}
	
	g_value_unset (&val);
	g_value_unset (&val1);
	
	dbus_g_connection_unref(connection);
	std_log(LOG_FILENAME_LINE, "Test Successful");
	create_xml(0);
	return 0;
}

