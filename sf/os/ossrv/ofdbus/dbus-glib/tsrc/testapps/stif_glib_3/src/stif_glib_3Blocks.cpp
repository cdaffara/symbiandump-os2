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
#include "dbus_glib3_stringsend-bindings.h"
#include <unistd.h>
#include <dbus/dbus.h>
#include "stif_glib_3.h"

#define MAX_TRY 10
#define MAX_THREAD 75

_LIT(KSuccess, "Test Successful");

int handle_error(GError* error)
	{
	g_printerr ("Failed to open connection to bus: %s\n",error->message);
	g_error_free (error);
	return 1;
	}

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Cstif_glib_3::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Cstif_glib_3::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Cstif_glib_3::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Cstif_glib_3::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY("stif_g_Send_message0",Cstif_glib_3::stif_Send_message0),
        ENTRY("stif_g_Send_message_invoke0",Cstif_glib_3::stif_Send_message_invoke0),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

TInt Cstif_glib_3::stif_Send_message0( CStifItemParser& aItem )
	{
		TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
		TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  
		DBusGConnection *connection;
		GError *error;
		DBusGProxy *proxy;
		
		
		gchar* 		st = "StringPassing";
		gint		 ret = 1;
		
		g_type_init ();
		
		error = NULL;
		
		connection = dbus_g_bus_get (DBUS_BUS_SESSION,&error);
		
		if (connection == NULL)
		{
		  g_printerr ("Failed to open connection to bus: %s\n",
		              error->message);
		  g_error_free (error);
		  exit (1);
		}
		
		proxy = dbus_g_proxy_new_for_name (connection,
		                                 "com.example.DBusGlibObject2",
		                                 "/com/example/DBusGlibObject2",
		                                 "com.example.DBusGlibObject2"); 
		for(int i = 0;i<5000;i++)
	{
		if(!com_example_DBusGlibObject2_send_message0(proxy, st, &ret, &error))
			{
				iLog->Log(_L8(" %s"),error->message);
				return 1;
			}
		
		if(ret!=9090)
		{
			iLog->Log(_L8("Return value is %d"), ret);
			return 1;
		}
		iLog->Log(_L8("Count = %d"),i);
		
	}
		
		dbus_g_connection_unref(connection);
		iLog->Log(KSuccess);
		return KErrNone;
	}



TInt Cstif_glib_3::stif_Send_message_invoke0( CStifItemParser& aItem )
	{
		TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
		TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  
		FILE* fp = NULL;
		
#if defined(__WINSCW__) || defined(__WINS__)
		fp = popen("Z:\\sys\\bin\\stif_glib_1_simple_message.exe", "r");
	#else
		fp = popen("C:\\sys\\bin\\stif_glib_1_simple_message.exe", "r");
	#endif
		iLog->Log(KSuccess);
		return KErrNone;
	}
