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
 @file te_utrace_suite_step_base.cpp
 @internalTechnology
*/

#include "te_suite_step_base.h"
#include "te_suite_defs.h"
#include "te_tracerecord.h"

CTe_UTraceSuiteStepBase::~CTe_UTraceSuiteStepBase()
	{
	}

CTe_UTraceSuiteStepBase::CTe_UTraceSuiteStepBase()
	{
	}

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
	if(TestStepResult()==EPass)
		{
		TRAPD(error, iLogger.DeleteLogfileL());
		error = ReadGeneralConfigs();
		if(error)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF2(_L("Test step failed on doTestStepPreambleL with error %i"), error);
			}
		}
	return TestStepResult();
	}

TInt CTe_UTraceSuiteStepBase::StartLoggingL(TTraceConfigs& aTraceData)
	{
	TInt err = iLogger.StartLoggingL(aTraceData);
	return err;
	}

void CTe_UTraceSuiteStepBase::SetLogFilename(const TDesC& aFilename)
	{
	iLogger.SetLogFilename(aFilename);
	}


TInt CTe_UTraceSuiteStepBase::ReadGeneralConfigs()
	{
	iTraceData.iGroupId = KGroupId;
	iTraceData.iTraceId = KTraceId;
	iTraceData.iHasThreadId = KContextId; 
	iTraceData.iHasProgramCounter = KPc; 	
	#ifdef	EXECUTABLE_DEFAULT_COMPONENTID
	iTraceData.iComponentId = EXECUTABLE_DEFAULT_COMPONENTID;
	#else
	iTraceData.iComponentId = KComponentId;	
	#endif
	return KErrNone;
	}

void CTe_UTraceSuiteStepBase::DeleteLogFileL()
	{
	iLogger.DeleteLogfileL();
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
	//not really doing anything here...
	return TestStepResult();
	}

TInt CTe_UTraceSuiteStepBase::StopLogging()
	{
	return iLogger.StopLogging();
	}
