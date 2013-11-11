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
    __UHEAP_MARK;
    int retval =ESuccess;
    char* mycharstring = "Hello String";
    TBuf8 <5> myBuf;
    retval = CharToTbuf8 (mycharstring, myBuf);

    if (retval == EInsufficientMemory)
    {
    printf("chartotbuf8 negative1 Passed\n");
    }
    else
    {
    assert_failed = true;
    printf("chartotbuf8 negative1 Failed\n");
    }      
    __UHEAP_MARKEND;
    testResultXml("test_chartotbuf8_negative1");
	return 0;
}
