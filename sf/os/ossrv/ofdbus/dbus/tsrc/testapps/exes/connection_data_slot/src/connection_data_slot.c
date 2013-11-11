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
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int data_free_flag = FALSE;

typedef struct
{
	int num;
	char* str;
} TestData;

void Free_Test_Data(void* data)
	{
		TestData* test_data;
		
		test_data = (TestData*)data;
		free(test_data->str);
		
		data_free_flag = TRUE;
	}


#define LOG_FILE "c:\\logs\\connection_data_slot_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
	{
	if(result)
		assert_failed = 1;
	
	testResultXml("connection_data_slot");
    close_log_file();
	}

int handle_error(DBusError* error)
	{
	std_log(LOG_FILENAME_LINE,"%s", error->name);
	std_log(LOG_FILENAME_LINE,"%s", error->message);
	dbus_error_free(error);
	create_xml(1);
	return 1; 
	} 


int main()
{
	DBusError error;
	DBusConnection* connection;
	dbus_int32_t data_slot = -1;
	TestData data;
	TestData data2;
	TestData* data1;
	DBusFreeFunction free_data;
	
	dbus_error_init(&error);
	
	connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
	if(!connection)
		return handle_error(&error);
	
	if(!dbus_connection_allocate_data_slot(&data_slot))
	{
		std_log(LOG_FILENAME_LINE, "Out of Memory");
		create_xml(1);
		return 1;
	}
	
	data.num = 20;
	data.str = (char*)malloc(data.num);
	data_free_flag = FALSE;
	strcpy(data.str, "CheckDBus");
	free_data = Free_Test_Data;
	
	if(!dbus_connection_set_data(connection, data_slot, (void*)&data, free_data))
	{
		std_log(LOG_FILENAME_LINE, "Out of Memory");
		create_xml(1);
		return 1;
	}
	
	data1 = (TestData*)dbus_connection_get_data(connection, data_slot);
	if(data1->num != data.num || strcmp(data1->str,data.str))
		{
			std_log(LOG_FILENAME_LINE, "Fail to retrieve data");
			create_xml(1);
			return 1;
		}
	
	std_log(LOG_FILENAME_LINE, "Data :: \n\tInteger value(20) = %d\n\tString value(CheckDBus) = %s", data1->num, data1->str);
	
	data2.num = 30;
	data2.str = (char*)malloc(data2.num);
	data_free_flag = FALSE;
	strcpy(data.str, "CheckDBusFree");
	free_data = Free_Test_Data;
	
	if(!dbus_connection_set_data(connection, data_slot, (void*)&data2, free_data))
	{
		std_log(LOG_FILENAME_LINE, "Out of Memory");
		create_xml(1);
		return 1;
	}
	
	if(!data_free_flag)
	{
		std_log(LOG_FILENAME_LINE, "Free Function is not called.");
		create_xml(1);
		return 1;
	}
			
	dbus_connection_free_data_slot(&data_slot);
	
	dbus_connection_close(connection);
	dbus_connection_unref(connection);
	dbus_shutdown();
	
	std_log(LOG_FILENAME_LINE, "Test Successful");
	create_xml(0);
	return 0;
}
