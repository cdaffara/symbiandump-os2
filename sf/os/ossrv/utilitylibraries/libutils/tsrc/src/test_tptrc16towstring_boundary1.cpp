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
    wstring des((const wchar_t*)"test"); 
    __UHEAP_MARK;
    TBufC16<30> buf _L("My wide string");
    TPtrC16 src(buf);
    int retval=ESuccess;
    //int size=30;
    retval= Tptrc16ToWstring(src,des);

    int tptrc_len = src.Length();
    int string_len = des.length(); 
    if(retval ==ESuccess && tptrc_len == string_len )
    {
    printf("Test tptrc16towstring boundary1 passed\n");
    }
    else
    {
    assert_failed = true;
    printf("Test tptrc16towstring boundary1 FAILURE\n");
    }	
    __UHEAP_MARKEND;
    testResultXml("test_tptrc16towstring_boundary1");
	
	return 0;
}
