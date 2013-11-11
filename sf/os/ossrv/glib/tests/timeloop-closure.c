/* Portion Copyright © 2008-09 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.*/
#undef G_DISABLE_ASSERT
#undef G_LOG_DOMAIN

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <glib.h>
#include <glib-object.h>

#ifdef __SYMBIAN32__
#include "mrt2_glib2_test.h"
#endif /*__SYMBIAN32__*/


static int n_children = 3;
static int n_active_children;
static int n_iters = 10000;
static GMainLoop *loop;

static void
io_pipe (GIOChannel **channels)
{
  int fds[2];

  if (pipe(fds) < 0)
    {
      fprintf (stderr, "Cannot create pipe %s\n", g_strerror (errno));
      exit (1);
    }

  channels[0] = g_io_channel_unix_new (fds[0]);
  channels[1] = g_io_channel_unix_new (fds[1]);
}

static gboolean
read_all (GIOChannel *channel, char *buf, int len)
{
  gsize bytes_read = 0;
  gsize count;
  GIOError err;

  while (bytes_read < len)
    {
      err = g_io_channel_read (channel, buf + bytes_read, len - bytes_read, &count);
      if (err)
	{
	  if (err != G_IO_ERROR_AGAIN)
	    return FALSE;
	}
      else if (count == 0)
	return FALSE;

      bytes_read += count;
    }

  return TRUE;
}

static gboolean
write_all (GIOChannel *channel, char *buf, int len)
{
  gsize bytes_written = 0;
  gsize count;
  GIOError err;

  while (bytes_written < len)
    {
      err = g_io_channel_write (channel, buf + bytes_written, len - bytes_written, &count);
      if (err && err != G_IO_ERROR_AGAIN)
	return FALSE;

      bytes_written += count;
    }

  return TRUE;
}

#ifndef __SYMBIAN32__
static void
run_child (GIOChannel *in_channel, GIOChannel *out_channel)
#else
gpointer
run_child (gpointer data)
#endif//__SYMBIAN32__
{
  int i;
  int val = 1;
#ifdef  __SYMBIAN32__ 
  GIOChannel *in_channel,*out_channel;
#endif//__SYMBIAN32__  
  GTimer *timer = g_timer_new();
#ifdef __SYMBIAN32__  
  GIOChannel **channels = data;
  
  in_channel = channels[0];
  out_channel = channels[1];
#endif//__SYMBIAN32__
  for (i = 0; i < n_iters; i++)
    {
      write_all (out_channel, (char *)&val, sizeof (val));
      read_all (in_channel, (char *)&val, sizeof (val));
    }

  val = 0;
  write_all (out_channel, (char *)&val, sizeof (val));

  val = g_timer_elapsed (timer, NULL) * 1000;
  
  write_all (out_channel, (char *)&val, sizeof (val));
  g_timer_destroy (timer);
#ifndef  __SYMBIAN32__
  exit (0);
#else
  return NULL;
#endif//__SYMBIAN32__    
}

static gboolean
input_callback (GIOChannel   *source,
		GIOCondition  condition,
		gpointer      data)
{
  int val;
  GIOChannel *dest = (GIOChannel *)data;
  
  if (!read_all (source, (char *)&val, sizeof(val)))
    {
      fprintf (stderr, "Unexpected EOF\n");
      exit (1);
    }

  if (val)
    {
      write_all (dest, (char *)&val, sizeof(val));
      
      return TRUE;
    }
  else
    {
      g_io_channel_close (source);
      g_io_channel_close (dest);
      
      g_io_channel_unref (source);
      g_io_channel_unref (dest);

      n_active_children--;
      if (n_active_children == 0)
	g_main_loop_quit (loop);
      
      return FALSE;
    }
}

static void
create_child (void)
{
  GError *err;
  GIOChannel *in_channels[2];
  GIOChannel *out_channels[2];
  GIOChannel **sub_channels;
  GSource *source;
  
  sub_channels = g_new (GIOChannel *, 2);
  
  io_pipe (in_channels);
  io_pipe (out_channels);
  
  sub_channels[0] = in_channels[0];
  sub_channels[1] = out_channels[1];

  source = g_io_create_watch (out_channels[0], G_IO_IN | G_IO_HUP);
  g_assert(source != NULL);
  g_source_set_closure (source,
  g_cclosure_new (G_CALLBACK (input_callback), in_channels[1], NULL));
  g_source_attach (source, NULL);

  g_thread_create(run_child,sub_channels,FALSE,&err);

}

static double 
difftimeval (struct timeval *old, struct timeval *new)
{
  return
    (new->tv_sec - old->tv_sec) * 1000. + (new->tv_usec - old->tv_usec) / 1000;
}

int 
main (int argc, char **argv)
{
  int i;

  #ifdef __SYMBIAN32__
  g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
  g_set_print_handler(mrtPrintHandler);
  #endif /*__SYMBIAN32__*/
   
  g_thread_init(NULL);
	  
  g_type_init ();
  
  if (argc > 1)
    n_children = atoi(argv[1]);

  if (argc > 2)
    n_iters = atoi(argv[2]);

  printf ("Children: %d     Iters: %d\n", n_children, n_iters);

  n_active_children = n_children;
  for (i = 0; i < n_children; i++)
    create_child ();

  loop = g_main_loop_new (NULL, FALSE);
  
  g_assert(loop != NULL);
  
  g_main_loop_run (loop);
  
  g_assert(n_active_children == 0);
  
  
#ifdef __SYMBIAN32__
  testResultXml("timeloop-closure");
#endif /* EMULATOR */
  return 0;
}
