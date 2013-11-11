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
    int retval =ESuccess;
    wchar_t* mywcharstring = L"Hello Widechar String";
    int wchar_length= wcslen(mywcharstring);
    TBuf8 <45> myBuffer;
    retval = WcharToTbuf8 (mywcharstring, myBuffer);

    int buf_len = myBuffer.Length();
    if (retval ==ESuccess && wchar_length == buf_len/2 )
    {
    printf("wchartotbuf8 boundary1 Passed\n");
    }
    else
    {
    assert_failed = true;
    printf("wchartotbuf8 boundary1 Failed\n");
    }      
    testResultXml("test_wchartotbuf8_boundary1");
}
