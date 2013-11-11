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
#include <stdlib.h>
#include <glib/gprintf.h>

#ifdef SYMBIAN
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/

void array_test()
{
	GArray *garray;
	gint i;
	int a = 1;
	int b[3] = 
	{
		4,5,6
	};
		
	garray = g_array_new (FALSE,FALSE,sizeof(gint));
	
	g_array_append_val(garray,a);
	g_array_append_val(garray,a);
	g_array_append_val(garray,a);
	
	g_array_insert_vals(garray,0,b,3);
	
    g_assert (g_array_index (garray, gint, 0) == 4);
    g_assert (g_array_index (garray, gint, 1) == 5);
    g_assert (g_array_index (garray, gint, 2) == 6);
	
	g_array_free(garray,TRUE);
	
}

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

void g_ascii_strup_test()
{
	gchar str[] = "ABCd";
	gchar *str1;
	str1 = g_ascii_strup(str,4);
	g_assert(str1[0] == 'A');
	g_assert(str1[1] == 'B');
	g_assert(str1[2] == 'C');
	g_assert(str1[3] == 'D');	
}

void g_fprintf_test()
{
	FILE *fp;
	char *teststring = "testing";
	int retVal;
	fp = fopen("c:\\test.txt","w");
	if(fp)
	{
		g_assert(g_fprintf(fp,"%s",teststring) == strlen(teststring));
		fclose(fp);
	}
}

void g_application_name_test()
{
	gchar *app_name = "Test App";
	g_set_application_name(app_name);
	g_assert(!strcmp(g_get_application_name(),app_name));
}

void g_listenv_test()
{
	gchar **e = NULL;
	int i;
	e = g_listenv();
	g_assert(e!=NULL);
	g_strfreev(e);
}

void g_direct_equal_test()
{
	int *i,a=1,*j;
	i = &a;
	j=i;
	g_assert(g_direct_equal(i,j));
	j++;
	g_assert(!g_direct_equal(i,j));
}

void g_direct_hash_test()
{
	int *i,a=1,j;
	int hash_value;
	i = &a;	
	j = (gint)i;
	g_assert(g_direct_hash(i) == j);
}

void g_bit_nth_lsf_test()
{
	gulong mask = 15;
	
	// 15 is 00000000 ........ 1111. Therefore the position of first 1 should be 0
	g_assert(g_bit_nth_lsf(mask,-1) == 0);
}

void g_bit_nth_msf_test()
{
	gulong mask = 15;
	
	// 15 is 00000000 ........ 1111. Therefore the position of last 1 should be 3
	g_assert(g_bit_nth_msf(mask,-1) == 3);
}

void g_basename_test()
{
	const gchar *filename;
	filename = g_basename("c:\\test\\test.txt");
	g_assert(!strcmp(filename,"test.txt"));
}


void function()
{
	return;
}

void g_atexit_test()
{
	g_atexit(function);
}

void g_bit_storage_test()
{
	g_assert(g_bit_storage(8) == 4);
}

void g_filename_display_basename_test()
{
	g_assert(!strcmp(g_filename_display_basename("c:/test/test.txt"),"test.txt"));
}

void g_find_program_in_path_test()
{
	char *program_name = g_find_program_in_path("misc_test.exe");
}

void g_atomic_test()
{
	int i = 5;
	int j;
	char *p,*q;
	
	j = g_atomic_int_get(&i);
	g_assert(j == i);
	
	p = g_malloc(1);
	q = g_atomic_pointer_get(&p);
	g_assert(p == q);
	
	g_free(p);
}

void g_error_test()
{
	GError err,*err_copy = NULL;
	err.domain = 1;
	err.code = 5;
	err.message = "test";
	err_copy = g_error_copy(&err);
	g_assert(err_copy->code == 5 && err_copy->code == 5 && !strcmp(err_copy->message,"test"));
	g_error_free(err_copy);
	err_copy = NULL;
	err_copy = g_error_new_literal(err.domain,err.code,"test is %s");
	g_assert(err_copy->code == 5 && err_copy->code == 5 && !strcmp(err_copy->message,"test is %s"));
	g_error_free(err_copy);
	err_copy = NULL;
}

static guint
my_hash (gconstpointer key)
{
  return (guint) *((const gint*) key);
}

static gboolean
my_hash_equal (gconstpointer a,
	       gconstpointer b)
{
  return *((const gint*) a) == *((const gint*) b);
}

static gboolean find_first     (gpointer key, 
				gpointer value, 
				gpointer user_data)
{
  gint *v = value; 
  gint *test = user_data;
  return (*v == *test);
}

gboolean func(gpointer key,gpointer value,gpointer user_data)
{
	gint *key1 = (int *)key;
	gint *value1 = (int *)value;
	gint *user_data1 = (int *)user_data;
	if(*key1 == *user_data1 && *value1 == *user_data1)
		return TRUE;
	else
		return FALSE;
}

void hash_test()
{
	GHashTable *hash_table;
	gint i;
	gint value = 4;
	gint *pvalue; 
	int array[10];
	int count = 0;

	hash_table = g_hash_table_new (my_hash, my_hash_equal);
	for (i = 0; i < 10; i++)
	{
	  array[i] = i;
	  g_hash_table_insert (hash_table, &array[i], &array[i]);
	}
	
	g_assert(g_hash_table_steal(hash_table,&value));
	
	pvalue = g_hash_table_find (hash_table, find_first, &value);
	
	//checks if g_hash_table_steal worked or not
	g_assert(pvalue == NULL);
	
	value = 5;
	
	pvalue = g_hash_table_find (hash_table, find_first, &value);
	
	count = g_hash_table_foreach_steal(hash_table,func,&value);
	
	pvalue = g_hash_table_find (hash_table, find_first, &value);
	
	g_assert(count == 1 && pvalue == NULL);

}

void check_version_test()
{
    const char *x = glib_check_version(2,8,3);
	g_assert(x == NULL);
}

int main (int   argc,
      char *argv[])
{
	#ifdef SYMBIAN
	g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
	#endif /*SYMBIAN*/
	
	array_test();
	g_ascii_strup_test();
	g_ascii_strdown_test();
	g_fprintf_test();
	g_application_name_test();
	g_listenv_test();
	g_direct_equal_test();
	g_direct_hash_test();
	g_bit_nth_lsf_test();
	g_bit_nth_msf_test();
	g_basename_test();
	g_atexit_test();
	g_bit_storage_test();
	g_filename_display_basename_test();
	g_find_program_in_path_test();
	g_atomic_test();
	g_error_test();
	hash_test();
	check_version_test();
	
	#ifdef SYMBIAN
  	testResultXml("misc_test");
  	#endif /* EMULATOR */
	
	return 0;
}
