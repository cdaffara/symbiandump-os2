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
 @file UTrace0Step.cpp
 @internalTechnology
*/
#include "cutrace_nonstatic_step.h"
#include "te_utracesuitedefs.h"


CUTrace_nonStatic_Step::~CUTrace_nonStatic_Step()
/**
 * Destructor
 */
	{
	}

CUTrace_nonStatic_Step::CUTrace_nonStatic_Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	ReplaceFile(KLogFileName);
	SetTestStepName(KUTrace_nonStatic_Step);
	}

/*TVerdict CUTrace_Printf_Char_Step::doTestStepPreambleL()
**
 * @return - TVerdict code
 * Override of base class virtual
 *
	{
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	// CTe_UTraceSuiteStepBase::doTestStepPreambleL();
	// if (TestStepResult()!=EPass)
	//    return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}*/




TVerdict CUTrace_nonStatic_Step::doTestStepL()
	{
	TInt failed = 0;
	if (TestStepResult()==EPass)
		{
		TBool logged = iLoggingEnabled;
		INFO_PRINTF2(_L("CUTrace_Printf_Char_Step - iTraceEnabled %i"),iTraceEnabled);
		TUTrace trace(iPF2,KText,EFalse, EFalse);
		trace.Printf("-----------headers------------");
		trace.Trace(UTRACE_HEADER(4,253,KBinary,EFalse,EFalse));
		trace.Trace(BTRACE_HEADER(4,253,KBinary));
		trace.Trace(UTRACE_HEADER(4,253,KBinary,EFalse,ETrue));
		trace.Trace(BTRACE_HEADER_P(4,253,KBinary));
		trace.Trace(UTRACE_HEADER(4,253,KBinary,ETrue,EFalse));
		trace.Trace(BTRACE_HEADER_C(4,253,KBinary));
		trace.Trace(UTRACE_HEADER(4,253,KBinary,ETrue,ETrue));
		trace.Trace(BTRACE_HEADER_CP(4,253,KBinary));
		trace.Trace(UTRACE_HEADER(4,253,KBinary,15,3));
		trace.Trace(BTRACE_HEADER_CP(4,253,KBinary));
		trace.Printf("-----------CUTrace_Printf_Char_Step------------");
		for(TInt context = 0; context <= 1; context++)//cn
			for(TInt pc = 0; pc <= 1; pc++)//pc
				{
				INFO_PRINTF1(_L("Tracing F1"));
				TUTrace trace1(iPF1 , KBinary, context, pc);
				logged = TUTrace::PrintfPrimary(253,EFalse,EFalse,"-------primary printf non static------");
				logged = trace1.Printf("c-style \"string\" with number %i!", 9999);
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log trace1.Printf(const char*: contex t%i, pc %i "), context, pc); failed++;}
				logged = trace1.Printf(KName16, 9999);
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log trace1.Printf(TRefByValue: contex t%i, pc %i "), context, pc); failed++;}
				logged = trace1.Printf(iBuf8);
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log trace1.Printf(iBuf8): contex t%i, pc %i "), context, pc); failed++;}
				logged = trace1.Printf(iBuf16);
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log trace1.Printf(const TDesC16&: contex t%i, pc %i "), context, pc); failed++;}
				logged = TUTrace::PrintfPrimary(253,EFalse,EFalse,"-------primary trace non static------");
				logged = trace1.Trace();
				BTrace0(iPF1,KBinary);
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log trace1.Trace(): contex t%i, pc %i "), context, pc); failed++;}
				logged = trace1.Trace(iA1);
				BTrace4(iPF1,KBinary,iA1);
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log trace1.Trace(TUint32: contex t%i, pc %i "), context, pc); failed++;}
				logged = trace1.Trace(iA1, iA2);
				BTrace8(iPF1,KBinary,iA1,iA2);
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log trace1.Trace(TUint32 , TUint32 ): contex t%i, pc %i "), context, pc); failed++;}
				logged = trace1.Trace(iA1, iA2, iA3);
				BTrace12(iPF1,KBinary,iA1, iA2, iA3);
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log trace1.Trace(TUint32 , TUint32 , TUint32 ): contex t%i, pc %i "), context, pc); failed++;}
				logged = trace1.Trace(&iAny16, sizeof(iAny16));
				BTraceBig(iPF1,KBinary,0, &iAny16, sizeof(iAny16));
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log trace1.Trace(const TAny*: contex t%i, pc %i "), context, pc); failed++;}

				INFO_PRINTF1(_L("Tracing F2"));
				TUTrace trace2(iPF1 , iUid , KBinary, context, pc );
				logged = TUTrace::PrintfPrimary(253,EFalse,EFalse,"-------secondary printf non static------");
				logged = trace2.Printf("c-style \"string\" with number %i!", 9999);
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log trace2.Printf(const char*: contex t%i, pc %i "), context, pc); failed++;}
				logged = trace2.Printf(KName16, 9999);
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log trace2.Printf(TRefByValue: contex t%i, pc %i "), context, pc); failed++;}
				logged = trace2.Printf(iBuf8);
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log trace2.Printf(const TDesC8: contex t%i, pc %i "), context, pc); failed++;}
				logged = trace2.Printf(iBuf16);
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log trace2.Printf(const TDesC16&: contex t%i, pc %i "), context, pc); failed++;}
				logged = TUTrace::PrintfPrimary(253,EFalse,EFalse,"-------secondary trace non static------");
				logged = trace2.Trace();
				BTraceFiltered4(iPF1,KBinary,iUid);
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log trace2.Trace(): contex t%i, pc %i "), context, pc); failed++;}
				logged = trace2.Trace(iA1);
				BTraceFiltered8(iPF1,KBinary,iUid,iA1);
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log trace2.Trace(iA1 : contex t%i, pc %i "), context, pc); failed++;}
				logged = trace2.Trace(iA1, iA2);
				BTraceFiltered12(iPF1,KBinary,iUid,iA1,iA2);
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log trace2.Trace(iA1 , iA1: contex t%i, pc %i "), context, pc); failed++;}
				logged = trace2.Trace(&iAny16, sizeof(iAny16) );
				BTraceFilteredBig(iPF1,KBinary, iUid,&iAny16, sizeof(iAny16));
				if(logged != iLoggingEnabled) {INFO_PRINTF3(_L("Didn't log trace2.Trace(const TAny*: contex t%i, pc %i "), context, pc); failed++;}
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
TVerdict CUTrace_Printf_Char_Step::doTestStepPostambleL()
**
 * @return - TVerdict code
 * Override of base class virtual
 *
	{
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_UTraceSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
	*/
