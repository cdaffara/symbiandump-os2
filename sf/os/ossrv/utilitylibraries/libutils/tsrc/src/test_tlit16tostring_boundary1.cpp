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
    _LIT16(Kname,"");
    TBuf <20>src;
    src.Copy(Kname);
    string str;
    //int size=10;
    int retval =ESuccess;
    retval= Tlit16ToString(src,str);

    if(retval == EDescriptorNoData)
    {
    printf(" Testtlit16tostring PASSED\n");
    }
    else
    {
    assert_failed = true;
    printf(" Testtlit16tostring FAILURE\n");
    }
    }
    __UHEAP_MARKEND;
    testResultXml("test_tlit16tostring_boundary1");
}
