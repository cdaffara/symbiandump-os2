/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include<stdio.h>  
#include <dbus/dbus.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <glib.h>
#include <locale.h>
#include <spawn.h>
#include "dbus-string.h"
#include "dbus-auth-script.h"


#include <sys/types.h>  
#include <dirent.h> 

#define PATH_FOR_SCRIPT "c:\\data\\dbus\\auth\\"

#define LOG_FILE "c:\\logs\\auth_test_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(char* file_name, int result)
	{
	if(result)
		assert_failed = 1;
	else
		assert_failed = 0;
	
	testResultXml(file_name);
	}

int main(int argc, char*argv[])
{
		DBusConnection* Connection;
		DBusError error;
		FILE *fp;
   		char temp[100] = PATH_FOR_SCRIPT;	
  		DBusString file_name;
  		int i = 0;		
		
  		char *testcases[] = 
  		{
  			"client-invalid-command-handle.auth-script",
  			"cancel.auth-script",
  			"client-side-successful.auth-script",
  			"client-out-of-mechanisms.auth-script",
  			"external-failed.auth-script",
  			"external-successful.auth-script",
  			"extra-bytes.auth-script",
  			"fail-after-n-attempts.auth-script",
  			"fallback.auth-script",
  			"invalid-command.auth-script",
  			"invalid-command-client.auth-script",
  			"invalid-hex-encoding.auth-script",
  			"mechanisms.auth-script",
  		
  		};
  		
  		fp = fopen("C:\\logs\\logs_for_auth_script.txt","w");
  		
		dbus_error_init(&error);
		
		Connection = dbus_bus_get_private(DBUS_BUS_SESSION,&error);
		
	
  		_dbus_string_init(&file_name);
  		
  		for(i =0;i<12;i++)
  		{
  		_dbus_string_append(&file_name, strcat(temp, testcases[i]));
  		if(!_dbus_auth_script_run(&file_name))
  		{
  				fprintf(fp,"FAILED - %s \n",testcases[i]);
				std_log(LOG_FILENAME_LINE, "FAILED - %s \n", testcases[i]);
				create_xml(testcases[i], 1);
  		}
  		else
  		{
  				fprintf(fp,"PASSED - %s \n",testcases[i]);
				std_log(LOG_FILENAME_LINE, "PASSED - %s \n", testcases[i]);
				create_xml(testcases[i], 0);
  		}
  		_dbus_string_set_length(&file_name,0);
  		fflush(fp);
  		strcpy(temp, PATH_FOR_SCRIPT);
  		}
		
		dbus_connection_close(Connection);
		dbus_connection_unref(Connection);
		dbus_shutdown();
		fclose(fp);
		close_log_file();
	
	return 0;
}
