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
    TBufC<20> buf _L("testing");
    TPtrC16 src = buf.Des();
    char * des= new char [20]; 
    int size=50;
    int retval =ESuccess;
//    int i=0;
//    while (i<=1000)
//    {
    retval= Tptrc16ToCharp(src,des,size);
//    i++;
//    printf("%d\n",i);
//    }    

    if(retval ==ESuccess)
    {
    printf(" Test_tptrc16tocharp_positive PASSED\n");
    }
    else
    {
    printf(" Test_tptrc16tocharp_positive FAILURE\n");
    }
    delete[] des;
    des = NULL;
    __UHEAP_MARKEND;
    testResultXml("test_tptrc16tocharp_positive");
	
	return 0;
	
}
