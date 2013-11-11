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
    _LIT8(Kname,"positive");
    wchar_t * des= new wchar_t [20]; 
    int retval =ESuccess;
    int size = 10;
    retval= Tlitc8ToWchar(Kname, des, size);

    if(retval ==ESuccess)
    {
    printf("Test_tlit8towchar_positive passed\n");
    }
    else
    {
    assert_failed = true;
    printf("Test_tlit8towchar_positive FAILURE\n");
    }
    delete[] des;
    des = NULL;	
    __UHEAP_MARKEND;
    testResultXml("test_tlit8towchar_positive");
}
