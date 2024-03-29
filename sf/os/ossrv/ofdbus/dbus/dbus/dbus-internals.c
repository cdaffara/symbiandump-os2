/* -*- mode: C; c-file-style: "gnu" -*- */
/* dbus-internals.c  random utility stuff (internal to D-Bus implementation)
 *
 * Copyright (C) 2002, 2003  Red Hat, Inc.
 * Portion Copyright � 2008 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
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
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "dbus-protocol.h"
#include "dbus-marshal-basic.h"
#include "dbus-test.h"
#include "dbus-internals.h"
#ifdef __SYMBIAN32__
#include "config.h"
#include "libdbus_wsd_solution.h"
#endif

#if EMULATOR
int GET_DBUS_WSD_VAR_NAME(_dbus_current_generation,dbus_memory,g)();
#define _dbus_current_generation (GET_DBUS_WSD_VAR_NAME(_dbus_current_generation,dbus_memory,g)())
#endif


/**
 * @defgroup DBusInternals D-Bus secret internal implementation details
 * @brief Documentation useful when developing or debugging D-Bus itself.
 * 
 */

/**
 * @defgroup DBusInternalsUtils Utilities and portability
 * @ingroup DBusInternals
 * @brief Utility functions (_dbus_assert(), _dbus_warn(), etc.)
 * @{
 */

/**
 * @def _dbus_assert
 *
 * Aborts with an error message if the condition is false.
 * 
 * @param condition condition which must be true.
 */

/**
 * @def _dbus_assert_not_reached
 *
 * Aborts with an error message if called.
 * The given explanation will be printed.
 * 
 * @param explanation explanation of what happened if the code was reached.
 */

/**
 * @def _DBUS_N_ELEMENTS
 *
 * Computes the number of elements in a fixed-size array using
 * sizeof().
 *
 * @param array the array to count elements in.
 */

/**
 * @def _DBUS_POINTER_TO_INT
 *
 * Safely casts a void* to an integer; should only be used on void*
 * that actually contain integers, for example one created with
 * _DBUS_INT_TO_POINTER.  Only guaranteed to preserve 32 bits.
 * (i.e. it's used to store 32-bit ints in pointers, but
 * can't be used to store 64-bit pointers in ints.)
 *
 * @param pointer pointer to extract an integer from.
 */
/**
 * @def _DBUS_INT_TO_POINTER
 *
 * Safely stuffs an integer into a pointer, to be extracted later with
 * _DBUS_POINTER_TO_INT. Only guaranteed to preserve 32 bits.
 *
 * @param integer the integer to stuff into a pointer.
 */
/**
 * @def _DBUS_ZERO
 *
 * Sets all bits in an object to zero.
 *
 * @param object the object to be zeroed.
 */
/**
 * @def _DBUS_INT16_MIN
 *
 * Minimum value of type "int16"
 */
/**
 * @def _DBUS_INT16_MAX
 *
 * Maximum value of type "int16"
 */
/**
 * @def _DBUS_UINT16_MAX
 *
 * Maximum value of type "uint16"
 */

/**
 * @def _DBUS_INT32_MIN
 *
 * Minimum value of type "int32"
 */
/**
 * @def _DBUS_INT32_MAX
 *
 * Maximum value of type "int32"
 */
/**
 * @def _DBUS_UINT32_MAX
 *
 * Maximum value of type "uint32"
 */

/**
 * @def _DBUS_INT_MIN
 *
 * Minimum value of type "int"
 */
/**
 * @def _DBUS_INT_MAX
 *
 * Maximum value of type "int"
 */
/**
 * @def _DBUS_UINT_MAX
 *
 * Maximum value of type "uint"
 */

/**
 * @typedef DBusForeachFunction
 * 
 * Used to iterate over each item in a collection, such as
 * a DBusList.
 */

/**
 * @def _DBUS_LOCK_NAME
 *
 * Expands to name of a global lock variable.
 */

/**
 * @def _DBUS_DEFINE_GLOBAL_LOCK
 *
 * Defines a global lock variable with the given name.
 * The lock must be added to the list to initialize
 * in dbus_threads_init().
 */

/**
 * @def _DBUS_DECLARE_GLOBAL_LOCK
 *
 * Expands to declaration of a global lock defined
 * with _DBUS_DEFINE_GLOBAL_LOCK.
 * The lock must be added to the list to initialize
 * in dbus_threads_init().
 */

/**
 * @def _DBUS_LOCK
 *
 * Locks a global lock
 */

/**
 * @def _DBUS_UNLOCK
 *
 * Unlocks a global lock
 */

/**
 * Fixed "out of memory" error message, just to avoid
 * making up a different string every time and wasting
 * space.
 */
#ifdef __SYMBIAN32__
 
 EXPORT_C 
char * symbian_dbus_no_memory_message()
{	
	
	return "Not enough memory";
}
#else
const char _dbus_no_memory_message[] = "Not enough memory";
#endif

#if EMULATOR
GET_STATIC_VAR_FROM_TLS(warn_initted,dbus_internal,dbus_bool_t)
#define warn_initted (*GET_DBUS_WSD_VAR_NAME(warn_initted,dbus_internal,s)())

GET_STATIC_VAR_FROM_TLS(fatal_warnings,dbus_internal,dbus_bool_t)
#define fatal_warnings (*GET_DBUS_WSD_VAR_NAME(fatal_warnings,dbus_internal,s)())

GET_STATIC_VAR_FROM_TLS(fatal_warnings_on_check_failed,dbus_internal,dbus_bool_t)
#define fatal_warnings_on_check_failed (*GET_DBUS_WSD_VAR_NAME(fatal_warnings_on_check_failed,dbus_internal,s)())
#else
static dbus_bool_t warn_initted = FALSE;
static dbus_bool_t fatal_warnings = FALSE;
static dbus_bool_t fatal_warnings_on_check_failed = TRUE;
#endif

static void
init_warnings(void)
{
  if (!warn_initted)
    {
      const char *s;
      s = _dbus_getenv ("DBUS_FATAL_WARNINGS");
      if (s && *s)
        {
          if (*s == '0')
            {
              fatal_warnings = FALSE;
              fatal_warnings_on_check_failed = FALSE;
            }
          else if (*s == '1')
            {
              fatal_warnings = TRUE;
              fatal_warnings_on_check_failed = TRUE;
            }
          else
            {
              fprintf(stderr, "DBUS_FATAL_WARNINGS should be set to 0 or 1 if set, not '%s'",
                      s);
            }
        }

      warn_initted = TRUE;
    }
}

/**
 * Prints a warning message to stderr. Can optionally be made to exit
 * fatally by setting DBUS_FATAL_WARNINGS, but this is rarely
 * used. This function should be considered pretty much equivalent to
 * fprintf(stderr). _dbus_warn_check_failed() on the other hand is
 * suitable for use when a programming mistake has been made.
 *
 * @param format printf-style format string.
 */

#ifdef __SYMBIAN32__
EXPORT_C
#endif
void
_dbus_warn (const char *format,
            ...)
{
  va_list args;

  if (!warn_initted)
    init_warnings ();
  
  va_start (args, format);
  vfprintf (stderr, format, args);
  va_end (args);

  if (fatal_warnings)
    {
      fflush (stderr);
      _dbus_abort ();
    }
}

/**
 * Prints a "critical" warning to stderr when an assertion fails;
 * differs from _dbus_warn primarily in that it prefixes the pid and
 * defaults to fatal. This should be used only when a programming
 * error has been detected. (NOT for unavoidable errors that an app
 * might handle - those should be returned as DBusError.) Calling this
 * means "there is a bug"
 */
#ifdef __SYMBIAN32__
 EXPORT_C
 #endif
void
_dbus_warn_check_failed(const char *format,
                        ...)
{
  va_list args;
  
  if (!warn_initted)
    init_warnings ();

  fprintf (stderr, "process %lu: ", _dbus_getpid ());
  
  va_start (args, format);
  vfprintf (stderr, format, args);
  va_end (args);

  if (fatal_warnings_on_check_failed)
    {
      fflush (stderr);
      _dbus_abort ();
    }
}

#ifdef __SYMBIAN32__
#ifndef DBUS_ENABLE_VERBOSE_MODE
#define DBUS_ENABLE_VERBOSE_MODE 1
#endif //DBUS_ENABLE_VERBOSE_MODE
#endif //__SYMBIAN32__


#ifdef DBUS_ENABLE_VERBOSE_MODE

#if EMULATOR
GET_STATIC_VAR_FROM_TLS(verbose_initted,dbus_internals,dbus_bool_t)
#define verbose_initted (*GET_DBUS_WSD_VAR_NAME(verbose_initted,dbus_internals,s)())

GET_STATIC_VAR_FROM_TLS(verbose,dbus_internals,dbus_bool_t)
#define verbose (*GET_DBUS_WSD_VAR_NAME(verbose,dbus_internals,s)())

#else
static dbus_bool_t verbose_initted = FALSE;
static dbus_bool_t verbose = TRUE;
#endif

/** Whether to show the current thread in verbose messages */
#define PTHREAD_IN_VERBOSE 0
#if PTHREAD_IN_VERBOSE
#include <pthread.h>
#endif

static inline void
_dbus_verbose_init (void)
{
  if (!verbose_initted)
    {
      const char *p = _dbus_getenv ("DBUS_VERBOSE"); 
      verbose = p != NULL && *p == '1';
      verbose_initted = TRUE;
    }
}

/**
 * Implementation of dbus_is_verbose() macro if built with verbose logging
 * enabled.
 * @returns whether verbose logging is active.
 */
dbus_bool_t
_dbus_is_verbose_real (void)
{
  _dbus_verbose_init ();
  return verbose;
}

/**
 * Prints a warning message to stderr
 * if the user has enabled verbose mode.
 * This is the real function implementation,
 * use _dbus_verbose() macro in code.
 *
 * @param format printf-style format string.
 */
#if EMULATOR
GET_STATIC_VAR_FROM_TLS(need_pid,dbus_internals,dbus_bool_t)
#define need_pid (*GET_DBUS_WSD_VAR_NAME(need_pid,dbus_internals,s)())
#endif

#ifdef __SYMBIAN32__
EXPORT_C 
#endif
void
_dbus_verbose_real (const char *format,
                    ...)
{
  va_list args;
#if !(EMULATOR)
  static dbus_bool_t need_pid = TRUE;
#endif // EMULATOR  
  int len;
  
  /* things are written a bit oddly here so that
   * in the non-verbose case we just have the one
   * conditional and return immediately.
   */
  if (!_dbus_is_verbose_real())
    return;

  /* Print out pid before the line */
  if (need_pid)
    {
#if PTHREAD_IN_VERBOSE
      fprintf (stderr, "%lu: 0x%lx: ", _dbus_getpid (), pthread_self ());
#else
      fprintf (stderr, "%lu: ", _dbus_getpid ());
#endif
    }
      

  /* Only print pid again if the next line is a new line */
  len = strlen (format);
  if (format[len-1] == '\n')
    need_pid = TRUE;
  else
    need_pid = FALSE;
  
  va_start (args, format);
  vfprintf (stderr, format, args);
  va_end (args);

  fflush (stderr);
}

#if EMULATOR
#undef need_pid
#endif //EMULATOR

/**
 * Reinitializes the verbose logging code, used
 * as a hack in dbus-spawn.c so that a child
 * process re-reads its pid
 *
 */
#ifdef __SYMBIAN32__
 EXPORT_C
 #endif
 void
_dbus_verbose_reset_real (void)
{
  verbose_initted = FALSE;
}

#endif /* DBUS_ENABLE_VERBOSE_MODE */

/**
 * Duplicates a string. Result must be freed with
 * dbus_free(). Returns #NULL if memory allocation fails.
 * If the string to be duplicated is #NULL, returns #NULL.
 * 
 * @param str string to duplicate.
 * @returns newly-allocated copy.
 */

#ifdef __SYMBIAN32__
 EXPORT_C
 #endif
 char*
_dbus_strdup (const char *str)
{
  size_t len;
  char *copy;
  
  if (str == NULL)
    return NULL;
  
  len = strlen (str);

  copy = dbus_malloc (len + 1);
  if (copy == NULL)
    return NULL;

  memcpy (copy, str, len + 1);
  
  return copy;
}

/**
 * Duplicates a block of memory. Returns
 * #NULL on failure.
 *
 * @param mem memory to copy
 * @param n_bytes number of bytes to copy
 * @returns the copy
 */
void*
_dbus_memdup (const void  *mem,
              size_t       n_bytes)
{
  void *copy;

  copy = dbus_malloc (n_bytes);
  if (copy == NULL)
    return NULL;

  memcpy (copy, mem, n_bytes);
  
  return copy;
}

/**
 * Duplicates a string array. Result may be freed with
 * dbus_free_string_array(). Returns #NULL if memory allocation fails.
 * If the array to be duplicated is #NULL, returns #NULL.
 * 
 * @param array array to duplicate.
 * @returns newly-allocated copy.
 */
char**
_dbus_dup_string_array (const char **array)
{
  int len;
  int i;
  char **copy;
  
  if (array == NULL)
    return NULL;

  for (len = 0; array[len] != NULL; ++len)
    ;

  copy = dbus_new0 (char*, len + 1);
  if (copy == NULL)
    return NULL;

  i = 0;
  while (i < len)
    {
      copy[i] = _dbus_strdup (array[i]);
      if (copy[i] == NULL)
        {
          dbus_free_string_array (copy);
          return NULL;
        }

      ++i;
    }

  return copy;
}

/**
 * Checks whether a string array contains the given string.
 * 
 * @param array array to search.
 * @param str string to look for
 * @returns #TRUE if array contains string
 */
#ifdef __SYMBIAN32__
 EXPORT_C
 #endif
dbus_bool_t
_dbus_string_array_contains (const char **array,
                             const char  *str)
{
  int i;

  i = 0;
  while (array[i] != NULL)
    {
      if (strcmp (array[i], str) == 0)
        return TRUE;
      ++i;
    }

  return FALSE;
}

/**
 * Generates a new UUID. If you change how this is done,
 * there's some text about it in the spec that should also change.
 *
 * @param uuid the uuid to initialize
 */
void
_dbus_generate_uuid (DBusGUID *uuid)
{
  long now;

  _dbus_get_current_time (&now, NULL);

  uuid->as_uint32s[DBUS_UUID_LENGTH_WORDS - 1] = DBUS_UINT32_TO_BE (now);
  
  _dbus_generate_random_bytes_buffer (uuid->as_bytes, DBUS_UUID_LENGTH_BYTES - 4);
}

/**
 * Hex-encode a UUID.
 *
 * @param uuid the uuid
 * @param encoded string to append hex uuid to
 * @returns #FALSE if no memory
 */
dbus_bool_t
_dbus_uuid_encode (const DBusGUID *uuid,
                   DBusString     *encoded)
{
  DBusString binary;
  _dbus_string_init_const_len (&binary, uuid->as_bytes, DBUS_UUID_LENGTH_BYTES);
  return _dbus_string_hex_encode (&binary, 0, encoded, _dbus_string_get_length (encoded));
}

static dbus_bool_t
_dbus_read_uuid_file_without_creating (const DBusString *filename,
                                       DBusGUID         *uuid,
                                       DBusError        *error)
{
  DBusString contents;
  DBusString decoded;
  int end;
  
  _dbus_string_init (&contents);
  _dbus_string_init (&decoded);
  
  if (!_dbus_file_get_contents (&contents, filename, error))
    goto error;

  _dbus_string_chop_white (&contents);

  if (_dbus_string_get_length (&contents) != DBUS_UUID_LENGTH_HEX)
    {
      dbus_set_error (error, DBUS_ERROR_INVALID_FILE_CONTENT,
                      "UUID file '%s' should contain a hex string of length %d, not length %d, with no other text",
                      _dbus_string_get_const_data (filename),
                      DBUS_UUID_LENGTH_HEX,
                      _dbus_string_get_length (&contents));
      goto error;
    }

  if (!_dbus_string_hex_decode (&contents, 0, &end, &decoded, 0))
    {
      _DBUS_SET_OOM (error);
      goto error;
    }

  if (end == 0)
    {
      dbus_set_error (error, DBUS_ERROR_INVALID_FILE_CONTENT,
                      "UUID file '%s' contains invalid hex data",
                      _dbus_string_get_const_data (filename));
      goto error;
    }

  if (_dbus_string_get_length (&decoded) != DBUS_UUID_LENGTH_BYTES)
    {
      dbus_set_error (error, DBUS_ERROR_INVALID_FILE_CONTENT,
                      "UUID file '%s' contains %d bytes of hex-encoded data instead of %d",
                      _dbus_string_get_const_data (filename),
                      _dbus_string_get_length (&decoded),
                      DBUS_UUID_LENGTH_BYTES);
      goto error;
    }

  _dbus_string_copy_to_buffer (&decoded, uuid->as_bytes, DBUS_UUID_LENGTH_BYTES);

  _dbus_string_free (&decoded);
  _dbus_string_free (&contents);

  _DBUS_ASSERT_ERROR_IS_CLEAR (error);

  return TRUE;
  
 error:
  _DBUS_ASSERT_ERROR_IS_SET (error);
  _dbus_string_free (&contents);
  _dbus_string_free (&decoded);
  return FALSE;
}

static dbus_bool_t
_dbus_create_uuid_file_exclusively (const DBusString *filename,
                                    DBusGUID         *uuid,
                                    DBusError        *error)
{
  DBusString encoded;

  _dbus_string_init (&encoded);

  _dbus_generate_uuid (uuid);
  
  if (!_dbus_uuid_encode (uuid, &encoded))
    {
      _DBUS_SET_OOM (error);
      goto error;
    }
  
  /* FIXME this is racy; we need a save_file_exclusively
   * function. But in practice this should be fine for now.
   *
   * - first be sure we can create the file and it
   *   doesn't exist by creating it empty with O_EXCL
   * - then create it by creating a temporary file and
   *   overwriting atomically with rename()
   */
  if (!_dbus_create_file_exclusively (filename, error))
    goto error;

  if (!_dbus_string_append_byte (&encoded, '\n'))
    {
      _DBUS_SET_OOM (error);
      goto error;
    }
  
  if (!_dbus_string_save_to_file (&encoded, filename, error))
    goto error;

  if (!_dbus_make_file_world_readable (filename, error))
    goto error;

  _dbus_string_free (&encoded);

  _DBUS_ASSERT_ERROR_IS_CLEAR (error);
  return TRUE;
  
 error:
  _DBUS_ASSERT_ERROR_IS_SET (error);
  _dbus_string_free (&encoded);
  return FALSE;        
}

/**
 * Reads (and optionally writes) a uuid to a file. Initializes the uuid
 * unless an error is returned.
 *
 * @param filename the name of the file
 * @param uuid uuid to be initialized with the loaded uuid
 * @param create_if_not_found #TRUE to create a new uuid and save it if the file doesn't exist
 * @param error the error return
 * @returns #FALSE if the error is set
 */
dbus_bool_t
_dbus_read_uuid_file (const DBusString *filename,
                      DBusGUID         *uuid,
                      dbus_bool_t       create_if_not_found,
                      DBusError        *error)
{
  DBusError read_error;

  dbus_error_init (&read_error);

  if (_dbus_read_uuid_file_without_creating (filename, uuid, &read_error))
    return TRUE;

  if (!create_if_not_found)
    {
      dbus_move_error (&read_error, error);
      return FALSE;
    }

  /* If the file exists and contains junk, we want to keep that error
   * message instead of overwriting it with a "file exists" error
   * message when we try to write
   */
  if (dbus_error_has_name (&read_error, DBUS_ERROR_INVALID_FILE_CONTENT))
    {
      dbus_move_error (&read_error, error);
      return FALSE;
    }
  else
    {
      dbus_error_free (&read_error);
      return _dbus_create_uuid_file_exclusively (filename, uuid, error);
    }
}
#if EMULATOR
GET_GLOBAL_VAR_FROM_TLS(_dbus_lock_machine_uuid,dbus_internals,DBusMutex *)
#define _dbus_lock_machine_uuid (*GET_DBUS_WSD_VAR_NAME(_dbus_lock_machine_uuid,dbus_internals,g)())
#else
_DBUS_DEFINE_GLOBAL_LOCK (machine_uuid);
#endif
#if EMULATOR
GET_STATIC_VAR_FROM_TLS(machine_uuid_initialized_generation,dbus_internals,int)
#define machine_uuid_initialized_generation (*GET_DBUS_WSD_VAR_NAME(machine_uuid_initialized_generation,dbus_internals,s)())

GET_STATIC_VAR_FROM_TLS(machine_uuid,dbus_internals,DBusGUID)
#define machine_uuid (*GET_DBUS_WSD_VAR_NAME(machine_uuid,dbus_internals,s)())

#else
static int machine_uuid_initialized_generation = 0;
static DBusGUID machine_uuid;
#endif

/**
 * Gets the hex-encoded UUID of the machine this function is
 * executed on. This UUID is guaranteed to be the same for a given
 * machine at least until it next reboots, though it also
 * makes some effort to be the same forever, it may change if the
 * machine is reconfigured or its hardware is modified.
 * 
 * @param uuid_str string to append hex-encoded machine uuid to
 * @returns #FALSE if no memory
 */
dbus_bool_t
_dbus_get_local_machine_uuid_encoded (DBusString *uuid_str)
{
  dbus_bool_t ok;
  
  _DBUS_LOCK (machine_uuid);
  if (machine_uuid_initialized_generation != _dbus_current_generation)
    {
      DBusError error;
      dbus_error_init (&error);
      if (!_dbus_read_local_machine_uuid (&machine_uuid, FALSE,
                                          &error))
        {          
#if 0        
#ifndef DBUS_BUILD_TESTS
          /* For the test suite, we may not be installed so just continue silently
           * here. But in a production build, we want to be nice and loud about
           * this.
           */
          _dbus_warn_check_failed ("D-Bus library appears to be incorrectly set up; failed to read machine uuid: %s\n"
                                   "See the manual page for dbus-uuidgen to correct this issue.\n",
                                   error.message);
#endif
#endif
          
          dbus_error_free (&error);
          
          _dbus_generate_uuid (&machine_uuid);
        }
    }

  ok = _dbus_uuid_encode (&machine_uuid, uuid_str);

  _DBUS_UNLOCK (machine_uuid);

  return ok;
}

#ifdef DBUS_BUILD_TESTS
/**
 * Returns a string describing the given name.
 *
 * @param header_field the field to describe
 * @returns a constant string describing the field
 */
const char *
_dbus_header_field_to_string (int header_field)
{
  switch (header_field)
    {
    case DBUS_HEADER_FIELD_INVALID:
      return "invalid";
    case DBUS_HEADER_FIELD_PATH:
      return "path";
    case DBUS_HEADER_FIELD_INTERFACE:
      return "interface";
    case DBUS_HEADER_FIELD_MEMBER:
      return "member";
    case DBUS_HEADER_FIELD_ERROR_NAME:
      return "error-name";
    case DBUS_HEADER_FIELD_REPLY_SERIAL:
      return "reply-serial";
    case DBUS_HEADER_FIELD_DESTINATION:
      return "destination";
    case DBUS_HEADER_FIELD_SENDER:
      return "sender";
    case DBUS_HEADER_FIELD_SIGNATURE:
      return "signature";
    default:
      return "unknown";
    }
}
#endif /* DBUS_BUILD_TESTS */

#ifndef DBUS_DISABLE_CHECKS
/** String used in _dbus_return_if_fail macro */
const char _dbus_return_if_fail_warning_format[] =
"arguments to %s() were incorrect, assertion \"%s\" failed in file %s line %d.\n"
"This is normally a bug in some application using the D-Bus library.\n";
#endif

#ifndef DBUS_DISABLE_ASSERT
/**
 * Internals of _dbus_assert(); it's a function
 * rather than a macro with the inline code so
 * that the assertion failure blocks don't show up
 * in test suite coverage, and to shrink code size.
 *
 * @param condition TRUE if assertion succeeded
 * @param condition_text condition as a string
 * @param file file the assertion is in
 * @param line line the assertion is in
 * @param func function the assertion is in
 */
void
_dbus_real_assert (dbus_bool_t  condition,
                   const char  *condition_text,
                   const char  *file,
                   int          line,
                   const char  *func)
{
  if (_DBUS_UNLIKELY (!condition))
    {
      _dbus_warn ("%lu: assertion failed \"%s\" file \"%s\" line %d function %s\n",
                  _dbus_getpid (), condition_text, file, line, func);
      _dbus_abort ();
    }
}

/**
 * Internals of _dbus_assert_not_reached(); it's a function
 * rather than a macro with the inline code so
 * that the assertion failure blocks don't show up
 * in test suite coverage, and to shrink code size.
 *
 * @param explanation what was reached that shouldn't have been
 * @param file file the assertion is in
 * @param line line the assertion is in
 */
void
_dbus_real_assert_not_reached (const char *explanation,
                               const char *file,
                               int         line)
{
  _dbus_warn ("File \"%s\" line %d process %lu should not have been reached: %s\n",
              file, line, _dbus_getpid (), explanation);
  _dbus_abort ();
}
#endif /* DBUS_DISABLE_ASSERT */
  
#ifdef DBUS_BUILD_TESTS
static dbus_bool_t
run_failing_each_malloc (int                    n_mallocs,
                         const char            *description,
                         DBusTestMemoryFunction func,
                         void                  *data)
{
  n_mallocs += 10; /* fudge factor to ensure reallocs etc. are covered */
  
  while (n_mallocs >= 0)
    {      
      _dbus_set_fail_alloc_counter (n_mallocs);

      _dbus_verbose ("\n===\n%s: (will fail malloc %d with %d failures)\n===\n",
                     description, n_mallocs,
                     _dbus_get_fail_alloc_failures ());

      if (!(* func) (data))
        return FALSE;
      
      n_mallocs -= 1;
    }

  _dbus_set_fail_alloc_counter (_DBUS_INT_MAX);

  return TRUE;
}                        

/**
 * Tests how well the given function responds to out-of-memory
 * situations. Calls the function repeatedly, failing a different
 * call to malloc() each time. If the function ever returns #FALSE,
 * the test fails. The function should return #TRUE whenever something
 * valid (such as returning an error, or succeeding) occurs, and #FALSE
 * if it gets confused in some way.
 *
 * @param description description of the test used in verbose output
 * @param func function to call
 * @param data data to pass to function
 * @returns #TRUE if the function never returns FALSE
 */
#ifdef __SYMBIAN32__
 EXPORT_C
 #endif
 dbus_bool_t
_dbus_test_oom_handling (const char             *description,
                         DBusTestMemoryFunction  func,
                         void                   *data)
{
  int approx_mallocs;
  const char *setting;
  int max_failures_to_try;
  int i;

  /* Run once to see about how many mallocs are involved */
  
  _dbus_set_fail_alloc_counter (_DBUS_INT_MAX);

  _dbus_verbose ("Running once to count mallocs\n");
  
  if (!(* func) (data))
    return FALSE;
  
  approx_mallocs = _DBUS_INT_MAX - _dbus_get_fail_alloc_counter ();

  _dbus_verbose ("\n=================\n%s: about %d mallocs total\n=================\n",
                 description, approx_mallocs);

  setting = _dbus_getenv ("DBUS_TEST_MALLOC_FAILURES");
  if (setting != NULL)
    {
      DBusString str;
      long v;
      _dbus_string_init_const (&str, setting);
      v = 4;
      if (!_dbus_string_parse_int (&str, 0, &v, NULL))
        _dbus_warn ("couldn't parse '%s' as integer\n", setting);
      max_failures_to_try = v;
    }
  else
    {
      max_failures_to_try = 4;
    }

  i = setting ? max_failures_to_try - 1 : 1;
  while (i < max_failures_to_try)
    {
      _dbus_set_fail_alloc_failures (i);
      if (!run_failing_each_malloc (approx_mallocs, description, func, data))
        return FALSE;
      ++i;
    }
  
  _dbus_verbose ("\n=================\n%s: all iterations passed\n=================\n",
                 description);

  return TRUE;
}
#endif /* DBUS_BUILD_TESTS */

/** @} */
