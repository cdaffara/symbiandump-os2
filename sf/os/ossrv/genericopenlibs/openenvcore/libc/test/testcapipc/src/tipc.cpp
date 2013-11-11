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
// Name        : tipc.cpp
// 
//



#include "tipc.h"

CTestipc::~CTestipc() 
	{ 
	}  

CTestipc::CTestipc(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestipc::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestipc::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestipc::doTestStepL()
	{
	int err;

	
	//captest steps
 if(TestStepName() == Kftoksys)
   			{
   	   		INFO_PRINTF1(_L("ftoksys():"));
   	   		err = ftoksys();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kftokprivate)
   			{
   	   		INFO_PRINTF1(_L("ftokprivate():"));
   	   		err = ftokprivate();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}


	
	
	return TestStepResult(); 

	}
 
