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
 @file TestGetTraceBufferSizeStep.cpp
 @internalTechnology
*/
#include "testgettracebuffersizestep.h"
#include "te_uloggerclientsuitedefs.h"

CTestGetTraceBufferSizeStep::~CTestGetTraceBufferSizeStep()
/**
 * Destructor
 */
	{
	}

CTestGetTraceBufferSizeStep::CTestGetTraceBufferSizeStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestGetTraceBufferSizeStep);
	}

TVerdict CTestGetTraceBufferSizeStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("TestGetTraceBufferSizeStep started"));
	CTestUloggerClientApiStepBase::doTestStepPreambleL();
	return TestStepResult();
	}


TVerdict CTestGetTraceBufferSizeStep::doTestStepL()
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
			TInt iBufferSize =  4096; // 4K buffer
			TInt err = iSession->SetBufferSize(iBufferSize); 
			if( err == KErrNone )
			{
				TInt iBufferSizeIn = 0;
				iSession->GetBufferSize(iBufferSizeIn);
				if( iBufferSize == iBufferSizeIn )
				{
					INFO_PRINTF1(_L("GetTraceBuffer() successful"));
					SetTestStepResult(EPass);
				}
				else
				{
					INFO_PRINTF1(_L("Buffer size returned, doesn't match"));
					SetTestStepResult(EFail);
				}
			}
			else
			{
				INFO_PRINTF2(_L("ResizeBuffer() returned error : %d"), err);
				SetTestStepResult(EFail);
			}
			
			SetTestStepResult(EPass);
		}
	  return TestStepResult();
	}



TVerdict CTestGetTraceBufferSizeStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("TestGetTraceBufferSizeStep completed"));
	CTestUloggerClientApiStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
