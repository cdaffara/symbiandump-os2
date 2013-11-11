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
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "glib.h"
#include <errno.h>
#ifdef SYMBIAN
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/


int main (int   argc,char *argv[])
{
	GDir *dir;
	GError *error;
	int ret;
	ret = mkdir("c:\\temp", 0666);
	g_print("mkdir for temp returns %d and error is %d", ret, errno);
	ret = mkdir("c:\\temp\\tmp", 0666);
	g_print("mkdir for temp returns %d and error is %d", ret, errno);
	#ifdef SYMBIAN
	g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
	g_set_print_handler(mrtPrintHandler);	
	#endif /*SYMBIAN*/
	
	dir = g_dir_open("c:\\temp",0,&error);
	
	g_assert(dir != NULL);
	
	g_assert(!strcmp(g_dir_read_name(dir),"tmp"));
	
	g_dir_close(dir);
	
	#if SYMBIAN
  	testResultXml("dirname_test");
  	#endif /* EMULATOR */

	rmdir("c:\\temp\\tmp");
	rmdir("c:\\temp");
	return 0;
}
