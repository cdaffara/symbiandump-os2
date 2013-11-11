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
    wstring des((const wchar_t*)"hello") ;
    TPtr16  src((unsigned short *)"",1, 30);
    int retval=ESuccess;
    //int size=30;
//    int i=0;
//    while (i<=1000)
//    {
    retval= Tptr16ToWstring(src,des);
//    i++;
//    printf("%d\n",i);
//    }    

    if(retval ==ESuccess)
    {
    printf("Test tptr16towstring positive passed\n");
    }
    else
    {
    assert_failed = true;
    printf("Test tptr16towstring positive FAILURE\n");
    }		
    }		
    __UHEAP_MARKEND;
    testResultXml("test_tptr16towstring_positive1");
	
	return 0;
}
