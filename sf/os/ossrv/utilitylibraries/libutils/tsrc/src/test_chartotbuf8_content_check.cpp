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
    char* mycharstring = "Hello char String";
    int char_length= strlen(mycharstring);
    TBuf8 <45> myBuffer;
    retval = CharToTbuf8 (mycharstring, myBuffer);

    int buf_len = myBuffer.Length();
    if (retval ==ESuccess &&\
    char_length == buf_len &&\
    strncmp("Hello char String",(char*)myBuffer.Ptr() , 17) ==0 )
    {
    printf("chartotbuf8 content check Passed\n");
    }
    else
    {
    assert_failed = true;
    printf("chartotbuf8 content check Failed\n");
    }      
    __UHEAP_MARKEND;
    testResultXml("test_chartotbuf8_content_check");
	return 0;
}
