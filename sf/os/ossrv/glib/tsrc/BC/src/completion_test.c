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
#include <string.h>

#ifdef SYMBIAN
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/

int main (int argc, char *argv[])
{
	GCompletion *cmp;
	GList *items;
	gchar *prefix;

	#ifdef SYMBIAN
	g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
	#endif 

	cmp = g_completion_new (NULL);

	items = NULL;
	items = g_list_append (items, "Abcdef");
	items = g_list_append (items, "AbcDEF");
	items = g_list_append (items, "bc");
	items = g_list_append (items, "bd");
			
	g_completion_add_items (cmp, items);
	
	g_completion_set_compare(cmp,strncasecmp);
	
	items = g_completion_complete (cmp, "aB", &prefix);
	g_assert (!strcmp ("aBc", prefix));
	g_free (prefix);
		
	g_completion_remove_items(cmp,items);
	
	items = g_completion_complete (cmp, "aB", &prefix);
//	g_assert(items == NULL);
	
	items = g_completion_complete (cmp, "b", &prefix);
	g_assert(!strcmp("b",prefix));
	g_free(prefix);
	
	g_completion_clear_items(cmp);
	
	items = g_completion_complete (cmp, "b", &prefix);
	g_assert(items == NULL);
	
	#if SYMBIAN
  	testResultXml("completion_test");
  	#endif /* EMULATOR */	

	return 0;
}
