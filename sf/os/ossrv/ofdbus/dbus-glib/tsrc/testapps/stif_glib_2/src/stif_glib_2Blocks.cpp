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





// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "stif_glib_2.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <glib/gtypes.h>
#include <glib/gerror.h>
#include <dbus/dbus-glib.h>
#include <pthread.h>

int result = 1;

_LIT(KSuccess, "Test Successful");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Cstif_glib_2::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Cstif_glib_2::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Cstif_glib_2::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Cstif_glib_2::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "stif_g_proxy_interface0", Cstif_glib_2::stif_Proxy_Interface0 ),
        ENTRY( "stif_g_proxy_path0", Cstif_glib_2::stif_Proxy_Path0 ),
        ENTRY( "stif_g_type_struct0", Cstif_glib_2::stif_Type_Struct0 ),
        ENTRY( "stif_g_type_map0", Cstif_glib_2::stif_Type_Map0 ),
        ENTRY("stif_g_Proxy_get_bus_name0", Cstif_glib_2::stif_Proxy_get_bus_name0),
        ENTRY("stif_g_Proxy_get_bus_name1", Cstif_glib_2::stif_Proxy_get_bus_name1),
        ENTRY("stif_Proxy_new_proxy0", Cstif_glib_2::stif_Proxy_new_proxy0),
        ENTRY("stif_dbus_connection_get_g_type0",Cstif_glib_2::stif_dbus_connection_get_g_type0),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// Cstif_glib_2::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
int iface_flag = 0;

static void* set_iface(void* data)
	{
	const char* iface1 = "Test.Proxy.Interface1";
	DBusGProxy* proxy = (DBusGProxy* )data;
	
	dbus_g_proxy_set_interface(proxy, iface1); 
	
	if(strcmp(iface1, dbus_g_proxy_get_interface(proxy)))
		{
		printf("Fail to check interface %s", iface1);
		}
	iface_flag = 1;
	return NULL;
	}


TInt Cstif_glib_2::stif_Proxy_Interface0( CStifItemParser& aItem )
{
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  

	DBusGConnection* connection;
	DBusGProxy* proxy;
	DBusGProxy* proxy1 ;//=NULL;
	GError* error = NULL;
	
	pthread_t thr_id;
	void* thrPtr;
	
	char* iface = "Test.Proxy.Interface";
	char* iface1 = "Test.Proxy.Interface1";
	
	g_type_init();
	
	connection = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
	if (connection == NULL)
		{
		  g_printerr ("Failed to open connection to bus: %s\n",
		              error->message);
		  g_error_free (error);
		  return 1;
		}
	
	proxy = dbus_g_proxy_new_for_name(connection, "Test.Glib.Proxy.Interface", "/Test/Proxy/Interface", iface);
	proxy1 = dbus_g_proxy_new_for_name(connection, "Test.Glib.Proxy.Interface", "/Test/Proxy/Interface", iface1);
	
	if(strcmp(iface, dbus_g_proxy_get_interface(proxy)))
		{
		iLog->Log(_L8("Fail to check interface %s"), iface);
		return 1;
		}
	
	pthread_create(&thr_id, NULL, &set_iface, proxy);
	pthread_join(thr_id, &thrPtr);
	
	if(strcmp(iface1, dbus_g_proxy_get_interface(proxy)))
		{
		iLog->Log(_L8("Fail to check interface %s"), iface1);
		return 1;
		}
	if(!iface_flag)
		{
		iLog->Log(_L8("Something wrong happens in thread."));
		return 1;
		}
	
	iLog->Log(KSuccess);
    return KErrNone;
}

TInt Cstif_glib_2::stif_Proxy_Path0( CStifItemParser& aItem )
{
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  

	DBusGConnection* connection;
	DBusGProxy* proxy;
	GError* error = NULL;
	
	char* path = "/Test/Proxy/Path";
	
	g_type_init();
	
	connection = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
	if (connection == NULL)
		{
		  g_printerr ("Failed to open connection to bus: %s\n",
		              error->message);
		  g_error_free (error);
		  return 1;
		}
	
	proxy = dbus_g_proxy_new_for_name(connection, "Test.Glib.Proxy.Path", path, "Test.Proxy.Interface");
	
	if(strcmp(path, dbus_g_proxy_get_path(proxy)))
		{
		iLog->Log(_L8("Fail to check interface %s"), path);
		return 1;
		}
	
	iLog->Log(KSuccess);
    return KErrNone;
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

TInt Cstif_glib_2::stif_Type_Struct0( CStifItemParser& aItem )
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
		iLog->Log(_L8("dbus_g_type_is_struct() failed."));
		return 1;
		}
		
	if(!dbus_g_type_struct_set(val1, 0, num, 1, str, 2, ch, 3, bl, 4, num_u, 5, num_l, 6, num_ul, 7, num_64, 8, num_u64, 9, dbl, G_MAXUINT))
		{
		iLog->Log(_L8("Fail in dbus_g_type_struct_set()"));
		return 1;
		}

	if(!dbus_g_type_struct_get(val1, 0, &num1, 1, &str1, 2, &ch1, 3, &bl1, 4, &num_u1, 5, &num_l1, 6, &num_ul1, 7, &num_641, 8, &num_u641, 9, &dbl1, G_MAXUINT))
		{
		iLog->Log(_L8("Fail in dbus_g_type_struct_get()"));
		return 1;
		}
	else
		if(num!=num1 || strcmp(str, str1) || ch!=ch1 || bl!=bl1 || num_u!=num_u1 || num_l!=num_l1 || num_ul!=num_ul1 || num_64!=num_641 || num_u64!=num_u641 || dbl!=dbl1)
			{
			iLog->Log(_L8("num1 = %d, str = %s, ch1 = %c, bl1 = %d, num_u1 = %u, num_l1 = %d, num_ul1 = %u, num_641 = %ld, num_u641 = %u, dbl1 = %lf"), num1, str1, ch1, bl1, num_u1, num_l1, num_ul1, num_641, num_u641, dbl1);
			return 1;
			}
	iLog->Log(KSuccess);
	return KErrNone;
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

TInt Cstif_glib_2::stif_Type_Map0( CStifItemParser& aItem )
{
	DBusGTypeSpecializedAppendContext  ctx;
	GValue                            *key       = NULL;
	GValue                            *value     = NULL;
	GValue                            *realValue = NULL;
	
	GValue* data;
	GType  map_type;
	GType key_type;
	GType val_type;
	
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
		iLog->Log(_L8("key_type is not matching its %d"), key_type);
		return 1;
		}
	val_type = dbus_g_type_get_map_value_specialization(map_type);
	if(val_type != G_TYPE_VALUE)
		{
		iLog->Log(_L8("value_type is not matching its %d"), val_type);
		return 1;
		}
	
	//prepare to add key/value pairs to our map
	dbus_g_type_specialized_init_append (data, &ctx);
	for(int i=0; i<5; i++)
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
		iLog->Log(_L8("Expected values not coming while iterating"));
		return 1;
		}
	iLog->Log(KSuccess);
	return KErrNone;
}

TInt Cstif_glib_2::stif_Proxy_get_bus_name0( CStifItemParser& aItem )
	{
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  

		DBusGConnection* connection;
		DBusGProxy* proxy;
		GError* error = NULL;
		const char*     proxy_get_name;
		
		char* path = "/Test/Proxy/Path";
		
		const char* name_bus = "Test.Glib.Proxy.Path1";
		int compare =0;
		g_type_init();
		 
		connection = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
		if (connection == NULL)
			{
			  g_printerr ("Failed to open connection to bus: %s\n",
			              error->message);
			  g_error_free (error);
			  return 1;
			}
		
		proxy = dbus_g_proxy_new_for_name(connection, "Test.Glib.Proxy.Path1", path, "Test.Proxy.Interface");
		
		proxy_get_name = dbus_g_proxy_get_bus_name (proxy);
		if(proxy_get_name != NULL)
		{
			
			iLog->Log(_L8("name_bus:%s"),proxy_get_name);
		}
		
			compare = strcmp(name_bus,proxy_get_name);
	
		if(compare == 0)
			{
			iLog->Log(_L8(" the bus name : %s"),proxy_get_name); 
			iLog->Log(KSuccess); 
			g_object_unref(proxy);
		    return KErrNone;
			}
		iLog->Log(_L8("FAIL")); 
		g_object_unref(proxy);
		return 1;
		
		
		
	}


TInt Cstif_glib_2::stif_Proxy_get_bus_name1( CStifItemParser& aItem )
	{
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  

		DBusGConnection* connection;
		DBusGProxy* proxy;
		GError* error = NULL;
		const char*     proxy_get_name;
		
		char* path = "/Test/Proxy/Path";
		
		//const char* name_bus = "Test.Glib.Proxy.Path1";
		int compare =0;
		g_type_init();
		 
		connection = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
		if (connection == NULL)
			{
			  g_printerr ("Failed to open connection to bus: %s\n",
			              error->message);
			  g_error_free (error);
			  return 1;
			}
		
		proxy = dbus_g_proxy_new_for_peer(connection, "Test.Glib.Proxy.Path", "Test.Proxy.Interface");
		
		proxy_get_name = dbus_g_proxy_get_bus_name (proxy);
		if(proxy_get_name == NULL)
			{
			iLog->Log(_L8("Proxy_get_name returns NULL as the proxy is obtained by dbus_g_proxy_new_for_peer"));

			iLog->Log(KSuccess);
			g_object_unref(proxy);
		    return KErrNone;
			}
		else 
			iLog->Log(_L8("FAIL"));
			g_object_unref(proxy);
			return 1;  
			
		
		
	}

TInt Cstif_glib_2::stif_Proxy_new_proxy0( CStifItemParser& aItem )
	{
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  

		DBusGConnection* connection;
		DBusGProxy* proxy;
		DBusGProxy* New_proxy;
		GError* error = NULL;
		
		char* path = "/Test/Proxy/Path";
		
		g_type_init();
		 
		connection = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
		if (connection == NULL)
			{
			  g_printerr ("Failed to open connection to bus: %s\n",
			              error->message);
			  g_error_free (error);
			  return 1;
			}
		proxy = dbus_g_proxy_new_for_name(connection, "Test.Glib.Proxy.Path1", path, "T   est.Proxy.Interface");
		
		
		New_proxy = dbus_g_proxy_new_from_proxy (proxy,"Test.Proxy.Interface",path);
		
		if(New_proxy==NULL)
			{

			iLog->Log(_L8("FAIL")); 
			return 1;
			}
		
		iLog->Log(_L8("New proxy is been created from the existing proxy"));
		
		
	g_object_unref(New_proxy);
	g_object_unref(proxy);

			iLog->Log(KSuccess);
		    return KErrNone;
		    
	}


TInt Cstif_glib_2::stif_dbus_connection_get_g_type0( CStifItemParser& aItem )
	{
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests ); 
	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  

		DBusGConnection* connection;
		DBusGProxy* proxy;
		GError* error = NULL;
		GType message_get;
		
		g_type_init();
		 
		connection = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
		if (connection == NULL)
			{
			  g_printerr ("Failed to open connection to bus: %s\n",error->message);
			  g_error_free (error);
			  return 1; 
			}
		message_get=dbus_g_connection_get_g_type();
		if(message_get == NULL)
			{
				iLog->Log(_L8("Fail"));
				g_object_unref(proxy);
				return 1;
			}
				
				iLog->Log(_L8("%ld"),message_get);
				iLog->Log(KSuccess);
			
			    return KErrNone;
			
	 }
	 
	 