// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : tprogname.cpp
// 
//

#include "tprogname.h"
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>


CTestProgName::~CTestProgName() 
	{ 
	}  

CTestProgName::CTestProgName(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestProgName::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestProgName::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestProgName::doTestStepL()
	{
	int err;

   	if(TestStepName() == KGetProgName)
   		{
   		INFO_PRINTF1(_L("GetProgName():"));
   		err = TGetProgName();
   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   		}
   	else
   		{
   		if(TestStepName() == KSetProgName)
   			{
   	   		INFO_PRINTF1(_L("SetProgName():"));
   	   		err = TSetProgName();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	}
	return TestStepResult(); 

	}
 

TInt CTestProgName::TGetProgName()
	{
	INFO_PRINTF1(_L("ProgName Test Getting and Setting"));	
	int retVal = 0;
		
	if (getprogname() == NULL)
		{
		INFO_PRINTF2(_L("getprogname Failed with %d"), errno);
		retVal = KErrGeneral;
		}
	else 
		{
		char progName[] = "tprogname.exe";
		setprogname("tprogname.exe");
		const char* progName1 = getprogname();
		if(strcmp(progName,progName1) != 0) 
			{
			INFO_PRINTF2(_L("getprogname Failed with %d"), errno);
			retVal = KErrGeneral;
			}
		else
			{
			INFO_PRINTF1(_L("getprogname success!"));
			}
		}
	return retVal;
	}
	
TInt CTestProgName::TSetProgName()
	{
	INFO_PRINTF1(_L("ProgName Test Setting"));	
	int retVal = 0;
		
	setprogname("tprogname.exe");
	char progName[] = "tprogname.exe";		
	const char* progName1 = getprogname();
	if(strcmp(progName,progName1) != 0) 
		{
		INFO_PRINTF2(_L("setprogname Failed with %d"), errno);
		retVal = KErrGeneral;
		}
	return retVal;
	}

