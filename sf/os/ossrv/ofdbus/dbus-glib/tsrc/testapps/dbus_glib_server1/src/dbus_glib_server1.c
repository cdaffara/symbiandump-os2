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
#include "dbus_glib_server1.h"
#include "dbus_glib1-glue.h"
#include <dbus/dbus-glib-bindings.h>
#include <errno.h>

static GObjectClass *parent_class = NULL;
static void dbus_glib_object1_init (DBusGlibObject1 *self);

gboolean dbus_glib_object1_variable_args_method_impl (DBusGlibObject1 *self, gboolean     b,        
		gshort		 n,
		gint         i,
        gint64       x,
        gushort		 q,
        guint        u,
        guint64      t,
        gdouble      d,
        gchar*		 s,
        GArray* 	 ai,
        gint*		 ret,
        GError **error)
{
	if(!b)
	{
		*ret = 1;
		return TRUE;
	}
	
	if(n != -16)
		{
			*ret = 3;
			return TRUE;
		}
	if(i != -32)
		{
			*ret = 3;
			return TRUE;
		}

	if(x != -64)
		{
			*ret = 4;
			return TRUE;
		}
	
	if(q != 16)
		{
			*ret = 6;
			return TRUE;
		}
	if(u != 32)
		{
			*ret = 6;
			return TRUE;
		}
	if(t != 64)
		{
			*ret = 7;
			return TRUE;
		}
	if(d != 1.234567)
		{
			*ret = 8;
			return TRUE;
		}
	if(strcmp("DBusGlibTesting~!@#$%^&*()_+`1234567890-=", s))
		{
			*ret = 9;
			return TRUE;
		}
	
	
	*ret =0;
	return TRUE;
}

gboolean dbus_glib_object1_simple_method_impl (DBusGlibObject1 *self, 
		gint         in_num,        
        gint*		 out_num,
        GError **error)
{	
	*out_num = 9090;
	return TRUE;
}

gboolean dbus_glib_object1_struct_example_method_impl (DBusGlibObject1 *self, 
		GValueArray*         st,        
        gint*		 ret,
        GError **error)
{	
	GValue *val;    
	
	val = g_value_array_get_nth(st, 0);
	  
	if (G_VALUE_HOLDS(val, G_TYPE_INT)) 
		if (g_value_type_compatible (G_VALUE_TYPE (val), G_TYPE_INT))
	    	if(g_value_get_int(val) != 1234)
	              *ret = -1;
	    	
	val = g_value_array_get_nth(st, 1);
  
	if (G_VALUE_HOLDS(val, G_TYPE_STRING))
    	if (g_value_type_compatible (G_VALUE_TYPE (val), G_TYPE_STRING))
    		if(strcmp("StructPassing", g_value_get_string(val)))
    				*ret = -1;
	
    if(*ret != -1)
    	*ret = 5050;
    
	return TRUE;
}

gboolean dbus_glib_object1_variant_send_method_impl(DBusGlibObject1 *self, 
		GValue*         in_val, 
		GValue*         out_val, 
        GError **error)
	{
		g_value_init (out_val, G_TYPE_STRING);
		g_value_copy(in_val, out_val);
		return TRUE;
	}

gboolean iter_func(gpointer key, gpointer value, gpointer user_data)
{
	FILE* fp = fopen("c:\\dict.txt", "a+");
	if(!fp)
		printf("Not able to open file.");
	
	fprintf(fp, "\nkey=%s value=%s", key, value);
	if(fp)
		fclose(fp);
	return TRUE;
}

gboolean dbus_glib_object1_dict_send_method_impl(DBusGlibObject1 *self, 
		GHashTable*         in_hash, 
		gint*         out_int, 
        GError **error)
	{
		g_hash_table_foreach_steal(in_hash, iter_func, NULL);
		
		*out_int = 5050;
		return TRUE;
	}

/* Public methods. */
gboolean dbus_glib_object1_variable_args_method (DBusGlibObject1 *self, gboolean     b,
		gshort		 n,
        gint         i,
        gint64       x,
        gushort		 q,
        guint        u,
        guint64      t,
        gdouble      d,
        gchar*		 s,
        GArray* 	 a,
        gint*		 r,
        GError **error)
{
	return DBUS_GLIB_OBJECT1_GET_CLASS (self)->method1 (self, b,n,i,x,q,u,t,d,s,a,r,error);
}

gboolean dbus_glib_object1_simple_method (DBusGlibObject1 *self, 
		gint         in_num,        
        gint*		 out_num,
        GError **error)
{
	return DBUS_GLIB_OBJECT1_GET_CLASS (self)->method2 (self, in_num, out_num, error);
}

gboolean dbus_glib_object1_struct_example_method (DBusGlibObject1 *self, 
		GValueArray*         st,        
        gint*		 ret,
        GError **error)
{
	return DBUS_GLIB_OBJECT1_GET_CLASS (self)->method3 (self, st, ret, error);
}

gboolean dbus_glib_object1_variant_send_method(DBusGlibObject1 *self, 
		GValue*         in_val,
		GValue*         out_val,
        GError **error)
{
	return DBUS_GLIB_OBJECT1_GET_CLASS (self)->method4 (self, in_val, out_val, error);
}

gboolean dbus_glib_object1_dict_send_method(DBusGlibObject1 *self, 
		GHashTable*         in_hash,
		gint*         out_int,
        GError **error)
{
	return DBUS_GLIB_OBJECT1_GET_CLASS (self)->method5 (self, in_hash, out_int, error);
}

void	dbus_glib_object1_dispose (GObject *self)
{
	static gboolean first_run = TRUE;

	if (first_run)
	{
		first_run = FALSE;
		
		/* Call g_object_unref on any GObjects that we hold, but don't break the object */

		parent_class-> dispose (self);
	}
}

void	dbus_glib_object1_finalize (GObject *self)
{
	parent_class-> finalize (self);
}

/* Here is where we override any functions. Since we have no properties or even fields, none of the below are needed. */
void	dbus_glib_object1_class_init		(gpointer g_class, gpointer class_data)
{
	GObjectClass	*object_class	= G_OBJECT_CLASS (g_class);
	DBusGlibObject1Class	*this_class	= DBUS_GLIB_OBJECT1_CLASS (g_class);
	
	//assign value to parent class
	parent_class = (GObjectClass *)g_type_class_peek_parent (g_class);
	
	//assing pointer values to the base class members
	object_class-> dispose = &dbus_glib_object1_dispose;
	object_class-> finalize = &dbus_glib_object1_finalize;
	
	//assign value to derived class members
	this_class->method1 = &dbus_glib_object1_variable_args_method_impl;
	this_class->method2 = &dbus_glib_object1_simple_method_impl;
	this_class->method3 = &dbus_glib_object1_struct_example_method_impl;
	this_class->method4 = &dbus_glib_object1_variant_send_method_impl;
	this_class->method5 = &dbus_glib_object1_dict_send_method_impl;
	
	dbus_g_object_type_install_info(G_TYPE_FROM_CLASS(this_class),&dbus_glib_DBusGlibObject1_object_info);
}

void dbus_glib_object1_init (DBusGlibObject1 *self)
{
	
}

GType dbus_glib_object1_get_type () 
{
	static GType g_define_type_id = 0; 
	if ((g_define_type_id == 0)) 
	{ 
		static const GTypeInfo g_define_type_info = 
		{ 
			sizeof (DBusGlibObject1Class), 
			(GBaseInitFunc) ((void *)0), 
			(GBaseFinalizeFunc) ((void *)0), 
			(GClassInitFunc) dbus_glib_object1_class_init, 
			(GClassFinalizeFunc) ((void *)0), 
			((void *)0), 
			sizeof (DBusGlibObject1), 
			0, 
			(GInstanceInitFunc) dbus_glib_object1_init, 
		}; 

		g_define_type_id = g_type_register_static 
		(
			G_TYPE_OBJECT, 
			"DBusGlibObject1", 
			&g_define_type_info, 
			(GTypeFlags) 0
		);
		
	} 

	return g_define_type_id; 
}


static void
hello_signal_handler (DBusGProxy *proxy, const char *hello_string, gpointer user_data)
{
  	FILE* fp = fopen("c:\\signal.txt", "w");
  	if(fp)
  		{
  		fprintf(fp, "Signal Received : %s", hello_string);
  		fclose(fp);
  		}
	printf ("Received signal and it says: %s\n", hello_string);
  
}

int main()
{
	DBusGlibObject1 *so = NULL;
	DBusGlibObject1 *look_so = NULL;
	DBusGConnection *bus;
	GMainLoop *mainLoop = NULL;
	DBusGProxy* proxy;
	unsigned int request_ret;
	GError *error = NULL;
	
	g_type_init();

	so = (DBusGlibObject1 *)g_object_new(DBUS_GLIB_OBJECT1_TYPE,NULL);

	bus = dbus_g_bus_get(DBUS_BUS_SESSION,NULL);

	proxy = dbus_g_proxy_new_for_name(bus,DBUS_SERVICE_DBUS,DBUS_PATH_DBUS,DBUS_INTERFACE_DBUS);

	dbus_g_connection_register_g_object(bus,"/com/example/DBusGlibObject1",G_OBJECT(so));
	
	look_so = (DBusGlibObject1 *)dbus_g_connection_lookup_g_object(bus, "/com/example/DBusGlibObject1");
	
	if(so != look_so)
		{
		g_print("dbus_g_connection_lookup_g_object() API Fail to return registered object.\n");
		return 1;
		}

	if(!org_freedesktop_DBus_request_name(proxy,"com.example.DBusGlibObject1",0,&request_ret,&error))
	{
		g_print("Unable to register service\n");
		return 1;
	}

	/* Tell DBus what the type signature of the signal callback is; this
	   * allows us to sanity-check incoming messages before invoking the
	   * callback.  You need to do this once for each proxy you create,
	   * not every time you want to connect to the signal.
	   */
	  dbus_g_proxy_add_signal (proxy, "hello_signal", G_TYPE_STRING, G_TYPE_INVALID);

	  /* Actually connect to the signal.  Note you can call
	   * dbus_g_proxy_connect_signal multiple times for one invocation of
	   * dbus_g_proxy_add_signal.
	   */
	  dbus_g_proxy_connect_signal (proxy, "hello_signal", G_CALLBACK (hello_signal_handler),
				       NULL, NULL);
	mainLoop = g_main_loop_new(NULL,FALSE);
	g_main_loop_run(mainLoop);
	
	return 0;
}

