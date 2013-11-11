/* Portion Copyright © 2008 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.*/
#include "test-utils.h"
#define LOG_FILE "c:\\logs\\test_names_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

static DBusLoop *loop;

void create_xml(int result)
	{
	if(result)
		assert_failed = 1;
	
	testResultXml("test_names");
    close_log_file();
	}
static void
die (const char *message)
{
  fprintf (stderr, "*** test-names: %s", message);
  //for logs
  std_log(LOG_FILENAME_LINE, "*** test-names: %s", message);
  create_xml(1);
  exit (1);
}

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
 
int
main ()
{
  DBusError error;
  DBusConnection *connection;
  
  //_dbus_setenv("DBUS_VERBOSE","1");
  
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
/*con2 = dbus_bus_get_private (DBUS_BUS_SESSION, &error);
  if (con2 == NULL)
    {
      fprintf (stderr, "*** Failed to open connection to system bus: %s\n",
               error.message);
      dbus_error_free (&error);
      return 1;
    }
*/
/*
  loop = _dbus_loop_new ();
  if (loop == NULL)
    die ("No memory\n");
  
  if (!test_connection_setup (loop, connection))
    die ("No memory\n");
*/ 
  TestName(connection, "org.freedesktop.DBus.Test", TRUE);
  TestName(connection, "org.freedesktop.DBus.Test-2", TRUE);
  TestName(connection, "org.freedesktop.DBus.Test_2", TRUE);
#if 0
  TestName(connection, "Test_2", TRUE);
#endif

//ch=getchar();

  _dbus_verbose ("*** Test service name exiting\n");
  create_xml(0);
  return 0;
}

