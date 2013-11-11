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
#include <e32base.h>
#include "libutils.h"
#include"std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
void main()
{
    __UHEAP_MARK;
    {
    string s("hello");  
    int retval =ESuccess;
    HBufC16 *buf = HBufC16::NewMaxL(12);
    CleanupStack::PushL(buf);
    wchar_t *wptr = new wchar_t[15];
    int i=0;
    while (i<=1000)
    {
  	retval = StringToHbufc16(s, buf);
    i++;
    printf("%d\n",i);
    }    

    if (retval == ESuccess)
    {
    printf("stringtorbuf16 Passed");
    }
    else
    {
    assert_failed = true;
    printf("stringtorbuf16 Failed");
    }
    CleanupStack::PopAndDestroy(1);
    delete []wptr;
    wptr = NULL;
  }
    __UHEAP_MARKEND;
    testResultXml("test_stringtohbufc16_positive");
}
