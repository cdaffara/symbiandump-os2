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
 @file UloggerServerTest2Step.cpp
 @internalTechnology
*/
#include "uloggerservertest2step.h"
#include "te_uloggerservertestsuitedefs.h"
#include "uloggerserver.h"
#include "uloggersession.h"

using namespace Ulogger;

CUloggerServerTest2Step::~CUloggerServerTest2Step()
/**
 * Destructor
 */
	{
	}

CUloggerServerTest2Step::CUloggerServerTest2Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KUloggerServerTest2Step);
	}

TVerdict CUloggerServerTest2Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	iScheduler = new (ELeave) CActiveScheduler();
	CActiveScheduler::Install(iScheduler);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CUloggerServerTest2Step::doTestStepL()
	{
	  if (TestStepResult()==EPass)
		{
		TInt errors = 0;
		
		errors += Test1L();//CULoggerSession::NewL
		errors += Test2L();//CULoggerSession::NewLC
		
		
		
			/*
			 * Method CULoggerSession::ServiceL cannot be tested here because it require
			 * of passing RMessage2 reference with valid RMessagePtr (IPC)
			 * If we are going to pass empty RMessage2 system will panic our process with
			 * User Panic 70.
			 * */
			
		//display results
		TBuf<128> res;
		res.AppendFormat(_L("%d errors"), errors);
		INFO_PRINTF1(_L("****Results****"));
		INFO_PRINTF1(res);
		if(errors == 0)  
			SetTestStepResult(EPass);
			else
				SetTestStepResult(EFail);
		}
	  return TestStepResult();
	}



TVerdict CUloggerServerTest2Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	delete iScheduler;
	iScheduler = NULL;
	return TestStepResult();
	}

TInt CUloggerServerTest2Step::Test1L()
{//CULoggerSession::NewL
	TInt errors = 0;
  	INFO_PRINTF1(_L("Testing - CULoggerSession::NewL method"));
  	CULoggerServer *server = NULL;
  	server = CULoggerServer::NewLC(EPriorityBackground);
  	if(server != NULL)
  		{
  		RThread thread;
  		CULoggerSession *serverSession = CULoggerSession::NewL(thread, *server);
  		if(serverSession != NULL)
  			{
  			//delete serverSession; //don't need to delete session
  			}
  			else
  				{
  				INFO_PRINTF1(_L("error"));
  				++errors;
  				}
  			
  		CleanupStack::PopAndDestroy();
  		}
  		else
  		{
  		INFO_PRINTF1(_L("server creation error"));
  		++errors;
  		}
  	return errors;
}

TInt CUloggerServerTest2Step::Test2L()
{//CULoggerSession::NewLC
	TInt errors = 0;
  	INFO_PRINTF1(_L("Testing - CULoggerSession::NewLC method"));
  	CULoggerServer *server = NULL;
  	server = CULoggerServer::NewLC(EPriorityBackground);
  	if(server != NULL)
  		{
  		RThread thread;
  		CULoggerSession *serverSession = CULoggerSession::NewLC(thread, *server);
  		if(serverSession != NULL)
  			{
  			CleanupStack::Pop();//serverSession
  			}
  			else
  				{
  				INFO_PRINTF1(_L("error"));
  				++errors;
  				}
  			
  		CleanupStack::PopAndDestroy();
  		}
  		else
  		{
  		INFO_PRINTF1(_L("server creation error"));
  		++errors;
  		}
  	return errors;
}
