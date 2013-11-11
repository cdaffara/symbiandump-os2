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



typedef struct _DBusGlibObject1 DBusGlibObject1;
struct _DBusGlibObject1
{
	GObject		parent_obj;
};

typedef struct _DBusGlibObject1Class DBusGlibObject1Class;
struct _DBusGlibObject1Class
{
	GObjectClass	parent_class;
};

GType	dbus_glib_object1_get_type ();


/* Handy macros */
#define DBUS_GLIB_OBJECT1_TYPE		(dbus_glib_object1_get_type ())
#define DBUS_GLIB_OBJECT1(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), DBUS_GLIB_OBJECT1_TYPE, DBusGlibObject1))
#define DBUS_GLIB_OBJECT1_CLASS(c)		(G_TYPE_CHECK_CLASS_CAST ((c), DBUS_GLIB_OBJECT1_TYPE, DBusGlibObject1Class))
#define DBUS_GLIB_IS_OBJECT(obj)		(G_TYPE_CHECK_TYPE ((obj), DBUS_GLIB_OBJECT1_TYPE))
#define DBUS_GLIB_IS_OBJECT_CLASS(c)		(G_TYPE_CHECK_CLASS_TYPE ((c), DBUS_GLIB_OBJECT1_TYPE))
#define DBUS_GLIB_OBJECT1_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), DBUS_GLIB_OBJECT1_TYPE, DBusGlibObject1Class))

#endif
