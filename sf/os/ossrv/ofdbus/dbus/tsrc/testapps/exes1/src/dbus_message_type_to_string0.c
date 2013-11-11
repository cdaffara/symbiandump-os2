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

#define LOG_FILE "c:\\logs\\dbus_message_type_to_string0_log1.txt"
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
	
	testResultXml("dbus_message_type_to_string0");
	close_log_file();
}

int main()
{

	
	if(strcmp("method_call", dbus_message_type_to_string(DBUS_MESSAGE_TYPE_METHOD_CALL)))
	{
		std_log(LOG_FILENAME_LINE,"dbus_message_type_to_string() failed for DBUS_MESSAGE_TYPE_METHOD_CALL.");
		create_xml(1);
		return 1;
	}
	
	if(strcmp("method_return", dbus_message_type_to_string(DBUS_MESSAGE_TYPE_METHOD_RETURN)))
	{
		std_log(LOG_FILENAME_LINE,"dbus_message_type_to_string() failed for DBUS_MESSAGE_TYPE_METHOD_RETURN.");
		create_xml(1);
		return 1;
	}
	
	if(strcmp("signal", dbus_message_type_to_string(DBUS_MESSAGE_TYPE_SIGNAL)))
	{
		std_log(LOG_FILENAME_LINE,"dbus_message_type_to_string() failed for DBUS_MESSAGE_TYPE_SIGNAL.");
		create_xml(1);
		return 1;
	}
	
	if(strcmp("error", dbus_message_type_to_string(DBUS_MESSAGE_TYPE_ERROR)))
	{
		std_log(LOG_FILENAME_LINE,"dbus_message_type_to_string() failed for DBUS_MESSAGE_TYPE_ERROR.");
		create_xml(1);
		return 1;
	}
	
	if(strcmp("invalid", dbus_message_type_to_string(123)))
	{
		std_log(LOG_FILENAME_LINE,"dbus_message_type_to_string() failed for DBUS_MESSAGE_TYPE_INVALID.");
		create_xml(1);
		return 1;
	}
	
	std_log(LOG_FILENAME_LINE,"Test Successful");
	create_xml(0);
	return 0;
}
