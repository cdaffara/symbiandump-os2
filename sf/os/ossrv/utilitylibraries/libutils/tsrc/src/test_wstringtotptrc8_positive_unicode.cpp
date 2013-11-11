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
    int retval =ESuccess, len = 0;
    wstring W(L"\xAE");
    __UHEAP_MARK;
    TPtrC8 buf;
    char *temp = new char [23];
    retval = WstringToTptrc8(W, temp, buf);
    if(retval == 0)
    printf("Conversion successful\n");

    wchar_t *wChar = new wchar_t[2];
    len = wcslen(W.c_str());
    mbstowcs(wChar , (const char *)buf.Ptr(), len);
    wChar[1]= L'\0';
    if (!wcscmp(wChar , L"\xAE"))
    {
    printf("wstringtotptrc8 Unicode Passed");
    }
    else
    {
    assert_failed = true;
    printf("wstringtotptr8 Unicode Failed");
    }     
    delete[] wChar;
    delete[] temp;
    wChar = NULL;
    temp = NULL;
    __UHEAP_MARKEND;
    testResultXml("test_wstringtotptrc8_positive_unicode");
	return 0;
}
