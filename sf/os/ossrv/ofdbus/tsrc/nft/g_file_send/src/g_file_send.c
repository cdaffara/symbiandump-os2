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
#include "file_send-bindings.h"
#include <dbus/dbus-glib-bindings.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

void start_time();
void end_time();
long time_diff();

#define MAX_TRY 10

#define LOG_FILE "c:\\logs\\g_file_send_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
	{
	if(result)
		assert_failed = 1;
	
	testResultXml("g_file_send");
    close_log_file();
	}

int send_file(DBusGProxy* proxy, char* file_name)
	{
		GError *error = NULL;
		unsigned int SIZE;
		
		GArray *array;
		guint length;
		FILE *fp = NULL;
		
		struct stat stat_buf;
		gchar *ptr = NULL;
		int temp;
		long diff;
		long diff_int64 = 0;
		int i;
		
		stat(file_name,&stat_buf);
		
		SIZE = stat_buf.st_size;
		ptr = (gchar*)g_malloc(SIZE);
		fp = fopen(file_name,"r");
		if(!fp)
			{
			std_log(LOG_FILENAME_LINE, "fp is NULL for %s", file_name);
			create_xml(1);
			return 1;
			}
		
		fread(ptr,1,SIZE,fp);
		
		array = g_array_new(FALSE,FALSE,1);
		g_array_append_vals(array,ptr,SIZE);
		
		std_log(LOG_FILENAME_LINE,"File %s size %d", file_name, SIZE);
		start_time();
		if(!com_example_SomeObject_method1(proxy,SIZE,array,&length,&error))
			{
			end_time();
			diff = time_diff();
			std_log(LOG_FILENAME_LINE, "may be time out of %d micro-sec for size %d", diff, SIZE);
			std_log(LOG_FILENAME_LINE," %s",error->message);
			create_xml(1);
				return 1;
			}
		end_time();
		diff = time_diff();
		std_log(LOG_FILENAME_LINE, "1st time %d micro-sec.", diff);

		for(i=0;i<MAX_TRY;i++)
		{
			start_time();
			if(!com_example_SomeObject_method1(proxy,SIZE,array,&length,&error))
			{
			end_time();
			diff = time_diff();
			std_log(LOG_FILENAME_LINE, "may be time out of %d micro-sec.", diff);
			std_log(LOG_FILENAME_LINE," %s",error->message);
			create_xml(1);
				return 1;
			}
			end_time();
			
			temp = time_diff();
			diff_int64 += temp;
			std_log(LOG_FILENAME_LINE, "Trial %d : %d", i, temp);
		}	
		diff_int64 = diff_int64/MAX_TRY;
		std_log(LOG_FILENAME_LINE, "\nAverage Time to get file %s of size %d is %d\n", file_name, length, diff_int64);
		
		fclose(fp);
		
		return 0;
	}

int main()
{
	DBusGConnection *connection;
	GError *error;
	DBusGProxy *proxy;
	
	long diff;
	
	g_type_init ();
	
	error = NULL;
	start_time();
	connection = dbus_g_bus_get (DBUS_BUS_SESSION,
	                           &error);
	end_time();
	diff = time_diff();
	std_log(LOG_FILENAME_LINE, "Time to get 1st time Connection %d", diff);
	
	if (connection == NULL)
	{
		std_log(LOG_FILENAME_LINE, "Failed to open connection to bus: %s\n",
	              error->message);
	  g_error_free (error);
	  create_xml(1);
	  exit (1);
	}
	
	proxy = dbus_g_proxy_new_for_name (connection,
	                                 "com.example.SomeObject",
	                                 "/com/example/SomeObject",
	                                 "com.example.SomeObject");
	
	error = NULL;
	
	if(send_file(proxy, "C:\\inbox1.jpg"))
		return 1;
	if(send_file(proxy, "C:\\bunbask1.jpg"))
		return 1;
	if(send_file(proxy, "C:\\sound_clip.amr"))
		return 1;
	if(send_file(proxy, "C:\\sound_clip1.amr"))
		return 1;
	if(send_file(proxy, "C:\\sound_clip2.amr"))
		return 1;
	if(send_file(proxy, "C:\\sound_clip4.amr"))
		return 1;
	if(send_file(proxy, "C:\\sound_clip5.amr"))
		return 1;
	if(send_file(proxy, "C:\\sound_clip3.amr"))
		return 1;
	
	dbus_g_connection_unref(connection);
	std_log(LOG_FILENAME_LINE, "Test Successful");
	create_xml(0);
	return 0;
}

