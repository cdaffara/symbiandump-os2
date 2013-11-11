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
// Name        : tutime.cpp
// 
//


#include "tutime.h"

CTestutime::~CTestutime() 
	{ 
	}  

CTestutime::CTestutime(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestutime::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestutime::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestutime::doTestStepL()
	{
	int err;

	
	//captest steps
 if(TestStepName() == Kutimesys)
   			{
   	   		INFO_PRINTF1(_L("utimesys():"));
   	   		err = utimesys();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kutimeprivate)
   			{
   	   		INFO_PRINTF1(_L("utimeprivate():"));
   	   		err = utimeprivate();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}

	
	
	return TestStepResult(); 

	}
 
