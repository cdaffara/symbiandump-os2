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
    TBufC8<10> src1((unsigned char*)"testing");
    char * des1= new char[10];
    int size1=50,ret,retval1,retval2,retval3,retval4,retval5;
    retval1= Tbufc8ToChar(src1,des1,size1);
    printf("retval1 value is %d\n", retval1);

    char * src2= des1;
    TPtr16 des2 ((unsigned short*)" ",30);
    wchar_t *aPtr = new wchar_t[50];
    retval2= CharpToTptr16(src2,des2);
    printf("retval2 value is %d\n", retval2);

    TPtr16 src3= des2;
    string des3;
    retval3= Tptr16ToString(src3,des3);
    printf("retval3 value is %d\n", retval3);

    string src4= des3;
    RBuf16 des4;
    des4.CreateL(30);
    retval4= StringToRbuf16(src4,des4);
    printf("retval4 value is %d\n", retval4);

    RBuf16 src5;
    src5.Create(des4);		
    char *des5= new char [50];
    retval5 = Rbuf16ToChar(src5,des5,size1);
    printf("retval5 value is %d\n", retval5);

    ret= strncmp("testing",(char*)des5,7);
    if(ret == ESuccess)
    {
    printf("integration_test8 PASSED\n");
    }
    else
    {
    printf("integration_test8 FAILED\n");
    assert_failed=true;
    }
    delete[] des5;
    des4.Close();
    src5.Close();
    delete[] aPtr;
    delete[] des1;
    }
    __UHEAP_MARKEND;
    testResultXml("integration_test_scenario8");
    return 0;
}
