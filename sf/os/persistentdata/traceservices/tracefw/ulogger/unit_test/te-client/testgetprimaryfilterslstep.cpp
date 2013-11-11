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
 @file TestGetPrimaryFiltersLStep.cpp
 @internalTechnology
*/
#include "testgetprimaryfilterslstep.h"
#include "te_uloggerclientsuitedefs.h"

CTestGetPrimaryFiltersLStep::~CTestGetPrimaryFiltersLStep()
/**
 * Destructor
 */
	{
	}

CTestGetPrimaryFiltersLStep::CTestGetPrimaryFiltersLStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
		
	SetTestStepName(KTestGetPrimaryFiltersLStep);
	}

TVerdict CTestGetPrimaryFiltersLStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("TestGetPrimaryFiltersLStep started"));
	
	CTestUloggerClientApiStepBase::doTestStepPreambleL();
	return TestStepResult();
	}


TVerdict CTestGetPrimaryFiltersLStep::doTestStepL()
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
		setfilters->AppendL(KPrimaryFlt);
	
		/**************First set primary filter*************/
		INFO_PRINTF1(_L("before iSession->Connect()"));
		TInt cerr = iSession->Connect();
		INFO_PRINTF2(_L("connect err: %d"), cerr);
		
		INFO_PRINTF1(_L("before iSession->SetPrimaryFiltersEnabled"));
		TInt iErrCode = iSession->SetPrimaryFiltersEnabled(*setfilters, ETrue);
		INFO_PRINTF2(_L("SetPrimaryFiltersEnabled err: %d"), iErrCode);

		if( iErrCode == KErrNone )
		{
			INFO_PRINTF2(_L("Primary filter has been set with single filter, %d, check log"), setfilters->At(0));
			
			setfilters->Reset();
			
			iSession->GetPrimaryFiltersEnabled(*setfilters);
		
			if( setfilters->Count() > 0 )
			{
				if( setfilters->At(0) == KPrimaryFlt )
				{
					INFO_PRINTF1(_L("Get primary filter successful"));	
					SetTestStepResult(EPass);
				}
				else
				{
					INFO_PRINTF1(_L("Get primary filter failed"));
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
		setfilters->Reset();
		getfilters->Reset();

		for(TInt i = 0; i < 256 ; i++ )
		{
			setfilters->AppendL((TUint8)(i));
		}

		TRAPD(err, iSession->SetPrimaryFiltersEnabled(*setfilters, ETrue));
		if( KErrNone ==  err )
		{
			INFO_PRINTF1(_L("Primary filter has been set with multiple filters,from 0 to 255"));
			SetTestStepResult(EPass);
		}
		else
		{
			INFO_PRINTF2(_L("Multiple primary filters have not been set succesfully, error code %d"), err);
			SetTestStepResult(EFail);
		}
	
		//Now get the primary filter set above
		iSession->GetPrimaryFiltersEnabled(*getfilters);
	
		if( getfilters->Count() == setfilters->Count() )
		{	
			INFO_PRINTF1(_L("setfilter and getfilter have the same count"));
			for(TInt i = 0; i < getfilters->Count(); i++)
			{
				for(TInt j = 0; j < setfilters->Count(); j++)
				{
					if(getfilters->At(i) == setfilters->At(j))
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

TVerdict CTestGetPrimaryFiltersLStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
		INFO_PRINTF1(_L("TestGetPrimaryFiltersLStep completed"));
		CTestUloggerClientApiStepBase::doTestStepPostambleL();	
		return TestStepResult();
	}
