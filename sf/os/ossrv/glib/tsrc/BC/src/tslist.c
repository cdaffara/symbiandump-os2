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
* Description:  ?Description
*
*/


#undef G_DISABLE_ASSERT
#undef G_LOG_DOMAIN


#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <fcntl.h>
#include <goption.h>

#ifdef SYMBIAN
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/

#define	C2P(c)		((gpointer) ((long) (c)))
#define GINT_TO_POINTER(i)	((gpointer)  (i))
#define GPOINTER_TO_INT(p)	((gint)   (p))
#define TESTPASS	1
#define TESTFAIL	0


//Ascending
gint compare_fun_gr(gconstpointer a,gconstpointer b)
{
	return ((*(int *)a==*(int *)b)?0:((*(int *)a>*(int *)b)?1:-1));
}

//Data
gint compare_fun_gr_data(gconstpointer a,gconstpointer b,gpointer data)
{
	return ((*(int *)a==*(int *)b)?0:((*(int *)a>*(int *)b)?1:-1));
}

// Tests for slist
void tg_slist_tests()
{
	GSList *slist,*st,*rem;
	gint nums[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	gint chk_buf[20];
	gint i;
	gint g_slist_insert_data;
	gint g_slist_insert_before_data;
	gint ip1 = 10;
	gint ip2 = 15;
	gint ip3 = 5;
	gint ip4 = 12;
	gint g_slist_nth_data_op,g_slist_find_custom_op;
	
	//Trying to use the allocators so that even they get tested!
	GAllocator* alloc = g_allocator_new ("alloc_slist",5000);
	g_slist_push_allocator (alloc);
	
	
	slist = NULL;
	for (i = 0; i < 10; i++)
		slist = g_slist_append (slist, &nums[i]);
	
	//List looks like:
	// 0   1   2   3   4   5   6   7   8   9
	
	//Test for g_slist_insert....inserted 10 at pos 4
	g_slist_insert(slist,&ip1,4);
	st = g_slist_nth (slist,0);
	for(i = 0;i < 4;i++)
		st = st->next;
	g_slist_insert_data = *((gint*) st->data);
	g_assert(g_slist_insert_data == 10);
	
/*	for (i = 0; i < 10; i++)
    {
      st = g_slist_nth (slist, i);
      chk_buf[i] = *((gint*) st->data);
    }*/
    
	//List looks like:
	// 0   1   2   3   10   4   5   6   7   8   9
	
	//Test for g_slist_insert_before....inserted 15 at pos 7
	st = g_slist_nth (slist,7);
	g_slist_insert_before(slist,st,&ip2);
	st = g_slist_nth (slist,0);
	for(i = 0;i < 7;i++)
		st = st->next;
	g_slist_insert_before_data = *((gint*) st->data);
	g_assert(g_slist_insert_before_data == 15);
	
	//List looks like:
	// 0   1   2   3   10   4   5   15   6   7   8   9
	
	//Test for g_slist_index....finding 15 at pos 7
	st = g_slist_nth (slist,0);
	g_assert(g_slist_index(st,&ip2)==7);

	//Test for g_slist_nth_data....getting 6 at position 8
	g_slist_nth_data_op = *((gint*) g_slist_nth_data(slist,8));
	g_assert(g_slist_nth_data_op == 6)	;

	//Test for g_slist_position
	st = g_slist_nth (slist,7);
	g_assert(g_slist_position (slist,st) == 7);

	//Test for g_slist_find_custom
	st = g_slist_find_custom(slist,&ip3,compare_fun_gr);
	g_slist_find_custom_op = *((gint*) st->data);
	g_assert(g_slist_find_custom_op == 5);
	
	//Test for g_slist_sort_with_data
	st = g_slist_sort_with_data(slist,compare_fun_gr_data,&ip3);
	for (i = 0; i < 10; i++)
    {
      st = g_slist_nth (slist, i);
      g_assert (*((gint*) st->data) == i);
    }

	//List looks like:
	// 0   1   2   3   4   5   6   7   8   9   10   15
	
	//Test for g_slist_remove_link
	st = g_slist_nth (slist, 5);
	rem = g_slist_remove_link(slist , st);
	st = g_slist_nth (slist, 5);
    g_assert (*((gint*) st->data) == 6);

	//List looks like:
	// 0   1   2   3   4   6   7   8   9   10   15

	//Test for g_slist_remove_all
	g_slist_insert(slist,&ip4,4);
	g_slist_insert(slist,&ip4,6);
	g_slist_insert(slist,&ip4,8);
	//List looks like:
	// 0   1   2   3   4   12   6   7   12   8   12   9   10   15
	g_slist_remove_all(slist ,&ip4);
    
	g_slist_free (slist);
	g_slist_pop_allocator ();
}


int main (int argc,char *argv[])
{

	#ifdef SYMBIAN
 	
 	g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO |  G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
 	#endif /*SYMBIAN*/
 	
 	tg_slist_tests();
 #ifdef SYMBIAN
  testResultXml("tslist");
#endif /* EMULATOR */
 	return 0;
}