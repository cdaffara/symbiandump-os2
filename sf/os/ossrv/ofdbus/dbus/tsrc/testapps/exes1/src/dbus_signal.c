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
#include <unistd.h>
#include <sys/stat.h>
#define LOG_FILE "c:\\logs\\dbus_signal_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

#define MAX_SIGNALS 3

void create_xml(int result)
{
	if(result)
		assert_failed = 1;
	
	testResultXml("dbus_signal");
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


int main()
{
	DBusConnection* connection;
	DBusError error;
	DBusMessage* msg;
	dbus_int32_t arg_int32 = 1010;
	DBusObjectPathVTable vtable =
	{
	NULL,NULL,NULL	
	};
	char* str = "DBus Testing. hjgfyh34348#$!%^45678901730952698376092869876DBus Testing. hjgfyh34348#$!%^~!@#$%^&*()_+`-=<>?:{},./;'[]45678901730952698376092869876";
	FILE* fp[MAX_SIGNALS]={NULL};
	char buf[20] = "";
	int cnt=0;
	char exe_param[100];
	char names[][40]={"test.Signal.Send3 ",
					"test.Signal.Send4 ",
					"test.Signal.Send5 "};
	char obj_path[][40]={"/Test/Signal/Object3",
					"/Test/Signal/Object4",
					"/Test/Signal/Object5"};
	int fd;
	const char* fifopath = "C:\\signalfifo.file";
	int err;
	err = mkfifo(fifopath, 0666);
	if (err != 0)
	    {
	    // probably file already exists, delete the file
        unlink(fifopath);
        // try once more..
        err = mkfifo(fifopath, 0666);
        if (err != 0)
            {
            create_xml(1);
            return 1;
            }
        }
	
	for(cnt=0;cnt< MAX_SIGNALS;cnt++)
	{
#if defined(__WINSCW__) || defined(__WINS__)
		strcpy(exe_param,"Z:\\sys\\bin\\Simple_Server.exe ");
#else
		strcpy(exe_param,"C:\\sys\\bin\\Simple_Server.exe ");
#endif
		strcat(exe_param, names[cnt]);
		strcat(exe_param, obj_path[cnt]);
		fp[cnt] = popen(exe_param, "r");
			if(!fp[cnt])
				{
				std_log(LOG_FILENAME_LINE, "fp %d is NULL", cnt); 
				create_xml(1);
				return 1;
				}
		//wait till the server is up
		fd = open(fifopath, O_RDONLY);
	    if (fd> 0)
	        {
	        err = read(fd, buf, 20);
	        close(fd);
	        }
	    else
	        {
	        std_log(LOG_FILENAME_LINE, "Error in FIFO open().");
	        create_xml(1);
	        return 1;
	        }
	    if (strcmp("done", buf))
	        {
	        std_log(LOG_FILENAME_LINE,"done is not returned from server.");
	        create_xml(1);
	        return 1;
	        }
	}
	unlink(fifopath);
	dbus_error_init(&error);
	connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
	 
	if(!connection || dbus_error_is_set(&error))
		return handle_error(&error);
	  
	msg = dbus_message_new_signal("/Test/Signal/Object", "Test.Signal.Send", "first");
	
//	sleep(2);
	if(!msg) 
	{
		std_log(LOG_FILENAME_LINE, "msg is NULL");
		create_xml(1);
		return 1;
	}
	 
	if(!dbus_message_append_args(msg, DBUS_TYPE_STRING, &str, DBUS_TYPE_INVALID))
	{
		std_log(LOG_FILENAME_LINE, "Fail to append args");
		create_xml(1);
		return 1;
	}
   dbus_bus_add_match(connection, "type='signal',interface='Test.Signal.Send1'",&error);
    
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
	
	dbus_connection_send(connection, msg, NULL);
	dbus_connection_flush(connection);
	 
	dbus_message_unref(msg);
	
	str = "";
	std_log(LOG_FILENAME_LINE, "First part over");

	cnt=0;
	while(TRUE)  
	{	
		dbus_connection_read_write(connection, 0);
		
		msg = dbus_connection_pop_message(connection);
			
		if(msg == NULL)
		{
			continue; 
		} 
		
		std_log(LOG_FILENAME_LINE, "Message Detected");

		if(dbus_message_is_signal(msg, "Test.Signal.Send1", "second"))
		{
			if(!dbus_message_get_args(msg, &error, DBUS_TYPE_STRING, &str, DBUS_TYPE_INVALID))
				{
					std_log(LOG_FILENAME_LINE, "Error while retriving arguments :: %s", error.name);
					dbus_error_free(&error);
					create_xml(1);
					return 1;
				}  
			std_log(LOG_FILENAME_LINE, "Got %d MSG : %s", cnt, str);
			cnt++;
			if(cnt==MAX_SIGNALS)
				break; 
		}  

		dbus_message_unref(msg);
	}
		 
	dbus_connection_close(connection);
	dbus_connection_unref(connection);
	dbus_shutdown();
	for(cnt=0;cnt<MAX_SIGNALS;cnt++)
		if(fp[cnt])
			pclose(fp[cnt]);

	std_log(LOG_FILENAME_LINE, "Test Successful"); 
	create_xml(0);
	return 0;
}
