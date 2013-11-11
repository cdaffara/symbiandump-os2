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
    _LIT16(Kname,"tbuf16tochar");
    char * des= new char [20]; 
    int size=50;
    int retval =ESuccess;
    int i=0;
    while (i<=1000)
    {
  	retval= Tlitc16ToChar(Kname,des,size);
    i++;
    printf("%d\n",i);
    }    

    if(retval ==ESuccess)
    {
    printf(" Test_tlit16tochar_positive PASSED\n");
    }
    else
    {
    assert_failed = true;
    printf(" Test_tlit16tochar_positive FAILURE\n");
    }
    delete[] des;
    des = NULL;
    __UHEAP_MARKEND; 
    testResultXml("test_tlit16tochar_positive");
}
