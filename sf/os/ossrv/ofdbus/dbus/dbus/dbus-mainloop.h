/* -*- mode: C; c-file-style: "gnu" -*- */
/* dbus-mainloop.h  Main loop utility
 *
 * Copyright (C) 2003  Red Hat, Inc.
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

#ifndef DBUS_MAINLOOP_H
#define DBUS_MAINLOOP_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <dbus/dbus.h>

DBUS_BEGIN_DECLS

typedef struct DBusLoop DBusLoop;

typedef dbus_bool_t (* DBusWatchFunction)   (DBusWatch     *watch,
                                             unsigned int   condition,
                                             void          *data);
typedef void        (* DBusTimeoutFunction) (DBusTimeout   *timeout,
                                             void          *data);

#ifdef __SYMBIAN32__
IMPORT_C
#endif
DBusLoop*   _dbus_loop_new            (void);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
DBusLoop*   _dbus_loop_ref            (DBusLoop            *loop);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
void        _dbus_loop_unref          (DBusLoop            *loop);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
dbus_bool_t _dbus_loop_add_watch      (DBusLoop            *loop,
                                       DBusWatch           *watch,
                                       DBusWatchFunction    function,
                                       void                *data,
                                       DBusFreeFunction     free_data_func);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
void        _dbus_loop_remove_watch   (DBusLoop            *loop,
                                       DBusWatch           *watch,
                                       DBusWatchFunction    function,
                                       void                *data);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
dbus_bool_t _dbus_loop_add_timeout    (DBusLoop            *loop,
                                       DBusTimeout         *timeout,
                                       DBusTimeoutFunction  function,
                                       void                *data,
                                       DBusFreeFunction     free_data_func);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
void        _dbus_loop_remove_timeout (DBusLoop            *loop,
                                       DBusTimeout         *timeout,
                                       DBusTimeoutFunction  function,
                                       void                *data);

#ifdef __SYMBIAN32__
IMPORT_C
#endif
dbus_bool_t _dbus_loop_queue_dispatch (DBusLoop            *loop,
                                       DBusConnection      *connection);

#ifdef __SYMBIAN32__
IMPORT_C
#endif
void        _dbus_loop_run            (DBusLoop            *loop);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
void        _dbus_loop_quit           (DBusLoop            *loop);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
dbus_bool_t _dbus_loop_iterate        (DBusLoop            *loop,
                                       dbus_bool_t          block);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
dbus_bool_t _dbus_loop_dispatch       (DBusLoop            *loop);

#ifdef __SYMBIAN32__
IMPORT_C
#endif
int  _dbus_get_oom_wait    (void);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
void _dbus_wait_for_memory (void);

DBUS_END_DECLS

#endif /* !DOXYGEN_SHOULD_SKIP_THIS */

#endif /* DBUS_MAINLOOP_H */

