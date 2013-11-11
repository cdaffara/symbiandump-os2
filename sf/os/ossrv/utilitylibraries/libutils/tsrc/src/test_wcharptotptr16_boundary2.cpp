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
    TBufC16<21> buf;
    TPtr16 myTptr = buf.Des();
    retval = WcharpToTptr16(mywcharstring, myTptr);

    int buf_len = myTptr.Length();
    if (retval ==ESuccess &&\
    wchar_length == buf_len &&\
    buf_len == 21 )
    {
    printf("wcharptotptr16 boundary2 Passed\n");
    }
    else
    {
    assert_failed = true;
    printf("wcharptotptr16 boundar2 Failed\n");
    }      
    testResultXml("test_wcharptotptr16_boundary2");
	
	return 0;
}
