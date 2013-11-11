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

#include <glib.h>
#include <stdio.h>

#ifdef SYMBIAN
#include <glib_global.h>
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/



void test_error_check_mutexes()
{
  
  GThread *thread =NULL;
  GMutex* test_g_mutex_mutex = NULL;
  
  g_thread_init(NULL);	
  
  test_g_mutex_mutex = g_mutex_new ();

  g_assert (g_mutex_trylock (test_g_mutex_mutex));
  g_assert (g_mutex_trylock (test_g_mutex_mutex) == FALSE);
  g_mutex_unlock (test_g_mutex_mutex);
  g_mutex_free (test_g_mutex_mutex);
}





int main (int   argc,
      char *argv[])
{
	#ifdef SYMBIAN
	g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
	g_set_print_handler(mrtPrintHandler);
	#endif /*SYMBIAN*/


	
	
	test_error_check_mutexes();
	
	#ifdef SYMBIAN
  	testResultXml("error_check_mutexes");
  	#endif /* EMULATOR */
	
	return 0;
}