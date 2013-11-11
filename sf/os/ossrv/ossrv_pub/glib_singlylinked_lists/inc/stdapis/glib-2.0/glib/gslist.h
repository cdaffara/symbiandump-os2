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

#ifndef __G_SLIST_H__
#define __G_SLIST_H__

#include <_ansi.h>
#include <glib/gmem.h>

G_BEGIN_DECLS

typedef struct _GSList GSList;

struct _GSList
{
  gpointer data;
  GSList *next;
};

/* Singly linked lists
 */
IMPORT_C GSList*  g_slist_alloc                   (void) G_GNUC_WARN_UNUSED_RESULT;
IMPORT_C void     g_slist_free           (GSList           *list);
IMPORT_C void     g_slist_free_1         (GSList           *list);
#define	 g_slist_free1		         g_slist_free_1
IMPORT_C GSList*  g_slist_append         (GSList           *list,
					  gpointer          data) G_GNUC_WARN_UNUSED_RESULT;
IMPORT_C GSList*  g_slist_prepend        (GSList           *list,
					  gpointer          data) G_GNUC_WARN_UNUSED_RESULT;
IMPORT_C GSList*  g_slist_insert                  (GSList           *list,
					  gpointer          data,
					  gint              position) G_GNUC_WARN_UNUSED_RESULT;
IMPORT_C GSList*  g_slist_insert_sorted           (GSList           *list,
					  gpointer          data,
					  GCompareFunc      func) G_GNUC_WARN_UNUSED_RESULT;
IMPORT_C GSList*  g_slist_insert_sorted_with_data (GSList           *list,
					  gpointer          data,
					  GCompareDataFunc  func,
					  gpointer          user_data) G_GNUC_WARN_UNUSED_RESULT;
IMPORT_C GSList*  g_slist_insert_before           (GSList           *slist,
					  GSList           *sibling,
					  gpointer          data) G_GNUC_WARN_UNUSED_RESULT;
IMPORT_C GSList*  g_slist_concat         (GSList           *list1,
					  GSList           *list2) G_GNUC_WARN_UNUSED_RESULT;
IMPORT_C GSList*  g_slist_remove         (GSList           *list,
					  gconstpointer     data) G_GNUC_WARN_UNUSED_RESULT;
IMPORT_C GSList*  g_slist_remove_all     (GSList           *list,
					  gconstpointer     data) G_GNUC_WARN_UNUSED_RESULT;
IMPORT_C GSList*  g_slist_remove_link    (GSList           *list,
					  GSList           *link_) G_GNUC_WARN_UNUSED_RESULT;
IMPORT_C GSList*  g_slist_delete_link    (GSList           *list,
					  GSList           *link_) G_GNUC_WARN_UNUSED_RESULT;
IMPORT_C GSList*  g_slist_reverse                 (GSList           *list) G_GNUC_WARN_UNUSED_RESULT;
IMPORT_C GSList*  g_slist_copy                    (GSList           *list) G_GNUC_WARN_UNUSED_RESULT;
IMPORT_C GSList*  g_slist_nth            (GSList           *list,
					  guint             n);
IMPORT_C GSList*  g_slist_find           (GSList           *list,
					  gconstpointer     data);
IMPORT_C GSList*  g_slist_find_custom    (GSList           *list,
					  gconstpointer     data,
					  GCompareFunc      func);
IMPORT_C gint     g_slist_position       (GSList           *list,
					  GSList           *llink);
IMPORT_C gint     g_slist_index          (GSList           *list,
					  gconstpointer     data);
IMPORT_C GSList*  g_slist_last           (GSList           *list);
IMPORT_C guint    g_slist_length         (GSList           *list);
IMPORT_C void     g_slist_foreach        (GSList           *list,
					  GFunc             func,
					  gpointer          user_data);
IMPORT_C GSList*  g_slist_sort           (GSList           *list,
					  GCompareFunc      compare_func) G_GNUC_WARN_UNUSED_RESULT;
IMPORT_C GSList*  g_slist_sort_with_data (GSList           *list,
					  GCompareDataFunc  compare_func,
					  gpointer          user_data) G_GNUC_WARN_UNUSED_RESULT;
IMPORT_C gpointer g_slist_nth_data       (GSList           *list,
					  guint             n);

#define  g_slist_next(slist)	         ((slist) ? (((GSList *)(slist))->next) : NULL)

#ifndef G_DISABLE_DEPRECATED
IMPORT_C void     g_slist_push_allocator          (gpointer	   dummy);
IMPORT_C void     g_slist_pop_allocator           (void);
#endif
G_END_DECLS

#endif /* __G_SLIST_H__ */

