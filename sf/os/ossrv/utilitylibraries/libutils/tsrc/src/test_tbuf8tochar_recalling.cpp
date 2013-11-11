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
    TBuf8<20>src1((TText8*)"recalling");
    char *des1= new char[20];
    int size=20;
    int retval =ESuccess;
    retval= Tbuf8ToChar(src1,des1,size);

    char *des2= new char[20];
    TBuf8<20>src2(src1);
    retval= Tbuf8ToChar(src2,des2,size);

    if(retval ==ESuccess)
    {
    printf("Test_tbuf8tochar_recalling passed\n");
    }
    else
    {
    assert_failed = true;
    printf("Test_tbuf8tochar_recalling FAILURE\n");
    }
    delete [] des1;
    delete [] des2;
    __UHEAP_MARKEND;
    testResultXml("Test_tbuf8tochar_recalling");
    close_log_file();
	return 0;
}
