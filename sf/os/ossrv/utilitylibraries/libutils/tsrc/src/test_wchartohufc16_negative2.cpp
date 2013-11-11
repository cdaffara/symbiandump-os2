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
#include <e32base.h>
#include "libutils.h"
#include"std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
int main()
{
    __UHEAP_MARK;
    int retval =ESuccess;
    wchar_t* mycharstring = NULL;
    _LIT(KTxtHello, "HelloHello");
    HBufC* hbufc = KTxtHello().AllocL();
    CleanupStack::PushL(hbufc);      
    retval= WcharToHbufc16(mycharstring,hbufc);

    if (retval == EInvalidPointer)
    {
    printf("\nwchartoHbuf16 Passed");
    }
    else
    {
    assert_failed = true;
    printf("\nwchartoHbuf16 Failed");
    }   
    CleanupStack::PopAndDestroy(1);   
    __UHEAP_MARKEND;
    testResultXml("test_wchartohufc16_negative2");
	
	return 0;
}
