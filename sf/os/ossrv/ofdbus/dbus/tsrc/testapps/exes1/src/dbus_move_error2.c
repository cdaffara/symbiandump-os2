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

#define LOG_FILE "c:\\logs\\dbus_move_error2_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
	if(result)
		assert_failed = 1;
	
	testResultXml("dbus_move_error2");
	close_log_file();
}

int main()
{
	DBusError src;
	char err[] = "Sample Error0";
	
	dbus_error_init(&src);
	
	dbus_set_error(&src, err, NULL);
	
	dbus_move_error(&src, NULL);
			
	if(dbus_error_is_set(&src))
	{
		std_log(LOG_FILENAME_LINE, "Source Error is still set");
		std_log(LOG_FILENAME_LINE, "Test Fail");
		create_xml(1);
		return 1;
	}
	else
	{
		if(src.name || src.message)
		{
			std_log(LOG_FILENAME_LINE, "Either Name or Message of Source is not NULL.");
			create_xml(1);
			return 1;
		}
		else
		{
			std_log(LOG_FILENAME_LINE, "Test Successful");
			create_xml(0);
			return 0;
		}
	}
}
