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
void main()
{
    __UHEAP_MARK;
    {
    _LIT8(Kname,"tlit8tochar");
    TBuf8 <20>src;
    src.Copy(Kname);
    wstring str;
    int retval =ESuccess;
    int i=0;
    while (i<=1000)
    {
  	retval= Tlitc8ToWstring(src,str);
    i++;
    printf("%d\n",i);
    }    

    if(retval == ESuccess)
    {
    printf(" Testtlit8towstring PASSED\n");
    }
    else
    {
    assert_failed = true;
    printf(" Testtlit8towstring FAILURE\n");
    }
    }
    __UHEAP_MARKEND;
    testResultXml("Test_tlit8towstring_positive");
}
