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
    _LIT8(src1,"integration_test9");
    char *des1= new char[20];
    int size1=20,ret,retval1,retval2,retval3,retval4,retval5,size2=40;
    retval1= Tlitc8ToChar(src1,des1,size1);
    printf("retval1 value is %d\n", retval1);

    char *src2= des1;
    RBuf8 des2;
    des2.CreateL(30);
    retval2= CharToRbuf8(src2,des2);
    printf("retval2 value is %d\n", retval2);

    RBuf8 src3;
    src3.Create(des2);
    string des3;
    retval3= Rbuf8ToString(src3,des3);
    printf("retval3 value is %d\n", retval3);

    string src4=des3;
    TPtrC16 des4((unsigned short*)" ",50);
    wchar_t *aPtr= new wchar_t[20];
    retval4= StringToTptrc16(src4,aPtr,des4);
    printf("retval4 value is %d\n", retval4);

    TPtrC16 src5= des4;
    char *des5= new char[30];
    retval5= Tptrc16ToCharp(src5,des5,size2);
    printf("retval5 value is %d\n", retval5);

    ret= strncmp("integration_test9",(char*)des5,17);
    if(ret == ESuccess)
    {
    printf("integration_test9 PASSED\n");
    }
    else
    {
    printf("integration_test9 FAILED\n");
    assert_failed = true;
    }
    delete[] aPtr;
    delete[] des5;
    src3.Close();
    delete[] des1;
    des2.Close();
    }
    __UHEAP_MARKEND;			
    testResultXml("integration_test_scenario9");
    return 0;
}
