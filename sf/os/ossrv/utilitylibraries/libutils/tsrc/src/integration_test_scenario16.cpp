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
    wchar_t* mycharstring = L"hellohello";
    RBuf16 buf;
    buf.CreateL(30);
    retval[0]= WcharToRbuf16(mycharstring,buf);
    printf("The first value in retval array is %d\n", retval[0]);

    wstring str;
    retval[1]= Rbuf16ToWstring(buf,str);
    printf("The second value in retval array is %d\n", retval[1]);

    TBuf16 <15> tbuf;
    retval[2]= WstringToTbuf16(str,tbuf);
    printf("The third value in retval array is %d\n", retval[2]);

    wchar_t* myfinalstring= new wchar_t[15];
    int size=15;
    retval[3]= Tbuf16ToWchar(tbuf,myfinalstring,size);
    printf("The fourth value in retval array is %d\n", retval[3]);

    if(!wcscmp(mycharstring,myfinalstring))
    {
    printf("\nintegration_test_scenario16 case passed");
    }
    else
    {
    printf("\nintegration_test_scenario16 case failed");
    assert_failed = true;
    }
    buf.Close();
    delete[] myfinalstring;	
    }
    __UHEAP_MARKEND;
    testResultXml("integration_test_scenario16");
    return 0;
}
