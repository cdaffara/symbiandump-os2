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

void start_time();
void end_time();
long time_diff();

#define LOG_FILE "c:\\logs\\file_transfer_time_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
	{
	if(result)
		assert_failed = 1;
	
	testResultXml("file_transfer_time");
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

int send_file(DBusConnection* connection, char* file_name)
	{
		FILE* fp;
		unsigned char arr[200];
		unsigned char* data;
		const unsigned char* p_arr = arr;
		DBusError error;
		DBusMessage* msg;
		DBusMessage* reply;
		DBusMessageIter append_iter;
		DBusMessageIter sub_iter;
		int num=1;
		int size=0;

		dbus_error_init(&error);
		
		fp = fopen(file_name, "r"); 
		if(!fp) 
		{
			std_log(LOG_FILENAME_LINE, "Not able to open File");
			return 1;
		}
		
		while(num)
		{
			num = fread(&arr, sizeof(unsigned char), 200, fp);
			size = size + num;
		}
		if(fp)
			fclose(fp);
		
		fp = NULL;
		fp = fopen(file_name, "r");
		if(!fp) 
			{
				std_log(LOG_FILENAME_LINE, "Not able to open File");
				return 1;
			}  
		data = (unsigned char*)dbus_malloc(size);
		if(!data)
			{
			std_log(LOG_FILENAME_LINE, "data is NULL for %s of size %d.", file_name, size);
			return 1;
			}
		if(size != fread(data, sizeof(unsigned char), size, fp))
		{
			std_log(LOG_FILENAME_LINE, "Error while reading file");
			return 1; 
		}
		if(fp)
			fclose(fp);
		msg = dbus_message_new_method_call("Test.Method.Call", "/Test/Method/Object", "test.Method.Call", "file_send_time");
		if(!msg)
		{ 
			std_log(LOG_FILENAME_LINE, "msg is NULL.");
			return 1;
		} 
		 
		std_log(LOG_FILENAME_LINE, "msg is created"); 
		  
		dbus_message_iter_init_append(msg, &append_iter);
		dbus_message_iter_append_basic(&append_iter, DBUS_TYPE_STRING, &file_name);
		// For Array	
		if(!dbus_message_iter_open_container(&append_iter, DBUS_TYPE_ARRAY, DBUS_TYPE_BYTE_AS_STRING, &sub_iter))
			{
			std_log(LOG_FILENAME_LINE, "Out Of Memory");
			return 1;
			}	 
		if(!dbus_message_iter_append_fixed_array(&sub_iter, DBUS_TYPE_BYTE, &data, size))
			{ 
			std_log(LOG_FILENAME_LINE, "Out Of Memory");
			return 1;  
			}   
		dbus_message_iter_close_container(&append_iter, &sub_iter);
		
		std_log(LOG_FILENAME_LINE, "Array Appended");
		start_time();
		reply = dbus_connection_send_with_reply_and_block(connection, msg, 15000, &error);
		end_time();
		if(!reply)
		{
			std_log(LOG_FILENAME_LINE, "Failed to get reply for %s of size %d", file_name, size);
			std_log(LOG_FILENAME_LINE, "Error Message :: %s", error.message);
			return 1;
		}
		
		std_log(LOG_FILENAME_LINE, "Time to send %s file of size %d is %d", file_name, size, time_diff());
		dbus_free(data);
		
		return 0;
	}


int main()
{
	DBusError error;
	DBusConnection *connection;	
	
	dbus_error_init (&error);
	start_time();
	connection = dbus_bus_get (DBUS_BUS_SESSION, &error);
	end_time();
	if (connection == NULL)
	  {
	  	std_log(LOG_FILENAME_LINE, "*** Failed to open connection to system bus: %s\n%s",  error.name, error.message);
	    dbus_error_free (&error);
	    create_xml(1);
	    return 1;
	  }
	
	std_log(LOG_FILENAME_LINE, "Time to get 1st time connection is %d", time_diff());
	
	if(send_file(connection, "c:\\inbox1.jpg"))
	{
		create_xml(1);
		return 1;
	}
	sleep(1);
	
	if(send_file(connection, "c:\\bunbask1.jpg"))
	{
		create_xml(1);
		return 1;
	}
	sleep(1);
	
	if(send_file(connection, "c:\\sound_clip.amr"))
	{
		create_xml(1);
		return 1;
	}
	sleep(1);
	
	if(send_file(connection, "c:\\inbox.jpg"))
		{
		create_xml(1);
		return 1;
	}
	sleep(1);
	
	if(send_file(connection, "c:\\sound_clip1.amr"))
	{
		create_xml(1);
		return 1;
	}
	sleep(1);
	
	if(send_file(connection, "c:\\sound_clip2.amr"))
	{
		create_xml(1);
		return 1;
	}
	sleep(1);
	
	if(send_file(connection, "c:\\sound_clip4.amr"))
	{
		create_xml(1);
		return 1;
	}
	sleep(1);
	
	if(send_file(connection, "c:\\sound_clip5.amr"))
	{
		create_xml(1);
		return 1;
	}
	sleep(1);
	
	if(send_file(connection, "c:\\sound_clip6.amr"))
	{
		create_xml(1);
		return 1;
	}
	sleep(1);
	
	if(send_file(connection, "c:\\sound_clip3.amr"))
	{
		create_xml(1);
		return 1;
	}
	
	dbus_connection_unref(connection);
	std_log(LOG_FILENAME_LINE, "Test Successful");
	create_xml(0);
	return 0;
}
