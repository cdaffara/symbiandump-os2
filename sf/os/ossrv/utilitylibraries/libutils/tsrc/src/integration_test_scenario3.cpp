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
    char * src1="test";
    HBufC8 *des1= HBufC8::NewMaxL(20);
    int ret,retval1,retval2,retval3,retval4,retval5,retval6;
    int /*size1=20,*/size2=20,size3=20; 	
    __UHEAP_MARK;
    {
    retval1= CharToHbufc8(src1,des1);
    printf("retval1 value is %d\n", retval1);

    string des2("test");
    HBufC8 *src2= des1;
    retval2= Hbufc8ToString(src2,des2);
    printf("retval2 value is %d\n", retval2);

    string src3=des2;
    TPtr8 des3((unsigned char *)" ",20);
    retval3 = StringToTptr8(src3, des3);
    printf("retval3 value is %d\n", retval3);

    TPtr8 src4 = des3;
    wchar_t *des4= new wchar_t[42];
    retval4= Tptr8ToWcharp(src4,des4,size2);
    printf("retval4 value is %d\n", retval4);

    wchar_t* src5=des4;
    TPtr8 des5((unsigned char*)" ",42);
    char * cPtr= new char[30]; 	
    retval5=WcharpToTptr8(src5,cPtr,des5);
    printf("retval5 value is %d\n", retval5);

    TPtr8 src6 = des5;
    char *des6= new char[42];
    retval6= Tptr8ToChar(src6,des6,size3);
    printf("retval6 value is %d\n", retval6);

    ret= strncmp((char*)src1,(char*)des6,4);
    if(ret == ESuccess)
    {
    printf("integration_test3 passed\n");
    }
    else
    {
    printf("integration_test3 failed\n");
    assert_failed = true;
    }	
    delete[] des4;
    des4=NULL;
    delete[] des6;
    des6=NULL;
    delete[] cPtr;
    delete des1;
    }
    __UHEAP_MARKEND;
    testResultXml("integration_test_scenario3");
    return 0;
}
