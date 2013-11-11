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
#define LOG_FILE "c:\\logs\\at_exit_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("at_exit_log");
    close_log_file();
}

void on_exit()
    {
    std_log(LOG_FILENAME_LINE,"Test Successful");
    create_xml(0);    
    }

int main (int argc, char *argv[])
{
    g_atexit(on_exit);
    std_log(LOG_FILENAME_LINE, "Test ll be considered failed if on_exit() is not called");
    return 0;
}
