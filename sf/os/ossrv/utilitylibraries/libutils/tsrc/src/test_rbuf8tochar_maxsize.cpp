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
#include<e32base.h>
#include "libutils.h"
#include"std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
int main()
{
    __UHEAP_MARK;
    RBuf8 myrbuf;
    TBufC8<300> myTBufC (_L8("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaahello"));
    myrbuf.Create(myTBufC);
    char *des= new char[300];
    int retval=ESuccess;
    int size=20000;
    retval= Rbuf8ToChar(myrbuf,des,size);

    if(retval ==ESuccess)
    {
    printf("Test_rbuf8tochar_maxsize passed\n");
    }
    else
    {
    assert_failed = true;
    printf("Test_rbuf8tochar_maxsize FAILURE\n");
    }
    delete[] des;
    des=NULL;
    myrbuf.Close();
    __UHEAP_MARKEND;
    testResultXml("Test_rbuf8tochar_maxsize");
	return 0;
}
