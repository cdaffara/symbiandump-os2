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
// Name        : tstdio.cpp
// 
//

 

#include "tstdio.h"

CTestStdio::~CTestStdio() 
	{ 
	}  

CTestStdio::CTestStdio(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestStdio::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestStdio::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestStdio::doTestStepL()
	{
	int err;

	
	//captest steps
 if(TestStepName() == Kfopensys1)
   			{
   	   		INFO_PRINTF1(_L("fopensysread():"));
   	   		err = fopensysread();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kfopensys2)
   			{
   	   		INFO_PRINTF1(_L("fopensyswrite():"));
   	   		err = fopensyswrite();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kfopenprivate1)
   			{
   	   		INFO_PRINTF1(_L("fopenprivateread():"));
   	   		err = fopenprivateread();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	
	else if(TestStepName() == Kfopenprivate2)
   			{
   	   		INFO_PRINTF1(_L("fopenprivatewrite():"));
   	   		err = fopenprivatewrite();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Krenamesys)
   			{
   	   		INFO_PRINTF1(_L("renamesys():"));
   	   		err = renamesys();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	
	
	else if(TestStepName() == Krenameprivate)
   			{
   	   		INFO_PRINTF1(_L("renameprivate():"));
   	   		err = renameprivate();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
		else if(TestStepName() == Kremovesys)
   			{
   	   		INFO_PRINTF1(_L("removesys():"));
   	   		err = removesys();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
		else if(TestStepName() == Kremoveprivate)
   			{
   	   		INFO_PRINTF1(_L("removeprivate():"));
   	   		err = removeprivate();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	
	return TestStepResult(); 

	}
 
