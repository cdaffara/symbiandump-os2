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



#include "tmmap.h"


CTestMmap::~CTestMmap() 
	{ 
	}  

CTestMmap::CTestMmap(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestMmap::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestMmap::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestMmap::doTestStepL()
	{
		int err;

   		if(TestStepName() == Kmemmap)
   			{
   	   		INFO_PRINTF1(_L("memmap():"));
   	   		err = memmap();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kmemmap1)
   			{
   	   		INFO_PRINTF1(_L("memmap1():"));
   	   		err = memmap1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kmemmap2)
   			{
   	   		INFO_PRINTF1(_L("memmap2():"));
   	   		err = memmap2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kmemmap3)
   			{
   	   		INFO_PRINTF1(_L("memmap3():"));
   	   		err = memmap3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kmemmap4)
   			{
   	   		INFO_PRINTF1(_L("memmap4():"));
   	   		err = memmap4();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   		if(TestStepName() == Kmemmap5)
   			{
   	   		INFO_PRINTF1(_L("memmap5():"));
   	   		err = memmap5();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
		else
   	   		if(TestStepName() == Kmemmap6)
   			{
   	   		INFO_PRINTF1(_L("memmap6():"));
   	   		err = memmap5();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}

		return TestStepResult(); 

	}
 


	
