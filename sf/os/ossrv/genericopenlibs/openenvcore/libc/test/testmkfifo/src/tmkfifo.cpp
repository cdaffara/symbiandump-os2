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


#include "tmkfifo.h"
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>


CTestMkfifo::~CTestMkfifo() 
	{ 
	}  

CTestMkfifo::CTestMkfifo(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestMkfifo::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestMkfifo::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	iParamCnt=0;
	return TestStepResult();
	}


TVerdict CTestMkfifo::doTestStepL()
	{
		int err;

   		if(TestStepName() == KIntgTest1)
   			{
   	   		INFO_PRINTF1(_L("IntgTest1():"));
   	   		err = IntgTest1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   	
   	   		if(TestStepName() == KIntgTest2)
   			{
   	   		INFO_PRINTF1(_L("IntgTest2():"));
   	   		err = IntgTest2();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   	
   	   		if(TestStepName() == KIntgTest3)
   			{
   	   		INFO_PRINTF1(_L("IntgTest3():"));
   	   		err = IntgTest3();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   	
   	   		if(TestStepName() == KIntgTest4)
   			{
   	   		INFO_PRINTF1(_L("IntgTest4():"));
   	   		err = IntgTest4();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   	
   	   		if(TestStepName() == KIntgTest5)
   			{
   	   		INFO_PRINTF1(_L("IntgTest5():"));
   	   		err = IntgTest5();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   	
   	   		if(TestStepName() == KIntgTest6)
   			{
   	   		INFO_PRINTF1(_L("IntgTest6():"));
   	   		err = IntgTest6();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   	
   	   		if(TestStepName() == KIntgTest7)
   			{
   	   		INFO_PRINTF1(_L("IntgTest7():"));
   	   		err = IntgTest7();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   	
   	   		if(TestStepName() == KFifoCreateNULL)
   			{
   	   		INFO_PRINTF1(_L("FifoCreateNULL():"));
   	   		err = FifoCreateNULL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   	
   	   		if(TestStepName() == KFifoCreateInSystemDir)
   			{
   	   		INFO_PRINTF1(_L("FifoCreateInSystemDir():"));
   	   		err = FifoCreateInSystemDir();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   	
   	   		if(TestStepName() == KFifoCreateNameTooLong)
   			{
   	   		INFO_PRINTF1(_L("FifoCreateNameTooLong():"));
   	   		err = FifoCreateNameTooLong();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else
   	   	
   	   		if(TestStepName() == KFifoCreateNonExistantDir)
   			{
   	   		INFO_PRINTF1(_L("FifoCreateNonExistantDir():"));
   	   		err = FifoCreateNonExistantDir();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else   	   	
   	   		if(TestStepName() == KTMkfifoReaderWriter)
   			{
   	   		INFO_PRINTF1(_L("TMkfifoReaderWriter():"));
   	   		err = TMkfifoReaderWriter();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}   
   	   	if(TestStepName() == KTMkfifoReaderWriter1)
   			{
   	   		INFO_PRINTF1(_L("TMkfifoReaderWriter1():"));
   	   		err = TMkfifoReaderWriter1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}  	      	 	
   	        else if(TestStepName() == KFifoCreateNULL1)
   			{
   	   		INFO_PRINTF1(_L("FifoCreateNULL1():"));
   	   		err = FifoCreateNULL1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		} 
   	   	else if(TestStepName() == KFifoCreate_invalid)
   			{
   	   		INFO_PRINTF1(_L("FifoCreateNULL1():"));
   	   		err = FifoCreateNULL1();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		} 	
		return TestStepResult(); 

	}
 

