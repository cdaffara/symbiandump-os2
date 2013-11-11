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

#define LOG_FILE "c:\\logs\\dbus_N_connections_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

#define MAX_CONN 21

void create_xml(int result)
{
	if(result)
		assert_failed = 1;
	
	testResultXml("dbus_N_connections");
	close_log_file();
}

int TestConnectionEqual(DBusConnection* conn1, DBusConnection* conn2)
	{
		return (conn1==conn2? TRUE : FALSE);
	}

int main()
{
	DBusConnection* connection[MAX_CONN];
	DBusError error;
	int cnt=0;
	int cnt1=0;
	
	dbus_error_init(&error);
	for(cnt=0;cnt<MAX_CONN;cnt++)
	{
		connection[cnt] = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
		if(!connection[cnt] || dbus_error_is_set(&error))
		{
			std_log(LOG_FILENAME_LINE, "Error_name = %s", error.name);
			std_log(LOG_FILENAME_LINE, "Error_msg = %s", error.message);
			break;
		}
		else
		{
			for(cnt1=0;cnt1<cnt;cnt1++)
			{
				if(TestConnectionEqual(connection[cnt], connection[cnt1]))
				{ 
					std_log(LOG_FILENAME_LINE, "%d and %d Connections are equal.", cnt, cnt1);
					dbus_connection_close(connection[cnt]);
					dbus_connection_unref(connection[cnt]);
					break;
				}
			}
			std_log(LOG_FILENAME_LINE, "Connection Success %d", cnt);
		}
	} 
	
	for(cnt1=0;cnt1<cnt;cnt1++)
		{
		dbus_connection_close(connection[cnt1]);
		dbus_connection_unref(connection[cnt1]);
		}
	dbus_shutdown();
	
	if(cnt!=MAX_CONN)
	{
		create_xml(1);
		return 1;
	}
	std_log(LOG_FILENAME_LINE, "Test Successful");
	create_xml(0);
	return 0;
}
