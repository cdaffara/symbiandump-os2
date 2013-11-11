/* -*- mode: C; c-file-style: "gnu" -*- */
/* main.c  main() for message bus
 *
 * Copyright (C) 2003  CodeFactory AB
 * Copyright (C) 2003  Red Hat, Inc.
 * Copyright (C) 2004  Imendio HB
 * Portion Copyright © 2008 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 *
 * Licensed under the Academic Free License version 2.1
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */ 
#include "bus.h"
#include "driver.h"
#ifndef __SYMBIAN32__
#include <dbus/dbus-internals.h>
#include <dbus/dbus-watch.h>
#else
#include "dbus-internals.h"
#include "dbus-watch.h"
#endif //__SYMBIAN32__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef __SYMBIAN32__
#include <signal.h>
#endif
#include <errno.h>
#include "selinux.h"

static BusContext *context;

static int reload_pipe[2];
#define RELOAD_READ_END 0
#define RELOAD_WRITE_END 1

#ifdef __SYMBIAN32__
#if 0 
 #include <fcntl.h>

int lock_file()
{



       /*
       struct flock {
	off_t	l_start;	 starting offset 
	off_t	l_len;		 len = 0 means until end of file 
	pid_t	l_pid;		 lock owner 
	short	l_type;		 lock type: read/write, etc. 
	short	l_whence;	 type of l_start 
};
 l_type   l_whence  l_start  l_len  l_pid   */


    //	struct flock fl = { F_WRLCK, SEEK_SET, 0,       0,     0 };
    //	struct flock fl = { 0, 0, 0,F_WRLCK, SEEK_SET};
    	int fd;
    
      //  fl.l_pid = getpid();
    
    	
    	if ((fd = open(DBUS_LOCK_FILE, O_RDWR|O_CREAT|O_EXCL, 0666)) == -1) 
    	{
    //	perror("open file status");
    	return 0;
    	
    	}
    
   /* fcntl file lock not supported in openc 	
    	if (fcntl(fd, F_SETLKW, &fl) == -1) {
    		perror("fcntl");
		close(fd);
    		return 0;
    	}
    
   */ 	
    close(fd);
	return 1;

}

#endif

#endif
void
signal_handler (int sig)
{
#ifndef __SYMBIAN32__
  DBusString str;

  switch (sig)
    {
#ifdef DBUS_BUS_ENABLE_DNOTIFY_ON_LINUX 
    case SIGIO: 
      /* explicit fall-through */
#endif /* DBUS_BUS_ENABLE_DNOTIFY_ON_LINUX  */
    case SIGHUP:
      _dbus_string_init_const (&str, "foo");
      if (!_dbus_write_socket (reload_pipe[RELOAD_WRITE_END], &str, 0, 1))
	{
	  _dbus_warn ("Unable to write to reload pipe.\n");
	  exit (1);
	}
      break;

    case SIGTERM:
      _dbus_loop_quit (bus_context_get_loop (context));
      break;
    }

#endif
}
static void
usage (void)
{
  fprintf (stderr, DAEMON_NAME " [--version] [--session] [--system] [--config-file=FILE] [--print-address[=DESCRIPTOR]] [--print-pid[=DESCRIPTOR]] [--fork] [--nofork] [--introspect]\n");
  exit (1);
}

static void
version (void)
{
  printf ("D-Bus Message Bus Daemon %s\n"
          "Copyright (C) 2002, 2003 Red Hat, Inc., CodeFactory AB, and others\n"
          "This is free software; see the source for copying conditions.\n"
          "There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n",
          VERSION);
  exit (0);
}

static void
introspect (void)
{
  DBusString xml;
  const char *v_STRING;  

  if (!_dbus_string_init (&xml))
    goto oom;

  if (!bus_driver_generate_introspect_string (&xml))
    {
      _dbus_string_free (&xml);
      goto oom;
    }

  v_STRING = _dbus_string_get_const_data (&xml);
  printf ("%s\n", v_STRING); 

  exit (0);
 
 oom:
  _dbus_warn ("Can not introspect - Out of memory\n");
  exit (1);
}
static void
check_two_config_files (const DBusString *config_file,
                        const char       *extra_arg)
{
  if (_dbus_string_get_length (config_file) > 0)
    {
      fprintf (stderr, "--%s specified but configuration file %s already requested\n",
               extra_arg, _dbus_string_get_const_data (config_file));
      exit (1);
    }
}

static void
check_two_addr_descriptors (const DBusString *addr_fd,
                            const char       *extra_arg)
{
  if (_dbus_string_get_length (addr_fd) > 0)
    {
      fprintf (stderr, "--%s specified but printing address to %s already requested\n",
               extra_arg, _dbus_string_get_const_data (addr_fd));
      exit (1);
    }
}

static void
check_two_pid_descriptors (const DBusString *pid_fd,
                           const char       *extra_arg)
{
  if (_dbus_string_get_length (pid_fd) > 0)
    {
      fprintf (stderr, "--%s specified but printing pid to %s already requested\n",
               extra_arg, _dbus_string_get_const_data (pid_fd));
      exit (1);
    }
}

static dbus_bool_t
handle_reload_watch (DBusWatch    *watch,
		     unsigned int  flags,
		     void         *data)
{
  DBusError error;
  DBusString str;
  _dbus_string_init (&str);
  if (_dbus_read_socket (reload_pipe[RELOAD_READ_END], &str, 1) != 1)
    {
      _dbus_warn ("Couldn't read from reload pipe.\n");
      exit (1);
    }
  _dbus_string_free (&str);

  dbus_error_init (&error);
  if (! bus_context_reload_config (context, &error))
    {
      _dbus_warn ("Unable to reload configuration: %s\n",
		  error.message);
      dbus_error_free (&error);
      exit (1);
    }
  return TRUE;
}


static dbus_bool_t
reload_watch_callback (DBusWatch    *watch,
		       unsigned int  condition,
		       void         *data)
{
  return dbus_watch_handle (watch, condition);
}

static void
setup_reload_pipe (DBusLoop *loop)
{
  DBusError error;
  DBusWatch *watch;

  dbus_error_init (&error);

  if (!_dbus_full_duplex_pipe (&reload_pipe[0], &reload_pipe[1],
			       TRUE, &error))
    {
      _dbus_warn ("Unable to create reload pipe: %s\n",
		  error.message);
      dbus_error_free (&error);
      exit (1);
    }

  _dbus_fd_set_close_on_exec (reload_pipe[0]);
  _dbus_fd_set_close_on_exec (reload_pipe[1]);

  watch = _dbus_watch_new (reload_pipe[RELOAD_READ_END],
			   DBUS_WATCH_READABLE, TRUE,
			   handle_reload_watch, NULL, NULL);

  if (watch == NULL)
    {
      _dbus_warn ("Unable to create reload watch: %s\n",
		  error.message);
      dbus_error_free (&error);
      exit (1);
    }

  if (!_dbus_loop_add_watch (loop, watch, reload_watch_callback,
			     NULL, NULL))
    {
      _dbus_warn ("Unable to add reload watch to main loop: %s\n",
		  error.message);
      dbus_error_free (&error);
      exit (1);
    }

}


int
main (int argc, char **argv)
{
  DBusError error;
  DBusString config_file;
  DBusString addr_fd;
  DBusString pid_fd;
  const char *prev_arg;
  int print_addr_fd;
  int print_pid_fd;
  int i;
  dbus_bool_t print_address;
  dbus_bool_t print_pid;
  int force_fork;
  #ifdef __SYMBIAN32__
  char systemconfpath[35];
#endif
  
 // write(1, "hi daemon", 9); 

  if (!_dbus_string_init (&config_file))
    return 1;

  if (!_dbus_string_init (&addr_fd))
    return 1;

  if (!_dbus_string_init (&pid_fd))
    return 1;

  print_address = FALSE;
  print_pid = FALSE;
  force_fork = FORK_FOLLOW_CONFIG_FILE;

#ifdef __SYMBIAN32__
// Open C Does not have command arguments
// Open C Does not have fork
// force_fork = FORK_NEVER;
// __SYMBIAN32__ uses system dbus only

//  _dbus_string_append (&config_file, DBUS_SYSTEM_CONFIG_FILE);
    systemconfpath[0]= 'z';//Default

 	systemconfpath[0]=getSystemConfDriveLetter();
 	
 	systemconfpath[1]=':';
    systemconfpath[2]='\0';

  strcat(systemconfpath,"\\data\\dbus\\system.conf");
  _dbus_string_append (&config_file, systemconfpath);
  
  force_fork = FORK_NEVER;
  
#else
  prev_arg = NULL;
  i = 1;
  while (i < argc)
    {
      const char *arg = argv[i];

      if (strcmp (arg, "--help") == 0 ||
          strcmp (arg, "-h") == 0 ||
          strcmp (arg, "-?") == 0)
        usage ();
      else if (strcmp (arg, "--version") == 0)
        version ();
      else if (strcmp (arg, "--introspect") == 0)
        introspect ();
      else if (strcmp (arg, "--nofork") == 0)
        force_fork = FORK_NEVER;
      else if (strcmp (arg, "--fork") == 0)
        force_fork = FORK_ALWAYS;
      else if (strcmp (arg, "--system") == 0)
        {
          check_two_config_files (&config_file, "system");

          if (!_dbus_string_append (&config_file, DBUS_SYSTEM_CONFIG_FILE))
            exit (1);
        }
      else if (strcmp (arg, "--session") == 0)
        {
          check_two_config_files (&config_file, "session");

          if (!_dbus_string_append (&config_file, DBUS_SESSION_CONFIG_FILE))
            exit (1);
        }
      else if (strstr (arg, "--config-file=") == arg)
        {
          const char *file;

          check_two_config_files (&config_file, "config-file");
          
          file = strchr (arg, '=');
          ++file;

          if (!_dbus_string_append (&config_file, file))
            exit (1);
        }
      else if (prev_arg &&
               strcmp (prev_arg, "--config-file") == 0)
        {
          check_two_config_files (&config_file, "config-file");
          
          if (!_dbus_string_append (&config_file, arg))
            exit (1);
        }
      else if (strcmp (arg, "--config-file") == 0)
        ; /* wait for next arg */
      else if (strstr (arg, "--print-address=") == arg)
        {
          const char *desc;

          check_two_addr_descriptors (&addr_fd, "print-address");
          
          desc = strchr (arg, '=');
          ++desc;

          if (!_dbus_string_append (&addr_fd, desc))
            exit (1);

          print_address = TRUE;
        }
      else if (prev_arg &&
               strcmp (prev_arg, "--print-address") == 0)
        {
          check_two_addr_descriptors (&addr_fd, "print-address");
          
          if (!_dbus_string_append (&addr_fd, arg))
            exit (1);

          print_address = TRUE;
        }
      else if (strcmp (arg, "--print-address") == 0)
        print_address = TRUE; /* and we'll get the next arg if appropriate */
      else if (strstr (arg, "--print-pid=") == arg)
        {
          const char *desc;

          check_two_pid_descriptors (&pid_fd, "print-pid");
          
          desc = strchr (arg, '=');
          ++desc;

          if (!_dbus_string_append (&pid_fd, desc))
            exit (1);

          print_pid = TRUE;
        }
      else if (prev_arg &&
               strcmp (prev_arg, "--print-pid") == 0)
        {
          check_two_pid_descriptors (&pid_fd, "print-pid");
          
          if (!_dbus_string_append (&pid_fd, arg))
            exit (1);
          
          print_pid = TRUE;
        }
      else if (strcmp (arg, "--print-pid") == 0)
        print_pid = TRUE; /* and we'll get the next arg if appropriate */
      else
        usage ();
      
      prev_arg = arg;
      
      ++i;
    }
#endif // else (systems with command line arguments)

  if (_dbus_string_get_length (&config_file) == 0)
    {
      fprintf (stderr, "No configuration file specified.\n");
      usage ();
    }

  print_addr_fd = -1;
  if (print_address)
    {
      print_addr_fd = 1; /* stdout */
      if (_dbus_string_get_length (&addr_fd) > 0)
        {
          long val;
          int end;
          if (!_dbus_string_parse_int (&addr_fd, 0, &val, &end) ||
              end != _dbus_string_get_length (&addr_fd) ||
              val < 0 || val > _DBUS_INT_MAX)
            {
              fprintf (stderr, "Invalid file descriptor: \"%s\"\n",
                       _dbus_string_get_const_data (&addr_fd));
              exit (1);
            }

          print_addr_fd = val;
        }
    }
  _dbus_string_free (&addr_fd);

  print_pid_fd = -1;
  if (print_pid)
    {
      print_pid_fd = 1; /* stdout */
      if (_dbus_string_get_length (&pid_fd) > 0)
        {
          long val;
          int end;
          if (!_dbus_string_parse_int (&pid_fd, 0, &val, &end) ||
              end != _dbus_string_get_length (&pid_fd) ||
              val < 0 || val > _DBUS_INT_MAX)
            {
              fprintf (stderr, "Invalid file descriptor: \"%s\"\n",
                       _dbus_string_get_const_data (&pid_fd));
              exit (1);
            }

          print_pid_fd = val;
        }
    }
  _dbus_string_free (&pid_fd);
  
  #ifdef __SYMBIAN32__
  
  print_addr_fd=1;
 
 /*uncomment above line, if the bus address needs to be sent to the client starting the bus*/
 /* one more work around could be to write the address of the bus in the dbus_lock file, which can be read 
 from the client program if needed */
 
  #endif

  if (!bus_selinux_pre_init ())
    {
      _dbus_warn ("SELinux pre-initialization failed\n");
      exit (1);
    }

  dbus_error_init (&error);
  context = bus_context_new (&config_file, force_fork,
                             print_addr_fd, print_pid_fd,
                             &error);
  _dbus_string_free (&config_file);
  if (context == NULL)
    {
      _dbus_warn ("Failed to start message bus: %s\n",
                  error.message);
      dbus_error_free (&error);
      exit (1);
    }

  
  
  #ifndef __SYMBIAN32__
setup_reload_pipe (bus_context_get_loop (context));
  _dbus_set_signal_handler (SIGHUP, signal_handler);
  _dbus_set_signal_handler (SIGTERM, signal_handler);
  #endif
  
#ifdef DBUS_BUS_ENABLE_DNOTIFY_ON_LINUX 
  _dbus_set_signal_handler (SIGIO, signal_handler);
#endif /* DBUS_BUS_ENABLE_DNOTIFY_ON_LINUX */
  
  _dbus_verbose ("We are on D-Bus...\n");
#ifdef __SYMBIAN32__  
  lock_file();

#endif  
 _dbus_loop_run (bus_context_get_loop (context));
#ifdef __SYMBIAN32__    
  remove(DBUS_LOCK_FILE);
#endif  
  bus_context_shutdown (context);
  bus_context_unref (context);
  bus_selinux_shutdown ();

  return 0;
}
