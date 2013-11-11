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
	//int count = 1;
	bool err = false;
	wstring myWstring(L"Hello");
	__UHEAP_MARK;
	__UHEAP_FAILNEXT(1);
	while(!err)	
	{
	int retval =ESuccess;
  TBuf8<20> myTbuf;
	
	retval = WstringToTbuf8(myWstring,myTbuf);
	
  if (retval ==EInsufficientSystemMemory)

    {

    	printf("wstringtotbuf8_reliability Passed\n");
    	err = true;
    	//Logging to some file can be done here
    }

  else

    {
			assert_failed = true;
    	printf("wstringtotbuf8_reliability  Failed\n");
    	//Logging to some file can be done here
    }  
	}
	__UHEAP_MARKEND;
	__UHEAP_RESET;
    testResultXml("test_wstringtotbuf8_reliability");
	return 0;
}
