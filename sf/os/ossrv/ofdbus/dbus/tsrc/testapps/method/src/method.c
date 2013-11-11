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
#include "test-utils.h"
#include<string.h> 
#include<ctype.h>
#include<unistd.h>  
#define TEST_BUS_NAME_TEMP "Test.Method.Call"
 
void called_method(DBusMessage* msg, DBusConnection* connection)
{
	DBusMessage* reply; 
	DBusMessageIter args;
	DBusMessageIter in_args;
	
	char* in_str;
	DBusError error;
	   
	dbus_uint32_t status = 94;

	dbus_error_init(&error);
	dbus_message_iter_init(msg, &in_args);
	 
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append(reply, &args);
	  
	do{
		dbus_message_iter_get_basic(&in_args, &in_str);
					
		dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &in_str);
		
	}while(dbus_message_iter_next(&in_args));
	 
	dbus_connection_send(connection, reply, NULL);
	
	dbus_connection_flush(connection);
}


void called_variable_args_method(DBusMessage* msg, DBusConnection* connection)
{
	DBusMessage* reply;
	DBusMessageIter iter;
	DBusMessageIter sub_iter;
	void* arg = malloc(20);
	char* str;
	int no;
	int i;
	dbus_int32_t* array;
	dbus_int32_t return_value = 0;
	dbus_bool_t valid_flag = TRUE;
	

	
	
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init(msg, &iter);
	
	do{
		switch(dbus_message_iter_get_arg_type(&iter))
		{
			case DBUS_TYPE_BOOLEAN:
				dbus_message_iter_get_basic(&iter, arg);
				if(*(dbus_bool_t*)arg != TRUE)
					valid_flag = FALSE;
				break;
			case DBUS_TYPE_INT16:
				dbus_message_iter_get_basic(&iter, arg);
				if(*(dbus_int16_t*)arg != -16)
					valid_flag = FALSE;
				break;
			case DBUS_TYPE_INT32:
				dbus_message_iter_get_basic(&iter, arg);
				if(*(dbus_int32_t*)arg != -32)
					valid_flag = FALSE;
				break;
			case DBUS_TYPE_INT64:
				dbus_message_iter_get_basic(&iter, arg);
				if(*(dbus_int64_t*)arg != -64)
					valid_flag = FALSE;
				break;
			case DBUS_TYPE_UINT16:
				dbus_message_iter_get_basic(&iter, arg);
				if(*(dbus_uint16_t*)arg != 16)
					valid_flag = FALSE;
				break;
			case DBUS_TYPE_UINT32:
				dbus_message_iter_get_basic(&iter, arg);
				if(*(dbus_uint32_t*)arg != 32)
					valid_flag = FALSE;
				break;
			case DBUS_TYPE_UINT64:
				dbus_message_iter_get_basic(&iter, arg);
				if(*(dbus_uint64_t*)arg != 64)
					valid_flag = FALSE;
				break;
			case DBUS_TYPE_DOUBLE:
				dbus_message_iter_get_basic(&iter, arg);
				if(*(double*)arg != 12.34567)
					valid_flag = FALSE;
				break;
			case DBUS_TYPE_STRING:
				dbus_message_iter_get_basic(&iter, &str);
				if(strcmp("DBus Testing", str))
					valid_flag = FALSE;
				break;
			case DBUS_TYPE_ARRAY: 
				dbus_message_iter_recurse(&iter, &sub_iter);
				no = dbus_message_iter_get_array_len(&sub_iter);
				dbus_message_iter_get_fixed_array(&sub_iter, &array, &no);
				for(i=0;i<no;i++)
					if(array[i] != (i+1))
						valid_flag = FALSE;
				break;
			case DBUS_TYPE_STRUCT: 
				dbus_message_iter_recurse(&iter, &sub_iter);
				do{
					switch(dbus_message_iter_get_arg_type(&sub_iter))
					{
					case DBUS_TYPE_INT32: 
						dbus_message_iter_get_basic(&sub_iter, arg);
						if(*(dbus_int32_t*)arg != -32)
							valid_flag = FALSE;
						break;    
					case DBUS_TYPE_STRING:
						dbus_message_iter_get_basic(&sub_iter, &str);
						if(strcmp("DBus Testing", str))
							valid_flag = FALSE;
						break;
					case DBUS_TYPE_DOUBLE:
						dbus_message_iter_get_basic(&sub_iter, arg);
						if(*(double*)arg != 12.34567)
							valid_flag = FALSE;
						break;
					default: 
						valid_flag = FALSE;
					
					}                               
				}while(dbus_message_iter_next(&sub_iter));
				
				break; 
			default: 
				valid_flag = FALSE;
		}
	}while(dbus_message_iter_next(&iter));
	
	if(!valid_flag)
		return_value = 1;
	else
		return_value = 0;
	
	dbus_message_append_args(reply, DBUS_TYPE_INT32, &return_value, DBUS_TYPE_INVALID);
	dbus_connection_send(connection, reply, NULL);
	dbus_connection_flush(connection);
}
 
void called_simple(DBusMessage* msg, DBusConnection* connection)
{
	DBusMessage* reply;	
	dbus_int32_t return_value = 9090;
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_append_args(reply, DBUS_TYPE_INT32, &return_value, DBUS_TYPE_INVALID);
	dbus_connection_send(connection, reply, NULL);
	dbus_connection_flush(connection);
}


void called_file_send(DBusMessage* msg, DBusConnection* connection)
{
	DBusMessage* reply;
	int return_value = 8080;
	DBusMessageIter iter;
	DBusMessageIter sub_iter;
	int no; 
	unsigned char* array;
	FILE* fp=NULL; 
	
	dbus_message_iter_init(msg, &iter);
	dbus_message_iter_recurse(&iter, &sub_iter);
	no = dbus_message_iter_get_array_len(&sub_iter);
	dbus_message_iter_get_fixed_array(&sub_iter, &array, &no);
	
	fp = fopen("c:\\data\\images\\pictures\\bunbask1.jpg", "a+"); 
	if(!fp) 
	{
		printf("Failed to open file");
		return;
	}
	
	fwrite(array, sizeof(unsigned char), no, fp);
	
	if(fp)
		fclose(fp);
	
	reply = dbus_message_new_method_return(msg);
	dbus_connection_send(connection, reply, NULL);
	dbus_connection_flush(connection);
}

void called_file_send_time(DBusMessage* msg,DBusConnection* connection)
	{
		DBusMessageIter iter;
		DBusMessageIter sub_iter;
		int no; 
		unsigned char* array;
		FILE* fp=NULL; 
		DBusMessage* reply;
		char* file_name;
		
		reply = dbus_message_new_method_return(msg);
		dbus_connection_send(connection, reply, NULL);
		dbus_connection_flush(connection);
		
		dbus_message_iter_init(msg, &iter);
		dbus_message_iter_get_basic(&iter, &file_name);
		dbus_message_iter_next(&iter);
		dbus_message_iter_recurse(&iter, &sub_iter);
		no = dbus_message_iter_get_array_len(&sub_iter);
		dbus_message_iter_get_fixed_array(&sub_iter, &array, &no);
		
		strcat(file_name, "_1");
		fp = fopen(file_name, "w+");
		if(!fp) 
		{
			printf("Failed to open file %s", file_name);
			getchar();
		}
		
		fwrite(array, sizeof(unsigned char), no, fp);
		if(fp)
			fclose(fp);
	}

void called_sign_test(DBusMessage* msg, DBusConnection* connection)
{
	DBusError error;
	const char* signature;
	const char* msg_sign;
	const char* msg_sign1;
	DBusSignatureIter sign_iter;
	DBusSignatureIter sub_sign_iter;
	DBusSignatureIter sub_sign_iter1;
	DBusMessageIter iter;
	DBusMessage* reply;
	int type;
	int sub_type;
	const char* sub_signature;
	
	dbus_error_init(&error);
	
	dbus_message_iter_init(msg, &iter);
	msg_sign = dbus_message_iter_get_signature(&iter);
	dbus_message_iter_next(&iter);
	dbus_message_iter_next(&iter);
	msg_sign1 = dbus_message_iter_get_signature(&iter);
	
	signature = dbus_message_get_signature(msg);
	dbus_signature_iter_init(&sign_iter, signature);
	if(dbus_signature_validate_single(signature, &error))
	{
		exit(1);
	}
	do{
		type = dbus_signature_iter_get_current_type(&sign_iter);
		switch(type)
		{
		case DBUS_TYPE_ARRAY:
			dbus_signature_iter_recurse(&sign_iter, &sub_sign_iter1);
			sub_type = dbus_signature_iter_get_element_type(&sign_iter);
			break;
		case DBUS_TYPE_STRUCT:
			dbus_signature_iter_recurse(&sign_iter, &sub_sign_iter);
			sub_signature = dbus_signature_iter_get_signature(&sign_iter); 
			dbus_error_init(&error);
			if(!dbus_signature_validate_single(sub_signature, &error))
			{
				exit(1);
			}
			break;
		}
	}while(dbus_signature_iter_next(&sign_iter));
	
	reply = dbus_message_new_method_return(msg);
	dbus_message_append_args(reply, DBUS_TYPE_SIGNATURE, &signature, DBUS_TYPE_SIGNATURE, &sub_signature, DBUS_TYPE_INT32, &sub_type, DBUS_TYPE_SIGNATURE, &msg_sign, DBUS_TYPE_SIGNATURE, &msg_sign1, DBUS_TYPE_INVALID);
	dbus_connection_send(connection, reply, NULL);
	dbus_connection_flush(connection);
	
	dbus_free((void*)msg_sign);
	dbus_free((void*)msg_sign1);
	dbus_message_unref(reply);
}

void called_file_send_whole(DBusMessage* msg, DBusConnection* connection)
	{
		DBusMessageIter iter;
		DBusMessageIter sub_iter;
		int no; 
		unsigned char* array;
		FILE* fp=NULL;
		DBusMessage* reply;
		
		reply = dbus_message_new_method_return(msg);
		dbus_connection_send(connection, reply, NULL);
		dbus_connection_flush(connection);
		
		dbus_message_iter_init(msg, &iter);
		dbus_message_iter_recurse(&iter, &sub_iter);
		no = dbus_message_iter_get_array_len(&sub_iter);
		dbus_message_iter_get_fixed_array(&sub_iter, &array, &no);
		
		fp = fopen("c:\\data\\Images\\Pictures\\test.mp3", "a+"); 
		if(!fp) 
		{
			printf("Failed to open file");
			getchar();
		}
		
		fwrite(array, sizeof(unsigned char), no, fp);
		
		if(fp)
			fclose(fp);
		
		dbus_message_unref(reply);
	}

int main()
{
	DBusError error;
	DBusError error1;
	DBusConnection* connection;
	DBusMessage* msg;
	DBusObjectPathVTable vtable = 
	{
		NULL,NULL,NULL
	}; 
	
	dbus_error_init(&error);
	dbus_error_init(&error1);

	connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
	
	if(dbus_error_is_set(&error))
	{
		fprintf(stdout, "Error Occured :: %s", error.name);
		return 1;
	}
	

	
	if(dbus_connection_register_object_path (connection, "/Test/Method/Object", &vtable, NULL))
	{
		fprintf(stdout, "Object Path registered.");
	}
	else
	{
		fprintf(stdout, "Object Path not able to register.");
		return 1;
	}
 


	if(dbus_bus_request_name (connection, TEST_BUS_NAME_TEMP, DBUS_NAME_FLAG_ALLOW_REPLACEMENT, &error1) == -1)
	{
		fprintf(stdout, "Not able to request name.");
	}
	else
	{
		fprintf(stdout, "Name Request Successful");
	}
	
	while(TRUE)
	{	
		dbus_connection_read_write(connection, 0);
		
		msg = dbus_connection_pop_message(connection);
			
		if(msg == NULL)
		{
			sleep(1);
			continue;
		}
		     
		fprintf(stdout, "Message Detected");
		
		if(DBUS_MESSAGE_TYPE_SIGNAL == dbus_message_get_type(msg))
		{
			fprintf(stdout, "Message is Signal.");
		}
		
		if(DBUS_MESSAGE_TYPE_METHOD_CALL == dbus_message_get_type(msg))
		{
			fprintf(stdout, "Message is Method call.");
		}
		
				
		if(dbus_message_is_method_call(msg, "test.Method.Call", "method"))
			{
			called_method(msg, connection);
			break;
			} 
		
		if(dbus_message_is_method_call(msg, "test.Method.Call", "variable_args_method"))
			{
			
			called_variable_args_method(msg, connection);
			break;
			} 
		 
		if(dbus_message_is_method_call(msg, "test.Method.Call", "simple"))
			{
			
			called_simple(msg, connection);
	//		break;
			}
		if(dbus_message_is_method_call(msg, "test.Method.Call", "file_send"))
			{
			
			called_file_send(msg, connection);
	//		break; 
			} 
		if(dbus_message_is_method_call(msg, "test.Method.Call", "sign_test"))
			{
			
			called_sign_test(msg, connection);
			break; 
			} 
		if(dbus_message_is_method_call(msg, "test.Method.Call", "file_send_whole"))
			{
			
			called_file_send_whole(msg, connection);
			break; 
			}
		
		if(dbus_message_is_method_call(msg, "test.Method.Call", "file_send_time"))
			{
			
			called_file_send_time(msg, connection);
	//		break;
			}
		
	dbus_message_unref(msg);	
		
	}
	
	dbus_message_unref(msg);
	dbus_bus_release_name(connection,TEST_BUS_NAME_TEMP,&error1);
	dbus_connection_unregister_object_path(connection,"/Test/Method/Object");

	dbus_connection_unref(connection);	

	return 0;
}
