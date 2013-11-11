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
#pragma diag_suppress 550
int main()
{
    bool assert_failed = false;
    __UHEAP_MARK;
    {
    string src1="testing";
    char tempChar[20];
    TPtr8 des1((unsigned char*)tempChar,20);
    int ret,size1=30,size2=42;
    int retval[50];
    retval[0] = StringToTptr8(src1, des1);

    TPtr8 src2 = des1;
    char *des2= new char[20];
    retval[1]= Tptr8ToChar(src2,des2,size1);

    char *src3=des2;
    wchar_t *aPtr= new wchar_t[20];
    wchar_t temp[30];
    TPtrC16 des3((unsigned short *)temp,30);
    retval[2]= CharpToTptrc16(src3,aPtr,des3);

    TPtrC16 src4= des3;
    string des4="";
    retval[3]=Tptrc16ToString(src4,des4);

    string src5=des4;
    HBufC16 *des5=HBufC16::NewMaxL(20);
    retval[4]= StringToHbufc16(src5,des5);

    HBufC16 *src6=des5;
    string des6("");
    retval[5]= Hbufc16ToString(src6,des6,size1);

    string src7=des4;
    HBufC8 *des7=HBufC8::NewMaxL(20);
    retval[6]= StringToHbufc8(src7,des7);

    HBufC8 *src8=des7;
    char *des8=new char[20]; 
    retval[7]= Hbufc8ToChar(src8,des8,size1);	

    char *src9=des8;
    HBufC16 *des9=HBufC16::NewMaxL(20);
    retval[8]=CharToHbufc16(src9,des9);

    HBufC16 *src10=des9;
    string des10="";
    retval[9]= Hbufc16ToString(src10,des10,size2);				

    string src11=des10;
    RBuf16 des11;
    des11.CreateL(30);
    retval[10]= StringToRbuf16(src11,des11);

    RBuf16 src12;
    src12.Create(14);
    src12.Copy((unsigned short*)des11.Ptr(), 14);
    des11.Close();
    char *des12=new char[30];
    int size3=50;	
    retval[11]= Rbuf16ToChar(src12,des12,size3);

    char *src13=des12;
    RBuf16 des13;
    des13.CreateL(30);
    retval[12]=CharToRbuf16(src13,des13);

    string des14;
    retval[13]=Rbuf16ToString(des13,des14);

    string src15=des14;
    RBuf8 des15;
    des15.CreateL(30);
    retval[14]= StringToRbuf8(src15,des15);

    RBuf8 src16;
    src16.Create(des15);
    char * des16= new char[61];
    retval[15]=Rbuf8ToChar(src16,des16,size1);

    char *src17=des16;
    RBuf16 des17;
    des17.CreateL(30);
    retval[16]=CharToRbuf16(src17,des17);

    RBuf16 src18;
    src18.Create(des17);
    wchar_t *des18= new wchar_t[70];
    retval[17]=Rbuf16ToWchar(src18,des18,size1);

    wchar_t *src19=des18;
    RBuf8 des19;
    des19.CreateL(50);
    retval[18]=WcharToRbuf8(src19,des19);

    wstring src20;
    retval[19] = Rbuf8ToWstring(des19,src20);

    TBuf16 <100> des20;
    retval[20] = WstringToTbuf16(src20,des20);

    wchar_t* src21= new wchar_t[100];
    int size_new=30;
    retval[21] = Tbuf16ToWchar(des20,src21,size_new);

    RBuf16 des21;
    des21.CreateL(30);
    retval[22]= WcharToRbuf16(src21,des21);

    wstring src22;
    retval[23]= Rbuf16ToWstring(des21,src22);

    TBuf16 <100> des22;
    retval[24]= WstringToTbuf16(src22,des22);

    wchar_t* src23= new wchar_t[100];
    int size=100;
    retval[25]= Tbuf16ToWchar(des22,src23,size);

    RBuf8 des23;
    des23.CreateL(50);
    retval[26]= WcharToRbuf8(src23,des23);

    wstring src24;
    retval[27]= Rbuf8ToWstring(des23,src24);

    des23.Close();
    TBuf8 <100> des24;
    retval[28]= WstringToTbuf8(src24,des24);

    wchar_t* des25 = new wchar_t[150];                
    int size8=150;
    retval[29]= Tbuf8ToWchar(des24,des25,size8);
    
    for(int i=1; i<=29; i++)
        {
        if (retval[i]!= 0)
                printf("Conversion failed for retval\n",retval[i]);
        }

    ret= strncmp((char *)src1.c_str(),(char *)des24.Ptr(),7);
    if(ret == ESuccess)
    {
    printf("integration_test_scenario_29 PASSED\n");
    }
    else
    {
    printf("integration_test_scenario_29 FAILED\n");
    assert_failed=true;
    }
    delete[] des2;
    delete[] des8;
    delete[] des12;
    delete[] aPtr;
    delete[] des16;
    delete[] des18;
    delete[] src21;
    delete[] src23;
    delete[] des25;
    delete des5;
    delete des7;
    delete des9;
    src12.Close();
    des13.Close();
    des15.Close();
    src16.Close();	
    des17.Close();	
    src18.Close();	
    des19.Close();
    des21.Close();
    }
    __UHEAP_MARKEND;
    testResultXml("integration_test_scenario29");
    return 0;
   
}
