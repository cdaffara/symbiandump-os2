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
 @file
 @internalTechnology
*/
#include "d32utrace.h"

#include "cutrace_kernperf_step.h"
#include "te_utracesuitedefs.h"


CUTrace_KernPerf_Step::~CUTrace_KernPerf_Step()
/**
 * Destructor
 */
	{
	}

CUTrace_KernPerf_Step::CUTrace_KernPerf_Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	ReplaceFile(KLogKBenchFileName);
	SetTestStepName(KUTrace_KernPerf_Step);
	}

/*TVerdict CUTrace_Performance_Step::doTestStepPreambleL()
**
 * @return - TVerdict code
 * Override of base class virtual
 *
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class CUTrace_Trace_a1a2a3_Step"));
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	// CTe_UTraceSuiteStepBase::doTestStepPreambleL();
	// if (TestStepResult()!=EPass)
	//    return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}
*/


TVerdict CUTrace_KernPerf_Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	if (TestStepResult()==EPass)
		{
		TInt err = KErrNone;
		TBool logged = iLoggingEnabled;
		TInt failed = 0;
		TUTrace trace(iPF2,KText,EFalse,EFalse);
		trace.Printf("CUtrace_Performance_Step");
		INFO_PRINTF1(_L("UID Context PC \t BTrace Time \t UTrace Time \t diff"));
		for(TBool logging = 0; logging <= 1; logging++)
			{
			if(logging)
				{
				err = StartLogging();
				trace.Printf("Traces enabled");
				INFO_PRINTF1(_L("Traces enabled"));
				}
			else
				{
				trace.Printf("Traces disabled");
				INFO_PRINTF1(_L("Traces disabled"));
				}
			if(!err)
				{
				TestTraces();
				for(TBool uid = 0; uid <=1; uid++)
					for(TBool context = 0; context <= 1; context++)
						for(TBool pc = 0; pc <=1; pc++)
							for(TInt type = 0; type <= 4; type++)
								{
								TInt utraceTime = 0;
								TInt btraceTime = 0;
								err = TestTraces();
								if(!err)
									{
									if(logged != iLoggingEnabled){INFO_PRINTF2(_L("BTrace failed to log = %i"), logged ); failed++;}
									TReal32 calc = utraceTime-btraceTime;
									calc = calc/btraceTime*100;
									TInt diff = calc;
									if(type==0)
										INFO_PRINTF7(_L("empty - %i%i%i \t\t %i \t\t %i \t = %i%%"), uid, context, pc, btraceTime, utraceTime, diff);
									if(type==1)
										INFO_PRINTF7(_L("a1 a2 - %i%i%i \t\t %i \t\t %i \t = %i%%"), uid, context, pc, btraceTime, utraceTime, diff);
									if(type==2)
										INFO_PRINTF7(_L("KAny  %i%i%i \t\t %i \t\t %i \t = %i%%"), uid, context, pc, btraceTime, utraceTime, diff);
									if(type==3)
										INFO_PRINTF7(_L("Printf char* - %i%i%i \t %i \t\t %i \t = %i%%"), uid, context, pc, btraceTime, utraceTime, diff);
									if(type==4)
										INFO_PRINTF7(_L("Printf TRefByVal - %i%i%i \t %i \t\t %i \t = %i%%"), uid, context, pc, btraceTime, utraceTime, diff);
									}
								}
				}//if(!err)
			if(logging && !err)
				StopLogging();
			}
		if(!err && failed == 0)
			SetTestStepResult(EPass);
		else
			SetTestStepResult(EFail);
		}
	//end of test
	return TestStepResult();
	}



/*
TVerdict CUTrace_KernPerf_Step::doTestStepPostambleL()
**
 * @return - TVerdict code
 * Override of base class virtual
 *
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Postamble in Class CUTrace_Trace_a1a2a3_Step"));
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_UTraceSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
*/


TInt CUTrace_KernPerf_Step::TestTraces()
{
	RUTrace tracer;
	tracer.Open();
	TInt failed = 0;
	TInt err = tracer.TestUtrace((TInt&)failed);
	return err;
}
