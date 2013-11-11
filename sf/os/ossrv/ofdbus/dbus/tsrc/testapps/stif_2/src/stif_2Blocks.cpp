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
#include "stif_2.h"  
#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <sys/stat.h>
#include <fcntl.h>

_LIT( KConnectionFail, "Failed to Open the connection with Error :: %s" );
_LIT( KFail, "Test Fail" );
_LIT( KSuccess, "Test Successful" );


TBool data_free_flag = FALSE;

struct TestData
{
	int num;
	char* str;
};

TBool TestConnectionEqual(DBusConnection* conn1, DBusConnection* conn2)
	{
		return (conn1==conn2? TRUE : FALSE);
	}
 

TBool TestMessageEqual(DBusMessage* msg1, DBusMessage* msg2)
{
	if(dbus_message_get_type(msg2) != dbus_message_get_type(msg1))
		return FALSE;
	
	if(strcmp(dbus_message_get_interface(msg2), dbus_message_get_interface(msg1)))
		return FALSE;
	
	if(strcmp(dbus_message_get_path(msg2), dbus_message_get_path(msg1)))
		return FALSE;
	
	if(strcmp(dbus_message_get_member(msg2), dbus_message_get_member(msg1)))
		return FALSE;
	  
	return TRUE;
}
	
void Free_Test_Data(void* data)
	{
		TestData* test_data;
		_LIT(KFree, "In Free function of TestData.");
		
		test_data = (TestData*)data;
		free(test_data->str);
		
		data_free_flag = TRUE;
	}



// ============================ MEMBER FUNCTIONS ===============================

TBool Cstif_2::CheckConnection(DBusConnection* conn, DBusError* error)
	{
		if(!conn)
		{
			iLog->Log(_L8("Failed to get connection with error :: %s\n%s"), error->name, error->message);
	    	
	    	dbus_error_free(error);
	    	return FALSE;
		}
		
		return TRUE;
	}


TBool Cstif_2::CheckMovedError(DBusError* src, DBusError* dest, char* err)
{
	_LIT(KErr1, "Destination Error is not set.");
	_LIT(KErr2, "Destination Error Name is not set.");
	_LIT(KErr3, "Source Error is not initialised.");
	_LIT(KErr4, "Source Error Name is not NULL.");
	if(dbus_error_is_set(dest))
	{
		if(strcmp(err, dest->name))
		{
			iLog->Log(KErr2);
			return FALSE;
		}
		else
		{
			if(dbus_error_is_set(src))
			{
				iLog->Log(KErr3);
				return FALSE;	
			}
			else
			{
				if(src->name)
				{
					iLog->Log(KErr4);
					return FALSE;			
				}
				else
				{
					return TRUE;
				}
			}
		}
	}
	else
	{
		iLog->Log(KErr1);
		return FALSE;
	}
}


// -----------------------------------------------------------------------------
// Cstif_2::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//

void Cstif_2::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Cstif_2::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Cstif_2::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        		 
                ENTRY( "dbus_error_init0", Cstif_2::DBus_Error_Init0 ),
                ENTRY( "dbus_set_error0", Cstif_2::DBus_Set_Error0 ),
                ENTRY( "dbus_set_error1", Cstif_2::DBus_Set_Error1 ),
                ENTRY( "dbus_set_error2", Cstif_2::DBus_Set_Error2 ),
                ENTRY( "dbus_error_has_name0", Cstif_2::DBus_Error_Has_Name0 ),
                ENTRY( "dbus_error_has_name1", Cstif_2::DBus_Error_Has_Name1 ),
                ENTRY( "dbus_error_is_set0", Cstif_2::DBus_Error_Is_Set0 ),
                ENTRY( "dbus_error_is_set1", Cstif_2::DBus_Error_Is_Set1 ),
                ENTRY( "dbus_move_error0", Cstif_2::DBus_Move_Error0 ),
                ENTRY( "dbus_move_error1", Cstif_2::DBus_Move_Error1 ),
                ENTRY( "dbus_move_error2", Cstif_2::DBus_Move_Error2 ),
                ENTRY( "dbus_bus_get0", Cstif_2::DBus_Bus_Get0 ),
                ENTRY( "dbus_bus_get_private0", Cstif_2::DBus_Bus_Get_Private0 ),
                ENTRY( "dbus_bus_get_unique_name0", Cstif_2::DBus_Bus_Get_Unique_Name0 ),
                ENTRY( "dbus_connection_get_is_authenticated0", Cstif_2::DBus_Connection_Get_Is_Authenticated0 ),
                ENTRY( "dbus_connection_get_is_connected0", Cstif_2::DBus_Connection_Get_Is_Connected0 ),
                ENTRY( "dbus_connection_data_slot0", Cstif_2::DBus_Connection_Data_Slot0),
                ENTRY( "dbus_connection_data_slot1", Cstif_2::DBus_Connection_Data_Slot1),
                ENTRY( "dbus_connection_data_slot2", Cstif_2::DBus_Connection_Data_Slot2),
                ENTRY( "dbus_message_path0", Cstif_2::DBus_Message_Path0),
                ENTRY( "dbus_message_path1", Cstif_2::DBus_Message_Path1),
                ENTRY( "dbus_message_path2", Cstif_2::DBus_Message_Path2),
                ENTRY( "dbus_message_path3", Cstif_2::DBus_Message_Path3),
                ENTRY( "dbus_message_type0", Cstif_2::DBus_Message_Type0),
                ENTRY( "dbus_message_type1", Cstif_2::DBus_Message_Type1),
                ENTRY( "dbus_message_type2", Cstif_2::DBus_Message_Type2),
                ENTRY( "dbus_message_type3", Cstif_2::DBus_Message_Type3),
                ENTRY( "dbus_message_data_slot0", Cstif_2::DBus_Message_Data_Slot0),
                ENTRY( "dbus_message_data_slot1", Cstif_2::DBus_Message_Data_Slot1),
                ENTRY( "dbus_bus_request_name0", Cstif_2::DBus_Bus_Request_Name0),
                ENTRY( "dbus_bus_name_has_owner0", Cstif_2::DBus_Bus_Name_Has_Owner0), 
                ENTRY( "dbus_message_copy0", Cstif_2::DBus_Message_Copy0),
                ENTRY( "dbus_message_type_to_string0", Cstif_2::DBus_Message_Type_To_String0),
        		ENTRY( "dbus_message_type_from_string0", Cstif_2::DBus_Message_Type_From_String0),
        		ENTRY( "dbus_message_interface0", Cstif_2::DBus_Message_Interface0),
        		ENTRY( "dbus_message_interface1", Cstif_2::DBus_Message_Interface1),
        		ENTRY( "dbus_message_error_name0", Cstif_2::DBus_Message_Error_Name0),
        		ENTRY( "dbus_type_basic0", Cstif_2::DBus_Type_Basic0),
        		ENTRY( "dbus_type_container0", Cstif_2::DBus_Type_Container0),
        		ENTRY( "dbus_type_fixed0", Cstif_2::DBus_Type_Fixed0),
        		ENTRY( "dbus_message_is_error0", Cstif_2::DBus_Message_Is_Error0),
        		ENTRY( "dbus_message_iter0", Cstif_2::DBus_Message_Iter0),
        		ENTRY( "dbus_bus_start_service_by_name0", Cstif_2::DBus_Bus_Start_Service_By_Name0),
        		ENTRY( "dbus_server_listen0", Cstif_2::DBus_Server_Listen0),
        		ENTRY( "dbus_server_data_slot0", Cstif_2::DBus_Server_Data_Slot0),
        		ENTRY( "dbus_connection_message_size0", Cstif_2::Dbus_Connection_Message_Size0),
        		ENTRY("dbus_get_local_machine_id",Cstif_2::Dbus_Get_Local_Machine_Id),
         		ENTRY("dbus_server_get_is_connected",Cstif_2::Dbus_Server_Get_Is_Connected),
         		ENTRY("dbus_threads_init_default",Cstif_2::Dbus_Threads_Init_Default),
         		ENTRY("dbus_maloc",Cstif_2::Dbus_Maloc),
         		ENTRY("Dbus_Server_Set_Auth_Mechanism",Cstif_2::Dbus_Server_Set_Auth_Mechanism),
         		ENTRY("DBus_Message_Get_Path_Decompose0",Cstif_2::DBus_Message_Get_Path_Decompose0),
         	

        		//ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

TInt Cstif_2::DBus_Error_Init0( CStifItemParser& aItem )
	{
		DBusError error;
		_LIT(KName, "Name is not NULL after initialisation of error");
		_LIT(KMessage, "Message is not NULL after initialisation of error");
		
		dbus_error_init(&error);
		
		if(error.name)
		{
			iLog->Log(KName);
			return 1;
		}
		
		if(error.message)
		{
			iLog->Log(KMessage);
			return 1;
		}
		
		iLog->Log(KSuccess);
		return KErrNone;
	}
	

TInt Cstif_2::DBus_Set_Error0( CStifItemParser& aItem )
	{
		DBusError error;
		_LIT(KDiff, "Mismatch in Error Name");
		char* err = "Sample Error0";
		
		dbus_error_init(&error);
		
		dbus_set_error(&error, err, NULL);
		
		if(strcmp(err, error.name))
		{
			iLog->Log(KDiff);
			return 1; 
		}
		if(strcmp(err, error.message))
		{
			iLog->Log(_L8("Message set to :: %s"), error.message);
			iLog->Log(_L8("Message is not set to NULL"));
			return 1;
		}
		
		dbus_error_free(&error);
		iLog->Log(KSuccess);
		return KErrNone;
	}
	
	
TInt Cstif_2::DBus_Set_Error1( CStifItemParser& aItem )
	{	
		dbus_set_error(NULL, "Sample Error0", NULL);
		
		iLog->Log(KSuccess);
		return KErrNone;
	}
	

TInt Cstif_2::DBus_Set_Error2( CStifItemParser& aItem )
	{
		DBusError error;
		char str[16] = "Sample Error No";
		int no = 1;
		_LIT(KDiff, "Mismatch in Error Message");
		
		dbus_error_init(&error);
		
		dbus_set_error(&error, "Sample Error0", "This is %s :: %d", str, no);
		
		if(strcmp("This is Sample Error No :: 1", error.message))
		{
			iLog->Log(KDiff);
			return 1;
		}
		
		dbus_error_free(&error);
		iLog->Log(KSuccess);
		return KErrNone;
	}
	

TInt Cstif_2::DBus_Error_Has_Name0( CStifItemParser& aItem )
	{
		DBusError error;
		_LIT(KDiff, "Mismatch in Error Name");
		
		dbus_error_init(&error);
		
		dbus_set_error(&error, "Sample Error0", NULL);
		
		if(!dbus_error_has_name(&error, "Sample Error0"))
		{
			iLog->Log(KDiff);
			return 1;
		}
		
		dbus_error_free(&error);
		iLog->Log(KSuccess);
		return KErrNone;
	}
	
	
TInt Cstif_2::DBus_Error_Has_Name1( CStifItemParser& aItem )
	{
		DBusError error;
		_LIT(KDiff, "Match in Error Name");
		
		dbus_error_init(&error);
		
		dbus_set_error(&error, "Sample Error0", NULL);
		
		if(dbus_error_has_name(&error, "Sample Error01"))
		{
			iLog->Log(KDiff);
			return 1;
		}
		
		dbus_error_free(&error);
		iLog->Log(KSuccess);
		return KErrNone;
	}


TInt Cstif_2::DBus_Error_Is_Set0 ( CStifItemParser& aItem )
	{
		DBusError error;
		_LIT(KErr, "Error is set.");
				
		dbus_error_init(&error);
		
		if(dbus_error_is_set(&error))
		{
			iLog->Log(KErr);
			return 1;
		}
		
		iLog->Log(KSuccess);
		return KErrNone;
	}
	

TInt Cstif_2::DBus_Error_Is_Set1 ( CStifItemParser& aItem )
	{
		DBusError error;
		_LIT(KErr, "Error is not set.");
				
		dbus_error_init(&error);
		dbus_set_error(&error, "Sample Error0", NULL);
		
		if(!dbus_error_is_set(&error))
		{
			iLog->Log(KErr);
			return 1;
		}
		
		dbus_error_free(&error);
		iLog->Log(KSuccess);
		return KErrNone;
	}
	
	
TInt Cstif_2::DBus_Move_Error0 ( CStifItemParser& aItem )
	{
		DBusError src;
		DBusError dest;
		char err[] = "Sample Error0";
		
				
		dbus_error_init(&src);
		dbus_error_init(&dest);
		
		dbus_set_error(&src, err, NULL);
		
		dbus_move_error(&src, &dest);
		
		if(!CheckMovedError(&src, &dest, err))
		{
			iLog->Log(KFail);
			dbus_error_free(&src);
			dbus_error_free(&dest);
			return 1;
		}
		else
		{
			iLog->Log(KSuccess);
			dbus_error_free(&dest);
			return KErrNone;
		}
	}
	
	
TInt Cstif_2::DBus_Move_Error1 ( CStifItemParser& aItem )
	{
		DBusError src;
		DBusError dest;
		char str[] = "Sample Error No.";
		int no = 1;
		char err[] = "Sample Error0";
		_LIT(KErr1, "Destination Message is not set Properly.");
		_LIT(KErr2, "Source Message is not NULL.");
		
				
		dbus_error_init(&src);
		dbus_error_init(&dest);
		
		dbus_set_error(&src, err, "This is %s :: %d", str, no);
		
		dbus_move_error(&src, &dest);
		
		if(!CheckMovedError(&src, &dest, err))
		{
			iLog->Log(KFail);
			dbus_error_free(&src);
			dbus_error_free(&dest);
			return 1;
		}
		else
		{
			if(strcmp("This is Sample Error No. :: 1", dest.message))
			{
				iLog->Log(KErr1);
				dbus_error_free(&src);
				dbus_error_free(&dest);
				return 1;	
			}
			
			if(src.message)
			{
				iLog->Log(KErr2);
				dbus_error_free(&src);
				dbus_error_free(&dest);
				return 1;
			}
			
			dbus_error_free(&dest);
			iLog->Log(KSuccess);
			return KErrNone;
		}
	}



TInt Cstif_2::DBus_Move_Error2 ( CStifItemParser& aItem )
	{
		DBusError src;
		char err[] = "Sample Error0";
		_LIT(KErr, "Either Name or Message of Source is not NULL.");
		_LIT(KErr1, "Source Error is still set");
		
		dbus_error_init(&src);
		
		dbus_set_error(&src, err, NULL);
		
		dbus_move_error(&src, NULL);
				
		if(dbus_error_is_set(&src))
		{
			iLog->Log(KErr1);
			iLog->Log(KFail);
			return 1;
		}
		else
		{
			if(src.name || src.message)
			{
				iLog->Log(KErr);
				return 1;
			}
			else
			{
				iLog->Log(KSuccess);
				return KErrNone;
			}
		}
	}
	
	    
    
TInt Cstif_2::DBus_Bus_Get0( CStifItemParser& aItem )
    {
    TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
    TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  
    
	_LIT( KDiffConn, "Two Different Connections created" );
	
    DBusError error;
    DBusError error1;
    DBusConnection* connection;
    DBusConnection* connection1;
    
    dbus_error_init (&error);
    dbus_error_init (&error1);
    
    connection = dbus_bus_get (DBUS_BUS_SESSION, &error);
        
    if(!CheckConnection(connection, &error))
    {
    	return 1;
    }
    else
    {
    	connection1 = dbus_bus_get (DBUS_BUS_SESSION, &error1);
    	if(!CheckConnection(connection1, &error1))
	    {
	    	return 1;
	    }
    	if( !TestConnectionEqual(connection, connection1) )
    	{
    		iLog->Log(KDiffConn);
    		return 1;
    	}
    }
    
    iLog->Log(KSuccess);
    dbus_connection_unref(connection);
    dbus_connection_unref(connection1);
    return KErrNone;
    }

TInt Cstif_2::DBus_Bus_Get_Private0( CStifItemParser& aItem )
    {
    TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
    TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  
    
	_LIT( KDiffConn, "Different connections not created" );
	
    DBusError error;
    DBusError error1;
    DBusConnection* connection;
    DBusConnection* connection1;
    
    dbus_error_init (&error);
    dbus_error_init (&error1);
    
    connection = dbus_bus_get (DBUS_BUS_SESSION, &error);
        
    if(!CheckConnection(connection, &error))
    {
    	return 1;
    }
    else
    { 
    	connection1 = dbus_bus_get_private (DBUS_BUS_SESSION, &error1);
    	if(!CheckConnection(connection1, &error1))
	    {
	    	return 1;
	    }
    	if( TestConnectionEqual(connection, connection1) )
    	{
    		iLog->Log(KDiffConn);
    		
    		dbus_error_free(&error);
    		dbus_error_free(&error1);
    		return 1;
    	}	
    }
    dbus_connection_unref(connection);
    
    dbus_connection_close(connection1);
    dbus_connection_unref(connection1);	
    
    iLog->Log(KSuccess);
    return KErrNone;

    }
    
    
    
TInt Cstif_2::DBus_Connection_Get_Is_Authenticated0( CStifItemParser& aItem )
	{
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  
	
		DBusError error;
		DBusConnection* connection; 
		_LIT(KAuthFail, "Authentication is not done");
		
		dbus_error_init(&error);
		
		connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
		if(!CheckConnection(connection, &error))
			return 1;
		
		if(!dbus_connection_get_is_authenticated(connection))
		{
			dbus_connection_close(connection);
			dbus_connection_unref(connection);
			dbus_shutdown();
			iLog->Log(KAuthFail);
			return 1;
		}
		dbus_connection_close(connection);
		dbus_connection_unref(connection);
		dbus_shutdown();
		iLog->Log(KSuccess);
		return KErrNone;
	}
    
    
    
TInt Cstif_2::DBus_Connection_Get_Is_Connected0( CStifItemParser& aItem )    
	{
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  
	
		DBusError error;
		DBusConnection* connection;
		_LIT(KConnDisConnected, "There is no Connection");
		_LIT(KConnConnected, "There is still Connection after connection closed");
		
		dbus_error_init(&error);
		
		connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
		if(!CheckConnection(connection, &error))
			return 1;
		
		if(!dbus_connection_get_is_connected(connection))
		{
			iLog->Log(KConnDisConnected);
			return 1;
		}
		
		dbus_connection_close(connection);
		
		if(dbus_connection_get_is_connected(connection))
		{
			iLog->Log(KConnConnected);
			return 1;
		}
			
		dbus_connection_unref(connection);
		dbus_shutdown();
		iLog->Log(KSuccess);
		return KErrNone;
	}
    
    
    
TInt Cstif_2::DBus_Bus_Get_Unique_Name0( CStifItemParser& aItem )
	{
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  
	
		DBusError error;
		DBusConnection* connection;
		char* name;
		char str[30];
		
		_LIT(KName, "Unique Name :: %s");
					
		dbus_error_init(&error);
		
		connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
		if(!CheckConnection(connection, &error))
	    {
	    	return 1;
	    }
		
		name = (char*)dbus_bus_get_unique_name(connection);
		
		if(!name)
			return 1;
		
		strcpy(str, name);
		
		iLog->Log(_L8("Unique Name :: %s"), str);
		
		dbus_connection_close(connection);
		dbus_connection_unref(connection);
		dbus_shutdown();
		
		iLog->Log(KSuccess);
		return KErrNone;
	}
       
    
TInt Cstif_2::DBus_Connection_Data_Slot0( CStifItemParser& aItem )  
	{
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  
	
		DBusError error;
		DBusConnection* connection;
		dbus_int32_t data_slot = -1;
		_LIT(KOOM, "Out of Memory");
		
		dbus_error_init(&error);
		
		connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
		if(!CheckConnection(connection, &error))
			return 1;
		
		if(!dbus_connection_allocate_data_slot(&data_slot))
		{
			iLog->Log(KOOM);
			return 1;
		}
		
		dbus_connection_free_data_slot(&data_slot);

		dbus_connection_close(connection);
		dbus_connection_unref(connection);
		dbus_shutdown();
		
		iLog->Log(KSuccess);
		return KErrNone;
	}
	
	
	
TInt Cstif_2::DBus_Connection_Data_Slot1( CStifItemParser& aItem )  
	{
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  
	
		DBusError error;
		DBusConnection* connection;
		dbus_int32_t data_slot = -1;
		TestData data;
		TestData* data1;
		DBusFreeFunction free_data;
		_LIT(KOOM, "Out of Memory");
		_LIT(KDataFail, "Fail to retrieve data");
		_LIT(KData, "Data :: \n\tInteger value(20) = %d\n\tString value(CheckDBus) = %s");
		
		dbus_error_init(&error);
		
		connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
		if(!CheckConnection(connection, &error))
			return 1;
		
		if(!dbus_connection_allocate_data_slot(&data_slot))
		{
			iLog->Log(KOOM);
			return 1;
		}
		
		data.num = 20;
		data.str = (char*)malloc(data.num);
		free_data = Free_Test_Data;
		data_free_flag = FALSE;
		strcpy(data.str, "CheckDBus");
		
		if(!dbus_connection_set_data(connection, data_slot, (void*)&data, free_data))
		{
			iLog->Log(KOOM);
			return 1;
		}
		
		data1 = (TestData*)dbus_connection_get_data(connection, data_slot);
		if(data1->num != data.num || strcmp(data1->str,data.str))
		{
			iLog->Log(KDataFail);
			return 1;
		}
		
		dbus_connection_free_data_slot(&data_slot);
			
		iLog->Log(_L8("Data :: \n\tInteger value(20) = %d\n\tString value(CheckDBus) = %s"), data1->num, data1->str);
		dbus_connection_close(connection);
		dbus_connection_unref(connection);
		dbus_shutdown();
		if(!data_free_flag)
		{
			iLog->Log(_L8("free_data function is not called"));
			return 1;
		} 
		iLog->Log(KSuccess);
		return KErrNone;
	}
	
	
TInt Cstif_2::DBus_Connection_Data_Slot2( CStifItemParser& aItem )  
	{
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  
	
		DBusError error;
		DBusConnection* connection;
		dbus_int32_t data_slot = -1;
		TestData data;
		TestData data2;
		TestData* data1;
		DBusFreeFunction free_data;
		_LIT(KOOM, "Out of Memory");
		_LIT(KDataFail, "Fail to retrieve data");
		_LIT(KData, "Data :: \n\tInteger value(20) = %d\n\tString value(CheckDBus) = %s");
		_LIT(KFreeFail, "Free Function is not called.");
		
		dbus_error_init(&error);
		
		connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
		if(!CheckConnection(connection, &error))
			return 1;
		
		if(!dbus_connection_allocate_data_slot(&data_slot))
		{
			iLog->Log(KOOM);
			return 1;
		}
		
		data.num = 20;
		data.str = (char*)malloc(data.num);
		data_free_flag = FALSE;
		strcpy(data.str, "CheckDBus");
		free_data = Free_Test_Data;
		
		if(!dbus_connection_set_data(connection, data_slot, (void*)&data, free_data))
		{
			iLog->Log(KOOM);
			return 1;
		}
		
		data1 = (TestData*)dbus_connection_get_data(connection, data_slot);
		if(data1->num != data.num || strcmp(data1->str,data.str))
			{
				iLog->Log(KDataFail);
				return 1;
			}
		
		iLog->Log(_L8 ("Data :: \n\tInteger value(20) = %d\n\tString value(CheckDBus) = %s"), data1->num, data1->str);
		
		data2.num = 30;
		data2.str = (char*)malloc(data2.num);
		data_free_flag = FALSE;
		strcpy(data.str, "CheckDBusFree");
		free_data = Free_Test_Data;
		
		if(!dbus_connection_set_data(connection, data_slot, (void*)&data2, free_data))
		{
			iLog->Log(KOOM);
			return 1;
		}
		
		if(!data_free_flag)
		{
			iLog->Log(KFreeFail);
			return 1;
		}
				
		dbus_connection_free_data_slot(&data_slot);
		
		dbus_connection_close(connection);
		dbus_connection_unref(connection);
		dbus_shutdown();
		
		iLog->Log(KSuccess);
		return KErrNone;
	}
    
    
TInt Cstif_2::DBus_Message_Path0( CStifItemParser& aItem )
{
	DBusMessage* msg;
	char* path = "/1type/Message2/Call4";
	
	msg = dbus_message_new(DBUS_MESSAGE_TYPE_METHOD_CALL);
	
	if(!msg)
	{
		iLog->Log(_L8("Fail to create Message"));
		return 1;
	}
	
	if(!dbus_message_set_path(msg, path))
	{
		iLog->Log(_L8("Not enough Memory."));
		return 1;
	}
	
	if(!dbus_message_has_path(msg, path))
	{
		iLog->Log(_L8("Path is not correct"));
		return 1;
	}
	
	iLog->Log(KSuccess);
	return KErrNone;
}


TInt Cstif_2::DBus_Message_Path1( CStifItemParser& aItem )
{
	DBusMessage* msg;
	char* path = "/type/Message/Call";
	
	msg = dbus_message_new(DBUS_MESSAGE_TYPE_METHOD_CALL);
	
	if(!msg)
	{
		iLog->Log(_L8("Fail to create Message"));
		return 1;
	}
	
	if(!dbus_message_set_path(msg, path))
	{
		iLog->Log(_L8("Not enough Memory."));
		return 1;
	}
	
	if(strcmp(path, dbus_message_get_path(msg)))
	{
		iLog->Log(_L8("Path is not correct"));
		return 1;
	}
	
	iLog->Log(KSuccess);
	return KErrNone;
}


TInt Cstif_2::DBus_Message_Path2( CStifItemParser& aItem )
{
	DBusMessage* msg;
	char* path = "/type/Message/Call";
	
	msg = dbus_message_new(DBUS_MESSAGE_TYPE_METHOD_CALL);
	
	if(!msg)
	{
		iLog->Log(_L8("Fail to create Message"));
		return 1;
	}
	
	if(!dbus_message_set_path(msg, path))
	{
		iLog->Log(_L8("Not enough Memory."));
		return 1;
	}
	
	fprintf(stdout, dbus_message_get_path(msg));
	
	if(!dbus_message_set_path(msg, NULL))
	{
		iLog->Log(_L8("Not enough Memory."));
		return 1;
	}
	
	if(dbus_message_get_path(msg))
	{
		iLog->Log(_L8("Path is not reset."));
		return 1;
	}
	
	iLog->Log(KSuccess);
	return KErrNone;
}


TInt Cstif_2::DBus_Message_Path3( CStifItemParser& aItem )
{
	DBusMessage* msg;
	char* path = "/";
	
	msg = dbus_message_new(DBUS_MESSAGE_TYPE_METHOD_CALL);
	
	if(!msg)
	{
		iLog->Log(_L8("Fail to create Message"));
		return 1;
	}
	
	if(!dbus_message_set_path(msg, path))
	{
		iLog->Log(_L8("Not enough Memory."));
		return 1;
	}
	
	if(strcmp(path, dbus_message_get_path(msg)))
	{
		iLog->Log(_L8("Path is not correct"));
		return 1;
	}
	
	iLog->Log(KSuccess);
	return KErrNone;
}


    
TInt Cstif_2::DBus_Message_Type0( CStifItemParser& aItem )
{
	DBusMessage* msg;
	
	msg = dbus_message_new(DBUS_MESSAGE_TYPE_METHOD_CALL);
	
	if(!msg)
	{
		iLog->Log(_L8("Fail to create Message"));
		return 1;
	}
	
	if(DBUS_MESSAGE_TYPE_METHOD_CALL != dbus_message_get_type(msg))
	{
		iLog->Log(_L8("Mismatch in Message type."));
		return 1;
	}
	
	iLog->Log(KSuccess);
	return KErrNone;
}


 
TInt Cstif_2::DBus_Message_Type1( CStifItemParser& aItem )
{
	DBusMessage* msg;
	
	msg = dbus_message_new(DBUS_MESSAGE_TYPE_SIGNAL);
	
	if(!msg)
	{
		iLog->Log(_L8("Fail to create Message"));
		return 1;
	}
	
	if(DBUS_MESSAGE_TYPE_SIGNAL != dbus_message_get_type(msg))
	{
		iLog->Log(_L8("Mismatch in Message type."));
		return 1;
	}
	
	iLog->Log(KSuccess);
	return KErrNone;
}


TInt Cstif_2::DBus_Message_Type2( CStifItemParser& aItem )
{
	DBusMessage* msg;
	
	msg = dbus_message_new(DBUS_MESSAGE_TYPE_METHOD_RETURN);
	
	if(!msg)
	{
		iLog->Log(_L8("Fail to create Message"));
		return 1;
	}
	
	if(DBUS_MESSAGE_TYPE_METHOD_RETURN != dbus_message_get_type(msg))
	{
		iLog->Log(_L8("Mismatch in Message type."));
		return 1;
	}
	
	iLog->Log(KSuccess);
	return KErrNone;
}


TInt Cstif_2::DBus_Message_Type3( CStifItemParser& aItem )
{
	DBusMessage* msg;
	
	msg = dbus_message_new(DBUS_MESSAGE_TYPE_ERROR);
	
	if(!msg)
	{
		iLog->Log(_L8("Fail to create Message"));
		return 1;
	}
	
	if(DBUS_MESSAGE_TYPE_ERROR != dbus_message_get_type(msg))
	{
		iLog->Log(_L8("Mismatch in Message type."));
		return 1;
	}
	
	iLog->Log(KSuccess);
	return KErrNone;
}

TInt Cstif_2::DBus_Message_Data_Slot0( CStifItemParser& aItem )
{
	DBusMessage* msg;
	dbus_int32_t data_slot = -1;
	TestData data;
	TestData* data1;
	DBusFreeFunction free_data;
	_LIT(KOOM, "Out of Memory");
	_LIT(KDataFail, "Fail to retrieve data");
	
	msg = dbus_message_new(DBUS_MESSAGE_TYPE_METHOD_CALL);
	
	if(!dbus_message_allocate_data_slot(&data_slot))
	{
		iLog->Log(_L8("Fail to allocate data slot"));
		return 1;
	} 
	
	data.num = 20;
	data.str = (char*)malloc(data.num);
	free_data = Free_Test_Data;
	data_free_flag = FALSE;
	strcpy(data.str, "CheckDBus");
	
	if(!dbus_message_set_data(msg, data_slot, (void*)&data, free_data))
	{
		iLog->Log(KOOM);
		return 1; 
	}
	
	data1 = (TestData*)dbus_message_get_data(msg, data_slot);
	if(data1->num != data.num || strcmp(data1->str,data.str))
	{
		iLog->Log(KDataFail);
		return 1;
	}
	
	dbus_message_free_data_slot(&data_slot);
		
	iLog->Log(_L8("Data :: \n\tInteger value(20) = %d\n\tString value(CheckDBus) = %s"), data1->num, data1->str);
	dbus_message_unref(msg);
	
	if(!data_free_flag)
	{
		iLog->Log(_L8("free_data function is not called"));
		return 1;
	} 
	iLog->Log(KSuccess);
	return KErrNone;
}


TInt Cstif_2::DBus_Message_Data_Slot1( CStifItemParser& aItem )
{
	DBusMessage* msg;
	dbus_int32_t data_slot = -1;
	TestData data;
	TestData* data1;
	DBusFreeFunction free_data;
	_LIT(KOOM, "Out of Memory");
	_LIT(KFreeFail, "Free Function is not called.");
	_LIT(KDataFail, "Fail to retrieve data");	
	
	msg = dbus_message_new(DBUS_MESSAGE_TYPE_METHOD_CALL);
	if(!dbus_message_allocate_data_slot(&data_slot))
	{
		iLog->Log(KOOM);
		return 1;
	}
	
	data.num = 20;
	data.str = (char*)malloc(data.num);
	data_free_flag = FALSE;
	strcpy(data.str, "CheckDBus");
	free_data = Free_Test_Data;
	
	if(!dbus_message_set_data(msg, data_slot, (void*)&data, free_data))
	{
		iLog->Log(KOOM);
		return 1;
	}
	
	data1 = (TestData*)dbus_message_get_data(msg, data_slot);
	if(!data1)
	{
		iLog->Log(KDataFail);
		return 1;
	} 
	
	iLog->Log(_L8 ("Data :: \n\tInteger value(20) = %d\n\tString value(CheckDBus) = %s"), data1->num, data1->str);
	
	data.num = 30;
	data.str = (char*)malloc(data.num);
	data_free_flag = FALSE;
	strcpy(data.str, "CheckDBusFree");
	free_data = Free_Test_Data;
	
	if(!dbus_message_set_data(msg, data_slot, (void*)&data, free_data))
	{
		iLog->Log(KOOM);
		return 1;
	}
	
	if(!data_free_flag)
	{
		iLog->Log(KFreeFail);
		return 1;
	}
			
	dbus_message_free_data_slot(&data_slot);
	
	iLog->Log(KSuccess);
	return KErrNone;
}


TInt Cstif_2::DBus_Bus_Request_Name0( CStifItemParser& aItem )
{
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  

	DBusConnection* connection;
	DBusError error;
	DBusError error1;
	int result;

	FILE* fp;
	char buf[20];
	
	dbus_error_init(&error);
	dbus_error_init(&error1);
	connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
	if(!CheckConnection(connection, &error))
	{
		iLog->Log(KConnectionFail, error.name);
		iLog->Log(KFail);
		return 1;
	}
	
	result = dbus_bus_request_name(connection, "Test.Method.Call", DBUS_NAME_FLAG_ALLOW_REPLACEMENT, &error1);
	if(result == -1)
		iLog->Log(_L8("Error Set :: %s\n%s"), error1.name,error1.message);
	if(result != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER )
	{
		iLog->Log(KFail);
		return 1;
	}
		
	result = dbus_bus_request_name(connection, "Test.Method.Call2", DBUS_NAME_FLAG_REPLACE_EXISTING, &error1);
	if(result == -1)
		iLog->Log(_L8("Error Set :: %s\n%s"), error1.name,error1.message);
	if(result != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER )
	{
		iLog->Log(KFail);
		return 1;
	}
	
	result = dbus_bus_request_name(connection, "Test.Method.Call2", DBUS_NAME_FLAG_REPLACE_EXISTING, &error1);
	if(result == -1)
		iLog->Log(_L8("Error Set :: %s\n%s"), error1.name,error1.message);
	if(result != DBUS_REQUEST_NAME_REPLY_ALREADY_OWNER )
	{
		iLog->Log(KFail);
		return 1;
	}
	
	//for FIFO
	const char* fifopath = "C:\\mkfifo2.file";
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
	fp = popen("z:\\sys\\bin\\request_name.exe","r");
#else
	fp = popen("c:\\sys\\bin\\request_name.exe","r");
#endif
	if(!fp)
		{
		iLog->Log(_L8("fp is NULL."));
		return 1;
		}	

	int fd = open(fifopath, O_RDONLY);
	if(fd > 0)
	{
		err = read (fd, buf, 20); 
		if(!strcmp("Error Occured", buf))
			{
			iLog->Log(_L8("Error while requesting name in request_name.exe."));
			return 1;
			}
		close(fd);
	}
	else
		{
		iLog->Log(_L8("Error in FIFO open()."));
		return 1;
		}
	unlink(fifopath);
	if(strcmp("done", buf))
		return 1;
	
	result = dbus_bus_request_name(connection, "Test.Method.Call3", DBUS_NAME_FLAG_DO_NOT_QUEUE , &error1);
	if(result == -1)
		iLog->Log(_L8("Error Set :: %s\n%s"), error1.name,error1.message);
	if(result != DBUS_REQUEST_NAME_REPLY_EXISTS )
	{
		iLog->Log(KFail);
		return 1;
	}	
	
	dbus_connection_close(connection);
	dbus_connection_unref(connection);
	dbus_shutdown();
	
	iLog->Log(KSuccess);
	return KErrNone;
}


TInt Cstif_2::DBus_Bus_Name_Has_Owner0( CStifItemParser& aItem )
{
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  

	DBusConnection* connection;
	DBusError error;
	
	dbus_error_init(&error);
	connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
	if(!CheckConnection(connection, &error))
		return 1;
	
	if(dbus_bus_request_name(connection, "Test.Request.Name", DBUS_NAME_FLAG_ALLOW_REPLACEMENT, &error) == -1)
	{
		iLog->Log(_L8("Error :: %s\nMessage :: %s"), error.name, error.message);
		return 1;
	}
	
	if(!dbus_bus_name_has_owner(connection, "Test.Request.Name", &error))
	{
		iLog->Log(_L8("Fail to check owner after requesting name."));
		return 1;
	}
	
	if(dbus_bus_release_name(connection, "Test.Request.Name", &error) == -1)
	{
		iLog->Log(_L8("Error :: %s\nMessage :: %s"), error.name, error.message);
		return 1;
	}
	
	if(dbus_bus_name_has_owner(connection, "Test.Request.Name", &error))
	{
		iLog->Log(_L8("owner is there after releasing name."));
		return 1;
	}
	
	dbus_connection_close(connection);
	dbus_connection_unref(connection);
	dbus_shutdown();
	
	iLog->Log(KSuccess);
	return KErrNone;
}


TInt Cstif_2::DBus_Message_Copy0( CStifItemParser& aItem )
{
	DBusMessage* src_msg;
	DBusMessage* dest_msg = NULL;
	_LIT(KOOM, "Not enough memory.");
	
	src_msg = dbus_message_new(DBUS_MESSAGE_TYPE_METHOD_CALL);
	
	if(!dbus_message_set_interface(src_msg, "Test.Message.Copy"))
	{
		iLog->Log(KOOM);
		return 1;
	}
	
	if(!dbus_message_set_path(src_msg, "/Test/Message/Copy0"))
	{
		iLog->Log(KOOM);
		return 1;
	}
	
	if(!dbus_message_set_member(src_msg, "Member"))
	{
		iLog->Log(KOOM);
		return 1;
	}
	
	dest_msg = dbus_message_copy(src_msg);
	
	if(!dest_msg)
	{
		iLog->Log(KOOM);
		return 1;
	}
	
	if(!TestMessageEqual(src_msg, dest_msg))
	{
		iLog->Log(_L8("Messages are different."));
		return 1;
	}
	
	
	iLog->Log(KSuccess);
	return KErrNone;
}


TInt Cstif_2::DBus_Message_Type_To_String0( CStifItemParser& aItem )
{
	if(strcmp("method_call", dbus_message_type_to_string(DBUS_MESSAGE_TYPE_METHOD_CALL)))
	{
		iLog->Log(_L8("dbus_message_type_to_string() failed for DBUS_MESSAGE_TYPE_METHOD_CALL."));
		return 1;
	}
	
	if(strcmp("method_return", dbus_message_type_to_string(DBUS_MESSAGE_TYPE_METHOD_RETURN)))
	{
		iLog->Log(_L8("dbus_message_type_to_string() failed for DBUS_MESSAGE_TYPE_METHOD_RETURN."));
		return 1;
	}
	
	if(strcmp("signal", dbus_message_type_to_string(DBUS_MESSAGE_TYPE_SIGNAL)))
	{
		iLog->Log(_L8("dbus_message_type_to_string() failed for DBUS_MESSAGE_TYPE_SIGNAL."));
		return 1;
	}
	
	if(strcmp("error", dbus_message_type_to_string(DBUS_MESSAGE_TYPE_ERROR)))
	{
		iLog->Log(_L8("dbus_message_type_to_string() failed for DBUS_MESSAGE_TYPE_ERROR."));
		return 1;
	}
	
	if(strcmp("invalid", dbus_message_type_to_string(123)))
	{
		iLog->Log(_L8("dbus_message_type_to_string() failed for DBUS_MESSAGE_TYPE_INVALID."));
		return 1;
	}
	
	iLog->Log(KSuccess);
	return KErrNone;
}


TInt Cstif_2::DBus_Message_Type_From_String0( CStifItemParser& aItem )
{
	if(DBUS_MESSAGE_TYPE_METHOD_CALL != dbus_message_type_from_string("method_call"))
	{
		iLog->Log(_L8("dbus_message_type_from_string() failed for DBUS_MESSAGE_TYPE_METHOD_CALL."));
		return 1;
	}
	
	if(DBUS_MESSAGE_TYPE_METHOD_RETURN != dbus_message_type_from_string("method_return"))
	{
		iLog->Log(_L8("dbus_message_type_from_string() failed for DBUS_MESSAGE_TYPE_METHOD_RETURN."));
		return 1;
	}
	
	if(DBUS_MESSAGE_TYPE_SIGNAL != dbus_message_type_from_string("signal"))
	{
		iLog->Log(_L8("dbus_message_type_from_string() failed for DBUS_MESSAGE_TYPE_SIGNAL."));
		return 1;
	}
	
	if(DBUS_MESSAGE_TYPE_ERROR != dbus_message_type_from_string("error"))
	{
		iLog->Log(_L8("dbus_message_type_from_string() failed for DBUS_MESSAGE_TYPE_ERROR."));
		return 1;
	}
	
	if(DBUS_MESSAGE_TYPE_INVALID != dbus_message_type_from_string("invalid"))
	{
		iLog->Log(_L8("dbus_message_type_from_string() failed for DBUS_MESSAGE_TYPE_INVALID."));
		return 1;
	}
	
	iLog->Log(KSuccess);
	return KErrNone;
}


TInt Cstif_2::DBus_Message_Interface0( CStifItemParser& aItem )
{
	DBusMessage* msg;
	_LIT(KOOM, "Not enough memory.");
	char* iface = "Test.Message.Iface";
	
	msg = dbus_message_new(DBUS_MESSAGE_TYPE_METHOD_CALL);
	
	if(!msg)
	{
		iLog->Log(KOOM);
		return 1;
	}
	
	if(!dbus_message_set_interface(msg, iface))
	{
		iLog->Log(KOOM);
		return 1;
	}
	
	if(strcmp(iface, dbus_message_get_interface(msg)))
	{
		iLog->Log(KOOM);
		return 1;
	}
	
	iLog->Log(KSuccess);
	return KErrNone;
}


TInt Cstif_2::DBus_Message_Interface1( CStifItemParser& aItem )
{
	DBusMessage* msg;
	_LIT(KOOM, "Not enough memory.");
	char* iface = "Test.Message.Iface";
	
	msg = dbus_message_new(DBUS_MESSAGE_TYPE_SIGNAL);
	
	if(!dbus_message_set_interface(msg, iface))
	{
		iLog->Log(KOOM);
		return 1;
	}
	
	if(strcmp(iface, dbus_message_get_interface(msg)))
	{
		iLog->Log(KOOM);
		return 1;
	}
	
	iLog->Log(KSuccess);
	return KErrNone;
}


TInt Cstif_2::DBus_Message_Error_Name0( CStifItemParser& aItem )
{
	DBusMessage* msg;
	_LIT(KOOM, "Not Enough Memory");
	char* err = "Testing.Error.Name.Setting.API";
	
	msg = dbus_message_new(DBUS_MESSAGE_TYPE_ERROR);
	 
	if(!msg)
	{
		iLog->Log(KOOM);
		return 1;
	}
	
	if(!dbus_message_set_error_name(msg, err))
	{
		iLog->Log(KOOM);
		return 1;
	}
	
	if(strcmp(err, dbus_message_get_error_name(msg)))
	{
		iLog->Log(_L8("Error Name not set Properly."));
		return 1;
	}
	
	iLog->Log(KSuccess);
	return KErrNone;
}


TInt Cstif_2::DBus_Type_Basic0( CStifItemParser& aItem )
{

	if(!dbus_type_is_basic(DBUS_TYPE_BOOLEAN))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_BOOLEAN");
		return 1;
	}
	if(!dbus_type_is_basic(DBUS_TYPE_INT16))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_INT16");
		return 1;
	}
	if(!dbus_type_is_basic(DBUS_TYPE_BYTE))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_BYTE");
		return 1;
	}
	if(!dbus_type_is_basic(DBUS_TYPE_UINT16))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_UINT16");
		return 1;
	}
	if(!dbus_type_is_basic(DBUS_TYPE_INT32))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_INT32");
		return 1;
	}
	if(!dbus_type_is_basic(DBUS_TYPE_UINT32))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_UINT32");
		return 1;
	}
	if(!dbus_type_is_basic(DBUS_TYPE_INT64))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_INT64");
		return 1;
	}
	if(!dbus_type_is_basic(DBUS_TYPE_UINT64))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_UINT64");
		return 1;
	}
	if(!dbus_type_is_basic(DBUS_TYPE_DOUBLE))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_DOUBLE");
		return 1;
	}
	if(!dbus_type_is_basic(DBUS_TYPE_STRING))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_STRING");
		return 1;
	}
	if(!dbus_type_is_basic(DBUS_TYPE_OBJECT_PATH))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_OBJECT_PATH");
		return 1;
	}
	if(!dbus_type_is_basic(DBUS_TYPE_SIGNATURE))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_SIGNATURE");
		return 1;
	}
	if(dbus_type_is_basic(DBUS_TYPE_ARRAY))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_ARRAY");
		return 1;
	}
	if(dbus_type_is_basic(DBUS_TYPE_VARIANT))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_VARIANT");
		return 1;
	}
	if(dbus_type_is_basic(DBUS_TYPE_STRUCT))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_STRUCT");
		return 1;
	}
	if(dbus_type_is_basic(DBUS_TYPE_DICT_ENTRY))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_DICT_ENTRY");
		return 1;
	} 
	
	iLog->Log(KSuccess);
	return KErrNone;
}


TInt Cstif_2::DBus_Type_Container0( CStifItemParser& aItem )
{

	if(dbus_type_is_container(DBUS_TYPE_BOOLEAN))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_BOOLEAN");
		return 1;
	}
	if(dbus_type_is_container(DBUS_TYPE_INT16))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_INT16");
		return 1;
	}
	if(dbus_type_is_container(DBUS_TYPE_BYTE))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_BYTE");
		return 1;
	}
	if(dbus_type_is_container(DBUS_TYPE_UINT16))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_UINT16");
		return 1;
	}
	if(dbus_type_is_container(DBUS_TYPE_INT32))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_INT32");
		return 1;
	}
	if(dbus_type_is_container(DBUS_TYPE_UINT32))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_UINT32");
		return 1;
	}
	if(dbus_type_is_container(DBUS_TYPE_INT64))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_INT64");
		return 1;
	}
	if(dbus_type_is_container(DBUS_TYPE_UINT64))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_UINT64");
		return 1;
	}
	if(dbus_type_is_container(DBUS_TYPE_DOUBLE))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_DOUBLE");
		return 1;
	}
	if(dbus_type_is_container(DBUS_TYPE_STRING))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_STRING");
		return 1;
	}
	if(dbus_type_is_container(DBUS_TYPE_OBJECT_PATH))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_OBJECT_PATH");
		return 1;
	}
	if(dbus_type_is_container(DBUS_TYPE_SIGNATURE))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_SIGNATURE");
		return 1;
	}
	if(!dbus_type_is_container(DBUS_TYPE_ARRAY))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_ARRAY");
		return 1;
	}
	if(!dbus_type_is_container(DBUS_TYPE_VARIANT))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_VARIANT");
		return 1;
	}
	if(!dbus_type_is_container(DBUS_TYPE_STRUCT))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_STRUCT");
		return 1;
	}
	if(!dbus_type_is_container(DBUS_TYPE_DICT_ENTRY))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_DICT_ENTRY");
		return 1;
	}
	
	iLog->Log(KSuccess);
	return KErrNone;
}

TInt Cstif_2::DBus_Type_Fixed0( CStifItemParser& aItem )
{

	if(!dbus_type_is_fixed(DBUS_TYPE_BOOLEAN))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_BOOLEAN");
		return 1;
	}
	if(!dbus_type_is_fixed(DBUS_TYPE_INT16))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_INT16");
		return 1;
	}
	if(!dbus_type_is_fixed(DBUS_TYPE_BYTE))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_BYTE");
		return 1;
	}
	if(!dbus_type_is_fixed(DBUS_TYPE_UINT16))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_UINT16");
		return 1;
	}
	if(!dbus_type_is_fixed(DBUS_TYPE_INT32))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_INT32");
		return 1;
	}
	if(!dbus_type_is_fixed(DBUS_TYPE_UINT32))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_UINT32");
		return 1;
	}
	if(!dbus_type_is_fixed(DBUS_TYPE_INT64))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_INT64");
		return 1;
	}
	if(!dbus_type_is_fixed(DBUS_TYPE_UINT64))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_UINT64");
		return 1;
	}
	if(!dbus_type_is_fixed(DBUS_TYPE_DOUBLE))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_DOUBLE");
		return 1;
	}
	if(dbus_type_is_fixed(DBUS_TYPE_STRING))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_STRING");
		return 1;
	}
	if(dbus_type_is_fixed(DBUS_TYPE_OBJECT_PATH))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_OBJECT_PATH");
		return 1;
	}
	if(dbus_type_is_fixed(DBUS_TYPE_SIGNATURE))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_SIGNATURE");
		return 1;
	}
	if(dbus_type_is_fixed(DBUS_TYPE_ARRAY))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_ARRAY");
		return 1;
	}
	if(dbus_type_is_fixed(DBUS_TYPE_VARIANT))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_VARIANT");
		return 1;
	}
	if(dbus_type_is_fixed(DBUS_TYPE_STRUCT))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_STRUCT");
		return 1;
	}
	if(dbus_type_is_fixed(DBUS_TYPE_DICT_ENTRY))	
	{
		iLog->Log(_L8("Fail for %s"), "DBUS_TYPE_DICT_ENTRY");
		return 1;
	}
	
	iLog->Log(KSuccess);
	return KErrNone;
}
   
 
TInt Cstif_2::DBus_Message_Is_Error0( CStifItemParser& aItem )
{
	DBusMessage* msg;
	_LIT(KOOM, "Not enough memory");
	char* err = "Testing.Is.Error.Setting.API";
	
	msg = dbus_message_new(DBUS_MESSAGE_TYPE_ERROR);
	
	if(!msg)
	{
		iLog->Log(KOOM);
		return 1;
	}
	
	iLog->Log(_L8("Error created."));
	
	if(!dbus_message_set_error_name(msg, err))
	{
		iLog->Log(KOOM);
		return 1;
	}
	
	iLog->Log(_L8("Error name set."));
	
	if(!dbus_message_is_error(msg, err))
	{
		iLog->Log(_L8("Correct Error Name is not matching"));
		return 1;
	}
	
	iLog->Log(KSuccess);
	return KErrNone;
}

TInt Cstif_2::DBus_Message_Iter0( CStifItemParser& aItem )
{
	DBusMessage* msg;
	DBusMessageIter iter_append;
	DBusMessageIter iter_read;
	DBusMessageIter sub_iter;
	dbus_uint32_t num = 22;
	dbus_uint32_t num_r = 22;
	const dbus_int32_t arr[] = {0,1,2,3,4,5};
	dbus_int32_t* arr_r;
	int arr_len;
	int arr_elements;
	const dbus_int32_t* array = arr;
	int type;
	int i;
	_LIT(KOOM, "Out of Memory");
	
	msg = dbus_message_new(DBUS_MESSAGE_TYPE_METHOD_CALL);
	if(!msg)
		{
		iLog->Log(KOOM);
		return 1;
		}
	
	dbus_message_iter_init_append(msg, &iter_append);
	
	if(!dbus_message_iter_append_basic(&iter_append, DBUS_TYPE_UINT32, &num))
		{
		iLog->Log(KOOM);
		return 1;
		}
	
	if(!dbus_message_iter_open_container(&iter_append, DBUS_TYPE_ARRAY, DBUS_TYPE_INT32_AS_STRING, &sub_iter))
		{
		iLog->Log(KOOM);
		return 1;
		}
	if(!dbus_message_iter_append_fixed_array(&sub_iter, DBUS_TYPE_INT32, &array, 6))
		{
		iLog->Log(KOOM);
		return 1;
		}
	
	dbus_message_iter_close_container(&iter_append, &sub_iter);
	if(!dbus_message_iter_init(msg, &iter_read))
		{
		iLog->Log(KOOM);
		return 1;
		}

	while((type = dbus_message_iter_get_arg_type(&iter_read)) != DBUS_TYPE_INVALID)
		{
		if(type == DBUS_TYPE_UINT32)
			{
			dbus_message_iter_get_basic(&iter_read, &num_r);
			if(num_r != 22)
				{
				iLog->Log(_L8("Argument mismatch for UINT32"));
				return 1;
				}
			}  
		if(type == DBUS_TYPE_ARRAY)
			{
			dbus_message_iter_recurse(&iter_read, &sub_iter);
			arr_len = dbus_message_iter_get_array_len(&sub_iter);
			iLog->Log(_L8("No. of bytes in array :: %d"), arr_len);
			dbus_message_iter_get_fixed_array(&sub_iter, &arr_r, &arr_elements);
			
			for(i=0; i<arr_elements; i++)
				{
				if(i != arr_r[i])
					{
					iLog->Log(_L8("Array Elements are not matching."));
					return 1;
					}
				iLog->Log(_L8("%d"),arr_r[i]);
				}
			}
		dbus_message_iter_next(&iter_read);
		}

	iLog->Log(KSuccess);
	return KErrNone;
}
 

TInt Cstif_2::DBus_Bus_Start_Service_By_Name0( CStifItemParser& aItem )
{
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  

	DBusError error;
	DBusConnection* connection;
	dbus_uint32_t result;
	
	dbus_error_init(&error);
	
	connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
	iLog->Log(_L8("conn created."));
	if(!CheckConnection(connection, &error))
		return 1;
	
	if(!dbus_bus_start_service_by_name(connection, "Test.Method.Call", 0, &result, &error))
	{
		iLog->Log(_L8("Failed to get connection with eroor :: %s\n%s"), error.name, error.message);
		
		dbus_error_free(&error);
		return 1;
	}
	
	if(result == DBUS_START_REPLY_SUCCESS)
		iLog->Log(_L8("Service Started with result DBUS_START_REPLY_SUCCESS"));
	else
		if(result == DBUS_START_REPLY_ALREADY_RUNNING)
			iLog->Log(_L8("Service Started with result DBUS_START_REPLY_ALREADY_RUNNING"));
		else
			iLog->Log(_L8("Service Started with result unknown"));
	
	dbus_connection_close(connection);
	dbus_connection_unref(connection);
	dbus_shutdown();
	
	iLog->Log(KSuccess);
	return KErrNone; 
}

   
TInt Cstif_2::DBus_Server_Listen0( CStifItemParser& aItem )
{
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  

	DBusServer* server;
	DBusConnection* connection;
	DBusError error;
	char* address;
		
	dbus_error_init(&error);
	server = dbus_server_listen("tcp:host=localhost,port=8085", &error);
	
	if(!server)
	{
		iLog->Log(_L8("Error :: %s\n%s"), error.name, error.message);
		iLog->Log(KFail);
		return 1;
	}
	
	address = dbus_server_get_address(server);
	iLog->Log(_L8("Address :: %s"), address);
	
	connection = dbus_connection_open_private("tcp:host=localhost,port=8085", &error);
	if(!CheckConnection(connection, &error))
		return 1;
	

	dbus_connection_close(connection);
	dbus_connection_unref(connection);
	dbus_shutdown();
	
	dbus_server_disconnect(server);	
	
	iLog->Log(KSuccess);
	return KErrNone;
}


TInt Cstif_2::DBus_Server_Data_Slot0( CStifItemParser& aItem )
{
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  

	DBusServer* server;
	DBusError error;
	int data_slot=-1;
	TestData data;
	TestData* data_r;
	DBusFreeFunction free_data;
	_LIT(KOOM, "Out Of Memory.");
		 
	dbus_error_init(&error);
	server = dbus_server_listen("tcp:host=localhost,port=8086", &error);
	
	if(!server)
	{
		iLog->Log(_L8("Error :: %s\n%s"), error.name, error.message);
		iLog->Log(KFail);
		return 1;
	}
	
	if(!dbus_server_allocate_data_slot(&data_slot))
		{
		iLog->Log(KOOM);
		dbus_server_disconnect(server);
		return 1;
		}
	
	data.num = 60;
	data.str = (char *)malloc(data.num);
	data_free_flag = FALSE;
	strcpy(data.str, "Test Server Data Slot.");
	free_data = Free_Test_Data;
	if(!dbus_server_set_data(server, data_slot, &data, free_data))
		{
		iLog->Log(KOOM);
		dbus_server_disconnect(server);
		return 1;
		}
	
	if((data_r = (TestData*)dbus_server_get_data(server, data_slot)) == NULL)
		{
		iLog->Log(_L8("Failed to get data."));
		dbus_server_disconnect(server);
		return 1;
		}
	
	if(data_r->num != data.num || strcmp(data.str, data_r->str))
		{
		iLog->Log(_L8("Wrong Data received."));
		dbus_server_disconnect(server);
		return 1;
		}
	
	iLog->Log(_L8("int :: %d\n string :: %s"), data_r->num, data_r->str);
	
	if(!dbus_server_set_data(server, data_slot, &data, free_data))
		{
		iLog->Log(KOOM);
		dbus_server_disconnect(server);
		return 1;
		}
	
	if(!data_free_flag)
		{
		iLog->Log(_L8("Data is not freed."));
		dbus_server_disconnect(server);
		return 1;
		}
	
	dbus_server_free_data_slot(&data_slot);
	dbus_server_disconnect(server);
	
	iLog->Log(KSuccess);
	return KErrNone;
}

TInt Cstif_2::Dbus_Connection_Message_Size0( CStifItemParser& aItem )
{
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  

	long set_size = 123456;
	long get_size = 0;
	DBusConnection* connection;
	DBusError error;
	
	dbus_error_init(&error);
	
	connection = dbus_bus_get_private(DBUS_BUS_SESSION, &error);
	if(!CheckConnection(connection, &error))
		return 1;
	
	get_size = dbus_connection_get_max_message_size(connection);
	iLog->Log(_L8("Max Message size is %ld"),  get_size);
	
	dbus_connection_set_max_message_size(connection, set_size);
	get_size = dbus_connection_get_max_message_size(connection);
	if(set_size != get_size)
		{
		iLog->Log(_L8("Failed in dbus_connection_max_message_size"));
		return 1;
		}
	
	get_size = dbus_connection_get_max_received_size(connection);
	iLog->Log(_L8("Max Receiveed size is %ld"), get_size);
	
	dbus_connection_set_max_received_size(connection, set_size);
	get_size = dbus_connection_get_max_received_size(connection);	
	if(set_size != get_size)
		{
		iLog->Log(_L8("Failed in dbus_connection_max_received_size"));
		return 1;
		}

	dbus_connection_close(connection);
	dbus_connection_unref(connection);
	dbus_shutdown();
	iLog->Log(KSuccess);  
	return KErrNone;
}

TInt Cstif_2::Dbus_Get_Local_Machine_Id( CStifItemParser& aItem )
    {
    
	_LIT( KDiffConn, "Different connections not created" );
	
    DBusError error;
    DBusError error1;
    
    char* id =  NULL; 
    
    dbus_error_init (&error); 
    dbus_error_init (&error1);
    
    id=dbus_get_local_machine_id();
    if(id == NULL)
    	{
    		iLog->Log(_L8("Id == NULL "));
    		iLog->Log(_L8("test fail"));  
    		iLog->Log(KFail); 
			return 1;
    	}
    else
    	{
	    iLog->Log(_L8("id is been executed %s"),id);  
	    iLog->Log(KSuccess);  
		return KErrNone;
    	}
    }   

TInt Cstif_2::Dbus_Server_Get_Is_Connected( CStifItemParser& aItem ) 
    {
    TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
    TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  
    
	_LIT( KDiffConn, "Different connections not created" );
	
		DBusServer* server;
		DBusError error;
    DBusConnection* connection; 
    dbus_bool_t connection1;
    
    char* address;
    char* check_connection;
	
	dbus_error_init(&error);
	server = dbus_server_listen("tcp:host=localhost,port=8085", &error);
	if(!server)
	{
		iLog->Log(_L8("Error :: %s\n%s"), error.name, error.message);
		iLog->Log(KFail); 
		return 1;
	}
	
	address = dbus_server_get_address(server);
	check_connection=((address));
	
	if(check_connection == NULL)
		{
			iLog->Log(_L8("Failed:"));
			return 1;
		}
	connection = dbus_connection_open("tcp:host=localhost,port=8085", &error);
	if(!CheckConnection(connection, &error))
		return 1;  
	
	iLog->Log(_L8("Address :: %s"), address);
	iLog->Log(_L8("Check_connection :: %s"), check_connection);
	
	
	connection1=dbus_server_get_is_connected(server);
		
	if(connection1)
		iLog->Log(_L8("Server running"));
		else
			iLog->Log(_L8("No connection"));
	
	dbus_server_disconnect(server);
	connection1=dbus_server_get_is_connected(server);//
	if(connection1) {
		iLog->Log(_L8("server still running "));
		iLog->Log(_L8("test failed"));
		iLog->Log(KFail); 
		return 1;
	}
		else {
		    iLog->Log(_L8("Connection is been closed"));
			iLog->Log(_L8("No connection to the server exists")); 
			iLog->Log(KSuccess);
			return KErrNone;
			
		}
	
}

TInt Cstif_2::Dbus_Threads_Init_Default( CStifItemParser& aItem )
	{
		_LIT( KDiffConn, "Different connections not created" );
		
		DBusError error;
	    dbus_error_init(&error);
		dbus_bool_t unique_name;

		unique_name = dbus_threads_init_default () ;
		
		if(unique_name)
			{
		    iLog->Log(_L8("dbus thread is succesful"));
		    iLog->Log(KSuccess);
			return KErrNone;  
			}
		else
			{
			 iLog->Log(_L8("dbus thread is not succesful"));
			 iLog->Log(KFail); 
			 return 1;
			}
				
	}


TInt Cstif_2::Dbus_Maloc( CStifItemParser& aItem )
	{
	_LIT( KDiffConn, "Different connections not created" );
	
	int *Mem_res;
	
	int *rel_res;
   
	Mem_res = (int*)dbus_malloc(sizeof(int));
	 
	if(Mem_res== NULL)
		{
		iLog->Log(_L8("malloc unsuccessful"));
		iLog->Log(KFail); 
		return 1;
		}
	else
		{
		iLog->Log(_L8("malloc successful"));
		
		rel_res = (int*)dbus_realloc(Mem_res,10);
		if(rel_res == NULL)
			{
			iLog->Log(_L8("realloc is unsuccessful"));
			iLog->Log(KFail); 
			dbus_free(Mem_res);
			return 1;
			}
			else
				{
				iLog->Log(_L8("Realloc is successful"));
				iLog->Log(KSuccess);
				return KErrNone;  
				}
		} 
		dbus_free(rel_res);
	} 

TInt Cstif_2::Dbus_Server_Set_Auth_Mechanism( CStifItemParser& aItem ) 
    {
    TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
    TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  
    
	_LIT( KDiffConn, "Different connections not created" );
	
	DBusServer* server;
	DBusError error;
    DBusConnection* connection; 
    dbus_bool_t connection1;
    dbus_bool_t server_set;
    
    char* address;
    char* check_connection;
	
	dbus_error_init(&error);
	server = dbus_server_listen("tcp:host=localhost,port=8085", &error);
	if(!server)
	{
		iLog->Log(_L8("Error :: %s\n%s"), error.name, error.message);
		iLog->Log(KFail); 
		return 1;
	}
	
	address = dbus_server_get_address(server);
	check_connection=((address));
	
	if(check_connection == NULL)
		{
			iLog->Log(_L8("Failed:"));
			return 1;
		}
		
		connection = dbus_connection_open("tcp:host=localhost,port=8085", &error);
	
	if(!CheckConnection(connection, &error))
		return 1;  
	
	iLog->Log(_L8("Check_connection :: %s"), check_connection);
	
	
	connection1=dbus_server_get_is_connected(server);
	if(connection1)
		iLog->Log(_L8("Server running"));
		else
			iLog->Log(_L8("No connection"));
	server_set = dbus_server_set_auth_mechanisms(server,NULL);
	if(server_set == TRUE)
		{
			iLog->Log(_L8("Server authentication success"));
			iLog->Log(KSuccess);
		}
	else
			iLog->Log(_L8("No memory server authentication failed"));
	
	dbus_server_disconnect(server);
	connection1=dbus_server_get_is_connected(server);
	if(connection1) {
		iLog->Log(_L8("server still running "));
		iLog->Log(_L8("test failed")); 
		iLog->Log(KFail);   
		return 1;
	}
		else {
		    iLog->Log(_L8("Connection is been closed"));
			iLog->Log(_L8("No connection to the server exists")); 
			iLog->Log(KSuccess);
			return KErrNone;
			
		}
}

TInt Cstif_2::DBus_Message_Get_Path_Decompose0( CStifItemParser& aItem )
	{
		DBusMessage* msg;
	char* path1;
		char** path2;
		char*** path ;
		int i=0;
		
		path1 = "/type/Message/Call";
	    path =  &path2;
		
		dbus_bool_t  path_decompose;
		msg = dbus_message_new(DBUS_MESSAGE_TYPE_METHOD_CALL);
		dbus_message_set_path(msg, path1);
		
		if(!msg)
		{ 
			iLog->Log(_L8("Fail to create Message"));
			return 1; 
		}
		
		path_decompose = dbus_message_get_path_decomposed(msg,&path2);		
		if(path_decompose )
		{
		while(path2[i] != NULL)
			{
			iLog->Log(_L8("%s"),path2[i]);    
			i++;}
			iLog->Log(KSuccess); 
			return KErrNone; 
			
		} 
		
		else
			{
				iLog->Log(KFail);
				return 1;
			} 
		
	} 

