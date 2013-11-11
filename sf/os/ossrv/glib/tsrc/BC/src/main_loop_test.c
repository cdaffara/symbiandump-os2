/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
*
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
*
* Description:
*
*/



#undef G_DISABLE_ASSERT
#undef G_LOG_DOMAIN

#include <stdio.h>
#include <string.h>
#include "glib.h"
#include <pthread.h>
#include <unistd.h>

#ifdef SYMBIAN
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/

GMainLoop *loop,*idle_loop;
GMainContext *context;
int fd[2];
char *buf[100];
GSource *source1,*source2;
pthread_t thread1;
gboolean e1_complete,e2_complete;

void quit()
{
	if(e1_complete && e2_complete)
		g_main_loop_quit(loop);
}


gboolean prepare(GSource *source,gint *timeout)
{
	*timeout = 0;
	return TRUE;
}

gboolean check(GSource *source)
{
	return TRUE;
}

gboolean dispatch(GSource *source,GSourceFunc callback,gpointer user_data)
{
	if(callback(user_data))
		return TRUE;
	else
		return FALSE;
}

gboolean my_callback(int* data)
{
	static int i = 100;
	int depth;

	//checks g_main_loop_is_running
	g_assert(g_main_loop_is_running(loop));

	depth = g_main_depth();

	//checks g_main_depth
	g_assert(depth == 1);

	i--;

	//printf("%d\n",i);

	if(i<0)
	{
		e1_complete = TRUE;
		quit();
		return FALSE;
	}
	else
	{
		++(*data);
		return TRUE;
	}
}


gboolean fd_prepare(GSource *source,gint *timeout)
{
	*timeout = -1;
	return FALSE;
}

gboolean fd_check(GSource *source)
{
	GPollFD *pollfd = source->poll_fds->data;
	if(pollfd->revents && (G_IO_IN | G_IO_HUP | G_IO_ERR))
		return TRUE;
	else
		return FALSE;
}

gboolean fd_dispatch(GSource *source,GSourceFunc callback,gpointer user_data)
{
	if(callback(user_data))
		return TRUE;
	else
		return FALSE;
}

gboolean fd_callback(int* data)
{
	pthread_join(thread1, NULL);
	read(fd[0],buf,sizeof(buf));

	// This asserts actually checks whether the callback for fd is successful or not
	// In other words it checks if g_source_add_poll was successful or not.
	g_assert(!strcmp((const char *)buf,"fd poll example"));

	*data = 1;
	//printf("%s",buf);
	//getchar();

	g_source_unref(source2);

	e2_complete = TRUE;
	quit();
	return FALSE;
}


void* thread_function(void *data)
{
	sleep(1);
	write(fd[1],"fd poll example",15);
	return NULL;
}

void main_loop_test()
{
	GMainContext *default_context;
	int depth;
	int id;
	GTimeVal time ={0,};
	int user_data = 0,fd_data = 0;
	GPollFD pollfd;
	GSource *source3;

	GSourceFuncs SourceFuncs =
	{
		prepare,
		check,
		dispatch,
		NULL
	};

	GSourceFuncs fd_SourceFuncs =
	{
		fd_prepare,
		fd_check,
		fd_dispatch,
		NULL
	};

	e1_complete = FALSE;
	e2_complete = FALSE;

	pipe(fd);

	pollfd.fd = fd[0];
	pollfd.events = G_IO_IN | G_IO_HUP | G_IO_ERR;
	pollfd.revents = 0;

	pthread_create(&thread1, NULL, thread_function, NULL);

	context = g_main_context_new();

	//g_main_context_add_poll(context,&pollfd,0);

	source1 = g_source_new(&SourceFuncs,sizeof(GSource));
	g_source_set_callback(source1,(GSourceFunc)my_callback,&user_data,NULL);

	id = g_source_attach(source1,context);

	g_assert(g_source_get_id(source1) == id);

	g_source_set_priority(source1,0);

	g_assert(g_source_get_priority(source1) == 0);

	loop = g_main_loop_new(context, FALSE);

	default_context = g_main_loop_get_context(loop);

	//checks g_main_loop_get_context
	g_assert(default_context == context);

	//checks g_main_loop_is_running
	g_assert(g_main_loop_is_running(loop) == FALSE);

	depth = g_main_depth();

	//checks g_main_depth
	g_assert(depth == 0);

	g_source_get_current_time(source1,&time);

	g_assert(time.tv_usec > 0);

	g_source_set_can_recurse(source1,TRUE);

	g_assert(g_source_get_can_recurse(source1) == TRUE);

	source2 = g_source_new(&fd_SourceFuncs,sizeof(GSource));
	g_source_set_callback(source2,(GSourceFunc)fd_callback,&fd_data,NULL);
	g_source_add_poll(source2,&pollfd);

	g_source_remove_poll(source2,&pollfd);

	// checks g_source_remove_poll
	g_assert(source2->poll_fds == NULL);

	g_source_add_poll(source2,&pollfd);

	// checks whether g_source_add_poll is successful.
	// one more check is done in fd_callback.
	// If that function is callled we are sure that add poll was successful
	g_assert(source2->poll_fds->data == &pollfd);

	source3 = g_source_ref(source2);

	g_assert(source3 == source2 && source2->ref_count == 2);

	g_source_unref(source3);

	id = g_source_attach(source2,context);

	//checks g_main_context_pending
	g_assert(g_main_context_pending(context));

	g_main_loop_run(loop);

	// ref is called here. Thats why two unrefs are called. If the 2nd unref is
	// callled with the unref, code should crash
	g_main_loop_ref(loop);

	g_main_loop_unref(loop);

	g_main_loop_unref(loop);

	//checks the number of times the call back function is called
	g_assert(user_data == 100);

	// checks whether set poll was successful and call back for the same
	// was called
	g_assert(fd_data == 1);
}

gint dummy_poll_func(GPollFD *ufds,guint nfsd,gint timeout_)
{
	return 0;
}


void misc_test()
{
	GPollFunc func = g_main_context_get_poll_func(context);

	g_assert(func != NULL);

	g_main_context_set_poll_func(context,dummy_poll_func);
	func = g_main_context_get_poll_func(context);

	g_assert(func == dummy_poll_func);
}

gboolean function(int *data)
{
	*data = 1;
	g_main_loop_quit(idle_loop);
	return FALSE;
}

void idle_test()
{
	int user_data = 0;
	gboolean retVal;
	GSource *s = g_idle_source_new();

	idle_loop = g_main_loop_new(NULL, FALSE);
	g_source_attach(s,context);

	g_idle_add((GSourceFunc)function,&user_data);

	retVal = g_idle_remove_by_data(&user_data);

	//checks g_idle_remove_by_data
	g_assert(retVal == TRUE);
	retVal = FALSE;

	g_idle_add((GSourceFunc)function,&user_data);

	retVal = g_source_remove_by_user_data(&user_data);

	//checks g_source_remove_by_user_data
	g_assert(retVal == TRUE);

	g_idle_add((GSourceFunc)function,&user_data);

	g_main_loop_run(idle_loop);

	g_main_loop_unref(idle_loop);

	//checks whether the function was run or not
	g_assert(user_data == 1);
}

int main()
{

	#ifdef SYMBIAN

	g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
	#endif

	//main_loop_test();
	//misc_test();
	//idle_test();


	#ifdef SYMBIAN
  	testResultXml("main_loop_test");
  	#endif /* EMULATOR */

	return 0;
}