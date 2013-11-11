/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
*
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
*
* Description:  ?Description
*
*/


#undef G_DISABLE_ASSERT
#undef G_LOG_DOMAIN


#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <fcntl.h>
#include <goption.h>

#ifdef SYMBIAN
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/

#define	C2P(c)		((gpointer) ((long) (c)))
#define GINT_TO_POINTER(i)	((gpointer)  (i))
#define GPOINTER_TO_INT(p)	((gint)   (p))
#define TESTPASS	1
#define TESTFAIL	0

int arg_test1_int;
int arg_test2_int;

static gboolean error_test1_pre_parse (GOptionContext *context,
				GOptionGroup   *group,
		       gpointer	       data,
		       GError        **error)
{
  g_assert (arg_test1_int == 0x12345678);

  return TRUE;
}

static gboolean error_test1_post_parse (GOptionContext *context,
			GOptionGroup   *group,
			gpointer	  data,
			GError        **error)
{
  g_assert (arg_test1_int == 20);

  /* Set an error in the post hook */
  g_set_error (error, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE, "");

  return FALSE;
}

gchar** split_string (const char *str, int *argc)
{
  gchar **argv;
  int len;
  
  argv = g_strsplit (str, " ", 0);

  for (len = 0; argv[len] != NULL; len++);

  if (argc)
    *argc = len;
    
  return argv;
}


void error_func (GOptionContext *context,GOptionGroup *group, gpointer data,GError **error)
{
	
}

void tg_option_tests()
{

	GOptionContext *context;
	GOptionContext *context1;//for testing set main group
	GOptionContext *context2;//for testing add group
	gboolean retval;
	GError *error = NULL;
	gchar **argv;
	int argc;
	gchar **argv1;
	int argc1;
	GOptionEntry entries [] =
	    { { "test", 0, 0, G_OPTION_ARG_INT, &arg_test1_int, "test_description", "test_arg" },
	      { NULL } };
	GOptionEntry entries1 [] =
	    { { "try", 0, 0, G_OPTION_ARG_INT, &arg_test1_int, "try_description", "try_arg" },
	      { NULL } };
	GOptionEntry entries2 [] =
	    { { "again", 0, 0, G_OPTION_ARG_INT, &arg_test1_int, "again_description", "again_arg" },
	      { NULL } };
	
	GOptionGroup *main_group;
		
	context = g_option_context_new (NULL);
	context1 = g_option_context_new (NULL);
	context2 = g_option_context_new (NULL);
	g_option_context_add_main_entries (context, entries, NULL);
	g_option_context_add_main_entries (context2, entries, NULL);
  	
  	main_group = g_option_context_get_main_group (context);
  	//Testing g_option_context_set_main_group with another context-context1
	g_option_group_add_entries (main_group,entries1);
	g_option_context_set_main_group(context1,main_group);

	//Testing g_option_context_set_help_enabled 
	//and g_option_context_get_help_enabled
	//and g_option_context_get_ignore_unknown_options
	g_option_context_set_help_enabled(context,TRUE);
	g_assert(g_option_context_get_help_enabled(context));
	g_assert(!g_option_context_get_ignore_unknown_options(context));
	
	/* Now try parsing on context1*/
	argv = split_string ("program --try 20 --try 30", &argc);
	  
	retval = g_option_context_parse (context1, &argc, &argv, &error);
	g_assert (retval);

	/* Last arg specified is the one that should be stored */
	g_assert (arg_test1_int == 30);

	g_option_group_set_error_hook (main_group, error_func);
	argv = split_string ("program --none 20 --none 30", &argc);
	retval = g_option_context_parse (context1, &argc, &argv, &error);
		
	g_strfreev (argv);
	g_option_context_free (context);
	  
}

void g_option_context_add_group_test()
{

	GOptionContext *context;
	GOptionEntry entries [] =
		{ { "test", 0, 0, G_OPTION_ARG_INT, &arg_test2_int, "test_description", "test_arg" },
	    { NULL } };
	GOptionGroup *group;
	gboolean retval;
	GError *error = NULL;
	gchar **argv;
	int argc;
	
	group = g_option_group_new ("test","test_description","help_desc",NULL,NULL);
	g_option_group_add_entries (group, entries);
	
	context = g_option_context_new (NULL);
	
	//Testing this:g_option_context_add_group
	g_option_context_add_group (context, group);
	
	/* Now try parsing on context1*/
	argv = split_string ("program --test 20 --test 30", &argc);
	  
	retval = g_option_context_parse (context, &argc, &argv, &error);
	g_assert (retval);

	/* Last arg specified is the one that should be stored */
	g_assert (arg_test2_int == 30);
	g_strfreev (argv);
	g_option_context_free (context);
}


int main (int argc,char *argv[])
{

	#ifdef SYMBIAN
 
 	g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO |  G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
 	#endif /*SYMBIAN*/
 	
 	tg_option_tests();
 	g_option_context_add_group_test();
 
#ifdef SYMBIAN
  testResultXml("toption");
#endif /* EMULATOR */
 	return 0;
}