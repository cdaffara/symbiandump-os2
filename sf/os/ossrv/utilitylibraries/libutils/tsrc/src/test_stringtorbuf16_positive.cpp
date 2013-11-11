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

#include <stdio.h>
#include<e32std.h>
#include "libutils.h"
#include"std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
int main()
{
    string s("testing");
    __UHEAP_MARK;
    int retval =ESuccess;
    RBuf16 buf;
    buf.CreateL(30);
    retval = StringToRbuf16(s, buf);

    if (retval == ESuccess)
    {
    printf("stringtorbuf16 Passed");
    }
    else
    {
    assert_failed = true;
    printf("stringtorbuf16 Failed");
    }
    buf.Close();
    __UHEAP_MARKEND;
    testResultXml("test_stringtorbuf16_positive");
	return 0;
}
