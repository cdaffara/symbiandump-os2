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
#include <dbus/dbus-glib-bindings.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>


int signal_first;

static void
hello_signal_handler (DBusGProxy *proxy, const char *hello_string, gpointer user_data)
{
  	FILE* fp = fopen("c:\\signal2.txt", "a+");
  	if(fp)
  		{
  		fprintf(fp, "Signal Received in first handler : %s\n", hello_string);
  		fclose(fp);
  		}
	fprintf (fp,"Received signal and it says: %s\n", hello_string);
	signal_first++;
}

static void
hello_signal_handler1 (DBusGProxy *proxy, const char *hello_string, gpointer user_data)
{
	static int signal_second = 0;
	int res = 0;
  	FILE* fp = fopen("c:\\signal2.txt", "a+");
  	
  	const char* fifopath = "C:\\mkfifo7.file";
  	int err;
	int fd;
	char* buf1 = NULL;
	
  	if(fp)
  		{
  		fprintf(fp, "Signal Received in second handler : %s\n", hello_string);
  		fclose(fp);
  		}
  	dbus_g_proxy_disconnect_signal (proxy, "HelloSignal", G_CALLBACK (hello_signal_handler), NULL);
	fprintf (fp,"Received signal and it says: %s\n", hello_string);
	
	signal_second++;
	if(signal_second == 2)
	{
		fd = open(fifopath, O_WRONLY);
		sleep(2);
		if(fd > 0)
		{
			if(signal_first == 1)
				buf1 = "1";
			if(signal_first == 2)
				buf1 = "2";
			if(signal_first == 0)
				buf1 = "0";
			err = write (fd, buf1, strlen(buf1));
			close(fd);
		}
		
	}
}


int main()
{

	DBusGConnection *bus;
	GMainLoop *mainLoop = NULL;
	DBusGProxy* proxy;
	GError *error = NULL;
   	int fd;
   	int err;
    const char* fifopath = "C:\\mkfifo8.file";
    const char* buf1 = "done";
	g_type_init();


	bus = dbus_g_bus_get(DBUS_BUS_SESSION,NULL);

	proxy = dbus_g_proxy_new_for_name (bus,
		     "org.designfu.TestService",
		     "/org/designfu/TestService/object",
		     "org.designfu.TestService");


	/* Tell DBus what the type signature of the signal callback is; this
	   * allows us to sanity-check incoming messages before invoking the
	   * callback.  You need to do this once for each proxy you create,
	   * not every time you want to connect to the signal.
	   */
	  dbus_g_proxy_add_signal (proxy, "HelloSignal", G_TYPE_STRING, G_TYPE_INVALID);

	  /* Actually connect to the signal.  Note you can call
	   * dbus_g_proxy_connect_signal multiple times for one invocation of
	   * dbus_g_proxy_add_signal.
	   */
	  dbus_g_proxy_connect_signal (proxy, "HelloSignal", G_CALLBACK (hello_signal_handler),
				       NULL, NULL);
	  dbus_g_proxy_connect_signal (proxy, "HelloSignal", G_CALLBACK (hello_signal_handler1),
		       NULL, NULL);
	   fd = open(fifopath, O_WRONLY);
	   sleep(2);
       if (fd > 0)
          {
          err = write(fd, buf1, strlen(buf1));
          close(fd);
          }
	mainLoop = g_main_loop_new(NULL,FALSE);
	g_main_loop_run(mainLoop);
	
	return 0;
}

