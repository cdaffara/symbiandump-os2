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

#include <fcntl.h>
#include <string.h>
#include "test-utils.h"
  
void handle_error(char* str)
	{
		FILE* fp = fopen("C:\\error.txt", "a+");
		if(fp)
			{
				fwrite(str, strlen(str), 1, fp);
				fclose(fp);
			} 
	}
int main()
{
	DBusError error; 
	DBusConnection* connection;
	DBusMessage* msg;
	DBusObjectPathVTable vtable = 
	{
		NULL,NULL,NULL
	};
	
	char* str;
	char buf[80];
    char* buf1;
	//fifo file used by test cases: signal, dbus_signal
    const char* fifopath = "C:\\signalfifo.file";
        int err;
        int fd;
	dbus_error_init(&error);
		
	connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
	  
	if(dbus_error_is_set(&error))  
	{ 
		sprintf(buf, "\nError Occured :: %s\n%s", error.name, error.message);
		handle_error(buf);
		return 1;
	}
	
	dbus_bus_add_match(connection, "type='signal',interface='Test.Signal.Send'",&error);
	
	if(dbus_error_is_set(&error))
	{
		sprintf(buf, "\nError :: %s\n%s", error.name, error.message);
		handle_error(buf);
		return 1;
	}
	/*
	if(!dbus_connection_register_object_path (connection, "/Test/Signal/Object", &vtable, NULL))
	{
		return 1;
	}
	
	if(!dbus_bus_request_name (connection, "test.Signal.Send", DBUS_NAME_FLAG_ALLOW_REPLACEMENT, &error) == -1)
	{
		return 1;
	} 
    */        
    fd = open(fifopath, O_WRONLY);
    if (fd > 0)
        {
        buf1 = "done";
        err = write(fd, buf1, strlen(buf1));
        close(fd);
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
		
		printf( "Message Detected");
		
		if(DBUS_MESSAGE_TYPE_SIGNAL == dbus_message_get_type(msg))
		{
			printf( "\nMessage is Signal.");
		}
		 
		if(DBUS_MESSAGE_TYPE_METHOD_CALL == dbus_message_get_type(msg))
		{
			printf( "Message is Method call.");
		}  
				
		if(dbus_message_is_signal(msg, "Test.Signal.Send", "first"))
		{
			printf("\n\nGot the new Signal.\n");
			if(!dbus_message_get_args(msg, &error, DBUS_TYPE_STRING, &str, DBUS_TYPE_INVALID))
				{
					return 1;
				}
			printf( "\nSTRING received is %s", str);
			break;
		}
		dbus_message_unref(msg);
	}
	
	printf( "\nCreating Message");
	strcat(str, "simple_Server"); 
	msg = dbus_message_new_signal("/Test/Signal/Object1", "Test.Signal.Send1", "second");
	 
	if(!msg)   
	{  
		handle_error("\nmsg is NULL.");
		return 1; 
	}
	 
	if(!dbus_message_append_args(msg, DBUS_TYPE_STRING, &str, DBUS_TYPE_INVALID))
	{
		handle_error("\nAppending arg fail.");
		return 1;  
	} 
	dbus_connection_send(connection, msg, NULL);
	dbus_connection_flush(connection);
	dbus_connection_unref(connection);
	return 0; 
}
