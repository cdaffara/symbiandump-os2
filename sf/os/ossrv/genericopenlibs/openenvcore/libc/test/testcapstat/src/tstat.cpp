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
// Name        : tstat.cpp
// 
//



#include "tstat.h"

CTeststat::~CTeststat() 
	{ 
	}  

CTeststat::CTeststat(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTeststat::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTeststat::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTeststat::doTestStepL()
	{
	int err;

	
	//captest steps
 if(TestStepName() == Kmkdirsys)
   			{
   	   		INFO_PRINTF1(_L("mkdirsys():"));
   	   		err = mkdirsys();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kmkdirprivate)
   			{
   	   		INFO_PRINTF1(_L("mkdirprivate():"));
   	   		err = mkdirprivate();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Klstatsys)
   			{
   	   		INFO_PRINTF1(_L("lstatsys():"));
   	   		err = lstatsys();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	
	else if(TestStepName() == Klstatprivate)
   			{
   	   		INFO_PRINTF1(_L("lstatprivate():"));
   	   		err = lstatprivate();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	else if(TestStepName() == Kstatsys)
   			{
   	   		INFO_PRINTF1(_L("statsys():"));
   	   		err = statsys();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	
	
	else if(TestStepName() == Kstatprivate)
   			{
   	   		INFO_PRINTF1(_L("statprivate():"));
   	   		err = statprivate();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
		else if(TestStepName() == Kmkfifosys)
   			{
   	   		INFO_PRINTF1(_L("mkfifosys():"));
   	   		err = mkfifosys();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
		else if(TestStepName() == Kmkfifoprivate)
   			{
   	   		INFO_PRINTF1(_L("mkfifoprivate():"));
   	   		err = mkfifoprivate();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	
		else if(TestStepName() == Kchmodsys)
   			{
   	   		INFO_PRINTF1(_L("chmodsys():"));
   	   		err = chmodsys();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	
		else if(TestStepName() == Kchmodprivate)
   			{
   	   		INFO_PRINTF1(_L("chmodprivate():"));
   	   		err = chmodprivate();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
	 
	
	return TestStepResult(); 

	}
 
