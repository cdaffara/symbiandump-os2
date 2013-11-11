/* GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
 * Portions copyright (c) 2006 Nokia Corporation.  All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the GLib Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/. 
 */

#ifndef __G_HASH_H__
#define __G_HASH_H__

#include <_ansi.h>
#include <glib/gtypes.h>

G_BEGIN_DECLS

typedef struct _GHashTable  GHashTable;

typedef gboolean  (*GHRFunc)  (gpointer  key,
                               gpointer  value,
                               gpointer  user_data);

/* Hash tables
 */
IMPORT_C GHashTable* g_hash_table_new		   (GHashFunc	    hash_func,
					    GEqualFunc	    key_equal_func);
IMPORT_C GHashTable* g_hash_table_new_full      	   (GHashFunc	    hash_func,
					    GEqualFunc	    key_equal_func,
					    GDestroyNotify  key_destroy_func,
					    GDestroyNotify  value_destroy_func);
IMPORT_C void	    g_hash_table_destroy	   (GHashTable	   *hash_table);
IMPORT_C void	    g_hash_table_insert		   (GHashTable	   *hash_table,
					    gpointer	    key,
					    gpointer	    value);
IMPORT_C void        g_hash_table_replace           (GHashTable     *hash_table,
					    gpointer	    key,
					    gpointer	    value);
IMPORT_C gboolean    g_hash_table_remove		   (GHashTable	   *hash_table,
					    gconstpointer   key);
IMPORT_C gboolean    g_hash_table_steal             (GHashTable     *hash_table,
					    gconstpointer   key);
IMPORT_C gpointer    g_hash_table_lookup		   (GHashTable	   *hash_table,
					    gconstpointer   key);
IMPORT_C gboolean    g_hash_table_lookup_extended   (GHashTable	   *hash_table,
					    gconstpointer   lookup_key,
					    gpointer	   *orig_key,
					    gpointer	   *value);
IMPORT_C void	    g_hash_table_foreach	   (GHashTable	   *hash_table,
					    GHFunc	    func,
					    gpointer	    user_data);
IMPORT_C gpointer    g_hash_table_find	   (GHashTable	   *hash_table,
					    GHRFunc	    predicate,
					    gpointer	    user_data);
IMPORT_C guint	    g_hash_table_foreach_remove	   (GHashTable	   *hash_table,
					    GHRFunc	    func,
					    gpointer	    user_data);
IMPORT_C guint	    g_hash_table_foreach_steal	   (GHashTable	   *hash_table,
					    GHRFunc	    func,
					    gpointer	    user_data);
IMPORT_C guint	    g_hash_table_size		   (GHashTable	   *hash_table);

/* keeping hash tables alive */
IMPORT_C GHashTable* g_hash_table_ref   		   (GHashTable 	   *hash_table);
IMPORT_C void        g_hash_table_unref             (GHashTable     *hash_table);

#ifndef G_DISABLE_DEPRECATED

/* The following two functions are deprecated and will be removed in
 * the next major release. They do no good. */
#define g_hash_table_freeze(hash_table) ((void)0)
#define g_hash_table_thaw(hash_table) ((void)0)

#endif /* G_DISABLE_DEPRECATED */

/* Hash Functions
 */
IMPORT_C gboolean g_str_equal (gconstpointer  v1,
                      gconstpointer  v2);
IMPORT_C guint    g_str_hash  (gconstpointer  v);

IMPORT_C gboolean g_int_equal (gconstpointer  v1,
                      gconstpointer  v2);
IMPORT_C guint    g_int_hash  (gconstpointer  v);

/* This "hash" function will just return the key's address as an
 * unsigned integer. Useful for hashing on plain addresses or
 * simple integer values.
 * Passing NULL into g_hash_table_new() as GHashFunc has the
 * same effect as passing g_direct_hash().
 */
IMPORT_C guint    g_direct_hash  (gconstpointer  v) G_GNUC_CONST;
IMPORT_C gboolean g_direct_equal (gconstpointer  v1,
                         gconstpointer  v2) G_GNUC_CONST;

G_END_DECLS

#endif /* __G_HASH_H__ */

