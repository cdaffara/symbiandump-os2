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
    TPtr8 src1((unsigned char*)"openc_test",15,30);
    char *des1= new char[20];
    int ret,retval1,retval2,retval3,retval4,retval5,retval6,size1=20,size2=80;
    retval1= Tptr8ToChar(src1,des1,size1);
    printf("retval1 value is %d\n", retval1);
    
    char *src2= des1;
    RBuf8 des2;
    des2.CreateL(30);
    retval2= CharToRbuf8(src2,des2);
    printf("retval2 value is %d\n", retval2);

    RBuf8 src3;
    src3.CreateL(des2,10);
    string des3;
    retval3= Rbuf8ToString(src3,des3);
    printf("retval3 value is %d\n", retval3);
    
    string src4=des3;
    wchar_t temp[30];
    TPtr16 des4((unsigned short *)temp,30);
    wchar_t *aPtr= new wchar_t[30];
    retval4= StringToTptr16(src4,aPtr,des4);
    printf("retval4 value is %d\n", retval4);
    
    TPtr16 src5=des4;
    char *des5= new char[50];
    retval5= Tptr16ToCharp(src5,des5,size2);
    printf("retval5 value is %d\n", retval5);
    
    char *src6= des5;
    TPtr8 des6((unsigned char*)" ",20);
    retval6= CharpToTptr8(src6,des6);
    printf("retval6 value is %d\n", retval6);

    ret= strncmp((const char *)src1.Ptr(),(const char *) des6.Ptr(),10);
    if(ret==ESuccess)
    {
    printf("integration_test10 PASSED\n");
    }
    else
    {
    printf("integration_tes10 FAILED\n");
    assert_failed = true;
    }
    delete[] des1;	
    delete[] des5;
    delete[] aPtr;
    src3.Close();
    des2.Close();
    }
    __UHEAP_MARKEND;
    testResultXml("integration_test_scenario10");
    return 0;
}
