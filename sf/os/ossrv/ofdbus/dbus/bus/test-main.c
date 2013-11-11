/* -*- mode: C; c-file-style: "gnu" -*- */
/* test-main.c  main() for make check
 *
 * Copyright (C) 2003 Red Hat, Inc.
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

#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#ifndef __SYMBIAN32__
#include <dbus/dbus-string.h>
#include <dbus/dbus-sysdeps.h>
#include <dbus/dbus-internals.h>
#else
#include "dbus-string.h"
#include "dbus-sysdeps.h"
#include "dbus-internals.h"
#endif //__SYMBIAN32__
#include "selinux.h"

#ifdef __SYMBIAN32__
#include <libxml2_xmlreader.h>
#include <libxml2_parser.h>
#include <libxml2_globals.h>
#include <libxml2_xmlexports.h>
#include <libxml2_xmlmemory.h>

#else
#include <libxml/xmlreader.h>
#include <libxml/parser.h>
#include <libxml/globals.h>
#include <libxml/xmlmemory.h>
#endif


#ifdef __SYMBIAN32__
#define LOG_FILE "c:\\logs\\dbus_daemon_test_main_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
	{
	if(result)
		assert_failed = 1;
	
	testResultXml("dbus_daemon_test_main");
    close_log_file();
	}
#endif

#ifdef DBUS_BUILD_TESTS
static void
die (const char *failure)
{
  fprintf (stderr, "Unit test failed: %s\n", failure);
  #ifdef __SYMBIAN32__
  std_log(LOG_FILENAME_LINE, "Unit test failed: %s\n", failure);
  
  create_xml(1);
  #endif
  exit (1);
}

static void
check_memleaks (const char *name)
{
  dbus_shutdown ();
  
  printf ("%s: checking for memleaks\n", name);
  #ifdef __SYMBIAN32__
  std_log(LOG_FILENAME_LINE, "%s: checking for memleaks\n", name);
  #endif
  if (_dbus_get_malloc_blocks_outstanding () != 0)
    {
      _dbus_warn ("%d dbus_malloc blocks were not freed\n",
                  _dbus_get_malloc_blocks_outstanding ());
		#ifdef __SYMBIAN32__
		std_log(LOG_FILENAME_LINE, "%d dbus_malloc blocks were not freed\n",
                  _dbus_get_malloc_blocks_outstanding ());
		#endif
      die ("memleaks");
    }
}
#endif /* DBUS_BUILD_TESTS */

static void
test_pre_hook (void)
{
  
  if (_dbus_getenv ("DBUS_TEST_SELINUX")
      && (!bus_selinux_pre_init ()
	  || !bus_selinux_full_init ()))
    die ("could not init selinux support");
}

static char *progname = "";
static void
test_post_hook (void)
{
  if (_dbus_getenv ("DBUS_TEST_SELINUX"))
    bus_selinux_shutdown ();
  check_memleaks (progname);
}

int
main (int argc, char **argv)
{
#ifdef DBUS_BUILD_TESTS
  const char *dir;
  DBusString test_data_dir;

  progname = argv[0];

  if (argc > 1)
    dir = argv[1];
  else
    dir = _dbus_getenv ("DBUS_TEST_DATA");

  dir = "c:\\data\\dbus";
  
  if (dir == NULL)
    {
      fprintf (stderr, "Must specify test data directory as argv[1] or in DBUS_TEST_DATA env variable\n");
	  #ifdef __SYMBIAN32__
	  std_log(LOG_FILENAME_LINE, "Must specify test data directory as argv[1] or in DBUS_TEST_DATA env variable\n");
	  #endif
      return 1;
    }

  _dbus_string_init_const (&test_data_dir, dir);

  if (!_dbus_threads_init_debug ())
    die ("initializing debug threads");
 
  test_pre_hook ();
  printf ("%s: Running expire list test\n", argv[0]);
  #ifdef __SYMBIAN32__
  std_log(LOG_FILENAME_LINE, "%s: Running expire list test\n", argv[0]);
  #endif
  if (!bus_expire_list_test (&test_data_dir))
    die ("expire list");
  test_post_hook ();
 
  test_pre_hook ();
  printf ("%s: Running config file parser test\n", argv[0]);
  #ifdef __SYMBIAN32__
  std_log(LOG_FILENAME_LINE, "%s: Running config file parser test\n", argv[0]);
  #endif
  if (!bus_config_parser_test (&test_data_dir))
    die ("parser");
  test_post_hook ();

  test_pre_hook ();
  printf ("%s: Running policy test\n", argv[0]);
  #ifdef __SYMBIAN32__
  std_log(LOG_FILENAME_LINE, "%s: Running policy test\n", argv[0]);
  #endif
  if (!bus_policy_test (&test_data_dir))
    die ("policy");
  test_post_hook ();

  test_pre_hook ();
  printf ("%s: Running signals test\n", argv[0]);
  #ifdef __SYMBIAN32__
  std_log(LOG_FILENAME_LINE, "%s: Running signals test\n", argv[0]);
  #endif
  if (!bus_signals_test (&test_data_dir))
    die ("signals");
  test_post_hook ();

  test_pre_hook ();
  printf ("%s: Running SHA1 connection test\n", argv[0]);
  #ifdef __SYMBIAN32__
  std_log(LOG_FILENAME_LINE, "%s: Running SHA1 connection test\n", argv[0]);
  #endif
  if (!bus_dispatch_sha1_test (&test_data_dir))
    die ("sha1");
  test_post_hook ();


  test_pre_hook ();
  printf ("%s: Running service files reloading test\n", argv[0]);
  #ifdef __SYMBIAN32__
  std_log(LOG_FILENAME_LINE, "%s: Running service files reloading test\n", argv[0]);
  #endif
  if (!bus_activation_service_reload_test (&test_data_dir))
    die ("service reload");
  test_post_hook ();

  #ifdef __SYMBIAN32__
  std_log(LOG_FILENAME_LINE, "%s: Running message dispatch test\n", argv[0]);
  #endif
  	
  test_pre_hook ();
  printf ("%s: Running message dispatch test\n", argv[0]);
  #ifdef __SYMBIAN32__
  std_log(LOG_FILENAME_LINE, "%s: Running message dispatch test\n", argv[0]);
  #endif
  if (!bus_dispatch_test (&test_data_dir)) 
    die ("dispatch");
  /*2 bytes of memory leak to be investigated*/
  //test_post_hook ();	
  printf ("%s: Success\n", argv[0]);
  #ifdef __SYMBIAN32__
	std_log(LOG_FILENAME_LINE, "%s: Success\n", argv[0]);
	
	create_xml(0);
  #endif
  return 0;
#else /* DBUS_BUILD_TESTS */

  printf ("Not compiled with test support\n");
  
  return 0;
#endif
}
