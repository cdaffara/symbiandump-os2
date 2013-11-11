/* -*- mode: C; c-file-style: "gnu" -*- */
/* dbus-userdb.h User database abstraction
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

#ifndef DBUS_USERDB_H
#define DBUS_USERDB_H

#ifdef __SYMBIAN32__
//these are not system headers in build time
#include "dbus-sysdeps.h"
#else
#include <dbus/dbus-sysdeps.h>
#endif

DBUS_BEGIN_DECLS

typedef struct DBusUserDatabase DBusUserDatabase;

#ifdef DBUS_USERDB_INCLUDES_PRIVATE
#ifdef __SYMBIAN32__
//these are not system headers in build time
#include "dbus-hash.h"
#else
#include <dbus/dbus-hash.h>
#endif

/**
 * Internals of DBusUserDatabase
 */
struct DBusUserDatabase
{
  int refcount; /**< Reference count */

  DBusHashTable *users; /**< Users in the database by UID */
  DBusHashTable *groups; /**< Groups in the database by GID */
  DBusHashTable *users_by_name; /**< Users in the database by name */
  DBusHashTable *groups_by_name; /**< Groups in the database by name */

};

#endif /* DBUS_USERDB_INCLUDES_PRIVATE */

#ifdef __SYMBIAN32__
IMPORT_C
#endif
DBusUserDatabase* _dbus_user_database_new           (void);
DBusUserDatabase* _dbus_user_database_ref           (DBusUserDatabase     *db);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
void              _dbus_user_database_flush         (DBusUserDatabase     *db);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
void              _dbus_user_database_unref         (DBusUserDatabase     *db);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
dbus_bool_t       _dbus_user_database_get_groups    (DBusUserDatabase     *db,
                                                     dbus_uid_t            uid,
                                                     dbus_gid_t          **group_ids,
                                                     int                  *n_group_ids,
                                                     DBusError            *error);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
dbus_bool_t       _dbus_user_database_get_uid       (DBusUserDatabase     *db,
                                                     dbus_uid_t            uid,
                                                     const DBusUserInfo  **info,
                                                     DBusError            *error);
dbus_bool_t       _dbus_user_database_get_gid       (DBusUserDatabase     *db,
                                                     dbus_gid_t            gid,
                                                     const DBusGroupInfo **info,
                                                     DBusError            *error);
dbus_bool_t       _dbus_user_database_get_username  (DBusUserDatabase     *db,
                                                     const DBusString     *username,
                                                     const DBusUserInfo  **info,
                                                     DBusError            *error);
dbus_bool_t       _dbus_user_database_get_groupname (DBusUserDatabase     *db,
                                                     const DBusString     *groupname,
                                                     const DBusGroupInfo **info,
                                                     DBusError            *error);

#ifdef DBUS_USERDB_INCLUDES_PRIVATE
#ifdef __SYMBIAN32__
IMPORT_C
#endif
DBusUserInfo*  _dbus_user_database_lookup       (DBusUserDatabase *db,
                                                 dbus_uid_t        uid,
                                                 const DBusString *username,
                                                 DBusError        *error);
DBusGroupInfo* _dbus_user_database_lookup_group (DBusUserDatabase *db,
                                                 dbus_gid_t        gid,
                                                 const DBusString *groupname,
                                                 DBusError        *error);
void           _dbus_user_info_free_allocated   (DBusUserInfo     *info);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
void           _dbus_group_info_free_allocated  (DBusGroupInfo    *info);
#endif /* DBUS_USERDB_INCLUDES_PRIVATE */

#ifdef __SYMBIAN32__
IMPORT_C
#endif
DBusUserDatabase* _dbus_user_database_get_system    (void);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
void              _dbus_user_database_lock_system   (void);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
void              _dbus_user_database_unlock_system (void);

#ifdef __SYMBIAN32__
IMPORT_C
#endif
dbus_bool_t _dbus_username_from_current_process (const DBusString **username);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
dbus_bool_t _dbus_homedir_from_current_process  (const DBusString **homedir);
dbus_bool_t _dbus_homedir_from_username         (const DBusString  *username,
                                                 DBusString        *homedir);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
dbus_bool_t _dbus_get_user_id                   (const DBusString  *username,
                                                 dbus_uid_t        *uid);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
dbus_bool_t _dbus_get_group_id                  (const DBusString  *group_name,
                                                 dbus_gid_t        *gid);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
dbus_bool_t _dbus_credentials_from_username     (const DBusString  *username,
                                                 DBusCredentials   *credentials);
dbus_bool_t _dbus_credentials_from_uid          (dbus_uid_t         user_id,
                                                 DBusCredentials   *credentials);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
dbus_bool_t _dbus_is_console_user               (dbus_uid_t         uid,
                                                 DBusError         *error);

#ifdef __SYMBIAN32__
IMPORT_C
#endif
dbus_bool_t _dbus_is_a_number                   (const DBusString *str, 
                                                 unsigned long    *num);


DBUS_END_DECLS

#endif /* DBUS_USERDB_H */
