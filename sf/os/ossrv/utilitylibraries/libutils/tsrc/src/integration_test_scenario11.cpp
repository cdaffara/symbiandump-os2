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
    TPtrC8 src1((unsigned const char*)"openc",10);
    char *des1=new char[10];
    int ret,retval1,retval2,retval3,retval4,retval5,retval6,size1=20,size2=40;
    retval1=Tptrc8ToCharp(src1,des1,size1);
    printf("retval1 value is %d\n", retval1);
    
    char *src2=des1;
    TBuf16<20> des2;
    retval2=CharToTbuf16(src2,des2);
    printf("retval2 value is %d\n", retval2);
    
    TBuf16<20>src3=des2;
    string des3;
    retval3=Tbuf16ToString(src3,des3);
    printf("retval3 value is %d\n", retval3);
    
    string src4=des3;
    TBuf16<20>des4;
    retval4= StringToTbuf16(src4,des4);
    printf("retval4 value is %d\n", retval4);
    
    TBuf16<40>src5=des4;
    char *des5= new char[40];
    retval5=Tbuf16ToChar(des4,des5,size2);
    printf("retval5 value is %d\n", retval5);
    
    char *src6=des5;
    TPtrC8 des6;
    retval6=CharpToTptrc8(src6,des6);
    printf("retval6 value is %d\n", retval6);
    
    ret= strncmp((char*)src1.Ptr(),(char*) des6.Ptr(),5);
    if(ret == ESuccess)
    {
    printf("integration_test11 PASSED\n");
    }
    else
    {
    printf("integration_tes11 FAILED\n");
    assert_failed = true;
    }
    delete[] des1;
    delete[] des5;
    }
    __UHEAP_MARKEND;
    testResultXml("integration_test_scenario11");
    return 0;
}
