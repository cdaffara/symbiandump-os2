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
#include <pthread.h>

#define LOG_FILE "c:\\logs\\g_signal_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
const char* fifopath1 = "C:\\mkfifo8.file";

void create_xml(int result)
	{
	if(result)
		assert_failed = 1;
	
	testResultXml("g_signal");
    close_log_file();
	}

int handle_error(GError* error)
	{
	std_log(LOG_FILENAME_LINE,"%s", error->message);
	create_xml(1);
	return 1; 
	} 

/////////////////////////////////////////////////////////
typedef struct TestObject TestObject;
typedef struct TestObjectClass TestObjectClass;

GType test_object_get_type (void);

struct TestObject
{
  GObject parent;
};

struct TestObjectClass
{
  GObjectClass parent;
};

enum
{
  HELLO_SIGNAL,
  LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

#define TEST_TYPE_OBJECT              (test_object_get_type ())
#define TEST_OBJECT(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), TEST_TYPE_OBJECT, TestObject))
#define TEST_OBJECT_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), TEST_TYPE_OBJECT, TestObjectClass))
#define TEST_IS_OBJECT(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), TEST_TYPE_OBJECT))
#define TEST_IS_OBJECT_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), TEST_TYPE_OBJECT))
#define TEST_OBJECT_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), TEST_TYPE_OBJECT, TestObjectClass))

G_DEFINE_TYPE(TestObject, test_object, G_TYPE_OBJECT)

gboolean test_object_emit_hello_signal (TestObject *obj, GError **error);

#include "signal-emitter-glue.h"

static void
test_object_init (TestObject *obj)
{
}

static void
test_object_class_init (TestObjectClass *klass)
{

  dbus_g_object_type_install_info (G_TYPE_FROM_CLASS(klass), &dbus_glib_test_object_object_info);	
  signals[HELLO_SIGNAL] =
    g_signal_new ("hello_signal",
		  G_OBJECT_CLASS_TYPE (klass),
                  (GSignalFlags ) (G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED),
                  0,
                  NULL, (gpointer *)NULL,
                  g_cclosure_marshal_VOID__STRING,
                  G_TYPE_NONE, 1, G_TYPE_STRING);
}

gboolean
test_object_emit_hello_signal (TestObject *obj, GError **error)
{
  g_signal_emit (G_OBJECT(obj), signals[HELLO_SIGNAL], 0, "Hello");
  return TRUE;
}
///////////////////////////////////////////////
const char* fifopath[] = {"C:\\mkfifo6.file", 
							"C:\\mkfifo7.file"};
int make_fifos()
{
	int err;
	int cnt;
	for(cnt=0; cnt<2; cnt++)
	{
		err = mkfifo (fifopath[cnt], 0666);
	   if(err != 0)
	   {
	       // probably file already exists, delete the file
	       unlink(fifopath[cnt]); 
	       // try once more..
	       err = mkfifo (fifopath[cnt], 0666);
	       if(err != 0)
	       {
	            return 1;
	       }
	   }
	}
	return 0;
}

int read_fifos()
{
	char buf[20];
	int fd;
	int res;
	int err;
	int cnt;
	for(cnt=0; cnt<2; cnt++)
	{
		fd = open(fifopath[cnt], O_RDONLY);
		if(fd > 0)
		{
			err = read (fd, buf, 20);  
			close(fd);
		}
		else
			{
			std_log(LOG_FILENAME_LINE, "Error in FIFO open().");
			return 1;
			}
		unlink(fifopath[cnt]);
		
		res = atoi(buf);
		switch(res)
		{
			case 0:
				std_log(LOG_FILENAME_LINE, "First Signal Not reached for %d exe", cnt);
				return 1;
			case 1:
				std_log(LOG_FILENAME_LINE, "All right for %d exe", cnt);
			break;
			case 2:
				std_log(LOG_FILENAME_LINE, "Disconnect signal does not have effect for %d exe", cnt);
				return 1;
		}
	}
	
	return 0;
}

char* invoke_exes()
{
	FILE* fp[2];
	int cnt;
	char* buf = (char*)malloc(80);
	char buf1[20] = ""; 
	int fd;
	int err;
	//start server 1
#if defined(__WINSCW__) || defined(__WINS__)
	fp[0] = popen("z:\\sys\\bin\\dbus_glib_signal_server1.exe", "r");
#else
	fp[0] = popen("c:\\sys\\bin\\dbus_glib_signal_server1.exe", "r");
#endif

	fd = open(fifopath1, O_RDONLY);
	if (fd > 0)
	    {
	    err = read(fd, buf1, 20);
	    close(fd);
	    }
	else
	    {
	    std_log(LOG_FILENAME_LINE, "Error in FIFO open().");
	    }

	if (buf1 && strcmp("done", buf1))
	    {
	    std_log(LOG_FILENAME_LINE, "done is not returned from server 1.");
	    }
	//start server 2
#if defined(__WINSCW__) || defined(__WINS__)
	fp[1] = popen("z:\\sys\\bin\\dbus_glib_signal_server2.exe", "r");
#else
	fp[1] = popen("c:\\sys\\bin\\dbus_glib_signal_server2.exe", "r");
#endif

	fd = open(fifopath1, O_RDONLY);
    if (fd > 0)
        {
        err = read(fd, buf1, 20);
        close(fd);
        }
    else
        {
        std_log(LOG_FILENAME_LINE, "Error in FIFO open().");
        }
    unlink(fifopath1);
    if (buf1 && strcmp("done", buf1))
        {
        std_log(LOG_FILENAME_LINE, "done is not returned from server 2.");
        }
	
	//done starting the 2 servers. check if it went as expected.
	for(cnt=0; cnt<2; cnt++)
	{
		if(!fp[cnt])
			{
			sprintf(buf, "popen fail for %d exe", cnt);
			return buf;
			}
	}
	return NULL;
}

int main()
{
	DBusGConnection *bus;
	DBusGProxy *bus_proxy;
	GError *error = NULL;
	TestObject *obj;
	GMainLoop *mainloop;
	guint request_name_result;
	char* buf;
	int err;
    err = mkfifo(fifopath1, 0666);
    if (err != 0)
        {
        // probably file already exists, delete the file
        unlink(fifopath1);
        // try once more..
        err = mkfifo(fifopath1, 0666);
        if (err != 0)
            {
            create_xml(1);
            return 1;
            }
        }
	make_fifos();
	
	buf = invoke_exes();
	if(buf)
	{
		std_log(LOG_FILENAME_LINE, "%s", buf);
		if(buf) free(buf);
		buf = NULL;
		return 1;
	}
		
	sleep(5);	
	
	g_type_init ();
	
	
	mainloop = g_main_loop_new (NULL, FALSE);
	
	bus = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
	if (!bus)
	  return handle_error(error);
	
	bus_proxy = dbus_g_proxy_new_for_name (bus, "org.freedesktop.DBus",
						 "/org/freedesktop/DBus",
						 "org.freedesktop.DBus");
	
	if (!dbus_g_proxy_call (bus_proxy, "RequestName", &error,
				  G_TYPE_STRING, "org.designfu.TestService",
				  G_TYPE_UINT, 0,
				  G_TYPE_INVALID,
				  G_TYPE_UINT, &request_name_result,
				  G_TYPE_INVALID))
	  return handle_error(error);
	
	obj = (TestObject *)g_object_new (TEST_TYPE_OBJECT, NULL);
	
	dbus_g_connection_register_g_object (bus, "/org/designfu/TestService/object", G_OBJECT (obj));
	
	
	//g_signal_emit (G_OBJECT(obj), signals[HELLO_SIGNAL], 0, "Hello");
	g_signal_emit_by_name(G_OBJECT(obj),"hello_signal","Hello");
	 
	sleep(5);
	
	g_signal_emit (G_OBJECT(obj), signals[HELLO_SIGNAL], 0, "Hello");
	
	if(read_fifos())
	{	
		dbus_g_connection_unref(bus);
		create_xml(1);
		return 1;
	}
	if(buf) free(buf);
	    buf = NULL;

	std_log(LOG_FILENAME_LINE, "Test Successful");
	create_xml(0);
	return 0;
}

