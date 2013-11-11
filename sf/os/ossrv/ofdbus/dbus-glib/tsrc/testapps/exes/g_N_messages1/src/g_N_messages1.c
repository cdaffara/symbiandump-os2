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


#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include "dbus_glib1-bindings.h"
#include <dbus/dbus-glib-bindings.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_THREAD 75
typedef struct{
pthread_mutex_t mutex;
pthread_cond_t cond;
DBusGProxy* proxy;
int ret; 
}threadData1;


#define LOG_FILE "c:\\logs\\g_N_messages1_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

gint static  count = 0;// vasanth

void create_xml(int result)
	{
	if(result)
		assert_failed = 1;
	
	testResultXml("g_N_messages1");
    close_log_file();
	}

static void* send_msg1(void* data)
{
	GError *error = NULL;
	static int cnt = 1;
	gint in_num = 5;
	gint out_num = 0;
	DBusGConnection *connection;
	DBusGProxy *proxy;
	
	threadData1* thrData = (threadData1*)data;
	if(!thrData)
		return;
	
	pthread_mutex_lock(&thrData->mutex);
	
	connection = dbus_g_bus_get (DBUS_BUS_SESSION,
            &error);
	
	if (connection == NULL)
	{
	std_log(LOG_FILENAME_LINE, "Failed to open connection to bus: %s\n",
	error->message);
	g_error_free (error);
	create_xml(1);
	exit (1);
	}
	
	proxy = dbus_g_proxy_new_for_name (connection,
	                  "com.example.DBusGlibObject1",
	                  "/com/example/DBusGlibObject1",
	                  "com.example.DBusGlibObject1");

			 count++;		//vasanth 	
		
	pthread_cond_wait(&thrData->cond,  &thrData->mutex);
	 
	// send message and get a handle for a reply
	  
	  if(!com_example_DBusGlibObject1_simple_method(proxy, in_num, &out_num, &error))
		{
			create_xml(1);
			exit(1);
		}
	
	   std_log(LOG_FILENAME_LINE, "%d\n", out_num);
	   if(out_num == 9090)
		   {
		   thrData->ret++;
		   }
	   
	   dbus_g_connection_unref(connection);
	   pthread_mutex_unlock(&thrData->mutex); 
}

int main()
{	
	gint in_num = 5;
	gint out_num = 0;
	
	int cnt;
	pthread_t thread[MAX_THREAD];
	int thrVal[MAX_THREAD]={0};
	void* thrValPtr[MAX_THREAD];
	threadData1 thrData;
	
	for(cnt=0; cnt<MAX_THREAD; cnt++)
		thrValPtr[cnt] = (void*)&thrVal[cnt];
	
	g_type_init ();
	
	pthread_mutex_init(&thrData.mutex, NULL);
	pthread_cond_init(&thrData.cond, NULL);
	thrData.ret = 0;
	thrData.proxy = NULL;
	
	for(cnt=0; cnt<MAX_THREAD; cnt++)
		pthread_create(&thread[cnt], NULL, &send_msg1, &thrData);
 
	while( count!= 75)				//vasanth
	    sleep(1);
	
	pthread_cond_broadcast(&thrData.cond);
	
	for(cnt=0; cnt<MAX_THREAD; cnt++)
		pthread_join(thread[cnt], &thrValPtr[cnt]); 
	 
	if(thrData.ret != MAX_THREAD)
	{ 
		std_log(LOG_FILENAME_LINE, "No. of threads crashed %d", (MAX_THREAD - thrData.ret));
		create_xml(1);
		return 1;
	}

	std_log(LOG_FILENAME_LINE, "Test Successful");
	create_xml(0);
	return 0;
}

