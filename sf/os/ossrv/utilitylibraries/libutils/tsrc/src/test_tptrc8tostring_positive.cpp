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
    {
    string s;
    int retval =ESuccess;
    TPtrC8  buf;
    buf.Set((unsigned char *)"Hello", 5);
//    int i=0;
//    while (i<=1000)
//    {
    retval = Tptrc8ToString(buf, s);
    if(retval==ESuccess)
        {
        printf("Conversion successful\n");
        }
//    i++;
//    printf("%d\n",i);
//    }    

    if (!strcmp(s.c_str(), "Hello"))
    {
    printf("tbuf8tostring Passed");
    }
    else
    {
    assert_failed = true;
    printf("tbuf8tostring Failed");
    }  	
    }  	
    __UHEAP_MARKEND;
    testResultXml("test_tptrc8tostring_positive");
	
	return 0;
}
