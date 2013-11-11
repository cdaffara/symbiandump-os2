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
    string str("Hello World");
    __UHEAP_MARK;
    HBufC8 *buf = HBufC8::NewMaxL(12);
    CleanupStack::PushL(buf);
    int retval =ESuccess;
    retval = StringToHbufc8(str, buf);

    if (retval ==ESuccess)
    {
    printf("stringtohbufc8 Passed");
    }
    else
    {
    assert_failed = true;
    printf("stringtohbufc8 Failed");
    }
    CleanupStack::PopAndDestroy(1);
    __UHEAP_MARKEND;
    testResultXml("test_stringtohbufc8_positive");
	return 0;
}
