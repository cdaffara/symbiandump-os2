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

#define LOG_FILE "c:\\logs\\dbus_message_get_path_decompose0_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
	if(result)
		assert_failed = 1;
	
	testResultXml("dbus_message_get_path_decompose0");
	close_log_file();
}

int main()
{
        DBusMessage* msg;
	char* path1;
		char** path2;
		char*** path ;
		int i=0;
		int path_decompose;
		
		path1 = "/type/Message/Call";
	    path =  &path2;
		
		
		msg = dbus_message_new(DBUS_MESSAGE_TYPE_METHOD_CALL);
		dbus_message_set_path(msg, path1);
		
		if(!msg)
		{ 
			std_log(LOG_FILENAME_LINE,"Fail to create Message");
			create_xml(1);
		    return 1; 
		}
		
		path_decompose = dbus_message_get_path_decomposed(msg,&path2);		
		if(path_decompose )
		{
		while(path2[i] != NULL)
			{
			std_log(LOG_FILENAME_LINE,"%s",path2[i]);    
			i++;}
			std_log(LOG_FILENAME_LINE,"Test Successful"); 
			create_xml(0);
			return 0; 
			
		} 
		
		else
			{
				std_log(LOG_FILENAME_LINE,"Test Fail");
				create_xml(1);
				return 1;
			} 
			
}
