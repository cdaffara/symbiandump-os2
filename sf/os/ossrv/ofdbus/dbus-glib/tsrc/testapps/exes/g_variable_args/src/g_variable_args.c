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


#define LOG_FILE "c:\\logs\\g_variable_args_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
	{
	if(result)
		assert_failed = 1;
	
	testResultXml("g_variable_args");
    close_log_file();
	}

int main()
{
	DBusGConnection *connection;
	GError *error;
	DBusGProxy *proxy;
	gboolean b = TRUE;
	gint16         n = -16;
	gint         i = -32;
	gint64       x = -64;
	guint16        q = 16;
	guint        u = 32;
	guint64      t = 64;
	gdouble      d = 1.234567;
	gchar*		 s = "DBusGlibTesting~!@#$%^&*()_+`1234567890-=";
	GArray* 	 ai;
	gint		 ret = 1;
	gint cnt;
	
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
	
	ai = g_array_new(FALSE,FALSE,sizeof(gint));
	for(cnt=0; cnt < 5; cnt++)
		g_array_append_val(ai, cnt);
	
	if(!com_example_DBusGlibObject1_variable_args_method(proxy, b, n, i, x, q, u, t, d, s, ai, &ret, &error))
		{
			std_log(LOG_FILENAME_LINE, " %s", error->message);
			create_xml(1);
			return 1;
		}
	
	if(ret)
	{
		std_log(LOG_FILENAME_LINE, "Return value is %d", ret);
		create_xml(1);
		return 1;
	}
	
	dbus_g_connection_unref(connection);
	std_log(LOG_FILENAME_LINE, "Test Successful");
	create_xml(0);
	return 0;
}

