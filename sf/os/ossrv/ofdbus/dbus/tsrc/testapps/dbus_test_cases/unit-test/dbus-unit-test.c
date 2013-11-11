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



#include "test-utils.h"
#include "string.h"

#define LOG_FILE "c:\\logs\\dbus_unit_test_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
	{
	if(result)
		assert_failed = 1;
	
	testResultXml("dbus_unit_test");
    close_log_file();
	}

static DBusLoop *loop;

static const char* echo_path = "/org/freedesktop/TestSuite" ;
static void
die (const char *message)
{
  fprintf (stderr, "*** dbus-unit-test: %s", message);
  std_log(LOG_FILENAME_LINE, "*** dbus-unit-test: %s", message);
  create_xml(1);
  exit (1);
}

static void
path_unregistered_func (DBusConnection  *connection,
                        void            *user_data)
{
  /* connection was finalized */
}

static DBusHandlerResult
path_message_func (DBusConnection  *connection,
                   DBusMessage     *message,
                   void            *user_data)
{
        return DBUS_HANDLER_RESULT_HANDLED;

}

static DBusObjectPathVTable
echo_vtable = {
  path_unregistered_func,
  path_message_func,
  NULL,
};


static void
TestName(DBusConnection *connection, const char *name, int expectedSuccess)
{
  DBusError error;
  dbus_error_init (&error);

  (void) dbus_bus_request_name (connection, name, 0, &error);
  if (dbus_error_is_set (&error))
    {
      if (expectedSuccess)
    	  {
    	  fprintf (stderr, "Error acquiring name '%s': %s\n", name, error.message);
    	  std_log(LOG_FILENAME_LINE, "Error acquiring name '%s': %s\n", name, error.message);
    	  }
      else
    	  {
    	  fprintf (stdout, "Expected Error acquiring name '%s': %s\n", name, error.message);
    	  std_log(LOG_FILENAME_LINE, "Expected Error acquiring name '%s': %s\n", name, error.message);
    	  }
      _dbus_verbose ("*** Failed to acquire name '%s': %s\n", name,
                     error.message);
      dbus_error_free (&error);
      if (expectedSuccess)
    	  {
    	  create_xml(1);
    	  exit (1);
    	  }
    }
  else 
    {
      if (!expectedSuccess)
    	  {
    	  fprintf (stderr, "Unexpected Success acquiring name '%s'\n", name);
    	  std_log(LOG_FILENAME_LINE, "Unexpected Success acquiring name '%s'\n", name);
    	  }
      else
    	  {
    	  fprintf (stdout, "Successfully acquired name '%s'\n", name);
    	  std_log(LOG_FILENAME_LINE, "Successfully acquired name '%s'\n", name);
    	  }
      _dbus_verbose ("*** Managed to acquire name '%s'\n", name);
      if (!expectedSuccess)
    	  {
    	  create_xml(1);
    	  exit (1);
    	  }
    }
}

static int TestBusHasOwner(DBusConnection *connection,char *name, DBusError *error)
{
	return dbus_bus_name_has_owner (connection,name,error);
}	

static void dbus_print_connection_stats(DBusConnection *connection)
{
	fprintf(stdout,"Max message size = %d\n",dbus_connection_get_max_message_size(connection));
	std_log(LOG_FILENAME_LINE, "Max message size = %d\n",dbus_connection_get_max_message_size(connection));
	fprintf(stdout,"Max received size = %d\n",dbus_connection_get_max_received_size(connection));
	std_log(LOG_FILENAME_LINE, "Max received size = %d\n",dbus_connection_get_max_received_size(connection));
	fprintf(stdout,"Outgoing size = %d\n",dbus_connection_get_outgoing_size(connection));
	std_log(LOG_FILENAME_LINE, "Outgoing size = %d\n",dbus_connection_get_outgoing_size(connection));
	fprintf(stdout,"Local machine id =%s\n",dbus_get_local_machine_id());
	std_log(LOG_FILENAME_LINE, "Local machine id =%s\n",dbus_get_local_machine_id());
}	

static void dbus_set_connection_params(DBusConnection *connection)
{
dbus_connection_set_max_message_size(connection,1234);
dbus_connection_set_max_received_size(connection,1234);
dbus_connection_set_route_peer_messages(connection,1234);
}

static int TestRegisterUnregisterObjectPath(DBusConnection *connection)
{
	    void *d;
	if (!dbus_connection_register_object_path (connection,
                                             echo_path,
                                             &echo_vtable,
                                             (void*) 0xdeadbeef))
    {                                             
     fprintf(stderr,"dbus_connection_register_object_path : No memory\n");
     std_log(LOG_FILENAME_LINE, "dbus_connection_register_object_path : No memory\n");
     return 1;
    }


    if (!dbus_connection_get_object_path_data (connection, echo_path, &d))
       {                                             
     fprintf(stderr,"dbus_connection_get_object_path_data : No memory");
     std_log(LOG_FILENAME_LINE, "dbus_connection_get_object_path_data : No memory");
     return 1;
    }
    if (d != (void*) 0xdeadbeef)
      {
      	fprintf(stderr,"dbus_connection_get_object_path_data() doesn't seem to work right\n");
        std_log(LOG_FILENAME_LINE, "dbus_connection_get_object_path_data() doesn't seem to work right\n");
      	return 1;
     
  }
	if (!dbus_connection_unregister_object_path (connection,
                                             echo_path))
    {                                             
     fprintf(stderr,"dbus_connection_unregister_object_path : No memory\n");
     std_log(LOG_FILENAME_LINE, "dbus_connection_unregister_object_path : No memory\n");
     return 1;
    }
	return 0;
}


static int  TestDbusMessageType(void)
  {
  	if(strcmp(dbus_message_type_to_string(DBUS_MESSAGE_TYPE_METHOD_CALL),"method_call"))
  	return 1; 
	if(strcmp(dbus_message_type_to_string(DBUS_MESSAGE_TYPE_METHOD_RETURN),"method_return"))
  	return 1; 
  	if(strcmp(dbus_message_type_to_string(DBUS_MESSAGE_TYPE_SIGNAL),"signal"))
  	return 1; 
  	if(strcmp(dbus_message_type_to_string(DBUS_MESSAGE_TYPE_ERROR),"error"))
  	return 1; 
    return 0;
  }

static int TestDbusAllocateDeallocateDataSlot(void)
{
	dbus_int32_t slot_id=-1;
	if(!dbus_message_allocate_data_slot(&slot_id))
	return 1;
	dbus_message_free_data_slot(&slot_id);
  
	return 0;
}	
void  free_data_func(void *data)
{
	return;
}	

static int TestDbusMessageSetGetApis(void)
{
	dbus_int32_t slot_id=-1;
	DBusMessage* msg=NULL;
	
	
	if(!dbus_message_allocate_data_slot(&slot_id))
	return 1;
	if((msg = dbus_message_new(DBUS_MESSAGE_TYPE_METHOD_CALL))==NULL)
	return 1;
	if(!dbus_message_set_data(msg,slot_id,(void *)0x1234,&free_data_func))
	return 1;
	if(dbus_message_get_data(msg, slot_id)!=(void *)0x1234)
	return 1;
/*  if(!dbus_message_set_destination (message, bus_connection_get_name (connection)) 
	return 1;*/
  if (!dbus_message_set_error_name (msg, DBUS_ERROR_NO_REPLY))
    {
      dbus_message_unref (msg);
      msg = NULL;
      return 1;
    }
  dbus_message_free_data_slot(&slot_id);
	

return 0;  

}	
int
main ()
{
  DBusError error;
  DBusConnection *connection;
   
  dbus_error_init (&error);
  connection = dbus_bus_get (DBUS_BUS_SESSION, &error);
  if (connection == NULL)
    {
      fprintf (stderr, "*** Failed to open connection to system bus: %s\n",
               error.message);
      std_log(LOG_FILENAME_LINE, "*** Failed to open connection to system bus: %s\n",
              error.message);
      dbus_error_free (&error);
      create_xml(1);
      return 1;
    }

  loop = _dbus_loop_new ();
  if (loop == NULL)
    die ("No memory\n");
  
  if (!test_connection_setup (loop, connection))
    die ("No memory\n");

  if(!dbus_connection_get_is_connected(connection))
    die ("Connection is not connected\n");
  if(TestBusHasOwner(connection,"org.freedesktop.DBus.Test",&error))
  {
      fprintf (stderr, "*** DBUS already has a owner : : %s\n",
               error.message);
      std_log(LOG_FILENAME_LINE, "*** DBUS already has a owner : : %s\n",
              error.message);
      dbus_error_free (&error);
      create_xml(1);
      return 1;
  }
  TestName(connection, "org.freedesktop.DBus.Test", TRUE);
  if(!TestBusHasOwner(connection,"org.freedesktop.DBus.Test",&error))
  {
      fprintf (stderr, "*** DBUS doesn't have a owner : : %s\n",
               error.message);
      std_log(LOG_FILENAME_LINE, "*** DBUS doesn't have a owner : : %s\n",
              error.message);
      dbus_error_free (&error);
      create_xml(1);
      return 1;
  }
 
 if(!dbus_connection_get_is_authenticated(connection))
	 {
  	 fprintf (stderr, "*** DBUS connection not authenticated\n");
  	 std_log(LOG_FILENAME_LINE, "*** DBUS connection not authenticated\n");
	 }
 else
	 {
  	 fprintf (stderr, "*** DBUS connection Authenticated\n");
  	 std_log(LOG_FILENAME_LINE, "*** DBUS connection Authenticated\n");
	 }

  dbus_print_connection_stats(connection);
  dbus_set_connection_params(connection);
  dbus_print_connection_stats(connection);
  if(TestRegisterUnregisterObjectPath(connection))
  {
      fprintf (stderr, "*** TestRegisterUnregisterObjectPath error : : \n");
      std_log(LOG_FILENAME_LINE, "*** TestRegisterUnregisterObjectPath error : : \n");
      dbus_error_free (&error);
      create_xml(1);
      return 1;
  	
  }
  if(TestDbusMessageType())
  {
      fprintf (stderr, "*** TestDbusMessageType error : : \n");
      std_log(LOG_FILENAME_LINE, "*** TestDbusMessageType error : : \n");
      dbus_error_free (&error);
      create_xml(1);
      return 1;
  }
  if(TestDbusAllocateDeallocateDataSlot())
  {
      fprintf (stderr, "*** TestDbusAllocateDeallocateDataSlot error : : \n");
      std_log(LOG_FILENAME_LINE, "*** TestDbusAllocateDeallocateDataSlot error : : \n");
      dbus_error_free (&error);
      create_xml(1);
      return 1;
	}
	if(TestDbusMessageSetGetApis())
	{
	    fprintf (stderr, "*** TestDbusMessageSetGetApis error : : \n");
	    std_log(LOG_FILENAME_LINE, "*** TestDbusMessageSetGetApis error : : \n");
      dbus_error_free (&error);
      create_xml(1);
      return 1;
	}
		
  dbus_connection_flush(connection);
  //dbus_connection_close(connection); should not be called for shared connection

  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
  return 0;
}
