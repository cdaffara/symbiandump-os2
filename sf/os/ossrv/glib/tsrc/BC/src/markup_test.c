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



/* This test case is dependent on the markup filr 1.gmarkup placed in the c dir.
   Make sure that the file is present before this test case is run. Export the file
   */

#undef G_DISABLE_ASSERT
#undef G_LOG_DOMAIN

#include <stdio.h>
#include <glib.h>

#ifdef SYMBIAN
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/

static int depth = 0;


static void
start_element_handler  (GMarkupParseContext *context,
                        const gchar         *element_name,
                        const gchar        **attribute_names,
                        const gchar        **attribute_values,
                        gpointer             user_data,
                        GError             **error)
{
  ++depth;
}

static void
end_element_handler    (GMarkupParseContext *context,
                        const gchar         *element_name,
                        gpointer             user_data,
                        GError             **error)
{
  --depth;
}

static void
text_handler           (GMarkupParseContext *context,
                        const gchar         *text,
                        gsize                text_len,
                        gpointer             user_data,
                        GError             **error)
{

}


static void
passthrough_handler    (GMarkupParseContext *context,
                        const gchar         *passthrough_text,
                        gsize                text_len,
                        gpointer             user_data,
                        GError             **error)
{
}

static void
error_handler          (GMarkupParseContext *context,
                        GError              *error,
                        gpointer             user_data)
{
  g_assert(FALSE && "markup_test failed");
  g_print(" %s\n", error->message);
}

static GMarkupParser parser = {
  start_element_handler,
  end_element_handler,
  text_handler,
  passthrough_handler,
  error_handler
};

static int
test_in_chunks (const gchar *contents,
                gint         length,
                gint         chunk_size)
{
  GMarkupParseContext *context;
  int i = 0;
  int line_number,char_number;
  
  context = g_markup_parse_context_new (&parser, 0, NULL, NULL);

  while (i < length)
    {
      int this_chunk = MIN (length - i, chunk_size);

      if (!g_markup_parse_context_parse (context,
                                         contents + i,
                                         this_chunk,
                                         NULL))
        {
          g_markup_parse_context_free (context);
          return 1;
     
        }
    
	g_assert(g_markup_parse_context_get_element(context) == NULL || !strcmp(g_markup_parse_context_get_element(context),"foobar"));
	
	g_markup_parse_context_get_position(context,&line_number,&char_number);
	
	g_assert(line_number == 1 && char_number == 25);
    
    i += this_chunk;
    }
      
  if (!g_markup_parse_context_end_parse (context, NULL))
    {
      g_markup_parse_context_free (context);
      return 1;
    }

  g_markup_parse_context_free (context);

  return 0;
}

static int
test_file (const gchar *filename)
{
  gchar *contents;
  gsize  length;
  GError *error;
  GMarkupParseContext *context;
  
  error = NULL;
  if (!g_file_get_contents (filename,
                            &contents,
                            &length,
                            &error))
    {
      g_assert(FALSE && "c:\\1.gmarkup not found");
      fprintf (stderr, "%s\n", error->message);
      g_error_free (error);
      return 1;
    }

  context = g_markup_parse_context_new (&parser, 0, NULL, NULL);

  if (!g_markup_parse_context_parse (context, contents, length, NULL))
    {
      g_markup_parse_context_free (context);
      return 1;
    }

  if (!g_markup_parse_context_end_parse (context, NULL))
    {
      g_markup_parse_context_free (context);
      return 1;
    }

  g_markup_parse_context_free (context);

  if (test_in_chunks (contents, length, 24) != 0)
    return 1;
  
  return 0;
}

void g_markup_printf_escaped_test()
{
	const char *store = "Fortnum & Mason";
	const char *item = "Tea";
	char *output;
 
	output = g_markup_printf_escaped ("<purchase>"
                                  "<store>%s</store>"
                                  "<item>%s</item>"
                                  "</purchase>",
                                  store, item);
                                  
    g_assert(!strcmp(output,"<purchase><store>Fortnum &amp; Mason</store><item>Tea</item></purchase>"));
}

int
main (int   argc,
      char *argv[])
{
  
	#ifdef SYMBIAN
	
	g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
	g_set_print_handler(mrtPrintHandler);
	#endif /*SYMBIAN*/
  
	test_file ("c:\\1.gmarkup");
	g_markup_printf_escaped_test();
	
	#ifdef SYMBIAN
  	testResultXml("markup_test");
  	#endif /* EMULATOR */
	
	return 0;
}

