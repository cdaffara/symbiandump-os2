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


#ifndef __SOME_OBJECT_H__
#define __SOME_OBJECT_H__

#include <glib-object.h>

typedef struct _SomeObject SomeObject;
struct _SomeObject
{
	GObject		parent_obj;
	gint 		m_a;
	gchar*		m_b;
	gfloat		m_c;
};

typedef struct _SomeObjectClass SomeObjectClass;
struct _SomeObjectClass
{
	GObjectClass	parent_class;

	/* Some useful methods may follow. */
	gboolean	(*method1)	(SomeObject *self, gint x,GArray *y,gint *z,GError **error);
	void		(*method2)	(SomeObject *self, gchar*);
};

GType	some_object_get_type ();

gboolean some_object_method1 (SomeObject *self, gint x,GArray *y,gint *z,GError **);	/* virtual */
void	some_object_method2 (SomeObject *self, gchar*);	/* virtual */
void	some_object_method3 (SomeObject *self, gfloat);	/* non-virtual */


/* Handy macros */
#define SOME_OBJECT_TYPE		(some_object_get_type ())
#define SOME_OBJECT(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), SOME_OBJECT_TYPE, SomeObject))
#define SOME_OBJECT_CLASS(c)		(G_TYPE_CHECK_CLASS_CAST ((c), SOME_OBJECT_TYPE, SomeObjectClass))
#define SOME_IS_OBJECT(obj)		(G_TYPE_CHECK_TYPE ((obj), SOME_OBJECT_TYPE))
#define SOME_IS_OBJECT_CLASS(c)		(G_TYPE_CHECK_CLASS_TYPE ((c), SOME_OBJECT_TYPE))
#define SOME_OBJECT_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), SOME_OBJECT_TYPE, SomeObjectClass))

#endif
