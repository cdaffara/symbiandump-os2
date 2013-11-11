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
 @file CUTrace_Static_Step.cpp
 @internalTechnology
*/
#include "cutrace_static_step.h"
#include "te_utracesuitedefs.h"


CUTrace_Static_Step::~CUTrace_Static_Step()
/**
 * Destructor
 */
	{
	}

CUTrace_Static_Step::CUTrace_Static_Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.

	SetTestStepName(KUTrace_Static_Step);
	}

/*TVerdict CUTrace_Static_Step::doTestStepPreambleL()
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

TVerdict CUTrace_Static_Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	TInt failed = 0;
	if (TestStepResult()==EPass)
		{
		TBool logged = iLoggingEnabled;

		TUTrace trace(iPF2,KText,EFalse,EFalse);
		trace.Printf("-----------CUTrace_Static_Step----------");
		for(TInt context = 0; context <= 1; context++)//cn
			for(TInt pc = 0; pc <= 1; pc++)//pc
				{
				logged = TUTrace::PrintfPrimary(iPF1, context, pc, "-----primary printf---");
				logged = TUTrace::PrintfPrimary(iPF1, context, pc, "static string no %i", 9999);
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log Printf(char *): context %i pc %i"), context, pc); failed++;}
				logged = TUTrace::PrintfPrimary(iPF1, context, pc, iBuf8);
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log Printf(TDes8): context %i pc %i"), context, pc); failed++;}
				logged = TUTrace::PrintfPrimary(iPF1, context, pc, KName16, 9999);
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log Printf(TRefByVal): context %i pc %i"), context, pc); failed++;}
				logged = TUTrace::PrintfPrimary(iPF1, context, pc, iBuf16);
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log Printf(TDesC16): context %i pc %i"), context, pc); failed++;}
				logged = TUTrace::PrintfPrimary(iPF1, context, pc, "-----secondary printf---");
				logged = TUTrace::Printf(iPF1, iUid, context, pc, "static string no %i", 9999);
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log Printf F2 (char*): context %i pc %i"), context, pc); failed++;}
				logged = TUTrace::Printf(iPF1, iUid, context, pc, iBuf8);
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log Printf F2(Des8): context %i pc %i"), context, pc); failed++;}
				logged = TUTrace::Printf(iPF1, iUid, context, pc, KName16, 9999);
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log Printf F2(TRefByVal): context %i pc %i"), context, pc); failed++;}
				logged = TUTrace::Printf(iPF1, iUid, context, pc, iBuf16);
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log Printf F2(TDes16): context %i pc %i"), context, pc); failed++;}

				logged = TUTrace::PrintfPrimary(iPF1, context, pc, "-----primary trace---");
				logged = TUTrace::TracePrimary(iPF1, KBinary, context, pc);
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log Trace(): context %i pc %i"), context, pc); failed++;}
				logged = TUTrace::TracePrimary(iPF1, KBinary, context, pc, iA1);
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log Trace(a1): context %i pc %i"), context, pc); failed++;}
				logged = TUTrace::TracePrimary(iPF1, KBinary, context, pc, iA1, iA2);
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log Trace(a1,a2): context %i pc %i"), context, pc); failed++;}
				logged = TUTrace::TracePrimary(iPF1, KBinary, context, pc, iA1, iA2, iA3);
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log Trace(): context %i pc %i"), context, pc); failed++;}
				logged = TUTrace::TracePrimary(iPF1, KBinary, context, pc, &KAny16, sizeof(KAny16));
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log Trace(Any): context %i pc %i"), context, pc); failed++;}
				logged = TUTrace::PrintfPrimary(iPF1, context, pc, "-----secondary trace---");
				logged = TUTrace::Trace(iPF1, iUid, KBinary, context, pc);
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log Trace(): context %i pc %i"), context, pc); failed++;}
				logged = TUTrace::Trace(iPF1, iUid, KBinary, context, pc, iA1);
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log Trace F2(a1): context %i pc %i"), context, pc); failed++;}
				logged = TUTrace::Trace(iPF1, iUid, KBinary, context, pc, iA1, iA2);
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log Trace F2(a1,a2): context %i pc %i"), context, pc); failed++;}
				logged = TUTrace::Trace(iPF1, iUid, KBinary, context, pc, &KAny16, sizeof(KAny16));
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log Trace F2(any): context %i pc %i"), context, pc); failed++;}
				}

		if(logged == iLoggingEnabled && failed == 0)
			SetTestStepResult(EPass);
		else
			SetTestStepResult(EFail);
		}
	//end of test
	return TestStepResult();
	}


/*
TVerdict CUTrace_Static_Step::doTestStepPostambleL()
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
