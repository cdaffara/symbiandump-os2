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
 @file TestSetDataNotificationSizeStep.cpp
 @internalTechnology
*/
#include "testsetdatanotificationsizestep.h"
#include "te_uloggerclientsuitedefs.h"

CTestSetDataNotificationSizeStep::~CTestSetDataNotificationSizeStep()
/**
 * Destructor
 */
	{
	}

CTestSetDataNotificationSizeStep::CTestSetDataNotificationSizeStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestSetDataNotificationSizeStep);
	}

TVerdict CTestSetDataNotificationSizeStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("TestSetDataNotificationSizeStep started"));
	CTestUloggerClientApiStepBase::doTestStepPreambleL();
	return TestStepResult();
	}


TVerdict CTestSetDataNotificationSizeStep::doTestStepL()
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
			TInt iDataNotifySize =  4096; // 4K buffer
			TInt err = iSession->SetNotificationSize(iDataNotifySize); 
			if( err == KErrNone )
			{
				TInt iDataNotifySizeIn = 0;
				iSession->GetNotificationSize(iDataNotifySizeIn);
				if( iDataNotifySize == iDataNotifySizeIn )
				{
					INFO_PRINTF1(_L("SetDataNotificationSize successful"));
					SetTestStepResult(EPass);
				}
				else
				{
					INFO_PRINTF1(_L("DataNotification size returned, doesn't match"));
					SetTestStepResult(EFail);
				}
			}
			else
			{
				INFO_PRINTF2(_L("SetDataNotificationSize() returned error : %d"), err);
				SetTestStepResult(EFail);
			}
			
			SetTestStepResult(EPass);
		}
	  return TestStepResult();
	}



TVerdict CTestSetDataNotificationSizeStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("CTestSetDataNotificationSizeStep Completed"));
	CTestUloggerClientApiStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
