/* Portion Copyright © 2008-09 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.*/
#undef G_DISABLE_ASSERT
#undef G_LOG_DOMAIN

#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <glib.h>
#include <string.h>

#ifdef SYMBIAN
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/


int main (int argc, char **argv)
{
  FILE *infile;
  char buffer[1024];
  char **strings;
  char *srcdir = getenv ("srcdir");
  char *filename;
  const char *locale;
  const char *test;
  char *convert;
#define true 1
#define false 0  
 char passed = true;
 int lines = 0;
   
  
  
  char *current_locale = setlocale (LC_CTYPE, NULL);
  gint result = 0;

	#ifdef SYMBIAN

  g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
  g_set_print_handler(mrtPrintHandler);
  #endif /*SYMBIAN*/
	  

  if (!srcdir)
    srcdir = "c:";
  filename = g_strconcat (srcdir, G_DIR_SEPARATOR_S, "casemap.bin", NULL);
  
  infile = fopen (filename, "r");
  if (!infile)
    {
      g_print("Failed to open %s\n", filename );
      g_assert(FALSE && "unicode-caseconv failed");
      
      #ifdef SYMBIAN
  	  testResultXml("unicode-caseconv");
  	  #endif /* EMULATOR */
      
      exit (1);
    }
  
   while (fgets (buffer, sizeof(buffer), infile))
    {
    lines++;
      if (buffer[0] == '#')
	continue;

      strings = g_strsplit (buffer, "\t", -1);

      locale = strings[0];

      if (!locale[0])
	locale = "C";
	
      if (strcmp (locale, current_locale) != 0)
	{
	  setlocale (LC_CTYPE, locale);
	  current_locale = setlocale (LC_CTYPE, NULL);

	  if (strncmp (current_locale, locale, 2) != 0)
	    {
	      g_print("Cannot set locale to %s, skipping\n", locale);
	      g_assert(FALSE && "unicode-caseconv failed");
	      goto next;
	    }
	}
      
      test = strings[1];

      convert = g_utf8_strup (test, -1);
      if (strcmp (convert, strings[4]) != 0)
	{
	  g_print("Failure: strup of line %d of casemap.bin toupper(%s) == %s, should have been %s\n",
		   lines,test, convert, strings[4]);
		   passed = false;
	  //g_assert(FALSE && "unicode-caseconv failed");
	  result = 1;
	}
      g_free (convert);

      convert = g_utf8_strdown (test, -1);
      if (strcmp (convert, strings[2]) != 0)
	{
	  g_print("Failure: strdown of line %d of casemap.bin tolower(%s) == %s, should have been %s\n",
		  lines,test, convert, strings[2]);
	  //g_assert(FALSE && "unicode-caseconv failed");
	  passed = false;
	  result = 1;
	}
      g_free (convert);

    next:
      g_strfreev (strings);
    }

  fclose (infile);

  g_free (filename);
  
  g_assert(passed && "unicode-caseconv failed");
  passed = true;
  filename = g_strconcat (srcdir, G_DIR_SEPARATOR_S, "casefold.bin", NULL);
  
  infile = fopen (filename, "r");
  if (!infile)
    {
      g_print("Failed to open %s\n", filename );
	  g_assert(FALSE && "unicode-caseconv failed");
	  
	  #ifdef SYMBIAN
  	  testResultXml("unicode-caseconv");
      #endif /* EMULATOR */
      
      exit (1);
    }
  
  lines = 0;
  while (fgets(buffer, sizeof(buffer), infile))
    {
     lines++;
      if (buffer[0] == '#')
	continue;

      buffer[strlen(buffer) - 1] = '\0';
      strings = g_strsplit (buffer, "\t", -1);

      test = strings[0];

      convert = g_utf8_casefold (test, -1);
      if (strcmp (convert, strings[1]) != 0)
	{
	  g_print("Failure: line %d casefold(%s) == '%s', should have been '%s'\n",
		   lines,test, convert, strings[1]);
	  //g_assert(FALSE && "unicode-caseconv failed");
	  passed = false;
	  result = 1;
	}
      g_free (convert);

      g_strfreev (strings);
    }

  fclose (infile);
  
  g_assert(passed && "unicode-caseconv failed");
  
  #ifdef SYMBIAN
  testResultXml("unicode-caseconv");
  #endif /* EMULATOR */

  return result;
}
