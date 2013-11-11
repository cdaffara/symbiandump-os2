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
    _LIT8(Kname,"testing");
    HBufC8 *src1=HBufC8::NewL(20);
    *src1=Kname;
    char *des1 = new char[30];
    int size1=30,retval1,retval2,retval3,retval4,ret;
    retval1= Hbufc8ToChar(src1,des1,size1);
    printf("retval1 value is %d\n", retval1);

    char *src2=des1;
    TPtrC16 des2((unsigned short *)"",20);
    wchar_t *aPtr=new wchar_t[20];	    				
    retval2=CharpToTptrc16(src2,aPtr,des2);
    printf("retval2 value is %d\n", retval2);
      
    TPtrC16 src3= des2;
    char *des3= new char[20];
    retval3= Tptrc16ToCharp(src3,des3,size1);      				
    printf("retval3 value is %d\n", retval3);
    
    char *src4=des3;
    HBufC16 *des4=HBufC16::NewMaxL(20);
    retval4= CharToHbufc16(src4,des4);
    printf("retval4 value is %d\n", retval4);
    
    char *temp = new char [des4->Length()];
    wcstombs(temp, (const wchar_t *)des4->Ptr() , 7);
    temp[7] = '\0';
    ret=strcmp(temp, "testing");				
    if(ret == 0)
    {
    printf("integration_test13 PASSED\n");
    }
    else
    {
    printf("integration_tes13 FAILED\n");
    assert_failed = true;
    }			
    delete[] des1;
    delete[] aPtr;
    delete[] temp;
    delete[] des3;
    delete src1;
    delete des4;
    }
    __UHEAP_MARKEND;
    testResultXml("integration_test_scenario13");
    return 0;
}
