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
    string str1("Integration test");
    TPtrC8 buf1;
    int retval1 = 0, retval2 = 0,retval3 = 0,retval4,retval5,retval6;
    int ret =0 ;
    retval1 = StringToTptrc8(str1, buf1);
    printf("retval1 value is %d\n", retval1);

    TPtrC8 src1 = buf1;
    char *des1= new char[20];
    int size1=20;
    retval2= Tptrc8ToCharp(src1,des1,size1);
    printf("retval2 value is %d\n", retval2);
    
    char* src2 = des1;
    TBuf8 <20> des2;
    retval3 = CharToTbuf8 (src2, des2);
    printf("retval3 value is %d\n", retval3);
    
    TBuf8<20>src3= des2;
    wchar_t * des3= new wchar_t [20]; 
    int size2=20;
    retval4= Tbuf8ToWchar(src3,des3,size2);
    printf("retval4 value is %d\n", retval4);
    
    wchar_t* src4 = des3;
    TPtrC8 des4;
    char *size = new char[2*wcslen(src4)];
    retval5 = WcharpToTptrc8(src4, size ,des4);
    printf("retval5 value is %d\n", retval5);
    
    string str2;
    TPtrC8  src5 = des4;
    retval6 = Tptrc8ToString(src5, str2);
    printf("retval6 value is %d\n", retval6);
    
    ret= strncmp((char *)str1.c_str(),(char *)str2.c_str(),11);
    if(ret == ESuccess)
    {
    printf("integration_test1 passed\n");
    }
    else
    {
    printf("integration_test1 failed\n");
    assert_failed = true;
    }
    delete[] des1;
    delete[] des3;
    delete[] size;		
    }
    __UHEAP_MARKEND;
    testResultXml("integration_test_scenario1");
    return 0;
}
