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
#include "file-transfer-server.h"
#include "file_send-glue.h"
#include <dbus/dbus-glib-bindings.h>
#include <errno.h>


static GObjectClass *parent_class = ((void *)0);
static void some_object_init (SomeObject *self);

char *FILENAME = "c:/bunbask1.jpg";
int SIZE = (4*1024);

gboolean some_object_method1_impl (SomeObject *self, gint a, GArray *y,gint *z,GError **error)
{
	*z = a;
	
	return TRUE;
}

void some_object_method2_impl (SomeObject *self, gchar* b)
{
	self->m_b = b;
	g_print ("Method2: %s\n", self->m_b);
}


/* Public methods. */
gboolean some_object_method1 (SomeObject *self, gint a,GArray *y,gint *z,GError **error)
{
	return SOME_OBJECT_GET_CLASS (self)->method1 (self, a,y,z,error);
}

void	some_object_method2 (SomeObject *self, gchar* b)
{
	SOME_OBJECT_GET_CLASS (self)->method2 (self, b);
}

void	some_object_method3 (SomeObject *self, gfloat c)
{
	self->m_c = c;
	g_print ("Method3: %f\n", self->m_c);
}


void	some_object_dispose (GObject *self)
{
	static gboolean first_run = TRUE;

	if (first_run)
	{
		first_run = FALSE;
		
		/* Call g_object_unref on any GObjects that we hold, but don't break the object */

		parent_class-> dispose (self);
	}
}

void	some_object_finalize (GObject *self)
{
	parent_class-> finalize (self);
}

/* Here is where we override any functions. Since we have no properties or even fields, none of the below are needed. */
void	some_object_class_init		(gpointer g_class, gpointer class_data)
{
	GObjectClass	*object_class	= G_OBJECT_CLASS (g_class);
	SomeObjectClass	*this_class	= SOME_OBJECT_CLASS (g_class);
	
	//assign value to parent class
	parent_class = g_type_class_peek_parent (g_class);
	
	//assing pointer values to the base class members
	object_class-> dispose = &some_object_dispose;
	object_class-> finalize = &some_object_finalize;
	
	//assign value to derived class members
	this_class->method1 = &some_object_method1_impl;
	this_class->method2 = &some_object_method2_impl;

	dbus_g_object_type_install_info(G_TYPE_FROM_CLASS(this_class),&dbus_glib__object_info);
}

void some_object_init (SomeObject *self)
{
	self->m_a = 1;
	self->m_c = 1.03f;
	self->m_b = "sumit";
}

GType some_object_get_type () 
{
	static GType g_define_type_id = 0; 
	if ((g_define_type_id == 0)) 
	{ 
		static const GTypeInfo g_define_type_info = 
		{ 
			sizeof (SomeObjectClass), 
			(GBaseInitFunc) ((void *)0), 
			(GBaseFinalizeFunc) ((void *)0), 
			(GClassInitFunc) some_object_class_init, 
			(GClassFinalizeFunc) ((void *)0), 
			((void *)0), 
			sizeof (SomeObject), 
			0, 
			(GInstanceInitFunc) some_object_init, 
		}; 

		g_define_type_id = g_type_register_static 
		(
			G_TYPE_OBJECT, 
			"SomeObject", 
			&g_define_type_info, 
			(GTypeFlags) 0
		);
		
	} 

	return g_define_type_id; 
}

int main()
{
	SomeObject *so = NULL;
	DBusGConnection *bus;
	GMainLoop *mainLoop = NULL;
	unsigned int request_ret;
	GError *error = NULL;

	DBusGProxy *proxy = NULL;
	
	g_type_init();

	so = g_object_new(SOME_OBJECT_TYPE,NULL);

	bus = dbus_g_bus_get(DBUS_BUS_SESSION,NULL);

	proxy = dbus_g_proxy_new_for_name(bus,DBUS_SERVICE_DBUS,DBUS_PATH_DBUS,DBUS_INTERFACE_DBUS);

	dbus_g_connection_register_g_object(bus,"/com/example/SomeObject",G_OBJECT(so));

	if(!org_freedesktop_DBus_request_name(proxy,"com.example.SomeObject",0,&request_ret,&error))
	{
		g_print("Unable to register service\n");
		return 1;
	}

	mainLoop = g_main_loop_new(NULL,FALSE);
	g_main_loop_run(mainLoop);
	
	return 0;
}

