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
#include <glib.h>
#include <gspawn.h>
#include <mrtlib.h>
#include <stdlib.h>

#ifdef SYMBIAN
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/

/* The following test function returns 1 only when the any one of the g_spawn_async tests fails. *
 * For pass of all the test the function returns 0.												 */

int g_spawn_async_test()
{
	gchar *working_directory = NULL;
    gchar **envp = NULL;
    gpointer user_data = "123";
	GError *error = NULL;
	GPid child_pid;
    GSpawnChildSetupFunc child_setup = NULL;
    int retVal = 0;
    
    int flags = 0;
    
    char **argv = NULL; // argv is NULL. should cause g_spawn_async to fail.
     
	argv = (char **)malloc(3*sizeof(char *));    
	argv[0] = "Helloworld1.exe"; // wrong exe name. Should cause g_spawn_async to fail
    argv[1] = "Hello";
    argv[2] = NULL;
	
	if (g_spawn_async(working_directory,argv,envp,flags,child_setup,user_data,&child_pid,&error))
	{
	 	retVal = 1;
	}
	else
	{
		
	}	

	argv[0] = "Helloworld.exe"; // set the correct value of argv so that g_spawn_sync is sucessful
	
	if (g_spawn_async(working_directory,argv,envp,flags,child_setup,user_data,&child_pid,&error))
	{
	
	}
	else
	{
		if(error)
		{
			g_log(NULL,G_LOG_LEVEL_CRITICAL,"file %s: line %d Error Message:%s",__FILE__,	 __LINE__, error->message);
			g_error_free(error);
			error = NULL;
		}
		else
		{
			g_log(NULL,G_LOG_LEVEL_CRITICAL,"Error in file %s: at line %d",__FILE__, __LINE__);
		}
		retVal = 1;
	}	
	
	child_setup = NULL;
	
	flags = G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_SEARCH_PATH;
	
	if (g_spawn_async(working_directory,argv,envp,flags,child_setup,user_data,&child_pid,&error))
	{
		g_spawn_close_pid(child_pid);
	}
	else
	{
		if(error)
		{
			g_log(NULL,G_LOG_LEVEL_CRITICAL,"file %s: line %d Error Message:%s",__FILE__,	 __LINE__, error->message);
			g_error_free(error);
			error = NULL;
		}
		else
		{
			g_log(NULL,G_LOG_LEVEL_CRITICAL,"Error in file %s: at line %d",__FILE__, __LINE__);
		}
		retVal = 1;
	}	
	
	
	flags = G_SPAWN_FILE_AND_ARGV_ZERO;
	if (g_spawn_async(working_directory,argv,envp,flags,child_setup,user_data,&child_pid,&error))
	{
	
	}
	else
	{
		if(error)
		{
			g_log(NULL,G_LOG_LEVEL_CRITICAL,"file %s: line %d Error Message:%s",__FILE__,	 __LINE__, error->message);
			g_error_free(error);
			error = NULL;
		}
		else
		{
			g_log(NULL,G_LOG_LEVEL_CRITICAL,"Error in file %s: at line %d",__FILE__, __LINE__);
		}
		retVal = 1;
	}	
	
	working_directory = "c:\\sys\\bin";
	envp = (char **)malloc(2*sizeof(char *));
	envp[0] = "path = c:\\sys\\bin";
	envp[1] = NULL;
	if (g_spawn_async(working_directory,argv,envp,flags,child_setup,user_data,&child_pid,&error))
	{
	
	}
	else
	{
		if(error)
		{
			g_log(NULL,G_LOG_LEVEL_CRITICAL,"file %s: line %d Error Message:%s",__FILE__,	 __LINE__, error->message);
			g_error_free(error);
			error = NULL;
		}
		else
		{
			g_log(NULL,G_LOG_LEVEL_CRITICAL,"Error in file %s: at line %d",__FILE__, __LINE__);
		}
		retVal = 1;
	}
	
	if(envp)
		free(envp);
	if(argv)
		free(argv);
	
	return retVal;
}


/* The following test function returns 1 only when the any one of the g_spawn_async_with_pipes   *
   tests fails.For pass of all the test the function returns 0.									 */
int g_spawn_async_with_pipes_test()
{
	gchar *working_directory = NULL;
    gchar **envp = NULL;
    gpointer user_data = "123";
	GError *error = NULL;
	GPid child_pid;
    GSpawnChildSetupFunc child_setup = NULL;
    int retVal = 0;
    
    int flags = 0;
    
    int standard_input, standard_output,standard_error;
    
    char **argv = (char **)malloc(3*sizeof(char *));    
	argv[0] = "Helloworld.exe"; // wrong exe name. Should cause g_spawn_async to fail
    argv[1] = "Hello";
    argv[2] = NULL;
    
    
    if(g_spawn_async_with_pipes(working_directory,argv,envp,flags,child_setup,user_data,&child_pid,NULL,NULL,NULL,&error))
    {
	
	}
	else
	{
		if(error)
		{
			g_log(NULL,G_LOG_LEVEL_CRITICAL,"file %s: line %d Error Message:%s",__FILE__,	 __LINE__, error->message);
			g_error_free(error);
			error = NULL;
		}
		else
		{
			g_log(NULL,G_LOG_LEVEL_CRITICAL,"Error in file %s: at line %d",__FILE__, __LINE__);
		}
		retVal = 1;
	}
   	
   	flags = G_SPAWN_FILE_AND_ARGV_ZERO;
   	
   	if(g_spawn_async_with_pipes(working_directory,argv,envp,flags,child_setup,user_data,&child_pid,&standard_input,&standard_output,&standard_error,&error))
    {
    	if(standard_input != -1 || standard_output != -1 || standard_error != -1)
    	{
    		g_log(NULL,G_LOG_LEVEL_CRITICAL,"Error in file %s: at line %d",__FILE__, __LINE__);
    		retVal = 1;
    	}
	}
	else
	{
		if(error)
		{
			g_log(NULL,G_LOG_LEVEL_CRITICAL,"file %s: line %d Error Message:%s",__FILE__,	 __LINE__, error->message);
			g_error_free(error);
			error = NULL;
		}
		else
		{
			g_log(NULL,G_LOG_LEVEL_CRITICAL,"Error in file %s: at line %d",__FILE__, __LINE__);
		}
		retVal = 1;
	}
   	return retVal; 
}

/* The following test function returns 1 only when the any one of the g_spawn_sync			     *
   tests fails.For pass of all the test the function returns 0.									 */
int g_spawn_sync_test()
{
	gchar *working_directory = NULL;
    gchar **envp = NULL;
    gpointer user_data = "123";
	GError *error = NULL;
	int exit_status;
	GPid child_pid;
    GSpawnChildSetupFunc child_setup = NULL;
    int retVal = 0;
    
    int flags = 0;
    
    gchar *standard_output = NULL, *standard_error = NULL;
    
    char **argv = (char **)malloc(3*sizeof(char *));    
	argv[0] = "Helloworld.exe"; 
    argv[1] = "Hello";
    argv[2] = NULL;
    
    flags = G_SPAWN_FILE_AND_ARGV_ZERO;
    
    if(g_spawn_sync(working_directory,argv,envp,flags,NULL,user_data,NULL,NULL,&exit_status,&error))
    {
    	if(exit_status != 0)
    	{
    		g_log(NULL,G_LOG_LEVEL_CRITICAL,"Error in file %s: at line %d",__FILE__, __LINE__);
    		retVal = 1;
    	}
    }
	else
	{
		if(error)
		{
			g_log(NULL,G_LOG_LEVEL_CRITICAL,"file %s: line %d Error Message:%s",__FILE__,	 __LINE__, error->message);
			g_error_free(error);
			error = NULL;
		}
		else
		{
			g_log(NULL,G_LOG_LEVEL_CRITICAL,"Error in file %s: at line %d",__FILE__, __LINE__);
		}
		retVal = 1;
	}
	
	if(g_spawn_sync(working_directory,argv,envp,flags,NULL,user_data,&standard_output,&standard_error,NULL,&error))
    {
    
   	}
	else
	{
		if(error)
		{
			g_log(NULL,G_LOG_LEVEL_CRITICAL,"file %s: line %d Error Message:%s",__FILE__,	 __LINE__, error->message);
			g_error_free(error);
			error = NULL;
		}
		else
		{
			g_log(NULL,G_LOG_LEVEL_CRITICAL,"Error in file %s: at line %d",__FILE__, __LINE__);
		}
		retVal = 1;
	}
   	
   	
   	return retVal; 
    
}


/* The following test function returns 1 only when the any one of the g_spawn_command_line_async *
   tests fails.For pass of all the test the function returns 0.									 */
int g_spawn_command_line_async_tests()
{
	GError *error = NULL;
	int retVal = 0;
	
	if(g_spawn_command_line_async("helloworld.exe 1 2 3",&error))
	{
    
   	}
	else
	{
		if(error)
		{
			g_log(NULL,G_LOG_LEVEL_CRITICAL,"file %s: line %d Error Message:%s",__FILE__,	 __LINE__, error->message);
			g_error_free(error);
			error = NULL;
		}
		else
		{
			g_log(NULL,G_LOG_LEVEL_CRITICAL,"Error in file %s: at line %d",__FILE__, __LINE__);
		}
		retVal = 1;
	}
	
	return retVal;
   	
}

/* The following test function returns 1 only when the any one of the g_spawn_command_line_sync  *
   tests fails.For pass of all the test the function returns 0.									 */
int g_spawn_command_line_sync_tests()
{
	GError *error = NULL;
	int retVal = 0;
	int exit_status;
	gchar *standard_output, *standard_error;
	
	if(g_spawn_command_line_sync("helloworld.exe 10 11 12",NULL,NULL,&exit_status,&error))
	{
    	if(exit_status != 0)
    	{
    		g_log(NULL,G_LOG_LEVEL_CRITICAL,"Error in file %s: at line %d",__FILE__, __LINE__);
    		retVal = 1;
    	}
   	}
	else
	{
		if(error)
		{
			g_log(NULL,G_LOG_LEVEL_CRITICAL,"file %s: line %d Error Message:%s",__FILE__,	 __LINE__, error->message);
			g_error_free(error);
			error = NULL;
		}
		else
		{
			g_log(NULL,G_LOG_LEVEL_CRITICAL,"Error in file %s: at line %d",__FILE__, __LINE__);
		}
		retVal = 1;
	}
	
	
	if(g_spawn_command_line_sync("helloworld.exe 10 11 12",&standard_output,&standard_error,&exit_status,&error))
	{
    	if(exit_status != 0 || standard_output != NULL || standard_error != NULL)
    	{
    		g_log(NULL,G_LOG_LEVEL_CRITICAL,"Error in file %s: at line %d",__FILE__, __LINE__);
    		retVal = 1;
    	}
   	}
	else
	{
		if(error)
		{
			g_log(NULL,G_LOG_LEVEL_CRITICAL,"file %s: line %d Error Message:%s",__FILE__,	 __LINE__, error->message);
			g_error_free(error);
			error = NULL;
		}
		else
		{
			g_log(NULL,G_LOG_LEVEL_CRITICAL,"Error in file %s: at line %d",__FILE__, __LINE__);
		}
		retVal = 1;
	}
	
	return retVal;
   	
}


int main()
{
	int retval = -1;
	
	#if defined(SYMBIAN) && (defined(__WINS__) || defined(__WINSCW__))
     
   	testResultXml("spawn_test");
   	return 0;
  
  	#endif // EMULATOR
	
	#ifdef SYMBIAN
	GLIB_INIT();
  	g_log_set_handler (NULL, G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
  	#endif
  	
    // calling g_spawn_close_pid() with some invalid handle. Should not cause panic.
    // Instead the API should return without any problems.
    g_spawn_close_pid(146545);

	if(g_spawn_async_test() || g_spawn_async_with_pipes_test() || g_spawn_sync_test() || g_spawn_command_line_async_tests() || g_spawn_command_line_sync_tests())
		retval = 1;
	else
		retval = 0;
	
	assert_failed = retval;
	
	#ifdef SYMBIAN
  	testResultXml("spawn_test");
  	#endif /* EMULATOR */
	
	return retval;
}
