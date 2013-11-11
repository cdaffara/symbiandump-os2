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
    char* mycharstring = NULL; 
    TBufC16<30> buf;
    TPtr16 myTptr = buf.Des();
    wchar_t* temp = new wchar_t[30];
    retval = CharpToTptr16(mycharstring, myTptr);

    if (retval == EInvalidPointer)
    {
    printf("charptotptr16 negative2 Passed\n");
    }
    else
    {
    assert_failed = true;
    printf("charptotptr16 negative2 Failed\n");
    }  
    delete[] temp;
    __UHEAP_MARKEND;   
    testResultXml("test_charptotptr16_negative2");
    return 0;
}
