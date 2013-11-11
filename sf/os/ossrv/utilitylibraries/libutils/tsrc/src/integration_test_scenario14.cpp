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
    string src1="testing";
    TBuf8<10> des1;
    int ret,retval1,retval2,retval3,retval4,retval5,retval6,size1=20,size2=40;
    retval1= StringToTbuf8(src1,des1);
    printf("retval1 value is %d\n", retval1);
    
    TBuf8<10>src2=des1;
    char *des2= new char[10];
    retval2=Tbuf8ToChar(src2,des2,size1);
    printf("retval2 value is %d\n", retval2);
    
    char *src3=des2;
    TBuf16<20> des3;
    retval3= CharToTbuf16(src3,des3);
    printf("retval3 value is %d\n", retval3);
    
    TBuf16<20> src4=des3;
    string des4("");
    retval4=Tbuf16ToString(src4,des4);
    printf("retval4 value is %d\n", retval4);
    
    string src5=des4;
    TPtr16 des5((unsigned short*)"",30);
    wchar_t *aPtr= new wchar_t[30];
    retval5= StringToTptr16(src5,aPtr,des5);
    printf("retval5 value is %d\n", retval5);
    
    TPtr16 src6=des5;
    char *des6= new char[20];
    retval6= Tptr16ToCharp(src6,des6,size2);
    printf("retval6 value is %d\n", retval6);
    
    ret= strncmp("testing",(const char*)des6,7);
    if(ret == ESuccess)
    {
    printf("integration_test14 PASSED\n");
    }
    else
    {
    printf("integration_tes14 FAILED\n");
    assert_failed = true;
    }	
    delete[] des2;
    delete[] aPtr;
    delete[] des6;	
    }
    __UHEAP_MARKEND;
    testResultXml("integration_test_scenario14");
    return 0;
}
