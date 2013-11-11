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
#include <pthread.h>
#include <fcntl.h>

#define LOG_FILE "c:\\logs\\dbus_pending_call_data_slot0_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

typedef struct 
{
	int num;
	char* str;
}TestData;

typedef struct 
{
	int data_slot;
	TestData* data;
}TestData1;

int pending_call_ret_val = -6;
int data_free_flag = FALSE;

void create_xml(int result)
{
	if(result)
		assert_failed = 1;
	
	testResultXml("dbus_pending_call_data_slot0");
	close_log_file();
}

int handle_error(DBusError* error)
	{
	std_log(LOG_FILENAME_LINE, "%s", error->name);
	std_log(LOG_FILENAME_LINE, "%s", error->message);
	dbus_error_free(error);
	create_xml(1);
	return 1; 
	} 

void Free_Test_Data(void* data)
	{
		TestData* test_data;
		test_data = (TestData*)data;
		
		free(test_data->str);
		free(test_data);
		data_free_flag = TRUE;
	}

	
void handle_reply_variable_args(DBusPendingCall* pending, void* data)
	{
		DBusMessage* msg1;
		DBusMessageIter return_iter;
		DBusError error;
		dbus_int32_t* return_value = (dbus_int32_t*)data;
		dbus_int32_t ret;
		
		// get the reply message
		msg1 = dbus_pending_call_steal_reply(pending);
		if (NULL == msg1) {
		*return_value = -2; 
		}

		// free the pending message handle
		dbus_pending_call_unref(pending);
			 
		dbus_error_init (&error);
		dbus_message_iter_init(msg1, &return_iter);

		dbus_message_iter_get_basic(&return_iter, return_value);
		ret = *return_value; 
		dbus_message_unref(msg1);  
	}

void handle_reply_pending_call_data_slot0(DBusPendingCall* pending, void* data)
{
	DBusMessage* msg1;
	DBusError error;
	int return_value;
	TestData1* data1 = (TestData1*)data;
	TestData* test_data;
	TestData* new_data;
	DBusFreeFunction free_data;
	
	// get the reply message
	msg1 = dbus_pending_call_steal_reply(pending);
	if (NULL == msg1) {
	return_value = -2; 
	return;
	}		
		 
	dbus_error_init (&error);
	
	dbus_message_get_args(msg1, &error, DBUS_TYPE_INT32, &return_value, DBUS_TYPE_INVALID);
	
	if(return_value == 9090)
		return_value = 0;
	
	test_data = (TestData*)dbus_pending_call_get_data(pending, data1->data_slot);
	if(test_data->num != data1->data->num || strcmp(test_data->str, data1->data->str))
		{
		return_value = -3;
		return;
		}

	new_data = (TestData *)malloc(sizeof(TestData));
	new_data->num = 234;
	new_data->str = (char*)malloc(20);
	strcpy(new_data->str, "Check DBus New");
	free_data = Free_Test_Data;
	data_free_flag = FALSE;
	if(!dbus_pending_call_set_data(pending, data1->data_slot, new_data, free_data))
		{
		return_value = -4; 
		return;
		}
	
	if(!data_free_flag)
		{
		return_value = -5;
		return;
		}
	data_free_flag = FALSE;
			
	pending_call_ret_val = return_value;
	dbus_message_unref(msg1);  
	// free the pending message handle
	dbus_pending_call_unref(pending);
}

	
int main()
{
	DBusConnection* connection;
	DBusError error;
	DBusMessage* msg;
	DBusPendingCall* pending;
	dbus_int32_t no;
	int data_slot = -1;
	TestData* data;
	TestData1 data1;
	DBusFreeFunction free_data;
	
	DBusPendingCallNotifyFunction reply_handler;
		
	dbus_error_init(&error);
	
	connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
	if(!connection || dbus_error_is_set(&error))
		handle_error(&error);
	
	msg = dbus_message_new_method_call("Test.Method.Call", "/Test/Method/Object", "test.Method.Call", "simple");
	if(!msg)
	{
		std_log(LOG_FILENAME_LINE, "Failed to creat Messsage");
		create_xml(1);
		return 1;
	}
	
	dbus_message_append_args(msg, DBUS_TYPE_INT32, &no, DBUS_TYPE_INVALID);
	dbus_connection_send_with_reply(connection, msg, &pending, -1);
	if(!pending)
	{
		std_log(LOG_FILENAME_LINE, "pending is NULL");
		create_xml(1);
		return 1;
	}
	
	if(!dbus_pending_call_allocate_data_slot(&data_slot))
	{
		std_log(LOG_FILENAME_LINE, "Not able to allocate data slot");
		create_xml(1);
		return 1;
	}
	
	data = (TestData*)malloc(sizeof(TestData));
	data->num = 123;
	data->str = (char*)malloc(20);
	strcpy(data->str, "Check DBus");
	free_data = Free_Test_Data;
	data_free_flag = FALSE;
	data1.data = data;
	data1.data_slot = data_slot;
	if(!dbus_pending_call_set_data(pending, data_slot, data, free_data))
		{
			std_log(LOG_FILENAME_LINE, "Out of Memory");
			create_xml(1);
			return 1; 
		}
	
	dbus_connection_flush(connection);
	
	reply_handler = handle_reply_pending_call_data_slot0;
	if(!dbus_pending_call_set_notify(pending, reply_handler, &data1, NULL))
	{
	std_log(LOG_FILENAME_LINE, "Failed to set handler for pending call");
	create_xml(1);
	return 1;
	}
	
	dbus_pending_call_block(pending);
	dbus_message_unref(msg);		
	
	std_log(LOG_FILENAME_LINE, "Reply = %d", pending_call_ret_val);
	switch(pending_call_ret_val)
	{
		case 0:
			break;
		case -2:
			std_log(LOG_FILENAME_LINE, "Error while stealing reply from pending call.");
			create_xml(1);
			return 1;
		case -3:
			std_log(LOG_FILENAME_LINE, "Not able to retrive data from pending call.");
			create_xml(1);
			return 1;
		case -4:
			std_log(LOG_FILENAME_LINE, "Not able to Reset data for pending call.");
			create_xml(1);
			return 1;
		case -5:
			std_log(LOG_FILENAME_LINE, "Free Function is not called.");
			create_xml(1);
			return 1;
		default:
			std_log(LOG_FILENAME_LINE, "Notify function is not called");
			return 1;
	}
	if(!data_free_flag)
		{
		std_log(LOG_FILENAME_LINE, "Free Function is not called.");
		create_xml(1);
		return 1;
		}
	
	dbus_connection_close(connection);
	dbus_connection_unref(connection);
	dbus_shutdown();
	std_log(LOG_FILENAME_LINE, "Test Successful");
	create_xml(0);
	return 0; 
}
