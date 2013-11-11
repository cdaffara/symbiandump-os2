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
	_LIT16(Kname,"tbuf16tochar");
	//int count = 2;
	bool err = false;
	__UHEAP_MARK;
	__UHEAP_FAILNEXT(2);
	while(!err)
	{
	int retval =ESuccess;
	char* myChar = new char[40];
	int size =40;

	retval = Tlitc16ToChar(Kname,myChar,size);
  
  if (retval ==EInsufficientSystemMemory)
    {

    	printf("tlit16tochar_reliability Passed\n");
    	err = true;
    	//Logging to some file can be done here
    }
  else
    {
			assert_failed = true;
			err = true;
    	printf("tlit16tochar_reliability Failed\n");
    	//Logging to some file can be done here
    }      
  delete[] myChar;
	}
	__UHEAP_MARKEND;
	__UHEAP_RESET;

  testResultXml("test_tlit16tochar_reliability");
	return 0;
}
