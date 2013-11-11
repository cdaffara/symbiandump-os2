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


#include <stdio.h> 
#include <dbus/dbus.h>
#include <stdlib.h>
#include <string.h>

#define LOG_FILE "c:\\logs\\DBus_Server_Data_Slot01_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

int data_free_flag = FALSE;

void create_xml(int result)
{
	if(result)
		assert_failed = 1;
	
	testResultXml("DBus_Server_Data_Slot0");
	close_log_file();
}

typedef struct 
{
	int num;
	char* str;
}TestData;

void Free_Test_Data(void* data)
	{
		TestData* test_data;
		
		
		test_data = (TestData*)data;
		free(test_data->str);
		
		data_free_flag = TRUE;
	}
	

	
int main()
{
	int data_slot=-1;
	DBusServer* server;
	DBusError error;

	TestData data;
	TestData* data_r;
	DBusFreeFunction free_data;
		 
	dbus_error_init(&error);
	server = dbus_server_listen("tcp:host=localhost,port=8086", &error);
	
	if(!server)
	{
		std_log(LOG_FILENAME_LINE, "Error::%s\n%s",error.name,error.message);
		std_log(LOG_FILENAME_LINE, "FAIL");
		create_xml(1);
		return 1;
	}
	
	if(!dbus_server_allocate_data_slot(&data_slot))
		{
		dbus_server_disconnect(server);
		create_xml(1);
		return 1;
		}
	
	data.num = 60;
	data.str = (char *)malloc(data.num);
	data_free_flag = FALSE;
	strcpy(data.str, "Test Server Data Slot.");
	free_data = Free_Test_Data;
	if(!dbus_server_set_data(server, data_slot, &data, free_data))
		{
		dbus_server_disconnect(server);
		create_xml(1);
		return 1;
		}
	
	if((data_r = (TestData*)dbus_server_get_data(server, data_slot)) == NULL)
		{
		std_log(LOG_FILENAME_LINE, "Failed to get data");
		dbus_server_disconnect(server);
		create_xml(1);
		return 1;
		}
	
	if(data_r->num != data.num || strcmp(data.str, data_r->str))
		{
		std_log(LOG_FILENAME_LINE, "Wrong data received");
		dbus_server_disconnect(server);
		create_xml(1);
		return 1;
		}
	
	std_log(LOG_FILENAME_LINE,( "int :: %d\n string :: %s"),data_r->num,data_r->str);
	
	
	if(!dbus_server_set_data(server, data_slot, &data, free_data))
		{
		dbus_server_disconnect(server);
		create_xml(1);
		return 1;
		}
	
	if(!data_free_flag)
		{
		std_log(LOG_FILENAME_LINE, "Data is not freed");
		dbus_server_disconnect(server);
		create_xml(1);
		return 1;
		}
	
	dbus_server_free_data_slot(&data_slot);
	dbus_server_disconnect(server);
	
	std_log(LOG_FILENAME_LINE, "Success");
	create_xml(0);
	return 0;
}
