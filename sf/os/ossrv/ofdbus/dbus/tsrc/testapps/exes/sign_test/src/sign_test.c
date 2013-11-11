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

#define LOG_FILE "c:\\logs\\sign_test_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
	{
	if(result)
		assert_failed = 1;
	
	testResultXml("sign_test");
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
	DBusConnection* connection;
	DBusError error;
	DBusMessage* msg;
	DBusMessage* reply = NULL;
	dbus_int32_t int_32 = -32;
	double d = 1.1234;
	char* str = "DBus Testing";
	const char* sign;
	const char* sub_sign;
	const char* iter_sign;
	const char* iter_sign1;
	dbus_int32_t type;
	dbus_int32_t arr[] = {0,1,2,3,4,5,6,7,8,9};
	dbus_int32_t* v_arr = arr;
	DBusMessageIter iter;
	DBusMessageIter sub_iter;
	DBusMessageIter sub_iter1;
	struct SignTest
	{
		int num1;
		int num2;
		int num3;
	}st;
	
	dbus_error_init(&error);
	connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
	if(!connection || dbus_error_is_set(&error))
		return handle_error(&error);
	
	msg = dbus_message_new_method_call("Test.Method.Call", "/Test/Method/Object", NULL, "sign_test");
	if(!msg)
	{
		std_log(LOG_FILENAME_LINE, "msg is NULL");
		create_xml(1);
		return 1;
	}
	
	st.num1 = 100;
	st.num2 = 200;
	st.num3 = 300;
	
	dbus_message_iter_init_append(msg, &iter);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &int_32);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &str);
	dbus_message_iter_open_container(&iter, DBUS_TYPE_STRUCT, NULL, &sub_iter);
	dbus_message_iter_append_basic(&sub_iter, DBUS_TYPE_INT32, &st.num1);
	dbus_message_iter_append_basic(&sub_iter, DBUS_TYPE_INT32, &st.num2);
	dbus_message_iter_append_basic(&sub_iter, DBUS_TYPE_INT32, &st.num3);
	dbus_message_iter_close_container(&iter, &sub_iter);
	dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, DBUS_TYPE_INT32_AS_STRING, &sub_iter1);
	dbus_message_iter_append_fixed_array(&sub_iter1, DBUS_TYPE_INT32, &v_arr, 10);
	dbus_message_iter_close_container(&iter, &sub_iter1);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_DOUBLE, &d);
	
	reply = dbus_connection_send_with_reply_and_block(connection, msg, 10000, &error);
	if(!reply)
		return handle_error(&error);
	
	if(!dbus_message_get_args(reply, &error, DBUS_TYPE_SIGNATURE, &sign, DBUS_TYPE_SIGNATURE, &sub_sign, DBUS_TYPE_INT32, &type, DBUS_TYPE_SIGNATURE, &iter_sign, DBUS_TYPE_SIGNATURE, &iter_sign1, DBUS_TYPE_INVALID))
		return handle_error(&error);
	
	std_log(LOG_FILENAME_LINE, "Signature :: %s\n\tSubSignature :: %s\n\tType :: %c\n\tMessageIterSignature :: %s\n\tMessageIterSignature1 :: %s", sign, sub_sign, type, iter_sign, iter_sign1);
	if(!dbus_message_has_signature(msg, sign))
	{
		std_log(LOG_FILENAME_LINE, "Signature returned is not correct.");
		create_xml(1);
		return 1;
	}
	if(strcmp("i", iter_sign))
	{
		std_log(LOG_FILENAME_LINE, "Signature returned is not correct.");
		create_xml(1);
		return 1;
	}
	if(strcmp("(iii)", sub_sign))
	{
	 	std_log(LOG_FILENAME_LINE, "SubSignature returned is not correct.");
	 	create_xml(1);
		return 1;
	}
	if(strcmp("(iii)", iter_sign1))
	{
		std_log(LOG_FILENAME_LINE, "SubSignature returned is not correct.");
		create_xml(1);
		return 1;
	}
	if('i' != type)
	{
		std_log(LOG_FILENAME_LINE, "type returned is not correct");
		create_xml(1);
		return 1;
	}
	
	dbus_message_unref(msg);
	dbus_message_unref(reply);
	dbus_connection_unref(connection);
	
	std_log(LOG_FILENAME_LINE, "Test Successful");  
	create_xml(0);
	return 0; 
}
