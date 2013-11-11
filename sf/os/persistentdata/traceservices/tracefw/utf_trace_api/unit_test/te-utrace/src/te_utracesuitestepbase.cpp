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
//

/**
 @file Te_UTraceSuiteStepBase.cpp
 @internalTechnology
*/

#include "te_utracesuitestepbase.h"
#include "te_utracesuitedefs.h"



// Device driver constants

TVerdict CTe_UTraceSuiteStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
	{
	if(GetConfigs()== KErrNone)
		{
		if(iLoggingEnabled)
			{
			INFO_PRINTF1(_L("startlogging"));
			StartLogging();
			SetTestStepResult(EPass);
			}
		}
	else
		{
		SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

TVerdict CTe_UTraceSuiteStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
	{

	// process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	//SetTestStepResult(EPass);  // or EFail
	if(iLoggingEnabled)
		StopLogging();
	return TestStepResult();
	}

TInt  CTe_UTraceSuiteStepBase::GetConfigs()
	{
	
	TInt pf1 = 0;
	TInt pf2 = 0;
	TInt sf1 = 0;
#if defined (__UTRACE_INCLUDE_DEBUG__) || (__UTRACE_INCLUDE_RELEASE__)
	iTraceEnabled = ETrue;
#else
	iTraceEnabled = EFalse;
#endif
	INFO_PRINTF2(_L("Trace enabled %i"),iTraceEnabled);

	if(!GetIntFromConfig(ConfigSection(),KTe_UTraceSuitePrimary1, pf1) ||
			!GetIntFromConfig(ConfigSection(),KTe_UTraceSuitePrimary2, pf2) ||
			!GetIntFromConfig(ConfigSection(),KTe_UTraceSuiteSecondary1, sf1) ||
			!GetBoolFromConfig(ConfigSection(),KTe_UTraceSuiteLoggingEnabled,iLoggingEnabled)||
			!GetIntFromConfig(ConfigSection(),KTe_UTraceSuiteA1,(TInt&) iA1) ||
			!GetIntFromConfig(ConfigSection(),KTe_UTraceSuiteA2, (TInt&) iA2) ||
			!GetIntFromConfig(ConfigSection(),KTe_UTraceSuiteA3, (TInt&) iA3) ||
			!GetIntFromConfig(ConfigSection(),KTe_UTraceSuiteUid, (TInt&) iUid) ||
			!GetIntFromConfig(ConfigSection(),KTe_UTraceSuiteTid, (TInt&) iTid) ||
			!GetIntFromConfig(ConfigSection(),KTe_UTraceSuiteUidReal, (TInt&) iUidReal)
			)
			{
			// Leave if there's any error.
			return KErrNotFound;
			}
		else
			{
			iPF1 = pf1; 
			iPF2 = pf2;
			iSF1 = sf1;
			INFO_PRINTF5(_L("Primary %i, primary2 %i, secondary %i, enabled %i"),iPF1, iPF2, iSF1, iLoggingEnabled);
			}
	return KErrNone;
	}
void CTe_UTraceSuiteStepBase::ReplaceFile(const TDesC& aFilename)
	{
	iFilename.Set(aFilename);
	iTracer.ReplaceFile(aFilename);
	}
void CTe_UTraceSuiteStepBase::OpenFile(const TDesC& aFilename)
	{
	iFilename.Set(aFilename);
	iTracer.OpenFile(aFilename);
	}
TInt CTe_UTraceSuiteStepBase::StartLogging()
    {
	//This bit enables the filters in BTrace so that they get logged		
    //TUint8 primary
    //TUint32 secondary
    //TBool logging enabled/disabled
    iLoggingEnabled = ETrue;
	iPrimaryFilters[iPF1] = 1;
	iPrimaryFilters[iPF2] = 1;
	iTracer.iSecondaryFilters.Append(iSF1);
	iTracer.iSecondaryFilters.Append(iUid);
	iTracer.iSecondaryFilters.Append(iUidReal);

	//This just prints so that we know what filters we set, can remove
	for(int i = 0; i < KPrimaryFilterMaxSize; i++)
		if(iPrimaryFilters[i])
			INFO_PRINTF2(_L("Primary filter set is %d"), i);
	for(int i = 0; i < iTracer.iSecondaryFilters.Count(); i++)
		INFO_PRINTF2(_L("Secondary filter set is %d"), iTracer.iSecondaryFilters[i]);

	INFO_PRINTF1(_L("Starting logging"));
	//This is where we actually set the filters	
	iTracer.SetTraceFilters();    
	return KErrNone;
    }

TInt CTe_UTraceSuiteStepBase::StopLogging()
    {
    TInt error = KErrNotFound;
    iLoggingEnabled = EFalse;
    //This resets the variable and the server again.
	for(int i = 0; i < KPrimaryFilterMaxSize; i++)
		iPrimaryFilters[i] = 0;
	iTracer.iSecondaryFilters.Reset();
	
	//This outputs the trace to file
	error = iTracer.OpenFile(iFilename);
	if(error == KErrNone)
		iTracer.DumpTracesToFile();
	else
		INFO_PRINTF2(_L("Log file wasnt created %d \n"), error);
	return error;
    }

CTe_UTraceSuiteStepBase::~CTe_UTraceSuiteStepBase()
	{
	}

CTe_UTraceSuiteStepBase::CTe_UTraceSuiteStepBase()
	{
	iPF1 = 0; 
	iPF2 = 0;
	iSF1 = 0; 
	iLoggingEnabled = EFalse;
	iTraceEnabled = EFalse;
	iBuf16 = KName16;
	iBuf8 = KName8;
	iAny16 = KAny16;
	iAny8 = KAny8;
	iA1 = 0;
	iA2 = 0;
	iA3 = 0;
	iUid = 0;
	iTid = 0;
	iUidReal = 0;
	iFilename.Set(KLogFileName); //default
	}
