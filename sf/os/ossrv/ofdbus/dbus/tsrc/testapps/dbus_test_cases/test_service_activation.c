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

#define LOG_FILE "c:\\logs\\test_service_activation_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
	{
	if(result)
		assert_failed = 1;
	
	testResultXml("test_service_activation");
    close_log_file();
	}

/**
 * Call a method on a remote object
 */
void query()
{
   DBusMessage* msg;
   DBusMessageIter args;
   DBusConnection* conn;
   DBusError err,error;
   DBusPendingCall* pending;
   int ret;
   char *param="test hello";
   char *s; 
//  char *s=(char*)malloc(6);
  // printf("Calling remote method with\n");

   // initialiset the errors
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
   if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) {
   		create_xml(1);
      exit(1);
   }

   // create a new method call and check for errors
   msg = dbus_message_new_method_call("org.freedesktop.DBus.TestSuiteEchoService", // target for the method call
                                      "/org/freedesktop/TestSuite", // object to call on
                                      "org.freedesktop.TestSuite", // interface to call on
                                      "Echo"); // method name
   if (NULL == msg) {
      fprintf(stderr, "Message Null\n");
      std_log(LOG_FILENAME_LINE, "Message Null\n");
      create_xml(1);
      exit(1);
   }

   // append arguments
   dbus_message_iter_init_append(msg, &args);
   if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &param)) {
      fprintf(stderr, "Out Of Memory!\n");
      std_log(LOG_FILENAME_LINE, "Out Of Memory!\n");
      create_xml(1);
      exit(1);
   }
   
  
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
/*
   // read the parameters
   if (!dbus_message_iter_init(msg, &args))
      printf( "Message has no arguments!\n");
   else 
    {
	if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
      printf( "Argument is not string!\n");
      else
	{
      dbus_message_iter_get_basic(&args, s);
      printf( "Argument is string!\n");

   if (!dbus_message_iter_next(&args))
      fprintf(stderr, "Message has too few arguments!\n");
   else if (DBUS_TYPE_UINT32 != dbus_message_iter_get_arg_type(&args))
      fprintf(stderr, "Argument is not int!\n");
   else
      dbus_message_iter_get_basic(&args, &level);
*/
dbus_error_init (&error);
if (!dbus_message_get_args (msg,
                              &error,
                              DBUS_TYPE_STRING, &s,
                              DBUS_TYPE_INVALID))
    {
      printf("no reply\n");
      std_log(LOG_FILENAME_LINE, "no reply\n");
      assert_failed = 1;
    }



    
   printf("Got Reply: %s, \n", s);
   std_log(LOG_FILENAME_LINE, "Got Reply: %s, \n", s);
//   ch=getchar();
  //   }
 //   }
   // free reply and close connection
   dbus_message_unref(msg);  
//   dbus_connection_close(conn);
}

int main()
{
  
      query();
      std_log(LOG_FILENAME_LINE, "Test Successful.");
      create_xml(0);
   return 0;
}

