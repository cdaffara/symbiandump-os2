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
    TBuf8<20>src1((unsigned char*)"testing");
    char * des1= new char [20]; 
    int size1=20,ret,retval1,retval2,retval3,retval4,retval5,retval6;
    retval1= Tbuf8ToChar(src1,des1,size1);
    printf("retval1 value is %d\n", retval1);

    char * src2= des1;
    TPtr8 des2((unsigned char*)" ",7);
    retval2=CharpToTptr8(src2,des2);
    printf("retval2 value is %d\n", retval2);

    TPtrC8 src3= des2;
    string des3;
    retval3= Tptrc8ToString(src3,des3);
    printf("retval3 value is %d\n", retval3);

    string src4=des3;
    HBufC16 *des4= HBufC16::NewMaxL(20);
    retval4= StringToHbufc16(src4,des4);
    printf("retval4 value is %d\n", retval4);

    HBufC16 *src5= des4;
    string des5="";
    int size2=40;		
    retval5= Hbufc16ToString(src5,des5,size2);
    printf("retval5 value is %d\n", retval5);

    string src6=des5;
    TBuf8<50> des6;
    retval6= StringToTbuf8(src6,des6);
    printf("retval6 value is %d\n", retval6);

    ret= strncmp((char *)src1.Ptr(),(char *)des6.Ptr(),7);		
    if(ret == ESuccess)
    {
    printf("integration_test7 PASSED\n");
    }
    else
    {
    printf("integration_test7 FAILED\n");
    assert_failed = true;
    }
    delete des4;
    delete[] des1;
    des1= NULL;
    }
    __UHEAP_MARKEND;				
    testResultXml("integration_test_scenario7");
    return 0;
}
