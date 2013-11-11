/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include "tsysunistd.h"

CTestSysunistd::~CTestSysunistd() 
	{ 
	}  

CTestSysunistd::CTestSysunistd(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestSysunistd::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestSysunistd::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestSysunistd::doTestStepL()
	{
	int err;

   	if(TestStepName() == Kftruncate_file)
   		{
   		INFO_PRINTF1(_L("ftruncate_file():"));
   		err = ftruncate_file();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
	else if(TestStepName() == Kftruncate_dir)
   		{
   	   	INFO_PRINTF1(_L("ftruncate_dir():"));
   	   	err = ftruncate_dir();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	else if(TestStepName() == Kftruncate_num)
   		{
   	   	INFO_PRINTF1(_L("ftruncate_num():"));
   	   	err = ftruncate_num();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	   	
	else if(TestStepName() == Kftruncate_siz)
   		{
   	   	INFO_PRINTF1(_L("ftruncate_siz():"));
   	   	err = ftruncate_siz();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	   	
	else if(TestStepName() == Kfchdir_dir)
   		{
   	   	INFO_PRINTF1(_L("fchdir_dir():"));
   	   	err = fchdir_dir();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	   	
	else if(TestStepName() == Kfchdir_file)
   		{
   	   	INFO_PRINTF1(_L("fchdir_file():"));
   	   	err = fchdir_file();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	   	
	else if(TestStepName() == Kfchdir_num)
   		{
   	   	INFO_PRINTF1(_L("fchdir_num():"));
   	   	err = fchdir_num();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	   	
	else if(TestStepName() == Kfdatasync_file)
   		{
   	   	INFO_PRINTF1(_L("fdatasync_file():"));
   	   	err = fdatasync_file();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	   	
	else if(TestStepName() == Kgetpagesize)
   		{
   	   	INFO_PRINTF1(_L("getpagesize():"));
   	   	err = getpagesizeL();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	   	
	else if(TestStepName() == Kgetpriority)
   		{
   	   	INFO_PRINTF1(_L("getpriority():"));
   	   	err = getpriorityL();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	   	
	else if(TestStepName() == Kgetpriority_other)
   		{
   	   	INFO_PRINTF1(_L("getpriority_other():"));
   	   	err = getpriority_other();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	   	
	else if(TestStepName() == Ksetpriority_get)
   		{
   	   	INFO_PRINTF1(_L("setpriority_get():"));
   	   	err = setpriority_get();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	   	
	else if(TestStepName() == Ksetpriority_high)
   		{
   	   	INFO_PRINTF1(_L("setpriority_high():"));
   	   	err = setpriority_high();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	   	
	else if(TestStepName() == Ksetpriority_other_lower)
   		{
   	   	INFO_PRINTF1(_L("setpriority_other_lower():"));
   	   	err = setpriority_other_lower();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	   	
	else if(TestStepName() == Ksetpriority_other_higher)
   		{
   	   	INFO_PRINTF1(_L("setpriority_other_higher():"));
   	   	err = setpriority_other_higher();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	   	
	else if(TestStepName() == Ksetpriority_other)
   		{
   	   	INFO_PRINTF1(_L("setpriority_other():"));
   	   	err = setpriority_other();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	   	
	else if(TestStepName() == Knice_within_range)
   		{
   	   	INFO_PRINTF1(_L("nice_within_range():"));
   	   	err = nice_within_range();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	   	
	else if(TestStepName() == Knice_more_than_highest)
   		{
   	   	INFO_PRINTF1(_L("nice_more_than_highest():"));
   	   	err = nice_more_than_highest();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	   	
   	else if(TestStepName() == Knice_less_than_lowest)
   		{
   	   	INFO_PRINTF1(_L("nice_less_than_lowest():"));
   	   	err = nice_less_than_lowest();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	   	
	else if(TestStepName() == Kpathconf_valid)
   		{
   	   	INFO_PRINTF1(_L("pathconf_valid():"));
   	   	err = pathconf_valid();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	   	
	else if(TestStepName() == Kpathconf_invalid)
   		{
   	   	INFO_PRINTF1(_L("pathconf_invalid():"));
   	   	err = pathconf_invalid();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	   	
	else if(TestStepName() == Kuname)
   		{
   	   	INFO_PRINTF1(_L("uname():"));
   	   	err = unameL();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	   	
   	else if(TestStepName() == Kuname_null)
   		{
   	   	INFO_PRINTF1(_L("uname_null():"));
   	   	err = uname_null();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   	
   	else if(TestStepName() == Kftruncate_negTest)
   	   		{
   	   	   	INFO_PRINTF1(_L("ftruncate_negTest():"));
   	   	   	err = ftruncate_negTest();
   	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	   	}
   	
   	else if(TestStepName() == Ktruncate_negTest)
   	   	   		{
   	   	   	   	INFO_PRINTF1(_L("truncate_negTest():"));
   	   	   	   	err = truncate_negTest();
   	   	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	   	   	}
	else if(TestStepName() == Kfcntl_largefileTest)
   	   	   		{
   	   	   	   	INFO_PRINTF1(_L("fcntl_largefileTest():"));
   	   	   	   	err = fcntl_largefileTest();
   	   	   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	   	   	}     	
	return TestStepResult(); 

	}

 
