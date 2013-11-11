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
    TBufC8<30> buf;
    TPtr8 des1= buf.Des();
    int ret, retval1,retval2,retval3,retval4,retval5,retval6,retval7,retval8;    
    retval1 = CharpToTptr8(src1,des1);
    printf("retval1 value is %d\n", retval1);

    __UHEAP_MARK;
    {
    string des2="";
    TPtr8  src2 = des1;
    retval2 = Tptr8ToString(src2, des2);
    printf("retval2 value is %d\n", retval2);
    
    string src3=des2;
    TBuf8 <30> des3;
    retval3 = StringToTbuf8(src3,des3);
    printf("retval3 value is %d\n", retval3);

    TBuf8<20>src4= des3;
    wchar_t * des4= new wchar_t [30]; 
    int size1=30;
    retval4= Tbuf8ToWchar(src4,des4,size1);
    printf("retval4 value is %d\n", retval4);

    wchar_t* src5 = des4;
    TBuf8<50> des5;
    retval5 = WcharToTbuf8(src5,des5);
    printf("retval5 value is %d\n", retval5);

    TBuf8 <50> src6 = des5;
    string des6="";
    retval6 = Tbuf8ToString(src6,des6);
    printf("retval6 value is %d\n", retval6);

    string src7=des6;
    TPtrC8 des7;
    retval7 = StringToTptrc8(src7,des7);
    printf("retval7 value is %d\n", retval7);

    TPtrC8 src8 = des7;
    char *des8= new char[30];
    int size2=30;
    retval8= Tptrc8ToCharp(src8,des8,size2);
    printf("retval8 value is %d\n", retval8);

    ret= strncmp("Hello Char String",(char*)des8,17);	
    if(ret == ESuccess)
    {
    printf("integration_test4 passed\n");
    }
    else
    {
    printf("integration_test4 FAILURE\n");
    assert_failed = true;
    }
    delete[] des4;
    delete[] des8;
    }
    __UHEAP_MARKEND;
    testResultXml("integration_test_scenario4");
    return 0;
}
