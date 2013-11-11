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
    wchar_t* mycharstring = L"hellohello";
    int retval[10];
    RBuf16 buf;
    buf.CreateL(30);
    retval[1]= WcharToRbuf16(mycharstring,buf);

    wstring str;
    retval[2]= Rbuf16ToWstring(buf,str);

    TBuf16 <15> tbuf;
    retval[3]= WstringToTbuf16(str,tbuf);

    buf.Close();
    wchar_t* myfinalstring= new wchar_t[25];
    int size=15;
    retval[4]= Tbuf16ToWchar(tbuf,myfinalstring,size);

    RBuf8 buf8;
    buf8.CreateL(50);
    retval[5]= WcharToRbuf8(myfinalstring,buf8);

    wstring str1;
    retval[6]= Rbuf8ToWstring(buf8,str1);

    TBuf8 <30> tbuf1;
    retval[7]= WstringToTbuf8(str1,tbuf1);

    buf8.Close();
    int size8=35;
    retval[8]= Tbuf8ToWchar(tbuf1,myfinalstring,size8);
    
    for(int i=1; i<=8; i++)
        {
        if (retval[i]!= 0)
                printf("Conversion failed for retval\n",retval[i]);
        }

    if(!wcscmp(mycharstring,myfinalstring))
    {
    printf("integration_test_scenario19 case passed\n");
    }
    else
    {
    printf("integration_test_scenario19 case failed\n");
    assert_failed = true;
    }
    delete[] myfinalstring;
    }
    __UHEAP_MARKEND;
    testResultXml("integration_test_scenario19");
    return 0;
}
