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

#define LOG_FILE "c:\\logs\\dbus_error_has_name1_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
	if(result)
		assert_failed = 1;
	
	testResultXml("dbus_error_has_name1");
	close_log_file();
}

int main()
{
	DBusError error;
	
	dbus_error_init(&error);
	
	dbus_set_error(&error, "Sample Error0", NULL);
	
	if(dbus_error_has_name(&error, "Sample Error01"))
	{
		std_log(LOG_FILENAME_LINE, "Match in Error Name");
		create_xml(1);
		return 1;
	}
	
	dbus_error_free(&error);
	std_log(LOG_FILENAME_LINE, "Test Successful");
	create_xml(0);
	return 0;
}
