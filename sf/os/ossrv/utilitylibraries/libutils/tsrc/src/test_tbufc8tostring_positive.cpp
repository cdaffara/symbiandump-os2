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
    __UHEAP_MARK;
    {
    string s;
    TBufC8 <12>  buf((const unsigned char *)"Hello");
//    int i=0;
//    while (i<=1000)
//    {
    retval = Tbufc8ToString(buf, s);
//    i++;
//    printf("%d\n",i);
//    }    

    if (retval ==ESuccess)
    {
    printf("Tbuf8ToString Passed");
    }
    else
    {
    assert_failed = true;
    printf("tbuf8tostring Failed");
    }  	
    }
    __UHEAP_MARKEND;
    testResultXml("test_tbufc8tostring_positive");
	return 0;
}
