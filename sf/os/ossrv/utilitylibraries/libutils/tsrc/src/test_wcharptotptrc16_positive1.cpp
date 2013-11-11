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
    TPtrC myTptrc;
    retval = WcharpToTptrc16(mywcharstring,myTptrc);

    if (retval ==ESuccess)
    {
    printf("wcharptotptrc16 positive Passed\n");
    }
    else
    {
    assert_failed = true;
    printf("wcharptotptrc16 positive Failed\n");
    }      
    testResultXml("test_wcharptotptrc16_positive1");
	
	return 0;
}
