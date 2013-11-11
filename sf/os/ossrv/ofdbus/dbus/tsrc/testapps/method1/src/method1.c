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


//#include<stdio.h>  
#include "test-utils.h"
#include<string.h> 
#include<ctype.h>
#include<unistd.h>  // sleep function is defined in this header file 
#define TEST_BUS_NAME_TEMP "Test.Method.Call1"
dbus_bool_t message_is_error;
 

void called_file_writecontact(DBusMessage* msg,DBusConnection* connection)
	{
	FILE *fp;  

	char* str;
	dbus_int32_t cnt;
	DBusMessage* reply;
	DBusMessageIter iter;
	DBusMessageIter sub_iter;
	
	dbus_message_iter_init(msg, &iter);   
	
	
	fp = fopen("c:\\data.txt","a+");
	do{
		switch(dbus_message_iter_get_arg_type(&iter))
		{
			
		
		
		case DBUS_TYPE_STRUCT: 
			dbus_message_iter_recurse(&iter, &sub_iter);
			do{
					switch(dbus_message_iter_get_arg_type(&sub_iter))
					{
					case DBUS_TYPE_INT32:
						dbus_message_iter_get_basic(&sub_iter, &cnt);
						fwrite(&cnt, sizeof(cnt),1,fp);
						break;
					case DBUS_TYPE_STRING:
						dbus_message_iter_get_basic(&sub_iter, &str);
						fwrite(str, strlen(str),1,fp);
					} 
					fwrite("\n", strlen("\n"),1,fp);
			}while(dbus_message_iter_next(&sub_iter));
			
			break;  
		default: 
		fprintf(stdout,"no input"); 
 
		}
	}while(dbus_message_iter_next(&iter));
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_append_args(reply, DBUS_TYPE_INT32, &cnt, DBUS_TYPE_INVALID);
	dbus_connection_send(connection, reply, NULL);
	dbus_connection_flush(connection); 
	}
	
void called_dbus_connection_borrow_message0(DBusMessage* msg,DBusConnection* connection)
	{
		DBusMessage* reply;	
		dbus_int32_t return_value = 9090;
	
		reply = dbus_message_new_method_return(msg);
		
		dbus_message_append_args(reply, DBUS_TYPE_INT32, &return_value, DBUS_TYPE_INVALID);
		dbus_connection_send(connection, reply, NULL); 
		dbus_connection_flush(connection); 
	}
void called_dbus_connection_pop_message0(DBusMessage* msg,DBusConnection* connection)
	{
	DBusMessage* reply;	
	dbus_int32_t return_value = 9090;

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_append_args(reply, DBUS_TYPE_INT32, &return_value, DBUS_TYPE_INVALID);
	dbus_connection_send(connection, reply, NULL);
	dbus_connection_flush(connection);
	}
void called_dbus_connection_steal_borrowed_message0(DBusMessage* msg,DBusConnection* connection)
	{
	DBusMessage* reply;	
	dbus_int32_t return_value = 9090;

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_append_args(reply, DBUS_TYPE_INT32, &return_value, DBUS_TYPE_INVALID);
	dbus_connection_send(connection, reply, NULL);
	dbus_connection_flush(connection);
	}
void called_dbus_connection_return_message0(DBusMessage* msg,DBusConnection* connection)
	{
	DBusMessage* reply;	
	
	reply = dbus_message_new_method_return(msg);

	dbus_connection_send(connection, reply, NULL);
	dbus_connection_flush(connection);
	}
void called_dbus_message_new_error_printf0(DBusMessage* msg,DBusConnection* connection)
	{
		char err_succes[]={"ERROR IS SAME"};
		
		char err_failed[]= {"MISMATCH IN ERROR"};//'M','i','s','m','a','t','c','h',' ','i','n',' ' ,'e','r','r','o','r'
		const char* errorname;
		FILE *fp;
		
		DBusMessage* err_msg;
		const char* error_name; 
		const char* error_message;
		dbus_int32_t return_value =9090;
		errorname = "dbus.test.error1";
		/* change the error name so that err_failed is printed
		 * for eg: errorname = "dbus.test.error1"
		 * 
		      */
		
		error_name = "dbus.test.error";  
		error_message = "dbus.message";
		
		fp = fopen("c:\\method1newerror.txt","w");
	
		err_msg = dbus_message_new_error(msg,error_name,error_message);  
		message_is_error=dbus_message_is_error(err_msg ,errorname);
		if(message_is_error)
			{
				fwrite(err_succes,1,sizeof(err_succes),fp); 
				
				fclose(fp);
			}
		else 
			{ 
			fwrite(err_failed,1,sizeof(err_failed),fp);  
			fclose(fp);
			}
		 
		   
		
	
		dbus_connection_send(connection, err_msg, NULL); 
		dbus_connection_flush(connection); 
	}

void called_dbus_message_get_no_reply0(DBusMessage* msg,DBusConnection* connection)
	{
		dbus_bool_t nomsg = FALSE;
		char err_succes[]={"Set reply is set/reply not required"};
		char err_fail[]={"Set reply is not been set/reply may be sent"};
		
		dbus_int32_t return_value = 9090;
		FILE *fp; 
		fp = fopen("c:\\msggetnorply.txt","w"); 
		
	
	
		nomsg = dbus_message_get_no_reply(msg) ;
		if(nomsg)
			{
			fwrite(err_succes,1,sizeof(err_succes),fp); 
			fclose(fp);
			}
		else  
			{
				fwrite(err_fail,1,sizeof(err_fail),fp);  
				fclose(fp);
			}
			
	
		dbus_connection_flush(connection); 
	}




int main()
{
	DBusError error;
	DBusError error1;
	DBusConnection* connection;
	DBusMessage* msg;
	DBusObjectPathVTable vtable;
	 
	dbus_error_init(&error);
	dbus_error_init(&error1);
	fprintf(stdout, "Starting Method");
	
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
		if(dbus_message_is_method_call(msg, "test.Method.Call", "file_writecontact"))
			{
			
			called_file_writecontact(msg, connection);
			break;
			}
		 
		
		 
		if(dbus_message_is_method_call(msg, "test.Method.Call", "dbus_connection_borrow_message0"))
			{
			
			called_dbus_connection_borrow_message0(msg, connection);
	
			}
		
		if(dbus_message_is_method_call(msg, "test.Method.Call", "dbus_connection_pop_message0"))
			{
			
			called_dbus_connection_pop_message0(msg, connection);
	
			}
		
		if(dbus_message_is_method_call(msg, "test.Method.Call", "dbus_connection_steal_borrowed_message0"))
			{
			
			called_dbus_connection_steal_borrowed_message0(msg, connection);
	
			}
		if(dbus_message_is_method_call(msg, "test.Method.Call", "dbus_connection_return_message0"))
			{
			
			called_dbus_connection_return_message0(msg, connection);
		
			}
		if(dbus_message_is_method_call(msg, "test.Method.Call", "dbus_message_new_error_printf0"))
			{
				called_dbus_message_new_error_printf0(msg, connection);
		
			}
		

		
		if(dbus_message_is_method_call(msg, "test.Method.Call", "dbus_message_get_no_reply0"))
			{
			
			called_dbus_message_get_no_reply0(msg, connection);
			break;
			} 
		dbus_message_unref(msg);    
	}
	
	dbus_connection_unref(connection); 
	return 0;
}
