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
#define LOG_FILE "c:\\logs\\list_env_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("list_env_log");
    close_log_file();
}

int main (int argc, char *argv[])
{
    const gchar *variable1 = "TEST_VAR1";
    gchar *value1 = "testvalue1";
    const gchar *variable2 = "TEST_VAR2";
    gchar *value2 = "testvalue2";
    gchar **env_list;
    gint i, found = 0;
    guint no_of_variables =0;
    gboolean found_var1 = 0,  found_var2 = 0;
    	
    if(g_setenv (variable1, value1, TRUE) && g_setenv (variable2, value2, TRUE))
        {
        env_list = g_listenv();
        
        if(env_list)
            {
            no_of_variables = g_strv_length(env_list);
            
            if(no_of_variables)
                {
                for(i = 0; i<no_of_variables; i++)
                    {
                    gchar *data = env_list[i];
                    
                    if(g_strcmp0(data, variable1) == 0)
                        found_var1 = 1;
                    else if(g_strcmp0(data, variable2) == 0)
                        found_var2 = 1;
                    }
                
                if(!(found_var1 && found_var2))
                    {
                    std_log(LOG_FILENAME_LINE, "g_listenv failed");
                    assert_failed = 1;
                    }
                }
            else
                {
                std_log(LOG_FILENAME_LINE, "g_listenv returned list is of length 0");
                assert_failed = 1;
                }
            
            g_strfreev(env_list);
            }
        else
            {
            std_log(LOG_FILENAME_LINE, "g_listenv returned NULL");
            assert_failed = 1;
            }
        }
    else
        {
        std_log(LOG_FILENAME_LINE, "setting env variable failed. errno = %d", errno);
        }
    
    if(assert_failed)
          std_log(LOG_FILENAME_LINE,"Test Failed");
    else
          std_log(LOG_FILENAME_LINE,"Test Successful");
	
    create_xml(0);

	return 0;
}
