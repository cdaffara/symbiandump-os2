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

#undef G_DISABLE_ASSERT
#undef G_LOG_DOMAIN

#include <stdio.h>
#include <string.h>
#include "glib.h"

#ifdef SYMBIAN
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/

int array[10000];
gboolean failed = FALSE;

#define	TEST(m,cond)	G_STMT_START { failed = !(cond); \
if (failed) \
  { assert_failed = TRUE;\
  	if (!m) \
      g_print ("\n(%s:%d) failed for: %s\n", __FILE__, __LINE__, ( # cond )); \
    else \
      g_print ("\n(%s:%d) failed for: %s: (%s)\n", __FILE__, __LINE__, ( # cond ), (gchar*)m); \
  } \
else \
  g_print ("."); fflush (stdout); \
} G_STMT_END

#define	C2P(c)		((gpointer) ((long) (c)))
#define	P2C(p)		((gchar) ((long) (p)))

#define GLIB_TEST_STRING "el dorado "
#define GLIB_TEST_STRING_5 "el do"


int
main (int   argc,
      char *argv[])
{
  gint i;
  struct {
    gchar *filename;
    gchar *dirname;
  } dirname_checks[] = {
    { "/", "/" },
    { "////", "/" },
    { ".////", "." },
    { ".", "." },
    { "..", "." },
    { "../", ".." },
    { "..////", ".." },
    { "", "." },
    { "a/b", "a" },
    { "a/b/", "a/b" },
    { "c///", "c" },
    { "/a/b", "/a" },
    { "/a/b/", "/a/b" },
#ifdef G_OS_WIN32
    { "\\", "\\" },
    { ".\\\\\\\\", "." },
    { ".\\/\\/", "." },
    { ".", "." },
    { "..", "." },
    { "..\\", ".." },
    { "..\\\\\\\\", ".." },
    { "..\\//\\", ".." },
    { "", "." },
    { "a\\b", "a" },
    { "a\\b\\", "a\\b" },
    { "\\a\\b", "\\a" },
    { "\\a\\b\\", "\\a\\b" },
    { "c\\\\\\", "c" },
    { "c/\\\\", "c" },
    { "a:", "a:." },
    { "a:foo", "a:." },
    { "a:foo\\bar", "a:foo" },
    { "a:/foo", "a:/" },
    { "a:/foo/bar", "a:/foo" },
    { "a:/", "a:/" },
    { "a://", "a:/" },
    { "a:\\foo", "a:\\" },
    { "a:\\", "a:\\" },
    { "a:\\\\", "a:\\" },
    { "a:\\/", "a:\\" },
#endif
  };
  guint n_dirname_checks = sizeof (dirname_checks) / sizeof (dirname_checks[0]);

  #ifdef SYMBIAN
  g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
  g_set_print_handler(mrtPrintHandler);
  #endif /*SYMBIAN*/
	  

  for (i = 0; i < n_dirname_checks; i++)
    {
      gchar *dirname;

      dirname = g_path_get_dirname (dirname_checks[i].filename);
      if (strcmp (dirname, dirname_checks[i].dirname) != 0)
      {
      	g_assert(FALSE && "dirname-test failed");
      	g_print ("%s returned %s, should return %s",dirname_checks[i].filename, dirname,dirname_checks[i].dirname);
      }
      g_free (dirname);
    }
    
    #if SYMBIAN
  	testResultXml("dirname-test");
  	#endif /* EMULATOR */

  return 0;
}

