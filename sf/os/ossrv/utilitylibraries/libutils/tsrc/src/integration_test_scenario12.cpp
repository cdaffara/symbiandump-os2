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
    RBuf8 src1;
    TBuf8<20>buf(_L8("descriptor"));
    src1.Create(buf);
    char *des1= new char[10];
    int ret,retval2,retval3,retval4,retval5,retval6;
    int size1=20;
    retval2=Rbuf8ToChar(src1,des1,size1);		

    char *src2=des1;
    RBuf16 des2;
    des2.CreateL(30);
    retval2=CharToRbuf16(src2,des2);
    printf("retval2 value is %d\n", retval2);

    RBuf16 src3;
    src3.Create(des2);
    string des3;
    retval3=Rbuf16ToString(src3,des3);
    printf("retval3 value is %d\n", retval3);

    string src4=des3;
    TBuf8 <30> des4;
    retval4 = StringToTbuf8(src4,des4);
    printf("retval4 value is %d\n", retval4);
    
    TBuf8<20>src5= des4;
    char * des5= new char [30]; 
    retval5= Tbuf8ToChar(src5,des5,size1);
    printf("retval5 value is %d\n", retval5);
    
    char* src6=des5;
    RBuf8 des6;
    des6.CreateL(30);
    retval6= CharToRbuf8(src6,des6);
    printf("retval6 value is %d\n", retval6);
    
    ret= strncmp((const char*)src1.Ptr(),(const char*)des6.Ptr(),10);
    if(ret == ESuccess)
    {
    printf("integration_test12 PASSED\n");
    }
    else
    {
    printf("integration_tes12 FAILED\n");
    assert_failed = true;
    }			
    delete[] des1;
    delete[] des5;
    src1.Close();
    des2.Close();
    src3.Close();
    des6.Close();
    }
    __UHEAP_MARKEND;
    testResultXml("integration_test_scenario12");
    return 0;
}
