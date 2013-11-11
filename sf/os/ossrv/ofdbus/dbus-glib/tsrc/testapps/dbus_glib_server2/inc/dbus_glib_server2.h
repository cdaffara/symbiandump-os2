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


#ifndef __DBUS_GLIB_OBJECT2_H__
#define __DBUS_GLIB_OBJECT2_H__

#include <glib-object.h>



typedef struct _DBusGlibObject2 DBusGlibObject2;
struct _DBusGlibObject2
{
	GObject		parent_obj;
};

typedef struct _DBusGlibObject2Class DBusGlibObject2Class;
struct _DBusGlibObject2Class
{
	GObjectClass	parent_class;

	/* Some useful methods may follow. */
	gboolean	(*method1)	(DBusGlibObject2 *self, gboolean     b,
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
	gboolean	(*method2)	(DBusGlibObject2 *self,
						gint     in_num,
			            gint*	 out_num,
										            GError **error);
	
	gboolean	(*method3)	(DBusGlibObject2 *self,
			GValueArray*     st,
            gint*	 ret,
							            GError **error);
	

	gboolean	(*method4)	(DBusGlibObject2 *self,
			gchar*     st,
            gint*	 ret, 
							            GError **error);
//	void		(*method2)	(DBusGlibObject1 *self, gchar*);
};

GType	dbus_glib_object2_get_type ();

gboolean dbus_glib_object2_variable_args_method (DBusGlibObject2 *self, gboolean     b,
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
gboolean dbus_glib_object2_simple_method (DBusGlibObject2 *self, 
        gint         in_num,        
        gint*		 out_num,
        GError **error);	/* virtual */

gboolean dbus_glib_object2_struct_example_method (DBusGlibObject2 *self, 
		GValueArray*         st,        
        gint*		 ret,
        GError **error);	/* virtual */

gboolean dbus_glib_object2_Send_message0 (DBusGlibObject2 *self, 
		gchar*         st,        
        gint*		 ret,
        GError **error);
/* Handy macros */
#define DBUS_GLIB_OBJECT2_TYPE		(dbus_glib_object2_get_type ())
#define DBUS_GLIB_OBJECT2(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), DBUS_GLIB_OBJECT2_TYPE, DBusGlibObject2))
#define DBUS_GLIB_OBJECT2_CLASS(c)		(G_TYPE_CHECK_CLASS_CAST ((c), DBUS_GLIB_OBJECT2_TYPE, DBusGlibObject2Class))
#define DBUS_GLIB_IS_OBJECT(obj)		(G_TYPE_CHECK_TYPE ((obj), DBUS_GLIB_OBJECT2_TYPE))
#define DBUS_GLIB_IS_OBJECT_CLASS(c)		(G_TYPE_CHECK_CLASS_TYPE ((c), DBUS_GLIB_OBJECT2_TYPE))
#define DBUS_GLIB_OBJECT2_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), DBUS_GLIB_OBJECT2_TYPE, DBusGlibObject2Class))

#endif
