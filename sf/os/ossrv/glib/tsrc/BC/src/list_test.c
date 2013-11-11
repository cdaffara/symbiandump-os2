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


int main (int   argc,char *argv[])
{
	
	GList *list1 = NULL,*list2 = NULL,*l,*l1,*list3 = NULL;
	int i;
	int *value;
	const char mem_allocator[]  = "mem_allocator";
	GAllocator *allocator;
	
	int num1[] = 
	{
		1,2,3
	};
	
	int num2[] = 
	{
		4,5,6
	};
	
	#ifdef SYMBIAN
	
	g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
	#endif /*SYMBIAN*/
	
	allocator = g_allocator_new(mem_allocator,500);

	for(i=0;i<3;i++)
		list1 = g_list_append (list1, &num1[i]);
		
	for(i=0;i<3;i++)
		list2 = g_list_append (list2, &num2[i]);
	
	
	list1 = g_list_concat(list1,list2);
	
	for(i=0;i<6;i++)
	{
		l = g_list_nth(list1,i);
		g_assert(*(gint *)(l->data) == i+1);
	}
	
	list2 = g_list_copy(list1);
	
	for(i=0;i<3;i++)
	{
		l = g_list_nth(list2,i);
		g_assert(*(gint *)(l->data) == i+1);
	}
	
	l = g_list_first(list2);
	g_assert(*(gint *)(l->data) == 1);
	
	value = (int *)g_list_nth_data(list1,1);
	
	g_assert(*value == 2);
	
	l = g_list_nth(list1,3);
	
	l1 = g_list_nth_prev(l,2);

	for(i=0;i<5;i++)
	{
		l = g_list_nth(l1,i);
		g_assert(*(gint *)(l->data) == i+2);
	}
	
	g_list_push_allocator(allocator);
	
	list3 = g_list_append(list3,&num1[2]);
	
	g_assert(*(gint *)(list3->data) == 3);
	
	g_list_pop_allocator();
	
	list3 = g_list_append(list3,&num1[0]);
	
	g_assert(*(gint *)(list3->next->data) == 1);
	
	list1 = g_list_append(list1,&num1[0]);
	
	i = g_list_length(list1);
	
	list1 = g_list_remove_all(list1,&num1[0]);
	
	i = g_list_length(list1);
	
	g_assert(g_list_length(list1) == 5); // should be this value as we will remove 2 1's from the list
	
	for(i==0;i<g_list_length(list1);i++)
	{
		l = g_list_nth(list1,i);
		g_assert(*(gint *)(l->data) != 1);
	}
	
	g_allocator_free(allocator);
	
	#ifdef SYMBIAN
  	testResultXml("list_test");
  	#endif /* EMULATOR */
	
	return 0;
}