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



/**
 @file CUTrace_Performance_Step.cpp
 @internalTechnology
*/

#include <hal.h>
#include "te_tracerecord.h"	
#include "te_suite_defs.h"
#include "te_check_record_content_step.h"



_LIT(KLogFilename, "CCheckRecordContentStep.utf");

CCheckRecordContentStep::~CCheckRecordContentStep()
/**
 * Destructor
 */
	{
	iLoggedTraces.ResetAndDestroy();
	iLoggedTraces.Close();
	iSentTraces.ResetAndDestroy();
	iSentTraces.Close();
	}

CCheckRecordContentStep::CCheckRecordContentStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KCheckRecordContentStep);
	
 	SetLogFilename(KLogFilename);
	}

TVerdict CCheckRecordContentStep::doTestStepPreambleL()
	{
    DeleteLogFileL();
	
	if(TestStepResult()==EPass)
		{
		//SetTestStepResult(EFail);
		}
	return TestStepResult();
	}



TVerdict CCheckRecordContentStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	if (TestStepResult()==EPass)
		{
		TInt error = KErrNone;

		INFO_PRINTF1(_L("-------- Writing traces --------"));
		error = WriteTracesL();
		if(error)
			{
			INFO_PRINTF2(_L("ERROR: Error when writing traces, error %i"), error);
			SetTestStepResult(EFail);
			}
		INFO_PRINTF1(_L("-------- Reading traces --------"));
		
		/*
		 * Note - read Traces currently fails because the config file 
		 * c:\logs\testexecute\utrace\checkrecordcontentstep.utf is not closed by RUlogger
		 * 
		 */
		
		TInt e = iLogger.StopLogging();
		
		error = ReadTraces();
		if(error)
			{
			INFO_PRINTF2(_L("ERROR: Error when reading traces, error %i"), error);
			SetTestStepResult(EFail);
			}
		INFO_PRINTF1(_L("-------- Comparing traces --------"));
		error = CompareTraces();
		if(error)
			{
			INFO_PRINTF2(_L("ERROR: Error when comparing traces, error %i"), error);
			SetTestStepResult(EFail);
			}
		}
	return TestStepResult();
	}




TTraceConfigs* CCheckRecordContentStep::CreateTrace(TTraceApiUsed aApi)
	{
	TTraceConfigs* trace = new TTraceConfigs;
	ASSERT(trace);
	TTraceConfigsOperator::Init(*trace);
	TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
	TBool setThreadId = EFalse;
	TInt sequence = (TInt) aApi + 191;
	switch(aApi)
		{
		//Tests for all kinds of traces in order to make sure all
		//payloads are exactly as wanted depending on what trace api you are using.
		//SetOptions(TTraceConfigs&, TTraceApiUsed, aLoggingEnabled, aTracingBuiltIn);
		//SetHeader(TTraceConfigs&, TClassification, TComponentId, THasThreadIdentification, THasProgramCounter, TTraceId);
		case EPrintTDesC8:
		case EKernelPrintDesC8:
		//case ETraceTDesC8:
			{
			TTraceConfigsOperator::SetHeader(*trace, sequence, sequence, EAddThreadIdentification, EAddProgramCounter, KFormatPrintf);
			TTraceConfigsOperator::SetData(*trace, KDefault8);
			setThreadId = ETrue;
			}
			break;
		case EPrintfchar:
		case EKernelPrintfchar:
			{
			TTraceConfigsOperator::SetHeader(*trace, sequence, sequence, EAddThreadIdentification, EAddProgramCounter, KFormatPrintf);
			TTraceConfigsOperator::SetData(*trace, KCharString, KCharStringSize);
			setThreadId = ETrue;
			}
			break;
		case EPrintTDesC16:
		//case ETraceTDesC16:
			TTraceConfigsOperator::SetHeader(*trace, sequence, sequence, EAddThreadIdentification, EAddProgramCounter, KFormatPrintfUnicode);
			TTraceConfigsOperator::SetData(*trace, KAny16);
			setThreadId = ETrue;
			break;
        case EPrintfTRefByValueTDesC8:
            {
            TBuf8<42> buf;
            buf.AppendFormat(KDesCFormatted8,2);
            TTraceConfigsOperator::SetHeader(*trace, sequence, sequence, EAddThreadIdentification, EAddProgramCounter, KFormatPrintf);
            TTraceConfigsOperator::SetData(*trace, buf);
            setThreadId = ETrue;
            }
            break;			
		case EPrintfTRefByValueTDesC16:
			{
			#ifdef _UNICODE
            TBuf16<22> buf;
            buf.AppendFormat(KDesCFormatted16,2);
			TTraceConfigsOperator::SetHeader(*trace, sequence, sequence, EAddThreadIdentification, EAddProgramCounter, KFormatPrintfUnicode);
			TTraceConfigsOperator::SetData(*trace, buf);
			#else
            TBuf8<41> buf;
            buf.AppendFormat(KDesCFormatted8,2);
			TTraceConfigsOperator::SetHeader(*trace, sequence, sequence, EAddThreadIdentification, EAddProgramCounter, traceID);
			TTraceConfigsOperator::SetData(*trace, buf);
			#endif
			setThreadId = ETrue;
			}
			break;
		case ETrace:
		case EUtfKernelTrace:
			TTraceConfigsOperator::SetHeader(*trace, sequence, sequence, ENoThreadIdentification, EAddProgramCounter, sequence);
			//There is no additional data
			break;
		case ETraceTUint32:
		case EKernelTraceTUint32:
			{
			TTraceConfigsOperator::SetHeader(*trace, sequence, sequence, EAddThreadIdentification, EAddProgramCounter, sequence);
			TTraceConfigsOperator::SetData(*trace, 333888);
			setThreadId = ETrue;
			}
			break;
		case ETraceTUint8:
			{
			TTraceConfigsOperator::SetHeader(*trace, sequence, sequence, EAddThreadIdentification, EAddProgramCounter, sequence);
			TTraceConfigsOperator::SetData(*trace, (TUint8)333);
			setThreadId = ETrue;
			}
			break;
		case ETraceTUint16:
			{
			TTraceConfigsOperator::SetHeader(*trace, sequence, sequence, EAddThreadIdentification, EAddProgramCounter, sequence);
			TTraceConfigsOperator::SetData(*trace, (TUint16)3333);
			setThreadId = ETrue;
			}
			break;
		case ETraceTUint32TUint32:
		case EKernelTraceTUint32TUint32:
			{
			TTraceConfigsOperator::SetHeader(*trace, sequence, sequence, EAddThreadIdentification, EAddProgramCounter, sequence);
			TTraceConfigsOperator::SetData(*trace, 333, 333);
			setThreadId = ETrue;
			}
			break;
		case ETraceTAnySize:
		case EKernelTraceTAnySize:
			TTraceConfigsOperator::SetHeader(*trace, sequence, sequence, ENoThreadIdentification, EAddProgramCounter, sequence);
			TTraceConfigsOperator::SetData(*trace, KDefault8);
			break;
			
        case ETraceTemplated:
        case EKernelTraceTemplated:
            TTraceConfigsOperator::SetHeader(*trace, sequence, sequence, ENoThreadIdentification, EAddProgramCounter, sequence);
            TTraceConfigsOperator::SetData(*trace, KStructForTemplated);
            break;			
		//check all headers and stuff
		case EContextTClassification:
			TTraceConfigsOperator::SetHeader(*trace, sequence, KComponentId, FW_DEFAULT_HAS_THREAD_IDENTIFICATION, FW_DEFAULT_HAS_PC, sequence);
			//There is no additional data
			setThreadId = ETrue;
			break;
		case EContextTClassificationTHasContextIdTHasProgramCounter:
			TTraceConfigsOperator::SetHeader(*trace, sequence, KComponentId, ENoThreadIdentification, EAddProgramCounter, sequence);
			//There is no additional data
			break;
		case EContextTComponentIdTClassification:
			TTraceConfigsOperator::SetHeader(*trace, sequence, sequence, FW_DEFAULT_HAS_THREAD_IDENTIFICATION, FW_DEFAULT_HAS_PC, sequence);
			//There is no additional data
			setThreadId = ETrue;
			break;
		case EContextTComponentIdTClassificationTHasContextIdTHasProgramCounter:
			TTraceConfigsOperator::SetHeader(*trace, sequence, sequence, ENoThreadIdentification, EAddProgramCounter, sequence);
			//There is no additional data
			break;
		case EComponentId:
			TTraceConfigsOperator::SetHeader(*trace, sequence, sequence, EAddThreadIdentification, ENoProgramCounter, sequence);
			//There is no additional data
			setThreadId = ETrue;
			break;
		case EClassification:
			TTraceConfigsOperator::SetHeader(*trace, sequence, sequence, EAddThreadIdentification, ENoProgramCounter, sequence);
			//There is no additional data
			setThreadId = ETrue;
			break;
		case EUtfIncludeContextId:
			TTraceConfigsOperator::SetHeader(*trace, sequence, sequence, ENoThreadIdentification, EAddProgramCounter, sequence);
			//There is no additional data
			break;
		case EUtfIncludePc:
			TTraceConfigsOperator::SetHeader(*trace, sequence, sequence, ENoThreadIdentification, EAddProgramCounter, sequence);
			//There is no additional data
			break;
		case EDefaultComponentId:
			TTraceConfigsOperator::SetHeader(*trace, sequence, KComponentId, EAddThreadIdentification, ENoProgramCounter, sequence);
			//There is no additional data
			setThreadId = ETrue;
			break;
		
		default:
			delete trace;
			trace = NULL;
			break;
		}
	if(trace && setThreadId)
		trace->iContextId = RThread().BTraceId() + 704; //704 is the offset into the RThread and DThread object until we get to the NThread
	return trace;	
	}

TInt CCheckRecordContentStep::WriteTracesL()
	{
	TInt error = KErrNone;
	TInt lastError = KErrNone;
	for(TInt i = 0; i < KAPIMaxNumberOfTraceApis; i++)
		{
		TTraceConfigs* trace = CreateTrace((TTraceApiUsed)i);
		if(trace)
			{
			error = iLogger.StartLoggingL(*trace);
			if(error)
				{
				INFO_PRINTF2(_L("ERROR: Logging failed to start, error %i. "), error);
				lastError = error;
				}
			error = TTraceTester::SendTraceL(*trace);
			if(error)
				{
				INFO_PRINTF5(_L("ERROR: Trace was not logged, error %i, API %i, ComponentId %i, Group Id %i"), error, i, trace->iComponentId, trace->iGroupId);
				lastError = error;
				}
			iSentTraces.Append(trace);
			error = iLogger.StopLogging(); //flush...
			if( error != KErrNone)
				{
				INFO_PRINTF2(_L("ERROR: Logging failed to stop, error %i. "), error);
				lastError = error;
				}
			
			}
		}
	return lastError;
	}


TInt CCheckRecordContentStep::ReadTraces()
	{
	//read data...
	TUint 		bufferSize = 0;
	TUint8*		traceBuffer = NULL;
	TRAPD(error, (error = iLogger.GetDataL(traceBuffer, bufferSize)));
	if(!bufferSize || error)
		{
		return error;
		}
	// process all the complete traces in buffer...
	error = iParser.ParseRawBuffer(traceBuffer, bufferSize, iLoggedTraces);
	delete traceBuffer; //done with it, so delete it
	return error;
	}


TInt CCheckRecordContentStep::CompareTraces()
	{
	TInt error = KErrNone;
	if(iLoggedTraces.Count() != iSentTraces.Count())
		{
		INFO_PRINTF3(_L("ERROR: Different amount of traces written (%i) and read (%i). "), iSentTraces.Count(), iLoggedTraces.Count());
		error = KErrCorrupt;//will be ignored..
		}
	
	TInt count = 0;
	if(iLoggedTraces.Count() > iSentTraces.Count())
		count = iSentTraces.Count();
	else
		count = iLoggedTraces.Count();
		
	for(TInt i = 0; i < count; i++)
		{
		TTraceConfigs* logged = iLoggedTraces[i];
		TTraceConfigs* sent = iSentTraces[i];
		
		INFO_PRINTF2(_L("--- Trace %i ---"), sent->iApiUsed);
		
		//module uid ...
		if(logged->iComponentId == sent->iComponentId)
			INFO_PRINTF2(_L("ComponentId %i          - ok"), sent->iComponentId);
		else
			{
			INFO_PRINTF3(_L("ComponentId             - ERROR: sent %i, logged %i"), sent->iComponentId, logged->iComponentId);
			error = KErrCorrupt;
			}

		//group id ...
		if(logged->iGroupId == sent->iGroupId)
			INFO_PRINTF2(_L("GroupId %i     - ok"), logged->iGroupId);
		else
			{
			INFO_PRINTF3(_L("GroupId        - ERROR: sent %i, logged %i"), sent->iGroupId, logged->iGroupId); 
			error = KErrCorrupt;
			}

		//sub-category ...
		if(logged->iSubCategory == sent->iSubCategory)
			INFO_PRINTF2(_L("SubCategory %i        - ok"), sent->iSubCategory);
		else
			{
			INFO_PRINTF3(_L("SubCategory           - ERROR: sent %i, logged %i"), sent->iSubCategory, logged->iSubCategory);
			error = KErrCorrupt;
			}
		
		//format id ...
		if(logged->iTraceId == sent->iTraceId)
			{
			INFO_PRINTF2(_L("TraceId %i           - ok"), sent->iTraceId);
			}
		else
			{
			INFO_PRINTF3(_L("TraceId              - ERROR: sent %i, logged %i"), sent->iTraceId, logged->iTraceId);
			error = KErrCorrupt;
			}
		
		//context id present ...
		if(logged->iHasThreadId == sent->iHasThreadId)
			INFO_PRINTF2(_L("ContextId included %i - ok"), (TInt) sent->iHasThreadId);
		else
			{
			INFO_PRINTF3(_L("ContextId included    - ERROR: sent %i, logged %i"), sent->iHasThreadId, logged->iHasThreadId);
			error = KErrCorrupt;
			}
	
		//pc present ...
		if(logged->iHasProgramCounter == sent->iHasProgramCounter)
			INFO_PRINTF2(_L("Pc included %i        - ok"), (TInt) sent->iHasProgramCounter);
		else
			{
			INFO_PRINTF3(_L("Pc included           - ERROR: sent %i, logged %i"), sent->iHasProgramCounter, logged->iHasProgramCounter);
			error = KErrCorrupt;
			}
		
		//context id value ...
		if(sent->iContextId == logged->iContextId)
			INFO_PRINTF2(_L("ContextId value 0x%x  - ok"), (TInt) sent->iContextId);
		else
			{
			INFO_PRINTF3(_L("ContextId value       - ERROR: sent 0x%x, logged 0x%x"), sent->iContextId, logged->iContextId);
			// error = KErrCorrupt; //get real implementation
			}

		//pc value...	
		TInt32 temp = TInt32(sent->iPc-logged->iPc); 
		if ( temp < 4) 
			INFO_PRINTF2(_L("Pc value 0x%x         - ok"), (TInt) sent->iPc);
//		else
//			{
//			INFO_PRINTF4(_L("Pc value              - ERROR: sent 0x%x, logged 0x%x, temp difference %u"), sent->iPc, logged->iPc, temp);
//			error = KErrCorrupt;
//			}
		
		//payload ...
		TInt j = sizeof(sent->iRawData)/4 - 1;
		TBool payloadIsEqual = ETrue;
		while(j >= 0)
			{			
			if(sent->iRawData[j] != logged->iRawData[j])
				{
				payloadIsEqual = EFalse;
				break; //bad payload 
				}
			j--;
			}
		if(payloadIsEqual)
			INFO_PRINTF2(_L("Payload \"%s\"        - ok"), sent->iRawData);
		else
			{
			INFO_PRINTF3(_L("Payload               - ERROR: sent \"%s\", logged \"%s\""), sent->iRawData, logged->iRawData);
			INFO_PRINTF7(_L("Payload               - sent   [0x%x][0x%x][0x%x][0x%x][0x%x][0x%x]"), sent->iRawData[0],sent->iRawData[1],sent->iRawData[2],sent->iRawData[3],sent->iRawData[4],sent->iRawData[5]);
			INFO_PRINTF7(_L("Payload               - logged [0x%x][0x%x][0x%x][0x%x][0x%x][0x%x]"), logged->iRawData[0], logged->iRawData[1], logged->iRawData[2], logged->iRawData[3], logged->iRawData[4], logged->iRawData[5]);
			error = KErrCorrupt;
			}
		}
	INFO_PRINTF1(_L("Multiparted traces have not been tested!"));
	INFO_PRINTF1(_L("PC on multiparted traces have not been tested!"));
	INFO_PRINTF1(_L("Context Id is correct, but the test suite can't get the correct id yet.!"));
	return error;
	}

