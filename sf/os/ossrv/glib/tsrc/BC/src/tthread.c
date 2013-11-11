#undef G_DISABLE_ASSERT
#undef G_LOG_DOMAIN

#include <glib.h>
#include <stdio.h>

#ifdef SYMBIAN
#include <glib_global.h>
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/

#define THREADS 10

/* GStaticRecMutex */

GStaticRecMutex test_g_static_rec_mutex_mutex;
static guint test_g_static_rec_mutex_int = 0;
static gboolean test_g_static_rec_mutex_thread_ready;

static gpointer
test_g_static_rec_mutex_thread (gpointer data)
{
  g_assert (GPOINTER_TO_INT (data) == 42);
  g_assert (g_static_rec_mutex_trylock (&test_g_static_rec_mutex_mutex) 
	    == FALSE);
  test_g_static_rec_mutex_thread_ready = TRUE;
  
  //Testing g_static_rec_mutex_lock_full
  g_static_rec_mutex_lock_full (&test_g_static_rec_mutex_mutex, 10);
  
  g_assert (test_g_static_rec_mutex_int == 42);
  test_g_static_rec_mutex_thread_ready = FALSE;
  
  //Testing g_static_rec_mutex_unlock_full for lock_full
  g_static_rec_mutex_unlock_full (&test_g_static_rec_mutex_mutex);
  
  g_thread_exit (GINT_TO_POINTER (43));
  
  g_assert_not_reached ();
  return NULL;
}

static void
test_g_static_rec_mutex (void)
{
  GThread *thread;

  //Test for g_static_rec_mutex_init	
  g_static_rec_mutex_init(&test_g_static_rec_mutex_mutex);

  g_assert (g_static_rec_mutex_trylock (&test_g_static_rec_mutex_mutex));
  test_g_static_rec_mutex_thread_ready = FALSE;
  thread = g_thread_create (test_g_static_rec_mutex_thread, 
			    GINT_TO_POINTER (42), TRUE, NULL);
  /* This busy wait is only for testing purposes and not an example of
   * good code!*/
  while (!test_g_static_rec_mutex_thread_ready)
    g_usleep (G_USEC_PER_SEC / 5);

  g_assert (g_static_rec_mutex_trylock (&test_g_static_rec_mutex_mutex));
  test_g_static_rec_mutex_int = 41;
  g_static_rec_mutex_unlock (&test_g_static_rec_mutex_mutex);
  test_g_static_rec_mutex_int = 42;  
  g_static_rec_mutex_unlock (&test_g_static_rec_mutex_mutex);

  /* This busy wait is only for testing purposes and not an example of
   * good code!*/
  while (test_g_static_rec_mutex_thread_ready)
    g_usleep (G_USEC_PER_SEC / 5);

  g_static_rec_mutex_lock (&test_g_static_rec_mutex_mutex);
  test_g_static_rec_mutex_int = 0;  
  
  //Testing g_static_rec_mutex_unlock_full with normal single lock
  g_static_rec_mutex_unlock_full (&test_g_static_rec_mutex_mutex);

  g_assert (GPOINTER_TO_INT (g_thread_join (thread)) == 43);
  g_static_rec_mutex_free (&test_g_static_rec_mutex_mutex);
}


/* GStaticRWLock */

/* -1 = writing; >0 = # of readers */
static gint test_g_static_rw_lock_state = 0; 
G_LOCK_DEFINE (test_g_static_rw_lock_state);

static gboolean test_g_static_rw_lock_run = TRUE; 
GStaticRWLock test_g_static_rw_lock_lock;// = G_STATIC_RW_LOCK_INIT;

static gpointer
test_g_static_rw_lock_thread (gpointer data)
{
  while (test_g_static_rw_lock_run)
    {
      if (g_random_double() > .2) /* I'm a reader */
	{
	  
	  if (g_random_double() > .2) /* I'll block */
	    g_static_rw_lock_reader_lock (&test_g_static_rw_lock_lock);
	  else /* I'll only try */
	    if (!g_static_rw_lock_reader_trylock (&test_g_static_rw_lock_lock))
	      continue;
	  G_LOCK (test_g_static_rw_lock_state);
	  g_assert (test_g_static_rw_lock_state >= 0);
	  test_g_static_rw_lock_state++;
	  G_UNLOCK (test_g_static_rw_lock_state);

	  g_usleep (g_random_int_range (20,1000));

	  G_LOCK (test_g_static_rw_lock_state);
	  test_g_static_rw_lock_state--;
	  G_UNLOCK (test_g_static_rw_lock_state);

	  g_static_rw_lock_reader_unlock (&test_g_static_rw_lock_lock);
	}
      else /* I'm a writer */
	{
	  
	  if (g_random_double() > .2) /* I'll block */ 
	    g_static_rw_lock_writer_lock (&test_g_static_rw_lock_lock);
	  else /* I'll only try */
	    if (!g_static_rw_lock_writer_trylock (&test_g_static_rw_lock_lock))
	      continue;
	  G_LOCK (test_g_static_rw_lock_state);
	  g_assert (test_g_static_rw_lock_state == 0);
	  test_g_static_rw_lock_state = -1;
	  G_UNLOCK (test_g_static_rw_lock_state);

	  g_usleep (g_random_int_range (20,1000));

	  G_LOCK (test_g_static_rw_lock_state);
	  test_g_static_rw_lock_state = 0;
	  G_UNLOCK (test_g_static_rw_lock_state);

	  g_static_rw_lock_writer_unlock (&test_g_static_rw_lock_lock);
	}
    }
  return NULL;
}

static void
test_g_static_rw_lock ()
{
  GThread *threads[THREADS];
  guint i;
  
  g_static_rw_lock_init(&test_g_static_rw_lock_lock);
  
  for (i = 0; i < THREADS; i++)
    {
      threads[i] = g_thread_create (test_g_static_rw_lock_thread, 
				    NULL, TRUE, NULL);      
    }
  g_usleep (G_USEC_PER_SEC * 5);
  test_g_static_rw_lock_run = FALSE;
  for (i = 0; i < THREADS; i++)
    {
      g_thread_join (threads[i]);
    }
  g_assert (test_g_static_rw_lock_state == 0);
  g_static_rw_lock_free(&test_g_static_rw_lock_lock);
}

void test_g_thread_error_quark()
{
	GQuark c = g_thread_error_quark();
	g_assert(c == 1);
}


static void test_g_thread_set_priority (void)
{
  GThread *thread;
  thread = g_thread_self();
  g_thread_set_priority (thread, G_THREAD_PRIORITY_HIGH);
  g_assert((thread->priority) == G_THREAD_PRIORITY_HIGH);
}


/* run all the tests */
void
run_all_thread_tests()
{
  test_g_static_rec_mutex ();
  test_g_static_rw_lock ();
  test_g_thread_error_quark();
  test_g_thread_set_priority();
}

int 
main (int   argc,
      char *argv[])
{
  #ifdef SYMBIAN
  
  g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
  g_set_print_handler(mrtPrintHandler);
  #endif /*SYMBIAN*/
	  

  /* Only run the test, if threads are enabled and a default thread
     implementation is available */
#if defined(G_THREADS_ENABLED) && ! defined(G_THREADS_IMPL_NONE)
  g_thread_init (NULL);
  run_all_thread_tests ();

  /* Now we rerun all tests, but this time we fool the system into
   * thinking, that the available thread system is not native, but
   * userprovided. */

  g_thread_use_default_impl = FALSE;
  run_all_thread_tests ();
  
#endif

#ifdef SYMBIAN
  testResultXml("tthread");
#endif /* EMULATOR */
  return 0;
}
