/* Portion Copyright © 2008-09 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.*/
#undef G_DISABLE_ASSERT
#undef G_LOG_DOMAIN

#include <stdarg.h>
#include <string.h>
#include <glib.h>
#include <stdio.h>

#ifdef SYMBIAN
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/


static void test_format (const gchar *format,
			 const gchar *expected, ...) G_GNUC_PRINTF (1, 3);

static gboolean error = FALSE;

static void
test (const gchar *original,
      const gchar *expected)
{
  gchar *result = g_markup_escape_text (original, -1);

  if (strcmp (result, expected) != 0)
    {
      g_print ("g_markup_escape_text(): expected '%s', got '%s'\n",
		  expected, result);
      error = TRUE;
      assert_failed = TRUE;
    }

  g_free (result);
}

static void
test_format (const gchar *format,
	     const gchar *expected,
	     ...)
{
  gchar *result;
  
  va_list args;
  
  va_start (args, expected);
  result = g_markup_vprintf_escaped (format, args);
  va_end (args);

  if (strcmp (result, expected) != 0)
    {
      g_print ("g_markup_printf_escaped(): expected '%s', got '%s'\n",
		  expected, result);
	  assert_failed = TRUE;
      error = TRUE;
    }

  g_free (result);
}

int main (int argc, char **argv)
{
  #ifdef SYMBIAN
  g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
  g_set_print_handler(mrtPrintHandler);
  #endif /*SYMBIAN*/
	  

  /* Tests for g_markup_escape_text() */
  test ("&", "&amp;");
  test ("<", "&lt;");
  test (">", "&gt;");
  test ("'", "&apos;");
  test ("\"", "&quot;");
  
  test ("", "");
  test ("A", "A");
  test ("A&", "A&amp;");
  test ("&A", "&amp;A");
  test ("A&A", "A&amp;A");
  test ("&&A", "&amp;&amp;A");
  test ("A&&", "A&amp;&amp;");
  test ("A&&A", "A&amp;&amp;A");
  test ("A&A&A", "A&amp;A&amp;A");
  
  /* Tests for g_markup_printf_escaped() */
  test_format ("A", "A");
  test_format ("A%s", "A&amp;", "&");
  test_format ("%sA", "&amp;A", "&");
  test_format ("A%sA", "A&amp;A", "&");
  test_format ("%s%sA", "&amp;&amp;A", "&", "&");
  test_format ("A%s%s", "A&amp;&amp;", "&", "&");
  test_format ("A%s%sA", "A&amp;&amp;A", "&", "&");
  test_format ("A%sA%sA", "A&amp;A&amp;A", "&", "&");
  
  test_format ("%s", "&lt;B&gt;&amp;",
	       "<B>&");
  test_format ("%c%c", "&lt;&amp;",
	       '<', '&');
  test_format (".%c.%c.", ".&lt;.&amp;.",
	       '<', '&');
  test_format ("%s", "",
	       "");
  test_format ("%-5s", "A    ",
	       "A");
  test_format ("%2$s%1$s", "B.A.",
	       "A.", "B.");
	       

  #ifdef SYMBIAN
  testResultXml("markup-escape-test");
  #endif /* EMULATOR */

  return error ? 1 : 0;
}
