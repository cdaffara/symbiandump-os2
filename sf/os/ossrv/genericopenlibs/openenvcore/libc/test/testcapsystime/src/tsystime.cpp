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
// Name        : tsystime.cpp
// 
//


#include "tsystime.h"

CTestsystime::~CTestsystime() 
	{ 
	}  

CTestsystime::CTestsystime(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestsystime::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestsystime::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestsystime::doTestStepL()
	{
	int err;

	
	//captest steps
 if(TestStepName() == Kutimessys)
   			{
   	   		INFO_PRINTF1(_L("utimessys():"));
   	   		err = utimessys();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kutimesprivate)
   			{
   	   		INFO_PRINTF1(_L("utimesprivate():"));
   	   		err = utimesprivate();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}

	
	
	
	return TestStepResult(); 

	}
 
