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

#define LOG_FILE "c:\\logs\\Dbus_Maloc1_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
	if(result)
		assert_failed = 1;
	
	testResultXml("Dbus_Maloc");
	close_log_file();
}

int main()
{
   int *Mem_res;
	 int *rel_res;
   
	Mem_res = (int*)dbus_malloc(sizeof(int));
	 
	if(Mem_res== NULL)
		{
		std_log(LOG_FILENAME_LINE, "Malloc unsuccessful");
		std_log(LOG_FILENAME_LINE, "FAIL");
		create_xml(1);
		return 1;
		}
	else
		{
			std_log(LOG_FILENAME_LINE, "malloc successful");
				
			rel_res = (int*)dbus_realloc(Mem_res,10);
			if(rel_res == NULL)
				{
					std_log(LOG_FILENAME_LINE, "realloc is unsuccessful");
					std_log(LOG_FILENAME_LINE, "FAIL");
					dbus_free(Mem_res);
					create_xml(1);
					return 1;
				}
				else
					{
						std_log(LOG_FILENAME_LINE, "realloc is successful");
					std_log(LOG_FILENAME_LINE, "Success");
					create_xml(0);
					return 0;  
					}
		} 
		dbus_free(rel_res);
}
