/* Portion Copyright © 2008-09 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.*/
#undef G_DISABLE_ASSERT
#undef G_LOG_DOMAIN

#include <stdio.h>
#include <glib.h>

#ifdef SYMBIAN
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/


static int depth = 0;

static void
indent (int extra)
{
  
}

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
  indent (0);
}

static void
text_handler           (GMarkupParseContext *context,
                        const gchar         *text,
                        gsize                text_len,
                        gpointer             user_data,
                        GError             **error)
{
  indent (0);
}


static void
passthrough_handler    (GMarkupParseContext *context,
                        const gchar         *passthrough_text,
                        gsize                text_len,
                        gpointer             user_data,
                        GError             **error)
{
  indent (0);
}

static void
error_handler          (GMarkupParseContext *context,
                        GError              *error,
                        gpointer             user_data)
{
  g_print(" %s\n", error->message);
  g_assert(FALSE && "markup-test failed");
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
  
  context = g_markup_parse_context_new (&parser, 0, NULL, NULL);
  if(!context)
  {
  	 g_print("markup-test.c : g_markup_parse_context_new failed @ line : %d",__LINE__);
  	 g_assert(FALSE && "markup-test failed");
  	 return 1;
  }

  while (i < length)
    {
      int this_chunk = MIN (length - i, chunk_size);

      if (!g_markup_parse_context_parse (context,
                                         contents + i,
                                         this_chunk,
                                         NULL))
        {
          g_print("markup-test.c : g_markup_parse_context_parse failed ");
          g_markup_parse_context_free (context);
          g_assert(FALSE && "markup-test failed");
          return 1;
        }

      i += this_chunk;
    }
      
  if (!g_markup_parse_context_end_parse (context, NULL))
    {
      g_print("markup-test.c : g_markup_parse_context_end_parse failed ");
      g_assert(FALSE && "markup-test failed");
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
      g_print("%s\n", error->message);
      g_error_free (error);
      return 1;
    }

  context = g_markup_parse_context_new (&parser, 0, NULL, NULL);
  if(!context)
  {
  	 g_print("markup-test.c : g_markup_parse_context_new failed @ line : %d",__LINE__);
  	 g_assert(FALSE && "markup-test failed");
  	 return 1;
  }
  
  if (!g_markup_parse_context_parse (context, contents, length, NULL))
    {
      g_print("markup-test.c : g_markup_parse_context_parse failed ");
      g_assert(FALSE && "markup-test failed");
      g_markup_parse_context_free (context);
      return 1;
    }

  if (!g_markup_parse_context_end_parse (context, NULL))
    {
      g_print("markup-test.c : g_markup_parse_context_end_parse failed ");
      g_assert(FALSE && "markup-test failed");
      g_markup_parse_context_free (context);
      return 1;
    }

  g_markup_parse_context_free (context);

  /* A byte at a time */
  if (test_in_chunks (contents, length, 1) != 0)
    return 1;

  /* 2 bytes */
  if (test_in_chunks (contents, length, 2) != 0)
    return 1;

  /*5 bytes */
  if (test_in_chunks (contents, length, 5) != 0)
    return 1;
  
  /* 12 bytes */
  if (test_in_chunks (contents, length, 12) != 0)
    return 1;
  
  /* 1024 bytes */
  if (test_in_chunks (contents, length, 1024) != 0)
    return 1;

  return 0;
}

int
main (int   argc,
      char *argv[])
{
  int retval;
  
  #ifdef SYMBIAN
  g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
  g_set_print_handler(mrtPrintHandler);
  #endif /*SYMBIAN*/


  retval = test_file ("c:\\1.gmarkup");
  
  #ifdef SYMBIAN
  testResultXml("markup-test");
  #endif /* EMULATOR */
  
  return retval;
}

