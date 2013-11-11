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
#include<ctype.h>
#include <pthread.h> 
#include <string.h>
#include <errno.h>
#include<unistd.h>   
 
int handle_error(DBusError* error)
	{
	printf( "%s", error->name);
	
	printf( "%s", error->message);
	
	dbus_error_free(error);
	return 1; 
	} 

int main()
{
	FILE* fp;
	unsigned char arr[200];
	unsigned char* data;
	const unsigned char* p_arr = arr;
	DBusError error;
	DBusConnection* connection;
	DBusMessage* msg;
	DBusMessage* reply;
	DBusMessageIter append_iter;
	DBusMessageIter sub_iter;
	int num=1;
	int size=0;
	
	dbus_error_init(&error); 
	
	connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
	
	if(!connection || dbus_error_is_set(&error))
		return handle_error(&error);
	  
	fp = fopen("c:\\private\\101FB3E3\\test.mp3", "r"); 
	if(!fp) 
	{
		printf("Not able to open File");
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
	fp = fopen("c:\\private\\101FB3E3\\test.mp3", "r"); 
	data = (unsigned char*)dbus_malloc(size);
	//data = new unsigned char[size];
	if(!data)
		{
		printf("Errno %d", errno);
		printf("data is NULL.");
		return 1;
		}
	if(size != fread(data, sizeof(unsigned char), size, fp))
	{
	printf("Error while reading file");
		return 1; 
	}
	fclose(fp);
	msg = dbus_message_new_method_call("Test.Method.Call", "/Test/Method/Object", "test.Method.Call", "file_send_whole");
	if(!msg)
	{ 
	printf("msg is NULL.");
		return 1;
	} 
	 
	printf("msg is created"); 
	  
	dbus_message_iter_init_append(msg, &append_iter);
	// For Array	
	if(!dbus_message_iter_open_container(&append_iter, DBUS_TYPE_ARRAY, DBUS_TYPE_BYTE_AS_STRING, &sub_iter))
		{
		printf("Out Of Memory");
		return 1;
		}	 
	if(!dbus_message_iter_append_fixed_array(&sub_iter, DBUS_TYPE_BYTE, &data, size))
		{ 
		printf("Out Of Memory");
		return 1;  
		}   
	dbus_message_iter_close_container(&append_iter, &sub_iter);
	
	printf("Array Appended"); 
	reply = dbus_connection_send_with_reply_and_block(connection, msg, -1, &error);

	if(!reply)
		return 1;
	dbus_message_unref(msg);
	free(data);	
	
	if(fp)
		fclose(fp);
	 
	return 0;


}
