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




// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>

#include <e32std.h>
#include <string.h> 
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <dbus/dbus.h>
#include "file_send-bindings.h"
#include "dbus_glib1-bindings.h"
#include <errno.h>
#include <fcntl.h>

#include "stif_glib_1.h"

#define MAX_TRY 10
#define MAX_THREAD 75

_LIT(KSuccess, "Test Successful");

int handle_error(GError* error)
	{
	g_printerr ("Failed to open connection to bus: %s\n",
            error->message);
	g_error_free (error);
	return 1;
	}

void Cstif_glib_1::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Cstif_glib_1::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Cstif_glib_1::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "stif_g_File_Send1", Cstif_glib_1::stif_File_Send1 ),
        ENTRY( "stif_g_variable_args", Cstif_glib_1::stif_Variable_Args ),
        ENTRY( "stif_g_signal", Cstif_glib_1::stif_Signal ),
        ENTRY( "stif_g_N_Messages", Cstif_glib_1::stif_N_Messages ),
        ENTRY( "stif_g_N_Messages1", Cstif_glib_1::stif_N_Messages1 ),
        ENTRY( "stif_g_proxy_begin_call", Cstif_glib_1::stif_Proxy_Begin_Call ),
        ENTRY( "stif_g_proxy_call_timeout", Cstif_glib_1::stif_Proxy_Call_Timeout ),
        ENTRY( "stif_g_proxy_begin_call_timeout", Cstif_glib_1::stif_Proxy_Begin_Call_Timeout ),
        ENTRY( "stif_g_struct_send0", Cstif_glib_1::stif_Struct_Send0 ),
        ENTRY( "stif_g_variant_send0", Cstif_glib_1::stif_Variant_Send0 ),
        ENTRY( "stif_g_dict_send0", Cstif_glib_1::stif_Dict_Send0 ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }


int Cstif_glib_1::send_file(DBusGProxy* proxy, char* file_name)
{
	TTime NowTime;
	TTime NowTime1;
	TTimeIntervalMicroSeconds micro_sec;
	TInt64 diff_int64 = 0;
	GError *error = NULL;
	unsigned int SIZE;
	
	GArray *array;
	guint length;
	FILE *fp = NULL;
	
	struct stat stat_buf;
	gchar *ptr = NULL;
	int temp;
	
	stat(file_name,&stat_buf);
	
	SIZE = stat_buf.st_size;
	ptr = (gchar*)g_malloc(SIZE);
	fp = fopen(file_name,"r");
	if(!fp)
		{
		iLog->Log(_L8("fp is NULL for %s"), file_name);
		return 1;
		}
	
	fread(ptr,1,SIZE,fp);
	
	array = g_array_new(FALSE,FALSE,1);
	g_array_append_vals(array,ptr,SIZE);
	
	iLog->Log(_L8("File %s size %d"), file_name, SIZE);
	NowTime.HomeTime();
	if(!com_example_SomeObject_method1(proxy,SIZE,array,&length,&error))
		{
			NowTime1.HomeTime();
			micro_sec = NowTime1.MicroSecondsFrom(NowTime);
			iLog->Log(_L8("may be time out of %d micro-sec for size %d"), micro_sec.Int64(), SIZE);
			iLog->Log(_L8(" %s"),error->message);
			return 1;
		}
	NowTime1.HomeTime();
	micro_sec = NowTime1.MicroSecondsFrom(NowTime);
	iLog->Log(_L8("1st time %d micro-sec."), micro_sec.Int64());

	for(int i=0;i<MAX_TRY;i++)
	{
		NowTime.HomeTime();
		if(!com_example_SomeObject_method1(proxy,SIZE,array,&length,&error))
		{
			NowTime1.HomeTime();
			micro_sec = NowTime1.MicroSecondsFrom(NowTime);
			iLog->Log(_L8("may be time out of %d micro-sec."), micro_sec.Int64());
			iLog->Log(_L8(" %s"),error->message);
			return 1;
		}
		NowTime1.HomeTime();
		micro_sec = NowTime1.MicroSecondsFrom(NowTime);
		
		temp = micro_sec.Int64();
		diff_int64 += temp;
		iLog->Log(_L8("Trial %d : %d"), i, temp);
	}	
	diff_int64 = diff_int64/MAX_TRY;
	iLog->Log(_L8("\nAverage Time to get file %s of size %d is %d\n"), file_name, length, diff_int64);
	
	fclose(fp);
	
	return 0;
}

TInt Cstif_glib_1::stif_File_Send1( CStifItemParser& aItem )
    {
    TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
    TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  
    DBusGConnection *connection;
	GError *error;
	DBusGProxy *proxy;
	
	
	TTime NowTime;
	TTime NowTime1;
	TTimeIntervalMicroSeconds micro_sec;
	TInt64 diff_int64;

	g_type_init ();

	error = NULL;
	NowTime.HomeTime();
	connection = dbus_g_bus_get (DBUS_BUS_SESSION,
	                           &error);
	NowTime1.HomeTime();
	micro_sec = NowTime1.MicroSecondsFrom(NowTime);
	diff_int64 = micro_sec.Int64();
	iLog->Log(_L8("Time to get 1st time Connection %d"), diff_int64);
	
	if (connection == NULL)
	{
		iLog->Log(_L8("Failed to open connection to bus: %s\n"), error->message);
	  g_error_free (error);
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
	/*if(send_file(proxy, "C:\\sample.txt"))
		return 1;*/
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
	iLog->Log(KSuccess);
    return KErrNone;
    }


TInt Cstif_glib_1::stif_Variable_Args( CStifItemParser& aItem )
{
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  
	DBusGConnection *connection;
	GError *error;
	DBusGProxy *proxy;
	gboolean b = TRUE;
	gint16         n = -16;
    gint         i = -32;
    gint64       x = -64;
    guint16        q = 16;
    guint        u = 32;
    guint64      t = 64;
    gdouble      d = 1.234567;
    gchar*		 s = "DBusGlibTesting~!@#$%^&*()_+`1234567890-=";
    GArray* 	 ai;
    gint		 ret = 1;
	
	g_type_init ();
	
	error = NULL;
	
	connection = dbus_g_bus_get (DBUS_BUS_SESSION,
	                           &error);
	
	if (connection == NULL)
	{
		iLog->Log(_L8("Failed to open connection to bus: %s\n"),
	              error->message);
	  g_error_free (error);
	  exit (1);
	}
	
	proxy = dbus_g_proxy_new_for_name (connection,
	                                 "com.example.DBusGlibObject1",
	                                 "/com/example/DBusGlibObject1",
	                                 "com.example.DBusGlibObject1");

	ai = g_array_new(FALSE,FALSE,sizeof(gint));
	for(gint cnt=0; cnt < 5; cnt++)
		g_array_append_val(ai, cnt);
	
	if(!com_example_DBusGlibObject1_variable_args_method(proxy, b, n, i, x, q, u, t, d, s, ai, &ret, &error))
		{
			iLog->Log(_L8(" %s"),error->message);
			return 1;
		}
	
	if(ret)
	{
		iLog->Log(_L8("Return value is %d"), ret);
		return 1;
	}
	
	dbus_g_connection_unref(connection);
	iLog->Log(KSuccess);
	return KErrNone;
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
  g_signal_emit (obj, signals[HELLO_SIGNAL], 0, "Hello");
  return TRUE;
}
///////////////////////////////////////////////
const char* fifopath[] = {"C:\\mkfifo6.file", 
							"C:\\mkfifo7.file"};
int make_fifos()
{
	int err;
	for(int cnt=0; cnt<2; cnt++)
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

int Cstif_glib_1::read_fifos()
{
	char buf[20];
	int fd;
	int res;
	int err;
	for(int cnt=0; cnt<2; cnt++)
	{
		fd = open(fifopath[cnt], O_RDONLY);
		if(fd > 0)
		{
			err = read (fd, buf, 20);  
			close(fd);
		}
		else
			{
			iLog->Log(_L8("Error in FIFO open()."));
			return 1;
			}
		unlink(fifopath[cnt]);
		
		res = atoi(buf);
		switch(res)
		{
			case 0:
				iLog->Log(_L8("First Signal Not reached for %d exe"), cnt);
				return 1;
			case 1:
				iLog->Log(_L8("All right for %d exe"), cnt);
			break;
			case 2:
				iLog->Log(_L8("Disconnect signal does not have effect for %d exe"), cnt);
				return 1;
		}
	}
	
	return 0;
}

char* invoke_exes()
{
	FILE* fp[2];
	char* buf = (char*)malloc(80);
#if defined(__WINSCW__) || defined(__WINS__)
	fp[0] = popen("z:\\sys\\bin\\dbus_glib_signal_server1.exe", "r");
	fp[1] = popen("z:\\sys\\bin\\dbus_glib_signal_server2.exe", "r");
#else
	fp[0] = popen("c:\\sys\\bin\\dbus_glib_signal_server1.exe", "r");
	fp[1] = popen("c:\\sys\\bin\\dbus_glib_signal_server2.exe", "r");
#endif
	for(int cnt=0; cnt<2; cnt++)
	{
		if(!fp[cnt])
			{
			sprintf(buf, "popen fail for %d exe", cnt);
			return buf;
			}
	}
	return NULL;
}

TInt Cstif_glib_1::stif_Signal( CStifItemParser& aItem )
{
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  

	DBusGConnection *bus;
	DBusGProxy *bus_proxy;
	GError *error = NULL;
	TestObject *obj;
	GMainLoop *mainloop;
	guint request_name_result;
	char* buf;
	
	make_fifos();
	
	buf = invoke_exes();
	if(buf)
	{
		iLog->Log(_L8(buf));
		return 1;
	}
		
	sleep(30);	
	
	g_type_init ();
	
	dbus_g_object_type_install_info (TEST_TYPE_OBJECT, &dbus_glib_test_object_object_info);
	
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
	
	test_object_emit_hello_signal (obj, &error);
	
	sleep(20);

	test_object_emit_hello_signal (obj, &error);
	
	if(read_fifos())
	{	
		dbus_g_connection_unref(bus);
		return 1;
	}
	
	iLog->Log(KSuccess);
	return KErrNone;
}


static void* send_msg(void* data)
{
	GError *error;
	static int cnt = 1;
	gint in_num = 5;
	gint out_num = 0;
		
	threadData1* thrData = (threadData1*)data;
	if(!thrData)
		return NULL;
	
	pthread_mutex_lock(&thrData->mutex);
	
	pthread_cond_wait(&thrData->cond,  &thrData->mutex);
	 
	// send message and get a handle for a reply
	  
	  if(!com_example_DBusGlibObject1_simple_method(thrData->proxy, in_num, &out_num, &error))
		{
			thrData->ret = -2;
			return NULL;
		}
	
	   FILE* fp = fopen("C:\\glib_messages.txt", "a+");
	   fprintf(fp, "%d\n", out_num);
	   fclose(fp);
	    
	   if(out_num == 9090)
		   {
		   thrData->ret++;
		   }
	   
	   pthread_mutex_unlock(&thrData->mutex); 
	   return NULL;
}



TInt Cstif_glib_1::stif_N_Messages( CStifItemParser& aItem )
{
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  
	DBusGConnection *connection;
	GError *error;
	DBusGProxy *proxy;
	gint in_num = 5;
	gint out_num = 0;
	
	int cnt;
	pthread_t thread[MAX_THREAD];
	int thrVal[MAX_THREAD]={0};
	void* thrValPtr[MAX_THREAD];
	
	for(cnt=0; cnt<MAX_THREAD; cnt++)
		thrValPtr[cnt] = (void*)&thrVal[cnt];

	threadData1 thrData;
	
	g_type_init ();
	
	error = NULL;
	
	connection = dbus_g_bus_get (DBUS_BUS_SESSION,
	                           &error);
	
	if (connection == NULL)
	{
		iLog->Log(_L8("Failed to open connection to bus: %s\n"),
	              error->message);
	  g_error_free (error);
	  exit (1);
	}
	
	proxy = dbus_g_proxy_new_for_name (connection,
	                                 "com.example.DBusGlibObject1",
	                                 "/com/example/DBusGlibObject1",
	                                 "com.example.DBusGlibObject1");


	pthread_mutex_init(&thrData.mutex, NULL);
	pthread_cond_init(&thrData.cond, NULL);
 	thrData.ret = 0;
 	thrData.proxy = proxy;

 	for(cnt=0; cnt<MAX_THREAD; cnt++)
 		pthread_create(&thread[cnt], NULL, &send_msg, &thrData);
	 
	sleep(1);   
	
	pthread_cond_broadcast(&thrData.cond);
	
	for(cnt=0; cnt<MAX_THREAD; cnt++)
		pthread_join(thread[cnt], &thrValPtr[cnt]); 
	 
	if(thrData.ret != MAX_THREAD)
	{ 
		iLog->Log(_L8("No. of threads crashed %d"), (MAX_THREAD - thrData.ret));
		return 1;
	}
	
	dbus_g_connection_unref(connection);		
	iLog->Log(KSuccess);
	return KErrNone;
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
		return NULL;
	
	pthread_mutex_lock(&thrData->mutex);
	
	connection = dbus_g_bus_get (DBUS_BUS_SESSION,
            &error);
	
	if (connection == NULL)
	{
		FILE* fp = fopen("C:\\glib_messages1.txt", "a+");
		if(fp)
			{
			fprintf(fp, "Failed to open connection to bus: %s\n", error->message);
			fclose(fp);
			}
		g_error_free (error);
		exit (1);
	}
	
	proxy = dbus_g_proxy_new_for_name (connection,
	                  "com.example.DBusGlibObject1",
	                  "/com/example/DBusGlibObject1",
	                  "com.example.DBusGlibObject1");
		
	pthread_cond_wait(&thrData->cond,  &thrData->mutex);

	// send message and get a handle for a reply
	  
	  if(!com_example_DBusGlibObject1_simple_method(proxy, in_num, &out_num, &error))
		{
			thrData->ret = -2;
			return NULL;
		}
	
	   FILE* fp = fopen("C:\\glib_messages1.txt", "a+");
	   fprintf(fp, "%d\n", out_num);
	   fclose(fp);
	    
	   if(out_num == 9090)
		   {
		   thrData->ret++;
		   }
	   
	   dbus_g_connection_unref(connection);
	   pthread_mutex_unlock(&thrData->mutex); 
	   return NULL;
}


TInt Cstif_glib_1::stif_N_Messages1( CStifItemParser& aItem )
	{
		TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
		TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  
				
		gint in_num = 5;
		gint out_num = 0;
				
		int cnt;
		pthread_t thread[MAX_THREAD];
		int thrVal[MAX_THREAD]={0};
		void* thrValPtr[MAX_THREAD];
		
		for(cnt=0; cnt<MAX_THREAD; cnt++)
			thrValPtr[cnt] = (void*)&thrVal[cnt];

		threadData1 thrData;
		
		g_type_init ();
		
		pthread_mutex_init(&thrData.mutex, NULL);
		pthread_cond_init(&thrData.cond, NULL);
	 	thrData.ret = 0;
	 	thrData.proxy = NULL;
	 	
	 	for(cnt=0; cnt<MAX_THREAD; cnt++)
	 		pthread_create(&thread[cnt], NULL, &send_msg1, &thrData);
		 
		sleep(30);  
		
		pthread_cond_broadcast(&thrData.cond);
		
		for(cnt=0; cnt<MAX_THREAD; cnt++)
			pthread_join(thread[cnt], &thrValPtr[cnt]); 
		 
		if(thrData.ret != MAX_THREAD)
		{ 
			iLog->Log(_L8("No. of threads crashed %d"), (MAX_THREAD - thrData.ret));
			return 1;
		}
			
		iLog->Log(KSuccess);
		return KErrNone;
	}


GMainLoop *mainLoop = NULL;

void simple_method_reply_handler(DBusGProxy *proxy, DBusGProxyCall *call_id, void *user_data)
{
	GError *error = NULL;
	gint res1;
	
	dbus_g_proxy_end_call(proxy, call_id, &error, G_TYPE_INT, &res1, G_TYPE_INVALID);
	
	dbus_g_proxy_cancel_call(proxy, call_id);
	
	*(gint *)user_data = res1;
	
	g_main_loop_quit(mainLoop);
}
	       
TInt Cstif_glib_1::stif_Proxy_Begin_Call( CStifItemParser& aItem )
{
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  

	DBusGConnection *connection;
	GError *error;
	DBusGProxy *proxy;
	DBusGProxyCall* call;
	
	gint res;
	gint res1 = 0;
	
	g_type_init ();

	error = NULL;
	connection = dbus_g_bus_get (DBUS_BUS_SESSION,
	                           &error);
	if (connection == NULL)
	{
		iLog->Log(_L8("Failed to open connection to bus: %s\n"),
	              error->message);
	  g_error_free (error);
	  exit (1);
	}

	proxy = dbus_g_proxy_new_for_name (connection,
            "com.example.DBusGlibObject1",
            "/com/example/DBusGlibObject1",
            "com.example.DBusGlibObject1");
	
	call = dbus_g_proxy_begin_call(proxy, "Simple_method", simple_method_reply_handler, &res1, NULL, G_TYPE_INT, 5, G_TYPE_INVALID, G_TYPE_INT, &res, G_TYPE_INVALID);

	mainLoop = g_main_loop_new(NULL,FALSE);
	g_main_loop_run(mainLoop);
		
	iLog->Log(_L8("res = %d, res1 = %d"), res, res1);
	if(res1 != 9090)
		{
		iLog->Log(_L8("return value is not matching."));
		return 1;
		}
	
	dbus_g_connection_unref(connection);
	iLog->Log(KSuccess);
	return KErrNone;
}

TInt Cstif_glib_1::stif_Proxy_Call_Timeout( CStifItemParser& aItem )
	{
		TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
		TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  

		DBusGConnection *connection;
		GError *error;
		DBusGProxy *proxy;
		
		gint res;
		
		g_type_init ();

		error = NULL;
		connection = dbus_g_bus_get (DBUS_BUS_SESSION,
		                           &error);
		if (connection == NULL)
		{
			iLog->Log(_L8("Failed to open connection to bus: %s\n"),
		              error->message);
		  g_error_free (error);
		  exit (1);
		}

		proxy = dbus_g_proxy_new_for_name (connection,
	            "com.example.DBusGlibObject1",
	            "/com/example/DBusGlibObject1",
	            "com.example.DBusGlibObject1");
		
		if(!dbus_g_proxy_call_with_timeout(proxy, "Simple_method", 10000, &error, G_TYPE_INT, 5, G_TYPE_INVALID, G_TYPE_INT, &res, G_TYPE_INVALID))
		{
			iLog->Log(_L8("Error :: %s"), error->message);
			return 1;
		}
		
		iLog->Log(_L8("Return value = %d"), res);
		if(res != 9090)
			{
			iLog->Log(_L8("return value is not matching."));
			return 1;
			}
		
		dbus_g_connection_unref(connection);
		iLog->Log(KSuccess);
		return KErrNone;
	}


TInt Cstif_glib_1::stif_Proxy_Begin_Call_Timeout( CStifItemParser& aItem )
	{
		TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
		TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  

		DBusGConnection *connection;
		GError *error;
		DBusGProxy *proxy;
		DBusGProxyCall* call;
		
		gint res;
		gint res1 = 0;
		
		g_type_init ();

		error = NULL;
		connection = dbus_g_bus_get (DBUS_BUS_SESSION,
		                           &error);
		if (connection == NULL)
		{
			iLog->Log(_L8("Failed to open connection to bus: %s\n"),
		              error->message);
		  g_error_free (error);
		  exit (1);
		}

		proxy = dbus_g_proxy_new_for_name (connection,
	            "com.example.DBusGlibObject1",
	            "/com/example/DBusGlibObject1",
	            "com.example.DBusGlibObject1");
		
		call = dbus_g_proxy_begin_call_with_timeout(proxy, "Simple_method", simple_method_reply_handler, &res1, NULL, 10000, G_TYPE_INT, 5, G_TYPE_INVALID, G_TYPE_INT, &res, G_TYPE_INVALID);
		
		mainLoop = g_main_loop_new(NULL,FALSE);
		g_main_loop_run(mainLoop);
			
		iLog->Log(_L8("res = %d, res1 = %d"), res, res1);
		if(res1 != 9090)
			{
			iLog->Log(_L8("return value is not matching."));
			return 1;
			}
		
		dbus_g_connection_unref(connection);
		iLog->Log(KSuccess);
		return KErrNone;
	}

TInt Cstif_glib_1::stif_Struct_Send0( CStifItemParser& aItem )
{
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  
	DBusGConnection *connection;
	GError *error;
	DBusGProxy *proxy;
	
	GValueArray* st;
	gint		 ret = 1;
	gint		st_int = 1234;
	gchar* 		st_str = "StructPassing";
	
	g_type_init ();
	
	error = NULL;
	
	connection = dbus_g_bus_get (DBUS_BUS_SESSION,
	                           &error);
	
	if (connection == NULL)
	{
		iLog->Log(_L8("Failed to open connection to bus: %s\n"),
	              error->message);
	  g_error_free (error);
	  exit (1);
	}
	
	proxy = dbus_g_proxy_new_for_name (connection,
	                                 "com.example.DBusGlibObject1",
	                                 "/com/example/DBusGlibObject1",
	                                 "com.example.DBusGlibObject1");
	
	st = g_value_array_new (2);
	GValue val ={0};
	  g_value_init(&val, G_TYPE_INT);
	  g_value_set_int(&val, st_int);
	  g_value_array_append(st, &val);
	  g_value_unset(&val);
	  
	  GValue val1 ={0};
	  g_value_init(&val1, G_TYPE_STRING);
	  g_value_set_string(&val1, st_str);
	  g_value_array_append(st, &val1);
	  g_value_unset(&val1);
		
	if(!com_example_DBusGlibObject1_struct_example_method(proxy, st, &ret, &error))
		{
			iLog->Log(_L8(" %s"),error->message);
			return 1;
		}
	
	if(ret!=5050)
	{
		iLog->Log(_L8("Return value is %d"), ret);
		return 1;
	}
	
	dbus_g_connection_unref(connection);
	iLog->Log(KSuccess);
	return KErrNone;
}

TInt Cstif_glib_1::stif_Variant_Send0( CStifItemParser& aItem )
{
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  
	DBusGConnection *connection;
	GError *error;
	DBusGProxy *proxy;
	
	GValue val = {0, };
	GValue val1 = {0, };
	
	char* in  = "Hello World...!!!";
	const char* out;
	
	g_type_init ();
	
	error = NULL;
	
	connection = dbus_g_bus_get (DBUS_BUS_SESSION,
	                           &error);
	
	if (connection == NULL)
	{
		iLog->Log(_L8("Failed to open connection to bus: %s\n"),
	              error->message);
	  g_error_free (error);
	  exit (1);
	}
	
	proxy = dbus_g_proxy_new_for_name (connection,
	                                 "com.example.DBusGlibObject1",
	                                 "/com/example/DBusGlibObject1",
	                                 "com.example.DBusGlibObject1");
	
	g_value_init (&val, G_TYPE_STRING);
	g_value_set_string (&val, in);
	
	error = NULL;
	
	if (!com_example_DBusGlibObject1_variant_send_method(proxy, &val, &val1, &error))
	  {
	    iLog->Log(_L8("Error :: %s"), error->message);
	    return 1;
	  }
	
	out = g_value_get_string(&val1);
	
	if(strcmp(in, out))
		{
		iLog->Log(_L8("Strings are mismatching"));
		return 1;
		}
	
	g_value_unset (&val);
	g_value_unset (&val1);
	
	dbus_g_connection_unref(connection);
	iLog->Log(KSuccess);
	return KErrNone;
}

TInt Cstif_glib_1::stif_Dict_Send0( CStifItemParser& aItem )
{
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  
	DBusGConnection *connection;
	GError *error;
	DBusGProxy *proxy;
	int ret;
	
	char* key[3] = { "Mahesh", 
				"Vasant",
				"Nitin"
	};
	
	char* values[3] = { "Mandhare",
					"Raj",
					"Verma"
	};
	
	
	GHashTable* hash;
	hash = g_hash_table_new(NULL, NULL);
	
	for(int cnt=0; cnt<3; cnt++)
		g_hash_table_insert(hash, key[cnt], values[cnt]);
	
	g_type_init ();
	dbus_g_type_specialized_init();
	
	error = NULL;
	
	connection = dbus_g_bus_get (DBUS_BUS_SESSION,
	                           &error);
	
	if (connection == NULL)
	{
		iLog->Log(_L8("Failed to open connection to bus: %s\n"),
	              error->message);
	  g_error_free (error);
	  exit (1);
	}
	
	proxy = dbus_g_proxy_new_for_name (connection,
	                                 "com.example.DBusGlibObject1",
	                                 "/com/example/DBusGlibObject1",
	                                 "com.example.DBusGlibObject1");
	
	if (!com_example_DBusGlibObject1_dict_send_method(proxy, hash, &ret, &error))
		  {
		    iLog->Log(_L8("Error :: %s"), error->message);
		    return 1;
		  }
	
	if(ret != 5050)
		{
		iLog->Log(_L8("ret = %d"), ret);
		return 1;
		}
	
	dbus_g_connection_unref(connection);
	iLog->Log(KSuccess);
	return KErrNone;
}


//  [End of File] - Do not remove
