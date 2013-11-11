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

#define LOG_FILE "c:\\logs\\dbus_connection_preallocate_send0_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
	if(result)
		assert_failed = 1;
	
	testResultXml("dbus_connection_preallocate_send0");
	close_log_file();
}

int main()
{
 	DBusConnection* connection;
		DBusError error;
		DBusMessage* msg;
		char error_name[40];
		char error_msg[40];
		DBusPreallocatedSend* resource; 
		dbus_error_init(&error);
		
		connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);

		if(!connection)
		{
			std_log(LOG_FILENAME_LINE, "Error_name:%s",error.name);
			std_log(LOG_FILENAME_LINE, "Error_msg:%s",error.message);
			create_xml(1);
			return 1;
		}  
		
	resource = dbus_connection_preallocate_send( connection ); 
	dbus_connection_free_preallocated_send(connection,resource);
	if(resource == NULL)
		{
			std_log(LOG_FILENAME_LINE, "Preallocate resources needed to send a message is Failed");
			std_log(LOG_FILENAME_LINE, "test case failed");
			create_xml(1);
			return 1;
		}
	else 
			std_log(LOG_FILENAME_LINE, "Preallocation of  resources needed to send a message is been created");
			
			
	
 
	
	  std_log(LOG_FILENAME_LINE, "SUCCESS");
	  create_xml(0);
	
	   
	   dbus_connection_close(connection);
	   dbus_connection_unref(connection);
	   dbus_shutdown();
 
    return 0;    
			
		
}
