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
    wstring W(L"Hello World");
    __UHEAP_MARK;
    TPtrC8 buf;
    char *temp = new char [23];
    retval = WstringToTptrc8(W, temp, buf);

    if (retval ==ESuccess)
    {
    printf("wstringtotptrc8 Passed");
    }
    else
    {
    assert_failed = true;
    printf("WstringToTptr8 Failed");
    }     
    delete[] temp;
    __UHEAP_MARKEND;
    testResultXml("test_wstringtotptrc8_positive");
	return 0;
}
