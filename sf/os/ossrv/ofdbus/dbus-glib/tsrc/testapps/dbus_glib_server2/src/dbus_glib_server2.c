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
#include "dbus_glib_server2.h"
#include "dbus_glib3_stringsend-glue.h"
#include <dbus/dbus-glib-bindings.h>
#include <errno.h>

static GObjectClass *parent_class = NULL;
static void dbus_glib_object2_init (DBusGlibObject2 *self);



gboolean dbus_glib_object2_Send_message0_impl(DBusGlibObject2 *self, 
		gchar*         st,        
        gint*		 ret,
        GError **error)
{	
	*ret = 9090;
	return TRUE;
}


gboolean dbus_glib_object2_Send_message0(DBusGlibObject2 *self, 
		gchar*         st,        
        gint*		 ret,
        GError **error)
{
	return DBUS_GLIB_OBJECT2_GET_CLASS (self)->method4 (self, st, ret, error); 
}

void	dbus_glib_object2_dispose (GObject *self)
{
	static gboolean first_run = TRUE;

	if (first_run)
	{
		first_run = FALSE;
		
		/* Call g_object_unref on any GObjects that we hold, but don't break the object */

	parent_class-> dispose (self);
	}
}

void	dbus_glib_object2_finalize (GObject *self)
{
	parent_class-> finalize (self);
}

/* Here is where we override any functions. Since we have no properties or even fields, none of the below are needed. */
void	dbus_glib_object2_class_init		(gpointer g_class, gpointer class_data)
{
	GObjectClass	*object_class	= G_OBJECT_CLASS (g_class);
	DBusGlibObject2Class	*this_class	= DBUS_GLIB_OBJECT2_CLASS (g_class);
	
	//assign value to parent class
	parent_class = (GObjectClass *)g_type_class_peek_parent (g_class);
	
	//assing pointer values to the base class members
	object_class-> dispose = &dbus_glib_object2_dispose;
	object_class-> finalize = &dbus_glib_object2_finalize;
	
	//assign value to derived class members
	
	this_class->method4 = &dbus_glib_object2_Send_message0_impl;

	dbus_g_object_type_install_info(G_TYPE_FROM_CLASS(this_class),&dbus_glib_DBUSGlibobject2_object_info);
}

void dbus_glib_object2_init (DBusGlibObject2 *self)
{
	
}

GType dbus_glib_object2_get_type () 
{
	static GType g_define_type_id = 0; 
	if ((g_define_type_id == 0)) 
	{ 
		static const GTypeInfo g_define_type_info = 
		{ 
			sizeof (DBusGlibObject2Class), 
			(GBaseInitFunc) ((void *)0), 
			(GBaseFinalizeFunc) ((void *)0), 
			(GClassInitFunc) dbus_glib_object2_class_init, 
			(GClassFinalizeFunc) ((void *)0), 
			((void *)0), 
			sizeof (DBusGlibObject2), 
			0, 
			(GInstanceInitFunc) dbus_glib_object2_init, 
		}; 

		g_define_type_id = g_type_register_static 
		(
			G_TYPE_OBJECT, 
			"DBusGlibObject2", 
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

int main(int argc,char *argv[])
{
	DBusGlibObject2 *so = NULL;
	DBusGlibObject2 *look_so = NULL;
	DBusGConnection *bus;
	GMainLoop *mainLoop = NULL;
	DBusGProxy* proxy;
	unsigned int request_ret;
	GError *error = NULL;
	char *x;
	
	g_type_init();

	so = (DBusGlibObject2 *)g_object_new(DBUS_GLIB_OBJECT2_TYPE,NULL);

	bus = dbus_g_bus_get(DBUS_BUS_SESSION,NULL);

	proxy = dbus_g_proxy_new_for_name(bus,DBUS_SERVICE_DBUS,DBUS_PATH_DBUS,DBUS_INTERFACE_DBUS);

	dbus_g_connection_register_g_object(bus,"/com/example/DBusGlibObject2",G_OBJECT(so));
	
	look_so = (DBusGlibObject2 *)dbus_g_connection_lookup_g_object(bus, "/com/example/DBusGlibObject2");
	
	if(so != look_so)
		{
		g_print("dbus_g_connection_lookup_g_object() API Fail to return registered object.\n");
		return 1;
		}

	if(!org_freedesktop_DBus_request_name(proxy,"com.example.DBusGlibObject2",0,&request_ret,&error))
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

