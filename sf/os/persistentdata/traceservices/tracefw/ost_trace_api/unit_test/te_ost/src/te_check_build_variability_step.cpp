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
// Example CTestStep derived implementation
//



/**
 @file te_check_build_variability_step.cpp
 @internalTechnology
*/
#include <hal.h>
#include "te_suite_defs.h"
#include "te_check_build_variability_step.h" //opensystemtrace gets included from here
#include "te_tracer.h"		//opensystemtrace and apicaller gets included from here

CCheckBuildVariabilityStep::~CCheckBuildVariabilityStep()
/**
 * Destructor
 */
	{
	}

CCheckBuildVariabilityStep::CCheckBuildVariabilityStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KCheckBuildVariabilityStep);
	#ifdef TE_TRACE_ENABLED
	_LIT(KLogFilename,"variability_systemenabled_traceenabled.utf");
	#elif TE_TRACE_DISABLED
	_LIT(KLogFilename,"variability_systemenabled_tracedisabled.utf");
	#else
	_LIT(KLogFilename,"dummy.utf");
	#endif

 	SetLogFilename(KLogFilename);
	}

TVerdict CCheckBuildVariabilityStep::CheckBuildtimeVariability()
{
	TBool builtIntoThisComponent = EFalse;
	TBool shouldBeBuiltIntoThisComponent = EFalse;

	#ifdef SYMBIAN_TRACE_EXECUTABLE_IS_INCLUDED
		builtIntoThisComponent = ETrue;
	#endif
	#ifdef TE_EXECUTABLE_SHOULD_BE_ENABLED
		shouldBeBuiltIntoThisComponent = ETrue;
	#endif

	iTraceData.iTracingBuiltIn = builtIntoThisComponent;

	INFO_PRINTF2(_L("Trace should be: built into executable = %i"), shouldBeBuiltIntoThisComponent);
	INFO_PRINTF2(_L("Trace is:        built into executable = %i"), builtIntoThisComponent);

	if((builtIntoThisComponent != shouldBeBuiltIntoThisComponent))
		{
		INFO_PRINTF1(_L("ERROR: Mismatched compile time variability"));
		SetTestStepResult(EFail);
		}

	return TestStepResult();
	}

TVerdict CCheckBuildVariabilityStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	if (TestStepResult()==EPass)
		{
		CheckBuildtimeVariability();
		if (TestStepResult()==EPass)
			{//now check that logging occurs properly
			CheckLoggingVariabilityL();
			}
		}
	return TestStepResult();
	}

TVerdict CCheckBuildVariabilityStep::CheckLoggingVariabilityL()
	{
	//test all cases when logging is enabled
	INFO_PRINTF2(_L("--- Logging enabled, tracing built in %i ---"), iTraceData.iTracingBuiltIn);
	iTraceData.iLoggingEnabled = ETrue;
	TTraceConfigsOperator::SetData(iTraceData, KData1, KData2, KAny1, KDefault8);
	TTraceConfigsOperator::SetData(iTraceData, KCharString, KCharStringSize);
	TInt error = StartLoggingL(iTraceData);
	if(error)
	{
		INFO_PRINTF2(_L("ERROR: Start logging failed, error %i "), error);
		SetTestStepResult(EFail);
		return TestStepResult();
	}
	TBool passLogging = TestTracesL();
	//test all cases where logging is disabled
	INFO_PRINTF1(_L("--- Logging disabled ---"));
	iTraceData.iLoggingEnabled = EFalse;
	error = StopLogging();
	if(error)
	{
		INFO_PRINTF2(_L("ERROR: Failed to stop logging, error %i "), error);
		SetTestStepResult(EFail);
		return TestStepResult();
	}

	TBool passNoLogging = TestTracesL();

	//if any of the cases failed, fail the test step
	if((passLogging == EFalse) || (passNoLogging == EFalse))
		{
		SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

TBool CCheckBuildVariabilityStep::TestTracesL()
	{
	TBool allPassed = ETrue;
	for(TInt i = 1; i <= KAPIMaxNumberOfTraceApis; i++)
		{
			if(i == EPrintTDesC8)
				INFO_PRINTF1(_L("-- Testing user side --"));
			if(i == EKernelPrintDesC8)
				INFO_PRINTF1(_L("-- Testing kernel side --"));
			if(i == EContextTClassification)
				INFO_PRINTF1(_L("-- Testing contexts --"));

			if(DoSendTraceL((TTraceApiUsed)i))
				INFO_PRINTF2(_L("Trace %i - ok"), i);
			else
				allPassed = EFalse;
		}
	INFO_PRINTF1(_L("Trace variability testing done"));

	return allPassed;
	}


TBool CCheckBuildVariabilityStep::DoSendTraceL(TTraceApiUsed aApiUsed)
	{
	iTraceData.iApiUsed = aApiUsed;
	TInt result = iTraceTester.SendTraceL(iTraceData);
	TBool pass = WasTraceVariabilitySuccessful(result);
	if(!pass)
		{
		if(iTraceData.iLoggingEnabled)
			{
				if(result == 1)
					INFO_PRINTF3(_L("Trace %i - No traces where output even though logging was ON"), aApiUsed, result);
				else
					INFO_PRINTF3(_L("Trace %i - Error %i, logging was ON"), aApiUsed, result);
			}
		else
			INFO_PRINTF3(_L("Trace %i - Error %i, logging was OFF"), aApiUsed, result);
		}

	return pass;
	}

TBool CCheckBuildVariabilityStep::WasTraceVariabilitySuccessful(TInt aTraceResult)
	{
	TBool passed = EFalse;

	if(iTraceData.iTracingBuiltIn)
		{
		if(iTraceData.iLoggingEnabled == (aTraceResult == KErrNone))
			{
			passed = ETrue;
			}
		}
	else //tracing not built in
		{
		if((aTraceResult == KInfoTraceNotOutput))
			passed = ETrue;
		}
	return passed;
	}
