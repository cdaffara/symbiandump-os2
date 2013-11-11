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
#define LOG_FILE "c:\\logs\\nullify_ptr_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("nullify_ptr_log");
    close_log_file();
}

int main (int argc, char *argv[])
{
    char *ptr;
	g_nullify_pointer((gpointer)&ptr);
	
	if(ptr != NULL)
	    {
        std_log(LOG_FILENAME_LINE,"ptr is not NULL");
	    assert_failed = 1;
	    }
	
	if(assert_failed)
          std_log(LOG_FILENAME_LINE,"Test Fail");
    else
          std_log(LOG_FILENAME_LINE,"Test Successful");
	
    create_xml(0);

	return 0;
}
