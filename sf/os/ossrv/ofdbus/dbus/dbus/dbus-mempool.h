/* -*- mode: C; c-file-style: "gnu" -*- */
/* dbus-mempool.h Memory pools
 * 
 * Copyright (C) 2002  Red Hat, Inc.
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

#ifndef DBUS_MEMPOOL_H
#define DBUS_MEMPOOL_H

#ifdef __SYMBIAN32__
//these are not system headers in build time
#include "dbus-internals.h"
#else
#include <dbus/dbus-internals.h>
#endif
#include <dbus/dbus-memory.h>
#include <dbus/dbus-types.h>


DBUS_BEGIN_DECLS

typedef struct DBusMemPool DBusMemPool;

#ifdef __SYMBIAN32__
IMPORT_C
#endif
DBusMemPool* _dbus_mem_pool_new     (int          element_size,
                                     dbus_bool_t  zero_elements);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
void         _dbus_mem_pool_free    (DBusMemPool *pool);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
void*        _dbus_mem_pool_alloc   (DBusMemPool *pool);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
dbus_bool_t  _dbus_mem_pool_dealloc (DBusMemPool *pool,
                                     void        *element);

DBUS_END_DECLS

#endif /* DBUS_MEMPOOL_H */
