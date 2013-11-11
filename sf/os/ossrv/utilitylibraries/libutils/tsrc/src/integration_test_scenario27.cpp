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
    _LIT(myTlitc,"hellohello");
    wchar_t* myWchar = new wchar_t[30];
    int size = 30;
    int retval[10];
    retval[0] = Tlitc16ToWchar(myTlitc,myWchar,size);

    RBuf8 buf;
    buf.CreateL(60);
    retval[1] = WcharToRbuf8(myWchar,buf);

    wstring str;
    retval[2] = Rbuf8ToWstring(buf,str);

    TBuf16 <30> tbuf;
    retval[3]= WstringToTbuf16(str,tbuf);

    wchar_t* myfinalstring= new wchar_t[15];
    int size_new=15;
    retval[4]= Tbuf16ToWchar(tbuf,myfinalstring,size_new);
    
    for(int i=1; i<=4; i++)
        {
        if (retval[i]!= 0)
                printf("Conversion failed for retval\n",retval[i]);
        }

    if(!wcscmp(myWchar,myfinalstring))
    {
    printf("\n\n\nintegration_test_scenario27 success");
    }
    else
    {
    assert_failed = true;
    printf("\n\n\nintegration_test_scenario27 failure");
    }
    delete[] myWchar;
    buf.Close();
    delete[] myfinalstring;       	
    }
    __UHEAP_MARKEND;
    testResultXml("integration_test_scenario27");
    return 0;
}
