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
#include <sys/stat.h>

#define LOG_FILE "c:\\logs\\match_rule_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
	{
	if(result)
		assert_failed = 1;
	
	testResultXml("match_rule");
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

int send_message(DBusConnection* connection, char* path, char* iface, char* member)
	{
		DBusMessage* msg;
		char* str = "DBus Testing.";
		
		msg = dbus_message_new_signal(path, iface, member);
		
		if(!msg) 
		{ 
			std_log(LOG_FILENAME_LINE, "msg is NULL");
			assert_failed = 1;
			return 1;
		}
		 
		if(!dbus_message_append_args(msg, DBUS_TYPE_STRING, &str, DBUS_TYPE_INVALID))
		{
			std_log(LOG_FILENAME_LINE, "Fail to append args");
			assert_failed = 1;
			return 1;
		}
		 
		dbus_connection_send(connection, msg, NULL);
		dbus_connection_flush(connection);	 
		dbus_message_unref(msg);
		return 0;
	}

int main()
{
	DBusConnection* connection;
	DBusError error;
	DBusMessage* msg;
	char* str;
	DBusObjectPathVTable vtable ={ NULL,NULL,NULL };
	char buf[180] = ""; 
	FILE* fp = NULL;
	
	int fd;
	const char* fifopath = "C:\\mkfifo1.file";
	int err;
	err = mkfifo (fifopath, 0666);
	   if(err != 0)
	   {
	       // probably file already exists, delete the file
	       unlink(fifopath); 
	       // try once more..
	       err = mkfifo (fifopath, 0666);
	       if(err != 0)
	       {
	       		std_log(LOG_FILENAME_LINE, "Failed to make FIFO");
	       		create_xml(1);
	            return 1;
	       }
	   }
	
	#if defined(__WINSCW__) || defined(__WINS__)
	fp = popen("Z:\\sys\\bin\\match_rule_server.exe", "r");
	#else
	fp = popen("C:\\sys\\bin\\match_rule_server.exe", "r");
	#endif
	if(!fp)
		std_log(LOG_FILENAME_LINE, "fp is NULL"); 
	
	//checkpoint 1. server up and registered to the dbus.	
	fd = open(fifopath, O_RDONLY);
	if(fd > 0)
	{
		err = read (fd, buf, 20);  
		close(fd);
	}
	else
		{
		std_log(LOG_FILENAME_LINE, "Error in FIFO open().");
   		create_xml(1);
		return 1;
		}
	//unlink(fifopath);
	if(strcmp("done1", buf))
		{
		std_log(LOG_FILENAME_LINE, "done is not returned from server.");
   		create_xml(1);
		return 1;
		}
	
	dbus_error_init(&error);
	connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
	 
	if(!connection || dbus_error_is_set(&error))
		return handle_error(&error);
	  
	send_message(connection, "/Test/Signal/Object", "Test.Signal.Send2", "first");
	
	std_log(LOG_FILENAME_LINE, "First part over");
	
	dbus_bus_add_match(connection, "type='signal',interface='Test.Signal.Send3'",&error);
	
	if(dbus_error_is_set(&error))
		return handle_error(&error);
	
	std_log(LOG_FILENAME_LINE, "Registering path");
	if(!dbus_connection_register_object_path (connection, "/Test/Signal/Object1", &vtable, NULL))
	{ 
		std_log(LOG_FILENAME_LINE, "Registering path fail");
   		create_xml(1);
		return 1;
	}
	std_log(LOG_FILENAME_LINE, "Requesting name");	
	
	if(!dbus_bus_request_name (connection, "test.Signal.Send1", DBUS_NAME_FLAG_ALLOW_REPLACEMENT, &error) == -1)
	{
		std_log(LOG_FILENAME_LINE, "Requesting name fail");	
   		create_xml(1);
		return 1;
	}
	
	std_log(LOG_FILENAME_LINE, "Starting while loop");
	while(TRUE)  
	{	
		dbus_connection_read_write(connection, 0);
		
		msg = dbus_connection_pop_message(connection);
			
		if(msg == NULL)
		{
			continue; 
		} 
		
		std_log(LOG_FILENAME_LINE, "Message Detected");
	
		if(dbus_message_is_signal(msg, "Test.Signal.Send3", "second"))
		{
			if(!dbus_message_get_args(msg, &error, DBUS_TYPE_STRING, &str, DBUS_TYPE_INVALID))
				{
					std_log(LOG_FILENAME_LINE, "Error while retriving arguments :: %s", error.name);
					dbus_error_free(&error);
		       		create_xml(1);
					return 1;
				}  
			std_log(LOG_FILENAME_LINE, "Got MSG : %s",  str); 
			break; 
		}    
	
		dbus_message_unref(msg); 
	} 
	
	// Checkpoint 2. match rule on server removed
	fd = open(fifopath, O_RDONLY);
    if (fd > 0)
        {
        err = read(fd, buf, 20);
        close(fd);
        }
    else
        {
        std_log(LOG_FILENAME_LINE, "2. Error in FIFO open().");
        create_xml(1);
        return 1;
        }
      unlink(fifopath);
    if (strcmp("done2", buf))
        {
        std_log(LOG_FILENAME_LINE, "2. done is not returned from server.");
        create_xml(1);
        return 1;
        }
		
	send_message(connection, "/Test/Signal/Object", "Test.Signal.Send2", "third");
	send_message(connection, "/Test/Signal/Object", "Test.Signal.Send2", "first");	
	send_message(connection, "/Test/Signal/Object", "Test.Signal.Send2", "third");
	
	while(TRUE)
	{
		dbus_connection_read_write(connection, 0);
		msg = dbus_connection_pop_message(connection);
		if(!msg)
			continue;
		if(!dbus_message_has_destination(msg, "test.Signal.Send1"))
		{
			std_log(LOG_FILENAME_LINE, "dbus_message_has_destination() failed to check Destination.");
       		create_xml(1);
			return 1;
		}
		
		if(dbus_message_get_type(msg) == DBUS_MESSAGE_TYPE_ERROR)
		{
			std_log(LOG_FILENAME_LINE, "Error is returned.");
			if(!dbus_message_has_member(msg, "error"))
				std_log(LOG_FILENAME_LINE, "dbus_message_has_member() failed to check member.");
       		create_xml(1);
			return 1;
		}
		else if(dbus_message_get_type(msg) == DBUS_MESSAGE_TYPE_METHOD_CALL)
			{
			std_log(LOG_FILENAME_LINE, "Method call is returned.");
			if(!dbus_message_has_member(msg, "success"))
			{
				std_log(LOG_FILENAME_LINE, "dbus_message_has_member() failed to check member.");
	       		create_xml(1);
				return 1;
			}
			break; 
			}
		
	}
	dbus_connection_unref(connection);
	if(fp) 
		pclose(fp);
	
	std_log(LOG_FILENAME_LINE, "Test Successful");  
	create_xml(0);
	return 0; 
}
