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
    wstring str(L"");
    __UHEAP_MARK;	
    RBuf8 buf;
    buf.CreateL(10);
    int retval =ESuccess;
    retval = WstringToRbuf8(str, buf);

    if (retval ==EStringNoData)
    {
    printf("wstringtorbuf8 Passed");
    }
    else
    {
    assert_failed = true;
    printf("wstringtorbuf8 Failed");
    }	
    buf.Close();
    __UHEAP_MARKEND;
    testResultXml("test_wstringtorbuf8_negative1");
	return 0;
}
