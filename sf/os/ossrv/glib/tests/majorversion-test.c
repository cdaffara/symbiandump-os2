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

#include<stdio.h>
#include "glib.h"
#define LOG_FILE "c:\\logs\\majorversion_test_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("version-test");
    close_log_file();
}


int
main (int   argc,
      char *argv[])
    {
    const guint *major_version = NULL;
    const guint *minor_version = NULL;
    const guint *micro_version = NULL;
    const guint *interface_age = NULL;
    const guint *binary_age = NULL;
    const gchar *check_version = NULL;
    major_version = _glib_major_version();
    minor_version = _glib_minor_version(); 
    micro_version = _glib_micro_version();
    interface_age = _glib_interface_age();
    binary_age    = _glib_binary_age();  
    std_log(LOG_FILENAME_LINE,"Binary age :%d\n Interface age:%d",*binary_age,*interface_age);
    check_version = glib_check_version((*major_version),(*minor_version),(*micro_version));
    if(check_version != NULL)
        {
        std_log(LOG_FILENAME_LINE,"GLib library in use is not compatible with the given verison");
        assert_failed = 1;
        }
    if(assert_failed)
        {
        std_log(LOG_FILENAME_LINE,"%s",check_version);
        std_log(LOG_FILENAME_LINE,"Test Fail");
        }
    else
        std_log(LOG_FILENAME_LINE,"Test Successful");    
          
    
    create_xml(assert_failed);
    return 0;
    }
    

