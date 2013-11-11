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
    TBufC<5> buf _L("hello");
    TPtrC16 src = buf.Des();
    char * des= new char [5]; 
    int size=15;
    int retval =ESuccess;
    retval= Tptrc16ToCharp(src,des,size);

    if(retval ==ESuccess)
    {
    printf(" Test_tptrc16tocharp_minsize PASSED\n");
    }
    else
    {
    assert_failed = true;
    printf(" Test_tptrc16tocharp_minsize FAILURE\n");
    }
    delete[] des;
    des = NULL;
    __UHEAP_MARKEND;
    testResultXml("test_tptrc16tocharp_minsize");
	
	return 0;
}
