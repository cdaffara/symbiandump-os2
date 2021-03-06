/* -*- mode: C; c-file-style: "gnu" -*- */
/* dbus-marshal-byteswap.h  Swap a block of marshaled data
 *
 * Copyright (C) 2005 Red Hat, Inc.
 * Portion Copyright � 2008 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 * Licensed under the Academic Free License version 2.1
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef DBUS_MARSHAL_BYTESWAP_H
#define DBUS_MARSHAL_BYTESWAP_H

#ifdef __SYMBIAN32__
//these are not system headers in build tile
#include "config.h"
#include "dbus-protocol.h"
#include "dbus-marshal-recursive.h"
#else
#include <config.h>
#include <dbus/dbus-protocol.h>
#include <dbus/dbus-marshal-recursive.h>
#endif

#ifndef PACKAGE
#error "config.h not included here"
#endif

#ifdef __SYMBIAN32__
IMPORT_C
#endif
void _dbus_marshal_byteswap (const DBusString *signature,
                             int               signature_start,
                             int               old_byte_order,
                             int               new_byte_order,
                             DBusString       *value_str,
                             int               value_pos);

#endif /* DBUS_MARSHAL_BYTESWAP_H */
