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
 @file TestSetSecondaryFltStep.cpp
 @internalTechnology
*/
#include "testsetsecondaryfltstep.h"
#include "te_uloggerclientsuitedefs.h"

CTestSetSecondaryFltStep::~CTestSetSecondaryFltStep()
/**
 * Destructor
 */
	{
	}

CTestSetSecondaryFltStep::CTestSetSecondaryFltStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestSetSecondaryFltStep);
	}

TVerdict CTestSetSecondaryFltStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("TestSetSecondaryFltStep started"));
	CTestUloggerClientApiStepBase::doTestStepPreambleL();
	return TestStepResult();
	}

TVerdict CTestSetSecondaryFltStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
{
	RArray<TUint32> *setfilters = new (ELeave)RArray<TUint32>(10);
	RArray<TUint32> *getfilters = new (ELeave)RArray<TUint32>(10);

	if (TestStepResult()==EPass)
	{
		setfilters->AppendL(KSecondaryFlt);

		/**************First set primary filter*************/

		iSession->Connect();
		TInt iErrCode = iSession->SetSecondaryFiltersEnabled(*setfilters, ETrue);

		if( iErrCode == KErrNone )
		{
			INFO_PRINTF2(_L("Secondary filter has been set with single filter, %d, check log"), (*setfilters)[0]);

			setfilters->Reset();

			iSession->GetSecondaryFiltersEnabled(*setfilters);

			if( setfilters->Count() > 0 )
			{
				if( (*setfilters)[0] == KSecondaryFlt )
				{
					INFO_PRINTF1(_L("Get secondary filter successful"));	
					SetTestStepResult(EPass);
				}
				else
				{
					INFO_PRINTF1(_L("Get secondary filter value does not match with set value"));
					SetTestStepResult(EFail);;
				}
			}
			else
			{
				INFO_PRINTF1(_L("GetFilter() Failed, can not varify the test output"));
				SetTestStepResult(EFail);
			}
		}
	}

	if(TestStepResult() == EPass)
	{
		//prepare test
		getfilters->Reset();
		iSession->GetSecondaryFiltersEnabled(*getfilters);
		iSession->SetSecondaryFiltersEnabled(*getfilters, EFalse);
		setfilters->Reset();
		getfilters->Reset();

		for(TInt i = 0; i < 4096 ; i++ )
		{
			setfilters->AppendL((TUint32)(i));
		}

		TRAPD(err, iSession->SetSecondaryFiltersEnabled(*setfilters, ETrue));
		if( KErrNone == err )
		{
			INFO_PRINTF1(_L("Secondary filter has been set with multiple filters,from 0 to 4095"));
			SetTestStepResult(EPass);
		}
		else
		{
			INFO_PRINTF2(_L("Multiple secondary filters have not been set succesfully, error code %d"), err);
			SetTestStepResult(EFail);
		}

		//Now get the primary filter set above
		iSession->GetSecondaryFiltersEnabled(*getfilters);

		if( getfilters->Count() == setfilters->Count() )
		{	
			INFO_PRINTF1(_L("setfilter and getfilter have the same count"));
			for(TInt i = 0; i < getfilters->Count(); i++)
			{
				for(TInt j = 0; j < setfilters->Count(); j++)
				{
					if( (*getfilters)[i] == (*setfilters)[j] )
						SetTestStepResult(EPass);
					else
						SetTestStepResult(EFail);
				}
			}
		}	
		else
		{
			INFO_PRINTF1(_L("setfilter and getfilter have different counts"));
			INFO_PRINTF2(_L("setfilter has %d"), setfilters->Count());
			INFO_PRINTF2(_L("getfilter has %d"), getfilters->Count());
			SetTestStepResult(EFail);
		}
	}

	setfilters->Close();
	getfilters->Close();

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

TVerdict CTestSetSecondaryFltStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
{
	INFO_PRINTF1(_L("TestSetSecondaryFltStep completed"));
	CTestUloggerClientApiStepBase::doTestStepPostambleL();
	return TestStepResult();
}
