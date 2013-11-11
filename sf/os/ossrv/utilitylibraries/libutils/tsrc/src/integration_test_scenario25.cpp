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
    _LIT(KTxt, "HelloHello");
    HBufC* buffer = HBufC::NewL(10);
    *buffer = KTxt;
    wstring myWstring;
    retval[0] = Hbufc16ToWstring(buffer,myWstring);

    TPtrC myTptr16(KTxt);
    retval[2] = WstringToTptrc16(myWstring,myTptr16);

    char* myChar = new char[40];
    int size = 40;
    retval[3] = Tptrc16ToCharp(myTptr16,myChar,size);

    TBufC8<30> buf8;
    TPtr8 myTptr = buf8.Des();  
    retval[4] = CharpToTptr8(myChar,myTptr);

    wchar_t* Wstr_fin = new wchar_t[36];
    int size_char=36;
    retval[5] = Tptr8ToWcharp(myTptr,Wstr_fin,size_char);

    HBufC16* finBuffer = HBufC16::NewMaxL(20);
    retval[6] = WcharToHbufc16(Wstr_fin,finBuffer);
    
    for(int i=1; i<=6; i++)
        {
        if (retval[i]!= 0)
                printf("Conversion failed for retval\n",retval[i]);
        }

    if(!buffer->Compare(finBuffer->Des()))
    {
    printf("\n\nintegration_test_scenario25 case passed");
    }
    else
    {
    printf("\n\nintegration_test_scenario25 case failed");
    assert_failed = true;
    }
    delete buffer;
    delete[] myChar;
    delete[] Wstr_fin;
    delete 	finBuffer;
    }
    __UHEAP_MARKEND;
    testResultXml("integration_test_scenario25");
    return 0;
}
