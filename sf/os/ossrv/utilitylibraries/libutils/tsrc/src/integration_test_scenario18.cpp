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
    RBuf8 buf;
    buf.CreateL(20);
    retval[1]= WcharToRbuf8(mycharstring,buf);

    wstring str;
    retval[2]= Rbuf8ToWstring(buf,str);

    TBuf8 <30> tbuf;
    retval[3]= WstringToTbuf8(str,tbuf);

    wchar_t* myfinalstring= new wchar_t[35];
    int size=35;
    retval[4]= Tbuf8ToWchar(tbuf,myfinalstring,size);
    
    for(int i=1; i<=4; i++)
        {
        if (retval[i]!= 0)
                printf("Conversion failed for retval\n",retval[i]);
        }

    if(!wcscmp(mycharstring,myfinalstring))
    {
    printf("\nintegration_test_scenario18 case passed");
    }
    else
    {
    printf("\nintegration_test_scenario18 case failed");
    assert_failed = true;
    }
    buf.Close();
    delete[] myfinalstring;	
    }
    __UHEAP_MARKEND;
    testResultXml("integration_test_scenario18");
    return 0;
}
