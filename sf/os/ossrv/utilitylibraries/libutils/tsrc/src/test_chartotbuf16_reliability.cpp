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
	__UHEAP_MARK;
	__UHEAP_FAILNEXT(1);
	while(!err)
	{
	int retval =ESuccess;
	char* mycharstring = "Hello Char String";
	TBuf16 <25> myTbuf;
	
	retval = CharToTbuf16(mycharstring, myTbuf);
	
  if (retval ==EInsufficientSystemMemory)

    {

    	printf("chartotbuf16_reliability Passed\n");
    	err = true;
    	//Logging to some file can be done here
    }

  else

    {
			assert_failed = true;
    	printf("chartotbuf16_reliability Failed\n");
    	//Logging to some file can be done here
    }      
	}
	__UHEAP_MARKEND;
	__UHEAP_RESET;
    testResultXml("test_chartotbuf16_reliability");
	return 0;
}
