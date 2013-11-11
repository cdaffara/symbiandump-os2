/* Portion Copyright © 2008 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.*/
#include <dbus/dbus.h>

#define DBUS_COMPILATION /* cheat and use dbus-sysdeps */
#ifndef __SYMBIAN32__
#include <dbus/dbus-sysdeps.h>
#include <dbus/dbus-spawn.h>
#else
#include "dbus-sysdeps.h"
#include "dbus-spawn.h"
#endif //__SYMBIAN32__
#undef DBUS_COMPILATION
#include <stdio.h>

static void
setup_func (void *data)
{
  printf ("entering setup func.\n");
}

int
main (int argc, char **argv)
{
  char **argv_copy;
  int i;
  DBusError error;
  
  if (argc < 2)
    {
      fprintf (stderr, "You need to specify a program to launch.\n");

      return -1;
    }

  argv_copy = dbus_new (char *, argc);
  for (i = 0; i < argc - 1; i++)
    argv_copy [i] = argv[i + 1];
  argv_copy[argc - 1] = NULL;
  
  if (!_dbus_spawn_async_with_babysitter (NULL, argv_copy, setup_func, NULL, &error))
    {
      fprintf (stderr, "Could not launch application: \"%s\"\n",
	       error.message);
    }
  
  return 0;
}
