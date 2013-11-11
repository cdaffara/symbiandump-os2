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
// Name        : tfcntl.cpp
// 
//

 

#include "tfcntl.h"

CTestfcntl::~CTestfcntl() 
	{ 
	}  

CTestfcntl::CTestfcntl(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestfcntl::doTestStepPreambleL()
	{
	__UHEAP_MARK;		
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestfcntl::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestfcntl::doTestStepL()
	{
	int err;

	
	//captest steps
 if(TestStepName() == Kopensys)
   			{
   	   		INFO_PRINTF1(_L("opensys():"));
   	   		err = opensys();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kopenprivate)
   			{
   	   		INFO_PRINTF1(_L("openprivate():"));
   	   		err = openprivate();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kcreatsys)
   			{
   	   		INFO_PRINTF1(_L("creatsys():"));
   	   		err = creatsys();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	
	else if(TestStepName() == Kcreatprivate)
   			{
   	   		INFO_PRINTF1(_L("creatprivate():"));
   	   		err = creatprivate();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}

	
	
	
	return TestStepResult(); 

	}
 
