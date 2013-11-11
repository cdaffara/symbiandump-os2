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
    int retval =ESuccess;
    wstring str;
    _LIT8(KTxt, "hellohello");
    HBufC8* buff = HBufC8::NewL(10);
    *buff = KTxt;
    CleanupStack::PushL(buff);
    int i=0;
    while (i<=1000)
    {
  	retval = Hbufc8ToWstring(buff,str);
    i++;
    printf("%d\n",i);
    }    

    if (retval ==ESuccess)
    {
    printf("hbufc8towstring Passed");
    }
    else
    {
    assert_failed = true;
    printf("hbufc8towstring failed");
    }      
    CleanupStack::PopAndDestroy(1);
    }
    __UHEAP_MARKEND;
    testResultXml("test_hbufc8towstring_positive");
}
