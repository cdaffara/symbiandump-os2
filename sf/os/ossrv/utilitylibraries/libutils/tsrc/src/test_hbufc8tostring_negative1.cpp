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
    {
    int retval =ESuccess;
    string str((const char*)"");
    _LIT8(KTxt, "");
    HBufC8* buff = HBufC8::NewL(10);
    *buff = KTxt;
    CleanupStack::PushL(buff);
    retval = Hbufc8ToString(buff,str);

    if (retval ==EDescriptorNoData)
    {
    printf("hbufc8tostring Passed");
    }
    else
    {
    assert_failed = true;
    printf("hbufc8tostring failed");
    }      
    CleanupStack::PopAndDestroy(1);
    }
    __UHEAP_MARKEND;
    testResultXml("test_hbufc8tostring_negative1");
	return 0;
}
