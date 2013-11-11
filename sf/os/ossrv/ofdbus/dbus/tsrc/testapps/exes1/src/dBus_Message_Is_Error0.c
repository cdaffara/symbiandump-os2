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

#define LOG_FILE "c:\\logs\\DBus_Message_Is_Error01_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
	if(result)
		assert_failed = 1;
	
	testResultXml("DBus_Message_Is_Error0");
	close_log_file();
}

int main()
{
 	DBusMessage* msg;
	char* err = "Testing.Is.Error.Setting.API";
	
	msg = dbus_message_new(DBUS_MESSAGE_TYPE_ERROR);
	
	if(!msg)
	{
		create_xml(1);
		return 1;
	}
	
	std_log(LOG_FILENAME_LINE, "Error created.");
	
	
	if(!dbus_message_set_error_name(msg, err))
	{
		create_xml(1);
		return 1;
	}
	
	std_log(LOG_FILENAME_LINE, "Error name set.");
	
	
	if(!dbus_message_is_error(msg, err))
	{
		std_log(LOG_FILENAME_LINE, "Correct Error Name is not matching");
		create_xml(1);
		return 1;
	}
	
	std_log(LOG_FILENAME_LINE, "SUCCESS");
	create_xml(0);
	return 0;
}
