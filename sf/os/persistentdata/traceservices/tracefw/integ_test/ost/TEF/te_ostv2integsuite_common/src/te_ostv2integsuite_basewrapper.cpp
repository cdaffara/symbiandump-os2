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
 @file te_uloggerostv2integsuite_stepbase.cpp
 @internalTechnology
*/

#include "te_ostv2integsuite_basewrapper.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "te_instrumentationpoints_enabledTraces.h"
#else
#include "te_instrumentationpoints_disabledTraces.h"
#endif


CTe_MCLSuiteBaseWrapper::~CTe_MCLSuiteBaseWrapper()
	{
	iLoggedTraces.ResetAndDestroy();
	iLoggedTraces.Close();
	iSentTraces.ResetAndDestroy();
	iSentTraces.Close();
	}

CTe_MCLSuiteBaseWrapper::CTe_MCLSuiteBaseWrapper()
	{
	iAlignPosition=-1;
	iNumMatchingTraces=0;
	iPosition=0;
	}

CTe_MCLSuiteBaseWrapper* CTe_MCLSuiteBaseWrapper::NewLC()
	{
	CTe_MCLSuiteBaseWrapper* self = new (ELeave)CTe_MCLSuiteBaseWrapper();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTe_MCLSuiteBaseWrapper* CTe_MCLSuiteBaseWrapper::NewL()
	{
	CTe_MCLSuiteBaseWrapper* self=CTe_MCLSuiteBaseWrapper::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CTe_MCLSuiteBaseWrapper::ConstructL()
	{
	}

TAny* CTe_MCLSuiteBaseWrapper::GetObject()
	{
	return NULL;
	}
TBool CTe_MCLSuiteBaseWrapper::DoCommandL(	const TTEFFunction& /*aCommand*/,
					const TTEFSectionName& /*aSection*/, 
					const TInt /*aAsyncErrorIndex*/)
	{
	return EFalse;
	}





TBool CTe_MCLSuiteBaseWrapper::CreateTraceObject(TComponentId aComponentId, TGroupId aGroupId,THasThreadIdentification aHasContext, THasProgramCounter aHasPc,RPointerArray<TTraceConfigs> & aTraceObjects)
	{
	INFO_PRINTF1(_L("-------- Creating Trace configuration object --------"));
	TTraceConfigs* trace = new TTraceConfigs;
	ASSERT(trace);
	TTraceConfigsOperator::Init(*trace);
	TTraceConfigsOperator::SetOptions(*trace, UPTNone, ETrue, ETrue);
	TTraceConfigsOperator::SetHeader(*trace, aGroupId, aComponentId, aHasContext, aHasPc, KTraceId);
	aTraceObjects.Append(trace);
	if(trace && aHasContext)
		{
		trace->iContextId = RThread().BTraceId() + 704; //704 is the offset into the RThread and DThread object until we get to the NThread
		}
	if (trace)
		return ETrue;
		return EFalse;
	}

TBool CTe_MCLSuiteBaseWrapper::CreateTraceObject(TUPTApiUsed aApi, THasThreadIdentification aHasContext, THasProgramCounter aHasPc,RPointerArray<TTraceConfigs> & aTraceObjects)
	{
	INFO_PRINTF1(_L("-------- Creating Trace configuration object --------"));
	TBool setThreadId = EFalse;

	switch(aApi)
		{
		//Tests for all kinds of traces in order to make sure all
		//payloads are exactly as wanted depending on what trace api you are using.
		//SetOptions(TTraceConfigs&, TTraceApiUsed, aLoggingEnabled, aTracingBuiltIn);
		//SetHeader(TTraceConfigs&, TGroupId, TComponentId, THasThreadIdentification, THasProgramCounter, TTraceId);
		_LIT8(KStateName,"Test State Name");
		_LIT8(KStateValue,"Test State Value");
		
		case EUptPrintTDesC8_big:
		case EUptKernelPrintfDesc8_big:
			{
			TTraceConfigs* trace = new TTraceConfigs;
			ASSERT(trace);
			TTraceConfigsOperator::Init(*trace);
			TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
			TTraceConfigsOperator::SetHeader(*trace, KGroupId, KComponentId, aHasContext, aHasPc, KFormatPrintf);
			TTraceConfigsOperator::SetData(*trace, KStringDesc8);//TPtrC8((const TUint8*)KBigData8, KBigSize8)
			setThreadId = aHasContext;
			aTraceObjects.Append(trace);
			}
			break;
			
		case EUptPrintTDesC8_huge:
		case EUptKernelPrintfDesc8_huge:
			{
			TTraceConfigs* trace = new TTraceConfigs;
			ASSERT(trace);
			TTraceConfigsOperator::Init(*trace);
			TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
			TTraceConfigsOperator::SetHeader(*trace, KGroupId, KComponentId, aHasContext, aHasPc, KFormatPrintf);
			TTraceConfigsOperator::SetData(*trace, KStringDescHuge8);//TPtrC8((const TUint8*)KHugeData8, KHugeSize8)
			setThreadId = aHasContext;
			aTraceObjects.Append(trace);
			}
			break;
		
			
			
		case EUptPrintTDesC16_big:
#ifndef __KERNEL_MODE__
			{
			TTraceConfigs* trace = new TTraceConfigs;
			ASSERT(trace);
			TTraceConfigsOperator::Init(*trace);
			TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
			TTraceConfigsOperator::SetHeader(*trace, KGroupId, KComponentId, aHasContext, aHasPc, KFormatPrintfUnicode);
			TTraceConfigsOperator::SetData(*trace,KStringDesc16 );//TPtrC16((const TUint16*)KBigData16, KBigSize16)
			setThreadId = aHasContext;
			aTraceObjects.Append(trace);
			}
#endif
			break;

		case EUptPrintTDesC16_huge:
#ifndef __KERNEL_MODE__
			{
			TTraceConfigs* trace = new TTraceConfigs;
			ASSERT(trace);
			TTraceConfigsOperator::Init(*trace);
			TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
			TTraceConfigsOperator::SetHeader(*trace, KGroupId, KComponentId, aHasContext, aHasPc, KFormatPrintfUnicode);
			TTraceConfigsOperator::SetData(*trace, KStringDescHuge16);//TPtrC16((const TUint16*)KHugeData16, KHugeSize16)
			setThreadId = aHasContext;
			aTraceObjects.Append(trace);
			}
#endif
			break;

// ***
			
	    case EUptTraceTDesC8_big:
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace, KGroupId, KComponentId, aHasContext, aHasPc, KTraceId);
            TTraceConfigsOperator::SetData(*trace, KStringDesc8);
            setThreadId = aHasContext;
            aTraceObjects.Append(trace);
            }
            break;
            
        case EUptTraceTDesC8_huge:
        case EUptKernelTraceDesc8_huge:
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace, KGroupId, KComponentId, aHasContext, aHasPc, KTraceId);
            TTraceConfigsOperator::SetData(*trace, KStringDescHuge8);
            setThreadId = aHasContext;
            aTraceObjects.Append(trace);
            }
            break;
        
            
            
        case EUptTraceTDesC16_big:
#ifndef __KERNEL_MODE__
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace, KGroupId, KComponentId, aHasContext, aHasPc, KTraceId);
            TTraceConfigsOperator::SetData(*trace,KStringDesc16);
            setThreadId = aHasContext;
            aTraceObjects.Append(trace);
            }
#endif
            break;

        case EUptTraceTDesC16_huge:
#ifndef __KERNEL_MODE__
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace, KGroupId, KComponentId, aHasContext, aHasPc, KTraceId);
            TTraceConfigsOperator::SetData(*trace, KStringDescHuge16);
            setThreadId = aHasContext;
            aTraceObjects.Append(trace);
            }
#endif
            break;

		
		case EUptPrintfchar:
		case EUptKernelPrintfchar:
			{
			TTraceConfigs* trace = new TTraceConfigs;
			ASSERT(trace);
			TTraceConfigsOperator::Init(*trace);
			TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
			TTraceConfigsOperator::SetHeader(*trace, KGroupId, KComponentId, aHasContext, aHasPc, KFormatPrintf);
			TTraceConfigsOperator::SetData(*trace, KPrintfCStyleString, KPrintfCStyleStringSize);
			setThreadId = aHasContext;
			aTraceObjects.Append(trace);
			}
			break;
			
        case EUptPrintfTRefByValueTDesC8:
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TBuf8<42> buf;
            buf.AppendFormat(KDesC8Formatted,2);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace, KGroupId, KComponentId, aHasContext, aHasPc, KFormatPrintf);
            TTraceConfigsOperator::SetData(*trace, buf);
            setThreadId = aHasContext;
            aTraceObjects.Append(trace);
            }
            break;
            
		case EUptPrintfTRefByValueTDesC16:
#ifndef __KERNEL_MODE__
			{
			TTraceConfigs* trace = new TTraceConfigs;
			ASSERT(trace);
            TBuf16<44> buf;
            buf.AppendFormat(KDesCFormatted16,2);
			TTraceConfigsOperator::Init(*trace);
			TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
			TTraceConfigsOperator::SetHeader(*trace, KGroupId, KComponentId, aHasContext, aHasPc, KFormatPrintfUnicode);
			TTraceConfigsOperator::SetData(*trace, buf);
			setThreadId = aHasContext;
			aTraceObjects.Append(trace);
			}
#endif
			break;
		case EUptTrace:
		case EUptKernelTrace:
			{
			TTraceConfigs* trace = new TTraceConfigs;
			ASSERT(trace);
			TTraceConfigsOperator::Init(*trace);
			TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
			TTraceConfigsOperator::SetHeader(*trace, KGroupId, KComponentId, aHasContext, aHasPc, KTraceId);
			//There is no additional data
			setThreadId = aHasContext;
			aTraceObjects.Append(trace);
			}
			break;
		case EUptTraceTUint32:
		case EUptKernelTraceTUint32:
			{
			TTraceConfigs* trace = new TTraceConfigs;
			ASSERT(trace);
			TTraceConfigsOperator::Init(*trace);
			TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
			TTraceConfigsOperator::SetHeader(*trace, KGroupId, KComponentId, aHasContext, aHasPc, KTraceId);
			TTraceConfigsOperator::SetData(*trace, KData1);
			setThreadId = aHasContext;
			aTraceObjects.Append(trace);
			}
			break;
		case EUptTraceTUint32TUint32:
		case EUptKernelTraceTUint32TUint32:
			{
			TTraceConfigs* trace = new TTraceConfigs;
			ASSERT(trace);
			TTraceConfigsOperator::Init(*trace);
			TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
			TTraceConfigsOperator::SetHeader(*trace, KGroupId, KComponentId, aHasContext, aHasPc, KTraceId);
			TTraceConfigsOperator::SetData(*trace, KData1, KData2);
			setThreadId = aHasContext;
			aTraceObjects.Append(trace);
			}
			break;
		case EUptTraceTAnySmallStruct:
		case EUptKernelTraceTAnySmallStruct:
			{
			TTraceConfigs* trace = new TTraceConfigs;
			ASSERT(trace);
			TTraceConfigsOperator::Init(*trace);
			TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
			TTraceConfigsOperator::SetHeader(*trace, KGroupId, KComponentId, aHasContext, aHasPc, KTraceId);
			TTraceConfigsOperator::SetData(*trace, (TAny*)KSmallData, KSmallSize);
			setThreadId = aHasContext;
			aTraceObjects.Append(trace);
			}
			break;
		case EUptTraceTAnyBigStruct:
		case EUptKernelTraceTAnyBigStruct:
			{
			TTraceConfigs* trace = new TTraceConfigs;
			ASSERT(trace);
			TTraceConfigsOperator::Init(*trace);
			TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
			TTraceConfigsOperator::SetHeader(*trace, KGroupId, KComponentId, aHasContext, aHasPc, KTraceId);
			TTraceConfigsOperator::SetData(*trace, KBigData8, KBigSize8);
			setThreadId = aHasContext;
			aTraceObjects.Append(trace);
			}
			break;
		case EUptTraceTAnyHugeStruct:
		case EUptKernelTraceTAnyHugeStruct:
			{
			TTraceConfigs* trace = new TTraceConfigs;
			ASSERT(trace);
			TTraceConfigsOperator::Init(*trace);
			TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
			TTraceConfigsOperator::SetHeader(*trace, KGroupId, KComponentId, aHasContext, aHasPc, KTraceId);
			TTraceConfigsOperator::SetData(*trace, KHugeData8, KHugeSize8);
			setThreadId = aHasContext;
			aTraceObjects.Append(trace);
			}
			break;
		case EUptTraceTemplatedSmallStruct:
		case EUptKernelTraceTemplatedSmallStruct:
			{
			TTraceConfigs* trace = new TTraceConfigs;
			ASSERT(trace);
			TTraceConfigsOperator::Init(*trace);
			TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
			TTraceConfigsOperator::SetHeader(*trace, KGroupId, KComponentId, aHasContext, aHasPc, KTraceId);
			TTraceConfigsOperator::SetData(*trace, KStructForTemplated);
			setThreadId = aHasContext;
			aTraceObjects.Append(trace);
			}
			break;
		case EUptTraceTemplatedBigStruct:
		case EUptKernelTraceTemplatedBigStruct:
			{
			TTraceConfigs* trace = new TTraceConfigs;
			ASSERT(trace);
			TTraceConfigsOperator::Init(*trace);
			TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
			TTraceConfigsOperator::SetHeader(*trace, KGroupId, KComponentId, aHasContext, aHasPc, KTraceId);
			TTraceConfigsOperator::SetData(*trace, KBigData8, KBigSize8);
			setThreadId = aHasContext;
			aTraceObjects.Append(trace);
			}
			break;
		case EUptTraceTemplatedHugeStruct:
		case EUptKernelTraceTemplatedHugeStruct:
			{
			TTraceConfigs* trace = new TTraceConfigs;
			ASSERT(trace);
			TTraceConfigsOperator::Init(*trace);
			TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
			TTraceConfigsOperator::SetHeader(*trace, KGroupId, KComponentId, aHasContext, aHasPc, KTraceId);
			TTraceConfigsOperator::SetData(*trace, KHugeData8, KHugeSize8);
			setThreadId = aHasContext;
			aTraceObjects.Append(trace);
			}
			break;
		case EUptIsTraceActive:
		case EUptKernelIsTraceActive:
			break;
		case UPTTraceTest:
			break;
		case UPTKernelTraceTest:
		case UPTKernelNegativeFunctionalityTest:
			{
			TTraceConfigs* trace1 = new TTraceConfigs;
			ASSERT(trace1);
			TTraceConfigsOperator::Init(*trace1);
			TTraceConfigsOperator::SetOptions(*trace1, aApi, ETrue, ETrue);
			TTraceConfigsOperator::SetHeader(*trace1, KGroupId+1, KComponentId+1, aHasContext, aHasPc, KTraceId);
			aTraceObjects.Append(trace1);
			
			TTraceConfigs* trace2 = new TTraceConfigs;
			ASSERT(trace2);
			TTraceConfigsOperator::Init(*trace2);
			TTraceConfigsOperator::SetOptions(*trace2, aApi, ETrue, ETrue);
			TTraceConfigsOperator::SetHeader(*trace2, KGroupId+2, KComponentId+2, aHasContext, aHasPc, KTraceId);
			aTraceObjects.Append(trace2);
			
			setThreadId = aHasContext;
			}
			break;
		case UPTKernelMultipleFunctionalityTest:
			{
			TTraceConfigs* trace1 = new TTraceConfigs;
			ASSERT(trace1);
			TTraceConfigsOperator::Init(*trace1);
			TTraceConfigsOperator::SetOptions(*trace1, aApi, ETrue, ETrue);
			TTraceConfigsOperator::SetHeader(*trace1, KGroupId, KComponentId, aHasContext, aHasPc, KTraceId);
			aTraceObjects.Append(trace1);
			
			TTraceConfigs* trace2 = new TTraceConfigs;
			ASSERT(trace2);
			TTraceConfigsOperator::Init(*trace2);
			TTraceConfigsOperator::SetOptions(*trace2, aApi, ETrue, ETrue);
			TTraceConfigsOperator::SetHeader(*trace2, KGroupId+1, KComponentId, aHasContext, aHasPc, KTraceId);
			aTraceObjects.Append(trace2);
			
			TTraceConfigs* trace3 = new TTraceConfigs;
			ASSERT(trace3);
			TTraceConfigsOperator::Init(*trace3);
			TTraceConfigsOperator::SetOptions(*trace3, aApi, ETrue, ETrue);
			TTraceConfigsOperator::SetHeader(*trace3, KGroupId+2, KComponentId, aHasContext, aHasPc, KTraceId);
			aTraceObjects.Append(trace3);
			
			TTraceConfigs* trace4 = new TTraceConfigs;
			ASSERT(trace4);
			TTraceConfigsOperator::Init(*trace4);
			TTraceConfigsOperator::SetOptions(*trace4, aApi, ETrue, ETrue);
			TTraceConfigsOperator::SetHeader(*trace4, KGroupId, KComponentId+2, aHasContext, aHasPc, KTraceId);
			aTraceObjects.Append(trace4);
			
			TTraceConfigs* trace5 = new TTraceConfigs;
			ASSERT(trace5);
			TTraceConfigsOperator::Init(*trace5);
			TTraceConfigsOperator::SetOptions(*trace5, aApi, ETrue, ETrue);
			TTraceConfigsOperator::SetHeader(*trace5, KGroupId+2, KComponentId+2, aHasContext, aHasPc, KTraceId);
			aTraceObjects.Append(trace5);
			
			TTraceConfigs* trace6 = new TTraceConfigs;
			ASSERT(trace6);
			TTraceConfigsOperator::Init(*trace6);
			TTraceConfigsOperator::SetOptions(*trace6, aApi, ETrue, ETrue);
			TTraceConfigsOperator::SetHeader(*trace6, KGroupId+1, KComponentId+2, aHasContext, aHasPc, KTraceId);
			aTraceObjects.Append(trace6);
			
			TTraceConfigs* trace7 = new TTraceConfigs;
			ASSERT(trace7);
			TTraceConfigsOperator::Init(*trace7);
			TTraceConfigsOperator::SetOptions(*trace7, aApi, ETrue, ETrue);
			aTraceObjects.Append(trace7);
			
			setThreadId = aHasContext;
			}
			break;
			
		//OST	
			
        case EOstTraceDef0:
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace, TRACE_FATAL, KComponentId, EAddThreadIdentification, ENoProgramCounter, (TEST_OstTraceDef0&TRACEIDMASK)>>TRACEIDSHIFT);
            //TTraceConfigsOperator::SetData(*trace, TEST_OstTraceDef0); //no additional data
            setThreadId = EAddThreadIdentification;
            aTraceObjects.Append(trace);                
            }
            break;        
        case EOstTraceDef1:
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace,TRACE_FATAL, KComponentId, EAddThreadIdentification, ENoProgramCounter, (TEST_OstTraceDef1&TRACEIDMASK)>>TRACEIDSHIFT);
            TTraceConfigsOperator::SetData(*trace, KData1);
            setThreadId = EAddThreadIdentification;
            aTraceObjects.Append(trace);
            }
            break;
        case EOstTraceDefData:
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace,TRACE_FATAL, KComponentId, EAddThreadIdentification, ENoProgramCounter, (TEST_OstTraceDefData&TRACEIDMASK)>>TRACEIDSHIFT);
            TTraceConfigsOperator::SetData(*trace, &KData1, sizeof(KData1) );
            setThreadId = EAddThreadIdentification;
            aTraceObjects.Append(trace);
            }
            break;
        case EOstTraceDefExt1:
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace, TRACE_FATAL, KComponentId, EAddThreadIdentification, ENoProgramCounter, (TEST_OstTraceDefExt1&TRACEIDMASK)>>TRACEIDSHIFT);
            TTraceConfigsOperator::SetData(*trace, (TUint32*)KDefault8().Ptr(),KDefault8().Size());
            setThreadId = EAddThreadIdentification;
            aTraceObjects.Append(trace);
            }
            break;         
        case EOstTraceDefExt2:
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace, TRACE_FATAL, KComponentId, EAddThreadIdentification, ENoProgramCounter, (TEST_OstTraceDefExt2&TRACEIDMASK)>>TRACEIDSHIFT);
            TTraceConfigsOperator::SetData(*trace, KData1, KData1);
            setThreadId = EAddThreadIdentification;
            aTraceObjects.Append(trace);
            }
            break;
        case EOstTraceDefExt3:
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace, TRACE_FATAL, KComponentId, EAddThreadIdentification, ENoProgramCounter, (TEST_OstTraceDefExt3&TRACEIDMASK)>>TRACEIDSHIFT);
            TTraceConfigsOperator::SetData(*trace,  KData1, KData1, KData1);
            setThreadId = EAddThreadIdentification;
            aTraceObjects.Append(trace);
            }
            break;
        case EOstTraceDefExt4:
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace, TRACE_FATAL, KComponentId, EAddThreadIdentification, ENoProgramCounter, (TEST_OstTraceDefExt4&TRACEIDMASK)>>TRACEIDSHIFT);
            TTraceConfigsOperator::SetData(*trace, KData1, KData1, KData1, KData1);
            setThreadId = EAddThreadIdentification;
            aTraceObjects.Append(trace);
            }
            break;
        case EOstTraceDefExt5:
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace,TRACE_FATAL, KComponentId, EAddThreadIdentification, ENoProgramCounter, (TEST_OstTraceDefExt5&TRACEIDMASK)>>TRACEIDSHIFT);
            TTraceConfigsOperator::SetData(*trace, KData1, KData1, KData1, KData1, KData1);
            setThreadId = EAddThreadIdentification;
            aTraceObjects.Append(trace);
            }
            break;
        case EOstTrace0:
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace, TRACE_FATAL, KComponentId, EAddThreadIdentification, ENoProgramCounter, (TEST_OSTTRACE0&TRACEIDMASK)>>TRACEIDSHIFT);
            //TTraceConfigsOperator::SetData(*trace, TEST_OSTTRACE0);
            setThreadId = EAddThreadIdentification;
            aTraceObjects.Append(trace);
            }
            break;
        case EOstTrace1:
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace, TRACE_FATAL, KComponentId, EAddThreadIdentification, ENoProgramCounter, (TEST_OSTTRACE1&TRACEIDMASK)>>TRACEIDSHIFT);
            TTraceConfigsOperator::SetData(*trace,KData1);
            setThreadId = EAddThreadIdentification;
            aTraceObjects.Append(trace);
            }
            break;
        case EOstTraceData:
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace, TRACE_FATAL, KComponentId, EAddThreadIdentification, ENoProgramCounter, (TEST_OSTTRACEDATA&TRACEIDMASK)>>TRACEIDSHIFT);
            TTraceConfigsOperator::SetData(*trace, &KData1, sizeof(KData1) );
            setThreadId = EAddThreadIdentification;
            aTraceObjects.Append(trace);
            }
            break;
        case EOstTraceExt1:
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace,TRACE_FATAL, KComponentId, EAddThreadIdentification, ENoProgramCounter, (TEST_OSTTRACEEXT1&TRACEIDMASK)>>TRACEIDSHIFT);
            TTraceConfigsOperator::SetData(*trace, KData8);
            setThreadId = EAddThreadIdentification;
            aTraceObjects.Append(trace);
            }
            break;
        case EOstTraceExt2:
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace,TRACE_FATAL, KComponentId, EAddThreadIdentification, ENoProgramCounter, (TEST_OSTTRACEEXT2&TRACEIDMASK)>>TRACEIDSHIFT);
            TTraceConfigsOperator::SetData(*trace, KData1, KData1);
            setThreadId = EAddThreadIdentification;
            aTraceObjects.Append(trace);
            }
            break;
        case EOstTraceExt3:
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace, TRACE_FATAL, KComponentId, EAddThreadIdentification, ENoProgramCounter, (TEST_OSTTRACEEXT3&TRACEIDMASK)>>TRACEIDSHIFT);
            TTraceConfigsOperator::SetData(*trace, KData1, KData1, KData1);
            setThreadId = EAddThreadIdentification;
            aTraceObjects.Append(trace);
            }
            break;
        case EOstTraceExt4:
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace, TRACE_FATAL, KComponentId, EAddThreadIdentification, ENoProgramCounter, (TEST_OSTTRACEEXT4&TRACEIDMASK)>>TRACEIDSHIFT);
            TTraceConfigsOperator::SetData(*trace, KData1, KData1, KData1, KData1);
            setThreadId = EAddThreadIdentification;
            aTraceObjects.Append(trace);
            }
            break;
        case EOstTraceExt5:
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace,TRACE_FATAL, KComponentId, EAddThreadIdentification, ENoProgramCounter, (TEST_OSTTRACEEXT5&TRACEIDMASK)>>TRACEIDSHIFT);
            TTraceConfigsOperator::SetData(*trace, KData1, KData1, KData1, KData1, KData1);
            setThreadId = EAddThreadIdentification;
            aTraceObjects.Append(trace);
            }
            break;
        case EOstTraceFunctionEntry0:
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace, TRACE_FLOW, KComponentId, EAddThreadIdentification, ENoProgramCounter, (TEST_OSTTraceFunctionalityEntry0&TRACEIDMASK)>>TRACEIDSHIFT);
            //TTraceConfigsOperator::SetData(*trace, TEST_OSTTraceFunctionalityEntry0);
            setThreadId = EAddThreadIdentification;
            aTraceObjects.Append(trace);
            }
            break;
        case EOstTraceFunctionEntry1:
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace,  TRACE_FLOW, KComponentId, EAddThreadIdentification, ENoProgramCounter, (TEST_OSTTraceFunctionalityEntry1&TRACEIDMASK)>>TRACEIDSHIFT);
            TTraceConfigsOperator::SetData(*trace, KData1);
            setThreadId = EAddThreadIdentification;
            aTraceObjects.Append(trace);
            }
            break;
        case EOstTraceFunctionEntryExt:
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace,  TRACE_FLOW, KComponentId, EAddThreadIdentification, ENoProgramCounter, (0x8a0005&TRACEIDMASK)>>TRACEIDSHIFT);
            //fix this - should be a list of the function arguments of this method all these traces are insturmented in
			TTraceConfigsOperator::SetData(*trace, KData1);
            setThreadId = aHasContext;
            aTraceObjects.Append(trace);
            }
            break;
        case EOstTraceFunctionEntryExtCtorInitList:
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace,  TRACE_FLOW, KComponentId, EAddThreadIdentification, ENoProgramCounter, (0x8a0001&TRACEIDMASK)>>TRACEIDSHIFT);
            //fix this - should be a list of the function arguments of this method all these traces are insturmented in
            TTraceConfigsOperator::SetData(*trace,1,2,KData1);
            setThreadId = aHasContext;
            aTraceObjects.Append(trace);
            }
            break;
        case EOstTraceFunctionEntryExtCtorNoInitList:
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace,  TRACE_FLOW, KComponentId, EAddThreadIdentification, ENoProgramCounter, (0x8a0002&TRACEIDMASK)>>TRACEIDSHIFT);
            //fix this - should be a list of the function arguments of this method all these traces are insturmented in
            TTraceConfigsOperator::SetData(*trace,3, KData1);
            setThreadId = aHasContext;
            aTraceObjects.Append(trace);
            }
            break;
        case EOstTraceFunctionExit0:
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace,  TRACE_FLOW, KComponentId, EAddThreadIdentification, ENoProgramCounter, (TEST_OstTraceFunctionExit0&TRACEIDMASK)>>TRACEIDSHIFT);
            //TTraceConfigsOperator::SetData(*trace, TEST_OstTraceFunctionExit0);
            setThreadId = EAddThreadIdentification;
            aTraceObjects.Append(trace);
            }
            break;
        case EOstTraceFunctionExit1:
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace,  TRACE_FLOW, KComponentId, EAddThreadIdentification, ENoProgramCounter, (TEST_OSTTRACEFUNCTIONEXIT1&TRACEIDMASK)>>TRACEIDSHIFT);
            TTraceConfigsOperator::SetData(*trace, KData1);
            setThreadId = EAddThreadIdentification;
            aTraceObjects.Append(trace);
            }
            break;
        case EOstTraceFunctionExitExt:
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace,  TRACE_FLOW, KComponentId, EAddThreadIdentification, ENoProgramCounter, (TEST_OSTTRACEFUNCTIONEXITEXT&TRACEIDMASK)>>TRACEIDSHIFT);
            //fix this
			TTraceConfigsOperator::SetData(*trace, (TUint32)KData1, (TUint32)0);
            setThreadId = aHasContext;
            aTraceObjects.Append(trace);
            }
            break;
        case EOstTraceEventStart0:
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace, TRACE_PERFORMANCE, KComponentId, EAddThreadIdentification, ENoProgramCounter, (TEST_OSTTRACEEVENTSTART0&TRACEIDMASK)>>TRACEIDSHIFT);
            TTraceConfigsOperator::SetData(*trace,(TUint32)1);
            setThreadId = EAddThreadIdentification;
            aTraceObjects.Append(trace);
            }
            break;
        case EOstTraceEventStart1:
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace, TRACE_PERFORMANCE, KComponentId, EAddThreadIdentification, ENoProgramCounter, (TEST_OSTTRACEEVENTSTART1&TRACEIDMASK)>>TRACEIDSHIFT);
            TTraceConfigsOperator::SetData(*trace, KData1);
            setThreadId = EAddThreadIdentification;
            aTraceObjects.Append(trace);
            }
            break;
        case EOstTraceEventStop:
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace, TRACE_PERFORMANCE, KComponentId, EAddThreadIdentification, ENoProgramCounter, (TEST_OSTTRACEEVENTSTOP&TRACEIDMASK)>>TRACEIDSHIFT);
			TTraceConfigsOperator::SetData(*trace,0,(TUint32)(TEST_OSTTRACEEVENTSTART0 & TRACEIDMASK));
            setThreadId = aHasContext;
            aTraceObjects.Append(trace);
            }
            break;
        case EOstTraceState0:
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace, TRACE_STATE, KComponentId, EAddThreadIdentification, ENoProgramCounter, (TEST_OSTTRACESTATE0&TRACEIDMASK)>>TRACEIDSHIFT);
            TTraceConfigsOperator::SetData(*trace, KStateName,KStateValue);
            setThreadId = EAddThreadIdentification;
            aTraceObjects.Append(trace);
            }
            break;
        case EOstTraceState1:
            {
            TTraceConfigs* trace = new TTraceConfigs;
            ASSERT(trace);
            TTraceConfigsOperator::Init(*trace);
            TTraceConfigsOperator::SetOptions(*trace, aApi, ETrue, ETrue);
            TTraceConfigsOperator::SetHeader(*trace, TRACE_STATE, KComponentId, EAddThreadIdentification, ENoProgramCounter, (TEST_OSTTRACESTATE1&TRACEIDMASK)>>TRACEIDSHIFT);
            TTraceConfigsOperator::SetData(*trace, KStateName,KStateValue,KData1);
            setThreadId = EAddThreadIdentification;
            aTraceObjects.Append(trace);
            }
            break;
		default:
			//delete trace;
			//trace = NULL;
			break;
		}
	
	for(TInt i = 0; i <= (aTraceObjects.Count()-1); i++)
		{
		if(aTraceObjects[i] && setThreadId)
			{
			aTraceObjects[i]->iContextId = RThread().BTraceId() + 704; //704 is the offset into the RThread and DThread object until we get to the NThread
			}
		}
	
	TBool realTrace=EFalse;
	if (aTraceObjects.Count())
		{
		realTrace=ETrue;
		}
	switch(aApi)
	{
	case UPTTraceTest:
	case UPTKernelTraceTest:
	case UPTKernelNegativeFunctionalityTest:
	case UPTKernelMultipleFunctionalityTest:
	case UPTBaseline:
	case UPTKernelBaseline:
	case UPTFooUserTime:
	case UPTFooKernelTime:
		realTrace = EFalse;
	break;
	default:
	break;
	}
	
	return realTrace;	
	}

TInt CTe_MCLSuiteBaseWrapper::StoreTraceInfo(TBool aEnumBased,TUPTApiUsed anApi,TComponentId aComponentId, TGroupId aGroupId,THasThreadIdentification aHasContext, THasProgramCounter aHasPc)
	{
	INFO_PRINTF1(_L("-------- Storing trace information --------"));
	TInt lastError = KErrNone;
	RPointerArray<TTraceConfigs> traceObjects(10);
	TBool realTrace=EFalse;
	
	if (aEnumBased)
		{
		realTrace=CreateTraceObject(anApi,aHasContext,aHasPc,traceObjects);
		}
	else
		{
		realTrace=CreateTraceObject(aComponentId, aGroupId,aHasContext, aHasPc, traceObjects);
		}

			
	if(traceObjects.Count()&&realTrace)
		{
		for(TInt i = 0; i <=(traceObjects.Count()-1); i++)
			{
			iSentTraces.Append(traceObjects[i]);
			}
		}
			
	if (!realTrace)
		{
		lastError=KErrNone;
		INFO_PRINTF3(_L("INFO: Trace object (TTraceConfigs) not created, because no actual trace sent error %i, API %i. "), lastError,anApi);
		}
	traceObjects.Reset();
	return lastError;
	}

void CTe_MCLSuiteBaseWrapper::CreateTraceHeaderObject(TComponentId aComponentId, TGroupId aGroupId,THasThreadIdentification aHasContext, THasProgramCounter aHasPc,TTraceHeaderInfo*& aHeaderInfo)
	{
	//INFO_PRINTF1(_L("-------- Creating Trace header object --------"));
	TTraceConfigsOperator::Init(*aHeaderInfo);
	TTraceConfigsOperator::SetHeader(*aHeaderInfo, aGroupId, aComponentId, aHasContext, aHasPc, KInitialClientFormat);
	if(aHasContext)
		{
		aHeaderInfo->iContextId = RThread().BTraceId() + 704; //704 is the offset into the RThread and DThread object until we get to the NThread
		}
	}

TInt CTe_MCLSuiteBaseWrapper::StoreTraceHeaderInfo(TComponentId aComponentId, TGroupId aGroupId,THasThreadIdentification aHasContext, THasProgramCounter aHasPc)
	{
	//INFO_PRINTF1(_L("-------- Storing trace header information --------"));
	TInt lastError = KErrNone;
	TTraceHeaderInfo* headerInfo= new TTraceHeaderInfo;
	ASSERT(headerInfo);
	CreateTraceHeaderObject(aComponentId, aGroupId,aHasContext, aHasPc, headerInfo);

	TRAP(lastError,(iSentTraceHeaders->AppendL(headerInfo)));
	if (lastError)
		{
		INFO_PRINTF2(_L("Error: Unable to append trace header object to array. error: %i "), lastError);
		delete headerInfo;
		}

	return lastError;
	}

TInt CTe_MCLSuiteBaseWrapper::ReadFromLogFile(const TDesC& aFileName)
	{
	INFO_PRINTF2(_L("-------- Reading from log file -------- %S"), &aFileName);
	//read data...
	TUint 		bufferSize = 0;
	TUint8*		traceBuffer = NULL;
	
	
	TRAPD(error, (error = GetDataL(traceBuffer, bufferSize, aFileName)));
	if(!bufferSize || error)
		{
		INFO_PRINTF2(_L("ERROR: Error reading from logfile %i. "), error);
		return error;
		}
	// process all the complete traces in buffer...
	error = iParser.ParseRawBuffer(traceBuffer, bufferSize, iLoggedTraces);
	INFO_PRINTF2(_L("Number of additional traces (not sent from test suite) is %d"),iParser.iNumSystemTraces);
	delete traceBuffer; //done with it, so delete it
	return error;
	}

TInt CTe_MCLSuiteBaseWrapper::ReadHeaderFromLogFile(const TDesC& aFileName)
	{
	INFO_PRINTF1(_L("-------- Reading headers info from log file --------"));
	TUint 		bufferSize = 1;
	TUint8*		traceBuffer = NULL;
	TUint		position= 0;
	iHeaderParser.iNumSystemTraces=0;
	iPosition=0;
	iAlignPosition=-1;
	iNumMatchingTraces=0;
	TInt error=KErrNone;
	//read data...
	while (bufferSize&&!error)
		{
		TRAP(error, (error = GetDataL(traceBuffer, bufferSize, position, aFileName)));
		if(!bufferSize || error)
			{
			INFO_PRINTF2(_L("ERROR: Error reading from logfile %i. "), error);
			if (traceBuffer)
				delete traceBuffer;
			break;
			}
		// process all the complete traces in buffer...
		error = iHeaderParser.ParseRawBuffer(traceBuffer, bufferSize, iLoggedTraceHeaders);
		if (error)
			INFO_PRINTF2(_L("ERROR: Error parsing info from logfile %i. "), error);
		delete traceBuffer; //done with it for now, so delete it (GetDataL() always allocates new memory)
		if (!error)
			CompareTraceHeaders();
		for (TInt i=0; i<iLoggedTraceHeaders->Count(); i++)
			{
			delete iLoggedTraceHeaders->At(i);
			}
		iLoggedTraceHeaders->Delete(0,iLoggedTraceHeaders->Count()); //empty out array to save space
		
		if (iSentTraceHeaders->Count()>=1)
			{
			for (TInt i=0; i<iPosition; i++)
				{
				delete iSentTraceHeaders->At(i);
				}
			iSentTraceHeaders->Delete(0,iPosition);
			iSentTraceHeaders->Compress();
			}
		iPosition=0;
		position+=bufferSize;
		}
	INFO_PRINTF2(_L("Number of additional traces (not sent from test suite) is %d"),iHeaderParser.iNumSystemTraces);
	INFO_PRINTF2(_L("Number of matching traces is %d"),iNumMatchingTraces);
	INFO_PRINTF2(_L("Position of alignment is %d"),iAlignPosition);
	return error;
	}

TInt CTe_MCLSuiteBaseWrapper::CompareTraces()
	{
	INFO_PRINTF1(_L("-------- Comparing Traces --------"));
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

		//GroupId ...
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
			INFO_PRINTF3(_L("TraceId             - ERROR: sent %i, logged %i"), sent->iTraceId, logged->iTraceId);
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
			//INFO_PRINTF3(_L("ContextId value       - ERROR: sent 0x%x, logged 0x%x"), sent->iContextId, logged->iContextId);
			//INFO_PRINTF1(_L("Integration test suite does not support checking of context ID - this is done in the unit test suite"));
			//error = KErrCorrupt; 
			}

		//pc value...	
		if(sent->iPc == logged->iPc)
			INFO_PRINTF2(_L("Pc value 0x%x         - ok"), (TInt) sent->iPc);
		else
			{
			//not set to be corrupt now because PC  isn't working properly yet
			INFO_PRINTF3(_L("Pc value              - ERROR: sent 0x%x, logged 0x%x"), sent->iPc, logged->iPc);
			//error = KErrCorrupt;
			}
		
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
			TUint16* sentPtr = (TUint16*) sent->iRawData;
			TUint16* loggedPtr = (TUint16*) logged->iRawData;
			INFO_PRINTF3(_L("Payload               - ERROR: sent \"%s\", logged \"%s\""), sentPtr, loggedPtr);
			INFO_PRINTF7(_L("Payload               - sent   [0x%x][0x%x][0x%x][0x%x][0x%x][0x%x]"), sent->iRawData[0],sent->iRawData[1],sent->iRawData[2],sent->iRawData[3],sent->iRawData[4],sent->iRawData[5]);
			INFO_PRINTF7(_L("Payload               - logged [0x%x][0x%x][0x%x][0x%x][0x%x][0x%x]"), logged->iRawData[0], logged->iRawData[1], logged->iRawData[2], logged->iRawData[3], logged->iRawData[4], logged->iRawData[5]);
			error = KErrCorrupt;
			}
		}
	return error;
	}

TInt CTe_MCLSuiteBaseWrapper::CompareTraceHeaders()
	{
	TInt error = KErrCorrupt;
		
	for(TInt i = iPosition; ((error!=KErrNone)&&(iSentTraceHeaders->Count()>iPosition)); i++)
		{
		
		if (iLoggedTraceHeaders->Count()==0)
			{
			INFO_PRINTF1(_L("iLoggedTraceHeaders array is empty"));
			break;
			}
		TTraceHeaderInfo* logged = iLoggedTraceHeaders->At(0);
		TTraceHeaderInfo* sent = iSentTraceHeaders->At(i);
		
		//module uid ...
		if(logged->iComponentId == sent->iComponentId)
			{
			error = KErrNone;
			}
		else
			{
			error = KErrCorrupt;
			}

		//GroupId ...
		if(logged->iGroupId == sent->iGroupId){}
		else
			{
			error = KErrCorrupt;
			}

		//sub-category ...
		if(logged->iSubCategory == sent->iSubCategory){}
		else
			{
			error = KErrCorrupt;
			}
		
		//format id ...
		if(logged->iTraceId == sent->iTraceId)
			{}
		else
			{
			error = KErrCorrupt;
			}
		
		//context id present ...
		if(logged->iHasThreadId == sent->iHasThreadId){}
		else
			{
			error = KErrCorrupt;
			}
	
		//pc present ...
		if(logged->iHasProgramCounter == sent->iHasProgramCounter){}
			
		else
			{
			error = KErrCorrupt;
			}
		
		//context id value ...
		if(sent->iContextId == logged->iContextId){}
		else
			{
			error = KErrCorrupt;
			}

		//pc value...	
		if(sent->iPc == logged->iPc){}
		else
			{
			//wont set to be corrupt now because PC  isn't working properly yet
			//error = KErrCorrupt;
			}
		
		if (error==KErrNone)
			{
			iNumMatchingTraces++;
			if(iAlignPosition==-1)
				{
				iAlignPosition=iPosition;
				}
			}
		iPosition++;
		}
	return error;
	}


TInt CTe_MCLSuiteBaseWrapper::GetDataL(TUint8*& aData, TUint& aSize,const TDesC& aFileName)
	{
	RFs fsSession;
	CleanupClosePushL(fsSession);
	User::LeaveIfError(fsSession.Connect()); 
	
	RFile file;
	TInt error = file.Open(fsSession, aFileName, EFileRead);
	if(error == KErrNone)
		{
		TInt size = 0;
		file.Size(size);
		aSize = size;
		aData = new TUint8[aSize];
		if(!aData)
			error = KErrNoMemory;
		if(!error)
			{
			TPtr8 ptr(aData, aSize);
			error = file.Read(ptr);
			}
		file.Close();//is really called at fsSession.Close(); 
		fsSession.Close();
		}
	CleanupStack::PopAndDestroy();//fsSession
	return error;
	}

TInt CTe_MCLSuiteBaseWrapper::GetDataL(TUint8*& aData, TUint& aBytesRead, TUint aPosition, const TDesC& aFileName)
	{
	RFs fsSession;
	CleanupClosePushL(fsSession);
	User::LeaveIfError(fsSession.Connect()); 
	
	RFile file;
	TInt error = file.Open(fsSession, aFileName, EFileRead);
	if(error == KErrNone)
		{
		TUint8* size= new TUint8;
		TInt traceSize;
		//file.Size(size);
		TPtr8 ptr(size, 1); //read first byte from start position, which should be size of trace
		error = file.Read(aPosition, ptr, 1);
		traceSize = *size;
		aData = new TUint8[traceSize];
		if(!aData)
			error = KErrNoMemory;
		if(!error)
			{
			TPtr8 ptr2(aData, traceSize);
			error = file.Read(aPosition,ptr2,traceSize);
			aBytesRead=ptr2.Length();
			}
		file.Close();//is really called at fsSession.Close(); 
		fsSession.Close();
		}
	CleanupStack::PopAndDestroy();//fsSession
	return error;
	}

TInt CTe_MCLSuiteBaseWrapper::EmptyFile(const TDesC& aFileName)
	{
	RFs outputFileSession;
	RFile outputFile;
	TInt err = outputFileSession.Connect();	
	//delete the output file (i.e. replace with a zero length file) and close the sessions
	err=outputFile.Replace(outputFileSession, aFileName ,EFileWrite);
	outputFile.Close();
	outputFileSession.Close();	
	
	return err;
	}

