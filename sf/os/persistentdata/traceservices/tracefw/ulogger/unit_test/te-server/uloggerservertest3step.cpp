// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file UloggerServerTest3Step.cpp
 @internalTechnology
*/


#include "uloggerservertest3step.h"
#include "te_uloggerservertestsuitedefs.h"
#include "uloggerwatcher.h"
#include "pluginallocator.h"


CUloggerServerTest3Step::~CUloggerServerTest3Step()
/**
 * Destructor
 */
	{
	}

CUloggerServerTest3Step::CUloggerServerTest3Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KUloggerServerTest3Step);
	}

TVerdict CUloggerServerTest3Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("****This is unit test for CPluginAllocator class****"));	
	
	
	iScheduler = new (ELeave) CActiveScheduler();
	CActiveScheduler::Install(iScheduler);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CUloggerServerTest3Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  iErrors = 0;
	  if (TestStepResult()==EPass)
	  {
	  
		iErrors += this->Test1L();//CPluginAllocator::NewL, GetOutputPlugin, GetControlPlugin
		iErrors += this->Test1_2L();//CPluginAllocator::NewL.... - condition 2
		iErrors += this->Test2L();//CPluginAllocator::NewLC
		iErrors += this->Test1_3L(); //CPluginAllocator::NewL - condition 3
		iErrors += this->Test1_4L(); //CPluginAllocator::NewL - condition 4

		//display results
		TBuf<128> res;
		res.AppendFormat(_L("%d errors"), iErrors);
		INFO_PRINTF1(_L("****Results****"));
		INFO_PRINTF1(res);
		if(iErrors == 0)  
			SetTestStepResult(EPass);
			else
				SetTestStepResult(EFail);
	  }
	  
	  return TestStepResult();
	}



TVerdict CUloggerServerTest3Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	delete iScheduler;
	iScheduler = NULL;
	return TestStepResult();
	}


TInt CUloggerServerTest3Step::Test1L()
	{
	TInt errors = 0;
	
	INFO_PRINTF1(_L("Testing - CPluginAllocator::NewL method - condition 1"));
	CPluginAllocator *obj = CPluginAllocator::NewL(KFileTestPlugin(), KNullTestPlugin());
	if(obj == NULL)
	  {
	  INFO_PRINTF1(_L("error"));
	  errors++;
	  }
	  else
		{
		MOutputPlugin* output = obj->GetOutputPlugin(); //should exists
		MInputPlugin* input = obj->GetInputPlugin(); //should not exists
		if(!output || input)
			errors++;
		
		//cleanup
		delete obj;
		obj = NULL;
		}
		
	return errors;
	}

TInt CUloggerServerTest3Step::Test1_2L()
	{
	TInt errors = 0;
	
	INFO_PRINTF1(_L("Testing - CPluginAllocator::NewL method - condition 2"));
	CPluginAllocator *obj = NULL;
	obj = CPluginAllocator::NewL(KNullTestPlugin(), KNullTestPlugin());
	if(obj)
		{
		delete obj;
		obj = NULL;
		}
	else
		{
		errors++;
		INFO_PRINTF1(_L("error - obj not exists!"));
		}
		
	return errors;
	}


TInt CUloggerServerTest3Step::Test1_3L()
	{
	TInt errors = 0;
	
	INFO_PRINTF1(_L("Testing - CPluginAllocator::NewL method - condition 3"));
	CPluginAllocator *obj = NULL;
	TRAPD(err, obj = CPluginAllocator::NewL(KFileTestPlugin(), KUsbTestPlugin()));
	if(obj && err==KErrNone)
		{
		MOutputPlugin* output = obj->GetOutputPlugin(); //should exists
		MInputPlugin* input = obj->GetInputPlugin(); //does not matter
		if(!output)
			errors++;
		
		delete obj;
		obj = NULL;
		}
	else
		{
		INFO_PRINTF1(_L("plugin allocator not create. probably there is no usb plugin."));
		}
		
	return errors;
	}


TInt CUloggerServerTest3Step::Test1_4L()
	{
	TInt errors = 0;
	
	INFO_PRINTF1(_L("Testing - CPluginAllocator::NewL method - condition 4"));
	CPluginAllocator *obj = NULL;
	//should leave
	TRAPD(errCode, obj=CPluginAllocator::NewL(KFileTestPlugin(), KBadNameTestPlugin()));
	if(errCode == KErrNone)
		{
		errors++;
		INFO_PRINTF1(_L("error - method should leave!"));		
		}
	if(obj)
		{
		errors++;
		INFO_PRINTF1(_L("error - obj should not exist!"));		
		delete obj;
		obj = NULL;
		}
		
	return errors;
	}


TInt CUloggerServerTest3Step::Test2L()
	{
	TInt errors = 0;
	
	INFO_PRINTF1(_L("Testing - CPluginAllocator::NewLC method"));
	CPluginAllocator *obj = CPluginAllocator::NewLC(KFileTestPlugin(), KNullTestPlugin());
	if(obj == NULL)
	  {
	  INFO_PRINTF1(_L("error"));
	  errors++;
	  }
	  else
		{
		MOutputPlugin* output = obj->GetOutputPlugin(); //should exists
		MInputPlugin* input = obj->GetInputPlugin(); //should not exists
		if(!output || input)
			{
			INFO_PRINTF1(_L("error"));
			errors++;
			}
		
		//cleanup
		CleanupStack::PopAndDestroy(); //obj
		}
		
	return errors;
	} 
