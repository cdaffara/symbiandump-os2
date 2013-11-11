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
    __UHEAP_MARK;
    {
    wstring W(L"Hello World");
    HBufC8 *buf = HBufC8::NewMaxL(23);
//    int i=0;
//    while (i<=1000)
//    {
    retval =  WstringToHbufc8(W, buf);
//    i++;
//    printf("%d\n",i);
//    }    

    if (retval ==ESuccess)
    {
    printf("wstringtotptrc8 Passed");
    }
    else
    {
    assert_failed = true;
    printf("wstringtotptr8 Failed");
    }     
    delete buf;
    }
    __UHEAP_MARKEND;
    testResultXml("test_wstringtohbufc8_positive");
	return 0;
}
