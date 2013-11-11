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


#include "test-utils.h"
#include <fcntl.h>
#include <string.h>  
  
void handle_error(char* str)
{
	FILE* fp = fopen("C:\\match_rule_data.txt", "a+");
	if(fp)
	{
		fwrite(str, strlen(str), 1, fp);
		fclose(fp);
	} 
}

int check_signal(DBusMessage* msg, char* iface, char* member)
{
	DBusError error;
	char buf[80]; 
	char* str;
	
	dbus_error_init(&error);
	if(dbus_message_is_signal(msg, iface, member))
	{
		printf("\n\nGot the new Signal.\n");
		if(!dbus_message_get_args(msg, &error, DBUS_TYPE_STRING, &str, DBUS_TYPE_INVALID))
		{
			handle_error("\nError while retrieving arguments");
			exit(1);
		}
		sprintf(buf, "\nSignal %s received is %s", member, str);
		handle_error(buf);
		return 1;
	}
	return 0;
}

int send_reply(DBusConnection* connection,int type, char* dest, char* path, char* iface, char* member)
{
	DBusMessage* reply;
	reply = dbus_message_new(type);
	if(!dbus_message_set_destination(reply, dest))
		return 1;
	if(!dbus_message_set_path(reply, path))
		return 1;
	if(!dbus_message_set_interface(reply, iface))
		return 1;
	if(!dbus_message_set_member(reply, member))
		return 1;
	if(type == DBUS_MESSAGE_TYPE_ERROR)
		if(!dbus_message_set_error_name(reply, "Test.Signal.Send.RemoveMatchRuleError"))
			return 1;
	
	dbus_connection_send(connection, reply, NULL);
	dbus_connection_flush(connection);
	return 0;
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
	int cnt = 0;
	//Note: fifo file used by tests: match_rule, dbus_match_rule
	const char* fifopath = "C:\\mkfifo1.file";
	int err;
	int fd;
	char* buf1;
	
	dbus_error_init(&error);
		 
	connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
	  
	if(dbus_error_is_set(&error))  
	{ 
		sprintf(buf, "\nError Occured :: %s\n%s", error.name, error.message);
		handle_error(buf);
		return 1;
	}
	
	dbus_bus_add_match(connection, "type='signal',interface='Test.Signal.Send2',member='first'",&error);
	dbus_bus_add_match(connection, "type='signal',interface='Test.Signal.Send2',member='third'",&error);
	
	if(dbus_error_is_set(&error))
	{
		sprintf(buf, "\nError :: %s\n%s", error.name, error.message);
		handle_error(buf);
		return 1;
	}
	
	if(!dbus_connection_register_object_path (connection, "/Test/Signal/Object", &vtable, NULL))
	{
		handle_error("Not able to register path.");
		return 1;
	}
	
	if(!dbus_bus_request_name (connection, "test.Signal.Send", DBUS_NAME_FLAG_ALLOW_REPLACEMENT, &error) == -1)
	{
		handle_error("Not able to Request name.");
		return 1;
	}
	
	fd = open(fifopath, O_WRONLY);
	if(fd > 0)
	{
		buf1 = "done1"; //for checkpoint 1
		err = write (fd, buf1, strlen(buf1)); 
		close(fd);
	}
	while(TRUE)
	{	
		dbus_connection_read_write(connection, 0);
		msg = dbus_connection_pop_message(connection);
		if(msg == NULL)
		{ 
		//	sleep(1);
			continue; 
		}	
		
		if(check_signal(msg, "Test.Signal.Send2", "first"))
			break;
		if(check_signal(msg, "Test.Signal.Send2", "third"))
			break;
		
		dbus_message_unref(msg);
	} 
	str = "server reply.";
	msg = dbus_message_new_signal("/Test/Signal/Object1", "Test.Signal.Send3", "second");
	 
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
	
	dbus_bus_remove_match(connection, "type='signal',interface='Test.Signal.Send2',member='first'", &error);
	if(	dbus_error_is_set(&error))
	{
		sprintf(buf, "Error :: %s\n%s", error.name, error.message);
		handle_error(buf);
	}
	
	fd = open(fifopath, O_WRONLY);
    if (fd > 0)
        {
        buf1 = "done2"; //for checkpoint 2
        sleep(2);
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
		
		if(check_signal(msg, "Test.Signal.Send2", "first"))
		{
			if(send_reply(connection, DBUS_MESSAGE_TYPE_ERROR, "test.Signal.Send1", "/Test/Signal/Object1", NULL, "error"))
				break; 
		
			break;//cnt++;
		}
		
		if(check_signal(msg, "Test.Signal.Send2", "third"))
		{ 
			cnt++;
			if(cnt==2)
			{
				if(send_reply(connection, DBUS_MESSAGE_TYPE_METHOD_CALL, "test.Signal.Send1", "/Test/Signal/Object1", NULL, "success"))
					break;
			
				break; 
			}
		} 
		dbus_message_unref(msg);
	}
	
	dbus_connection_unref(connection);
	return 0; 
}
