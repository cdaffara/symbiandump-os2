/* GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
 * Portion Copyright © 2008-09 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the GLib Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/. 
 */
 
 // This test case is a threaded test case.

#include "config.h"
#include <stdio.h>

#include <sys/types.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdlib.h>

#include <glib.h>

#ifdef SYMBIAN
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/

#define TEST_THREAD 1

#ifdef G_OS_WIN32
#include <windows.h>
#endif

#ifdef G_OS_WIN32
#define GPID_FORMAT "%p"
#else
#define GPID_FORMAT "%d"
#endif

GMainLoop *main_loop;
gint alive;

#if defined(G_OS_WIN32) || defined(SYMBIAN)
char *argv0;
#endif

GPid
get_a_child (gint ttl)
{
  GPid pid;

#ifdef G_OS_WIN32
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  gchar *cmdline;

  memset (&si, 0, sizeof (si));
  si.cb = sizeof (&si);
  memset (&pi, 0, sizeof (pi));

  cmdline = g_strdup_printf( "child-test -c%d", ttl);

  if (!CreateProcess (argv0, cmdline, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    g_error ("CreateProcess failed: %s\n", g_win32_error_message (GetLastError ()));

  g_free(cmdline);

  CloseHandle (pi.hThread);
  pid = pi.hProcess;

  return pid;
#elif defined(SYMBIAN)
  	gchar *working_directory = NULL;
    gchar **envp = NULL;
    gpointer user_data = NULL;
	GError *error = NULL;
	GPid child_pid;
	   
    GSpawnChildSetupFunc child_setup = NULL;
    
    int retVal = 0;
    
    int flags = G_SPAWN_FILE_AND_ARGV_ZERO|G_SPAWN_DO_NOT_REAP_CHILD;
    
    char **argv = NULL;      
	argv = (char **)malloc(3*sizeof(char *));
	argv[1] = (char *)malloc(10*sizeof(char));
	argv[0] = argv0;
    sprintf(argv[1],"-c%d",ttl);
    argv[2] = NULL;
    g_assert(g_spawn_async(working_directory,argv,envp,flags,child_setup,user_data,&child_pid,&error));
    return child_pid;
    
#else
  pid = fork ();
  if (pid < 0)
    exit (1);

  if (pid > 0)
    return pid;

  sleep (ttl);
  _exit (0);
#endif /* G_OS_WIN32 */
}

gboolean
child_watch_callback (GPid pid, gint status, gpointer data)
{
#ifdef VERBOSE
  gint ttl = GPOINTER_TO_INT (data);

  g_print ("child " GPID_FORMAT " (ttl %d) exited, status %d\n", pid, ttl, status);
#endif

  g_spawn_close_pid (pid);

  if (--alive == 0)
    g_main_loop_quit (main_loop);

  return TRUE;
}

static gboolean
quit_loop (gpointer data)
{
  GMainLoop *main_loop = data;

  g_main_loop_quit (main_loop);

  return TRUE;
}

#ifdef TEST_THREAD
static gpointer
test_thread (gpointer data)
{
  GMainLoop *new_main_loop;
  GSource *source;
  GPid pid = 0;
  gint ttl = GPOINTER_TO_INT (data);

  new_main_loop = g_main_loop_new (NULL, FALSE);

  pid = get_a_child (ttl);
  g_assert(pid != 0);
  source = g_child_watch_source_new (pid);
  g_source_set_callback (source, (GSourceFunc) child_watch_callback, data, NULL);
  g_source_attach (source, g_main_loop_get_context (new_main_loop));
  g_source_unref (source);

#ifdef VERBOSE
  g_print ("whee! created pid: " GPID_FORMAT " (ttl %d)\n", pid, ttl);
#endif

  g_main_loop_run (new_main_loop);

  return NULL;
}
#endif

int
main (int argc, char *argv[])
{
  GPid pid;
  
  #if defined(SYMBIAN) && (defined(__WINS__) || defined(__WINSCW__))
     
   testResultXml("child-test2");
   return 0;
  
  #endif // EMULATOR
#if defined(G_OS_WIN32) || defined(SYMBIAN)
  argv0 = argv[0];
  
  if (argc > 1 && argv[1][0] == '-' && argv[1][1] == 'c')
    {
      int ttl = atoi (argv[1] + 2);
      usleep (ttl);
      exit (0);
    }
#endif

	#ifdef SYMBIAN

	g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
	g_set_print_handler(mrtPrintHandler);
	#endif /*SYMBIAN*/

  /* Only run the test, if threads are enabled and a default thread
   * implementation is available.
   */
#if defined(G_THREADS_ENABLED) && ! defined(G_THREADS_IMPL_NONE)
	#ifdef TEST_THREAD
		g_thread_init (NULL);
	#endif
	
		main_loop = g_main_loop_new (NULL, FALSE);
		g_assert(main_loop != NULL);

	#ifdef G_OS_WIN32
		system ("ipconfig /all");
	#else
		system ("/bin/true");
	#endif

	alive = 2;
	g_timeout_add (30000, quit_loop, main_loop);

	#ifdef TEST_THREAD
		g_thread_create (test_thread, GINT_TO_POINTER (10), FALSE, NULL);
		g_thread_create (test_thread, GINT_TO_POINTER (20), FALSE, NULL);
	#else
		pid = get_a_child (10);
		g_assert(pid != 0);
		g_child_watch_add (pid, (GChildWatchFunc) child_watch_callback,
		     GINT_TO_POINTER (10));
		pid = get_a_child (20);
		g_assert(pid != 0);
		g_child_watch_add (pid, (GChildWatchFunc) child_watch_callback,
		     GINT_TO_POINTER (20));
	#endif
	
	g_main_loop_run (main_loop);
	
  	if (alive > 0)
    {
      g_warning ("%d children still alive\n", alive);
      g_assert(FALSE && "some children still alive");
      return 1;
    }
    
#endif

   #if SYMBIAN
   testResultXml("child-test2");
   #endif
   
   return 0;
}
