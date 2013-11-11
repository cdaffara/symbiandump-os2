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
#include <e32base.h>
#include"std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
int main()
{
    string s("");
    __UHEAP_MARK;	
    int retval =ESuccess;
    RBuf16 buff;
    buff.CreateL(30);
    buff.CleanupClosePushL();
    wchar_t *wptr = new wchar_t[15];
    retval = StringToRbuf16(s, buff);

    wchar_t* temp = (wchar_t *) buff.Ptr();
    if (retval ==EStringNoData)
    {
    printf("stringtorbuf16 Passed");
    }
    else
    {
    assert_failed = true;
    printf("stringtorbuf16 Failed");
    }
    delete[] wptr;
    wptr=NULL;
    CleanupStack::PopAndDestroy(1);
    __UHEAP_MARKEND;
    testResultXml("test_stringtorbuf16_negative1");
	return 0;
}
