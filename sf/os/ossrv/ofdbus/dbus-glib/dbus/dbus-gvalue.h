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
#ifndef DBUS_GOBJECT_VALUE_H
#define DBUS_GOBJECT_VALUE_H

#include <dbus/dbus.h>
#include <dbus/dbus-signature.h>
#include <glib.h>
#include <glib-object.h>
#include "dbus/dbus-glib.h"

G_BEGIN_DECLS

typedef struct {
  DBusGConnection    *gconnection;
  DBusGProxy         *proxy;
} DBusGValueMarshalCtx;

void           _dbus_g_value_types_init        (void);

char *         _dbus_gtype_to_signature        (GType                    type);
char *         _dbus_gvalue_to_signature       (const GValue            *val);

gboolean       _dbus_gvalue_demarshal          (DBusGValueMarshalCtx    *context,
					       DBusMessageIter         *iter,
					       GValue                  *value,
					       GError                 **error);

gboolean       _dbus_gvalue_demarshal_variant  (DBusGValueMarshalCtx    *context,
					       DBusMessageIter         *iter,
					       GValue                  *value,
					       GError                 **error);

GValueArray *  _dbus_gvalue_demarshal_message  (DBusGValueMarshalCtx    *context,
					       DBusMessage             *message,
					       guint                    n_params,
					       const GType             *types, 
					       GError                 **error);

gboolean       _dbus_gvalue_marshal            (DBusMessageIter         *iter,
					       const GValue            *value);

G_END_DECLS

#endif /* DBUS_GOBJECT_VALUE_H */
