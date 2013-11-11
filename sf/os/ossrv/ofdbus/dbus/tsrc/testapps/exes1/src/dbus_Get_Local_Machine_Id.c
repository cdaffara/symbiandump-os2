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

#define LOG_FILE "c:\\logs\\dbus_get_local_machine_id_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
	if(result)
		assert_failed = 1;
	
	testResultXml("dbus_get_local_machine_id");
	close_log_file();
}

int main()
{
    DBusError error;
    DBusError error1;
    
    char* id =  NULL; 
    
    dbus_error_init (&error); 
    dbus_error_init (&error1);
    
    id=dbus_get_local_machine_id();
    if(id == NULL)
    	{
    		std_log(LOG_FILENAME_LINE, "ID = NULL");
    		std_log(LOG_FILENAME_LINE, "test fail");
				create_xml(1);
    		return 1;
    	}
    else
    	{
	    std_log(LOG_FILENAME_LINE, "SUCCESS");
	    create_xml(0);
			return 0;
    	}
}
