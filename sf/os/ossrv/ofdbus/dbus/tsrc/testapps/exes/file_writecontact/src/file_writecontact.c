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

#define LOG_FILE "c:\\logs\\file_writecontact_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
	{
	if(result)
		assert_failed = 1;
	
	testResultXml("file_writecontact");
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
	DBusMessage* msg1;
	DBusMessageIter append_iter;
	DBusMessageIter return_iter;
	DBusMessageIter sub_iter;
	DBusPendingCall* pending;
	char error_name[40];
	char error_msg[40];
	
	
	char* arg_str = "DBus Testing";
	char* arg_obj_path = "\Test\Object\Path";
	int size,i;
	dbus_int32_t return_value;
	char ret_msg[20]; 
	
	struct data_contact
	{
		char *name;
		char *ph_no;
		dbus_int32_t cnt;
	}data;
	size = sizeof(struct data_contact);
	std_log(LOG_FILENAME_LINE, "Size of struct is %d",size);
		
	data.name = "DBus";
	data.ph_no = "+91234455";
		
	dbus_error_init(&error);
	
	connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
	if(!connection)
	{
		sprintf(error_name, "Error_name : %s", error.name);
		std_log(LOG_FILENAME_LINE, "Connection is not been established");
		sprintf(error_msg, "Error_msg : %s", error.message);
		create_xml(1);
		return 1;
	}  
	msg = dbus_message_new_method_call("Test.Method.Call1", "/Test/Method/Object", "test.Method.Call", "file_writecontact");
	if(msg == NULL)
		{ 
		std_log(LOG_FILENAME_LINE, "Message error");
		create_xml(1);
		return 1;
		}
	std_log(LOG_FILENAME_LINE, "Message is created successfully");
	
	dbus_message_iter_init_append(msg, &append_iter);
	
	if(!dbus_message_iter_append_basic(&append_iter, DBUS_TYPE_STRING, &arg_str))
		{
		std_log(LOG_FILENAME_LINE, "Out of memory");
		create_xml(1);
		return 1;
		}
	
	// structure 		 
	for(i=0;i<=50; i++)  
	{
	if(!dbus_message_iter_open_container(&append_iter, DBUS_TYPE_STRUCT, NULL, &sub_iter))
		{
		std_log(LOG_FILENAME_LINE, "Out of memory ");
		create_xml(1); 
		return 1;
		}	     
		data.cnt = i; 
		if(!dbus_message_iter_append_basic(&sub_iter, DBUS_TYPE_INT32, &data.cnt))
		{ 
			std_log(LOG_FILENAME_LINE, "out of memory");
			create_xml(1);
			return 1;  
		} 
		if(!dbus_message_iter_append_basic(&sub_iter, DBUS_TYPE_STRING, &data.name))
		{ 
			std_log(LOG_FILENAME_LINE, "out of memory");
			create_xml(1);
			return 1;  
		} 
		if(!dbus_message_iter_append_basic(&sub_iter, DBUS_TYPE_STRING, &data.ph_no))
		{ 
			std_log(LOG_FILENAME_LINE, "Out of memory");
			create_xml(1);
			return 1;  
		} 
		dbus_message_iter_close_container(&append_iter, &sub_iter);  // for 80 structure
	}
	
	// send message and get a handle for a reply
	   if (!dbus_connection_send_with_reply (connection, msg, &pending, -1)) { // -1 is default timeout
	   	std_log(LOG_FILENAME_LINE, "Message send error");
	   	create_xml(1);
	   		exit(1);
	   }   
	   if (NULL == pending) {
	   	std_log(LOG_FILENAME_LINE, "pending is null");
	   		create_xml(1);
	      exit(1);
	   }
	   dbus_connection_flush(connection);
	   
	   // free message
	   dbus_message_unref(msg); 
	  
	   // block until we recieve a reply
	   dbus_pending_call_block(pending);
	
	   // get the reply message
	   msg1 = dbus_pending_call_steal_reply(pending);
	   if (NULL == msg1) { 
	   	std_log(LOG_FILENAME_LINE, "Reply error");
	    create_xml(1);
	      exit(1);
	   } 
	   
	   // free the pending message handle
	   dbus_pending_call_unref(pending);
		 
		dbus_error_init (&error);
		dbus_message_iter_init(msg1, &return_iter);

		dbus_message_iter_get_basic(&return_iter, &return_value);
		sprintf(ret_msg,"Reply = %d", return_value); 
		std_log(LOG_FILENAME_LINE, "%s",ret_msg);
			if(return_value != 50)
			{
			std_log(LOG_FILENAME_LINE, "Return value is not what is sent");
			create_xml(1);
			return 1;
			} 
		  
		   // free reply and close connection
		   dbus_message_unref(msg);  
		  dbus_connection_unref(connection);
		  std_log(LOG_FILENAME_LINE, "Success");
			create_xml(0);
		    return 0;  
	
	}
	