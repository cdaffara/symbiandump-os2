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
 @file TestResizeTraceBufferSizeStep.cpp
 @internalTechnology
*/
#include "testtracebuffermodestep.h"
#include "te_uloggerclientsuitedefs.h"

CTestTraceBufferModeStep::~CTestTraceBufferModeStep()
/**
 * Destructor
 */
	{
	}

CTestTraceBufferModeStep::CTestTraceBufferModeStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestTraceBufferModeStep);
	}

TVerdict CTestTraceBufferModeStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("TestResizeTraceBufferSizeStep started"));
	CTestUloggerClientApiStepBase::doTestStepPreambleL();
	return TestStepResult();
	}


TVerdict CTestTraceBufferModeStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
			iSession->Connect();
			TInt iBufferMode =  0; // circular buffer mode
			TInt err = iSession->SetBufferMode(iBufferMode); 
			if( err == KErrNone )
			{
				TInt iBufferModeIn = 0;
				iSession->GetBufferMode(iBufferModeIn);
				if( iBufferMode == iBufferModeIn )
				{
					INFO_PRINTF1(_L("Buffer Mode right"));
					SetTestStepResult(EPass);
				}
				else
				{
					INFO_PRINTF1(_L("Buffer Mode returned, doesn't match"));
					SetTestStepResult(EFail);
				}
			}
			else
			{
				INFO_PRINTF2(_L("SetBufferMode() returned error : %d"), err);
				SetTestStepResult(EFail);
			}
			
			
			iBufferMode =  1; // straight buffer mode
			err = iSession->SetBufferMode(iBufferMode); 
			if( err == KErrNone )
			{
				TInt iBufferModeIn = 0;
				iSession->GetBufferMode(iBufferModeIn);
				if( iBufferMode == iBufferModeIn )
				{
					INFO_PRINTF1(_L("Buffer Mode right"));
					SetTestStepResult(EPass);
				}
				else
				{
					INFO_PRINTF1(_L("Buffer Mode returned, doesn't match"));
					SetTestStepResult(EFail);
				}
			}
			else
			{
				INFO_PRINTF2(_L("SetBufferMode() returned error : %d"), err);
				SetTestStepResult(EFail);
			}
			
			iBufferMode =  99; // wrong buffer mode
			err = iSession->SetBufferMode(iBufferMode); 
			if( err == KErrNone )
			{
				INFO_PRINTF1(_L("Buffer Mode set wrong"));
				SetTestStepResult(EFail);
				
			}
			else
			{
				INFO_PRINTF1(_L("Buffer mode worked correctly"));
				SetTestStepResult(EPass);
			}
			
			//SetTestStepResult(EPass);
			
			//SetTestStepResult(EPass);
		}
	  return TestStepResult();
	}



TVerdict CTestTraceBufferModeStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("TestResizeTraceBufferSizeStep completed"));
	CTestUloggerClientApiStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
