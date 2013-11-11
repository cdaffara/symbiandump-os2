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

gboolean func1(int *data)
{
	*data = 1;
	return TRUE;
}

gboolean func2(int *data)
{
	*data = 2;
	return TRUE;
}

gboolean func3(int *data)
{
	*data = 3;
	return FALSE;
}

gboolean func4(int *data)
{
	*data = 4;
	return TRUE;
}

gint sort_func(GHook *new_hook,GHook *sibling)
{
	if(new_hook->hook_id < sibling->hook_id)
		return 0;
	else
		return 1;
}

gboolean find_func(GHook *hook,gpointer data)
{
	if(hook->hook_id == 1)
		return TRUE;
	else
		return FALSE;
}

void marshaller(GHook *hook,gpointer mashal_data)
{
	gint *data = (int *)hook->data;
	*data = -1;
}

gboolean check_marshaller(GHook *hook,gpointer mashal_data)
{
	if(hook->hook_id == 2) // for hook2 id is 2
		return FALSE;
	else
		return TRUE;
}


void hook_test()
{
	GHookList hooklist;
	GHook *hook1 = NULL,*hook2 = NULL,*hook3 = NULL,*hook4 = NULL,*temp_hook;
	int data1 = 0,data2 = 0,data3 = 0,data4 = 0;
	int comp_value;
	gboolean val;
	
	g_hook_list_init(&hooklist,sizeof(GHook));
	
	hook1 = g_hook_alloc(&hooklist);
	hook1->func = (gpointer)func1;
	hook1->data = &data1;
	
	hook2 = g_hook_alloc(&hooklist);
	hook2->func = (gpointer)func2;
	hook2->data = &data2;
	
	hook3 = g_hook_alloc(&hooklist);
	hook3->func = (gpointer)func3;
	hook3->data = &data3;
	
	hook4 = g_hook_alloc(&hooklist);
	hook4->func = (gpointer)func4;
	hook4->data = &data4;
	
	g_hook_append(&hooklist,hook4);
	g_hook_prepend(&hooklist,hook3);
	g_hook_insert_before(&hooklist,hook3,hook2);
	g_hook_insert_sorted(&hooklist,hook1,sort_func);
	
	g_hook_list_invoke(&hooklist,FALSE);
	
	// checks g_hook_list_init,g_hook_alloc,g_hook_append,g_hook_prepend,g_hook_insert_before,g_hook_insert_sorted
	g_assert(data1 == 1 && data2 == 2 && data3 == 3 && data4 == 4);
	
	comp_value = g_hook_compare_ids(hook2,hook1);
	
	//checks g_hook_compare_ids
	g_assert(comp_value < 0);
	
	temp_hook = g_hook_get(&hooklist,10);
	
	//checks g_hook_get
	g_assert(temp_hook == NULL);
	
	temp_hook = g_hook_get(&hooklist,1);
	
	//checks g_hook_get
	g_assert(temp_hook == hook4);
	
	temp_hook = NULL;
	
	temp_hook = g_hook_find(&hooklist,TRUE,find_func,NULL);
	
	//checks g_hook_find
	g_assert(temp_hook == hook4);
	
	temp_hook = NULL;
	
	temp_hook = g_hook_find_data(&hooklist,TRUE,&data1);
	
	//checks g_hook_find_data
	g_assert(temp_hook == hook1);
	
	temp_hook = NULL;
	
	temp_hook = g_hook_find_func(&hooklist,TRUE,(gpointer)func2);
	
	//checks g_hook_find_func
	g_assert(temp_hook == hook2);
	
	temp_hook = g_hook_find_func_data(&hooklist,TRUE,(gpointer)func2,&data1);
	
	//checks g_hook_find_func_data
	g_assert(temp_hook == NULL);
	
	temp_hook = g_hook_find_func_data(&hooklist,TRUE,(gpointer)func1,&data1);
	
	//checks g_hook_find_func_data
	g_assert(temp_hook == hook1);
		
	temp_hook = NULL;
	
	temp_hook = g_hook_ref(&hooklist,hook3);
	
	//checks g_hook_ref
	g_assert(hook3->ref_count == 2);
	
	g_hook_unref(&hooklist,hook3);
	
	//checks g_hook_unref
	g_assert(hook3->ref_count == 1);
	
	g_hook_list_marshal(&hooklist,TRUE,marshaller,NULL);
	
	//checks g_hook_list_marshal
	g_assert(data1 == -1 && data2 == -1 && data3 == -1 && data4 == -1);
	
	g_hook_list_marshal_check(&hooklist,TRUE,check_marshaller,NULL);
	
	// checks g_hook_list_marshal_check
	// func3 is for hook3 and the check_marshaller returns FALSE for hook3
	// As a rsult the hook is deleted from the hook list.
	g_assert(g_hook_find_func(&hooklist,TRUE,(gpointer)func3) == NULL);
	
	g_hook_list_clear(&hooklist);
	
	//checks g_hook_list_clear
	g_assert(hooklist.hooks == NULL);
}

int main (int   argc,
      char *argv[])
{
	#ifdef SYMBIAN
	
	
	g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
	#endif /*SYMBIAN*/
	
	hook_test();
	
	#if SYMBIAN
  	testResultXml("hook_test");
  	#endif /* EMULATOR */
	
	return 0;
	
}