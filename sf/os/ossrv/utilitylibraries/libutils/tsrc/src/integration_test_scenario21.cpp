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
    wstring mystring(L"hellohello");
    TBufC16<30> buf;
    TPtr16 myTptr = buf.Des();
    retval[1] = WstringToTptr16(mystring,myTptr);

    wchar_t* myWchar = new wchar_t[30];
    int size=30;
    retval[2] = Tptr16ToWcharp(myTptr,myWchar,size);

    HBufC16 *hbufc = HBufC16::NewMaxL(30);
    retval[3] = WcharToHbufc16(myWchar,hbufc);

    wstring myFinalWstr(L"kjhskjfhsakhssjhfdkjshfdks") ;
    retval[4] = Hbufc16ToWstring(hbufc,myFinalWstr);
    
    for(int i=1; i<=4; i++)
        {
        if (retval[i]!= 0)
                printf("Conversion failed for retval\n",retval[i]);
        }

    if(!wcscmp((wchar_t *)mystring.c_str(),(wchar_t *)myFinalWstr.c_str()))
    {
    printf("\nintegration_test_scenario21 case passed");
    }
    else
    {
    printf("\n\nintegration_test_scenario21 case failed");
    assert_failed = true;
    }
    delete[] myWchar;
    delete hbufc;
    }
    __UHEAP_MARKEND;
    testResultXml("integration_test_scenario21");
    return 0;
}
