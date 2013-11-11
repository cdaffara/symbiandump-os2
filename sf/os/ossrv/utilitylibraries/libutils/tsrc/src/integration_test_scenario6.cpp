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
    char* src1="integration_test6";
    RBuf8 des1;
    int ret,retval1,retval2,retval3,retval4,retval5,retval6;
    int size1=39;
    des1.CreateL(30);
    retval1= CharToRbuf8(src1,des1);
    printf("retval1 value is %d\n", retval1);

    RBuf8 src2;
    src2.Create(des1);
    string des2;
    retval2= Rbuf8ToString(src2,des2);
    printf("retval2 value is %d\n", retval2);

    string src3=des2;
    HBufC8 *des3= HBufC8::NewMaxL(20);
    retval3= StringToHbufc8(src3,des3);
    printf("retval3 value is %d\n", retval3);

    HBufC8 *src4=des3;
    string des4;
    retval4= Hbufc8ToString(src4,des4);
    printf("retval4 value is %d\n", retval4);

    string src5=des4;
    TBuf16<20> des5;
    retval5= StringToTbuf16(src5,des5);
    printf("retval5 value is %d\n", retval5);

    TBuf16<20>	src6=des5;
    char *des6= new char[39];
    retval6= Tbuf16ToChar(src6,des6,size1);		
    printf("retval6 value is %d\n", retval6);
    
    ret= strncmp((char*)src1,(char*)des6,17);
    if(ret == ESuccess)
    {
    printf("integration_test6 PASSED\n");
    }
    else
    {
    printf("integration_test6 FAILED\n");
    assert_failed = true;
    }
    delete des3;
    delete[]des6;
    des1.Close();
    src2.Close();	
    }
    __UHEAP_MARKEND;
    testResultXml("integration_test_scenario6");
    return 0;
}
