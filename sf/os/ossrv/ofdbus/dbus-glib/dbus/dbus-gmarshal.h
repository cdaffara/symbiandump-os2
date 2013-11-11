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

#ifndef ___dbus_g_marshal_MARSHAL_H__
#define ___dbus_g_marshal_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* NONE:STRING,STRING,STRING (dbus-gmarshal.list:1) */
extern void _dbus_g_marshal_VOID__STRING_STRING_STRING (GClosure     *closure,
                                                        GValue       *return_value,
                                                        guint         n_param_values,
                                                        const GValue *param_values,
                                                        gpointer      invocation_hint,
                                                        gpointer      marshal_data);
#define _dbus_g_marshal_NONE__STRING_STRING_STRING	_dbus_g_marshal_VOID__STRING_STRING_STRING

G_END_DECLS

#endif /* ___dbus_g_marshal_MARSHAL_H__ */

