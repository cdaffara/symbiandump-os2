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


#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define LOG_FILE "c:\\logs\\stress_test_service_activation_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__


void create_xml(int result)
	{
	if(result)
		assert_failed = 1;
	
	testResultXml("stress_test_service_activation");
    close_log_file();
	}




DBusConnection* connect_bus()
{
DBusConnection* conn;	
DBusError err;	
int ret;
dbus_error_init(&err);

 // connect to the system bus and check for errors
   conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
   if (dbus_error_is_set(&err)) {
      fprintf(stderr, "Connection Error (%s)\n", err.message);
      std_log(LOG_FILENAME_LINE, "Connection Error (%s)\n", err.message);
      dbus_error_free(&err);
      assert_failed = 1;
   }
   if (NULL == conn) {
   		create_xml(1);
      exit(1);
   }
   
   

   // request our name on the bus
   ret = dbus_bus_request_name(conn, "test.method.caller2", DBUS_NAME_FLAG_REPLACE_EXISTING , &err);
   if (dbus_error_is_set(&err)) {
      fprintf(stderr, "Name Error (%s)\n", err.message);
      std_log(LOG_FILENAME_LINE, "Name Error (%s)\n", err.message);
      dbus_error_free(&err);
      assert_failed = 1;
   }
   
   return conn;
	
}



void send_msg(DBusConnection* conn)
{
	 
   DBusMessage* msg;
   
   DBusError err,error;
   DBusPendingCall* pending;
   char *param="test hello";
   char *s; 
   int no=5;

   dbus_error_init(&err);

 	msg = dbus_message_new_method_call("Test.Method.Call", "/Test/Method/Object", "test.Method.Call", "simple");
	
	dbus_message_append_args(msg, DBUS_TYPE_INT32, &no, DBUS_TYPE_INVALID);
  
   // send message and get a handle for a reply
   if (!dbus_connection_send_with_reply (conn, msg, &pending, -1)) { // -1 is default timeout
      fprintf(stderr, "Out Of Memory!\n");
      std_log(LOG_FILENAME_LINE, "Out Of Memory!\n");
      create_xml(1);
      exit(1);
   }
   if (NULL == pending) {
      fprintf(stderr, "Pending Call Null\n");
      std_log(LOG_FILENAME_LINE, "Pending Call Null\n");
      create_xml(1);
      exit(1);
   }
   dbus_connection_flush(conn);
  
   printf("Request Sent\n");
   std_log(LOG_FILENAME_LINE, "Request Sent\n");
   // free message
   dbus_message_unref(msg);
  
   // block until we recieve a reply
   dbus_pending_call_block(pending);

   // get the reply message
   msg = dbus_pending_call_steal_reply(pending);
   if (NULL == msg) {
      fprintf(stderr, "Reply Null\n");
      std_log(LOG_FILENAME_LINE, "Reply Null\n");
      create_xml(1);
      exit(1);
   }
   // free the pending message handle
   dbus_pending_call_unref(pending);

dbus_error_init (&error);

if(dbus_message_get_type(msg) == DBUS_MESSAGE_TYPE_ERROR)
{
	
	if (!dbus_message_get_args (msg,
                              &error,
                              DBUS_TYPE_STRING, &s,
                              DBUS_TYPE_INVALID))
    {
      printf("no reply\n");
      std_log(LOG_FILENAME_LINE, "no reply with errr : %s\n",error.message);
      create_xml(1);
      exit(1);
    }
	  std_log(LOG_FILENAME_LINE, "reply with errr : %s\n",s);
}

if (!dbus_message_get_args (msg,
                              &error,
                              DBUS_TYPE_INT32, &no,
                              DBUS_TYPE_INVALID))
    {
      printf("no reply\n");
      std_log(LOG_FILENAME_LINE, "no reply with errr : %s\n",error.message);
      create_xml(1);
      exit(1);
    }
else
	{
			    
	   printf("Got Reply: %d, \n", no);
	   std_log(LOG_FILENAME_LINE, "Got Reply: %d, \n", no);
	}
	  
   dbus_message_unref(msg);  
   	 	  
}
 
 
                    

int main()
{
  
  DBusConnection* conn; 
  int i=0;   
 
 while(i<=225)
 {
 conn=connect_bus();
 send_msg(conn);
 
 dbus_connection_unref(conn);
 i++;

 }
  if(assert_failed == 0)
  {
  	std_log(LOG_FILENAME_LINE, "Test Successful.");
  	create_xml(0);
  }
   return 0;
}

