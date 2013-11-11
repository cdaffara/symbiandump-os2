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
    TPtr8 buf((unsigned char *)"", 10 , 24);
    char *temp = new char [23];
    retval = WstringToTptr8(W, temp, buf);

    if (retval ==ESuccess)
    {
    printf("wstringtotptr8 Passed");
    }
    else
    {
    assert_failed = true;
    printf("wstringtotptr8 Failed");
    }     
    delete[] temp;
    temp=NULL;
    __UHEAP_MARKEND;
    testResultXml("test_wstringtotptr8_positive");
	return 0;
}
