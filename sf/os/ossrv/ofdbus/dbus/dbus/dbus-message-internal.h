/* -*- mode: C; c-file-style: "gnu" -*- */
/* dbus-message-internal.h DBusMessage object internal interfaces
 *
 * Copyright (C) 2002  Red Hat Inc.
 * Portion Copyright © 2008 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
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
#ifndef DBUS_MESSAGE_INTERNAL_H
#define DBUS_MESSAGE_INTERNAL_H

#ifdef __SYMBIAN32__
//these are not system headers in build time
#include <dbus/dbus-message.h>
#include "dbus-resources.h"
#include "dbus-list.h"
#else
#include <dbus/dbus-message.h>
#include <dbus/dbus-resources.h>
#include <dbus/dbus-list.h>
#endif

DBUS_BEGIN_DECLS

typedef struct DBusMessageLoader DBusMessageLoader;

void _dbus_message_get_network_data  (DBusMessage       *message,
				      const DBusString **header,
				      const DBusString **body);

#ifdef __SYMBIAN32__
IMPORT_C
#endif
void        _dbus_message_lock                  (DBusMessage  *message);
void        _dbus_message_unlock                (DBusMessage  *message);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
void        _dbus_message_set_serial            (DBusMessage  *message,
                                                 dbus_uint32_t serial);
dbus_bool_t _dbus_message_add_size_counter      (DBusMessage  *message,
                                                 DBusCounter  *counter);
void        _dbus_message_add_size_counter_link (DBusMessage  *message,
                                                 DBusList     *link);
void        _dbus_message_remove_size_counter   (DBusMessage  *message,
                                                 DBusCounter  *counter,
                                                 DBusList    **link_return);

#ifdef __SYMBIAN32__
IMPORT_C
#endif
DBusMessageLoader* _dbus_message_loader_new                   (void);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
DBusMessageLoader* _dbus_message_loader_ref                   (DBusMessageLoader  *loader);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
void               _dbus_message_loader_unref                 (DBusMessageLoader  *loader);

#ifdef __SYMBIAN32__
IMPORT_C
#endif
void               _dbus_message_loader_get_buffer            (DBusMessageLoader  *loader,
                                                               DBusString        **buffer);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
void               _dbus_message_loader_return_buffer         (DBusMessageLoader  *loader,
                                                               DBusString         *buffer,
                                                               int                 bytes_read);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
dbus_bool_t        _dbus_message_loader_queue_messages        (DBusMessageLoader  *loader);
DBusMessage*       _dbus_message_loader_peek_message          (DBusMessageLoader  *loader);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
DBusMessage*       _dbus_message_loader_pop_message           (DBusMessageLoader  *loader);
DBusList*          _dbus_message_loader_pop_message_link      (DBusMessageLoader  *loader);
void               _dbus_message_loader_putback_message_link  (DBusMessageLoader  *loader,
                                                               DBusList           *link);

#ifdef __SYMBIAN32__
IMPORT_C
#endif
dbus_bool_t        _dbus_message_loader_get_is_corrupted      (DBusMessageLoader  *loader);

void               _dbus_message_loader_set_max_message_size  (DBusMessageLoader  *loader,
                                                               long                size);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
long               _dbus_message_loader_get_max_message_size  (DBusMessageLoader  *loader);

DBUS_END_DECLS

#endif /* DBUS_MESSAGE_INTERNAL_H */
