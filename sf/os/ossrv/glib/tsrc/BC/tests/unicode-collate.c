/* Portion Copyright © 2008-09 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.*/
#undef G_DISABLE_ASSERT
#undef G_LOG_DOMAIN

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef SYMBIAN
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/

typedef struct {
  const char *key;
  const char *str;
} Line;
  

int 
compare_collate (const void *a, const void *b)
{
  const Line *line_a = a;
  const Line *line_b = b;

  return g_utf8_collate (line_a->str, line_b->str);
}

int 
compare_key (const void *a, const void *b)
{
  const Line *line_a = a;
  const Line *line_b = b;

  return strcmp (line_a->key, line_b->key);
}

gchar* sorted_res_arr[15] =
{
  "ABC",
  "Bart",
  "BART",
  "Bartlett",
  "CA",
  "can",
  "Canada",
  "CBS",
  "NBC",
  "next",
  "NeXT",
  "west",
  "West",
  "western",
   NULL
};

int main (int argc, char **argv)
{
  GIOChannel *in;
  GError *error = NULL;
  gchar *srcdir = getenv ("srcdir");
  gchar *testfile;
  GArray *line_array;
  guint i;
  
  #ifdef SYMBIAN
  g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
  g_set_print_handler(mrtPrintHandler);
  #endif /*SYMBIAN*/
  
  line_array = g_array_new (FALSE, FALSE, sizeof(Line));

  if (!srcdir)
	srcdir = "c:";

  testfile = g_strconcat (srcdir, G_DIR_SEPARATOR_S "casecollate.txt", NULL);

  in = g_io_channel_new_file (testfile, "r", &error);
  if (!in)
  {
  	g_print("Cannot open %s: %s\n", testfile, error->message);
  	
  	g_assert(FALSE && "unicode-collate failed");
  	
  	#ifdef SYMBIAN
  	testResultXml("unicode-collate");
  	#endif /* EMULATOR */	
	
	return 1;
  }

  while (TRUE)
    {
      gsize term_pos;
      gchar *str;
      Line line;
		gint keylen;
		gint strlen;

      if (g_io_channel_read_line (in, &str, NULL, &term_pos, &error) != G_IO_STATUS_NORMAL)
	break;

      str[term_pos] = '\0';

      line.key = g_utf8_collate_key (str, -1);
      line.str = str;

      g_array_append_val (line_array, line);
    }

  if (error)
    {
      g_print("Error reading test file, %s\n", error->message);
      
      g_assert(FALSE && "unicode-collate failed");
      
      #ifdef SYMBIAN
  	  testResultXml("unicode-collate");
  	  #endif /* EMULATOR */
  	  
      return 1;
    }

	qsort (line_array->data, line_array->len, sizeof (Line), compare_collate);
	for (i = 0; i < line_array->len; i++)
	{
		if(strcmp(sorted_res_arr[i], g_array_index (line_array, Line, i).str))
		{
			g_assert(FALSE && "compare_collate failed\n");
			
		}
	}

  qsort (line_array->data, line_array->len, sizeof (Line), compare_key);
	for (i = 0; i < line_array->len; i++)
	{
		if(strcmp(sorted_res_arr[i], g_array_index (line_array, Line, i).str))
		{
			g_assert(FALSE && "compare_key failed\n");
		}
	}

  g_io_channel_unref (in);
  
  #ifdef SYMBIAN
  testResultXml("unicode-collate");
  #endif /* EMULATOR */

  return 0;
}
