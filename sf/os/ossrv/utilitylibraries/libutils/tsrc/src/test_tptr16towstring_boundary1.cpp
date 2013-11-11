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
    __UHEAP_MARK;
    {
    wstring des(L"hellohello");
    int len = des.length();
    TPtr16  src((unsigned short *)"My wide string",14, 30);
    int retval=ESuccess;
    retval= Tptr16ToWstring(src,des);

    int tptrc_len = src.Length();
    int string_len = des.length();
    #ifdef __WINS__
    if(retval ==ESuccess && tptrc_len == string_len )
    {
    printf("Test tptr16towstring boundary1 passed\n");
    }
    else
    {	
    assert_failed = true;
    printf("Test tptr16towstring boundary1 FAILURE\n");
    }
    #else 
    if(retval ==ESuccess && (tptrc_len+1)/2 == string_len )
    {
    printf("Test tptr16towstring boundary1 passed\n");
    }
    else
    {	
    assert_failed = true;
    printf("Test tptr16towstring boundary1 FAILURE\n");
    }
    #endif
    }
    __UHEAP_MARKEND;
    testResultXml("test_tptr16towstring_boundary1");
}
