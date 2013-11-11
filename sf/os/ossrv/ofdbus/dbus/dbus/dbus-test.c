/* -*- mode: C; c-file-style: "gnu" -*- */
/* dbus-test.c  Program to run all tests
 *
 * Copyright (C) 2002, 2003, 2004, 2005  Red Hat Inc.
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

#ifdef __SYMBIAN32__
#include "config.h"
#else
#include <config.h>
#endif
#include "dbus-test.h"
#include "dbus-sysdeps.h"
#include "dbus-internals.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef __SYMBIAN32__
#define LOG_FILE "c:\\logs\\dbus_test_main_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
	{
	if(result)
		assert_failed = 1;
	
	testResultXml("dbus_test_main");
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
check_memleaks (void)
{
  dbus_shutdown ();

  printf ("%s: checking for memleaks\n", "dbus-test");
  #ifdef __SYMBIAN32__
  std_log(LOG_FILENAME_LINE, "%s: checking for memleaks\n", "dbus-test");
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

typedef dbus_bool_t (*TestFunc)(void);
typedef dbus_bool_t (*TestDataFunc)(const char *data);

static void
run_test (const char             *test_name,
	  const char             *specific_test,
	  TestFunc                test)
{
  if (!specific_test || strcmp (specific_test, test_name) == 0)
    {
      printf ("%s: running %s tests\n", "dbus-test", test_name);
	  #ifdef __SYMBIAN32__
	  std_log(LOG_FILENAME_LINE, "%s: running %s tests\n", "dbus-test", test_name);
	  #endif
      if (!test ())
    	  die (test_name);
	#ifdef __SYMBIAN32__
      else
    	  std_log(LOG_FILENAME_LINE, "Unit test passed: %s\n", test_name);
	#endif
    }

  check_memleaks ();
}

static void
run_data_test (const char             *test_name,
	       const char             *specific_test,
	       TestDataFunc            test,
	       const char             *test_data_dir)
{
  if (!specific_test || strcmp (specific_test, test_name) == 0)
    {
      printf ("%s: running %s tests\n", "dbus-test", test_name);
	  #ifdef __SYMBIAN32__
	  std_log(LOG_FILENAME_LINE, "%s: running %s tests\n", "dbus-test", test_name);
	  #endif
      if (!test (test_data_dir))
    	  die (test_name);
	#ifdef __SYMBIAN32__
      else
    	  std_log(LOG_FILENAME_LINE, "Unit test passed: %s\n", test_name);
	#endif
    }

  check_memleaks ();
}

#endif /* DBUS_BUILD_TESTS */

/**
 * An exported symbol to be run in order to execute
 * unit tests. Should not be used by
 * any app other than our test app, this symbol
 * won't exist in some builds of the library.
 * (with --enable-tests=no)
 *
 * @param test_data_dir the directory with test data (test/data normally)
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif
void
dbus_internal_do_not_use_run_tests (const char *test_data_dir, const char *specific_test)
{
#ifdef DBUS_BUILD_TESTS
	int ret;
	//set the test home directory to the private folder on C
	ret = setenv("DBUS_TEST_HOMEDIR", "c:\\private\\037b382d", 1);
	if(-1 == ret)
	{
		std_log(LOG_FILENAME_LINE, "Setting test home dir returned errno : %d\n", errno);
	}
  if (!_dbus_threads_init_debug ())
    die ("debug threads init");
  
  if (test_data_dir == NULL)
    test_data_dir = _dbus_getenv ("DBUS_TEST_DATA");

  if (test_data_dir != NULL)
  #ifdef __SYMBIAN32__
  {
  #endif
    printf ("Test data in %s\n", test_data_dir);
	#ifdef __SYMBIAN32__
	std_log(LOG_FILENAME_LINE, "Test data in %s\n", test_data_dir);
	}
	#endif
  else
  #ifdef __SYMBIAN32__
  {
  #endif
    printf ("No test data!\n");
	#ifdef __SYMBIAN32__
	std_log(LOG_FILENAME_LINE, "No test data!\n");
	}
	#endif

  run_test ("string", specific_test, _dbus_string_test);
  
  run_test ("sysdeps", specific_test, _dbus_sysdeps_test);
  
  run_test ("data-slot", specific_test, _dbus_data_slot_test);

  run_test ("misc", specific_test, _dbus_misc_test);
  
  run_test ("address", specific_test, _dbus_address_test);

  run_test ("server", specific_test, _dbus_server_test);

  run_test ("object-tree", specific_test, _dbus_object_tree_test);

  run_test ("signature", specific_test, _dbus_signature_test);
  
  run_test ("marshalling", specific_test, _dbus_marshal_test);

#if 0
  printf ("%s: running recursive marshalling tests\n", "dbus-test");
  #ifdef __SYMBIAN32__
  std_log(LOG_FILENAME_LINE, "%s: running recursive marshalling tests\n", "dbus-test");
  #endif
  if (!_dbus_marshal_recursive_test ())
    die ("recursive marshal");

  check_memleaks ();
#else
  _dbus_warn ("recursive marshal tests disabled\n");
  #ifdef __SYMBIAN32__
  std_log(LOG_FILENAME_LINE, "recursive marshal tests disabled\n");
  #endif
#endif

  run_test ("byteswap", specific_test, _dbus_marshal_byteswap_test);
  
  run_test ("memory", specific_test, _dbus_memory_test);

#if 1
  run_test ("mem-pool", specific_test, _dbus_mem_pool_test);
#endif
  
  run_test ("list", specific_test, _dbus_list_test);

  run_test ("marshal-validate", specific_test, _dbus_marshal_validate_test);

  run_test ("marshal-header", specific_test, _dbus_marshal_header_test);
  
  run_data_test ("message", specific_test, _dbus_message_test, test_data_dir);
  
  run_test ("hash", specific_test, _dbus_hash_test);

  run_data_test ("spawn", specific_test, _dbus_spawn_test, test_data_dir);
  
  run_data_test ("userdb", specific_test, _dbus_userdb_test, test_data_dir);
  
  run_test ("keyring", specific_test, _dbus_keyring_test);
  
#if 0
  printf ("%s: running md5 tests\n", "dbus-test");
  #ifdef __SYMBIAN32__
  std_log(LOG_FILENAME_LINE, "%s: running md5 tests\n", "dbus-test");
  #endif
  if (!_dbus_md5_test ())
    die ("md5");

  check_memleaks ();
#endif
  
  run_data_test ("sha", specific_test, _dbus_sha_test, test_data_dir);
  
  run_data_test ("auth", specific_test, _dbus_auth_test, test_data_dir);

  run_data_test ("pending-call", specific_test, _dbus_pending_call_test, test_data_dir);
  
  printf ("%s: completed successfully\n", "dbus-test");
  #ifdef __SYMBIAN32__
  std_log(LOG_FILENAME_LINE, "%s: completed successfully\n", "dbus-test");
  create_xml(0);
  #endif
#else
  printf ("Not compiled with unit tests, not running any\n");
  #ifdef __SYMBIAN32__
  std_log(LOG_FILENAME_LINE, "Not compiled with unit tests, not running any\n");
  create_xml(0);
  #endif
#endif
}

