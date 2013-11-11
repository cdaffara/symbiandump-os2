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
    TBuf16 <11> buf;
    int retval =ESuccess;
    retval = WstringToTbuf16(str, buf);

    if (retval == ESuccess)
    {
    printf("wstringtotbuf16_boundry Passed");
    }
    else
    {
    assert_failed = true;
    printf("wstringtotbuf16_boundry Failed");
    }   	
    __UHEAP_MARKEND;
    testResultXml("test_wstringtotbuf16_boundry2");
	return 0;
}
