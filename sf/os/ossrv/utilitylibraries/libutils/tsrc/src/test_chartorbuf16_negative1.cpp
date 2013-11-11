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

#include <e32base.h>
#include <stdio.h>
#include<e32std.h>
#include "libutils.h"
#include"std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
int main()
{
    __UHEAP_MARK;
    int retval =ESuccess;
    char* mycharstring = NULL;
    RBuf16 myRBuf;
    myRBuf.CreateL(30);
    myRBuf.CleanupClosePushL(); 
    retval = CharToRbuf16(mycharstring, myRBuf);

    if (retval == EInvalidPointer)
    {
    printf("chartorbuf16 negative1 Passed\n");
    }
    else
    {
    assert_failed = true;
    printf("chartorbuf16 negative1 Failed\n");
    }      
    CleanupStack::PopAndDestroy(1);
    __UHEAP_MARKEND;
    testResultXml("test_chartorbuf16_negative1");
	return 0;
}
