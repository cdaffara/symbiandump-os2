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
    TBufC<30> buf;
    TPtrC16 myTptr=buf.Des();
    wchar_t* myWchar =L"hellohello";
    retval[0] = WcharpToTptrc16(myWchar,myTptr);

    wstring myWstring(L"gfgfdfdafggffghghf");
    retval[1] = Tptrc16ToWstring(myTptr,myWstring);

    HBufC16 *hbufc = HBufC16::NewMaxL(20);
    retval[2] = WstringToHbufc16(myWstring,hbufc);

    wstring myFinWstring(L"aghfgafdsgfg");
    retval[3] = Hbufc16ToWstring(hbufc,myFinWstring);

    char* temp = new char[30];
    TBufC8<30> buf8;
    TPtrC8 myTptr8 = buf8.Des();
    retval[4] = WstringToTptrc8(myFinWstring,temp,myTptr8);

    wchar_t* myFinWchar = new wchar_t[21];
    int size = 20;
    retval[5] = Tptrc8ToWcharp(myTptr8,myFinWchar,size);
    
    for(int i=1; i<=5; i++)
        {
        if (retval[i]!= 0)
                printf("Conversion failed for retval\n",retval[i]);
        }

    if(!wcscmp(myWchar,myFinWchar))
    {
    printf("\n\nintegration_test_scenario22 case passed");
    }
    else
    {
    printf("\n\nintegration_test_scenario22 case failed");
    assert_failed = true;
    }
    delete[] temp;
    delete hbufc;
    delete[] myFinWchar;
    }
    __UHEAP_MARKEND;
    testResultXml("integration_test_scenario22");
    return 0;
}
