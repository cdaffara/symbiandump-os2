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
    int retval =ESuccess;
    wchar_t* mywcharstring = L"\xF6þ\xFF";
    TBuf8 <45> f;
    retval = WcharToTbuf8 (mywcharstring, f);

    wchar_t* temp = new wchar_t[50];
    temp[3] =L'\0';
    mbstowcs(temp, (const char*)f.Ptr(),3);
    if (retval ==ESuccess && wcscmp(L"öþÿ",temp)==0)
    {
    printf("wchartotbuf8 unicode Passed\n");
    }
    else
    {
    assert_failed = true;
    printf("wchartotbuf8 unicode Failed\n");
    }     
    delete []temp; 
    __UHEAP_MARKEND;
    testResultXml("test_wchartotbuf8_unicode");
	return 0;
}
