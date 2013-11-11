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
    int retval[10];
    wchar_t* mycharstring = L"hello";
    RBuf16 buf;
    buf.CreateL(30);
    retval[0]= WcharToRbuf16(mycharstring,buf);

    wstring str;
    retval[1]= Rbuf16ToWstring(buf,str);

    TPtr8 tptr8((unsigned char*)"",10,20);
    char* temp_c = new char[20];
    retval[2]= WstringToTptr8(str,temp_c,tptr8);

    char* mychar = new char[30];
    int size=30;
    retval[3]= Tptr8ToChar(tptr8,mychar,size);

    HBufC16 *hbufc = HBufC16::NewMaxL(20);
    retval[4]= CharToHbufc16(mychar,hbufc);

    string mystring;
    int size_hbufc=55;
    retval[5]= Hbufc16ToString(hbufc,mystring,size_hbufc);

    wchar_t* temp = new wchar_t[50];
    TPtrC16 tptrc;
    retval[6]= StringToTptrc16(mystring,temp,tptrc);
    
    for(int i=1; i<=6; i++)
        {
        if (retval[i]!= 0)
                printf("Conversion failed for retval\n",retval[i]);
        }

    wchar_t* myfinalstring = new wchar_t[50];
    int size_fin=55;
    retval[7]= Tptrc16ToWcharp(tptrc,myfinalstring,size_fin);

    if(!wcsncmp(mycharstring,myfinalstring,5))
    {
    printf("\nintegration_test_scenario20 case passed");
    }
    else
    {
    printf("\nintegration_test_scenario20 case failed");
    assert_failed = true;
    }
    buf.Close();
    delete[] temp_c;
    delete[] mychar;
    delete hbufc;
    delete[] temp;
    delete[] myfinalstring;
    }
    __UHEAP_MARKEND;
    testResultXml("integration_test_scenario20");
    return 0;
}
