// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name        : tcomport.cpp
// 
//


#include "tcomport.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <wchar.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <spawn.h>
#include <sched.h>
#include <sys/wait.h>


CTestcomport::~CTestcomport() 
	{ 
	}  

CTestcomport::CTestcomport(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestcomport::doTestStepL()
	{
	__UHEAP_MARK;
	int err;
   	if(TestStepName() == Ktest_printf_COM)
   		{
   		INFO_PRINTF1(_L("test_printf_COM():"));
   	   	err = test_printf_COM();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}   		
   	else if(TestStepName() == Ktest_scanf_COM)
   		{
   		INFO_PRINTF1(_L("test_scanf_COM():"));
   	   	err = test_scanf_COM();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	} 

	__UHEAP_MARKEND;  	   	   	   	   	 
	return TestStepResult(); 
	}
	
// -----------------------------------------------------------------------------
// Tests to improve coverage
// -----------------------------------------------------------------------------
//                  
TVerdict CTestcomport::test_printf_COM()
    {    
    SetTestStepResult(EPass);
    TInt ret;
    ret = printf("Hello World\n");
    if (ret != strlen("Hello World\n"))	
    	{
    	INFO_PRINTF1(_L("Error in printing..."));
    	SetTestStepResult(EFail);    	    	
		return TestStepResult();
    	}
	return TestStepResult();    	   
    }  
    
// -----------------------------------------------------------------------------
// Tests to improve coverage - manual tests
// -----------------------------------------------------------------------------
TVerdict CTestcomport::test_scanf_COM()
	{
    SetTestStepResult(EPass);
    TInt ret;
    char tmp[50];
    ret = scanf("%s",tmp);	
    if (ret <= 0)	
    	{
    	INFO_PRINTF1(_L("Error in scanning..."));
    	SetTestStepResult(EFail);    	    	
		return TestStepResult();
    	}
    printf("Read string is %s\n",tmp);
   	return TestStepResult();  		
	}


