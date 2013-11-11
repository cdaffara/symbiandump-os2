/* Portion Copyright © 2008-09 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.*/
#undef G_DISABLE_ASSERT
#undef G_LOG_DOMAIN

#ifdef GLIB_COMPILATION
#undef GLIB_COMPILATION
#endif

#include "glib.h"

#include <stdio.h>
#include <string.h>
#include <locale.h>

#ifdef SYMBIAN
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/


void g_date_debug_print(GDate* d)
{
  if (!d) g_print("NULL!\n");
  else 
    g_print("julian: %u (%s) DMY: %u %u %u (%s)\n",
	    d->julian_days, 
	    d->julian ? "valid" : "invalid",
	    d->day,
	    d->month,
	    d->year,
	    d->dmy ? "valid" : "invalid");
  
  fflush(stdout);
}

/* These only work in the POSIX locale, maybe C too - 
 * type POSIX into the program to check them
 */
char* posix_tests [] = {
  "19981024",
  "981024",
  "October 1998",
  "October 98",
  "oCT 98",
  "10/24/98",
  "10 -- 24 -- 98",
  "10/24/1998",
  "October 24, 1998",
  NULL
};

int main(int argc, char** argv)
{
	GDate* d;
	gchar* loc;
	char** s = posix_tests;

	#ifdef SYMBIAN
	g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
	g_set_print_handler(mrtPrintHandler);
	#endif /*SYMBIAN*/
	  

	loc = setlocale(LC_ALL,"");

	d = g_date_new();
	
	while (*s) 
	{
		g_date_set_parse(d, *s);

		if (!g_date_valid(d))
		  {
		    g_print(" failed.\n");
		    g_assert(FALSE && "testgdateparser");
		  }
		else 
		  {
		    gchar buf[256];
		    
		    g_date_strftime(buf,100," parsed `%x' (%B %d %Y)\n",
		                    d);
		  }
		++s;
	}
	
	g_date_free(d);
#ifdef SYMBIAN
  testResultXml("testgdateparser");
#endif /* EMULATOR */
	return 0;
}


