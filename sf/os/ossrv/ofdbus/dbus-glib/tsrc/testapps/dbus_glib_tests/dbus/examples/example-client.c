#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


static void lose (const char *fmt, ...) G_GNUC_NORETURN G_GNUC_PRINTF (1, 2);
static void lose_gerror (const char *prefix, GError *error) G_GNUC_NORETURN;

static void
lose (const char *str, ...)
{
  va_list args;

  va_start (args, str);

  vfprintf (stderr, str, args);
  fputc ('\n', stderr);

  va_end (args);

   getchar();
  exit (1);
}

static void
lose_gerror (const char *prefix, GError *error) 
{
  lose ("%s: %s", prefix, error->message);
}

static void
print_hash_value (gpointer key, gpointer val, gpointer data)
{
  printf ("%s -> %s\n", (char *) key, (char *) val);
}

int
main ()
{
  DBusGConnection *bus;
  DBusGProxy *remote_object;
  DBusGProxy *remote_object_introspectable;
  GError *error = NULL;
  char **reply_list;
  char **reply_ptr;
  GHashTable *hello_reply_dict;
  char *introspect_data;
  char ch;

  g_type_init ();

  {
    GLogLevelFlags fatal_mask;
    
    fatal_mask = g_log_set_always_fatal (G_LOG_FATAL_MASK);
    fatal_mask |= G_LOG_LEVEL_WARNING | G_LOG_LEVEL_CRITICAL;
    g_log_set_always_fatal (fatal_mask);
  }

  bus = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
  if (!bus)
    lose_gerror ("Couldn't connect to session bus", error);
  
  remote_object = dbus_g_proxy_new_for_name (bus,
					     "org.designfu.SampleService",
					     "/SomeObject",
					     "org.designfu.SampleInterface");

  if (!dbus_g_proxy_call (remote_object, "HelloWorld", &error,
			  G_TYPE_STRING, "Hello from example-client.c!", G_TYPE_INVALID,
			  G_TYPE_STRV, &reply_list, G_TYPE_INVALID))
    lose_gerror ("Failed to complete HelloWorld", error);

  /*
  if (!dbus_g_proxy_call (remote_object, "GetTuple", &error,
			  G_TYPE_INVALID,
			  G_TYPE_VALUE_ARRAY, &hello_reply_struct, G_TYPE_INVALID))
    lose_gerror ("Failed to complete GetTuple", error);
  */
  if (!dbus_g_proxy_call (remote_object, "GetDict", &error,
			  G_TYPE_INVALID,
			  DBUS_TYPE_G_STRING_STRING_HASHTABLE, &hello_reply_dict, G_TYPE_INVALID))
    lose_gerror ("Failed to complete GetDict", error);

  printf ("reply_list: ");
  for (reply_ptr = reply_list; *reply_ptr; reply_ptr++)
    printf ("\"%s\" ", *reply_ptr);
  printf ("\n");
  g_strfreev (reply_list);
  ch=getchar();
/*
  for (i = 0; i < hello_reply_struct->n_values; i++)
    {
      GValue strval = { 0, };

      g_value_init (&strval, G_TYPE_STRING);
      if (!g_value_transform (g_value_array_get_nth (hello_reply_struct, i), &strval))
	g_value_set_static_string (&strval, "(couldn't transform to string)");
      g_print ("%s: %s\n", g_type_name (G_VALUE_TYPE (g_value_array_get_nth (hello_reply_struct, i))),
	       g_value_get_string (&strval));
    }
  g_value_array_free (hello_reply_struct);
  printf ("\n");
*/
  g_hash_table_foreach (hello_reply_dict, print_hash_value, NULL);
  g_hash_table_destroy (hello_reply_dict);

  remote_object_introspectable = dbus_g_proxy_new_for_name (bus,
							    "org.designfu.SampleService",
							    "/SomeObject",
							    "org.freedesktop.DBus.Introspectable");
  if (!dbus_g_proxy_call (remote_object_introspectable, "Introspect", &error,
			  G_TYPE_INVALID,
			  G_TYPE_STRING, &introspect_data, G_TYPE_INVALID))
    lose_gerror ("Failed to complete Introspect", error);
  printf ("%s", introspect_data);
  g_free (introspect_data);
  ch=getchar();
  g_object_unref (G_OBJECT (remote_object_introspectable));
  g_object_unref (G_OBJECT (remote_object));

  exit(0);
 
}

/*
int
main (int argc, char **argv)
{
  DBusGConnection *connection;
  GError *error;
  DBusGProxy *proxy;
  char **name_list;
  char **name_list_ptr;
  
  g_type_init ();

  error = NULL;
  connection = dbus_g_bus_get (DBUS_BUS_SESSION,
                               &error);
  if (connection == NULL)
    {
      g_printerr ("Failed to open connection to bus: %s\n",
                  error->message);
      g_error_free (error);
      exit (1);
    }

// Create a proxy object for the "bus driver" (name "org.freedesktop.DBus") 
  
  proxy = dbus_g_proxy_new_for_name (connection,
                                     DBUS_SERVICE_DBUS,
                                     DBUS_PATH_DBUS,
                                     DBUS_INTERFACE_DBUS);

 // Call ListNames method, wait for reply 
  error = NULL;
  if (!dbus_g_proxy_call (proxy, "ListNames", &error, G_TYPE_INVALID,
                          G_TYPE_STRV, &name_list, G_TYPE_INVALID))
    {
   // Just do demonstrate remote exceptions versus regular GError 
      if (error->domain == DBUS_GERROR && error->code == DBUS_GERROR_REMOTE_EXCEPTION)
        g_printerr ("Caught remote method exception %s: %s",
	            dbus_g_error_get_name (error),
	            error->message);
      else
        g_printerr ("Error: %s\n", error->message);
      g_error_free (error);
      exit (1);
    }

  // Print the results 
 
  g_print ("Names on the message bus:\n");
  
  for (name_list_ptr = name_list; *name_list_ptr; name_list_ptr++)
    {
      g_print ("  %s\n", *name_list_ptr);
    }
  g_strfreev (name_list);

  g_object_unref (proxy);

  return 0;
}

*/
