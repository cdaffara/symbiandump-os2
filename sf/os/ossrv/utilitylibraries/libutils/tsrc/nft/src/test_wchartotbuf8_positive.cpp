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
void main()
{
    __UHEAP_MARK;
    int retval =ESuccess;
    wchar_t* mystr = L"hello";
    TBuf8 <45> f;
    int i=0;
    while (i<=1000)
    {
  	retval = WcharToTbuf8 (mystr, f);
    i++;
    printf("%d\n",i);
    }    

    wchar_t* temp = new wchar_t[5];
    temp[1]= L'\0';
    mbstowcs(temp, (const char*)f.Ptr(),1);
    if (retval ==ESuccess)
    {
    printf("wchartotbuf8 positive0 Passed\n");
    }
    else
    {
    assert_failed = true;
    printf("wchartotbuf8 positive0 Failed\n");
    }
    delete[] temp;
    temp = NULL;
    __UHEAP_MARKEND;
    testResultXml("test_WcharToTbuf8_positive");
}
