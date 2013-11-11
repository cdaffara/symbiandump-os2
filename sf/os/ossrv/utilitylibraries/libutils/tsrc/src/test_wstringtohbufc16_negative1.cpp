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
    wstring str(L"Hello World");
    __UHEAP_MARK;	
    HBufC16 *buf = HBufC16::NewL(12);
    int retval =ESuccess;
    retval = WstringToHbufc16(str, buf);

    if (retval == EUseNewMaxL)
    {
    printf("wstringtohbufc16 Passed");
    }
    else
    {
    assert_failed = true;
    printf("wstringtohbufc16 Failed");
    }
    delete buf;
    __UHEAP_MARKEND;
    testResultXml("test_wstringtohbufc16_negative1");
	return 0;
}
