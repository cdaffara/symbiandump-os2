// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : tcapsignal.cpp
// 
//

 
#include "tcapsignal.h"

CTestcapsignal::~CTestcapsignal() 
	{ 
	}  

CTestcapsignal::CTestcapsignal(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestcapsignal::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestcapsignal::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestcapsignal::doTestStepL()
	{
	int err;

	
	//captest steps
 if(TestStepName() == Kcaptestonkill1)
	 {
	 INFO_PRINTF1(_L("captestonkill1():"));
	 err = captestonkill1();
	 SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	 }
 if(TestStepName() == Kcaptestonkill2)
	 {
	 INFO_PRINTF1(_L("captestonkill2():"));
	 err = captestonkill2();
	 SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
	 }
	return TestStepResult(); 

	}
 
