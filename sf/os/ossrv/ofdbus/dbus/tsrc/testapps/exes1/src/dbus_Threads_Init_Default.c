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

#define LOG_FILE "c:\\logs\\Dbus_Threads_Init_Default1_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
	if(result)
		assert_failed = 1;
	
	testResultXml("Dbus_Threads_Init_Default");
	close_log_file();
}

int main()
{
		int unique_name;
    DBusError error;
	  dbus_error_init(&error);
	

		unique_name = dbus_threads_init_default () ;
		
		if(unique_name)
			{
				std_log(LOG_FILENAME_LINE, "dbus thread is succesful");
		    std_log(LOG_FILENAME_LINE, "Success");
		    create_xml(0);
				return 0;  
			}
		else
			{
			 std_log(LOG_FILENAME_LINE, "dbus thread is  not succesful");
			 std_log(LOG_FILENAME_LINE, "FAIL");
			 create_xml(1);
			 return 1;
			}
}
