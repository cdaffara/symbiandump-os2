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

int var_arg_ret_val = -3;

#define LOG_FILE "c:\\logs\\variable_args_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
	{
	if(result)
		assert_failed = 1;
	
	testResultXml("variable_args");
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

void free_mem(void* data)
	{
	dbus_int32_t* return_value = (dbus_int32_t*)data;
	var_arg_ret_val = *return_value;
	free(data);
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

int main()
{
	DBusConnection* connection;
	DBusError error;
	DBusMessage* msg;
	DBusMessageIter append_iter;
	DBusMessageIter sub_iter;
	DBusPendingCall* pending;
	
	dbus_bool_t arg_bool = TRUE;
	dbus_int16_t arg_int16 = -16;
	dbus_int32_t arg_int32 = -32;
	dbus_int64_t arg_int64 = -64;
	dbus_uint16_t arg_uint16 = 16;
	dbus_uint32_t arg_uint32 = 32;
	dbus_uint64_t arg_uint64 = 64;
	double arg_double = 12.34567;
	char* arg_str = "DBus Testing";
	const dbus_int32_t array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	const dbus_int32_t *arr = array;
	
	DBusPendingCallNotifyFunction reply_handler;
	DBusFreeFunction free_data;
	dbus_int32_t* return_value;
	
	dbus_error_init(&error); 
	connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
	if(!connection)
		return handle_error(&error);
	
	msg = dbus_message_new_method_call("Test.Method.Call", "/Test/Method/Object", "test.Method.Call", "variable_args_method");
	if(msg == NULL)
		{ 
		std_log(LOG_FILENAME_LINE,"message error");
		create_xml(1);
		return 1;
		}
	std_log(LOG_FILENAME_LINE,"message created successfully");
	
	dbus_message_iter_init_append(msg, &append_iter);
	
	if(!dbus_message_iter_append_basic(&append_iter, DBUS_TYPE_BOOLEAN, &arg_bool))
		{ 
		std_log(LOG_FILENAME_LINE, "Out Of Memory" );
		create_xml(1);
		return 1; 
		} 
	if(!dbus_message_iter_append_basic(&append_iter, DBUS_TYPE_INT16, &arg_int16))
		{
		std_log(LOG_FILENAME_LINE, "Out Of Memory" );
		create_xml(1);
		return 1;
		}
	if(!dbus_message_iter_append_basic(&append_iter, DBUS_TYPE_INT32, &arg_int32))
		{
		std_log(LOG_FILENAME_LINE, "Out Of Memory" );
		create_xml(1);
		return 1;
		}
	if(!dbus_message_iter_append_basic(&append_iter, DBUS_TYPE_INT64, &arg_int64))
		{
		std_log(LOG_FILENAME_LINE, "Out Of Memory" );
		create_xml(1);
		return 1;
		}
	if(!dbus_message_iter_append_basic(&append_iter, DBUS_TYPE_UINT16, &arg_uint16))
		{
		std_log(LOG_FILENAME_LINE, "Out Of Memory" );
		create_xml(1);
		return 1;
		}
	if(!dbus_message_iter_append_basic(&append_iter, DBUS_TYPE_UINT32, &arg_uint32))
		{
		std_log(LOG_FILENAME_LINE, "Out Of Memory" );
		create_xml(1);
		return 1;
		}
	if(!dbus_message_iter_append_basic(&append_iter, DBUS_TYPE_UINT64, &arg_uint64))
		{
		std_log(LOG_FILENAME_LINE, "Out Of Memory" );
		create_xml(1);
		return 1;
		}
	if(!dbus_message_iter_append_basic(&append_iter, DBUS_TYPE_DOUBLE, &arg_double))
		{
		std_log(LOG_FILENAME_LINE, "Out Of Memory" );
		create_xml(1);
		return 1;
		}
	
	if(!dbus_message_iter_append_basic(&append_iter, DBUS_TYPE_STRING, &arg_str))
		{
		std_log(LOG_FILENAME_LINE, "Out Of Memory" );
		create_xml(1);
		return 1;
		}
	
	// For Array	
	if(!dbus_message_iter_open_container(&append_iter, DBUS_TYPE_ARRAY, DBUS_TYPE_INT32_AS_STRING, &sub_iter))
		{
		std_log(LOG_FILENAME_LINE, "Out Of Memory" );
		create_xml(1);
		return 1;
		}	 
	if(!dbus_message_iter_append_fixed_array(&sub_iter, DBUS_TYPE_INT32, &arr,  10))
		{ 
		std_log(LOG_FILENAME_LINE, "Out Of Memory" );
		create_xml(1);
		return 1;  
		}
	dbus_message_iter_close_container(&append_iter, &sub_iter);
	
	
	//For Structure	
	if(!dbus_message_iter_open_container(&append_iter, DBUS_TYPE_STRUCT, NULL, &sub_iter))
		{
		std_log(LOG_FILENAME_LINE, "Out Of Memory" );
		create_xml(1);
		return 1;
		}	 
	if(!dbus_message_iter_append_basic(&sub_iter, DBUS_TYPE_INT32, &arg_int32))
		{ 
		std_log(LOG_FILENAME_LINE, "Out Of Memory" );
		create_xml(1);
		return 1;  
		}
	if(!dbus_message_iter_append_basic(&sub_iter, DBUS_TYPE_STRING, &arg_str))
		{ 
		std_log(LOG_FILENAME_LINE, "Out Of Memory" );
		create_xml(1);
		return 1;  
		} 
	if(!dbus_message_iter_append_basic(&sub_iter, DBUS_TYPE_DOUBLE, &arg_double))
		{ 
		std_log(LOG_FILENAME_LINE, "Out Of Memory" );
		create_xml(1);
		return 1;  
		}
	dbus_message_iter_close_container(&append_iter, &sub_iter);
	
	
	  
	// send message and get a handle for a reply
	   if (!dbus_connection_send_with_reply (connection, msg, &pending, -1)) { // -1 is default timeout
	   		std_log(LOG_FILENAME_LINE,"message send error");
	   		create_xml(1);
	   		exit(1);
	   } 
	   if (NULL == pending) {
	   		std_log(LOG_FILENAME_LINE,"pending is null");
	   		create_xml(1);
	      exit(1);
	   }
	   dbus_connection_flush(connection);
	   
		// free message
	   dbus_message_unref(msg); 
	  
	   reply_handler = handle_reply_variable_args;
	   return_value = (dbus_int32_t*)malloc(sizeof(dbus_int32_t));
	   free_data = free_mem;
	   dbus_pending_call_set_notify(pending, reply_handler, return_value, free_data);
	   
	   
	   // block until we recieve a reply
	   dbus_pending_call_block(pending); 
	
	 	std_log(LOG_FILENAME_LINE,"Reply = %d", var_arg_ret_val);
		switch(var_arg_ret_val)
		{
			case 0:
				break;
			case -2:
				std_log(LOG_FILENAME_LINE,"Error while stealing reply from pending call");
				create_xml(1);
				return 1;
			case -3:
				std_log(LOG_FILENAME_LINE,"free_data function not called.");
				create_xml(1);
				return 1;
			default:
				std_log(LOG_FILENAME_LINE,"Undefined return value");
				create_xml(1);
				return 1;
		}
			
	   dbus_connection_unref(connection);
	
	   std_log(LOG_FILENAME_LINE, "Test Successful");
	   create_xml(0);
	   return 0;
}
