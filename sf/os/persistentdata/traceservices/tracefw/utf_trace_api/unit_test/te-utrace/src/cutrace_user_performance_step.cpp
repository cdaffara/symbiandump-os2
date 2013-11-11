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
 @file CUTrace_Performance_Step.cpp
 @internalTechnology
*/
#include "cutrace_user_performance_step.h"
#include "te_utracesuitedefs.h"
#include <hal.h>



CUTrace_User_Performance_Step::~CUTrace_User_Performance_Step()
/**
 * Destructor
 */
	{
	}

CUTrace_User_Performance_Step::CUTrace_User_Performance_Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	ReplaceFile(KLogUBenchFileName);
	SetTestStepName(KUTrace_User_Performance_Step);
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


TVerdict CUTrace_User_Performance_Step::doTestStepL()
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
				for(TBool uid = 0; uid <=1; uid++)
					for(TBool context = 0; context <= 1; context++)
						for(TBool pc = 0; pc <=1; pc++)
							for(TInt type = 0; type <= 4; type++)
							{
							TInt utraceTime = 0;
							TInt btraceTime = 0;
							if(uid)
								logged = UserUTraceBench(iPF1, type, iUid, context, pc, utraceTime);
							else
								logged = UserUTraceBench(iPF1, type, uid, context, pc, utraceTime);
							if(logged != iLoggingEnabled){INFO_PRINTF2(_L("Utrace failed to log = %i"), logged ); failed++;}
							logged = UserBTraceBench(iPF1, type, uid, context, pc, btraceTime);
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
							/*							if(type==5)
							if(type==6)
										INFO_PRINTF7(_L("KAny  %i%i%i \t\t %i \t\t %i \t = %i%%"), uid, context, pc, btraceTime, utraceTime, diff);
							if(type==7)
								INFO_PRINTF7(_L("Printf TDes16 %i%i%i \t %i \t\t %i \t = %i%%"), uid, context, pc, btraceTime, utraceTime, diff);
							if(type==8)
								INFO_PRINTF7(_L("Printf TDes8 %i%i%i \t %i \t\t %i \t = %i%%"), uid, context, pc, btraceTime, utraceTime, diff);
*/							}
				}
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
TVerdict CUTrace_Performance_Step::doTestStepPostambleL()
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



TBool CUTrace_User_Performance_Step::UserUTraceBench(TPrimaryFilter aF1, TInt aType, TBool aUID, TBool aContext,TBool aPC, TInt& aTime)
	{
	TInt count = 0;
	RTimer timer;
	TRequestStatus status;
	TInt err = KErrNone;
	TBool logged = EFalse;
	err = timer.CreateLocal();
	if(err != KErrNone)
		return err;

	timer.After(status,1);
	User::WaitForRequest(status);
	timer.After(status,1000000);

	TTraceSchema format = 3;


	if(aType == 0)
		if(aUID>0)
			do
				{
	//			UTrace trace(aF1, KBinary, aUID, aContext, aPC);
	//			logged = trace.TraceB();
				logged = TUTrace::Trace(aF1,aUID,format,aContext,aPC);
				++count;
				}
			while(status==KRequestPending);
		else
			do
				{
	//			UTrace trace(aF1, KBinary, aUID, aContext, aPC);
	//			logged = trace.TraceB();
				logged = TUTrace::TracePrimary(aF1,format,aContext,aPC);
				++count;
				}
			while(status==KRequestPending);
	if(aType == 1 )
		if(aUID > 0)
			do
				{
	//			UTrace trace(aF1, 3, aUID, aContext, aPC);
	//			logged = trace.TraceB(iA1,iA2);
				logged = TUTrace::Trace(aF1,aUID,format,aContext,aPC,iA1,iA2);
				++count;
				}
			while(status==KRequestPending);
		else
			do
				{
//				UTrace trace(aF1, 3, aUID, aContext, aPC);
	//			logged = trace.TraceB(iA1,iA2);
				logged = TUTrace::TracePrimary(aF1,format,aContext,aPC,iA1,iA2);
				++count;
				}
			while(status==KRequestPending);
	if(aType == 2)
		if(aUID>0)
			do
				{
//				UTrace trace(aF1, 3, aUID, aContext, aPC);
//				logged = trace.TraceB(&iBuf16,sizeof(iBuf16));
				logged = TUTrace::Trace(aF1,aUID,format,aContext,aPC,&iBuf16,sizeof(iBuf16));
				++count;
				}
			while(status==KRequestPending);
		else
			do
				{
//				UTrace trace(aF1, 3, aUID, aContext, aPC);
//				logged = trace.TraceB(&iBuf16,sizeof(iBuf16));
				logged = TUTrace::TracePrimary(aF1,format,aContext,aPC,&iBuf16,sizeof(iBuf16));
				++count;
				}
			while(status==KRequestPending);
	if(aType == 3)
		if(aUID>0)
			do
				{
//				UTrace trace(aF1, 3, aUID, aContext, aPC);
//				logged = trace.PrintF("hello world");
				logged = TUTrace::Printf(aF1,aUID,aContext,aPC,"Hello world %i", 9999);
				++count;
				}
			while(status==KRequestPending);
			do
				{
//				UTrace trace(aF1, 3, aUID, aContext, aPC);
//				logged = trace.PrintF("hello world");
				logged = TUTrace::PrintfPrimary(aF1,aContext,aPC,"Hello world %i", 9999);
				++count;
				}
			while(status==KRequestPending);
	if(aType == 4)
		if(aUID > 0)
			do
				{
//				UTrace trace(aF1, 3, aUID, aContext, aPC);
//				logged = trace.PrintF(_L("My printf string 1234 = %i"), 1234);
				logged = TUTrace::Printf(aF1,aUID,aContext,aPC,KName16,9999);
				++count;
				}
			while(status==KRequestPending);
		else
			do
				{
//				UTrace trace(aF1, 3, aUID, aContext, aPC);
//				logged = trace.PrintF(_L("My printf string 1234 = %i"), 1234);
				logged = TUTrace::PrintfPrimary(aF1,aContext,aPC,KName16,9999);
				++count;
				}
			while(status==KRequestPending);
	if(aType > 4)
		count = 1;

	User::WaitForRequest(status);
	aTime = 1000000000/count;
	return logged;
	}

TBool CUTrace_User_Performance_Step::UserBTraceBench(TPrimaryFilter aF1, TInt aType, TBool aUID, TBool aContext, TBool aPC, TInt& aTime)
	{
	TInt count = 0;
	TBool logged = EFalse;
	RTimer timer;
	TRequestStatus status;
	TInt err = timer.CreateLocal();
	if(err != KErrNone)
		return err;

	timer.After(status,1);
	User::WaitForRequest(status);
	timer.After(status,1000000);

	if(aUID==false && aContext==false && aPC==false)
		{
		if(aType == 0)
			do
				{
				logged = BTrace0(aF1, 3);
				++count;
				}
			while(status==KRequestPending);
		if(aType == 1)
			do
				{
				logged = BTrace8(aF1, 3,iA1,iA2);
				++count;
				}
			while(status==KRequestPending);
		if(aType == 2)
			do
				{
				logged = BTraceBig(aF1, 3,iA1, &iBuf16,sizeof(iBuf16));
				++count;
				}
			while(status==KRequestPending);
		if(aType == 3)
			do
				{
				logged = BTraceBig(aF1, 3, iA1, &iBuf8, sizeof(iBuf8));
				++count;
				}
			while(status==KRequestPending);
		if(aType == 4)
			do
				{
				logged = BTraceBig(aF1, 3, iA1, &KAny16, sizeof(KAny16));
				++count;
				}
			while(status==KRequestPending);
		}
	if(aUID==false && aContext==false && aPC==true)
		{
		if(aType == 0)
			do
				{
				logged = BTracePc0(aF1, 3);
				++count;
				}
			while(status==KRequestPending);
		if(aType == 1)
			do
				{
				logged = BTracePc8(aF1, 3,iA1,iA2);
				++count;
				}
			while(status==KRequestPending);
		if(aType == 2)
			do
				{
				logged = BTracePcBig(aF1, 3, iA1, &iBuf16,sizeof(iBuf16));
				++count;
				}
			while(status==KRequestPending);
		if(aType == 3)
			do
				{
				logged = BTracePcBig(aF1, 3, iA1, &iBuf8, sizeof(iBuf8));
				++count;
				}
			while(status==KRequestPending);
		if(aType == 4)
			do
				{
				logged = BTracePcBig(aF1, 3, iA1, &KAny16, sizeof(KAny16));
				++count;
				}
			while(status==KRequestPending);
		}
	if(aUID==false && aContext==true && aPC==false)
		{
		if(aType == 0)
			do
				{
				logged = BTraceContext0(aF1, 3);
				++count;
				}
			while(status==KRequestPending);
		if(aType == 1)
			do
				{
				logged = BTraceContext8(aF1, 3,iA1,iA2);
				++count;
				}
			while(status==KRequestPending);
		if(aType == 2)
			do
				{
				logged = BTraceContextBig(aF1, 3, iA1, &iBuf16,sizeof(iBuf16));
				++count;
				}
			while(status==KRequestPending);
		if(aType == 3)
			do
				{
				logged = BTraceContextBig(aF1, 3, iA1, &iBuf8, sizeof(iBuf8));
				++count;
				}
			while(status==KRequestPending);
		if(aType == 4)
			do
				{
				logged = BTraceContextBig(aF1, 3, iA1, &KAny16, sizeof(KAny16));
				++count;
				}
			while(status==KRequestPending);
		}
	if(aUID==false && aContext==true && aPC==true)
		{
		if(aType == 0)
			do
				{
				logged = BTraceContextPc0(aF1, 3);
				++count;
				}
			while(status==KRequestPending);
		if(aType == 1)
			do
				{
				logged = BTraceContextPc8(aF1, 3,iA1,iA2);
				++count;
				}
			while(status==KRequestPending);
		if(aType == 2)
			do
				{
				logged = BTraceContextPcBig(aF1, 3, iA1, &iBuf16,sizeof(iBuf16));
				++count;
				}
			while(status==KRequestPending);
		if(aType == 3)
			do
				{
				logged = BTraceContextPcBig(aF1, 3, iA1, &iBuf8, sizeof(iBuf8));
				++count;
				}
			while(status==KRequestPending);
		if(aType == 4)
			do
				{
				logged = BTraceContextPcBig(aF1, 3, iA1, &KAny16, sizeof(KAny16));
				++count;
				}
			while(status==KRequestPending);
		}
	if(aUID==true && aContext==false && aPC==false)
		{
		if(aType == 0)
			do
				{
				logged = BTraceFiltered4(aF1, 3, iUid);
				++count;
				}
			while(status==KRequestPending);
		if(aType == 1)
			do
				{
				logged = BTraceFiltered12(aF1, 3,iUid, iA1,iA2);
				++count;
				}
			while(status==KRequestPending);
		if(aType == 2)
			do
				{
				logged = BTraceFilteredBig(aF1, 3, iUid, &iBuf16,sizeof(iBuf16));
				++count;
				}
			while(status==KRequestPending);
		if(aType == 3)
			do
				{
				logged = BTraceFilteredBig(aF1, 3, iUid, &iBuf8, sizeof(iBuf8));
				++count;
				}
			while(status==KRequestPending);
		if(aType == 4)
			do
				{
				logged = BTraceFilteredBig(aF1, 3, iUid, &KAny16, sizeof(KAny16));
				++count;
				}
			while(status==KRequestPending);
		}
	if(aUID==true && aContext==false && aPC==true)
		{
		if(aType == 0)
			do
				{
				logged = BTraceFilteredPc4(aF1, 3, iUid);
				++count;
				}
			while(status==KRequestPending);
		if(aType == 1)
			do
				{
				logged = BTraceFilteredPc12(aF1, 3, iUid,iA1,iA2);
				++count;
				}
			while(status==KRequestPending);
		if(aType == 2)
			do
				{
				logged = BTraceFilteredPcBig(aF1, 3, iUid, &iBuf16,sizeof(iBuf16));
				++count;
				}
			while(status==KRequestPending);
		if(aType == 3)
			do
				{
				logged = BTraceFilteredPcBig(aF1, 3, iUid, &iBuf8, sizeof(iBuf8));
				++count;
				}
			while(status==KRequestPending);
		if(aType == 4)
			do
				{
				logged = BTraceFilteredPcBig(aF1, 3, iUid, &KAny16, sizeof(KAny16));
				++count;
				}
			while(status==KRequestPending);
		}
	if(aUID==true && aContext==true && aPC==false)
		{
		if(aType == 0)
			do
				{
				logged = BTraceFilteredContext4(aF1, 3, iUid);
				++count;
				}
			while(status==KRequestPending);
		if(aType == 1)
			do
				{
				logged = BTraceFilteredContext12(aF1, 3, iUid,iA1,iA2);
				++count;
				}
			while(status==KRequestPending);
		if(aType == 2)
			do
				{
				logged = BTraceFilteredContextBig(aF1, 3, iUid, &iBuf16,sizeof(iBuf16));
				++count;
				}
			while(status==KRequestPending);
		if(aType == 3)
			do
				{
				logged = BTraceFilteredContextBig(aF1, 3, iUid, &iBuf8, sizeof(iBuf8));
				++count;
				}
			while(status==KRequestPending);
		if(aType == 4)
			do
				{
				logged = BTraceFilteredContextBig(aF1, 3, iUid, &KAny16, sizeof(KAny16));
				++count;
				}
			while(status==KRequestPending);
		}
	if(aUID==true && aContext==true && aPC==true)
		{
		if(aType == 0)
			do
				{
				logged = BTraceFilteredContext4(aF1, 3, iUid);
				++count;
				}
			while(status==KRequestPending);
		if(aType == 1)
			do
				{
				logged = BTraceFilteredContext12(aF1, 3, iUid,iA1,iA2);
				++count;
				}
			while(status==KRequestPending);
		if(aType == 2)
			do
				{
				logged = BTraceFilteredContextPcBig(aF1, 3, iUid, &iBuf16,sizeof(iBuf16));
				++count;
				}
			while(status==KRequestPending);
		if(aType == 3)
			do
				{
				logged = BTraceFilteredContextBig(aF1, 3, iUid, &iBuf8, sizeof(iBuf8));
				++count;
				}
			while(status==KRequestPending);
		if(aType == 4)
			do
				{
				logged = BTraceFilteredContextBig(aF1, 3, iUid, &KAny16, sizeof(KAny16));
				++count;
				}
			while(status==KRequestPending);
		}
		if(aType > 4)
			count = 1;


	User::WaitForRequest(status);
	aTime = 1000000000/count;
	return logged;
	}


