/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <liboil/liboil.h>
#include <liboil/liboildebug.h>
#include <liboil/liboilfunction.h>
#include <stdio.h>
#include <stdlib.h>

#include <liboil/globals.h>

#define LOG_FILE "c:\\logs\\testsuite_debug_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("testsuite_debug");
    close_log_file();
}

void test()
    {
    int level = 2;
    int ret;
    
    oil_debug_set_level(level);
    ret = oil_debug_get_level();
    
    if(ret != level)
        {
        std_log(LOG_FILENAME_LINE,"oil_debug_get/set method failed. errno = %d", errno);
        assert_failed = 1;
        }
    }

int main (int argc, char *argv[])
{
    oil_init ();

    test();
    
    if(assert_failed)
      std_log(LOG_FILENAME_LINE,"Test Failed");
    else
      std_log(LOG_FILENAME_LINE,"Test Successful");
    
    create_xml(0);
    return 0;
}

