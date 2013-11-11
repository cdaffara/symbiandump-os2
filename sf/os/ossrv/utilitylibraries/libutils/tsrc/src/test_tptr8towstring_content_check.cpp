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
    wstring des((const wchar_t*)"hellohello") ;
    __UHEAP_MARK; 
    TPtr8  src((unsigned char *)"My wide string",14, 30);
    int retval=ESuccess;
    //int size=30;
    retval= Tptr8ToWstring(src,des);

    if(retval ==ESuccess && wcscmp(L"My wide string",des.c_str()) == 0 )
    {
    printf("Test tptr8towstring content check passed\n");
    }
    else
    {
    assert_failed = true;
    printf("Test tptr8towstring content check FAILURE\n");
    }	
    __UHEAP_MARKEND;
    testResultXml("test_tptr8towstring_content_check");
	
	return 0;
}
