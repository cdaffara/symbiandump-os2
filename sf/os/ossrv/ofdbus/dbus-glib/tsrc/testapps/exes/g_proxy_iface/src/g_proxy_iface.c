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

#define LOG_FILE "c:\\logs\\g_proxy_iface_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
	{
	if(result)
		assert_failed = 1;
	
	testResultXml("g_proxy_iface");
    close_log_file();
	}

int iface_flag = 0;

static void* set_iface(void* data)
	{
	const char* iface1 = "Test.Proxy.Interface1";
	DBusGProxy* proxy = (DBusGProxy* )data;
	
	dbus_g_proxy_set_interface(proxy, iface1); 
	
	if(strcmp(iface1, dbus_g_proxy_get_interface(proxy)))
		{
		std_log(LOG_FILENAME_LINE, "Fail to check interface %s", iface1);
		}
	iface_flag = 1;
	return NULL;
	}

int main()
{
	DBusGConnection* connection;
	DBusGProxy* proxy;
	DBusGProxy* proxy1 ;//=NULL;
	GError* error = NULL;
	
	pthread_t thr_id;
	void* thrPtr;
	
	char* iface = "Test.Proxy.Interface";
	char* iface1 = "Test.Proxy.Interface1";
	
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
	
	proxy = dbus_g_proxy_new_for_name(connection, "Test.Glib.Proxy.Interface", "/Test/Proxy/Interface", iface);
	proxy1 = dbus_g_proxy_new_for_name(connection, "Test.Glib.Proxy.Interface", "/Test/Proxy/Interface", iface1);
	
	if(strcmp(iface, dbus_g_proxy_get_interface(proxy)))
		{
		std_log(LOG_FILENAME_LINE, "Fail to check interface %s", iface);
		create_xml(1);
		return 1;
		}
	
	pthread_create(&thr_id, NULL, &set_iface, proxy);
	pthread_join(thr_id, &thrPtr);
	
	if(strcmp(iface1, dbus_g_proxy_get_interface(proxy)))
		{
		std_log(LOG_FILENAME_LINE, "Fail to check interface %s", iface1);
		create_xml(1);
		return 1;
		}
	if(!iface_flag)
		{
		std_log(LOG_FILENAME_LINE, "Something wrong happens in thread.");
		create_xml(1);
		return 1;
		}
	
	std_log(LOG_FILENAME_LINE, "Test Successful");
	create_xml(0);
	return 0;
}

