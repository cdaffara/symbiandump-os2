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
 @file te_check_performance_step.cpp
 @internalTechnology
*/

#include <hal.h>
#include "te_suite_defs.h"
#include "te_check_performance_step.h"
#include "te_logger.h"
#include "te_tracerecord.h"
#include "te_tracer.h"
#include "te_trace_for_lauterbach.h"


_LIT(KLogFilename, "CCheckPerformanceStep.utf");

CCheckPerformanceStep::~CCheckPerformanceStep()
/**
 * Destructor
 */
	{
	}

CCheckPerformanceStep::CCheckPerformanceStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KCheckPerformanceStep);

 	SetLogFilename(KLogFilename);
	}

TVerdict CCheckPerformanceStep::doTestStepPreambleL()
	{
	DeleteLogFileL();
	
	if(TestStepResult()==EPass)
		{
		//SetTestStepResult(EFail);
		}
	return TestStepResult();
	}



TVerdict CCheckPerformanceStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	if (TestStepResult()==EPass)
		{
		//set up the trace record(s)
		TTraceConfigs trace;
		TTraceConfigsOperator::Init(trace);
		TTraceConfigsOperator::SetOptions(trace, EPrintTDesC8, ETrue, ETrue);
		TTraceConfigsOperator::SetHeader(trace, TRACE_FATAL, 0, EAddThreadIdentification, ENoProgramCounter, 0);
		TTraceConfigsOperator::SetData(trace, KData1, KData2, KAny1, KAny8);
		TTraceConfigsOperator::SetData(trace, KCharString, KCharStringSize);
		trace.iDesC16.Copy(KAny16);
		StartLoggingL(trace);

		//Set up the time variables
		TUint32 baseline = 0;
		TUint32 templatebaseline = 0;
		TUint32 time = 0;
		TUint32 count = 0;

		INFO_PRINTF1(_L("---Baseline figures, user side---"));

#ifdef __WINS__
		INFO_PRINTF1(_L("WARNING these are emulator figures !! - Not Valid"));
#endif

		
		
		//Get some baseline figures
		//To see the variation I call it several times...
		trace.iApiUsed = EReferenceBaseline;
		TTraceTester::TimeTraceL(trace, baseline, count);
		INFO_PRINTF3(_L(" %i ns, %i traces/us - Baseline"), baseline, count);
		TTraceTester::TimeTraceL(trace, templatebaseline, count);
		INFO_PRINTF3(_L(" %i ns, %i traces/us - Baseline, variation"), baseline, count);
		TTraceTester::TimeTraceL(trace, baseline, count);
		INFO_PRINTF3(_L(" %i ns, %i traces/us - Baseline, variation"), baseline, count);
		TTraceTester::TimeTraceL(trace, baseline, count);
		INFO_PRINTF3(_L(" %i ns, %i traces/us - Baseline, variation"), baseline, count);
		TTraceTester::TimeTraceL(trace, baseline, count);
		INFO_PRINTF3(_L(" %i ns, %i traces/us - Baseline, variation"), baseline, count);
		TTraceTester::TimeTraceL(trace, baseline, count);
		INFO_PRINTF3(_L(" %i ns, %i traces/us - Baseline, variation"), baseline, count);
		TTraceTester::TimeTraceL(trace, baseline, count);
		INFO_PRINTF3(_L(" %i ns, %i traces/us - Baseline, variation"), baseline, count);


		trace.iApiUsed = EReferenceForLoop;
		TTraceTester::TimeTraceL(trace, time, count);
		INFO_PRINTF3(_L(" %i ns, %i traces/us - For loop, no baseline"), time, count);
		TTraceTester::TimeTraceL(trace, time, count);
		INFO_PRINTF3(_L(" %i ns, %i traces/us - For loop, no baseline, variation"), time, count);
		TTraceTester::TimeTraceL(trace, time, count);
		INFO_PRINTF3(_L(" %i ns, %i traces/us - For loop, no baseline, variation"), time, count);

		trace.iApiUsed = EReferenceInlinedEmpty;
		TTraceTester::TimeTraceL(trace, templatebaseline, count);
		INFO_PRINTF3(_L(" %i ns, %i traces/us - InlinedEmpty as baseline"), templatebaseline, count);
		TTraceTester::TimeTraceL(trace, templatebaseline, count);
		INFO_PRINTF3(_L(" %i ns, %i traces/us - InlinedEmpty as baseline"), templatebaseline, count);

		trace.iApiUsed = EReferenceExportedEmpty;
		TTraceTester::TimeTraceL(trace, templatebaseline, count);
		INFO_PRINTF3(_L(" %i ns, %i traces/us - ExportedEmpty as baseline"), templatebaseline, count);
		TTraceTester::TimeTraceL(trace, templatebaseline, count);
		INFO_PRINTF3(_L(" %i ns, %i traces/us - ExportedEmpty as baseline"), templatebaseline, count);


		INFO_PRINTF1(_L("---Reference tests---"));
		//Check inlined vs exported
		//reference for kernel and user side...
		trace.iApiUsed = EReferenceForLoop;
		TTraceTester::TimeTraceL(trace, time, count);
		time = time - baseline;
		INFO_PRINTF3(_L(" %i ns, %i traces/us - for(loop)"), time, count);

		//inlined/exported
		trace.iApiUsed = EReferenceInlinedEmpty;
		TTraceTester::TimeTraceL(trace, time, count);
		time = time - baseline;
		INFO_PRINTF3(_L(" %i ns, %i traces/us - InlinedEmpty()"), time, count);

		trace.iApiUsed = EReferenceInlined;
		TTraceTester::TimeTraceL(trace, time, count);
		time = time - baseline;
		INFO_PRINTF3(_L(" %i ns, %i traces/us - Inlined()"), time, count);

		trace.iApiUsed = EReferenceExportedEmpty;
		TTraceTester::TimeTraceL(trace, time, count);
		time = time - baseline;
		INFO_PRINTF3(_L(" %i ns, %i traces/us - ExportedEmpty()"), time, count);

		trace.iApiUsed = EReferenceExported;
		TTraceTester::TimeTraceL(trace, time, count);
		time = time - baseline;
		INFO_PRINTF3(_L(" %i ns, %i traces/us - Exported()"), time, count);

		trace.iApiUsed = EReferenceTraceNormal2Data;
		TTraceTester::TimeTraceL(trace, time, count);
		time = time - baseline;
		INFO_PRINTF3(_L(" %i ns, %i traces/us - OstTrace(TUint32, TUint32) no TraceId"), time, count);

		trace.iApiUsed = EReferenceTraceTraceId2Data;
		TTraceTester::TimeTraceL(trace, time, count);
		time = time - baseline;
		INFO_PRINTF3(_L(" %i ns, %i traces/us - OstTrace(TUint32, TUint32) with TraceId"), time, count);

		trace.iApiUsed = EReferenceTraceNormalAny;
		TTraceTester::TimeTraceL(trace, time, count);
		time = time - baseline;
		INFO_PRINTF3(_L(" %i ns, %i traces/us - OstTrace(TAny*, TInt) no TraceId"), time, count);

		trace.iApiUsed = EReferenceTraceTraceIdAny;
		TTraceTester::TimeTraceL(trace, time, count);
		time = time - baseline;
		INFO_PRINTF3(_L(" %i ns, %i traces/us - OstTrace(TAny*, TInt) with TraceId"), time, count);

		//Check some real traces
		INFO_PRINTF1(_L("---Ost, user side---"));
		trace.iApiUsed = EPerformanceBaseline;
		TTraceTester::TimeTraceL(trace, baseline, count);
		//INFO_PRINTF3(_L(" %i ns, %i traces/us - Baseline"), baseline, count);

		trace.iApiUsed = ETrace;
		TTraceTester::TimeTraceL(trace, time, count);
		time -= baseline;
		INFO_PRINTF3(_L(" %i ns, %i traces/us - static OstTrace();"), time, count);

		trace.iApiUsed = BTraceFilteredContext8;
		TTraceTester::TimeTraceL(trace, time, count);
		time -= baseline;
		INFO_PRINTF3(_L(" %i ns, %i traces/us - BTraceFilteredContext8 = OstTrace()"), time, count);

		trace.iApiUsed = ETraceTUint32;
		TTraceTester::TimeTraceL(trace, time, count);
		time = time - baseline;
		INFO_PRINTF3(_L(" %i ns, %i traces/us - OstTrace(TUint32)"), time, count);

		trace.iApiUsed = BTraceFilteredContextPc12;
		TTraceTester::TimeTraceL(trace, time, count);
		time -= baseline;
		INFO_PRINTF3(_L(" %i ns, %i traces/us - BTraceFilteredContextPc12 = OstTrace(TUint32)"), time, count);

		trace.iApiUsed = ETraceTUint32TUint32;
		TTraceTester::TimeTraceL(trace, time, count);
		time = time - baseline;
		INFO_PRINTF3(_L(" %i ns, %i traces/us - OstTrace(TUint32, TUint32)"), time, count);

		trace.iApiUsed = ETraceTAnySize;
		TTraceTester::TimeTraceL(trace, time, count);
		time = time - baseline;
		INFO_PRINTF3(_L(" %i ns, %i traces/us - OstTrace(TAny, aSize)"), time, count);

		trace.iApiUsed = BTraceFilteredContextPcBig;
		TTraceTester::TimeTraceL(trace, time, count);
		time = time - baseline;
		INFO_PRINTF3(_L(" %i ns, %i traces/us - BTraceFilteredContextPcBig = OstTrace(TAny, aSize)"), time, count);

		trace.iApiUsed = EPrintfTRefByValueTDesC16;
		TTraceTester::TimeTraceL(trace, time, count);
		time = time - baseline;
		INFO_PRINTF3(_L(" %i ns, %i traces/us - OstPrintf(TRefByValue<TDesC16>)"), time, count);

		trace.iApiUsed = EPrintfchar;
		TTraceTester::TimeTraceL(trace, time, count);
		time = time - baseline;
		INFO_PRINTF3(_L(" %i ns, %i traces/us - OstPrintf(char*)"), time, count);

	//context creation
		INFO_PRINTF1(_L("---TTraceContext alloations---"));
		//first call the Trace only call
		trace.iApiUsed = ETrace;
		TUint32 timeTraceOnly = 0;
		TTraceTester::TimeTraceL(trace, timeTraceOnly, count);
		timeTraceOnly -= baseline;

		//call the context stuff, then subtrace the time it took to make the call
		trace.iApiUsed = EDefaultComponentId;
		TUint32 timeIncTrace = 0;
		TTraceTester::TimeTraceL(trace, timeIncTrace, count);
		timeIncTrace -= baseline;
		time = timeIncTrace - timeTraceOnly;
		INFO_PRINTF3(_L(" %i ns, %i traces/us - DefaultComponentId()"), time, count);

		//call the context stuff, then subtrace the time it took to make the call
		trace.iApiUsed = EContextTClassification;
		TTraceTester::TimeTraceL(trace, timeIncTrace, count);
		timeIncTrace -= baseline;
		time = timeIncTrace - timeTraceOnly;
		INFO_PRINTF3(_L(" %i ns, %i traces/us - TTraceContext(TClassification) (using DefaultComponentId())"), time, count);

		trace.iApiUsed = EContextTClassificationTHasContextIdTHasProgramCounter;
		TTraceTester::TimeTraceL(trace, timeIncTrace, count);
		timeIncTrace -= baseline;
		time = timeIncTrace - timeTraceOnly;
		INFO_PRINTF3(_L(" %i ns, %i traces/us - TTraceContext(TClassification, THasThreadId, THasPC) (using DefaultComponentId())"), time, count);

		trace.iApiUsed = EContextTComponentIdTClassification;
		TTraceTester::TimeTraceL(trace, timeIncTrace, count);
		timeIncTrace -= baseline;
		time = timeIncTrace - timeTraceOnly;
		INFO_PRINTF3(_L(" %i ns, %i traces/us - TTraceContext(TComponentId, TClassification)"), time, count);

		trace.iApiUsed = EContextTComponentIdTClassificationTHasContextIdTHasProgramCounter;
		TTraceTester::TimeTraceL(trace, timeIncTrace, count);
		timeIncTrace -= baseline;
		time = timeIncTrace - timeTraceOnly;
		INFO_PRINTF3(_L(" %i ns, %i traces/us - TTraceContext(TComponentId, TClassification, THasThreadId, THasPC)"), time, count);

	//kernel traces
		/*INFO_PRINTF1(_L("---Kernel side, Baseline figures---"));
		trace.iApiUsed = EPerformanceKernelBaseline;
		TTraceTester::TimeTraceL(trace, baseline, count);
		INFO_PRINTF3(_L(" %i ns, %i traces/us - baseline"), baseline, count);
		TUint32 baseline2 = 0;
		TTraceTester::TimeTraceL(trace, baseline2, count);
		INFO_PRINTF3(_L(" %i ns, %i traces/us - baseline to see variation"), baseline2, count);
		TUint32 baseline3 = 0;
		TTraceTester::TimeTraceL(trace, baseline3, count);
		INFO_PRINTF3(_L(" %i ns, %i traces/us - baseline to see variation"), baseline3, count);
		TInt smallest = 0;
		TInt biggest = 0;
		//Very ugly, but just temp thing...
		//find the smallest baseline
		if(baseline < baseline2)
			if(baseline < baseline3)
				smallest = baseline;
			else
				smallest = baseline3;
		else
			if(baseline2 < baseline3)
				smallest = baseline2;
			else
				smallest = baseline3;
		//find the largest baseline
		if(baseline > baseline2)
			if(baseline > baseline3)
				biggest = baseline;
			else
				biggest = baseline3;
		else
			if(baseline2 > baseline3)
				biggest = baseline2;
			else
				biggest = baseline3;

		INFO_PRINTF4(_L("Variation of baselines from %i - %i, diff %i ns"), smallest, biggest, (biggest - smallest));

		trace.iApiUsed = EReferenceKernelForLoop;
		TTraceTester::TimeTraceL(trace, time, count);
		INFO_PRINTF3(_L(" %i ns, %i traces/us - for loop - no baseline"), time, count);
		TTraceTester::TimeTraceL(trace, time, count);
		INFO_PRINTF3(_L(" %i ns, %i traces/us - for loop - no baseline"), time, count);
		TTraceTester::TimeTraceL(trace, time, count);
		INFO_PRINTF3(_L(" %i ns, %i traces/us - for loop - no baseline"), time, count);

		INFO_PRINTF1(_L("---Ost calls, kernel side---"));

		trace.iApiUsed = EUtfKernelTrace;
		TTraceTester::TimeTraceL(trace, time, count);
		time -= baseline;
		INFO_PRINTF3(_L(" %i ns, %i traces/us - TUTraceNormal::Trace()"), time, count);

		trace.iApiUsed = BTraceKernelFilteredContext8;
		TTraceTester::TimeTraceL(trace, time, count);
		time -= baseline;
		INFO_PRINTF3(_L(" %i ns, %i traces/us - BTraceKernelFilteredContext8, equivalent of TUTraceNormal::Trace()"), time, count);

		trace.iApiUsed = EKernelTraceTTemplated;
		TTraceTester::TimeTraceL(trace, time, count);
		time -= baseline;
		INFO_PRINTF3(_L(" %i ns, %i traces/us - TUTraceNormal::Trace(const T&)"), time, count);
		*/
		StopLogging();
		}
	return TestStepResult();
	}


TInt CCheckPerformanceStep::FindLargestAndSmallest(RArray<TUint32> /*aList*/, TInt& /*aLargest*/, TInt& /*aSmallest*/)
	{
	return KErrNotSupported;
	}


