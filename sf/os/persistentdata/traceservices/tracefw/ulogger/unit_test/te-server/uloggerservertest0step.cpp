// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example CTestStep derived implementation
// 
//

/**
 @file UloggerServerTest0Step.cpp
 @internalTechnology
*/
#include "uloggerservertest0step.h"
#include "te_uloggerservertestsuitedefs.h"
#include "uloggerwatcher.h"



CUloggerServerTest0Step::~CUloggerServerTest0Step()
/**
 * Destructor
 */
	{
	}

CUloggerServerTest0Step::CUloggerServerTest0Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KUloggerServerTest0Step);
	}

TVerdict CUloggerServerTest0Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("****This is unit test for CULoggerWatcher class****"));	
	
	INFO_PRINTF1(_L("************************************************"));
	INFO_PRINTF1(_L("Note: To test CUloggerServer properly we must:"));
	INFO_PRINTF1(_L("- change UID3 for our test unit as we need access to server private directory"));
	INFO_PRINTF1(_L("************************************************"));

	
	iScheduler = new (ELeave) CActiveScheduler();
	CActiveScheduler::Install(iScheduler);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CUloggerServerTest0Step::doTestStepL()
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
		  iErrors += this->Test1L();//CULoggerWatcher::NewL
		  iErrors += this->Test2L();//CULoggerWatcher::NewLC
		  iErrors += this->Test3L();//CULoggerWatcher::StartWatching
		  iErrors += this->Test4L();//CULoggerWatcher::StopWatching
		  iErrors += this->Test5L();//CULoggerWatcher::RunL

		  
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



TVerdict CUloggerServerTest0Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	delete iScheduler;
	iScheduler = NULL;
	return TestStepResult();
	}


TInt CUloggerServerTest0Step::Test1L()
	{
	TInt errors = 0;
	INFO_PRINTF1(_L("Testing - CULoggerWatcher::NewL method"));
	CULoggerWatcher *obj = CULoggerWatcher::NewL();
	if(obj == NULL)
	  {
	  INFO_PRINTF1(_L("error"));
	  errors++;
	  }
	  else
		{
		delete obj;
		obj = NULL;
		}
	return errors;
	}

TInt CUloggerServerTest0Step::Test2L()
	{
	TInt errors = 0;
	  //CULoggerWatcher::NewLC
	  INFO_PRINTF1(_L("Testing - CULoggerWatcher::NewLC method"));
	  CULoggerWatcher *obj = CULoggerWatcher::NewLC();
	  if(obj == NULL)
	  	{
		  INFO_PRINTF1(_L("error"));
		  errors++;
	  	}
	  else
		  CleanupStack::PopAndDestroy();
	  return errors;
	  } 

TInt CUloggerServerTest0Step::Test3L()
{
	TInt errors = 0;
	  //CULoggerWatcher::StartWatching
	  INFO_PRINTF1(_L("Testing - CULoggerWatcher::StartWatching method"));
	  CULoggerWatcher *obj = CULoggerWatcher::NewL();
	  /*
	   * We need to create service provider for active object (RTimer is good choice)
	   * */
	  RTimer timer;
	  timer.CreateLocal();
	  timer.After(obj->GetStatus(), 2*1000*1000);
	  obj->StartWatching(this);
	  obj->StopWatching();
	  timer.Close();
	  if(obj->GetStatus().Int() != KErrNone)
		{
		INFO_PRINTF1(_L("error"));
		errors++;
		} 
	   delete obj;
	   return errors;
} 

TInt CUloggerServerTest0Step::Test4L()
{
	TInt errors = 0;
	   //CULoggerWatcher::StopWatching
		INFO_PRINTF1(_L("Testing - CULoggerWatcher::StopWatching method"));
	    CULoggerWatcher *obj = CULoggerWatcher::NewL();
	    RTimer timer;
	    timer.CreateLocal();
	    timer.After(obj->GetStatus(), 2*1000*1000);
	    obj->StartWatching(this);
	    obj->StopWatching();
	    timer.Close();
	    TBuf<128> buf;
	    buf.AppendFormat(_L("Result of Stopping: %d"), obj->GetStatus().Int());
	    INFO_PRINTF1(buf);
		if(obj->GetStatus().Int() != KErrNone)
		 {
		 INFO_PRINTF1(_L("error"));
		 errors++;
		 }
		delete obj;
		
		return errors;
} 

TInt CUloggerServerTest0Step::Test5L()
{
	TInt errors = 0;
	  //CULoggerWatcher::RunL
	  INFO_PRINTF1(_L("Testing - CULoggerWatcher::RunL method"));
    CULoggerWatcher *obj = CULoggerWatcher::NewL();
    RTimer timer;
    timer.CreateLocal();
	      timer.After(obj->GetStatus(), 2*1000*1000);
    obj->StartWatching(this); //CULogger watcher must have observer initialized
    							//becouse testing null pointer is not allowed
    obj->StopWatching();
    timer.Close();
    TRAPD(err, obj->RunL());
	  if(err != KErrNone)
		  {
		  INFO_PRINTF1(_L("error"));
		  errors++;
		  }
	  delete obj;
	  
	  return errors;
}

