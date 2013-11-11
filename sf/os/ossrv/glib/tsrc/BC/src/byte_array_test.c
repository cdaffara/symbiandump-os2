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

int sort_func(gconstpointer _a,gconstpointer _b,gpointer _user_data)
{
	const guint8 *a = _a;
	const guint8 *b = _b;
	
	gint *user_data = _user_data;
	
	if(*user_data == 1)
	{
		if((*a) > (*b))
			return -1;
		else if((*a) == (*b))
			return 0;
		else
			return 1;
	}
	else
	{
		if((*a) < (*b))
			return -1;
		else if(*a == *b)
			return 0;
		else
			return 1;	
	}
}

int ascending(gconstpointer _a,gconstpointer _b)
{
	const guint8 *a = _a;
	const guint8 *b = _b;
	
	if((*a) < (*b))
		return -1;
	else if(*a == *b)
		return 0;
	else
		return 1;
	
}

int main (int   argc,
      char *argv[])
{
	GByteArray *gbarray;
	gint i;
	int user_data = 1;
	
	#ifdef SYMBIAN
	g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
	#endif /*SYMBIAN*/

	gbarray = g_byte_array_new ();
	
	g_byte_array_prepend(gbarray,(guint8 *)"c",1);
	g_byte_array_prepend(gbarray,(guint8 *)"b",1);
	g_byte_array_prepend(gbarray,(guint8 *)"a",1);
	
	g_assert(gbarray->data[0] == 'a');
	g_assert(gbarray->data[1] == 'b');
	g_assert(gbarray->data[2] == 'c');
	
	g_byte_array_remove_index(gbarray,1);
	
	g_assert(gbarray->data[0] == 'a');
	g_assert(gbarray->data[1] == 'c');
	
	g_byte_array_append(gbarray,(guint8 *)"b",1);
	
	g_byte_array_remove_index_fast(gbarray,1);
	
	g_assert(gbarray->data[1] == 'b');
	
	g_byte_array_append(gbarray,(guint8 *)"c",1);
	
	g_byte_array_append(gbarray,(guint8 *)"d",1);
	
	g_byte_array_append(gbarray,(guint8 *)"e",1);
	
	g_byte_array_remove_range(gbarray,0,3);
	
	g_assert(gbarray->data[0] == 'd');
	g_assert(gbarray->data[1] == 'e');
	
	g_byte_array_set_size(gbarray,10);
	
	g_assert(gbarray->len == 10);
	
	g_byte_array_free(gbarray,TRUE);
	
	gbarray = g_byte_array_sized_new (10);
	
	g_assert(gbarray->len == 0);
	
	g_byte_array_append(gbarray,(guint8 *)"c",1);
	g_byte_array_append(gbarray,(guint8 *)"b",1);
	g_byte_array_append(gbarray,(guint8 *)"a",1);
	
	g_byte_array_sort(gbarray,ascending);
	
	g_assert(gbarray->data[0] == 'a');
	g_assert(gbarray->data[1] == 'b');
	g_assert(gbarray->data[2] == 'c');
	
	g_byte_array_sort_with_data(gbarray,sort_func,&user_data);
	
	g_assert(gbarray->data[0] == 'c');
	g_assert(gbarray->data[1] == 'b');
	g_assert(gbarray->data[2] == 'a');
	
	g_byte_array_free(gbarray,TRUE);
	
	#if SYMBIAN
  	testResultXml("byte_array_test");
  	#endif /* EMULATOR */		
	
	return 0;
}