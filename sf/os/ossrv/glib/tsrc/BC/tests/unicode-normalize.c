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

gboolean success = TRUE;

static char *decode (const gchar *input)
{
  unsigned ch;
  int offset = 0;
  GString *result = g_string_new (NULL);
  
  do 
    {
      if (sscanf (input + offset, "%x", &ch) != 1)
	{
	  g_print("Error parsing character string %s\n", input);
	  
	  g_assert(FALSE && "unicode-normalize failed");
	  
	  #ifdef SYMBIAN
  	  testResultXml("unicode-normalize");
  	  #endif /* EMULATOR */
  	  
	  exit (1);
    }

      g_string_append_unichar (result, ch);
      
      while (input[offset] && input[offset] != ' ')
	offset++;
      while (input[offset] && input[offset] == ' ')
	offset++;
    }
  while (input[offset]);
  return g_string_free (result, FALSE);
}

const char *names[4] = {
  "NFD",
  "NFC",
  "NFKD",
  "NFKC"
};

static void
test_form (int line,
	   GNormalizeMode mode,
	   gboolean       do_compat,
	   int            expected,
	   char         **c,
	   char         **raw)
{
  int i;
  
  gboolean mode_is_compat = (mode == G_NORMALIZE_NFKC ||
			     mode == G_NORMALIZE_NFKD);

  if (mode_is_compat || !do_compat)
    {
      for (i = 0; i < 3; i++)
	{
	  char *result = g_utf8_normalize (c[i], -1, mode);
	  if (strcmp (result, c[expected]) != 0)
	    {
	      g_print("\nFailure: %d/%d: %s\n", line, i + 1, raw[5]);
	      g_print("  g_utf8_normalize (%s, %s) != %s\n",
		   raw[i], names[mode], raw[expected]);
	      success = FALSE;
	    }
	  
	  g_free (result);
	}
    }
  if (mode_is_compat || do_compat)
    {
      for (i = 3; i < 5; i++)
	{
	  char *result = g_utf8_normalize (c[i], -1, mode);
	  if (strcmp (result, c[expected]) != 0)
	    {
	      g_print("\nFailure: %d/%d: %s\n", line, i, raw[5]);
	      g_print("  g_utf8_normalize (%s, %s) != %s\n",
		   raw[i], names[mode], raw[expected]);
	      success = FALSE;
	    }
	  
	  g_free (result);
	}
    }
}

static gboolean
process_one (int line, gchar **columns)
{
  char *c[5];
  int i;
  gboolean skip = FALSE;

  for (i=0; i < 5; i++)
    {
      c[i] = decode(columns[i]);
      if (!c[i])
	skip = TRUE;
    }

  if (!skip)
    {
      test_form (line, G_NORMALIZE_NFD, FALSE, 2, c, columns);
      test_form (line, G_NORMALIZE_NFD, TRUE, 4, c, columns);
      test_form (line, G_NORMALIZE_NFC, FALSE, 1, c, columns);
      test_form (line, G_NORMALIZE_NFC, TRUE, 3, c, columns);
      test_form (line, G_NORMALIZE_NFKD, TRUE, 4, c, columns);
      test_form (line, G_NORMALIZE_NFKC, TRUE, 3, c, columns);
    }

  for (i=0; i < 5; i++)
    g_free (c[i]);
  
  return TRUE;
}

int main (int argc, char **argv)
{
  GIOChannel *in;
  GError *error = NULL;
  GString *buffer;
  int line_to_do = 0;
  int line = 1;
  gchar *srcdir = getenv ("srcdir");
  gchar *testfile;
  
  #ifdef SYMBIAN
  g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
  g_set_print_handler(mrtPrintHandler);
  #endif /*SYMBIAN*/
	  
  buffer = g_string_new (NULL);

/*if (argc != 2 && argc != 3)
    {
      g_print("Usage: unicode-normalize NormalizationTest.txt LINE\n");
      return 1;
    }

  if (argc == 3)
    line_to_do = atoi(argv[2]);
*/

  if (!srcdir)
	srcdir = "c:";

  testfile = g_strconcat (srcdir, G_DIR_SEPARATOR_S "NormalizationTest.txt", NULL);
  
  in = g_io_channel_new_file (testfile, "r", &error);
  if (!in)
    {
      g_print("Cannot open %s: %s\n", testfile, error->message);
      g_assert(FALSE && "unicode-normalize failed");
      
      #ifdef SYMBIAN
      testResultXml("unicode-normalize");
      #endif /* EMULATOR */
      
      return 1;
    }

  while (TRUE)
    {
      gsize term_pos;
      gchar **columns;

      if (g_io_channel_read_line_string (in, buffer, &term_pos, &error) != G_IO_STATUS_NORMAL)
	break;
	
      //if (line_to_do && line != line_to_do)
	//goto next;
      
      buffer->str[term_pos] = '\0';
      
      if (buffer->str[0] == '#') /* Comment */
	goto next;
      if (buffer->str[0] == '@') /* Part */
	{
	  //g_print("\nProcessing %s\n",buffer->str + 1);
	  goto next;
	}
      
      columns = g_strsplit (buffer->str, ";", -1);
      if (!columns[0])
	goto next;
      
      if (!process_one (line, columns))
	return 1;
      g_strfreev (columns);

    next:
      g_string_truncate (buffer, 0);
      line++;
    }

  if (error)
    {
      g_print("Error reading test file, %s\n", error->message);
       
      g_assert(FALSE && "unicode-normalize failed");
	  
	  #ifdef SYMBIAN
  	  testResultXml("unicode-normalize");
  	  #endif /* EMULATOR */
      
      return 1;
    }

  g_io_channel_unref (in);
  g_string_free (buffer, TRUE);
    
  #ifdef SYMBIAN
  assert_failed = !success;
  testResultXml("unicode-normalize");
  #endif /* EMULATOR */

  return !success;
}
