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
    wstring des ((const wchar_t*)"mytest");	
    __UHEAP_MARK;
    TBufC8<30> buf((unsigned char*)"My wide string");
    TPtrC8 src(buf);
    int retval=ESuccess;
    //int size=14;
    retval= Tptrc8ToWstring(src,des);

    if(retval ==ESuccess && wcscmp(L"My wide string",des.c_str()) == 0 )
    {
    printf("Test tptrc8towstring content check passed\n");
    }
    else
    {
    assert_failed = true;
    printf("Test tptrc8towstring content check FAILURE\n");
    }	
    __UHEAP_MARKEND;
    testResultXml("test_tptrc8towstring_content_check");
	
	return 0;
}
