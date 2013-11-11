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
 @file TestRemovePrimaryFltStep.cpp
 @internalTechnology
*/
#include "testremoveprimaryfltstep.h"
#include "te_uloggerclientsuitedefs.h"

CTestRemovePrimaryFltStep::~CTestRemovePrimaryFltStep()
/**
 * Destructor
 */
	{
	}

CTestRemovePrimaryFltStep::CTestRemovePrimaryFltStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestRemovePrimaryFltStep);
	}

TVerdict CTestRemovePrimaryFltStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
{
	CTestUloggerClientApiStepBase::doTestStepPreambleL();		
	return TestStepResult();
}


TVerdict CTestRemovePrimaryFltStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
{
	CArrayFixFlat<TUint8> *setfilters = new (ELeave)CArrayFixFlat<TUint8>(10);
	CArrayFixFlat<TUint8> *getfilters = new (ELeave)CArrayFixFlat<TUint8>(10);

	if (TestStepResult()==EPass)
	{
		const unsigned char KPrimaryFilter = 23;
		setfilters->AppendL(KPrimaryFilter);

		/**************First set primary filter*************/

		iSession->Connect();
		TInt iErrCode = iSession->SetPrimaryFiltersEnabled(*setfilters, ETrue);

		if( iErrCode == KErrNone )
		{
			iErrCode = iSession->SetPrimaryFiltersEnabled(*setfilters, EFalse);
			if( iErrCode == KErrNone )
			{
				SetTestStepResult(EPass);
				setfilters->Reset();
				iSession->GetPrimaryFiltersEnabled(*setfilters);

				if( setfilters->Count() <= 0 )
				{
					INFO_PRINTF2(_L("Primary filter has been removed , %d"), setfilters->At(0));
					SetTestStepResult(EPass);
				}
				else
				{
					for(TInt i = 0; i < setfilters->Count(); i++)
					{
						if(setfilters->At(i) == KPrimaryFilter)
						{
							INFO_PRINTF1(_L("GetFilter() Failed, can not varify the test output"));
							SetTestStepResult(EFail);
						}
						else
						{
							INFO_PRINTF2(_L("Primary filter has been removed , %d"), setfilters->At(0));
							SetTestStepResult(EPass);
						}
					}
				}
			}
			else
			{
				INFO_PRINTF2(_L("Primary filter remove failed , %d"), setfilters->At(0));
				SetTestStepResult(EFail);
			}	
		}
	}

	if(TestStepResult() == EPass)
	{
		setfilters->Reset();
		getfilters->Reset();

		for(TInt i = 0; i < 256 ; i++ )
		{
			setfilters->AppendL((TUint8)(i));
		}

		TRAPD(err, iSession->SetPrimaryFiltersEnabled(*setfilters, ETrue));
		if( KErrNone ==  err )
		{
			TInt iFailCount = 0;
		//	for(TInt i = 0; i < 256; i++)
		//	{
				 if(iSession->SetPrimaryFiltersEnabled(*setfilters, EFalse) != KErrNone)
				 {
				 	iFailCount ++;
				 	INFO_PRINTF1(_L("Remove primary filters failed"));
					SetTestStepResult(EFail);
		//		 }
			}
			if(iFailCount == 0)
			{
			 	INFO_PRINTF1(_L("All primary filters Removed"));
				SetTestStepResult(EPass);
			}
		}
	}

	setfilters->Reset();
	getfilters->Reset();

	if(setfilters)
	{
		delete setfilters;
		setfilters=NULL;
	}

	if(getfilters)
	{
		delete getfilters;
		getfilters=NULL;
	}

	return TestStepResult();
}

TVerdict CTestRemovePrimaryFltStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
{
	CTestUloggerClientApiStepBase::doTestStepPostambleL();
	return TestStepResult();
}
