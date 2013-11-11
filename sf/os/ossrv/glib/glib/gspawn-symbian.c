/* gspawn-spawn.c - Process launching on Symbian
 *
 *  Copyright 2000 Red Hat, Inc.
 *  Copyright 2003 Tor Lillqvist
 * Portions copyright (c) 2006-2009 Nokia Corporation.  All rights reserved.
 *
 * GLib is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * GLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GLib; see the file COPYING.LIB.  If not, write
 * to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
/*
 * Implementation of g_spawn family of functions on Symbian.

 */
/* Define this to get some logging all the time */
/* #define G_SPAWN_SYMBIAN_DEBUG */



/*This program is adapted to be used in the Symbian OS scenario*/

#include "config.h"

#include "glib.h"
#include "gprintfint.h"
#include "galias.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <errno.h>
#include <fcntl.h>

#include "glibintl.h"

#ifdef __SYMBIAN32__
#include <sys/select.h>
#include "glibbackend.h"
#include "glib_wsd.h"
#endif//__SYMBIAN32__

#if EMULATOR

PLS(debug,gspawn_symbian,int)
#define debug  (*FUNCTION_NAME(debug,gspawn_symbian)())

#endif /* EMULATOR */

#if (defined G_SPAWN_WIN32_DEBUG || defined G_SPAWN_SYMBIAN_DEBUG)
#if !(EMULATOR)
  static int debug = 1;
#endif /* EMULATOR */
  #define SETUP_DEBUG() /* empty */
#else
#if !(EMULATOR)
  static int debug = -1;
#endif /* EMULATOR */
  #define SETUP_DEBUG()					\
    G_STMT_START					\
      {							\
	if (debug == -1)				\
	  {						\
	    if (getenv ("G_SPAWN_WIN32_DEBUG") != NULL)	\
	      debug = 1;				\
	    else					\
	      debug = 0;				\
	  }						\
      }							\
    G_STMT_END
#endif

enum
{
  CHILD_NO_ERROR,
  CHILD_CHDIR_FAILED,
  CHILD_SPAWN_FAILED,
};

enum {
  ARG_CHILD_ERR_REPORT = 1,
  ARG_STDIN,
  ARG_STDOUT,
  ARG_STDERR,
  ARG_WORKING_DIRECTORY,
  ARG_CLOSE_DESCRIPTORS,
  ARG_USE_PATH,
  ARG_WAIT,
  ARG_PROGRAM,
  ARG_COUNT = ARG_PROGRAM
};


static gchar *
protect_argv_string (const gchar *string)
{
  const gchar *p = string;
  gchar *retval, *q;
  gint len = 0;
  gboolean need_dblquotes = FALSE;
  while (*p)
    {
      if (*p == ' ' || *p == '\t')
	need_dblquotes = TRUE;
      else if (*p == '"')
	len++;
      else if (*p == '\\')
	{
	  const gchar *pp = p;
	  while (*pp && *pp == '\\')
	    pp++;
	  if (*pp == '"')
	    len++;
	}
      len++;
      p++;
    }
  q = retval = g_malloc (len + need_dblquotes*2 + 1);
  p = string;

  if (need_dblquotes)
    *q++ = '"';
  
  while (*p)
    {
      if (*p == '"')
	*q++ = '\\';
      else if (*p == '\\')
	{
	  const gchar *pp = p;
	  while (*pp && *pp == '\\')
	    pp++;
	  if (*pp == '"')
	    *q++ = '\\';
	}
      *q++ = *p;
      p++;
    }
  
  if (need_dblquotes)
    *q++ = '"';
  *q++ = '\0';

  return retval;
}

static gint
protect_argv (gchar  **argv,
	      gchar ***new_argv)
{
  gint i;
  gint argc = 0;
  
  while (argv[argc])
    ++argc;
  *new_argv = g_new (gchar *, argc+1);

  /* Quote each argv element if necessary, so that it will get
   * reconstructed correctly in the C runtime startup code.  Note that
   * the unquoting algorithm in the C runtime is really weird, and
   * rather different than what Unix shells do. See stdargv.c in the C
   * runtime sources (in the Platform SDK, in src/crt).
   *
   * Note that an new_argv[0] constructed by this function should
   * *not* be passed as the filename argument to a spawn* or exec*
   * family function. That argument should be the real file name
   * without any quoting.
   */
  for (i = 0; i < argc; i++)
    (*new_argv)[i] = protect_argv_string (argv[i]);

  (*new_argv)[argc] = NULL;

  return argc;
}

EXPORT_C GQuark
g_spawn_error_quark (void)
{
  return g_quark_from_static_string ("g-exec-error-quark");
}

EXPORT_C gboolean
g_spawn_async (const gchar          *working_directory,
		    gchar               **argv,
		    gchar               **envp,
		    GSpawnFlags           flags,
		    GSpawnChildSetupFunc  child_setup,
		    gpointer              user_data,
		    GPid                 *child_handle,
		    GError              **error)
{
  g_return_val_if_fail (argv != NULL, FALSE);
  
  return g_spawn_async_with_pipes (working_directory,
					argv, envp,
					flags,
					child_setup,
					user_data,
					child_handle,
					NULL, NULL, NULL,
					error);
}

static gboolean
utf8_charv_to_wcharv (char     **utf8_charv,
		      wchar_t ***wcharv,
		      int       *error_index,
		      GError   **error)
{
  wchar_t **retval = NULL;

  *wcharv = NULL;
  if (utf8_charv != NULL)
    {
      int n = 0, i;

      while (utf8_charv[n])
		n++;
      retval = g_new (wchar_t *, n + 1);
            

      for (i = 0; i < n; i++)
	{
	  retval[i] = g_utf8_to_utf16 (utf8_charv[i], -1, NULL, NULL, error);
	  if (retval[i] == NULL)
	    {
	      if (error_index)
		*error_index = i;
	      while (i)
		g_free (retval[--i]);
	      g_free (retval);
	      return FALSE;
	    }
	}
	    
      retval[n] = NULL;
    }
  *wcharv = retval;
  return TRUE;
}

static gboolean
utf8_charv_to_cp_charv (char   **utf8_charv,
			gchar ***cp_charv,
			int     *error_index,
			GError **error)
{
  char **retval = NULL;

  *cp_charv = NULL;
  if (utf8_charv != NULL)
    {
      int n = 0, i;

      while (utf8_charv[n])
		n++;
      retval = g_new (char *, n + 1);

      for (i = 0; i < n; i++)
	{
	  retval[i] = g_locale_from_utf8 (utf8_charv[i], -1, NULL, NULL, error);
	  if (retval[i] == NULL)
	    {
	      if (error_index)
		*error_index = i;
	      while (i)
		g_free (retval[--i]);
	      g_free (retval);
	      return FALSE;
	    }
	}
      retval[n] = NULL;
    }

  *cp_charv = retval;
  return TRUE;
}

static gboolean
do_spawn_directly (gint                 *exit_status,
		   gboolean		 do_return_handle,
		   GSpawnFlags           flags,
		   gchar               **argv,
		   char                **envp,
		   char                **protected_argv,
		   GSpawnChildSetupFunc  child_setup,
		   gpointer              user_data,
		   GPid                 *child_handle,
		   GError              **error)     
{
  const int mode = (exit_status == NULL) ? P_NOWAIT : P_WAIT;
  char **new_argv;
  int rc = -1;
  int saved_errno;
  GError *conv_error = NULL;
  gint conv_error_index;
  
  char *cpargv0, **cpargv, **cpenvp;

  /*
  if G_SPAWN_FILE_AND_ARGV_ZERO is specified all the argv except the first
  ie argv[0] is passed to the child. if the flag is not specified all the 
  arguements are passed to the child including the the argv[0]
  */
  new_argv = (flags & G_SPAWN_FILE_AND_ARGV_ZERO) ? protected_argv + 1 : protected_argv; 
      
  cpargv0 = g_locale_from_utf8 (argv[0], -1, NULL, NULL, &conv_error);
  if (cpargv0 == NULL)
  {
  	g_set_error (error, G_SPAWN_ERROR, G_SPAWN_ERROR_FAILED,
		       "Invalid program name: %s",
		       conv_error->message);
	g_error_free (conv_error);

	return FALSE;
  }

  if  (!utf8_charv_to_cp_charv (new_argv, &cpargv, &conv_error_index, &conv_error))
  {
	g_set_error (error, G_SPAWN_ERROR, G_SPAWN_ERROR_FAILED,
		       "Invalid string in argument vector at %d: %s",
		       conv_error_index, conv_error->message);
	g_error_free (conv_error);
	g_free (cpargv0);

	return FALSE;
  }

  if (!utf8_charv_to_cp_charv (envp, &cpenvp, NULL, &conv_error))
  {
	g_set_error (error, G_SPAWN_ERROR, G_SPAWN_ERROR_FAILED,
		       "Invalid string in environment: %s",
		       conv_error->message);
	g_error_free (conv_error);
	g_free (cpargv0);
	g_strfreev (cpargv);

	return FALSE;
  }

  if (child_setup)
  	(* child_setup) (user_data);
  
  if (flags & G_SPAWN_SEARCH_PATH)
	if (cpenvp != NULL)
	  rc = spawnvpe (mode, cpargv0, (const char **) cpargv, (const char **) cpenvp);
	else
	  rc = spawnvp (mode, cpargv0, (const char **) cpargv);
  else
	if (envp != NULL)
	  rc = spawnve (mode, cpargv0, (const char **) cpargv, (const char **) cpenvp);
	else
	  rc = spawnv (mode, cpargv0, (const char **) cpargv);

  g_free (cpargv0);
  g_strfreev (cpargv);
  g_strfreev (cpenvp);
  

  saved_errno = errno;

  if (rc == -1 && saved_errno != 0)
    {
      g_set_error (error, G_SPAWN_ERROR, G_SPAWN_ERROR_FAILED,
		   _("Failed to execute child process (%s)"),
		   g_strerror (saved_errno));
      return FALSE;
    }

  if (exit_status == NULL)
    {
      if (child_handle && do_return_handle)
	  	*child_handle = (GPid) rc;
      else
      	{
	  	CloseHandle ((HANDLE) rc);
	  	if (child_handle)
	    	*child_handle = 0;
	  	}
    }
  else
    *exit_status = rc;

  return TRUE;
}

#if EMULATOR

PLS(warned_about_child_setup ,do_spawn_with_pipes,gboolean)
#define warned_about_child_setup (*FUNCTION_NAME(warned_about_child_setup,do_spawn_with_pipes)())

#endif /* EMULATOR */

static gboolean
do_spawn_with_pipes (gint                 *exit_status,
		     gboolean		   do_return_handle,
		     const gchar          *working_directory,
		     gchar               **argv,
		     char                **envp,
		     GSpawnFlags           flags,
		     GSpawnChildSetupFunc  child_setup,
		     gpointer              user_data,
		     GPid                 *child_handle,
		     gint                 *standard_input,
		     gint                 *standard_output,
		     gint                 *standard_error,
		     gint		  *err_report,
		     GError              **error)     
{
  char **protected_argv;
  int rc = -1;
  int argc;
  gboolean retval;

  #if !(EMULATOR)
  static gboolean warned_about_child_setup = FALSE;
  #endif /* EMULATOR */
  
  GError *conv_error = NULL;
    
  SETUP_DEBUG();

  if (child_setup && !warned_about_child_setup)
    {
      warned_about_child_setup = TRUE;
      g_warning ("passing a child setup function to the g_spawn functions is pointless and dangerous on Win32");
    }

  argc = protect_argv (argv, &protected_argv);

  retval =
	do_spawn_directly (exit_status, do_return_handle, flags,
			   argv, envp, protected_argv,
			   child_setup, user_data, child_handle,
			   error);
      g_strfreev (protected_argv);
   
   if(standard_input)
   	*standard_input = -1;
   if(standard_output)
   	*standard_output = -1;
   if(standard_error)
   	*standard_error = -1;
   
   return retval;  
}

#if EMULATOR
#undef warned_about_child_setup 
#endif /* EMULATOR */

EXPORT_C gboolean
g_spawn_sync (const gchar          *working_directory,
		   gchar               **argv,
		   gchar               **envp,
		   GSpawnFlags           flags,
		   GSpawnChildSetupFunc  child_setup,
		   gpointer              user_data,
		   gchar               **standard_output,
		   gchar               **standard_error,
		   gint                 *exit_status,
		   GError              **error)     
{
  gint outpipe = -1;
  gint errpipe = -1;
  gint reportpipe = -1;
  gint outindex = -1;
  gint errindex = -1;
  GString *outstr = NULL;
  GString *errstr = NULL;
  gint status;
  
  g_return_val_if_fail (argv != NULL, FALSE);
  g_return_val_if_fail (!(flags & G_SPAWN_DO_NOT_REAP_CHILD), FALSE);
  g_return_val_if_fail (standard_output == NULL ||
                        !(flags & G_SPAWN_STDOUT_TO_DEV_NULL), FALSE);
  g_return_val_if_fail (standard_error == NULL ||
                        !(flags & G_SPAWN_STDERR_TO_DEV_NULL), FALSE);
  
  /* Just to ensure segfaults if callers try to use
   * these when an error is reported.
   */
  if (standard_output)
    *standard_output = NULL;

  if (standard_error)
    *standard_error = NULL;
  
  if (!do_spawn_with_pipes (&status,
			    FALSE,
			    working_directory,
			    argv,
			    envp,
			    flags,
			    child_setup,
			    user_data,
			    NULL,
			    NULL,
			    standard_output ? &outpipe : NULL,
			    standard_error ? &errpipe : NULL,
			    &reportpipe,
			    error))
    return FALSE;

    if (exit_status)
    	*exit_status = status;
    return TRUE;
}

EXPORT_C gboolean
g_spawn_async_with_pipes (const gchar          *working_directory,
			       gchar               **argv,
			       gchar               **envp,
			       GSpawnFlags           flags,
			       GSpawnChildSetupFunc  child_setup,
			       gpointer              user_data,
			       GPid                 *child_handle,
			       gint                 *standard_input,
			       gint                 *standard_output,
			       gint                 *standard_error,
			       GError              **error)
{
  g_return_val_if_fail (argv != NULL, FALSE);
  g_return_val_if_fail (standard_output == NULL ||
                        !(flags & G_SPAWN_STDOUT_TO_DEV_NULL), FALSE);
  g_return_val_if_fail (standard_error == NULL ||
                        !(flags & G_SPAWN_STDERR_TO_DEV_NULL), FALSE);
  /* can't inherit stdin if we have an input pipe. */
  g_return_val_if_fail (standard_input == NULL ||
                        !(flags & G_SPAWN_CHILD_INHERITS_STDIN), FALSE);
  
  return do_spawn_with_pipes (NULL,
			      (flags & G_SPAWN_DO_NOT_REAP_CHILD),
			      working_directory,
			      argv,
			      envp,
			      flags,
			      child_setup,
			      user_data,
			      child_handle,
			      standard_input,
			      standard_output,
			      standard_error,
			      NULL,
			      error);
}

EXPORT_C gboolean
g_spawn_command_line_sync (const gchar  *command_line,
				gchar       **standard_output,
				gchar       **standard_error,
				gint         *exit_status,
				GError      **error)
{
  gboolean retval;
  gchar **argv = 0;

  g_return_val_if_fail (command_line != NULL, FALSE);
  
  if (!g_shell_parse_argv (command_line,
                           NULL, &argv,
                           error))
    return FALSE;
  
  retval = g_spawn_sync (NULL,
			      argv,
			      NULL,
			      G_SPAWN_SEARCH_PATH,
			      NULL,
			      NULL,
			      standard_output,
			      standard_error,
			      exit_status,
			      error);
  g_strfreev (argv);

  return retval;
}

EXPORT_C gboolean
g_spawn_command_line_async (const gchar *command_line,
				 GError     **error)
{
  gboolean retval;
  gchar **argv = 0;

  g_return_val_if_fail (command_line != NULL, FALSE);

  if (!g_shell_parse_argv (command_line,
                           NULL, &argv,
                           error))
    return FALSE;
  
  retval = g_spawn_async (NULL,
			       argv,
			       NULL,
			       G_SPAWN_SEARCH_PATH,
			       NULL,
			       NULL,
			       NULL,
			       error);
  g_strfreev (argv);

  return retval;
}

EXPORT_C void
g_spawn_close_pid (GPid pid)
{
    CloseHandle (pid);
}

#define __G_SPAWN_SYMBIAN_C__
#include "galiasdef.c"
