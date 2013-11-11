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
	//int count = 2;
	bool err = false;
	__UHEAP_MARK;
	__UHEAP_FAILNEXT(2);
	while(!err)
	{
	int retval =ESuccess;
	char* mycharstring = "Hello";
	_LIT16(KTxt,"dcdfdddcdcdc");
  HBufC16* buff = HBufC16::NewL(40);
  *buff = KTxt;
  CleanupStack::PushL(buff);
	
	retval = CharToHbufc16(mycharstring, buff);
	
  if (retval ==EInsufficientSystemMemory)

    {

    	printf("chartohbufc16_reliability Passed\n");
    	err = true;
    	//Logging to some file can be done here
    }

  else

    {
			assert_failed = true;
			err = true;
    	printf("chartohbufc16_reliability  Failed\n");
    	//Logging to some file can be done here
    }  
	}
	CleanupStack::PopAndDestroy();

	__UHEAP_MARKEND;
	__UHEAP_RESET;
    testResultXml("test_chartohbufc16_reliability");
	return 0;
}
