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

//Test for g_nullify_pointer
void tg_nullify_pointer()
{
	char nullify_pointer[]="abcd";
	g_nullify_pointer((void*)nullify_pointer);
	g_assert(!strcmp(nullify_pointer,"\0"));
}

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


//Tests for g_ptr_array
void tg_ptr_array_tests()
{
	GPtrArray *gparray;
	int i;
	gint str_ds[12]=
	{
		12,11,10,9,8,7,6,5,4,3,2,1
	};
	
	gparray = g_ptr_array_new ();
	for(i=0;i<12;i++)
	{
		g_ptr_array_add (gparray, (gpointer)str_ds[i] );
	}
	
	g_ptr_array_sort(gparray,compare_fun_gr);
	g_ptr_array_remove_range(gparray,2,4);
	g_ptr_array_sort_with_data(gparray,compare_fun_gr_data,0);
	
	
}

int cmp_func(gconstpointer a,gconstpointer b)
{
	if(a==b)
		return 0;
	else
		return -1;
}

//Test for g_queue_find_custom
void tg_queue_find_custom()
{
	GQueue *q;
	GList *node;
	gpointer data;
	gpointer srch_data=GINT_TO_POINTER(5);
	int i;
	int j=10;
	int g_queue_find_custom_pass=TESTFAIL;
	
	q = g_queue_new ();
	for(i=0;i<10;i++)
	{
		g_queue_push_head (q, GINT_TO_POINTER (j));
		j--;
	}
	g_queue_push_nth(q,GINT_TO_POINTER (5),9);
	
	node= g_queue_find_custom(q,GINT_TO_POINTER (5),cmp_func);

	if(node->data==srch_data)
	{
		g_queue_find_custom_pass=TESTPASS;
	}
			
	g_assert(g_queue_find_custom_pass==TESTPASS);
}


//Test for g_timer_reset
void tg_timer_test()
{
	int i=0;
	GTimer *timer;
	timer = g_timer_new ();
	g_timer_start (timer);
	do
	{
		while (g_timer_elapsed (timer, NULL) < 1);
		g_timer_reset(timer);
		i++;
	}while(i<3);
		
	g_timer_stop (timer);
	g_timer_destroy (timer);
	
}

//Test for g_try_malloc0
void tg_try_malloc0()
{
	char* s;
	gpointer try = g_try_malloc0 (sizeof(s));
	g_assert (try != NULL);
}

int main (int argc,char *argv[])
{

	#ifdef SYMBIAN
 
 	g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO |  G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
 	#endif /*SYMBIAN*/
 	
 	tg_nullify_pointer();
	tg_ptr_array_tests();
	tg_queue_find_custom();
	tg_timer_test();
	tg_try_malloc0();
 
#ifdef SYMBIAN
  testResultXml("tmisc");
#endif /* EMULATOR */
 	return 0;
}