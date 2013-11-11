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
#include <pthread.h>
#include <unistd.h>

#define MAX_THREAD 25

typedef struct{
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	DBusConnection* connection;
	int ret; 
}threadData;

#define LOG_FILE "c:\\logs\\send_messages_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
	{
	if(result)
		assert_failed = 1;
	
	testResultXml("send_messages");
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

static void* send_msg(void* data)
{
	static int cnt = 1;
	dbus_int32_t no = 5;
	DBusPendingCall* pending;
	DBusMessage* msg1;
	DBusMessage* msg;
	DBusError error; 
	
	threadData* thrData = (threadData*)data;
		
	pthread_mutex_lock(&thrData->mutex);
	
	msg = dbus_message_new_method_call("Test.Method.Call", "/Test/Method/Object", "test.Method.Call", "simple");
	
	dbus_message_append_args(msg, DBUS_TYPE_INT32, &no, DBUS_TYPE_INVALID);
	 
	pthread_cond_wait(&thrData->cond,  &thrData->mutex);
	 
	// send message and get a handle for a reply
	   if (!dbus_connection_send_with_reply (thrData->connection, msg, &pending, -1)) { // -1 is default timeout
	   thrData->ret = 2;
	   }   
	   if (NULL == pending) {
	   thrData->ret = 2;
	   } 
	   dbus_connection_flush(thrData->connection);
	   
		// free message
	   dbus_message_unref(msg);   
	  
	   // block until we recieve a reply
	   dbus_pending_call_block(pending);
	
	   // get the reply message
	   msg1 = dbus_pending_call_steal_reply(pending);
	   if (NULL == msg1) {
	   thrData->ret = 2;
	
	   }  
	   // free the pending message handle
	   dbus_pending_call_unref(pending);
		 
	   dbus_error_init(&error);
	   dbus_message_get_args(msg1, &error, DBUS_TYPE_INT32, &no, DBUS_TYPE_INVALID);
	  
	   std_log(LOG_FILENAME_LINE, "%d\n", no);
	   
	   if(no == 9090)
		   {
		   thrData->ret++;
		   }
	   
	 	 
	   // free reply and close connection
	   dbus_message_unref(msg1); 
	   pthread_mutex_unlock(&thrData->mutex); 
	   return NULL;
}
 
int main()
{
	DBusError error;
	int cnt;
	
	threadData thrData;
	unsigned int thread[MAX_THREAD];
	int thrVal[MAX_THREAD]={0};
	void* thrValPtr[MAX_THREAD];
	
	for(cnt=0; cnt<MAX_THREAD; cnt++)
		thrValPtr[cnt] = (void*)&thrVal[cnt];
	 
	dbus_error_init(&error);
	thrData.connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
	if(!thrData.connection || dbus_error_is_set(&error))
		return handle_error(&error);
	
	pthread_mutex_init(&thrData.mutex, NULL);
	pthread_cond_init(&thrData.cond, NULL);
	thrData.ret = 0;

	for(cnt=0; cnt<MAX_THREAD; cnt++)
		pthread_create(&thread[cnt], NULL, &send_msg, &thrData);
 
	sleep(1);  
	
	pthread_cond_broadcast(&thrData.cond);
	
	for(cnt=0; cnt<MAX_THREAD; cnt++)
		pthread_join(thread[cnt], &thrValPtr[cnt]); 
	 
	if(thrData.ret != MAX_THREAD)
	{ 
		std_log(LOG_FILENAME_LINE,"No. of threads crashed %d", (MAX_THREAD - thrData.ret));
		create_xml(1);
		return 1;
	}
	 
	dbus_connection_unref(thrData.connection);
	
	std_log(LOG_FILENAME_LINE, "Test Successful"); 
	create_xml(0);
	return 0;
}
