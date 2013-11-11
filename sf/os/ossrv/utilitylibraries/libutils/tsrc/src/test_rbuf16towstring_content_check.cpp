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
    wstring des;
    __UHEAP_MARK;
    int retval =ESuccess;
    RBuf16  buf;
    buf.Create(10);
    buf.Copy((TUint16 *)"Hello", 5);
    buf.CleanupClosePushL();
    //int size =15;
    retval = Rbuf16ToWstring(buf, des);

    if (retval ==ESuccess && strcmp("Hello" ,(const char*)des.c_str()) == 0)
    {
    printf("test rbuf16towstring content check Passed\n");
    }
    else
    {
    assert_failed = true;
    printf("test rbuf16towstring content check Failed\n");
    }
    CleanupStack::PopAndDestroy(1);
    __UHEAP_MARKEND;
    testResultXml("test_rbuf16towstring_content_check");
	return 0;
}
