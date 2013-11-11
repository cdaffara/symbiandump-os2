#undef G_DISABLE_ASSERT
#undef G_LOG_DOMAIN

#include <glib.h>
#include <stdio.h>
#include <unistd.h>

#ifdef SYMBIAN
#include <glib_global.h>
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/


static GAsyncQueue *queue;
G_LOCK_DEFINE_STATIC (queue_lock);
static guint queue_ready;
#define THREADS 10

static gpointer thread_push (gpointer data)
{
	g_async_queue_push (queue, data);
	return NULL;
}

static gpointer thread_pop (gpointer data)
{
	gint flag = *(gint*)&data;
	gpointer res = g_async_queue_pop (queue);
	gint g_async_queue_pop_op = *(gint*)&res;
	
	G_LOCK (queue_lock);
	queue_ready++;
	G_UNLOCK (queue_lock);
	g_assert (g_async_queue_pop_op == (34 + flag));
	return NULL;
}

void basic_test()
{
	GThread *thread1[THREADS], *thread2[THREADS];
	int i;
	
	queue = g_async_queue_new ();
	
	for(i = 0; i < THREADS; i++)
	{
		thread1[i] = g_thread_create (thread_push, GINT_TO_POINTER(34+i), TRUE, NULL);
		thread2[i] = g_thread_create (thread_pop, GINT_TO_POINTER(i), TRUE, NULL);
	}
	
	/*while(queue_ready != 9)
		g_usleep (G_USEC_PER_SEC / 5);*/
	
	for(i = 0; i < THREADS; i++)
	{
		g_thread_join (thread1[i]);
		g_thread_join (thread2[i]);
	}
	
}


static gpointer thread_ref_push (gpointer data)
{
	g_async_queue_lock (queue);					//Lock the queue
	g_async_queue_ref (queue);					//Increase ref by 1
	g_async_queue_unref_and_unlock (queue);		//Decrease ref by 1 and unlock
	g_async_queue_push (queue, data);
	sleep(2);
	g_async_queue_push (queue, data);
	return NULL;
}

static gpointer thread_ref_pop (gpointer data)
{
	gpointer res;
	gint g_async_queue_try_pop_unlocked_op;
	GTimeVal time_val = 
	{
		0,100
	};
	
	g_async_queue_ref_unlocked (queue);			//Increase ref by 1 and unlock
	
	g_async_queue_lock (queue);					//Lock the queue
	res = g_async_queue_try_pop_unlocked (queue);
	g_async_queue_try_pop_unlocked_op = *(gint*)&res;
	g_async_queue_unlock (queue);					//unlock the queue while the other thread sleeps
	g_assert(g_async_queue_timed_pop (queue, &time_val) == NULL);
	
	G_LOCK (queue_lock);
	queue_ready++;
	G_UNLOCK (queue_lock);
	g_assert (g_async_queue_try_pop_unlocked_op == 55);
	return NULL;
}

void ref_test()
{
	GThread *thread1, *thread2;
	
	g_assert (g_async_queue_try_pop (queue) == NULL);
	
	thread1 = g_thread_create (thread_ref_push, GINT_TO_POINTER(55), TRUE, NULL);
	thread2 = g_thread_create (thread_ref_pop, NULL, TRUE, NULL);
		
/*	while(!queue_ready)
		g_usleep (G_USEC_PER_SEC / 5);*/
	
	g_thread_join (thread1);
	g_thread_join (thread2);
		
}


/* run all the tests */
void
run_all_tests()
{
	queue_ready = 0;
	basic_test();
	queue_ready = 0;
	ref_test()	;
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
  run_all_tests ();
#endif
#ifdef SYMBIAN
  testResultXml("tasyncqueue");
#endif /* EMULATOR */
  return 0;
}
