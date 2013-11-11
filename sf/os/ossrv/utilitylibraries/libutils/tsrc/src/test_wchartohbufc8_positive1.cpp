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
    wchar_t* mywcharstring = L"Hello Widechar String";
    HBufC8 *hbuf = HBufC8::NewMaxL(30);
    CleanupStack::PushL(hbuf);
    int ilen = hbuf->Size();
    int ilen2 = (hbuf->Des()).MaxLength();
    retval = WcharToHbufc8(mywcharstring, hbuf);

    int length = hbuf->Length();
    int size = hbuf->Size();
    if (retval ==ESuccess)
    {
    printf("wchartohbufc8 positive Passed\n");
    }
    else
    {
    assert_failed = true;
    printf("wchartohbufc8 positive Failed\n");
    }  
    CleanupStack::PopAndDestroy(1);
    __UHEAP_MARKEND;
    testResultXml("test_wchartohbufc8_positive1");
	
	return 0;
}
