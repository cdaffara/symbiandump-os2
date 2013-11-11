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
    string src1("@#$%^^&");
    TPtr8 des1((unsigned char *)"",30);
    int retval1, retval2,retval3,retval4,retval5,retval6;
    int ret;
    retval1 = StringToTptr8(src1, des1);
    printf("retval1 value is %d\n", retval1);

    TPtr8 src2 = des1;
    char *des2= new char[30];
    int size1=30;
    retval2= Tptr8ToChar(src2,des2,size1);
    printf("retval2 value is %d\n", retval2);

    char* src3 = des2;
    TBuf8 <30> des3;
    retval3 = CharToTbuf8 (src3, des3);
    printf("retval3 value is %d\n", retval3);
    
    TBuf8<30>src4= des3;
    wchar_t * des4= new wchar_t [30]; 
    int size2=30;
    retval4= Tbuf8ToWchar(src4,des4,size2);
    printf("retval4 value is %d\n", retval4);

    wchar_t* src5 = des4;
    TPtr8 des5((unsigned char *)" ",50);
    char * cPtr= new char[50];
    retval5 = WcharpToTptr8(src5,cPtr,des5);
    printf("retval5 value is %d\n", retval5);

    string des6("");
    TPtr8  src6 = des5;
    retval6 = Tptr8ToString(src6, des6);
    printf("retval6 value is %d\n", retval6);
    
    ret= strncmp((char *)src1.c_str(),(char *)des6.c_str(),11);
    if(ret == ESuccess)
    {
    printf("integration_test2 passed\n");
    }
    else
    {
    printf("integration_test2 failed\n");
    assert_failed = true;
    }	
    delete[] des2;
    delete[] des4;
    delete[] cPtr;
    }
    __UHEAP_MARKEND;
    testResultXml("integration_test_scenario2");
    return 0;
}
