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
#include <semaphore.h>

#define MAX_THREAD 25

typedef struct{
pthread_mutex_t mutex;
pthread_cond_t cond;
int ret; 
}threadData1;

#define LOG_FILE "c:\\logs\\send_messages1_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

//semaphore to keep in sync with thread creation and connect to d-bus
sem_t threadCountingSem; 

void create_xml(int result)
	{
	if(result)
		assert_failed = 1;
	
	testResultXml("send_messages1");
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

static void* send_msg1(void* data)
{
	DBusConnection* connection;
	DBusError error;
	static int cnt = 1;
	dbus_int32_t no = 5;
	DBusPendingCall* pending;
	DBusMessage* msg1;
	DBusMessage* msg;
	int data_slot = *(int*)data;
	threadData1* thrData;

	dbus_error_init(&error);
	connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
	if(!connection)
		{
		handle_error(&error);
		return NULL;
		}
	
	thrData = (threadData1*)dbus_connection_get_data(connection, data_slot);
	if(!thrData)
		return NULL;
	
	pthread_mutex_lock(&thrData->mutex);
	
	msg = dbus_message_new_method_call("Test.Method.Call", "/Test/Method/Object", "test.Method.Call", "simple");
	
	dbus_message_append_args(msg, DBUS_TYPE_INT32, &no, DBUS_TYPE_INVALID);
	
	//Done connecting to DBUS, ready to receive the broadcast signal from main
    if( sem_post(&threadCountingSem) < 0 ){
        std_log(LOG_FILENAME_LINE, "error sem_post");
    }

	pthread_cond_wait(&thrData->cond,  &thrData->mutex);
	 
	// send message and get a handle for a reply
	   if (!dbus_connection_send_with_reply (connection, msg, &pending, -1)) { // -1 is default timeout
	   thrData->ret = 2;
	   }   
	   if (NULL == pending) {
	   thrData->ret = 2;
	   } 
	   dbus_connection_flush(connection);
	   
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
		 
	  
	   dbus_message_get_args(msg1, &error, DBUS_TYPE_INT32, &no, DBUS_TYPE_INVALID);
	   
	   std_log(LOG_FILENAME_LINE, "%d\n", no);
	   
	   if(no == 9090)
		   {
		   thrData->ret++;
		   }
	   
	 	 
	   // free reply and close connection
	   dbus_message_unref(msg1); 
	   dbus_connection_unref(connection);
	   pthread_mutex_unlock(&thrData->mutex); 
	   return NULL;
}
 
int main()
{
	DBusConnection* connection;
	DBusError error;
	int cnt;
	int data_slot = -1;
	
	pthread_t thread[MAX_THREAD];
	int thrVal[MAX_THREAD]={0};
	void* thrValPtr[MAX_THREAD];
	threadData1 thrData;
	
	for(cnt=0; cnt<MAX_THREAD; cnt++)
		thrValPtr[cnt] = (void*)&thrVal[cnt];	
	 
	dbus_error_init(&error);
	connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
	if(!connection || dbus_error_is_set(&error))
		return handle_error(&error);
	
	pthread_mutex_init(&thrData.mutex, NULL);
	pthread_cond_init(&thrData.cond, NULL);
		thrData.ret = 0;
		
    // initializing the semaphore
    if( sem_init(&threadCountingSem, 0, 0) < 0 ){
        std_log(LOG_FILENAME_LINE, "Error sem_init()\n");
    }

		dbus_connection_allocate_data_slot(&data_slot);
	dbus_connection_set_data(connection, data_slot, &thrData, NULL);
		
		dbus_threads_init_default();
	
		for(cnt=0; cnt<MAX_THREAD; cnt++) {
			pthread_create(&thread[cnt], NULL, &send_msg1, &data_slot);
			//wait for each thread to connect to DBUS and be prepared to
			//handle the cond_broadcast
			sem_wait(&threadCountingSem);
		}
	 

	pthread_cond_broadcast(&thrData.cond);

	//semaphore not required anymore.
    sem_destroy(&threadCountingSem);
	
	for(cnt=0; cnt<MAX_THREAD; cnt++)
		pthread_join(thread[cnt], &thrValPtr[cnt]); 
	 
	if(thrData.ret != MAX_THREAD)
	{ 
		std_log(LOG_FILENAME_LINE, "No. of threads crashed %d", (MAX_THREAD - thrData.ret));
		create_xml(1);
		return 1;
	}
	 
	dbus_connection_unref(connection);
	
	std_log(LOG_FILENAME_LINE, "Test Successful"); 
	create_xml(0);
	return 0;
}
