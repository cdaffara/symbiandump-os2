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

#define LOG_FILE "c:\\logs\\file_send_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
	{
	if(result)
		assert_failed = 1;
	
	testResultXml("file_send");
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
	FILE* fp;
	unsigned char arr[201];
	const unsigned char* p_arr = arr;
	DBusError error;
	DBusConnection* connection;
	DBusMessage* msg;
	int num;
	int i=0;
	
	dbus_error_init(&error); 
	
	connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
	
	if(!connection || dbus_error_is_set(&error))
		return handle_error(&error);
	  
	fp = fopen("c:\\bunbask1.jpg", "r"); 
	if(!fp) 
	{
		std_log(LOG_FILENAME_LINE, "Not able to open File");
		create_xml(1);
		return 1;
	}  
	 
	while (!feof(fp)) 
	{ 
	    num = fread(&arr, sizeof(unsigned char), 200, fp);
	    i++;

	    msg = dbus_message_new_method_call("Test.Method.Call", "/Test/Method/Object", "test.Method.Call", "file_send");
	    if(!msg)
	    { 
	    	std_log(LOG_FILENAME_LINE, "msg is NULL.");
	    	create_xml(1);
	    	return 1;
	    } 
	     
	    std_log(LOG_FILENAME_LINE, "msg is created"); 
	    
	    std_log(LOG_FILENAME_LINE, "num = %d", num); 
	    if(!dbus_message_append_args(msg, DBUS_TYPE_ARRAY, DBUS_TYPE_BYTE, &p_arr, num, DBUS_TYPE_INVALID))
	    { 
	    	std_log(LOG_FILENAME_LINE, "Fail to append ARRAY.");
	    	create_xml(1);
	    	return 1;
	    }
	    
	    std_log(LOG_FILENAME_LINE, "Array Appended"); 
	    dbus_connection_send_with_reply_and_block(connection, msg, -1, &error);
		if(dbus_error_is_set(&error))
		{
			std_log(LOG_FILENAME_LINE, "Test Fail with error %s", error.message);
			create_xml(1);
			exit(1);
		}
		dbus_connection_flush(connection);
		
		dbus_message_unref(msg);
		msg=NULL;
	} 
	if(fp)
		fclose(fp);
	
	std_log(LOG_FILENAME_LINE, "Test Successful");
	create_xml(0);
	return 0;
}
