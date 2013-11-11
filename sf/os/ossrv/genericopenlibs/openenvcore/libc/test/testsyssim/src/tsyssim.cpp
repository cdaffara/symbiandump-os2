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



#include "tsyssim.h"
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>


CTestSyssim::~CTestSyssim() 
	{ 
	}  

CTestSyssim::CTestSyssim(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestSyssim::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestSyssim::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestSyssim::doTestStepL()
	{
	int err;

	if(TestStepName() == Kgetgrpid)
   		{
   		INFO_PRINTF1(_L("getgrpid():"));
   		err = getgrpid();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
		}
	else if(TestStepName() == Ksetgrpid)
   		{
   	   	INFO_PRINTF1(_L("setgrpid():"));
   	   	err = setgrpid();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
   else if(TestStepName() == Kgeteffgrpid)
   		{
   	   	INFO_PRINTF1(_L("geteffgrpid():"));
   	   	err = geteffgrpid();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Kseteffgrpid)
   		{
   	   	INFO_PRINTF1(_L("seteffgrpid():"));
   	   	err = seteffgrpid();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Ksetregrpid)
   		{
   	   	INFO_PRINTF1(_L("setregrpid():"));
   	   	err = setregrpid();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Kgetusrid)
   		{
   	   	INFO_PRINTF1(_L("getusrid():"));
   	   	err = getusrid();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Ksetusrid)
   		{
   	   	INFO_PRINTF1(_L("setusrid():"));
   	   	err = setusrid();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Kgeteffusrid)
   		{
   	   	INFO_PRINTF1(_L("geteffusrid():"));
   	   	err = geteffusrid();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Kseteffusrid)
   		{
   	   	INFO_PRINTF1(_L("seteffusrid():"));
   	   	err = seteffusrid();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Ksetreusrid)
   		{
   	   	INFO_PRINTF1(_L("setreusrid():"));
   	   	err = setreusrid();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Kgetpgrpid)
   		{
   	   	INFO_PRINTF1(_L("getpgrpid():"));
   	   	err = getpgrpid();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Ksetpgrpid)
   		{
   	   	INFO_PRINTF1(_L("setpgrpid():"));
   	   	err = setpgrpid();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Kgetprocgrp)
   		{
   	   	INFO_PRINTF1(_L("getprocgrp():"));
   	   	err = getprocgrp();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Ksetprocgrp)
   		{
   	   	INFO_PRINTF1(_L("setprocgrp():"));
   	   	err = setprocgrp();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Kgetgrps)
   		{
   	   	INFO_PRINTF1(_L("getgrps():"));
   	   	err = getgrps();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Ksetsessid)
   		{
   	   	INFO_PRINTF1(_L("setsessid():"));
   	   	err = setsessid();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Kgetpprocid)
   		{
   	   	INFO_PRINTF1(_L("getpprocid():"));
   	   	err = getpprocid();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Kfilemask)
   		{
   	   	INFO_PRINTF1(_L("filemask():"));
   	   	err = filemask();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Kchowner)
   		{
   	   	INFO_PRINTF1(_L("chowner():"));
   	   	err = chowner();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Klchowner)
   		{
   	   	INFO_PRINTF1(_L("lchowner():"));
   	   	err = lchowner();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Kinitgrps)
   		{
   	   	INFO_PRINTF1(_L("initgrps():"));
   	   	err = initgrps();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Kschedyield)
   		{
   	   	INFO_PRINTF1(_L("schedyield():"));
   	   	err = schedyield();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Kgetproioritymax)
   		{
   	   	INFO_PRINTF1(_L("getproioritymax():"));
   	   	err = getproioritymax();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	else if(TestStepName() == Kgetproioritymin)
   		{
   	   	INFO_PRINTF1(_L("getproioritymin():"));
   	   	err = getproioritymin();
   	   	SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   	}
	return TestStepResult(); 
	}
 

