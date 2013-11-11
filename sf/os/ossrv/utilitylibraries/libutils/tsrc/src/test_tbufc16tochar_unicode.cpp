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
    _LIT16(Kname,"\xAE");
    TBufC16<20>src(Kname);
    char * des= new char [30]; 
    int size=50;
    int retval =ESuccess;
    retval= Tbufc16ToChar(src,des,size);

    wchar_t *temp = new wchar_t[2];
    mbstowcs(temp, des, 1);
    temp[1] = L'\0';
    if(retval ==ESuccess)
    {
    printf(" Test_tbufc16tochar_unicode PASSED\n");
    }
    else
    {
    assert_failed = true;
    printf(" Test_tbufc16tochar_unicode FAILURE\n");
    }
    delete[] des;
    delete[] temp;
    temp = NULL;
    des = NULL;
    __UHEAP_MARKEND;	
    testResultXml("test_tbufc16tochar_unicode");
	return 0;
}
