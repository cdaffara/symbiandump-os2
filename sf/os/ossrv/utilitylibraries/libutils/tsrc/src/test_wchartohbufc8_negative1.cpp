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
    wchar_t* mycharstring = L"hihihihi";
    HBufC8* hbufc = HBufC8::NewMaxL(1);
    CleanupStack::PushL(hbufc);
    retval= WcharToHbufc8(mycharstring,hbufc);

    if (retval == EInsufficientMemory)
    {
    printf("\nwchartohbuf8 Passed");
    }
    else
    {
    assert_failed = true;
    printf("\nwchartohbuf8 Failed");
    }      
    CleanupStack::PopAndDestroy(1);
    __UHEAP_MARKEND;
    testResultXml("test_wchartohbufc8_negative1");
	
	return 0;
}
