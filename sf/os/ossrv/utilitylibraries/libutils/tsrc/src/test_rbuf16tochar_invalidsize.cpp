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
    RBuf16 myrbuf;
    TBufC16<20> myTBufC (_L16("Descriptor data"));
    myrbuf.Create(myTBufC);
    myrbuf.CleanupClosePushL();
    char *des= new char[20];
    int retval=ESuccess;
    int size=2;
    retval= Rbuf16ToChar(myrbuf,des,size);

    if(retval == EInvalidSize)
    {
    printf("Test_rbuf16tochar_invalidsize passed\n");
    }
    else
    {
    assert_failed = true;
    printf("Test_rbuf16tochar_invalidsize FAILURE\n");
    }
    delete[] des;
    des=NULL;
    CleanupStack::PopAndDestroy(1);
    __UHEAP_MARKEND;
    testResultXml("test_rbuf16tochar_invalidsize");
	return 0;
}
