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
#include <string.h>
#include "glib.h"

#ifdef SYMBIAN
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/

void function(gchar *key,gchar *value,gint *user_data)
{
	// give the count of the number of times the function was called
	(*user_data)++;
}

void cache_test()
{
	char *str1,*str2,*str3;
	GCache *cache = NULL;
	gint user_data = 0;
	
	g_assert((cache = g_cache_new( (GCacheNewFunc) g_ascii_strup,
					g_free, (GCacheDupFunc) g_strdup, g_free, g_str_hash, 
    				g_str_hash, g_str_equal)) != NULL);
    				
    str1 = g_cache_insert(cache,"test");
    
    g_assert(!strcmp("TEST",str1));
    
	str2 = g_cache_insert(cache,"test");

	g_assert(!strcmp("TEST",str1));
	
	str3 = g_cache_insert(cache,"glib");
	
	g_assert(!strcmp("GLIB",str3));
	
	g_cache_key_foreach (cache,(GHFunc)function,&user_data);
	
	//g_cache_key_foreach would call function twice and make user_data == 2
	g_assert(user_data == 2);
	
	g_cache_value_foreach (cache,(GHFunc)function,&user_data);
	
	//g_cache_value_foreach would call function twice and make user_data == 4
	g_assert(user_data == 4);
	
	g_cache_remove(cache,str1);
	g_cache_remove(cache,str2);	
	g_cache_remove(cache,str3);	
	
	g_cache_destroy(cache);
	
}


int main (int   argc,
      char *argv[])
{
	#ifdef SYMBIAN
	g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
	#endif /*SYMBIAN*/
	
	cache_test();
	
	#if SYMBIAN
  	testResultXml("cache_test");
  	#endif /* EMULATOR */
	
	return 0;
}