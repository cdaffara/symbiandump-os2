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
int main()
{
    __UHEAP_MARK;
    int retval =ESuccess;
    char* mycharstring = "Hello char String";
    HBufC16 *buf = HBufC16::NewMaxL(20);
    CleanupStack::PushL(buf);
//    int i=0;
//    while (i<=1000)
//    {
    retval = CharToHbufc16(mycharstring,buf);
//    i++;
//    printf("%d\n",i);
//    }    
    if (retval ==ESuccess)
    {
    printf("chartohbufc16 Passed");
    }
    else
    {
    assert_failed = true;
    printf("chartohbufc16 Failed");
    }      
    CleanupStack::PopAndDestroy(1);
    __UHEAP_MARKEND;
    testResultXml("test_chartohbufc16_positive");
	return 0;
}
