/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <glib/gtypes.h>
#include <glib/gerror.h>
#include <dbus/dbus-glib.h>

#define LOG_FILE "c:\\logs\\g_type_struct_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
	{
	if(result)
		assert_failed = 1;
	
	testResultXml("g_type_struct");
    close_log_file();
	}

gboolean get_member(GType type, gpointer instance, guint member, GValue *ret_value)
	{	
	GValueArray *va = (GValueArray*) instance;
	  const GValue *val;
	  if (member < dbus_g_type_get_struct_size (type))
	    {
	      val = g_value_array_get_nth (va, member);
	      g_value_copy (val, ret_value);
	      return TRUE;
	    }
	  else
	    return FALSE;
	}
	
gboolean set_member(GType type, gpointer instance, guint member, const GValue *new_value)
	{
	GValueArray *va = (GValueArray*) instance;
	  GValue *vp;
	  if (member < dbus_g_type_get_struct_size (type))
	    {
	      vp = g_value_array_get_nth (va, member);
	      g_value_copy (new_value, vp);
	      return TRUE;
	    }
	  else
	    return FALSE;
	}

gpointer  construct_struct(GType type)
	{
	  GValueArray *ret;
	  guint size = dbus_g_type_get_struct_size (type);
	  guint i;
	  ret = g_value_array_new (size);
	  for (i=0; i < size; i++)
	    {
	      GValue val = {0,};
	      g_value_init (&val, dbus_g_type_get_struct_member_type (type, i));
	      g_value_array_append(ret, &val);
	    }
	  return (gpointer)ret;
	}
	
int main()
{
	GValue* val1 = NULL;
	GType struct_type;
	
	gint num1, num = 25;
	gchar* str = "Glib Testing";
	gchar *str1 = (gchar *)malloc(20);
	gchar ch1, ch = 'M';
	gboolean bl1, bl = TRUE;
	guint num_u1, num_u = 1234;
	glong num_l1, num_l = -12345;
	gulong num_ul1, num_ul = 123456;
	gint64 num_641, num_64 = 1234567;
	guint64 num_u641, num_u64 = -1234567;
	gdouble dbl1, dbl = 1.2345;
	
	DBusGTypeSpecializedStructVtable ds_vtable;
	DBusGTypeSpecializedVtable base_vtable;
	
	base_vtable.constructor = construct_struct;
	base_vtable.copy_func = NULL;
	base_vtable.free_func = NULL;
	base_vtable.simple_free_func = NULL;
	
	ds_vtable.base_vtable = base_vtable;
	ds_vtable.get_member = get_member;
	ds_vtable.set_member = set_member;
	
	g_type_init();
	dbus_g_type_specialized_init();
	
	dbus_g_type_register_struct("struct_type", &ds_vtable, 0);
	
	struct_type = dbus_g_type_get_struct("struct_type", G_TYPE_INT, G_TYPE_STRING, G_TYPE_CHAR, G_TYPE_BOOLEAN, G_TYPE_UINT, G_TYPE_LONG, G_TYPE_ULONG, G_TYPE_INT64, G_TYPE_UINT64, G_TYPE_DOUBLE, G_TYPE_INVALID);
	
	val1 = g_new0 (GValue, 1);
	g_value_init(val1, struct_type);
	g_value_take_boxed(val1, dbus_g_type_specialized_construct(struct_type));
	
	if(!dbus_g_type_is_struct(struct_type))
		{
		std_log(LOG_FILENAME_LINE, "dbus_g_type_is_struct() failed.");
		create_xml(1);
		return 1;
		}
		
	if(!dbus_g_type_struct_set(val1, 0, num, 1, str, 2, ch, 3, bl, 4, num_u, 5, num_l, 6, num_ul, 7, num_64, 8, num_u64, 9, dbl, G_MAXUINT))
		{
		std_log(LOG_FILENAME_LINE, "Fail in dbus_g_type_struct_set()");
		create_xml(1);
		return 1;
		}

	if(!dbus_g_type_struct_get(val1, 0, &num1, 1, &str1, 2, &ch1, 3, &bl1, 4, &num_u1, 5, &num_l1, 6, &num_ul1, 7, &num_641, 8, &num_u641, 9, &dbl1, G_MAXUINT))
		{
		std_log(LOG_FILENAME_LINE, "Fail in dbus_g_type_struct_get()");
		create_xml(1);
		return 1;
		}
	else
		if(num!=num1 || strcmp(str, str1) || ch!=ch1 || bl!=bl1 || num_u!=num_u1 || num_l!=num_l1 || num_ul!=num_ul1 || num_64!=num_641 || num_u64!=num_u641 || dbl!=dbl1)
			{
			std_log(LOG_FILENAME_LINE, "num1 = %d, str = %s, ch1 = %c, bl1 = %d, num_u1 = %u, num_l1 = %d, num_ul1 = %u, num_641 = %ld, num_u641 = %u, dbl1 = %lf", num1, str1, ch1, bl1, num_u1, num_l1, num_ul1, num_641, num_u641, dbl1);
			create_xml(1);
			return 1;
			}
	std_log(LOG_FILENAME_LINE, "Test Successful");
	create_xml(0);
	return 0;
}

