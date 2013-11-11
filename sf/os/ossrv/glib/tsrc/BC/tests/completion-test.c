/* GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
 * Portion Copyright © 2008-09 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
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
#include <string.h>
#include <stdio.h>

#include "glib.h"

#ifdef SYMBIAN
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/


int main (int argc, char *argv[])
{
  GCompletion *cmp;
  GList *items;
  gchar *prefix;
  
  #ifdef SYMBIAN
  g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
  g_set_print_handler(mrtPrintHandler);
  #endif /*SYMBIAN*/
	  

  cmp = g_completion_new (NULL);

  items = NULL;
  items = g_list_append (items, "a\302\243");
  items = g_list_append (items, "a\302\244");
  items = g_list_append (items, "bb");
  items = g_list_append (items, "bc");
  g_completion_add_items (cmp, items);

  items = g_completion_complete (cmp, "a", &prefix);
  g_assert (!strcmp ("a\302", prefix));
  g_free (prefix);
  
  items = g_completion_complete_utf8 (cmp, "a", &prefix);
  g_assert (!strcmp ("a", prefix));
  g_free (prefix);

  items = g_completion_complete (cmp, "b", &prefix);
  g_assert (!strcmp ("b", prefix));
  g_free (prefix);
  
  items = g_completion_complete_utf8 (cmp, "b", &prefix);
  g_assert (!strcmp ("b", prefix));
  g_free (prefix);
  
  #if SYMBIAN
  testResultXml("completion-test");
  #endif /* EMULATOR */

  return 0;
}
