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

int result = 1;

#define LOG_FILE "c:\\logs\\g_type_map_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
	{
	if(result)
		assert_failed = 1;
	
	testResultXml("g_type_map");
    close_log_file();
	}

void call_iterator_for_hash_table(gpointer key, gpointer value, gpointer data)
	{
		GValue* key_val = (GValue*)key;
		GValue* value_val = (GValue*)value;
		DBusGTypeSpecializedMapIterator iter = (DBusGTypeSpecializedMapIterator)data;
		
		iter(key_val, value_val, NULL);
	}

void map_iterator_fun (GType type, gpointer instance, DBusGTypeSpecializedMapIterator iterator, gpointer user_data)
{
	GHashTable* hash_table = (GHashTable *)instance;
	
	g_hash_table_foreach(hash_table, call_iterator_for_hash_table, (gpointer)iterator);
}

void map_append_fun (DBusGTypeSpecializedAppendContext *ctx, GValue *key, GValue *val)
{
	GHashTable *hashTable = (GHashTable*)g_value_get_boxed(ctx->val);
	g_hash_table_insert(hashTable, key, val);
}

void  simple_free_fun  (gpointer       data)
{

}

gpointer  construct_map(GType type)
	{
	  GHashTable *ret;
	  
	  ret = g_hash_table_new(NULL, NULL);
	  return ret;
	}

void  iterator_map(const GValue *key_val, const GValue *value_val, gpointer user_data)
{
	const char *key;
	const char* val;
	int age_val;
	GValue* temp;
	
	FILE* fp = fopen("c:\\map.txt", "a+");
	if(!fp)
		printf("Not able to open map.txt");
	
	key = g_value_get_string(key_val);
	fprintf(fp, "%s  =  ", key);
	
	if(strcmp("age", key))
		{
		temp = (GValue *)g_value_get_boxed(value_val);
		val = g_value_get_string(temp);
		fprintf(fp, "%s\n", val);
		if(!strcmp("firstname", key))
			if(strcmp("Joe", val))
				result = 0;
		if(!strcmp("lastname", key))
			if(strcmp("Average", val))
				result = 0;
		}
	else
		{
		temp = (GValue *)g_value_get_boxed(value_val);
		age_val = g_value_get_int(temp);
		fprintf(fp, "%d\n", age_val);
		if(age_val != 35)
			result = 0;
		}
	fclose(fp);
}

int main()
{
	DBusGTypeSpecializedAppendContext  ctx;
	GValue                            *key       = NULL;
	GValue                            *value     = NULL;
	GValue                            *realValue = NULL;
	
	GValue* data;
	GType  map_type;
	GType key_type;
	GType val_type;
	int i;
	
	DBusGTypeSpecializedMapVtable map_vtable;
	DBusGTypeSpecializedVtable base_vtable;
	
	base_vtable.constructor = construct_map;
	base_vtable.copy_func = NULL;
	base_vtable.free_func = NULL;
	base_vtable.simple_free_func = simple_free_fun;
	
	map_vtable.base_vtable = base_vtable;
	map_vtable.append_func = map_append_fun;
	map_vtable.iterator = map_iterator_fun;
	
	g_type_init();
	dbus_g_type_specialized_init();
	
	dbus_g_type_register_map("GHashTable", &map_vtable, 0);
	map_type = dbus_g_type_get_map("GHashTable", G_TYPE_STRING, G_TYPE_VALUE);
	 
	data = g_new0(GValue, 1);
	
	//init our map
	g_value_init (data, map_type);
	g_value_take_boxed (data, dbus_g_type_specialized_construct (map_type));
	
	key_type = dbus_g_type_get_map_key_specialization(map_type);
	if(key_type != G_TYPE_STRING)
		{
		std_log(LOG_FILENAME_LINE, "key_type is not matching its %d", key_type);
		create_xml(1);
		return 1;
		}
	val_type = dbus_g_type_get_map_value_specialization(map_type);
	if(val_type != G_TYPE_VALUE)
		{
		std_log(LOG_FILENAME_LINE, "value_type is not matching its %d", val_type);
		create_xml(1);
		return 1;
		}
	
	//prepare to add key/value pairs to our map
	dbus_g_type_specialized_init_append (data, &ctx);
	for(i=0; i<5; i++)
		{
		//prepare and add the first key/value pair (firstName => Joe)
		key       = g_new0(GValue, 1);
		value     = g_new0(GValue, 1);
		realValue = g_new0(GValue, 1);
		g_value_init (key,       G_TYPE_STRING);
		g_value_init (value,     G_TYPE_VALUE);
		g_value_init (realValue, G_TYPE_STRING);
		g_value_set_string (key,       "firstName");
		g_value_set_string (realValue, "Joe");
		g_value_set_boxed  (value,     realValue);
		dbus_g_type_specialized_map_append(&ctx, key, value);
		 
		//prepare and add the second key/value pair (lastName => Average)
		key       = g_new0(GValue, 1);
		value     = g_new0(GValue, 1);
		realValue = g_new0(GValue, 1);
		g_value_init (key,       G_TYPE_STRING);
		g_value_init (value,     G_TYPE_VALUE);
		g_value_init (realValue, G_TYPE_STRING);
		g_value_set_string (key,       "lastName");
		g_value_set_string (realValue, "Average");
		g_value_set_boxed  (value,     realValue);
		dbus_g_type_specialized_map_append(&ctx, key, value);
		 
		//prepare and add the third key/value pair (age => 35)
		key       = g_new0(GValue, 1);
		value     = g_new0(GValue, 1);
		realValue = g_new0(GValue, 1);
		g_value_init (key,       G_TYPE_STRING);
		g_value_init (value,     G_TYPE_VALUE);
		g_value_init (realValue, G_TYPE_INT);
		g_value_set_string (key,       "age");
		g_value_set_int    (realValue, 35);
		g_value_set_boxed  (value,     realValue);
		dbus_g_type_specialized_map_append(&ctx, key, value);
		}
	dbus_g_type_map_value_iterate(data, iterator_map, NULL);
	
	if(result == 0)
		{
		std_log(LOG_FILENAME_LINE, "Expected values not coming while iterating");
		create_xml(1);
		return 1;
		}
	std_log(LOG_FILENAME_LINE, "Test Successful");
	create_xml(0);
	return 0;
}

