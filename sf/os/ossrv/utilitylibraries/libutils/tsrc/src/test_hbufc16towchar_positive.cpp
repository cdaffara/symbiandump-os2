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
    wchar_t* mycharstring = new wchar_t[30];
    _LIT16(KTxt, "HelloHello");
    HBufC16* buff = HBufC16::NewL(10);
    *buff = KTxt;
    CleanupStack::PushL(buff);
    int size = 30;
    retval = Hbufc16ToWchar(buff,mycharstring,size);

    if (retval ==ESuccess)
    {
    printf("hbufc16towchar Passed");
    }
    else
    {
    assert_failed = true;
    printf("hbufc16towchar failed");
    }      
    CleanupStack::PopAndDestroy(1);
    delete []mycharstring;
    mycharstring = NULL; 
    __UHEAP_MARKEND;
    testResultXml("test_hbufc16towchar_positive");
	return 0;
}
