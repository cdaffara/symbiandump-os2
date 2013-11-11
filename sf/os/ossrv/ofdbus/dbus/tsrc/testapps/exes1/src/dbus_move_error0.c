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

#define LOG_FILE "c:\\logs\\dbus_move_error0_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
	if(result)
		assert_failed = 1;
	
	testResultXml("dbus_move_error0");
	close_log_file();
}

int CheckMovedError(DBusError* src, DBusError* dest, char* err)
{
	if(dbus_error_is_set(dest))
	{
		if(strcmp(err, dest->name))
		{
			std_log(LOG_FILENAME_LINE, "Destination Error Name is not set.");
			return FALSE;
		}
		else
		{
			if(dbus_error_is_set(src))
			{
				std_log(LOG_FILENAME_LINE, "Source Error is not initialised.");
				return FALSE;	
			}
			else
			{
				if(src->name)
				{
					std_log(LOG_FILENAME_LINE, "Source Error Name is not NULL.");
					return FALSE;			
				}
				else
				{
					return TRUE;
				}
			}
		}
	}
	else
	{
		std_log(LOG_FILENAME_LINE, "Destination Error is not set.");
		return FALSE;
	}
}

int main()
{
	DBusError src;
	DBusError dest;
	char err[] = "Sample Error0";	
			
	dbus_error_init(&src);
	dbus_error_init(&dest);
	
	dbus_set_error(&src, err, NULL);
	
	dbus_move_error(&src, &dest);
	
	if(!CheckMovedError(&src, &dest, err))
	{
		std_log(LOG_FILENAME_LINE, "Test Fail");
		dbus_error_free(&src);
		dbus_error_free(&dest);
		create_xml(1);
		return 1;
	}
	else
	{
		std_log(LOG_FILENAME_LINE, "Test Successful");
		dbus_error_free(&dest);
		create_xml(0);
		return 0;
	}
}
