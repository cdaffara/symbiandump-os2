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
    char* src1 = "Hello Char String";
    TPtrC8 des1;
    int ret,retval1,retval2,retval3,retval4,retval5,retval6;
    retval1 = CharpToTptrc8(src1,des1);
    printf("retval1 value is %d\n", retval1);

    __UHEAP_MARK;
    {
    string des2;
    TPtrC8  src2 = des1;
    retval2 = Tptrc8ToString(src2, des2);
    printf("retval2 value is %d\n", retval2);

    string src3="Hello Char String";
    TBuf8 <30> des3;
    retval3 = StringToTbuf8(src3,des3);
    printf("retval3 value is %d\n", retval3);

    TBuf8<20>src4= des3;
    char * des4= new char [30]; 
    int size1=30;
    retval4= Tbuf8ToChar(src4,des4,size1);
    printf("retval4 value is %d\n", retval4);

    char * src5= des4;
    HBufC8 *des5= HBufC8::NewMaxL(30);
    retval5= CharToHbufc8(src5,des5);
    printf("retval5 value is %d\n", retval5);

    HBufC8 *src6= des5;
    char *des6 = new char[30];
    int size2=30;
    retval6= Hbufc8ToChar(src6,des6,size2);
    printf("retval6 value is %d\n", retval6);

    ret= strncmp((char*)src1,(char*)des6,17);
    if(ret == ESuccess)
    {
    printf("integration_test5 PASSED\n");
    }
    else
    {
    printf("integration_test5 FAILED\n");
    assert_failed = true;
    }
    delete[] des4;
    delete[] des6;
    delete des5;
    }
    __UHEAP_MARKEND;
    testResultXml("integration_test_scenario5");
    return 0;
}
