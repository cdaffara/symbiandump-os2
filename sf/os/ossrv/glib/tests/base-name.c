// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#undef G_DISABLE_ASSERT
#undef G_LOG_DOMAIN

#include <glib.h>
#include <errno.h>
#include <string.h>
#define LOG_FILE "c:\\logs\\base_name_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

#define MAX_FILENAME_LENGTH	256
#define MAX_PATH_LENGTH		256

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("base_name_log");
    close_log_file();
}

int main (int argc, char *argv[])
{
	char folder_name[] = "c:\\example\\test\\";
	char file_name[]   = "test.txt";
	const gchar *ret_file_name;

	gchar input_file[MAX_PATH_LENGTH];
	sprintf(input_file, "%s%s", folder_name, file_name);

	ret_file_name = g_basename(input_file);
	
	if(ret_file_name != NULL)
	    {
        if(g_strcmp0(ret_file_name, file_name) !=0 )
            {
            std_log(LOG_FILENAME_LINE, "g_basename returned wrong file name");
            assert_failed = 1;
            }
	    }
	else
	    {
        std_log(LOG_FILENAME_LINE, "g_basename returned NULL. errno  = %d", errno);
        assert_failed = 1;
	    }

	
	if(assert_failed)
          std_log(LOG_FILENAME_LINE,"Test Failed");
    else
          std_log(LOG_FILENAME_LINE,"Test Successful");
	
    create_xml(0);

	return 0;
}
