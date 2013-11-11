/* Portion Copyright © 2008-09 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.*/




#include <unistd.h>
#include <glib.h>
#include <glib-object.h>

#ifdef SYMBIAN
#include "glib_global.h"
#include "mrt2_glib2_test.h"
#define MULTITHREAD
//#define TEST_STOP_EMISSION
//#define VERBOSE
#define LOOP 3 
int TESTNUM;
int notifynum;
int handlernum;
gulong  hookid;
GValue gv ;
GSignalQuery gq;
guint gi;
#endif /*SYMBIAN*/


#define G_TYPE_TEST               (g_test_get_type ())
#define G_TEST(test)              (G_TYPE_CHECK_INSTANCE_CAST ((test), G_TYPE_TEST, GTest))
#define G_IS_TEST(test)           (G_TYPE_CHECK_INSTANCE_TYPE ((test), G_TYPE_TEST))
#define G_TEST_CLASS(tclass)      (G_TYPE_CHECK_CLASS_CAST ((tclass), G_TYPE_TEST, GTestClass))
#define G_IS_TEST_CLASS(tclass)   (G_TYPE_CHECK_CLASS_TYPE ((tclass), G_TYPE_TEST))
#define G_TEST_GET_CLASS(test)    (G_TYPE_INSTANCE_GET_CLASS ((test), G_TYPE_TEST, GTestClass))

static GRand *grand;

typedef struct _GTest GTest;
typedef struct _GTestClass GTestClass;

struct _GTest
{
  GObject object;

  gint value;
};

struct _GTestClass
{
  GObjectClass parent_class;

  void (*test_signal1) (GTest * test, gint an_int);
  void (*test_signal2) (GTest * test, gint an_int);
};

static GType g_test_get_type (void);
static volatile gboolean stopping;

/* Element signals and args */
enum
{
  TEST_SIGNAL1,
  TEST_SIGNAL2,
  /* add more above */
  LAST_SIGNAL
};

enum
{
  ARG_0,
  ARG_TEST_PROP
};

static void g_test_class_init (GTestClass * klass);
static void g_test_init (GTest * test);
static void g_test_dispose (GObject * object);

static void signal2_handler (GTest * test, gint anint);

static void g_test_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void g_test_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

static GObjectClass *parent_class = NULL;

static guint g_test_signals[LAST_SIGNAL] = { 0 };

static GType
g_test_get_type (void)
{
  static GType test_type = 0;

  if (!test_type) {
    static const GTypeInfo test_info = {
      sizeof (GTestClass),
      NULL,
      NULL,
      (GClassInitFunc) g_test_class_init,
      NULL,
      NULL,
      sizeof (GTest),
      0,
      (GInstanceInitFunc) g_test_init,
      NULL
    };

    grand = g_rand_new();

    test_type = g_type_register_static (G_TYPE_OBJECT, "GTest",
        &test_info, 0);
  }
  return test_type;
}

static void
g_test_class_init (GTestClass * klass)
{
  GObjectClass *gobject_class;

  gobject_class = (GObjectClass *) klass;

  parent_class = g_type_class_ref (G_TYPE_OBJECT);
#ifndef SYMBIAN
  if (!g_thread_supported ())
    g_thread_init (NULL);
#else

#ifdef MULTITHREAD  
  if (!g_thread_supported ())
    g_thread_init (NULL);
#endif /*MULTITHREAD*/

#endif /*SYMBAIN*/

  gobject_class->dispose = g_test_dispose;
  gobject_class->set_property = g_test_set_property;
  gobject_class->get_property = g_test_get_property;

  g_test_signals[TEST_SIGNAL1] =
      g_signal_new ("test-signal1", G_TYPE_FROM_CLASS (klass),
      G_SIGNAL_RUN_LAST, G_STRUCT_OFFSET (GTestClass, test_signal1), NULL,
      NULL, g_cclosure_marshal_VOID__INT, G_TYPE_NONE, 1, G_TYPE_INT);
  g_test_signals[TEST_SIGNAL2] =
      g_signal_new ("test-signal2", G_TYPE_FROM_CLASS (klass),
      G_SIGNAL_RUN_LAST, G_STRUCT_OFFSET (GTestClass, test_signal2), NULL,
      NULL, g_cclosure_marshal_VOID__INT, G_TYPE_NONE, 1, G_TYPE_INT);

  g_object_class_install_property (G_OBJECT_CLASS (klass), ARG_TEST_PROP,
      g_param_spec_int ("test-prop", "Test Prop", "Test property",
          0, 1, 0, G_PARAM_READWRITE));

  klass->test_signal2 = signal2_handler;
}

static void
g_test_init (GTest * test)
{
  #ifndef SYMBIAN
  g_print ("init %p\n", test);
  #else
  
  #ifdef VERBOSE
  g_print ("init %p\n", test);
  #endif
  
  #endif /*SYMBIAN*/  

  test->value = 0;
}

static void
g_test_dispose (GObject * object)
{
  GTest *test;

  test = G_TEST (object);

  g_print ("dispose %p!\n", object);

  G_OBJECT_CLASS (parent_class)->dispose (object);
}

static void
g_test_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GTest *test;

  test = G_TEST (object);

  switch (prop_id) {
    case ARG_TEST_PROP:
      test->value = g_value_get_int (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
g_test_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GTest *test;

  test = G_TEST (object);

  switch (prop_id) {
    case ARG_TEST_PROP:
      g_value_set_int (value, test->value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
g_test_do_signal1 (GTest * test)
{
  g_signal_emit (G_OBJECT (test), g_test_signals[TEST_SIGNAL1], 0, 0);
}

static void
signal2_handler (GTest * test, gint anint)
{
	#ifdef VERBOSE
	g_print ("Function: signal2_handler called\n");
	#endif
	
	handlernum++;
}

static void
g_test_do_signal2 (GTest * test)
{
  g_signal_emit (G_OBJECT (test), g_test_signals[TEST_SIGNAL2], 0, 0);
}

static void
g_test_do_prop (GTest * test)
{
  test->value = g_rand_int (grand);
  g_object_notify (G_OBJECT (test), "test-prop");
}

static gpointer
run_thread (GTest * test)
{
  gint i = 1;

#ifndef SYMBIAN
  while (!stopping)
   {
    if (TESTNUM == 1)
      g_test_do_signal1 (test);    
    if (TESTNUM == 2)
      g_test_do_signal2 (test);
    if (TESTNUM == 3)
      g_test_do_prop (test);
    if ((i++ % 10000) == 0) {
      g_print (".");
     g_thread_yield();  /*force context switch */
    }
   }
#else

#ifdef MULTITHREAD
  while (!stopping)
   {
    if (TESTNUM == 1)
      g_test_do_signal1 (test);
    if (TESTNUM == 2)
      g_test_do_signal2 (test);
    if (TESTNUM == 3)
      g_test_do_prop (test);
    if ((i++ % 10/*000*/) == 0) 
    {
    #ifdef VERBOSE
      g_print (".");
    #endif  /*VERBOSE*/
     g_thread_yield(); /* force context switch */
    }
   }
#else
  for(i=0;i <= LOOP;i++)    
  {
    if (TESTNUM == 1)
      g_test_do_signal1 (test);
    if (TESTNUM == 2)
      g_test_do_signal2 (test);
    if (TESTNUM == 3)
      g_test_do_prop (test);  
    
    #ifdef 	VERBOSE
    g_print(".");
    #endif
  }
#endif /*MULTITHREAD*/

#endif /*SYMBIAN*/ 

  return NULL;
}




static void
notify (GObject *object, GParamSpec *spec, gpointer user_data)
{
  gint value;
  
  notifynum++;
	#ifdef VERBOSE
	g_print ("Function: notify called\n");
	#endif

  g_object_get (object, "test-prop", &value, NULL);
  //g_print ("+ %d", value);
}

#ifdef SYMBIAN
void hook_function()
{
	#ifdef VERBOSE
	g_print ("Function: hook_function called\n");
	#endif
	handlernum++;
}
#endif

int
main (int argc, char **argv)
{
  gint i;
  GTest *test1, *test2;
  GArray *test_threads;
  const gint n_threads = 1;

  #ifdef SYMBIAN
  g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
  g_set_print_handler(mrtPrintHandler);
  #endif /*SYMBIAN*/

  g_thread_init (NULL);
  
  #ifndef SYMBIAN
  g_print ("START: %s\n", argv[0]);
  #else
  
  #ifdef VERBOSE
  g_print ("START: %s\n", argv[0]);
  #endif
  
  #endif /*SYMBIAN*/  
  g_log_set_always_fatal (G_LOG_LEVEL_WARNING | G_LOG_LEVEL_CRITICAL | g_log_set_always_fatal (G_LOG_FATAL_MASK));
  g_type_init ();

  test1 = g_object_new (G_TYPE_TEST, NULL);
  test2 = g_object_new (G_TYPE_TEST, NULL);

  g_signal_connect (test1, "notify::test-prop", G_CALLBACK (notify), NULL);
  g_signal_connect (test1, "test-signal1", G_CALLBACK (notify), NULL);
  g_signal_connect (test1, "test-signal2", G_CALLBACK (notify), NULL);




#ifndef SYMBIAN
  test_threads = g_array_new (FALSE, FALSE, sizeof (GThread *));

  stopping = FALSE;

  for (i = 0; i < n_threads; i++) {
    GThread *thread;

    thread = g_thread_create ((GThreadFunc) run_thread, test1, TRUE, NULL);
    g_array_append_val (test_threads, thread);

    thread = g_thread_create ((GThreadFunc) run_thread, test2, TRUE, NULL);
    g_array_append_val (test_threads, thread);
  }
  g_usleep (500000);

  stopping = TRUE;

  g_print ("\nstopping\n");

  /* join all threads */
  for (i = 0; i < 2 * n_threads; i++) {
    GThread *thread;

    thread = g_array_index (test_threads, GThread *, i);
    g_thread_join (thread);
  }

  g_print ("stopped:%d\n",TESTNUM);
#else

#ifdef MULTITHREAD

  test_threads = g_array_new (FALSE, FALSE, sizeof (GThread *));
  stopping = FALSE;
  TESTNUM=1;
  notifynum=0;
  handlernum=0;
  for (i = 0; i < n_threads; i++) 
  {
    GThread *thread;

    thread = g_thread_create ((GThreadFunc) run_thread, test1, TRUE, NULL);
    g_array_append_val (test_threads, thread);

    thread = g_thread_create ((GThreadFunc) run_thread, test2, TRUE, NULL);
    g_array_append_val (test_threads, thread);
  }
  g_usleep (500000);

  stopping = TRUE;
  
  #ifdef VERBOSE	
  g_print ("\nstopping\n");
  #endif
  /* join all threads */
  for (i = 0; i < 2 * n_threads; i++) 
  {
    GThread *thread;

    thread = g_array_index (test_threads, GThread *, i);
    g_thread_join (thread);
  }    
  
  g_assert(notifynum != 0);
  g_assert(handlernum == 0);
  
  g_array_free (test_threads, TRUE);
  
  #ifdef VERBOSE
  g_print ("Signals.c: completed for TESTNUM = %d\n",TESTNUM);
  #endif


  
  test_threads = g_array_new (FALSE, FALSE, sizeof (GThread *));
  stopping = FALSE;
  TESTNUM=2;
  notifynum=0;
  handlernum=0;
  
  for (i = 0; i < n_threads; i++) 
  {
    GThread *thread;

    thread = g_thread_create ((GThreadFunc) run_thread, test1, TRUE, NULL);
    g_array_append_val (test_threads, thread);

    thread = g_thread_create ((GThreadFunc) run_thread, test2, TRUE, NULL);
    g_array_append_val (test_threads, thread);
  }
  g_usleep (500000);

  stopping = TRUE;
  
  #ifdef VERBOSE	
  g_print ("\nstopping\n");
  #endif
  /* join all threads */
  for (i = 0; i < 2 * n_threads; i++) 
  {
    GThread *thread;

    thread = g_array_index (test_threads, GThread *, i);
    g_thread_join (thread);
  }
  
  g_assert(notifynum != 0);
  g_assert(handlernum != 0);

  g_array_free (test_threads, TRUE);

  #ifdef VERBOSE
  g_print ("Signals.c: completed for TESTNUM = %d\n",TESTNUM);
  #endif  


  test_threads = g_array_new (FALSE, FALSE, sizeof (GThread *));
  stopping = FALSE;
  TESTNUM=3;
  notifynum=0;
  handlernum=0;
  
  for (i = 0; i < n_threads; i++) 
  {
    GThread *thread;

    thread = g_thread_create ((GThreadFunc) run_thread, test1, TRUE, NULL);
    g_array_append_val (test_threads, thread);

    thread = g_thread_create ((GThreadFunc) run_thread, test2, TRUE, NULL);
    g_array_append_val (test_threads, thread);
  }
  g_usleep (5000000);

  stopping = TRUE;
  
  #ifdef VERBOSE	
  g_print ("\nstopping\n");
  #endif
  /* join all threads */
  for (i = 0; i < 2 * n_threads; i++) 
  {
    GThread *thread;

    thread = g_array_index (test_threads, GThread *, i);
    g_thread_join (thread);
  }
  
  g_assert(notifynum != 0);
  g_assert(handlernum == 0);

  g_array_free (test_threads, TRUE);

  #ifdef VERBOSE
  g_print ("Signals.c: completed for TESTNUM = %d\n",TESTNUM);
  #endif  

#else	/* ! MULTITHREAD*/


TESTNUM=1;
#ifdef VERBOSE	
g_print ("\nStarting with TESTNUM=%d\n",TESTNUM);
#endif

notifynum=0;
handlernum=0;
run_thread(test1);
g_assert(notifynum == LOOP +1);
g_assert(handlernum ==0);

notifynum=0;
handlernum=0;
run_thread(test2);
g_assert(notifynum == 0);
g_assert(handlernum == 0);

TESTNUM=2;
#ifdef VERBOSE	
g_print ("\nStarting with TESTNUM=%d\n",TESTNUM);
#endif

notifynum=0;
handlernum=0;
run_thread(test1);
g_assert(notifynum == LOOP+1);
g_assert(handlernum == LOOP+1);

notifynum=0;
handlernum=0;
run_thread(test2);
g_assert(notifynum == 0);
g_assert(handlernum == LOOP+1);

TESTNUM=3;
#ifdef VERBOSE	
g_print ("\nStarting with TESTNUM=%d\n",TESTNUM);
#endif

notifynum=0;
handlernum=0;
run_thread(test1);
g_assert(notifynum == LOOP +1);
g_assert(handlernum ==0);

notifynum=0;
handlernum=0;
run_thread(test2);
g_assert(notifynum == 0);
g_assert(handlernum == 0);

g_assert(g_signal_has_handler_pending(G_OBJECT(test1),g_signal_lookup("test-signal2",G_TYPE_TEST),NULL,TRUE)==TRUE);
g_assert(g_signal_has_handler_pending(G_OBJECT(test2),g_signal_lookup("test-signal2",G_TYPE_TEST),NULL,TRUE)==FALSE);

g_assert(g_signal_handler_is_connected(G_OBJECT(test1),g_signal_lookup("test-signal1",G_TYPE_TEST))==TRUE);
g_assert(g_signal_handler_is_connected(G_OBJECT(test2),g_signal_lookup("test-signal2",G_TYPE_TEST))==FALSE);

handlernum=g_signal_lookup("test-signal1",G_TYPE_TEST);
#ifdef VERBOSE
g_print("Signal id: %d\n",handlernum);
#endif

g_signal_connect (test2, "test-signal1", G_CALLBACK (notify), NULL);

hookid=g_signal_add_emission_hook(handlernum,NULL,(GSignalEmissionHook) hook_function,NULL,NULL);

#ifdef VERBOSE
g_print("Hookid: %d\n",hookid);
#endif
/********************/

#ifdef TEST_STOP_EMISSION
/*
notifynum=0;
handlernum=0;
g_print("The following call stops signal emission\n");
g_signal_stop_emission(G_OBJECT(test1),g_signal_lookup("test-signal1",G_TYPE_TEST),0);

g_print("The following call should abort and it is normal\n");
run_thread(test1);
printf("Notifynum: %d and Handlernum: %d\n",notifynum,handlernum);

*/
notifynum=0;
handlernum=0;
g_signal_stop_emission_by_name(G_OBJECT(test1),"test-signal1");
//run_thread(test1);
g_print("Notifynum: %d and Handlernum: %d\n",notifynum,handlernum);


#endif /*TEST_STOP_EMISSION*/

/*******************/


handlernum=0;
g_signal_emit(G_OBJECT (test1), g_test_signals[TEST_SIGNAL1], 0, 0);
g_signal_emit(G_OBJECT (test2), g_test_signals[TEST_SIGNAL1], 0, 0);
g_assert(handlernum==2);


g_signal_remove_emission_hook(g_signal_lookup("test-signal1",G_TYPE_TEST),hookid);

#ifdef VERBOSE
g_print("Emitting signal again after removing emission hook\n");
#endif

handlernum=0;
g_signal_emit (G_OBJECT (test1), g_test_signals[TEST_SIGNAL1], 0, 0);
g_signal_emit (G_OBJECT (test2), g_test_signals[TEST_SIGNAL1], 0, 0);
g_assert(handlernum==0);

g_assert (strcmp ("test-signal1", g_signal_name (g_signal_lookup("test-signal1",G_TYPE_TEST))) == 0);
g_assert (strcmp ("test-signal2", g_signal_name (g_signal_lookup("test-signal2",G_TYPE_TEST))) == 0);


memset(&gv,0,sizeof(gv));
g_value_init(&gv,G_TYPE_OBJECT);
g_value_set_object(&gv,test1);

gi=0;
g_signal_list_ids(G_OBJECT_TYPE(test1),&gi);
g_assert(g_signal_lookup("test-signal1",G_TYPE_TEST)==gi);

notifynum=0;
g_signal_emitv (&gv, g_test_signals[TEST_SIGNAL1], 0, &gv);
g_assert(notifynum==1);

g_signal_query(g_signal_lookup("test-signal1",G_TYPE_TEST),&gq);
g_assert(strcmp("test-signal1",gq.signal_name)==0);
g_assert(g_signal_lookup("test-signal1",G_TYPE_TEST)==gq.signal_id);


g_assert(g_signal_handler_find(G_OBJECT(test1), G_SIGNAL_RUN_LAST,g_signal_lookup("test-signal2",G_TYPE_TEST),NULL,NULL,NULL,NULL)==2);


notifynum=g_signal_handlers_block_matched(G_OBJECT(test1),G_SIGNAL_MATCH_FUNC,g_signal_lookup("test-signal2",G_TYPE_TEST),NULL,NULL,(gpointer)G_CALLBACK(notify),NULL);
handlernum=g_signal_handlers_unblock_matched(G_OBJECT(test1),G_SIGNAL_MATCH_FUNC,g_signal_lookup("test-signal2",G_TYPE_TEST),NULL,NULL,(gpointer)G_CALLBACK(notify),NULL);
g_assert(notifynum==handlernum);

#endif /*MULTITHREAD*/

#ifdef VERBOSE	
g_printf ("\nsignals.c: Completed all tests\n");
#endif

#endif /*SYMBIAN*/ 



  #if SYMBIAN
  testResultXml("signals-multithread");
  #endif /* EMULATOR */

  return 0;
}
