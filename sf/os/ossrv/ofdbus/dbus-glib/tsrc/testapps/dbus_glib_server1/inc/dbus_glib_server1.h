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


#ifndef __DBUS_GLIB_OBJECT1_H__
#define __DBUS_GLIB_OBJECT1_H__

#include <glib-object.h>
#include <glib.h>



typedef struct _DBusGlibObject1 DBusGlibObject1;
struct _DBusGlibObject1
{
	GObject		parent_obj;
};

typedef struct _DBusGlibObject1Class DBusGlibObject1Class;
struct _DBusGlibObject1Class
{
	GObjectClass	parent_class;

	/* Some useful methods may follow. */
	gboolean	(*method1)	(DBusGlibObject1 *self, gboolean     b,
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
										            GError **error);
	gboolean	(*method2)	(DBusGlibObject1 *self,
						gint     in_num,
			            gint*	 out_num,
										            GError **error);
	
	gboolean	(*method3)	(DBusGlibObject1 *self,
			GValueArray*     st,
            gint*	 ret,
							            GError **error);
	gboolean	(*method4)	(DBusGlibObject1 *self,
			GValue*     in_val,
			GValue*	 	out_val,
							            GError **error);
	gboolean	(*method5)	(DBusGlibObject1 *self,
			GHashTable*     in_hash,
			gint*	 	out_int,
							            GError **error);
};

GType	dbus_glib_object1_get_type ();

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
        GError **error);	/* virtual */
gboolean dbus_glib_object1_simple_method (DBusGlibObject1 *self, 
        gint         in_num,        
        gint*		 out_num,
        GError **error);	/* virtual */

gboolean dbus_glib_object1_struct_example_method (DBusGlibObject1 *self, 
		GValueArray*         st,        
        gint*		 ret,
        GError **error);	/* virtual */
gboolean dbus_glib_object1_variant_send_method(DBusGlibObject1 *self, 
		GValue*         in_val,        
		GValue*		 	out_val,
        GError **error);	/* virtual */
gboolean dbus_glib_object1_dict_send_method(DBusGlibObject1 *self, 
		GHashTable*         in_hash,        
		gint*		 	out_int,
        GError **error);	/* virtual */


/* Handy macros */
#define DBUS_GLIB_OBJECT1_TYPE		(dbus_glib_object1_get_type ())
#define DBUS_GLIB_OBJECT1(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), DBUS_GLIB_OBJECT1_TYPE, DBusGlibObject1))
#define DBUS_GLIB_OBJECT1_CLASS(c)		(G_TYPE_CHECK_CLASS_CAST ((c), DBUS_GLIB_OBJECT1_TYPE, DBusGlibObject1Class))
#define DBUS_GLIB_IS_OBJECT(obj)		(G_TYPE_CHECK_TYPE ((obj), DBUS_GLIB_OBJECT1_TYPE))
#define DBUS_GLIB_IS_OBJECT_CLASS(c)		(G_TYPE_CHECK_CLASS_TYPE ((c), DBUS_GLIB_OBJECT1_TYPE))
#define DBUS_GLIB_OBJECT1_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), DBUS_GLIB_OBJECT1_TYPE, DBusGlibObject1Class))

#endif
