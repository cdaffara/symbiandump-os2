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
#include <unistd.h>					//sleep function 
#include "stif_3.h" 

#include <pthread.h> 
#include <string.h>

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
	TestData data;
};


 


	




// ============================ MEMBER FUNCTIONS ===============================
int Cstif_3::handle_error(DBusError* error)
	{
	char err[80];
	sprintf(err, "%s", error->name);
	iLog->Log(_L8(err));
	sprintf(err, "%s", error->message);
	iLog->Log(_L8(err));
	dbus_error_free(error);
	return 1; 
	} 

// -----------------------------------------------------------------------------
// Cstif_3::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Cstif_3::Delete() 
    {

    }
TBool TestConnectionEqual(DBusConnection* conn1, DBusConnection* conn2)
	{
		return (conn1==conn2? TRUE : FALSE);
	}

// -----------------------------------------------------------------------------
// Cstif_3::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Cstif_3::RunMethodL( 
    CStifItemParser& aItem )  
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
      //ENTRY( "Example", Cstif_3::ExampleL ),
        ENTRY( "stif_File_Writecontact", Cstif_3::stif_file_writecontact),      
        ENTRY( "stif_dbus_connection_borrow_message0", Cstif_3::stif_dbus_connection_borrow_message0),
        ENTRY( "stif_dbus_connection_pop_message0", Cstif_3::stif_dbus_connection_pop_message0 ), 
        ENTRY( "stif_dbus_connection_steal_borrowed_message0", Cstif_3::stif_dbus_connection_steal_borrowed_message0),
        ENTRY( "stif_dbus_connection_return_message0", Cstif_3::stif_dbus_connection_return_message0),
        ENTRY( "stif_dbus_connection_preallocate_send0", Cstif_3::stif_dbus_connection_preallocate_send0),
       ENTRY( "stif_dbus_connection_get_outgoing_size0", Cstif_3::stif_dbus_connection_get_outgoing_size0),
        ENTRY ("stif_dbus_message_new_error_printf0", Cstif_3::stif_dbus_message_new_error_printf0),
        ENTRY ("stif_dbus_message_get_no_reply0",Cstif_3::stif_dbus_message_get_no_reply0)
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove 

        }; 

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// Cstif_3::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//


TInt Cstif_3::stif_file_writecontact( CStifItemParser& aItem )
	{

	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
    TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  
    //_dbus_wsd_reset();
	_LIT(KOOM, "Out Of Memory");
	DBusConnection* connection;
	DBusError error;
	DBusMessage* msg;
	DBusMessage* msg1;
	DBusMessageIter append_iter;
	DBusMessageIter return_iter;
	DBusMessageIter sub_iter;
	DBusPendingCall* pending;
	char error_name[40];
	char error_msg[40];
	
	
	char* arg_str = "DBus Testing";
	char* arg_obj_path = "\Test\Object\Path";
	int size;
	struct data_contact
	{
		char *name;
		char *ph_no;
		dbus_int32_t cnt;
	}data;
	size = sizeof(struct data_contact);
	iLog->Log(_L8("size of struct is %d"),size);   
	 
	
	data.name = "DBus";
	data.ph_no = "+91234455";
	//cnt=0;
	dbus_int32_t return_value;
	char ret_msg[20]; 
	
	dbus_error_init(&error);
	
	connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
	if(!connection)
	{
		sprintf(error_name, "Error_name : %s", error.name);
		iLog->Log(_L8(error_name));
		sprintf(error_msg, "Error_msg : %s", error.message);
		iLog->Log(_L8(error_msg));
		return 1;
	}  
	msg = dbus_message_new_method_call("Test.Method.Call1", "/Test/Method/Object", "test.Method.Call", "file_writecontact");
	if(msg == NULL)
		{ 
		iLog->Log(_L8("message error"));
		return 1;
		}
	iLog->Log(_L8("message created successfully"));  
	
	dbus_message_iter_init_append(msg, &append_iter);
	
	if(!dbus_message_iter_append_basic(&append_iter, DBUS_TYPE_STRING, &arg_str))
		{
		iLog->Log(KOOM);
		return 1;
		}
	
	// structure 
	
		 
	for(int i=0; i<=50; i++)  
	{
	if(!dbus_message_iter_open_container(&append_iter, DBUS_TYPE_STRUCT, NULL, &sub_iter))
		{
		iLog->Log(KOOM); 
		return 1;
		}	     
		data.cnt = i; 
		if(!dbus_message_iter_append_basic(&sub_iter, DBUS_TYPE_INT32, &data.cnt))
		{ 
			iLog->Log(KOOM);
			return 1;  
		} 
		if(!dbus_message_iter_append_basic(&sub_iter, DBUS_TYPE_STRING, &data.name))
		{ 
			iLog->Log(KOOM);
			return 1;  
		} 
		if(!dbus_message_iter_append_basic(&sub_iter, DBUS_TYPE_STRING, &data.ph_no))
		{ 
			iLog->Log(KOOM);
			return 1;  
		} 
		dbus_message_iter_close_container(&append_iter, &sub_iter);  // for 80 structure
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
	   msg1 = dbus_pending_call_steal_reply(pending);
	   if (NULL == msg1) { 
	   iLog->Log(_L8("Reply error"));
	 
	      exit(1);
	   } 
	   
	   // free the pending message handle
	   dbus_pending_call_unref(pending);
		 
		dbus_error_init (&error);
		dbus_message_iter_init(msg1, &return_iter);

		dbus_message_iter_get_basic(&return_iter, &return_value);
		sprintf(ret_msg,"Reply = %d", return_value); 
		iLog->Log(_L8(ret_msg));
		if(return_value != 50)
			{
			iLog->Log(_L8("Return value is not what is sent"));
			return 1;
			} 
			dbus_message_unref(msg);  
	   
	   dbus_connection_close(connection);
	   dbus_connection_unref(connection);
	   dbus_shutdown();
 		   iLog->Log( KSuccess );
		    return KErrNone;  
	
	}
TInt Cstif_3::stif_dbus_connection_borrow_message0( CStifItemParser& aItem )
	{

		TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
		TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  
		
		DBusConnection* connection;
		DBusError error;
		DBusMessage* msg;
		DBusMessage* borrow_message;
		DBusMessage* reply = NULL;
		dbus_int32_t no = 5;
		char error_name[40]; 
		char error_msg[40];
		
 		dbus_error_init(&error); 
		
		connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
	
		if(!connection)
		{
			sprintf(error_name, "Error_name : %s", error.name);
			iLog->Log(_L8(error_name));
			sprintf(error_msg, "Error_msg : %s", error.message);
			iLog->Log(_L8(error_msg));
			return 1;
		}  
		msg = dbus_message_new_method_call("Test.Method.Call1", "/Test/Method/Object", "test.Method.Call", "dbus_connection_borrow_message0");
		if(msg == NULL)
			{ 
			iLog->Log(_L8("message error"));
			return 1;
			}

		
		reply = dbus_connection_send_with_reply_and_block(connection, msg, 10000, &error);
		if(!reply)
			{
			  dbus_message_unref(msg);  
				
					dbus_connection_unref(connection);
					return handle_error(&error);
					
			}		
		
		iLog->Log(_L8("Reply Message is been received "));
		
		
		
		dbus_message_get_args(reply, &error, DBUS_TYPE_INT32, &no, DBUS_TYPE_INVALID);
		
	
		
	// to check the reply is proper	
		if(no == 9090)
			   {
			   iLog->Log(_L8("the reply is proper"));
			   }
		borrow_message =  dbus_connection_borrow_message (connection )   ;
		if(borrow_message== NULL)
			{
				iLog->Log(_L8("Queue is empty"));
			}
		else
			{
				iLog->Log(_L8("borrow message is executed succesfully"));
				iLog->Log( KSuccess );
			    
			  dbus_message_unref(msg);  
			   
			   dbus_connection_close(connection);
			   dbus_connection_unref(connection);
			   dbus_shutdown();
 
			    return KErrNone; 
			}
	
		return 0;
		
	}

TInt Cstif_3::stif_dbus_connection_pop_message0( CStifItemParser& aItem )
	{

		TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
		TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles ); 
		
		
		DBusConnection* connection;
		DBusError error; 
		DBusMessage* msg;
		DBusMessage*pop_message;
		DBusMessage* reply = NULL;
		char error_name[40];
		char error_msg[40];
		dbus_int32_t no = 5;
		
		dbus_error_init(&error);
		
		connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
	
		if(!connection)
		{
			sprintf(error_name, "Error_name : %s", error.name);
			iLog->Log(_L8(error_name));
			sprintf(error_msg, "Error_msg : %s", error.message);
			iLog->Log(_L8(error_msg));
			return 1;
		}  
		msg = dbus_message_new_method_call("Test.Method.Call1", "/Test/Method/Object", "test.Method.Call", "dbus_connection_pop_message0");
		if(msg == NULL)
			{ 
			iLog->Log(_L8("message error"));
			return 1;
			}
		
		reply = dbus_connection_send_with_reply_and_block(connection, msg, 10000, &error);
		if(!reply)
			return handle_error(&error);
		dbus_message_get_args(reply, &error, DBUS_TYPE_INT32, &no, DBUS_TYPE_INVALID);
	
		pop_message =  dbus_connection_pop_message (connection )   ;
		
		iLog->Log(_L8("Pop message is been executed succesfully"));
		iLog->Log(_L8("Checking for the message in the Queue"));
		pop_message = dbus_connection_borrow_message (connection );
		if(pop_message == NULL)
			{
			iLog->Log(_L8("Queue is empty as the pop message is removed the message from the Queue"));
				iLog->Log( KSuccess );
				dbus_message_unref(msg);  
				   
				   dbus_connection_close(connection);
				   dbus_connection_unref(connection);
				   dbus_shutdown();
 
				
			    return KErrNone;  
			
				
			}
		else
			{
				
				iLog->Log(_L8("TEST CASE FAILED"));
				return 1;	
				
			}
	}
TInt Cstif_3::stif_dbus_connection_steal_borrowed_message0( CStifItemParser& aItem )
	{

		TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
		TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  
		
		//_dbus_wsd_reset();
		DBusConnection* connection;
		DBusError error; 
		DBusMessage* msg;
		DBusMessage*message;
		DBusMessage* reply = NULL;
		char error_name[40];
		char error_msg[40];
		
		dbus_error_init(&error);
		
		connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
	
		if(!connection)
		{
			sprintf(error_name, "Error_name : %s", error.name);
			iLog->Log(_L8(error_name));
			sprintf(error_msg, "Error_msg : %s", error.message);
			iLog->Log(_L8(error_msg));
			return 1;
		}  
		
		msg = dbus_message_new_method_call("Test.Method.Call1", "/Test/Method/Object", "test.Method.Call", "dbus_connection_steal_borrowed_message0");
		
		if(msg == NULL)
			{ 
			iLog->Log(_L8("message error"));
			return 1;
			}
		
	
// reply 		
		reply = dbus_connection_send_with_reply_and_block(connection, msg, 10000, &error);
		if(!reply)
			return handle_error(&error);
		message =  dbus_connection_borrow_message (connection )   ;
		
		dbus_connection_steal_borrowed_message(connection,message);
		
		message =  dbus_connection_borrow_message (connection )   ;
		
	
		if(message== NULL)
		{
			iLog->Log(_L8("Queue is empty"));
			iLog->Log(_L8("Test success as the message is been stealed "));
			iLog->Log( KSuccess );
			dbus_message_unref(msg);  
	   
	   dbus_connection_close(connection);
	   dbus_connection_unref(connection);
	   dbus_shutdown();

		    return KErrNone;  
		}
		else
			{
				iLog->Log(_L8("steal borrowed message is not executed properly "));
				iLog->Log(_L8("Test case failed "));
				return 1;
			}	// end of else 	
			
		
	}




TInt Cstif_3::stif_dbus_connection_return_message0( CStifItemParser& aItem )
	{

	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  

	DBusConnection* connection;
	DBusError error;
	DBusMessage* msg;
	DBusMessage* borrow_message;
	DBusMessage* reply = NULL;
	char error_name[40];
	char error_msg[40];
	
	dbus_error_init(&error);
	
	connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);

	if(!connection)
	{
		sprintf(error_name, "Error_name : %s", error.name);
		iLog->Log(_L8(error_name));
		sprintf(error_msg, "Error_msg : %s", error.message);
		iLog->Log(_L8(error_msg));
		return 1;
	}  
	msg = dbus_message_new_method_call("Test.Method.Call1", "/Test/Method/Object", "test.Method.Call", "dbus_connection_return_message0");
	if(msg == NULL)
		{ 
		iLog->Log(_L8("message error"));
		return 1;
		}
		
	reply = dbus_connection_send_with_reply_and_block(connection, msg, 10000, &error);
	if(!reply)
		return handle_error(&error);
	iLog->Log(_L8("Reply Message is been received "));
	

	borrow_message =  dbus_connection_borrow_message (connection )   ;
	
	if(borrow_message== NULL)
		{
			iLog->Log(_L8("Queue is empty"));
			iLog->Log(_L8("Test case failed"));
			return 1;
		}
	else
		{
			dbus_connection_return_message(connection,borrow_message);
			iLog->Log(_L8("dbus_connection_return_message is executed succesfully"));
			iLog->Log( KSuccess );
			dbus_message_unref(msg);  
	   
		   dbus_connection_close(connection);
		   dbus_connection_unref(connection);
		   dbus_shutdown();
 
		    return KErrNone;
		}
	}


TInt Cstif_3::stif_dbus_connection_preallocate_send0( CStifItemParser& aItem )
	{

		TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
		TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  
	
		DBusConnection* connection;
		DBusError error;
		DBusMessage* msg;
		char error_name[40];
		char error_msg[40];
		DBusPreallocatedSend* resource; 
		dbus_error_init(&error);
		
		connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);

		if(!connection)
		{
			sprintf(error_name, "Error_name : %s", error.name);
			iLog->Log(_L8(error_name));
			sprintf(error_msg, "Error_msg : %s", error.message);
			iLog->Log(_L8(error_msg));
			return 1;
		}  
		
	resource = dbus_connection_preallocate_send( connection ); 
	dbus_connection_free_preallocated_send(connection,resource);
	if(resource == NULL)
		{
			iLog->Log(_L8("Preallocates resources needed to send a message is failed  "));
			iLog->Log(_L8(" test case Failed"));
			return 1;
		}
	else 
			iLog->Log(_L8("Preallocation of  resources needed to send a message is been created   "));
			
	
 
	iLog->Log( KSuccess );
	  
	
	   
	   dbus_connection_close(connection);
	   dbus_connection_unref(connection);
	   dbus_shutdown();
 
    return KErrNone;    
	}



TInt Cstif_3::stif_dbus_message_new_error_printf0( CStifItemParser& aItem )
	{ 
		
		TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
		TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  

		DBusConnection* connection;
		DBusError error;
		DBusMessage* msg;
		char error_name[40];
		char error_msg[40];
		dbus_error_init(&error);
		DBusMessage* reply = NULL;
		long outgoing_size = 0 ;
		const char* errorname;
		errorname = "dbus.test.error"; 
		
		FILE *fp;
		long lsize;
		char *buffer;
		size_t result;
		
		connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
	
		if(!connection)
		{
			sprintf(error_name, "Error_name : %s", error.name);
			iLog->Log(_L8(error_name));
			sprintf(error_msg, "Error_msg : %s", error.message);
			iLog->Log(_L8(error_msg));
			return 1; 
		}  
		msg = dbus_message_new_method_call("Test.Method.Call1", "/Test/Method/Object", "test.Method.Call", "dbus_message_new_error_printf0");
		if(msg == NULL) 
			{ 
			iLog->Log(_L8("message error"));
			return 1;
			} 
		else 
			iLog->Log(_L8("message created"));
	  	reply = dbus_connection_send_with_reply_and_block(connection, msg, 10000, &error);
	  	if(!reply)
	  		{	  		
	  		fp = fopen("c:\\method1newerror.txt","rb");   // the file msggetnorply.txt is been written in method1.c
			if(fp == NULL)
				iLog->Log(_L8("File doesnt exists"));
				fseek(fp,0,SEEK_END); 
				lsize = ftell(fp);
				rewind(fp);								// Sets the position indicator associated with stream to the beginning of the file.
				
				buffer = (char*) malloc (sizeof(char)*lsize);
				if(buffer == NULL) 
					iLog->Log(_L8("memory error"));
					
				result = fread(buffer,1,lsize,fp);	
				if (result != lsize) 
					iLog->Log(_L8("reading error")); 
				iLog->Log(_L8("%s"),buffer);
	  			iLog->Log( KSuccess );  
	  			return;
	  		}	
	  	dbus_message_unref(msg);  
	   
	   dbus_connection_close(connection);
	   dbus_connection_unref(connection);
	   dbus_shutdown();
 	 	 return 0;
			
	}

TInt Cstif_3::stif_dbus_message_get_no_reply0( CStifItemParser& aItem )
	{

		TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
		TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  
		DBusConnection* connection;
		DBusError error;
		DBusMessage* msg;
		char error_name[40];
		char error_msg[40];
		dbus_error_init(&error);
		DBusMessage* reply = NULL;
		dbus_bool_t noreply = TRUE;
		/*noreply =
		 *        TRUE if the  " Set reply to be set"  / the message doesnt wait for reply
     		      FALSE if the " Set reply not to be set" / reply may be expected   */
		
		FILE *fp;
		long lsize;
		char *buffer;
		size_t result;
				
		connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error); 
		if(!connection)
		{
			sprintf(error_name, "Error_name : %s", error.name);
			iLog->Log(_L8(error_name));
			sprintf(error_msg, "Error_msg : %s", error.message);
			iLog->Log(_L8(error_msg));
			return 1;
		}  
		msg = dbus_message_new_method_call("Test.Method.Call1", "/Test/Method/Object", "test.Method.Call", "dbus_message_get_no_reply0");
		if(msg == NULL)
			{ 
			iLog->Log(_L8("message error"));
			return 1;
			}
		dbus_message_set_no_reply( msg,noreply);	 
		dbus_connection_send(connection, msg, NULL);
		iLog->Log(_L8("Message sent"));
		sleep (5);
		fp = fopen("c:\\msggetnorply.txt","rb");   // the file msggetnorply.txt is been written in method1.c
		if(fp == NULL)
			iLog->Log(_L8("File doesnt exists"));
			fseek(fp,0,SEEK_END); 
			lsize = ftell(fp);
			rewind(fp);								// Sets the position indicator associated with stream to the beginning of the file.
			
			buffer = (char*) malloc (sizeof(char)*lsize);
			if(buffer == NULL) 
				iLog->Log(_L8("memory error"));
				
			result = fread(buffer,1,lsize,fp);	
			if (result != lsize) 
				iLog->Log(_L8("reading error")); 
			iLog->Log(_L8("%s"),buffer);
			iLog->Log( KSuccess ); 
			dbus_message_unref(msg);  
	   
	   dbus_connection_close(connection);
	   dbus_connection_unref(connection);
	   dbus_shutdown();
 
       	return KErrNone;
        	
      
	 
	} 
 
TInt Cstif_3::stif_dbus_connection_get_outgoing_size0( CStifItemParser& aItem )
	{ 
		
		TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
		TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  

		DBusConnection* connection;
		DBusError error;
		DBusMessage* msg;
		char error_name[40];
		char error_msg[40];

		dbus_error_init(&error);
		DBusMessage* reply = NULL; 
		long outgoing_size = 0 ;
		
		connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
	
		if(!connection)
		{
			sprintf(error_name, "Error_name : %s", error.name);
			iLog->Log(_L8(error_name));
			sprintf(error_msg, "Error_msg : %s", error.message);
			iLog->Log(_L8(error_msg));
			return 1;
		}  
		msg = dbus_message_new_method_call("Test.Method.Call1", "/Test/Method/Object", "test.Method.Call", "dbus_connection_get_outgoing_size0");
		if(msg == NULL)
			{ 
			iLog->Log(_L8("message error"));
			return 1;
			}
		outgoing_size = dbus_connection_get_outgoing_size( connection ) ;
		iLog->Log(_L8("out going message size before sent %d"),outgoing_size);
		dbus_connection_send(connection, msg, NULL);
	
	
		iLog->Log(_L8("Message sent"));
 		outgoing_size = dbus_connection_get_outgoing_size( connection ) ;
		iLog->Log(_L8("out going message size %d"),outgoing_size);
		dbus_message_unref(msg);  
	   
	   dbus_connection_close(connection);
	   dbus_connection_unref(connection);
	   dbus_shutdown();
 
		iLog->Log( KSuccess ); 
		  return KErrNone;  
		
	}


// -----------------------------------------------------------------------------
// Cstif_3::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt Cstif_3::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
