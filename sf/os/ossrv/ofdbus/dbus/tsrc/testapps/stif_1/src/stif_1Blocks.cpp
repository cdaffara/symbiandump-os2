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
#include <e32std.h>
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h> 
#include <unistd.h>
#include "stif_1.h"
#include <sys/stat.h>

#include <pthread.h>
#include <fcntl.h>
#include <string.h>
#include <spawn.h>

#include "errno.h"
#define MAX_THREAD 25
#define MAX_SIGNALS 3
#define MAX_CONN 21
#define MAX_CONN_EXE 25

_LIT(KSuccess, "Test Successful");
_LIT(KOOM, "Out Of Memory");
int var_arg_ret_val = -3;
int pending_call_ret_val = -6;
TBool data_free_flag = FALSE;

struct TestData
{
	int num;
	char* str;
};

struct TestData1
{
	int data_slot;
	TestData* data;
};


void Free_Test_Data(void* data)
	{
		TestData* test_data;
		test_data = (TestData*)data;
		
		free(test_data->str);
		free(test_data);
		data_free_flag = TRUE;
	}
 
void handle_reply_variable_args(DBusPendingCall* pending, void* data)
	{
		DBusMessage* msg1;
		DBusMessageIter return_iter;
		DBusError error;
		dbus_int32_t* return_value = (dbus_int32_t*)data;
		dbus_int32_t ret;
		
		// get the reply message
		msg1 = dbus_pending_call_steal_reply(pending);
		if (NULL == msg1) {
		*return_value = -2; 
		}

		// free the pending message handle
		dbus_pending_call_unref(pending);
			 
		dbus_error_init (&error);
		dbus_message_iter_init(msg1, &return_iter);

		dbus_message_iter_get_basic(&return_iter, return_value);
		ret = *return_value; 
		RDebug::Printf("return value is:%d\n", ret);
		dbus_message_unref(msg1);  
	}

void handle_reply_pending_call_data_slot0(DBusPendingCall* pending, void* data)
	{
		DBusMessage* msg1;
		DBusError error;
		int return_value;
		TestData1* data1 = (TestData1*)data;
		TestData* test_data;
		TestData* new_data;
		DBusFreeFunction free_data;
		
		// get the reply message
		msg1 = dbus_pending_call_steal_reply(pending);
		if (NULL == msg1) {
		return_value = -2; 
		return;
		}		
			 
		dbus_error_init (&error);
		
		dbus_message_get_args(msg1, &error, DBUS_TYPE_INT32, &return_value, DBUS_TYPE_INVALID);
		
		if(return_value == 9090)
			return_value = 0;
		
		test_data = (TestData*)dbus_pending_call_get_data(pending, data1->data_slot);
		if(test_data->num != data1->data->num || strcmp(test_data->str, data1->data->str))
			{
			return_value = -3;
			return;
			}

		new_data = (TestData *)malloc(sizeof(TestData));
		new_data->num = 234;
		new_data->str = (char*)malloc(20);
		strcpy(new_data->str, "Check DBus New");
		free_data = Free_Test_Data;
		data_free_flag = FALSE;
		if(!dbus_pending_call_set_data(pending, data1->data_slot, new_data, free_data))
			{
			return_value = -4; 
			return;
			}
		
		if(!data_free_flag)
			{
			return_value = -5;
			return;
			}
		data_free_flag = FALSE;
				
		pending_call_ret_val = return_value;
		dbus_message_unref(msg1);  
		// free the pending message handle
		dbus_pending_call_unref(pending);
	}

void free_mem(void* data)
	{
	dbus_int32_t* return_value = (dbus_int32_t*)data;
	var_arg_ret_val = *return_value;
	free(data);
	}

// ============================ MEMBER FUNCTIONS ===============================
 
int Cstif_1::handle_error(DBusError* error)
	{
	iLog->Log(_L8("%s"), error->name);
	iLog->Log(_L8("%s"), error->message);
	dbus_error_free(error);
	return 1; 
	} 

// -----------------------------------------------------------------------------
// Cstif_1::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Cstif_1::Delete() 
    {

    }
TBool TestConnectionEqual(DBusConnection* conn1, DBusConnection* conn2)
	{
		return (conn1==conn2? TRUE : FALSE);
	}

// -----------------------------------------------------------------------------
// Cstif_1::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Cstif_1::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "stif_Test_Names", Cstif_1::stif_Test_Names ),
		ENTRY( "stif_Method_Call", Cstif_1::stif_Method_Call ),
        ENTRY( "stif_N_Connections", Cstif_1::stif_N_Connections ),
        ENTRY( "stif_Variable_Args", Cstif_1::stif_Variable_Args ),
        ENTRY( "stif_N_Messages", Cstif_1::stif_N_Messages ),
        ENTRY( "stif_N_Messages1", Cstif_1::stif_N_Messages1 ),
        ENTRY( "stif_Signal", Cstif_1::stif_Signal ),
        ENTRY( "stif_File_Send", Cstif_1::stif_File_Send ),
        ENTRY( "stif_File_Send_Whole", Cstif_1::stif_File_Send_Whole ),
        ENTRY( "stif_Match_Rule", Cstif_1::stif_Match_Rule ),
        ENTRY( "stif_Sign_Test", Cstif_1::stif_Sign_Test ),
        ENTRY( "stif_N_Separate_Connections", Cstif_1::stif_N_Separate_Connections ),
        ENTRY( "stif_Pending_Call_Data_Slot0", Cstif_1::stif_Pending_Call_Data_Slot0),   
        ENTRY( "stif_Time0", Cstif_1::stif_Time0),
        ENTRY( "stif_Simultaneous_Connections", Cstif_1::stif_Simultaneous_Connections),
        ENTRY( "stif_Daemon_Run_Time0", Cstif_1::stif_Daemon_Run_Time0)
        
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        }; 
 
    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

void Cstif_1::TestName(DBusConnection *connection, const char *name, int expectedSuccess, int* result)
{
  	DBusError error;
  	dbus_error_init (&error);

  	(void) dbus_bus_request_name (connection, name, 0, &error);
  	if (dbus_error_is_set (&error))
    {
    	if (expectedSuccess)
    	{
    		iLog->Log(_L8("Error acquiring name '%s': %s\n"), name, error.message);
    		*result = 1;
    	}
    	else
   		{
   			iLog->Log(_L8("Expected Error acquiring name '%s': %s\n"), name, error.message);
   			*result = 1;
   		}
    	dbus_error_free (&error);
    }
  	else 
    {
    	if (!expectedSuccess)
    	{
    		iLog->Log(_L8("Unexpected Success acquiring name '%s'\n"), name);
    		*result = 1;
    	}
    	else
    		iLog->Log(_L8("Successfully acquired name '%s'\n"), name);
    }
}


TInt Cstif_1::stif_Test_Names( CStifItemParser& aItem )
{
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  


	DBusError error;
	DBusConnection *connection;
	int result = 0;
	
	dbus_error_init (&error);
	connection = dbus_bus_get_private (DBUS_BUS_SESSION, &error);
	if (connection == NULL)
	  {
	  			iLog->Log(_L8("*** Failed to open connection to system bus: %s\n%s"),  error.name, error.message);
	    dbus_error_free (&error);
	    return 1;
	  }
	
	TestName(connection, "org.freedesktop.DBus.Test", TRUE, &result);
	TestName(connection, "org.freedesktop.DBus.Test-2", TRUE, &result);
	TestName(connection, "org.freedesktop.DBus.Test_2", TRUE, &result);
	#if 0
	TestName(connection, "Test_2", TRUE, &result);
	#endif
	if(result)
		return 1;
	
	dbus_connection_close(connection);
	dbus_connection_unref(connection);
	dbus_shutdown();
	
	iLog->Log(KSuccess);
	return KErrNone;
}


TInt Cstif_1::stif_Method_Call( CStifItemParser& aItem )
    { 

    TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
    TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles ); 
    

    __UHEAP_MARK;
    _LIT(KStart, "Test Start");

	    DBusError error; 
		DBusConnection* connection;
		DBusMessage* msg;
		DBusMessageIter args;
		DBusMessageIter append_args;
		//~!@#$%^&*()_  . hjgfyh34348#$!%^4567
		char* str = "DBus Testing";
		char* str1;
		int i;
		       
		DBusPendingCall* pending; 
		iLog->Log(KStart);
		    
		dbus_error_init(&error); 
		 
		iLog->Log(_L8("creating connection"));	
		
		connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
		
		if(dbus_error_is_set(&error))
			return handle_error(&error);
		 
		iLog->Log(_L8("connection created successfully"));
		msg = dbus_message_new_method_call("Test.Method.Call", "/Test/Method/Object", NULL, "method");
	  
		if(msg == NULL)
			{
			iLog->Log(_L8("message error")); 
			return 1;
			}
		iLog->Log(_L8("message created successfully"));
	 
		dbus_message_iter_init_append(msg, &append_args);
		for(i=0;i<255;i++)
		{
			if(!dbus_message_iter_append_basic(&append_args, DBUS_TYPE_STRING, &str))
			{
				iLog->Log(_L8("Unable to append arguments. msg no. %d"));
				return 1; 
			} 
		}	   

		// send message and get a handle for a reply
	   if (!dbus_connection_send_with_reply (connection, msg, &pending, -1)) { // -1 is default timeout
	   		iLog->Log(_L8("message send error"));
	   		exit(1);
	   } 
	   if (NULL == pending) { 
	   		iLog->Log(_L8("pending is null"));
	      exit(1); 
	   } 
	   dbus_connection_flush(connection);
	   
		// free message
	   dbus_message_unref(msg);
	  
	   // block until we recieve a reply
	   dbus_pending_call_block(pending);
	
	   // get the reply message
	   msg = dbus_pending_call_steal_reply(pending);
	   if (NULL == msg) {
	   iLog->Log(_L8("Reply error"));	   		
	      return 1;
	   } 
	   // free the pending message handle
	   dbus_pending_call_unref(pending);
		 
		dbus_error_init (&error);
		dbus_message_iter_init(msg, &args);
		i=1; 
		do{
			dbus_message_iter_get_basic(&args, &str1);
			iLog->Log(_L8("%d th Reply = %s"), i, str1);
			 
			if(strcmp(str, str1)) 
				   {
				   iLog->Log(_L8("Invalid reply error"));
				   return 1;
				   }
			  
			i++;
		}while(dbus_message_iter_next(&args)); 
	 
	   // free reply and close connection
	   dbus_message_unref(msg);  
	   
	   dbus_connection_close(connection);
	   dbus_connection_unref(connection);
	   dbus_shutdown();
 
	   iLog->Log( KSuccess );
	   __UHEAP_MARKEND;
	   return KErrNone;
    }


// Currently 21 connections are created If tried more will failed to get connection
TInt Cstif_1::stif_N_Connections( CStifItemParser& aItem )
	{
//		__UHEAP_MARK;
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
    TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles ); 
    

 		DBusConnection* connection[MAX_CONN];
		DBusError error;
		int cnt=0;
		int cnt1=0;
		
		dbus_error_init(&error);
		for(cnt=0;cnt<MAX_CONN;cnt++)
		{
			connection[cnt] = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
			if(!connection[cnt] || dbus_error_is_set(&error))
			{
				iLog->Log(_L8("Error_name = %s"), error.name);
				iLog->Log(_L8("Error_msg = %s"), error.message);
				break;
			}
			else
			{
				for(cnt1=0;cnt1<cnt;cnt1++)
				{
					if(TestConnectionEqual(connection[cnt], connection[cnt1]))
					{ 
						iLog->Log(_L8("%d and %d Connections are equal."), cnt, cnt1);
						dbus_connection_close(connection[cnt]);
						dbus_connection_unref(connection[cnt]);
						break;
					}
				}
				iLog->Log(_L8("Connection Success %d"), cnt);
			}
		} 
		
		for(cnt1=0;cnt1<cnt;cnt1++)
			{
			dbus_connection_close(connection[cnt1]);
			dbus_connection_unref(connection[cnt1]);
			}
		dbus_shutdown();
		
		if(cnt!=MAX_CONN)
			return 1;
		iLog->Log(KSuccess);
		
//		__UHEAP_MARKEND;
		return KErrNone;
		
	}


TInt Cstif_1::stif_Variable_Args( CStifItemParser& aItem )
	{
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
    TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  
    

		_LIT(KOOM, "Out Of Memory");
		DBusConnection* connection;
		DBusError error;
		DBusMessage* msg;
		DBusMessageIter append_iter;
		DBusMessageIter sub_iter;
		DBusPendingCall* pending;
		
		dbus_bool_t arg_bool = TRUE;
		dbus_int16_t arg_int16 = -16;
		dbus_int32_t arg_int32 = -32;
		dbus_int64_t arg_int64 = -64;
		dbus_uint16_t arg_uint16 = 16;
		dbus_uint32_t arg_uint32 = 32;
		dbus_uint64_t arg_uint64 = 64;
		double arg_double = 12.34567;
		char* arg_str = "DBus Testing";
		const dbus_int32_t array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
		const dbus_int32_t *arr = array;
		
		DBusPendingCallNotifyFunction reply_handler;
		DBusFreeFunction free_data;
		dbus_int32_t* return_value;
		
		dbus_error_init(&error); 

		connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
		if(!connection)
			return handle_error(&error);
		
		msg = dbus_message_new_method_call("Test.Method.Call", "/Test/Method/Object", "test.Method.Call", "variable_args_method");
		if(msg == NULL)
			{ 
			iLog->Log(_L8("message error"));
			return 1;
			}
		iLog->Log(_L8("message created successfully"));

		dbus_message_iter_init_append(msg, &append_iter);
		
		if(!dbus_message_iter_append_basic(&append_iter, DBUS_TYPE_BOOLEAN, &arg_bool))
			{ 
			iLog->Log(KOOM);
			return 1; 
			} 
		if(!dbus_message_iter_append_basic(&append_iter, DBUS_TYPE_INT16, &arg_int16))
			{
			iLog->Log(KOOM);
			return 1;
			}
		if(!dbus_message_iter_append_basic(&append_iter, DBUS_TYPE_INT32, &arg_int32))
			{
			iLog->Log(KOOM);
			return 1;
			}
		if(!dbus_message_iter_append_basic(&append_iter, DBUS_TYPE_INT64, &arg_int64))
			{
			iLog->Log(KOOM);
			return 1;
			}
		if(!dbus_message_iter_append_basic(&append_iter, DBUS_TYPE_UINT16, &arg_uint16))
			{
			iLog->Log(KOOM);
			return 1;
			}
		if(!dbus_message_iter_append_basic(&append_iter, DBUS_TYPE_UINT32, &arg_uint32))
			{
			iLog->Log(KOOM);
			return 1;
			}
		if(!dbus_message_iter_append_basic(&append_iter, DBUS_TYPE_UINT64, &arg_uint64))
			{
			iLog->Log(KOOM);
			return 1;
			}
		if(!dbus_message_iter_append_basic(&append_iter, DBUS_TYPE_DOUBLE, &arg_double))
			{
			iLog->Log(KOOM);
			return 1;
			}
		
		if(!dbus_message_iter_append_basic(&append_iter, DBUS_TYPE_STRING, &arg_str))
			{
			iLog->Log(KOOM);
			return 1;
			}
	
		// For Array	
		if(!dbus_message_iter_open_container(&append_iter, DBUS_TYPE_ARRAY, DBUS_TYPE_INT32_AS_STRING, &sub_iter))
			{
			iLog->Log(KOOM);
			return 1;
			}	 
		if(!dbus_message_iter_append_fixed_array(&sub_iter, DBUS_TYPE_INT32, &arr,  10))
			{ 
			iLog->Log(KOOM);
			return 1;  
			}
		dbus_message_iter_close_container(&append_iter, &sub_iter);
		
		
		//For Structure	
		if(!dbus_message_iter_open_container(&append_iter, DBUS_TYPE_STRUCT, NULL, &sub_iter))
			{
			iLog->Log(KOOM);
			return 1;
			}	 
		if(!dbus_message_iter_append_basic(&sub_iter, DBUS_TYPE_INT32, &arg_int32))
			{ 
			iLog->Log(KOOM);
			return 1;  
			}
		if(!dbus_message_iter_append_basic(&sub_iter, DBUS_TYPE_STRING, &arg_str))
			{ 
			iLog->Log(KOOM);
			return 1;  
			} 
		if(!dbus_message_iter_append_basic(&sub_iter, DBUS_TYPE_DOUBLE, &arg_double))
			{ 
			iLog->Log(KOOM);
			return 1;  
			}
		dbus_message_iter_close_container(&append_iter, &sub_iter);
		
		
		  
		// send message and get a handle for a reply
		   if (!dbus_connection_send_with_reply (connection, msg, &pending, -1)) { // -1 is default timeout
		   		iLog->Log(_L8("message send error"));
		   		exit(1);
		   } 
		   if (NULL == pending) {
		   		iLog->Log(_L8("pending is null"));
		      exit(1);
		   }
		   dbus_connection_flush(connection);
		   
			// free message
		   dbus_message_unref(msg); 
		  
		   reply_handler = handle_reply_variable_args;
		   return_value = (dbus_int32_t*)malloc(sizeof(dbus_int32_t));
		   free_data = free_mem;
		   dbus_pending_call_set_notify(pending, reply_handler, return_value, free_data);
		   
		   
		   // block until we recieve a reply
		   dbus_pending_call_block(pending); 
		   
			iLog->Log(_L8("Reply = %d"), var_arg_ret_val);
			switch(var_arg_ret_val)
			{
				case 0:
					break;
				case -2:
					iLog->Log(_L8("Error while stealing reply from pending call"));
					return 1;
				case -3:
					iLog->Log(_L8("free_data function not called."));
					return 1;
				default:
					iLog->Log(_L8("Undefined return value"));
					return 1;
			}
				
			dbus_connection_close(connection);
			dbus_connection_unref(connection);
			dbus_shutdown();
		
			iLog->Log( KSuccess );
			return KErrNone;
	}
  

static void* send_msg(void* data)
{
	threadData* thrData = (threadData*)data;
	
	pthread_mutex_lock(&thrData->mutex);
	 
	//static int cnt = 1;
	dbus_int32_t no = 5;
	DBusPendingCall* pending;
	DBusMessage* msg1;
	DBusMessage* msg;
	DBusError error; 
	
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
	   
	   FILE* fp = fopen("C:\\new.txt", "a+");
	   fprintf(fp, "%d\n", no);
	   fclose(fp);
	    
	   if(no == 9090)
		   {
		   thrData->ret++;
		   }
	   
	 	 
	   // free reply and close connection
	   dbus_message_unref(msg1); 
	   pthread_mutex_unlock(&thrData->mutex); 
	   return NULL;
}
 

TInt Cstif_1::stif_N_Messages( CStifItemParser& aItem )
{
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  


	DBusError error;
	char err[80];
	int cnt;
	
	pthread_t thread[MAX_THREAD];
	int thrVal[MAX_THREAD]={0};
	void* thrValPtr[MAX_THREAD];
	
	for(cnt=0; cnt<MAX_THREAD; cnt++)
		thrValPtr[cnt] = (void*)&thrVal[cnt];

	threadData thrData;
	 
	dbus_error_init(&error);
	thrData.connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
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
		sprintf(err, "No. of threads crashed %d", (MAX_THREAD - thrData.ret));
		iLog->Log(_L8(err));
		return 1;
	}
	
	dbus_connection_close(thrData.connection);
	dbus_connection_unref(thrData.connection);
	dbus_shutdown();
	
	iLog->Log(KSuccess); 
	return KErrNone;
}

static void* send_msg1(void* data)
{
	DBusConnection* connection;
	DBusError error;
	//static int cnt = 1;
	dbus_int32_t no = 5;
	DBusPendingCall* pending;
	DBusMessage* msg1;
	DBusMessage* msg;
	int data_slot = *(int*)data;

	dbus_error_init(&error);
	connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
	
	threadData1* thrData = (threadData1*)dbus_connection_get_data(connection, data_slot);
	if(!thrData)
		return NULL;
	
	pthread_mutex_lock(&thrData->mutex);
	
	msg = dbus_message_new_method_call("Test.Method.Call", "/Test/Method/Object", "test.Method.Call", "simple");
	
	dbus_message_append_args(msg, DBUS_TYPE_INT32, &no, DBUS_TYPE_INVALID);
	 
	pthread_cond_wait(&thrData->cond,  &thrData->mutex);
	 
	// send message and get a handle for a reply
	   if (!dbus_connection_send_with_reply (connection, msg, &pending, -1)) { // -1 is default timeout
	   thrData->ret = 2;
//	   		exit(1);
	   }   
	   if (NULL == pending) {
	   thrData->ret = 2;
//	      exit(1);
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
	   
	   FILE* fp = fopen("C:\\new.txt", "a+");
	   fprintf(fp, "%d\n", no);
	   fclose(fp);
	    
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

TInt Cstif_1::stif_N_Messages1( CStifItemParser& aItem )
{
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  


	DBusConnection* connection;
	DBusError error;
	int cnt;
	int data_slot = -1;
	
	pthread_t thread[MAX_THREAD];
	int thrVal[MAX_THREAD]={0};
	void* thrValPtr[MAX_THREAD];
	
	for(cnt=0; cnt<MAX_THREAD; cnt++)
		thrValPtr[cnt] = (void*)&thrVal[cnt];

	threadData1 thrData;
	 
	dbus_error_init(&error);
	connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
	if(!connection || dbus_error_is_set(&error))
		return handle_error(&error);
	
	pthread_mutex_init(&thrData.mutex, NULL);
	pthread_cond_init(&thrData.cond, NULL);
 	thrData.ret = 0;
 	
 	dbus_connection_allocate_data_slot(&data_slot);
	dbus_connection_set_data(connection, data_slot, &thrData, NULL);
 	
 	dbus_threads_init_default();

 	for(cnt=0; cnt<MAX_THREAD; cnt++)
 		pthread_create(&thread[cnt], NULL, &send_msg1, &data_slot);
	 
	sleep(1);  
	
	pthread_cond_broadcast(&thrData.cond);
	
	for(cnt=0; cnt<MAX_THREAD; cnt++)
		pthread_join(thread[cnt], &thrValPtr[cnt]); 
	 
	if(thrData.ret != MAX_THREAD)
	{ 
		iLog->Log(_L8("No. of threads crashed %d"), (MAX_THREAD - thrData.ret));
		return 1;
	}
	 
	dbus_connection_unref(connection);
	
	iLog->Log(KSuccess); 
	return KErrNone;
}


TInt Cstif_1::stif_Signal( CStifItemParser& aItem )
{
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );


	DBusConnection* connection;
	DBusError error;
	DBusMessage* msg;
	//dbus_int32_t arg_int32 = 1010;
	DBusObjectPathVTable vtable =
	{
	NULL,NULL,NULL	
	};
	char* str = "DBus Testing. hjgfyh34348#$!%^45678901730952698376092869876DBus Testing. hjgfyh34348#$!%^~!@#$%^&*()_+`-=<>?:{},./;'[]45678901730952698376092869876";
	FILE* fp[MAX_SIGNALS]={NULL};

	int cnt=0;
	char exe_param[100];
	char names[][40]={"test.Signal.Send3 ",
					"test.Signal.Send4 ",
					"test.Signal.Send5 "};
	char obj_path[][40]={"/Test/Signal/Object3",
					"/Test/Signal/Object4",
					"/Test/Signal/Object5"};
	
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
				iLog->Log(_L8("fp %d is NULL"), cnt); 
				return 1;
				}
			//User::After(3000000);  
	}

	//User::After(3000000);  
	 	
	dbus_error_init(&error);
	connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
	 
	if(!connection || dbus_error_is_set(&error))
		return handle_error(&error);
	  
	msg = dbus_message_new_signal("/Test/Signal/Object", "Test.Signal.Send", "first");
	
	sleep(2);
	if(!msg) 
	{
		iLog->Log(_L8("msg is NULL"));
		return 1;
	}
	 
	if(!dbus_message_append_args(msg, DBUS_TYPE_STRING, &str, DBUS_TYPE_INVALID))
	{
		iLog->Log(_L8("Fail to append args"));
		return 1;
	}
	
	dbus_connection_send(connection, msg, NULL);
	dbus_connection_flush(connection);
	 
	dbus_message_unref(msg);
	
	str = "";
	iLog->Log(_L8("First part over"));
	
	dbus_bus_add_match(connection, "type='signal',interface='Test.Signal.Send1'",&error);
	
	if(dbus_error_is_set(&error))
		return handle_error(&error);
	
	iLog->Log(_L8("Registering path"));
	if(!dbus_connection_register_object_path (connection, "/Test/Signal/Object1", &vtable, NULL))
	{ 
		iLog->Log(_L8("Registering path fail"));
		return 1;
	}
	iLog->Log(_L8("Requesting name"));	
	if(!dbus_bus_request_name (connection, "test.Signal.Send1", DBUS_NAME_FLAG_ALLOW_REPLACEMENT, &error) == (unsigned int)-1)
	{
		iLog->Log(_L8("Requesting name fail"));	
		return 1;
	}
	 
	iLog->Log(_L8("Starting while loop"));
	cnt=0;
	while(TRUE)  
	{	
		dbus_connection_read_write(connection, 0);
		
		msg = dbus_connection_pop_message(connection);
			
		if(msg == NULL)
		{
			continue; 
		} 
		
		iLog->Log(_L8("Message Detected"));

		if(dbus_message_is_signal(msg, "Test.Signal.Send1", "second"))
		{
			if(!dbus_message_get_args(msg, &error, DBUS_TYPE_STRING, &str, DBUS_TYPE_INVALID))
				{
					iLog->Log(_L8("Error while retriving arguments :: %s"), error.name);
					dbus_error_free(&error);
					return 1;
				}  
			iLog->Log(_L8("Got %d MSG : %s"), cnt, str);
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

	iLog->Log(KSuccess); 
	return KErrNone;
}


TInt Cstif_1::stif_File_Send( CStifItemParser& aItem )
{
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles ); 


	//_LIT(KOOM, "Out Of Memory");
	FILE* fp;
	unsigned char arr[200];
	const unsigned char* p_arr = arr;
	DBusError error;
	DBusConnection* connection;
	DBusMessage* msg;
	int num;
	
	dbus_error_init(&error); 
	
	connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
	
	if(!connection || dbus_error_is_set(&error))
		return handle_error(&error);
	  
	fp = fopen("c:\\bunbask1.jpg", "r"); 
	if(!fp) 
	{
		iLog->Log(_L8("Not able to open File"));
		return 1;
	}  
	int i=0; 
	while (!feof(fp)) 
    {   
        num = fread(&arr, sizeof(unsigned char), 200, fp);
        i++;  
        
        msg = dbus_message_new_method_call("Test.Method.Call", "/Test/Method/Object", "test.Method.Call", "file_send");
        if(!msg)
        { 
        	iLog->Log(_L8("msg is NULL."));
        	return 1;
        } 
         
        iLog->Log(_L8("msg is created")); 
        
        iLog->Log(_L8("num = %d"), num); 
        if(!dbus_message_append_args(msg, DBUS_TYPE_ARRAY, DBUS_TYPE_BYTE, &p_arr, num, DBUS_TYPE_INVALID))
        { 
        	iLog->Log(_L8("Fail to append ARRAY."));
        	return 1;
        }
        
    	iLog->Log(_L8("Array Appended")); 
    	dbus_connection_send(connection, msg, NULL);   
    	dbus_connection_flush(connection);
    	
    	dbus_message_unref(msg);
    	msg=NULL;
    } 
	if(fp)
		fclose(fp);
	
	dbus_connection_close(connection);
	dbus_connection_unref(connection);
	dbus_shutdown();
	return KErrNone;
}

TInt Cstif_1::stif_File_Send_Whole( CStifItemParser& aItem )
	{
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
    TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );
    
    
    FILE* fp;
	int ret =0;
	
#if defined(__WINSCW__) || defined(__WINS__)
	fp = popen("Z:\\sys\\bin\\file_send_whole.exe", "r");
#else
	fp = popen("C:\\sys\\bin\\file_send_whole.exe", "r");
#endif
	if(!fp)
	{
		iLog->Log(_L8("Failed to open file_send_whole.exe"));
		return 1;
	}
	
	ret = pclose(fp);	
	if(ret)
		{
		iLog->Log(_L8("Test Fail"));
		return 1;
		}
	iLog->Log(KSuccess);
	return KErrNone;
	}


int Cstif_1::send_message(DBusConnection* connection, char* path, char* iface, char* member)
{
	DBusMessage* msg;
	char* str = "DBus Testing.";
	
	msg = dbus_message_new_signal(path, iface, member);
	
	if(!msg) 
	{ 
		iLog->Log(_L8("msg is NULL"));
		return 1;
	}
	 
	if(!dbus_message_append_args(msg, DBUS_TYPE_STRING, &str, DBUS_TYPE_INVALID))
	{
		iLog->Log(_L8("Fail to append args"));
		return 1;
	}
	 
	dbus_connection_send(connection, msg, NULL);
	dbus_connection_flush(connection);	 
	dbus_message_unref(msg);
	return 0;
}


TInt Cstif_1::stif_Match_Rule( CStifItemParser& aItem )
{
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  


	DBusConnection* connection;
	DBusError error;
	DBusMessage* msg;
	char* str;
	DBusObjectPathVTable vtable ={
  NULL,
  NULL,
  NULL,
	};
	char buf[180] = ""; 
	FILE* fp = NULL;

	const char* fifopath = "C:\\mkfifo1.file";
	int err;
	err = mkfifo (fifopath, 0666);
	   if(err != 0)
	   {
	       // probably file already exists, delete the file
	       unlink(fifopath); 
	       // try once more..
	       err = mkfifo (fifopath, 0666);
	       if(err != 0)
	       {
	            return 1;
	       }
	   }

#if defined(__WINSCW__) || defined(__WINS__)
	fp = popen("Z:\\sys\\bin\\match_rule_server.exe", "r");
#else
	fp = popen("C:\\sys\\bin\\match_rule_server.exe", "r");
#endif
	if(!fp)
		iLog->Log(_L8("fp is NULL")); 

	
	int fd = open(fifopath, O_RDONLY);
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
	unlink(fifopath);
	if(strcmp("done", buf))
		{
		iLog->Log(_L8("done is not returned from server."));
		return 1;
		}
	
	dbus_error_init(&error);
	connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
	 
	if(!connection || dbus_error_is_set(&error))
		return handle_error(&error);
	  
	send_message(connection, "/Test/Signal/Object", "Test.Signal.Send2", "first");
	
	iLog->Log(_L8("First part over"));
	
	dbus_bus_add_match(connection, "type='signal',interface='Test.Signal.Send3'",&error);
	
	if(dbus_error_is_set(&error))
		return handle_error(&error);
	
	iLog->Log(_L8("Registering path"));
	if(!dbus_connection_register_object_path (connection, "/Test/Signal/Object1", &vtable, NULL))
	{ 
		iLog->Log(_L8("Registering path fail"));
		return 1;
	}
	iLog->Log(_L8("Requesting name"));	
	
	if(!dbus_bus_request_name (connection, "test.Signal.Send1", DBUS_NAME_FLAG_ALLOW_REPLACEMENT, &error) == (unsigned int)-1)
	{
		iLog->Log(_L8("Requesting name fail"));	
		return 1;
	}
	
	iLog->Log(_L8("Starting while loop"));
	while(TRUE)  
	{	
		dbus_connection_read_write(connection, 0);
		
		msg = dbus_connection_pop_message(connection);
			
		if(msg == NULL)
		{
			continue; 
		} 
		
		iLog->Log(_L8("Message Detected"));
	
		if(dbus_message_is_signal(msg, "Test.Signal.Send3", "second"))
		{
			if(!dbus_message_get_args(msg, &error, DBUS_TYPE_STRING, &str, DBUS_TYPE_INVALID))
				{
					dbus_error_free(&error);
					return 1;
				}  
			iLog->Log(_L8("Got MSG : %s"),  str);
			break; 
		}    
	
		dbus_message_unref(msg); 
	} 
	
//	sleep(2);
	send_message(connection, "/Test/Signal/Object", "Test.Signal.Send2", "third");
	send_message(connection, "/Test/Signal/Object", "Test.Signal.Send2", "first");	
	send_message(connection, "/Test/Signal/Object", "Test.Signal.Send2", "third");

	while(TRUE)
	{
		dbus_connection_read_write(connection, 0);
		msg = dbus_connection_pop_message(connection);
		if(!msg)
			continue;
		if(!dbus_message_has_destination(msg, "test.Signal.Send1"))
		{
			iLog->Log(_L8("dbus_message_has_destination() failed to check Destination."));
			return 1;
		}
		
		if(dbus_message_get_type(msg) == DBUS_MESSAGE_TYPE_ERROR)
		{
			iLog->Log(_L8("Error is returned."));
			if(!dbus_message_has_member(msg, "error"))
				iLog->Log(_L8("dbus_message_has_destination() failed to check Destination."));
			return 1;
		}
		else if(dbus_message_get_type(msg) == DBUS_MESSAGE_TYPE_METHOD_CALL)
			{
			iLog->Log(_L8("Method call is returned."));
			if(!dbus_message_has_member(msg, "success"))
			{
				iLog->Log(_L8("dbus_message_has_destination() failed to check Destination."));
				return 1;
			}
			break; 
			}
		
	}
	dbus_connection_close(connection);
	dbus_connection_unref(connection);
	dbus_shutdown();
	if(fp) 
		pclose(fp);
	
	iLog->Log(KSuccess);  
	return KErrNone; 
}

TInt Cstif_1::stif_Sign_Test( CStifItemParser& aItem )
{
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles ); 


	DBusConnection* connection;
	DBusError error;
	DBusMessage* msg;
	DBusMessage* reply = NULL;
	dbus_int32_t int_32 = -32;
	double d = 1.1234;
	char* str = "DBus Testing";
	const char* sign;
	const char* sub_sign;
	const char* iter_sign;
	const char* iter_sign1;
	dbus_int32_t type;
	dbus_int32_t arr[] = {0,1,2,3,4,5,6,7,8,9};
	dbus_int32_t* v_arr = arr;
	DBusMessageIter iter;
	DBusMessageIter sub_iter;
	DBusMessageIter sub_iter1;
	struct SignTest
	{
		int num1;
		int num2;
		int num3;
	}st;
	
	dbus_error_init(&error);
	connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
	if(!connection || dbus_error_is_set(&error))
		return handle_error(&error);
	
	msg = dbus_message_new_method_call("Test.Method.Call", "/Test/Method/Object", NULL, "sign_test");
	if(!msg)
	{
		iLog->Log(_L8("msg is NULL"));
		return 1;
	}
	
	st.num1 = 100;
	st.num2 = 200;
	st.num3 = 300;
	
	dbus_message_iter_init_append(msg, &iter);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &int_32);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &str);
	dbus_message_iter_open_container(&iter, DBUS_TYPE_STRUCT, NULL, &sub_iter);
	dbus_message_iter_append_basic(&sub_iter, DBUS_TYPE_INT32, &st.num1);
	dbus_message_iter_append_basic(&sub_iter, DBUS_TYPE_INT32, &st.num2);
	dbus_message_iter_append_basic(&sub_iter, DBUS_TYPE_INT32, &st.num3);
	dbus_message_iter_close_container(&iter, &sub_iter);
	dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, DBUS_TYPE_INT32_AS_STRING, &sub_iter1);
	dbus_message_iter_append_fixed_array(&sub_iter1, DBUS_TYPE_INT32, &v_arr, 10);
	dbus_message_iter_close_container(&iter, &sub_iter1);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_DOUBLE, &d);
	
	reply = dbus_connection_send_with_reply_and_block(connection, msg, 10000, &error);
	if(!reply)
		return handle_error(&error);
	
	if(!dbus_message_get_args(reply, &error, DBUS_TYPE_SIGNATURE, &sign, DBUS_TYPE_SIGNATURE, &sub_sign, DBUS_TYPE_INT32, &type, DBUS_TYPE_SIGNATURE, &iter_sign, DBUS_TYPE_SIGNATURE, &iter_sign1, DBUS_TYPE_INVALID))
		return handle_error(&error);

	iLog->Log(_L8("Signature :: %s\n\tSubSignature :: %s\n\tType :: %c\n\tMessageIterSignature :: %s\n\tMessageIterSignature1 :: %s"), sign, sub_sign, type, iter_sign, iter_sign1);
	if(!dbus_message_has_signature(msg, sign))
	{
		iLog->Log(_L8("Signature returned is not correct."));
		return 1;
	}
	if(strcmp("i", iter_sign))
	{
		iLog->Log(_L8("Signature returned is not correct."));
		return 1;
	}
	if(strcmp("(iii)", sub_sign))
	{
		iLog->Log(_L8("SubSignature returned is not correct."));
		return 1;
	}
	if(strcmp("(iii)", iter_sign1))
	{
		iLog->Log(_L8("SubSignature returned is not correct."));
		return 1;
	}
	if('i' != type)
	{
		iLog->Log(_L8("type returned is not correct"));
		return 1;
	}
	
	dbus_message_unref(msg);
	dbus_message_unref(reply);
	
	dbus_connection_close(connection);
	dbus_connection_unref(connection);
	dbus_shutdown();
	
	iLog->Log(KSuccess);  
	return KErrNone; 
}



TInt Cstif_1::stif_N_Separate_Connections( CStifItemParser& aItem )
{
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles ); 

	const char* fifopath = "C:\\mkfifo.file";
	int cnt =0;
	int err =0;
	int fd;
	char buf[20] = {NULL};
	pid_t pid;
	
	err = mkfifo (fifopath, 0666);
	   if(err != 0)
	   {
	       // probably file already exists, delete the file
	       unlink(fifopath); 
	       // try once more..
	       err = mkfifo (fifopath, 0666);
	       if(err != 0)
	       {
	            return 1;
	       }
	   }
	
	for(cnt=0; cnt<MAX_CONN_EXE; cnt++)
	{
#if defined(__WINSCW__) || defined(__WINS__)
		posix_spawn(&pid, "Z:\\sys\\bin\\private_connection.exe", NULL, NULL, NULL, NULL);
#else
		posix_spawn(&pid, "C:\\sys\\bin\\private_connection.exe", NULL, NULL, NULL, NULL);
#endif
		fd = open(fifopath, O_RDONLY);
		if(fd > 0)
		{
			err = read (fd, buf, 20);  
			close(fd);
			iLog->Log(_L8("%d :: %s"), cnt, buf);
			if(!strcmp("Error Occured", buf))
				{
				iLog->Log(_L8("Error at %d EXE"), cnt);
				return 1;
				}
		}
	} 
	
	unlink(fifopath);
	
	return KErrNone;
}

TInt Cstif_1::stif_Pending_Call_Data_Slot0( CStifItemParser& aItem )
	{
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
    TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );
    

		DBusConnection* connection;
		DBusError error;
		DBusMessage* msg;
		DBusPendingCall* pending;
		dbus_int32_t no;
		int data_slot = -1;
		TestData* data;
		TestData1 data1;
		DBusFreeFunction free_data;
		
		DBusPendingCallNotifyFunction reply_handler;
			
		dbus_error_init(&error);
		
		connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
		if(!connection || dbus_error_is_set(&error))
			handle_error(&error);
		
		msg = dbus_message_new_method_call("Test.Method.Call", "/Test/Method/Object", "test.Method.Call", "simple");
		if(!msg)
		{
			iLog->Log(_L8("Failed to creat Messsage"));
			return 1;
		}
		
		dbus_message_append_args(msg, DBUS_TYPE_INT32, &no, DBUS_TYPE_INVALID);
		dbus_connection_send_with_reply(connection, msg, &pending, -1);
		if(!pending)
		{
			iLog->Log(_L8("pending is NULL"));
			return 1;
		}
		
		if(!dbus_pending_call_allocate_data_slot(&data_slot))
		{
			iLog->Log(_L8("Not able to allocate data slot"));
			return 1;
		}
		
		data = (TestData*)malloc(sizeof(TestData));
		data->num = 123;
		data->str = (char*)malloc(20);
		strcpy(data->str, "Check DBus");
		free_data = Free_Test_Data;
		data_free_flag = FALSE;
		data1.data = data;
		data1.data_slot = data_slot;
		if(!dbus_pending_call_set_data(pending, data_slot, data, free_data))
			{
				iLog->Log(KOOM);
				return 1; 
			}
		
		dbus_connection_flush(connection);
		
		reply_handler = handle_reply_pending_call_data_slot0;
	    if(!dbus_pending_call_set_notify(pending, reply_handler, &data1, NULL))
	    	{
	    	iLog->Log(_L8("Failed to set handler for pending call"));
	    	return 1;
	    	}
		
		dbus_pending_call_block(pending);
		dbus_message_unref(msg);		
		
		iLog->Log(_L8("Reply = %d"), pending_call_ret_val);
		switch(pending_call_ret_val)
		{
			case 0:
				break;
			case -2:
				iLog->Log(_L8("Error while stealing reply from pending call."));
				return 1;
			case -3:
				iLog->Log(_L8("Not able to retrive data from pending call."));
				return 1;
			case -4:
				iLog->Log(_L8("Not able to Reset data for pending call."));
				return 1;
			case -5:
				iLog->Log(_L8("Free Function is not called."));
				return 1;
			default:
				iLog->Log(_L8("Notify function is not called"));
				return 1;
		}
		if(!data_free_flag)
			{
			iLog->Log(_L8("Free Function is not called."));
			return 1;
			}
		
		dbus_connection_close(connection);
		dbus_connection_unref(connection);
		dbus_shutdown();
		iLog->Log(KSuccess);
		return KErrNone;
	}


int Cstif_1::send_file(DBusConnection* connection, char* file_name)
{
	FILE* fp;
	unsigned char arr[200];
	unsigned char* data;
	//const unsigned char* p_arr = arr;
	DBusError error;
	DBusMessage* msg;
	DBusMessage* reply;
	DBusMessageIter append_iter;
	DBusMessageIter sub_iter;
	int num=1;
	int size=0;
	TTime NowTime;
	TTime NowTime1;
	TTimeIntervalMicroSeconds micro_sec;
	TInt64 diff_int64;
	dbus_error_init(&error);
	
	fp = fopen(file_name, "r"); 
	if(!fp) 
	{
		iLog->Log(_L8("Not able to open File"));
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
	fp = fopen(file_name, "r");
	if(!fp) 
		{
			iLog->Log(_L8("Not able to open File"));
			return 1;
		}  
	data = (unsigned char*)dbus_malloc(size);
	if(!data)
		{
		iLog->Log(_L8("Errno %d"), errno);
		iLog->Log(_L8("data is NULL for %s of size %d."), file_name, size);
		return 1;
		}
	if(size != fread(data, sizeof(unsigned char), size, fp))
	{
		iLog->Log(_L8("Error while reading file"));
		return 1; 
	}

	if(fp)
		fclose(fp);
	msg = dbus_message_new_method_call("Test.Method.Call", "/Test/Method/Object", "test.Method.Call", "file_send_time");
	if(!msg)
	{ 
		iLog->Log(_L8("msg is NULL."));
		return 1;
	} 
	 
	iLog->Log(_L8("msg is created")); 
	  
	dbus_message_iter_init_append(msg, &append_iter);
	dbus_message_iter_append_basic(&append_iter, DBUS_TYPE_STRING, &file_name);
	
	// For Array	
	if(!dbus_message_iter_open_container(&append_iter, DBUS_TYPE_ARRAY, DBUS_TYPE_BYTE_AS_STRING, &sub_iter))
		{
		iLog->Log(KOOM);
		return 1;
		}	 
	if(!dbus_message_iter_append_fixed_array(&sub_iter, DBUS_TYPE_BYTE, &data, size))
		{ 
		iLog->Log(KOOM);
		return 1;  
		}   
	dbus_message_iter_close_container(&append_iter, &sub_iter);
	
	iLog->Log(_L8("Array Appended"));
	NowTime.HomeTime();
	reply = dbus_connection_send_with_reply_and_block(connection, msg, 15000, &error);
	NowTime1.HomeTime();
	if(!reply)
	{
		iLog->Log(_L8("Failed to get reply for %s of size %d"), file_name, size);
		iLog->Log(_L8("Error Message :: %s"), error.message);
		return 1;
	}
	
	micro_sec = NowTime1.MicroSecondsFrom(NowTime);
	diff_int64 = micro_sec.Int64();
	iLog->Log(_L8("Time to send %s file of size %d is %d"), file_name, size, diff_int64);
	dbus_free(data);
	
	return 0;
}

TInt Cstif_1::stif_Time0( CStifItemParser& aItem )
{
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  

	DBusError error;
	DBusConnection *connection;
	
	TTime NowTime;
	TTime NowTime1;
	TTimeIntervalMicroSeconds micro_sec;
	TInt64 diff_int64;
	
	dbus_error_init (&error);
	NowTime.HomeTime();
	connection = dbus_bus_get_private (DBUS_BUS_SESSION, &error);
	NowTime1.HomeTime();
	if (connection == NULL)
	  {
	  			iLog->Log(_L8("*** Failed to open connection to system bus: %s\n%s"),  error.name, error.message);
	    dbus_error_free (&error);
	    return 1;
	  }
	micro_sec = NowTime1.MicroSecondsFrom(NowTime);
	diff_int64 = micro_sec.Int64();
	iLog->Log(_L8("Time to get 1st time connection is %d"), diff_int64);
	
	if(send_file(connection, "c:\\inbox1.jpg"))
		return 1;
	sleep(1);
		
	if(send_file(connection, "c:\\bunbask1.jpg"))
		return 1;
	sleep(1);
	
	if(send_file(connection, "c:\\sound_clip.amr"))
		return 1;
	sleep(1);
	
	if(send_file(connection, "c:\\inbox.jpg"))
		return 1;
	sleep(1);
	
	if(send_file(connection, "c:\\sound_clip1.amr"))
		return 1;
	sleep(1);
	
	if(send_file(connection, "c:\\sound_clip2.amr"))
		return 1;
	sleep(1);
	
	if(send_file(connection, "c:\\sound_clip4.amr"))
		return 1;
	sleep(1);
	
	if(send_file(connection, "c:\\sound_clip5.amr"))
		return 1;
	sleep(1);
	
	if(send_file(connection, "c:\\sound_clip6.amr"))
		return 1;
	sleep(1);
	
	if(send_file(connection, "c:\\sound_clip3.amr"))
		return 1;
	
	dbus_connection_close(connection);
	dbus_connection_unref(connection);
	dbus_shutdown();
	iLog->Log(KSuccess);
	return 0;
}

TInt Cstif_1::stif_Daemon_Run_Time0( CStifItemParser& aItem )
    { 
    TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
    TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles ); 
    
#if defined(__WINSCW__) || defined(__WINS__)
    FILE* fp = popen("z:\\sys\\bin\\idle_daemon_run.exe", "r");
#else
    FILE* fp = popen("c:\\sys\\bin\\idle_daemon_run.exe", "r");
#endif
    if(!fp)
    	{
    	iLog->Log(_L8("Fail to open exe"));
    	return 1;
    	}
    
	iLog->Log(KSuccess);
	return KErrNone;
    }

int make_fifo(char* fifopath)
{
	int err =0;
	err = mkfifo (fifopath, 0666);
	   if(err != 0)
	   {
	       // probably file already exists, delete the file
	       unlink(fifopath); 
	       // try once more..
	       err = mkfifo (fifopath, 0666);
	       if(err != 0)
	       {
	            return 1;
	       }
	   }
	   return 0;
}

int read_fifo(char* fifopath, char* str)
{
	int fd = open(fifopath, O_RDONLY);
	int err;
	char buf[180];
	
	if(fd > 0)
	{
		err = read (fd, buf, 80);
		RDebug::Printf("error value is:%d\n", err);
		close(fd);
	}
	else
		{
		return 1;
		}
	unlink(fifopath);
	if(strcmp(buf, "done"))
	{
		strcpy(str, buf);
		return 1;
	}
	
	return 0;
}

TInt Cstif_1::stif_Simultaneous_Connections( CStifItemParser& aItem )
{
	FILE* fp[3];
	int cnt;
	char res[80];
	
	char* fifopath[3] = {	"C:\\mkfifo001.file",
								"C:\\mkfifo002.file",
								"C:\\mkfifo003.file"};
	for(cnt=0; cnt<3; cnt++)
	{
		if(make_fifo(fifopath[cnt]))
			{
			iLog->Log(_L8("Failed to open FIFO for %d count"), cnt);
			return 1;
			}
	}
	
#if defined(__WINSCW__) || defined(__WINS__)
	char* exe_names[] = { 	"z:\\sys\\bin\\get_connection_1.exe",
							"z:\\sys\\bin\\get_connection_2.exe",
							"z:\\sys\\bin\\get_connection_3.exe"
						};
#else
	char* exe_names[] = { 	"c:\\sys\\bin\\get_connection_1.exe",
							"c:\\sys\\bin\\get_connection_2.exe",
							"c:\\sys\\bin\\get_connection_3.exe"
						};
#endif
	
	
	for(cnt=0; cnt<3; cnt++)
		{
			fp[cnt] = popen(exe_names[cnt], "r");
			if(!fp[cnt])
				{
				iLog->Log(_L8("Failed to open %d th EXE."), cnt);
				return 1;
				}
		}
	
	for(cnt=0; cnt<3; cnt++)
	{
		if(read_fifo(fifopath[cnt], res))
			{
			iLog->Log(_L8("Failed to read FIFO for %d exe and %s"), cnt, res);
			return 1;
			}
	}	
	
	iLog->Log(KSuccess);
	return KErrNone;
}

