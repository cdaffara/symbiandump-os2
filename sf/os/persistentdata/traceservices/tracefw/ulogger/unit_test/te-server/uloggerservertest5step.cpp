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
 @file UloggerServerTest5Step.cpp
 @internalTechnology
*/
#include "uloggerservertest5step.h"
#include "te_uloggerservertestsuitedefs.h"
#include "uloggerdatatypes.h"

namespace Ulogger
{

CUloggerServerTest5Step::~CUloggerServerTest5Step()
/**
 * Destructor
 */
	{
	
	}

CUloggerServerTest5Step::CUloggerServerTest5Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KUloggerServerTest5Step);
	}

TVerdict CUloggerServerTest5Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("****This is unit test for CUloggerChannelSettings class****"));	
	iScheduler = new (ELeave) CActiveScheduler();
	CActiveScheduler::Install(iScheduler);
	

	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CUloggerServerTest5Step::doTestStepL()
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
		iErrors += Test1L(); //CControlFramework::StartReading + StopReading
		
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



TVerdict CUloggerServerTest5Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	delete iScheduler;
	iScheduler = NULL;
	
	return TestStepResult();
	}




TInt CUloggerServerTest5Step::Test1L()
	{
	TInt errors = 0;
	
	return errors;
	}

}
