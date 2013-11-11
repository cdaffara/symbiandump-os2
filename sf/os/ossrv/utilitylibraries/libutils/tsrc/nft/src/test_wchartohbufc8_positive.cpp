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
    int retval =ESuccess;
    wchar_t* mycharstring = L"Hello";
    HBufC8* hbufc  = HBufC8::NewL(10);
    CleanupStack::PushL(hbufc);
    int i=0;
    while (i<=1000)
    {
  	retval= WcharToHbufc8(mycharstring,hbufc);
    i++;
    printf("%d\n",i);
    }    

    if (retval ==EUseNewMaxL)
    {
    printf("\nwchartoHbuf8 Passed");
    }
    else
    {
    assert_failed = true;
    printf("\nwchartoHbuf8 Failed");
    }    
    CleanupStack::PopAndDestroy(1);  
    __UHEAP_MARKEND;
    testResultXml("test_wchartohbufc8_positive");
}
