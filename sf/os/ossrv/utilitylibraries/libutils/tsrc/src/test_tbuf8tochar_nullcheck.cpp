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
    TBuf8<20>src ((TText8*)"#$%^&*()");
    char *des= NULL;
    int size=20;
    int retval =ESuccess;
    retval= Tbuf8ToChar(src,des,size);

    if(retval == EInvalidPointer)
    {
    printf("Test_tbuf8tochar_nullcheck passed\n");
    }
    else
    {
    assert_failed = true;
    printf("Test_tbuf8tochar_nullcheck FAILURE\n");
    }
    __UHEAP_MARKEND;
    testResultXml("Test_tbuf8tochar_nullcheck");
	return 0;
}
