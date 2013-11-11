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
#include <glib/gatomic.h>
#include <glib/galloca.h>
#include <glib/gprintf.h>
#include <glib-object.h>

#include <stdlib.h>


#ifdef SYMBIAN
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/

#define THREADS 10

typedef enum
{
  MY_ENUM_FOO,
  MY_ENUM_BAR,
} myEnum;

typedef enum
{
  MY_FLAG_FOO = 1 << 0,
  MY_FLAG_BAR = 1 << 1,
} myFlags;



#define	TEST(m,cond)	G_STMT_START { failed = !(cond); \
if (failed) \
  { ++notpassed; \
  	assert_failed = TRUE; \
    if (!m) \
      g_print ("\n(%s:%d) failed for: %s\n", __FILE__, __LINE__, ( # cond )); \
    else \
      g_print ("\n(%s:%d) failed for: %s: (%s)\n", __FILE__, __LINE__, ( # cond ), (gchar*)m); \
  } \
else \
  ++passed;    \
  if ((passed+notpassed) % 10000 == 0) /*g_print (".")*/; fflush (stdout); \
} G_STMT_END



void g_ascii_strdown_test()
{
	gchar str[] = "ABCd";
	gchar *str1;
	str1 = g_ascii_strdown(str,4);
	g_assert(str1[0] == 'a');
	g_assert(str1[1] == 'b');
	g_assert(str1[2] == 'c');
	g_assert(str1[3] == 'd');	
}

#if 0
void g_assert_warning_test()
{
	g_assert_warning(NULL, __FILE__,__LINE__,"g_assert_warning_test","'a'== 'a'");
	#ifdef SYMBIAN
  	testResultXml("extra_test");
  	#endif /* EMULATOR */
	/*following will abort!*/
	g_assert_warning(NULL, __FILE__,__LINE__,"g_assert_warning_test","'a'== 'b'");
}
#endif


static gpointer
atomic_int_thread (gpointer data)
{
	gint* val = (gint*)data;
	g_atomic_int_set ((gint *) data,
                  (*val)+1);

	return NULL;
}


static void
test_g_atomic_int_set ()
{
  GThread *threads[THREADS];
  guint i;
  
    
  for (i = 0; i < THREADS; i++)
    {
      int data;
      data = i;
      threads[i] = g_thread_create (atomic_int_thread, 
				    &data, TRUE, NULL);      
    }
  g_usleep (G_USEC_PER_SEC * 5);
  for (i = 0; i < THREADS; i++)
    {
      g_thread_join (threads[i]);
    }
}


static gpointer
atomic_pointer_thread (gpointer data)
{
	
	g_atomic_pointer_set ((gpointer*) &data,NULL);

	return NULL;
}


static void
test_g_atomic_pointer_set ()
{
  GThread *threads[THREADS];
  guint i;
  
    
  for (i = 0; i < THREADS; i++)
    {
      int data;
      data = i;
      threads[i] = g_thread_create (atomic_pointer_thread, 
				    &data, TRUE, NULL);      
    }
  g_usleep (G_USEC_PER_SEC * 5);
  for (i = 0; i < THREADS; i++)
    {
      g_thread_join (threads[i]);
    }
}


#if 0
void test_g_get_codeset()
{
	gchar* charset =  g_get_codeset ();
    g_assert(!strcmp(charset,"US-ASCII"));
    g_free(charset);
}
#endif


void test_g_blow_chunks()
{
	gchar *name = "chunk";
	GMemChunk *mem_chunk = g_mem_chunk_new(name,2,10,G_ALLOC_ONLY);
	g_blow_chunks();
}

void test_g_date_set_time_val()
{
  GDate* d = g_date_new();
  
  GTimeVal current_time;
  g_get_current_time (&current_time);
  g_date_set_time_val(d, &current_time);
  
  g_assert(g_date_valid(d));
  

}



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
	
	g_hook_append(&hooklist,hook1);
	g_hook_append(&hooklist,hook2);
	g_hook_append(&hooklist,hook3);
	g_hook_append(&hooklist,hook4);
	
	g_hook_list_invoke_check(&hooklist,FALSE);
	
	g_assert(data1 == 1 && data2 == 2 && data3 == 3 && data4 == 4);
	
	//now only func3 must be in hooklist
	data1 = 0,data2 = 0,data3 = 0,data4 = 0;
	g_hook_list_invoke_check(&hooklist,FALSE);
	
	//check for implemention behaviour as opposed to documented behaviour
	
	//enable this to check for documented behaviour
	//g_assert(data1 == 0 && data2 == 0 && data3 == 3 && data4 == 0);
	
	//disable this to stop checking implemented behaviour
	g_assert(data1 == 1 && data2 == 2 && data3 == 0 && data4 == 4);
	
	g_hook_list_clear(&hooklist);
	
}

void test_g_mem_chunk_alloc0()
{
	gchar *name = "chunk";
	char* pchar;
	GMemChunk *mem_chunk = g_mem_chunk_new(name,2,10,G_ALLOC_ONLY);
	gpointer data = g_mem_chunk_alloc0(mem_chunk);
	g_assert(data != NULL);
	pchar = (char*)data;
	g_assert( (*pchar) == '\0' && *(pchar+1) == '\0');
	g_mem_chunk_print(mem_chunk);
	g_mem_chunk_clean(mem_chunk);
	g_mem_chunk_destroy(mem_chunk);
}



gpointer theFunc(gpointer data)
{
	int* pval = (int*) data;
	(*pval)++;
	return NULL;
}

void test_gonce()
{
	GOnce onceObject;
	int val = 1;
	
	g_once_impl (&onceObject,theFunc, &val);
	g_once_impl (&onceObject,theFunc, &val);
	g_once_impl (&onceObject,theFunc, &val);
	g_assert(val == 2);
}

void test_g_return_if_fail_warning()
{
	//currently not exported 
	//g_return_if_fail_warning (NULL,"extra_tests::main","1== 1");
}

void test_g_slist_alloc()
{
	GSList*    pList =  g_slist_alloc();
	g_assert(pList != NULL);
	g_slist_free_1 (pList);
}

void test_g_string_insert_c()
{
	GString* string1 = g_string_new ("firstlast");
    g_string_insert_c (string1, 5, '_');
  	g_assert (strcmp (string1->str, "first_last") == 0);
  	g_string_free (string1, TRUE);
}

void test_g_strsignal()
{
	const gchar* errmsg = g_strsignal(0);
	g_assert(strcmp(errmsg, "unknown signal (0)")==0);	
}



void test_g_generictype_get_type()
{
	GType type_id, type_id2;
	int i;
	

	GType (*fnArray[])() =
	{
	g_closure_get_type,
	g_date_get_type,	
	g_gstring_get_type,	
	g_hash_table_get_type,
	g_io_channel_get_type,
	g_io_condition_get_type,
	g_strv_get_type,
	g_value_get_type,
	};
	
		
	
#define NumFns sizeof(fnArray)/sizeof(GType (*)())	
		
	for(i =0; i<NumFns;i++)
	{
		type_id =  fnArray[i]();
		g_assert(type_id != 0);
		type_id2 =  fnArray[i]();
		g_assert(type_id == type_id2);	
		//pInstance = g_type_create_instance(type_id);
		//g_assert(g_type_name(type_id) == g_type_name_from_instance(pInstance) );
	}
	
}

void test_enumClass()
{
	GType type_id = 0;
	GEnumClass* pPointer = NULL;
	GEnumValue* retrievedValue;
	static GEnumValue enum_array[] =
	{
		{ 0, "EZero", "zero"},
		{ 1, "EOne", "One"},
		{ 2, "ETwo", "Two"},
		{ 0, NULL, NULL},
	};

	//g_type_init();

	type_id =  g_enum_register_static("egEnum",enum_array);
	pPointer = g_type_class_ref(type_id);
	if(pPointer)
	{

		retrievedValue = g_enum_get_value(pPointer,1);
		g_assert(retrievedValue && retrievedValue->value == 1);
		retrievedValue = g_enum_get_value(pPointer,5);
		g_assert(retrievedValue == NULL);

		retrievedValue = g_enum_get_value_by_name(pPointer,"EOne");
		g_assert(retrievedValue && retrievedValue->value == 1);
		retrievedValue = g_enum_get_value_by_name(pPointer,"EFive");
		g_assert(retrievedValue == NULL);

		retrievedValue = g_enum_get_value_by_nick(pPointer,"One");
		g_assert(retrievedValue && retrievedValue->value == 1);
		retrievedValue = g_enum_get_value_by_nick(pPointer,"Five");
		g_assert(retrievedValue == NULL);

	}
}//fn


void test_flagsClass()
{
	GType type_id = 0;
	GFlagsClass* pPointer = NULL;
	GFlagsValue* retrievedValue;
	static GFlagsValue flags_array[] =
	{
		{ 1, "EOne", "One"},
		{ 2, "ETwo", "Two"},
		{ 4, "EFour", "Four"},
		{ 0, NULL, NULL},
	};

	//g_type_init();

	type_id =  g_flags_register_static("egFlags",flags_array);
	pPointer = g_type_class_ref(type_id);
	if(pPointer)
	{

		retrievedValue = g_flags_get_value_by_name(pPointer,"EOne");
		g_assert(retrievedValue && retrievedValue->value == 1);
		retrievedValue = g_flags_get_value_by_name(pPointer,"EFive");
		g_assert(retrievedValue == NULL);

		retrievedValue = g_flags_get_value_by_nick(pPointer,"One");
		g_assert(retrievedValue && retrievedValue->value == 1);
		retrievedValue = g_flags_get_value_by_nick(pPointer,"Five");
		g_assert(retrievedValue == NULL);
	}
}//fn



int main (int   argc,
      char *argv[])
{
	#ifdef SYMBIAN
	int handler = g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
	#endif /*SYMBIAN*/

	g_thread_init(NULL);	
	g_type_init();
	
	g_ascii_strdown_test();
    test_g_atomic_int_set();
    test_g_atomic_pointer_set();

	//test_g_get_codeset();    
	test_g_blow_chunks();
	test_g_date_set_time_val();
	hook_test();
	
	test_g_mem_chunk_alloc0();
	test_gonce();
	//test_g_return_if_fail_warning ();
	test_g_slist_alloc();
	test_g_string_insert_c();
	test_g_strsignal();
	
	test_g_generictype_get_type();
	
	test_enumClass();
 	test_flagsClass();
	
	
	
	//test 
	#ifdef SYMBIAN
	g_log_remove_handler (NULL, handler);
	g_warning("This test message should have been printed on console\n");
	g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
	#endif /*SYMBIAN*/
	
    
		
	//g_assert_warning_test();
	
	#ifdef SYMBIAN
  	testResultXml("extra_tests");
  	#endif /* EMULATOR */
	
	return 0;
}