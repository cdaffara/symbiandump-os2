/* Portions copyright (c) 2009 Nokia Corporation.  All rights reserved. */
#undef G_DISABLE_ASSERT
#undef G_LOG_DOMAIN
#define G_ERRORCHECK_MUTEXES

#include <glib.h>
#include <stdio.h>
#include <string.h>
#ifdef __SYMBIAN32__
#include <glib_global.h>
#include "mrt2_glib2_test.h"
#endif /*__SYMBIAN32__*/
static gpointer
locking_thread (gpointer mutex)
{
  g_mutex_lock ((GMutex*)mutex);

  return NULL;
}

static void
lock_locked_mutex (void)
{
  GMutex* mutex = g_mutex_new ();
  g_mutex_lock (mutex);
  g_mutex_lock (mutex);
}

static void
trylock_locked_mutex (void)
{
  GMutex* mutex = g_mutex_new ();
  g_mutex_lock (mutex);
  g_mutex_trylock (mutex);
}

static void
unlock_unlocked_mutex (void)
{
  GMutex* mutex = g_mutex_new ();
  g_mutex_lock (mutex);
  g_mutex_unlock (mutex);
  g_mutex_unlock (mutex);
}

static void
free_locked_mutex (void)
{
  GMutex* mutex = g_mutex_new ();
  g_mutex_lock (mutex);
  g_mutex_free (mutex);
}

static void
wait_on_unlocked_mutex (void)
{
  GMutex* mutex = g_mutex_new ();
  GCond* cond = g_cond_new ();
  g_cond_wait (cond, mutex);
}

static void
wait_on_otherwise_locked_mutex (void)
{
  GMutex* mutex = g_mutex_new ();
  GCond* cond = g_cond_new ();
  GThread* thread = g_thread_create (locking_thread, mutex, TRUE, NULL);
  g_assert (thread != NULL);
  g_usleep (G_USEC_PER_SEC);
  g_cond_wait (cond, mutex);
}

static void
timed_wait_on_unlocked_mutex (void)
{
  GMutex* mutex = g_mutex_new ();
  GCond* cond = g_cond_new ();
  g_cond_timed_wait (cond, mutex, NULL);
}

static void
timed_wait_on_otherwise_locked_mutex (void)
{
  GMutex* mutex = g_mutex_new ();
  GCond* cond = g_cond_new ();
  GThread* thread = g_thread_create (locking_thread, mutex, TRUE, NULL);
  g_assert (thread != NULL);
  g_usleep (G_USEC_PER_SEC);
  g_cond_timed_wait (cond, mutex, NULL);
}

struct
{
  char *name;
  void (*func)();
} func_table[] =
{
  {"lock_locked_mutex", lock_locked_mutex},
  {"trylock_locked_mutex", trylock_locked_mutex},
  {"unlock_unlocked_mutex", unlock_unlocked_mutex},
  {"free_locked_mutex", free_locked_mutex},
  {"wait_on_unlocked_mutex", wait_on_unlocked_mutex},
  {"wait_on_otherwise_locked_mutex", wait_on_otherwise_locked_mutex},
  {"timed_wait_on_unlocked_mutex", timed_wait_on_unlocked_mutex},
  {"timed_wait_on_otherwise_locked_mutex",
   timed_wait_on_otherwise_locked_mutex}
};

int
main (int argc, char* argv[])
{
  int i;
#ifdef __SYMBIAN32__
 g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
 g_set_print_handler(mrtPrintHandler);
 #endif /*__SYMBIAN32__*/
  if (argc == 2)
    {
      for (i = 0; i < G_N_ELEMENTS (func_table); i++)
        {
          if (strcmp (func_table[i].name, argv[1]) == 0)
            {
              g_thread_init (NULL);
              func_table[i].func ();
              g_assert_not_reached ();
            }
        }
    }

  fprintf (stderr, "Usage: errorcheck-mutex-test [TEST]\n\n");
  fprintf (stderr, "   where TEST can be one of:\n\n");
  for (i = 0; i < G_N_ELEMENTS (func_table); i++)
    {
      fprintf (stderr, "      %s\n", func_table[i].name);
    }
#ifdef __SYMBIAN32__
  testResultXml("errorcheck-mutex-test");
  #endif /* EMULATOR */
  return 0;
}
