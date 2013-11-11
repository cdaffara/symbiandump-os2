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
#define LOG_FILE "c:\\logs\\utf8_string_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("utf8_string_log");
    close_log_file();
}

int main (int argc, char *argv[])
{
    gchar *src = "TestString";
    gchar dest[20];
    gchar *p;
    
    p = g_utf8_strncpy(dest, src, strlen(src));
    
    if(p == dest)
        {
        if(strcmp(dest, src))
            {
            std_log(LOG_FILENAME_LINE,"g_utf8_strncpy didnt work as expected");
            assert_failed = 1;
            }
        }
    else
        {
        std_log(LOG_FILENAME_LINE,"g_utf8_strncpy's return value is invalid");
        assert_failed = 1;
        }
    
    p = NULL;
    p = g_utf8_strrchr(src, strlen(src), 't');
    if(strcmp(p, "tring"))
        {
        std_log(LOG_FILENAME_LINE,"g_utf8_strrchr didnt work as expected");
        assert_failed = 1;
        }
    
	if(assert_failed)
          std_log(LOG_FILENAME_LINE,"Test Failed");
    else
          std_log(LOG_FILENAME_LINE,"Test Successful");
	
    create_xml(0);

	return 0;
}
