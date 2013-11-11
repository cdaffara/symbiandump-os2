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

#define LOG_FILE "c:\\logs\\DBus_Type_Container01_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
	if(result)
		assert_failed = 1;
	
	testResultXml("DBus_Type_Container0");
	close_log_file();
}

int main()
{
 	if(dbus_type_is_container(DBUS_TYPE_BOOLEAN))	
	{
		std_log(LOG_FILENAME_LINE, "Fail for %s","DBUS_TYPE_BOOLEAN");
		create_xml(1);
		return 1;
	}
	if(dbus_type_is_container(DBUS_TYPE_INT16))	
	{
		std_log(LOG_FILENAME_LINE, "Fail for %s","DBUS_TYPE_INT16");
		create_xml(1);
		return 1;
	}
	if(dbus_type_is_container(DBUS_TYPE_BYTE))	
	{
		std_log(LOG_FILENAME_LINE, "Fail for %s","DBUS_TYPE_BYTE");
		create_xml(1);
		return 1;
	}
	if(dbus_type_is_container(DBUS_TYPE_UINT16))	
	{
		std_log(LOG_FILENAME_LINE, "Fail for %s","DBUS_TYPE_UINT16");
		create_xml(1);
		return 1;
	}
	if(dbus_type_is_container(DBUS_TYPE_INT32))	
	{
		std_log(LOG_FILENAME_LINE, "Fail for %s","DBUS_TYPE_INT32");
		create_xml(1);
		return 1;
	}
	if(dbus_type_is_container(DBUS_TYPE_UINT32))	
	{
		std_log(LOG_FILENAME_LINE, "Fail for %s","DBUS_TYPE_UINT32");
		create_xml(1);
		return 1;
	}
	if(dbus_type_is_container(DBUS_TYPE_INT64))	
	{
		std_log(LOG_FILENAME_LINE, "Fail for %s","DBUS_TYPE_INT64");
		create_xml(1);
		return 1;
	}
	if(dbus_type_is_container(DBUS_TYPE_UINT64))	
	{
		std_log(LOG_FILENAME_LINE, "Fail for %s","DBUS_TYPE_UINT64");
		create_xml(1);
		return 1;
	}
	if(dbus_type_is_container(DBUS_TYPE_DOUBLE))	
	{
		std_log(LOG_FILENAME_LINE, "Fail for %s","DBUS_TYPE_DOUBLE");
		create_xml(1);
		return 1;
	}
	if(dbus_type_is_container(DBUS_TYPE_STRING))	
	{
		std_log(LOG_FILENAME_LINE, "Fail for %s","DBUS_TYPE_STRING");
		create_xml(1);
		return 1;
	}
	if(dbus_type_is_container(DBUS_TYPE_OBJECT_PATH))	
	{
		std_log(LOG_FILENAME_LINE, "Fail for %s","DBUS_TYPE_OBJECT_PATH");
		create_xml(1);
		return 1;
	}
	if(dbus_type_is_container(DBUS_TYPE_SIGNATURE))	
	{
		std_log(LOG_FILENAME_LINE, "Fail for %s","DBUS_TYPE_SIGNATURE");
		create_xml(1);
		return 1;
	}
	if(!dbus_type_is_container(DBUS_TYPE_ARRAY))	
	{
		std_log(LOG_FILENAME_LINE, "Fail for %s","DBUS_TYPE_ARRAY");
		create_xml(1);
		return 1;
	}
	if(!dbus_type_is_container(DBUS_TYPE_VARIANT))	
	{
		std_log(LOG_FILENAME_LINE, "Fail for %s","DBUS_TYPE_VARIANT");
		create_xml(1);
		return 1;
	}
	if(!dbus_type_is_container(DBUS_TYPE_STRUCT))	
	{
	  std_log(LOG_FILENAME_LINE, "Fail for %s","DBUS_TYPE_STRUCT");
		create_xml(1);
		return 1;
	}
	if(!dbus_type_is_container(DBUS_TYPE_DICT_ENTRY))	
	{
		std_log(LOG_FILENAME_LINE, "Fail for %s","DBUS_TYPE_DICT_ENTRY");
		create_xml(1);
		return 1;
	}
	
	std_log(LOG_FILENAME_LINE, "Success");
		create_xml(0);
		return 0;
}
