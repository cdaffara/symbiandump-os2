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
    int retval =ESuccess;
    wstring W = L"hello";
    __UHEAP_MARK;
    TPtrC8 buf;
    char *temp = NULL;
    retval = WstringToTptrc8(W, temp, buf);

    if (retval == EInvalidPointer)
    {
    printf("wstringtotptrc8 Passed");
    }
    else
    {
    assert_failed = true;
    printf("wstringtotptr8 Failed");
    }     
    __UHEAP_MARKEND;
    testResultXml("test_wstringtotptrc8_negative1");
	return 0;
}
