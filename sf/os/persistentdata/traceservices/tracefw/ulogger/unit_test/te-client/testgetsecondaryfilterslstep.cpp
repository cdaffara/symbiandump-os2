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
 @file TestGetSecondaryFiltersLStep.cpp
 @internalTechnology
*/
#include "testgetsecondaryfilterslstep.h"
#include "te_uloggerclientsuitedefs.h"

CTestGetSecondaryFiltersLStep::~CTestGetSecondaryFiltersLStep()
/**
 * Destructor
 */
	{
	}

CTestGetSecondaryFiltersLStep::CTestGetSecondaryFiltersLStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestGetSecondaryFiltersLStep);
	}

TVerdict CTestGetSecondaryFiltersLStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
{
	INFO_PRINTF1(_L("TestGetSecondaryFiltersLStep started"));
	CTestUloggerClientApiStepBase::doTestStepPreambleL();
	return TestStepResult();
}


TVerdict CTestGetSecondaryFiltersLStep::doTestStepL()
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

		/**************First set secondary filter all *************/

		TInt iErrCode = iSession->SetSecondaryFilteringEnabled(EFalse);

		if( iErrCode == KErrNone )
		{
			TBool secFilteringEnabled;
			RArray<TUint32> enabledSecFilters;
			INFO_PRINTF1(_L("Global secondary filter has been enabled\n"));

			iSession->GetSecondaryFilteringEnabled(secFilteringEnabled);
			iSession->GetSecondaryFiltersEnabled(enabledSecFilters);			

			// need to implement proper check that makes sense here
 			// The algorithm below is based on the old API
//			for(TInt i=0; i<ptrArray->Count(); ++i)
//			{
//				secondary_filter.AppendL(*(ptrArray->At(i)));
//			}
//
//			TPtrC8 arg1 = secondary_filter[1];
//			if(arg1.Compare(_L8("enable")) == 0)		
//			{
//				INFO_PRINTF1(_L("Get secondary filter successful"));	
//				SetTestStepResult(EPass);
//			}
//			else
//				{
//				SetTestStepResult(EFail);		
//				}
			SetTestStepResult(EPass);

//			secondary_filter.Close();
		}
	}

	if (TestStepResult()==EPass)
	{	
		/**************First remove secondary filter all *************/

		TInt iErrCode = iSession->SetSecondaryFilteringEnabled(ETrue);

		if( iErrCode == KErrNone )
		{
//			CArrayPtrFlat<HBufC8> *ptrArray = new (ELeave) CArrayPtrFlat<HBufC8>(1);
//			CleanupStack::PushL(ptrArray);
			INFO_PRINTF1(_L("Global secondary filter has been enabled\n"));

			// New:
			TBool secFilteringEnabled;
			RArray<TUint32> enabledSecFilters;

			iSession->GetSecondaryFilteringEnabled(secFilteringEnabled);
			iSession->GetSecondaryFiltersEnabled(enabledSecFilters);			

			SetTestStepResult(EPass);

			// need to implement proper check that makes sense here
 			// The algorithm below is based on the old API
//			RArray<TPtrC8> secondary_filter;
//
//			iSession->GetSecondaryFilteringEnabled(*ptrArray);
//			for(TInt i=0; i<ptrArray->Count(); ++i)
//				secondary_filter.AppendL(*(ptrArray->At(i)));
//
//			TPtrC8 arg1 = secondary_filter[1];
//
//			if(arg1.Compare(_L8("disable")) == 0)		
//			{
//				INFO_PRINTF1(_L("Get secondary filter successful"));	
//				SetTestStepResult(EPass);
//			}	
//			else
//				SetTestStepResult(EFail);
//
//			secondary_filter.Close();
//
//			//delete pointers from array
//			ptrArray->ResetAndDestroy();
//			CleanupStack::PopAndDestroy();//ptrArray
		}
	}

	RArray<TUint32> *setfilters = new (ELeave)RArray<TUint32>(10);
	RArray<TUint32> *getfilters = new (ELeave)RArray<TUint32>(10);

	if (TestStepResult()==EPass)
	{
		setfilters->AppendL(KSecondaryFlt);

		/**************First set primary filter*************/

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
					INFO_PRINTF1(_L("Get primary filter value does not match with set value"));
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
		
		for(TInt i = 0; i < 4095 ; i++ )
		{
			setfilters->AppendL((TUint32)(i));
		}

		TRAPD(err, iSession->SetSecondaryFiltersEnabled(*setfilters, ETrue));
		if( KErrNone ==  err )
		{
			INFO_PRINTF1(_L("Secondary filter has been set with multiple filters,from 0 to 4094"));
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

TVerdict CTestGetSecondaryFiltersLStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("TestGetSecondaryFiltersLStep started"));
	CTestUloggerClientApiStepBase::doTestStepPreambleL();
	return TestStepResult();
	}
