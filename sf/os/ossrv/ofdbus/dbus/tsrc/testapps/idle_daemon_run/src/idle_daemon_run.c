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
#include "test-utils.h"
#include <dbus/dbus.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main()
	{
	DBusError error;
	DBusConnection *connection;
	
	dbus_int32_t no = 5;
	DBusPendingCall* pending;
	DBusMessage* msg1;
	DBusMessage* msg;
	int i;
	
	FILE* fp;
	fp= fopen("c:\\idle_daemon_run.txt", "w+");
	if(!fp)
		printf("Failed to open idle_daemon_run.txt");
	
	
	dbus_error_init (&error);
	connection = dbus_bus_get (DBUS_BUS_SESSION, &error);
	if (connection == NULL)
	  {
	  	fprintf(fp, "*** Failed to open connection to system bus: %s\n%s",  error.name, error.message);
	  	fclose(fp);
	    dbus_error_free (&error);
	    return 1;
	  }
	
	fprintf(fp, "starting for loop\n");	
	fclose(fp);
	
		for(i=0; i<10; i++)
		{
			fp= fopen("c:\\idle_daemon_run.txt", "a+");
			if(!fp)
				printf("Failed to open idle_daemon_run.txt");
			
			if(!dbus_connection_get_is_connected(connection))
				{
				fprintf(fp, "Failed at %d th iteration\n", i);
				fclose(fp);
				return 1;
				}
			fprintf(fp, "Success in %d th iteration\n", i);
			
			msg = dbus_message_new_method_call("Test.Method.Call", "/Test/Method/Object", "test.Method.Call", "simple");
			
			dbus_message_append_args(msg, DBUS_TYPE_INT32, &no, DBUS_TYPE_INVALID);
		 
		// send message and get a handle for a reply
		   if (!dbus_connection_send_with_reply (connection, msg, &pending, -1)) { // -1 is default timeout
		   fprintf(fp, "Fail in dbus_connection_send_with_reply() API\n");
		   	fprintf(fp, "Failed at %d th iteration\n", i);
			fclose(fp);
			return 1;
		   }   
		   if (NULL == pending) {
		   fprintf(fp, "pending is NULL\n");
		   fprintf(fp, "Failed at %d th iteration\n", i);
		   fclose(fp);
		   return 1;
		   } 
		   dbus_connection_flush(connection);
		   
			// free message
		   dbus_message_unref(msg);   
		  
		   // block until we recieve a reply
		   dbus_pending_call_block(pending);
		
		   // get the reply message
		   msg1 = dbus_pending_call_steal_reply(pending);
		   if (NULL == msg1) {
		   fprintf(fp, "msg1 is NULL\n");
		   fprintf(fp, "Failed at %d th iteration\n", i);
		   fclose(fp);
		   return 1;
		   }  
		   // free the pending message handle
		   dbus_pending_call_unref(pending);
			 
		   dbus_error_init(&error);
		   dbus_message_get_args(msg1, &error, DBUS_TYPE_INT32, &no, DBUS_TYPE_INVALID);		   
		    
		   if(no != 9090)
			   {
			   fprintf(fp, "no. is not expected it is %d in %d th iteration\n", no, i);
			   fclose(fp);
			   return 1;
			   }
		   else
			   fprintf(fp, "iteration %d th method call is passing\n", i);
		   
		 	 
		   // free reply and close connection
		   dbus_message_unref(msg1); 
		   fclose(fp);
		   sleep(30*60);
		   sleep(30*60);
		   sleep(30*60);
		   sleep(30*60);

		}
		fp= fopen("c:\\idle_daemon_run.txt", "a+");
		if(!fp)
			printf("Failed to open idle_daemon_run.txt");
		
		fprintf(fp, "Test Successful\n");
		fclose(fp);
		return 0;
}
