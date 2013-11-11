/* GLib testing utilities
 * Copyright (C) 2007 Imendio AB
 * Authors: Tim Janik
 * Portions copyright (c) 2009 Nokia Corporation.  All rights reserved.
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

#if defined(G_DISABLE_SINGLE_INCLUDES) && !defined (__GLIB_H_INSIDE__) && !defined (GLIB_COMPILATION)
#error "Only <glib.h> can be included directly."
#endif

#ifndef __G_TEST_UTILS_H__
#define __G_TEST_UTILS_H__

#include <glib.h>

G_BEGIN_DECLS

typedef struct GTestCase  GTestCase;
typedef struct GTestSuite GTestSuite;

/* assertion API */
#define g_assert_cmpstr(s1, cmp, s2)    do { const char *__s1 = (s1), *__s2 = (s2); \
                                             if (g_strcmp0 (__s1, __s2) cmp 0) ; else \
                                               g_assertion_message_cmpstr (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, \
                                                 #s1 " " #cmp " " #s2, __s1, #cmp, __s2); } while (0)
#define g_assert_cmpint(n1, cmp, n2)    do { gint64 __n1 = (n1), __n2 = (n2); \
                                             if (__n1 cmp __n2) ; else \
                                               g_assertion_message_cmpnum (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, \
                                                 #n1 " " #cmp " " #n2, __n1, #cmp, __n2, 'i'); } while (0)
#define g_assert_cmpuint(n1, cmp, n2)   do { guint64 __n1 = (n1), __n2 = (n2); \
                                             if (__n1 cmp __n2) ; else \
                                               g_assertion_message_cmpnum (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, \
                                                 #n1 " " #cmp " " #n2, __n1, #cmp, __n2, 'i'); } while (0)
#define g_assert_cmphex(n1, cmp, n2)    do { guint64 __n1 = (n1), __n2 = (n2); \
                                             if (__n1 cmp __n2) ; else \
                                               g_assertion_message_cmpnum (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, \
                                                 #n1 " " #cmp " " #n2, __n1, #cmp, __n2, 'x'); } while (0)
#define g_assert_cmpfloat(n1,cmp,n2)    do { long double __n1 = (n1), __n2 = (n2); \
                                             if (__n1 cmp __n2) ; else \
                                               g_assertion_message_cmpnum (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, \
                                                 #n1 " " #cmp " " #n2, __n1, #cmp, __n2, 'f'); } while (0)
#define g_assert_no_error(err)          do { if (err) \
                                               g_assertion_message_error (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, \
                                                 #err, err, 0, 0); } while (0)
#define g_assert_error(err, dom, c)	do { if (!err || (err)->domain != dom || (err)->code != c) \
                                               g_assertion_message_error (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, \
                                                 #err, err, dom, c); } while (0)
#ifdef G_DISABLE_ASSERT
#define g_assert_not_reached()          do { (void) 0; } while (0)
#define g_assert(expr)                  do { (void) 0; } while (0)
#else /* !G_DISABLE_ASSERT */
#define g_assert_not_reached()          do { g_assertion_message (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, NULL); } while (0)
#define g_assert(expr)                  do { if G_LIKELY (expr) ; else \
                                               g_assertion_message_expr (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, \
                                                 #expr); } while (0)
#endif /* !G_DISABLE_ASSERT */

IMPORT_C int     g_strcmp0                       (const char     *str1,
                                         const char     *str2);

/* report performance results */
IMPORT_C void    g_test_minimized_result         (double          minimized_quantity,
                                         const char     *format,
                                         ...) G_GNUC_PRINTF (2, 3);
IMPORT_C void    g_test_maximized_result         (double          maximized_quantity,
                                         const char     *format,
                                         ...) G_GNUC_PRINTF (2, 3);

/* initialize testing framework */
IMPORT_C void    g_test_init                     (int            *argc,
                                         char         ***argv,
                                         ...);
/* query testing framework config */
#define g_test_quick()                  (g_test_config_vars->test_quick)
#define g_test_slow()                   (!g_test_config_vars->test_quick)
#define g_test_thorough()               (!g_test_config_vars->test_quick)
#define g_test_perf()                   (g_test_config_vars->test_perf)
#define g_test_verbose()                (g_test_config_vars->test_verbose)
#define g_test_quiet()                  (g_test_config_vars->test_quiet)
/* run all tests under toplevel suite (path: /) */
IMPORT_C int     g_test_run                      (void);
/* hook up a test functions under test path */
IMPORT_C void    g_test_add_func                 (const char     *testpath,
                                         void          (*test_func) (void));
IMPORT_C void    g_test_add_data_func            (const char     *testpath,
                                         gconstpointer   test_data,
                                         void          (*test_func) (gconstpointer));
/* hook up a test with fixture under test path */
#define g_test_add(testpath, Fixture, tdata, fsetup, ftest, fteardown) \
					G_STMT_START {			\
                                         void (*add_vtable) (const char*,       \
                                                    gsize,             \
                                                    gconstpointer,     \
                                                    void (*) (Fixture*, gconstpointer),   \
                                                    void (*) (Fixture*, gconstpointer),   \
                                                    void (*) (Fixture*, gconstpointer)) =  (void (*) (const gchar *, gsize, gconstpointer, void (*) (Fixture*, gconstpointer), void (*) (Fixture*, gconstpointer), void (*) (Fixture*, gconstpointer))) g_test_add_vtable; \
                                         add_vtable \
                                          (testpath, sizeof (Fixture), tdata, fsetup, ftest, fteardown); \
					} G_STMT_END

/* add test messages to the test report */
IMPORT_C void    g_test_message                  (const char *format,
                                         ...) G_GNUC_PRINTF (1, 2);
IMPORT_C void    g_test_bug_base                 (const char *uri_pattern);
IMPORT_C void    g_test_bug                      (const char *bug_uri_snippet);
/* measure test timings */
IMPORT_C void    g_test_timer_start              (void);
IMPORT_C double  g_test_timer_elapsed            (void); /* elapsed seconds */
IMPORT_C double  g_test_timer_last               (void); /* repeat last elapsed() result */

/* automatically g_free or g_object_unref upon teardown */
IMPORT_C void    g_test_queue_free               (gpointer gfree_pointer);
IMPORT_C void    g_test_queue_destroy            (GDestroyNotify destroy_func,
                                         gpointer       destroy_data);
#define g_test_queue_unref(gobject)     g_test_queue_destroy (g_object_unref, gobject)

/* test traps are guards used around forked tests */
typedef enum {
  G_TEST_TRAP_SILENCE_STDOUT    = 1 << 7,
  G_TEST_TRAP_SILENCE_STDERR    = 1 << 8,
  G_TEST_TRAP_INHERIT_STDIN     = 1 << 9
} GTestTrapFlags;
IMPORT_C gboolean g_test_trap_fork               (guint64              usec_timeout,
                                         GTestTrapFlags       test_trap_flags);
IMPORT_C gboolean g_test_trap_has_passed         (void);
IMPORT_C gboolean g_test_trap_reached_timeout    (void);
#define  g_test_trap_assert_passed()                      g_test_trap_assertions (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, 0, 0)
#define  g_test_trap_assert_failed()                      g_test_trap_assertions (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, 1, 0)
#define  g_test_trap_assert_stdout(soutpattern)           g_test_trap_assertions (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, 2, soutpattern)
#define  g_test_trap_assert_stdout_unmatched(soutpattern) g_test_trap_assertions (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, 3, soutpattern)
#define  g_test_trap_assert_stderr(serrpattern)           g_test_trap_assertions (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, 4, serrpattern)
#define  g_test_trap_assert_stderr_unmatched(serrpattern) g_test_trap_assertions (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, 5, serrpattern)

/* provide seed-able random numbers for tests */
#define  g_test_rand_bit()              (0 != (g_test_rand_int() & (1 << 15)))
IMPORT_C gint32   g_test_rand_int                (void);
IMPORT_C gint32   g_test_rand_int_range          (gint32          begin,
                                         gint32          end);
IMPORT_C double   g_test_rand_double             (void);
IMPORT_C double   g_test_rand_double_range       (double          range_start,
                                         double          range_end);

/* semi-internal API */
IMPORT_C GTestCase*    g_test_create_case        (const char     *test_name,
                                         gsize           data_size,
                                         gconstpointer   test_data,
                                         void          (*data_setup) (void),
                                         void          (*data_test) (void),
                                         void          (*data_teardown) (void));
IMPORT_C GTestSuite*   g_test_create_suite       (const char     *suite_name);
IMPORT_C GTestSuite*   g_test_get_root           (void);
IMPORT_C void          g_test_suite_add          (GTestSuite     *suite,
                                         GTestCase      *test_case);
IMPORT_C void          g_test_suite_add_suite    (GTestSuite     *suite,
                                         GTestSuite     *nestedsuite);
IMPORT_C int           g_test_run_suite          (GTestSuite     *suite);

/* internal ABI */
IMPORT_C void    g_test_trap_assertions          (const char     *domain,
                                         const char     *file,
                                         int             line,
                                         const char     *func,
                                         guint64         assertion_flags, /* 0-pass, 1-fail, 2-outpattern, 4-errpattern */
                                         const char     *pattern);
IMPORT_C void    g_assertion_message             (const char     *domain,
                                         const char     *file,
                                         int             line,
                                         const char     *func,
                                         const char     *message) G_GNUC_NORETURN;
IMPORT_C void    g_assertion_message_expr        (const char     *domain,
                                         const char     *file,
                                         int             line,
                                         const char     *func,
                                         const char     *expr) G_GNUC_NORETURN;
IMPORT_C void    g_assertion_message_cmpstr      (const char     *domain,
                                         const char     *file,
                                         int             line,
                                         const char     *func,
                                         const char     *expr,
                                         const char     *arg1,
                                         const char     *cmp,
                                         const char     *arg2) G_GNUC_NORETURN;
IMPORT_C void    g_assertion_message_cmpnum      (const char     *domain,
                                         const char     *file,
                                         int             line,
                                         const char     *func,
                                         const char     *expr,
                                         long double     arg1,
                                         const char     *cmp,
                                         long double     arg2,
                                         char            numtype) G_GNUC_NORETURN;
IMPORT_C void    g_assertion_message_error       (const char     *domain,
                                         const char     *file,
                                         int             line,
                                         const char     *func,
                                         const char     *expr,
                                         GError         *error,
                                         GQuark          error_domain,
                                         int             error_code) G_GNUC_NORETURN;
IMPORT_C void    g_test_add_vtable               (const char     *testpath,
                                         gsize           data_size,
                                         gconstpointer   test_data,
                                         void          (*data_setup)    (void),
                                         void          (*data_test)     (void),
                                         void          (*data_teardown) (void));
typedef struct {
  gboolean      test_initialized;
  gboolean      test_quick;     /* disable thorough tests */
  gboolean      test_perf;      /* run performance tests */
  gboolean      test_verbose;   /* extra info */
  gboolean      test_quiet;     /* reduce output */
} GTestConfig;

GLIB_VAR const GTestConfig * const g_test_config_vars;
#ifdef __SYMBIAN32__
IMPORT_C const GTestConfig * const *_g_test_config_vars(void);
//#define g_test_config_vars (*_g_test_config_vars())
#endif /*__SYMBIAN32__*/
/* internal logging API */
typedef enum {
  G_TEST_LOG_NONE,
  G_TEST_LOG_ERROR,             /* s:msg */
  G_TEST_LOG_START_BINARY,      /* s:binaryname s:seed */
  G_TEST_LOG_LIST_CASE,         /* s:testpath */
  G_TEST_LOG_SKIP_CASE,         /* s:testpath */
  G_TEST_LOG_START_CASE,        /* s:testpath */
  G_TEST_LOG_STOP_CASE,         /* d:status d:nforks d:elapsed */
  G_TEST_LOG_MIN_RESULT,        /* s:blurb d:result */
  G_TEST_LOG_MAX_RESULT,        /* s:blurb d:result */
  G_TEST_LOG_MESSAGE            /* s:blurb */
} GTestLogType;

typedef struct GTestLogMsg{
  GTestLogType  log_type;
  guint         n_strings;
  gchar       **strings; /* NULL terminated */
  guint         n_nums;
  long double  *nums;
} GTestLogMsg;
typedef struct GTestLogBuffer{
  /*< private >*/
  GString     *data;
  GSList      *msgs;
} GTestLogBuffer;

IMPORT_C const char*     g_test_log_type_name    (GTestLogType    log_type);
IMPORT_C GTestLogBuffer* g_test_log_buffer_new   (void);
IMPORT_C void            g_test_log_buffer_free  (GTestLogBuffer *tbuffer);
IMPORT_C void            g_test_log_buffer_push  (GTestLogBuffer *tbuffer,
                                         guint           n_bytes,
                                         const guint8   *bytes);
IMPORT_C GTestLogMsg*    g_test_log_buffer_pop   (GTestLogBuffer *tbuffer);
IMPORT_C void            g_test_log_msg_free     (GTestLogMsg    *tmsg);

G_END_DECLS

#endif /* __G_TEST_UTILS_H__ */
