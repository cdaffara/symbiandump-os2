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

#define LOG_FILE "c:\\logs\\dbus_message_copy0_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

int TestMessageEqual(DBusMessage* msg1, DBusMessage* msg2)
{
	if(dbus_message_get_type(msg2) != dbus_message_get_type(msg1))
		return FALSE;
	
	if(strcmp(dbus_message_get_interface(msg2), dbus_message_get_interface(msg1)))
		return FALSE;
	
	if(strcmp(dbus_message_get_path(msg2), dbus_message_get_path(msg1)))
		return FALSE;
	
	if(strcmp(dbus_message_get_member(msg2), dbus_message_get_member(msg1)))
		return FALSE;
	  
	return TRUE;
}
int CheckConnection(DBusConnection* conn, DBusError* error)
	{
		if(!conn)
		{
			std_log(LOG_FILENAME_LINE,"Failed to get connection with error :: %s\n%s", error->name, error->message);
	    	
	    	dbus_error_free(error);
	    	return FALSE;
		}
		
		return TRUE;
	}

void create_xml(int result)
{
	if(result)
		assert_failed = 1;
	
	testResultXml("dbus_message_copy0");
	close_log_file();
}

int main()
{

	
	DBusMessage* src_msg;
	DBusMessage* dest_msg = NULL;

	
	src_msg = dbus_message_new(DBUS_MESSAGE_TYPE_METHOD_CALL);
	
	if(!dbus_message_set_interface(src_msg, "Test.Message.Copy"))
	{
		std_log(LOG_FILENAME_LINE,"Not enough memory");
		create_xml(1);
		return 1;
	}
	
	if(!dbus_message_set_path(src_msg, "/Test/Message/Copy0"))
	{
		std_log(LOG_FILENAME_LINE,"Not enough memory");
		create_xml(1);
		return 1;
	}
	
	if(!dbus_message_set_member(src_msg, "Member"))
	{
		std_log(LOG_FILENAME_LINE,"Not enough memory");
		create_xml(1);
		return 1;
	}
	
	dest_msg = dbus_message_copy(src_msg);
	
	if(!dest_msg)
	{
		std_log(LOG_FILENAME_LINE,"Not enough memory");
		create_xml(1);
		return 1;
	}
	
	if(!TestMessageEqual(src_msg, dest_msg))
	{
		std_log(LOG_FILENAME_LINE,"Messages are different.");
		create_xml(1);
		return 1;
	}
	
	
	std_log(LOG_FILENAME_LINE,"Test Successful");
	create_xml(0);
	return 0;
	
}
