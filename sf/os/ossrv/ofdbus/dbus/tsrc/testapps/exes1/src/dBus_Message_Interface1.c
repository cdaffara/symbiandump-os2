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

#define LOG_FILE "c:\\logs\\DBus_Message_Interface11_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
	if(result)
		assert_failed = 1;
	
	testResultXml("DBus_Message_Interface1");
	close_log_file();
}

int main()
{
 	DBusMessage* msg;
 	char* iface = "Test.Message.Iface";
	
	msg = dbus_message_new(DBUS_MESSAGE_TYPE_SIGNAL);
	
	if(!dbus_message_set_interface(msg, iface))
	{
		std_log(LOG_FILENAME_LINE, "Not enough memory");
		create_xml(1);
		return 1;
	}
	
	if(strcmp(iface, dbus_message_get_interface(msg)))
	{
		std_log(LOG_FILENAME_LINE, "Not enough memory");
		create_xml(1);
		return 1;
	}
	
	std_log(LOG_FILENAME_LINE, "SUCCESS");
		create_xml(0);
	return 0;
}
