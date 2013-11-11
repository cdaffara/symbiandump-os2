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
* Description:
*
*/



#undef G_DISABLE_ASSERT
#undef G_LOG_DOMAIN

#include <stdio.h>
#include <glib.h>
#include <locale.h>
#include <string.h>
#include <stdlib.h>

#ifdef SYMBIAN
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/


static GKeyFile *
load_data (const gchar   *data, 
	   GKeyFileFlags  flags)
{
  GKeyFile *keyfile;
  GError *error = NULL;

  keyfile = g_key_file_new ();
  g_key_file_load_from_data (keyfile, data, -1, flags, &error);
  if (error)
    {
      g_print ("Could not load data: %s\n", error->message);
      
      g_assert(FALSE && "keyfile_test failed");
      
      #ifdef SYMBIAN
  	  testResultXml("keyfile_test");
  	  #endif /* EMULATOR */
      
      exit (1);
    }
  
  return keyfile;
}

static void
check_error (GError **error,
	     GQuark   domain,
	     gint     code)
{
  if (*error == NULL)
    {
      g_print ("Missing an error\n");
      exit (1);
    }
  
  if ((*error)->domain != domain)
    {
      g_print ("Wrong error domain: got %s, expected %s\n",
	       g_quark_to_string ((*error)->domain),
	       g_quark_to_string (domain));
	  
	  g_assert(FALSE && "keyfile_test failed");
      
      #ifdef SYMBIAN
  	  testResultXml("keyfile_test");
  	  #endif /* EMULATOR */
  	  
      exit (1);
    }
  
  if ((*error)->code != code)
    {
      g_print ("Wrong error code: got %d, expected %d\n",
	       (*error)->code, code);
	       
	  g_assert(FALSE && "keyfile_test failed");
      
      #ifdef SYMBIAN
  	  testResultXml("keyfile_test");
  	  #endif /* EMULATOR */
      
      exit (1);
    }

  g_error_free (*error);
  *error = NULL;
}

static void 
check_no_error (GError **error)
{
  if (*error != NULL)
    {
      g_print ("Unexpected error: (%s, %d) %s\n",
	       g_quark_to_string ((*error)->domain),
	       (*error)->code, (*error)->message);
	  g_assert(FALSE && "keyfile_test failed");
      
      #ifdef SYMBIAN
  	  testResultXml("keyfile_test");
  	  #endif /* EMULATOR */
      
      exit (1);
    }
}

static void
check_string_value (GKeyFile    *keyfile,
		    const gchar *group,
		    const gchar *key,
		    const gchar *expected) 
{
  GError *error = NULL;
  gchar *value;

  value = g_key_file_get_string (keyfile, group, key, &error);
  check_no_error (&error);
  g_assert (value != NULL);

  if (strcmp (value, expected) != 0)
    {
      g_print ("Group %s key %s: "
	       "expected string value '%s', actual value '%s'\n",
	       group, key, expected, value);      
	  
	  g_assert(FALSE && "keyfile_test failed");
      
      #ifdef SYMBIAN
  	  testResultXml("keyfile_test");
  	  #endif /* EMULATOR */
      
      exit (1);
    }

  g_free (value);
}

static void
check_locale_string_value (GKeyFile    *keyfile,
			   const gchar *group,
			   const gchar *key,
			   const gchar *locale,
			   const gchar *expected) 
{
  GError *error = NULL;
  gchar *value;

  value = g_key_file_get_locale_string (keyfile, group, key, locale, &error);
  check_no_error (&error);
  g_assert (value != NULL);

  if (strcmp (value, expected) != 0)
    {
      g_print ("Group %s key %s locale %s: "
	       "expected string value '%s', actual value '%s'\n",
	       group, key, locale, expected, value);      
	  
	  g_assert(FALSE && "keyfile_test failed");
      
      #ifdef SYMBIAN
  	  testResultXml("keyfile_test");
  	  #endif /* EMULATOR */
  	  
      exit (1);
    }

  g_free (value);
}

static void
check_boolean_value (GKeyFile    *keyfile,
		     const gchar *group,
		     const gchar *key,
		     gboolean     expected) 
{
  GError *error = NULL;
  gboolean value;

  value = g_key_file_get_boolean (keyfile, group, key, &error);
  check_no_error (&error);

  if (value != expected)
    {
      g_print ("Group %s key %s: "
	       "expected boolean value '%s', actual value '%s'\n",
	       group, key, 
	       expected ? "true" : "false", 
	       value ? "true" : "false");      
	       
	       
	  g_assert(FALSE && "keyfile_test failed");
      
      #ifdef SYMBIAN
  	  testResultXml("keyfile_test");
  	  #endif /* EMULATOR */
  	  
      exit (1);
    }
}

static void
check_integer_value (GKeyFile    *keyfile,
		     const gchar *group,
		     const gchar *key,
		     gint         expected) 
{
  GError *error = NULL;
  gint value;

  value = g_key_file_get_integer (keyfile, group, key, &error);
  check_no_error (&error);

  if (value != expected)
    {
      g_print ("Group %s key %s: "
	       "expected integer value %d, actual value %d\n",
	       group, key, expected, value);      
	  
	  g_assert(FALSE && "keyfile_test failed");
      
      #ifdef SYMBIAN
  	  testResultXml("keyfile_test");
  	  #endif /* EMULATOR */
  	  
      exit (1);
    }
}

int create_file(char *path)
{
	FILE *fp = fopen(path,"w");
	if(fp)
	{
		fprintf(fp,"[group1]\n");
		fprintf(fp,"key1=123\n");
		fprintf(fp,"key2=456\n");
		fclose(fp);
		return TRUE;
	}
	return FALSE;
}

void test_loadfromfile()
{
	
	GKeyFile *keyfile = g_key_file_new(),*keyfile1=g_key_file_new();
	
	gchar *full_path = NULL;	

	int ret;
	if(create_file("c:\\tempfile.txt"))
	{
		g_assert(g_key_file_load_from_file(keyfile,"c:\\tempfile.txt",0,NULL));
		check_string_value (keyfile, "group1", "key1", "123");
		check_string_value (keyfile, "group1", "key2", "456");
	}
	else
		g_assert(FALSE);
	
	g_key_file_free(keyfile);
	
	if(create_file("tempfile.txt"))
		g_assert(g_key_file_load_from_data_dirs(keyfile1,"tempfile.txt",&full_path,0,NULL));
	
	
}

void test_setstring()
{
	GKeyFile *keyfile;
	gchar **value;
	gsize n = 2;
	
	const gchar *list[2] = 
	{
		"test1","test2"	
	};

  	const gchar *data = 
	    "[1]\n"
	    "key1=123\n"
	    "[2]\n"
	    "key2=456\n";

	keyfile = load_data (data, 0);
	
	check_string_value (keyfile, "1", "key1", "123");
	g_key_file_set_string(keyfile,"1","key1","789");
	check_string_value (keyfile, "1", "key1", "789");
	
	g_key_file_set_string_list(keyfile,"1","key1",list,2);
	
	value = g_key_file_get_string_list(keyfile, "1", "key1",&n, NULL);
	
	g_assert(!strcmp(value[0],"test1"));
	g_assert(!strcmp(value[1],"test2"));
	g_strfreev(value);
}

void test_setboolean()
{
	GKeyFile *keyfile;
	gboolean *value;
	gsize n = 2;
	
	gboolean list[2] = {TRUE,FALSE};
	
	const gchar *data = 
	    "[1]\n"
	    "key1=true\n"
	    "key2=false\n";

	keyfile = load_data (data, 0);
	
	check_boolean_value (keyfile, "1", "key1", TRUE);
	g_key_file_set_boolean(keyfile,"1","key1",FALSE);
	check_boolean_value (keyfile, "1", "key1", FALSE);
	
	g_key_file_set_boolean_list(keyfile,"1","key1",list,2);
	
	value = g_key_file_get_boolean_list(keyfile, "1", "key1",&n, NULL);
	
	g_assert(value[0] == TRUE);
	g_assert(value[1] == FALSE);
	
}

void test_setinteger()
{
	GKeyFile *keyfile;
	gint *value;
	gsize n = 2;
	
	gint list[2] = {111,222};
	
	const gchar *data = 
	    "[1]\n"
	    "key1=123\n"
	    "key2=456\n";

	keyfile = load_data (data, 0);
	
	check_integer_value (keyfile, "1", "key1", 123);
	g_key_file_set_integer(keyfile,"1","key1",789);
	check_integer_value (keyfile, "1", "key1", 789);
	
	g_key_file_set_integer_list(keyfile,"1","key1",list,2);
	
	value = g_key_file_get_integer_list(keyfile, "1", "key1",&n, NULL);
	
	g_assert(value[0] == 111);
	g_assert(value[1] == 222);
	
}

void test_keyfiletodata()
{
	GKeyFile *keyfile;
	
	gchar *retVal = NULL;
	gsize length = 0;
	
	const gchar *data = 
	    "[1]\n"
	    "key1=123\n"
	    "key2=456\n";
	
	keyfile = load_data (data, 0);
	
	retVal = g_key_file_to_data(keyfile,&length,NULL);
	
	g_assert(retVal != NULL && length > 0);
}

void test_setcomment()
{
	GKeyFile *keyfile;
	gchar *comment;

	const gchar *data = 
	    "[1]\n"
	    "key1=123\n"
	    "key2=456\n";
	
	keyfile = load_data (data, 0);
	
	g_key_file_set_comment(keyfile,"1","key1","comment1",NULL);
	
	comment = g_key_file_get_comment(keyfile,"1","key1",NULL);
	
	g_assert(!strcmp(comment,"comment1\n"));
	
	g_key_file_remove_comment(keyfile,"1","key1",NULL);
	
	g_free(comment);
	
	comment = g_key_file_get_comment(keyfile,"1","key1",NULL);
	
	g_assert(comment == NULL);
	
}

void test_setlocale()
{
	GKeyFile *keyfile;
	gchar *value;
	gchar **list;
	gchar *input_list[2] = {"test2","test3"};
	gsize length = 2;
	
	const gchar *data = 
	    "[1]\n"
	    "key1=123\n"
	    "key2=456\n";
	
	keyfile = load_data (data, 0);
	
	g_key_file_set_locale_string(keyfile,"1","key3","C","test");
	
	value = g_key_file_get_locale_string(keyfile,"1","key3","C",NULL);
	
	g_assert(!strcmp(value,"test"));
	
	g_free(value);
	
	value = g_key_file_get_locale_string(keyfile,"1","key3","de",NULL);
	
	g_assert(value == NULL);
	
	g_key_file_set_locale_string_list(keyfile,"1","key4","C",input_list,2);
	
	list = g_key_file_get_locale_string_list(keyfile,"1","key4","C",&length,NULL);
	
	g_assert(!strcmp(list[0],"test2"));
	g_assert(!strcmp(list[1],"test3"));
	
}

int
main (int argc, char *argv[])
{
  	#ifdef SYMBIAN
	
	g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
	#endif /*SYMBIAN*/
  
  test_loadfromfile();
  test_setstring();
  test_setboolean();
  test_setinteger();
  test_keyfiletodata();
  test_setcomment();
  test_setlocale();
  
  #ifdef SYMBIAN
  testResultXml("keyfile_test");
  #endif /* EMULATOR */
   
  #ifdef SYMBIAN
  testResultXml("keyfile_test");
  #endif /* EMULATOR */
  
  return 0;
}
