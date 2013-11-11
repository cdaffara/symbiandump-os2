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
#define LOG_FILE "c:\\logs\\app_name_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("app_name_log");
    close_log_file();
}

int main (int argc, char *argv[])
{
    char appName[] = "TestApp";
    const char *retAppName;
    
    g_set_application_name(appName);
    
    retAppName = g_get_application_name();
	
	if(retAppName != NULL)
	    {
        if(g_strcmp0(appName, retAppName) !=0 )
            {
            std_log(LOG_FILENAME_LINE, "g_get_application_name returned wrong name");
            assert_failed = 1;
            }
	    }
	else
	    {
        std_log(LOG_FILENAME_LINE, "g_get_application_name returned NULL. errno  = %d", errno);
        assert_failed = 1;
	    }
	
	g_free((void *)retAppName);
	
	if(assert_failed)
          std_log(LOG_FILENAME_LINE,"Test Failed");
    else
          std_log(LOG_FILENAME_LINE,"Test Successful");
	
    create_xml(0);

	return 0;
}
