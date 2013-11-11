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


#define LOG_FILE "c:\\logs\\g_dbus_connection_get_g_type_log1.txt" 
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
	{
	if(result)
		assert_failed = 1;
	
	testResultXml("g_dbus_connection_get_g_type");
    close_log_file();
	}

int main()
{
		DBusGConnection* connection;
		//DBusGProxy* proxy;
		GError* error = NULL;
		GType message_get;
		
		
		g_type_init();
		 
		connection = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
		if (connection == NULL)
			{
			  g_printerr ("Failed to open connection to bus: %s\n",error->message);
			  g_error_free (error);
			  create_xml(1);
			  return 1; 
			}
		message_get=dbus_g_connection_get_g_type();
		if(message_get == 0) 
			{
				std_log(LOG_FILENAME_LINE,"FAIL");
				//g_object_unref(proxy);
				create_xml(1);
				return 1;
			}
				
				std_log(LOG_FILENAME_LINE,"%ld",message_get);
				std_log(LOG_FILENAME_LINE,"Success");
				create_xml(0);
			    return 0;
			
	 }