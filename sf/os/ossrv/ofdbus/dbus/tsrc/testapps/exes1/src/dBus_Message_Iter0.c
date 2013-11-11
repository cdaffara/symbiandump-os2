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

#define LOG_FILE "c:\\logs\\DBus_Message_Iter01_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
	if(result)
		assert_failed = 1;
	
	testResultXml("DBus_Message_Iter0");
	close_log_file();
}

int main()
{
 	DBusMessage* msg;
	DBusMessageIter iter_append;
	DBusMessageIter iter_read;
	DBusMessageIter sub_iter;
	dbus_uint32_t num = 22;
	dbus_uint32_t num_r = 22;
	const dbus_int32_t arr[] = {0,1,2,3,4,5};
	dbus_int32_t* arr_r;
	int arr_len;
	int arr_elements;
	const dbus_int32_t* array = arr;
	int type;
	int i;
	
	
	msg = dbus_message_new(DBUS_MESSAGE_TYPE_METHOD_CALL);
	if(!msg)
		{
		create_xml(1);
		return 1;
		}
	
	dbus_message_iter_init_append(msg, &iter_append);
	
	if(!dbus_message_iter_append_basic(&iter_append, DBUS_TYPE_UINT32, &num))
		{
		create_xml(1);
		return 1;
		}
	
	if(!dbus_message_iter_open_container(&iter_append, DBUS_TYPE_ARRAY, DBUS_TYPE_INT32_AS_STRING, &sub_iter))
		{
		create_xml(1);
		return 1;
		}
	if(!dbus_message_iter_append_fixed_array(&sub_iter, DBUS_TYPE_INT32, &array, 6))
		{
		create_xml(1);
		return 1;
		}
	
	dbus_message_iter_close_container(&iter_append, &sub_iter);
	if(!dbus_message_iter_init(msg, &iter_read))
		{
		create_xml(1);
		return 1;
		}

	while((type = dbus_message_iter_get_arg_type(&iter_read)) != DBUS_TYPE_INVALID)
		{
		if(type == DBUS_TYPE_UINT32)
			{
			dbus_message_iter_get_basic(&iter_read, &num_r);
			if(num_r != 22)
				{
				std_log(LOG_FILENAME_LINE, "Argumnet Mismatch for UINT32");
				create_xml(1);
				return 1;
				}
			}  
		if(type == DBUS_TYPE_ARRAY)
			{
			dbus_message_iter_recurse(&iter_read, &sub_iter);
			arr_len = dbus_message_iter_get_array_len(&sub_iter);
			std_log(LOG_FILENAME_LINE, "No.of bytes in array :: %d",arr_len);
			dbus_message_iter_get_fixed_array(&sub_iter, &arr_r, &arr_elements);
			
			for(i=0; i<arr_elements; i++)
				{
				if(i != arr_r[i])
					{
					std_log(LOG_FILENAME_LINE, "Array Elements are not matching");
					create_xml(1);
					return 1;
					}
				std_log(LOG_FILENAME_LINE, "%d",arr_r[i]);
				}
			}
		dbus_message_iter_next(&iter_read);
		}

	std_log(LOG_FILENAME_LINE, "SUCCESS");
	create_xml(0);
	return 0;
}
