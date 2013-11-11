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
 @file te_apicaller.cpp
 @internalTechnology
*/
#include "te_apicaller.h"
#include "utracemodutracetest.h"
#include "te_trace_for_lauterbach.h"
#include <e32btrace.h> //for comaprison with btrace... should refactor this out at some stage

using namespace UTraceModUtraceTest;


TBool TApiCaller::IsKernelTrace(TTraceApiUsed aApi)
	{
	TBool isKernelTrace = EFalse;
	switch(aApi)
		{
		//it's a kernel side trace, we need to call the
		//device driver to make the call...
		case EKernelPrintDesC8:
		case EKernelPrintfchar:
		case EUtfKernelTrace:
		case EKernelTraceTUint32:
		case EKernelTraceTUint8:
		case EKernelTraceTUint16:
		case EKernelTraceTDesC8:
		case EKernelTraceTUint32TUint32:
		case EKernelTraceTAnySize:
		case EKernelIsTraceActive:
		case EReferenceKernelSide:
		case EReferenceKernelInlinedEmpty:
		case EReferenceKernelInlined:
		case EReferenceKernelExportedEmpty:
		case EReferenceKernelExported:
		case EPerformanceKernelBaseline:
		case EReferenceKernelForLoop:
		case BTraceKernelFilteredContext8:
		case BTraceKernelFilteredContextPcBig:
		case BTraceKernelFilteredContextPc12:
		case EKernelContextTClassification:
		case EKernelContextTClassificationTHasContextIdTHasProgramCounter:
		case EKernelContextTComponentIdTClassification:
		case EKernelContextTComponentIdTClassificationTHasContextIdTHasProgramCounter:
		case EKernelComponentId:
		case EKernelClassification:
		case EKernelIncludeContextId:
		case EKernelIncludePc:	
		case EKernelDefaultComponentId:
		case EKernelUid3:
			isKernelTrace = ETrue;
		break;
		default:
		break;
		}
	return isKernelTrace;
	}

TUint32 TApiCaller::PcOfPreviousTraceCall()
	{
	TUint32 pc = 0;
	#ifdef __MARM__
		asm("mov pc, __return_address()");
		return pc - 0x14;
		// We have removed this offset, as this is the size of the function. We require the PC from where this function is called from. The offset differs between platforms
	#elif __WINS__
		_asm push edx
		_asm mov edx, [ebp+4]
		_asm mov [pc], edx
		_asm pop edx
		return pc - 0x1A;
		// We have removed this offset, as this is the size of the function. We require the PC from where this function is called from. The offset differs between platforms
		//This is instead of doing "asm(mov pc, ebp+4)" as that gives warnings about registers being spilled.
	#elif __X86__ 
		//not tested yet!
		asm("push edx");
		asm("mov %0, [edx+4]": "=r" (pc));
		asm("pop edx");
	#endif

	/*
	TUint32 myPc = 0;
	#ifdef __WINS__
	__asm
		{
		push edx
		mov edx, [ebp+4]
		mov [myPc], edx
		pop edx
		}
	//	asm(mov myPc, ebp+4);
	#else
	asm("mov myPc, __current_pc()");
	#endif*/
	
	 //assuming the previous pc will always appear at a certain "length" away, disregarding of platform...
	}

TInt TApiCaller::DoSendTraceL(TTraceConfigs& aTraceData)
	{
	TInt error = KErrNone;
	TBool logged = EFalse;
	TTraceContext context(aTraceData.iComponentId, aTraceData.iGroupId, aTraceData.iHasThreadId, aTraceData.iHasProgramCounter);
	switch(aTraceData.iApiUsed)
		{		
		case EPrintTDesC8:
		case EKernelPrintDesC8:
			{
			logged = OstPrint(context, aTraceData.iDesC8);
			if(aTraceData.iHasProgramCounter == EAddProgramCounter)
				aTraceData.iPc = PcOfPreviousTraceCall();
			}
		break;
		
		case EPrintTDesC16:
		#ifndef __KERNEL_MODE__
			{
			logged = OstPrint(context, aTraceData.iDesC16);
			if(aTraceData.iHasProgramCounter == EAddProgramCounter)
				aTraceData.iPc = PcOfPreviousTraceCall();
			}
		#endif
		break;
		
		case EPrintfchar:
		case EKernelPrintfchar:
			{
			logged = OstPrintf(context, aTraceData.iChar);
			if(aTraceData.iHasProgramCounter == EAddProgramCounter)
				aTraceData.iPc = PcOfPreviousTraceCall();
			}
		break;
		
		case EPrintfTRefByValueTDesC16:
			{
			#ifndef __KERNEL_MODE__
			logged = OstPrintf(context,KDesCFormatted16,2 );//aTraceData.iDesC16);
			if(aTraceData.iHasProgramCounter == EAddProgramCounter)
				aTraceData.iPc = PcOfPreviousTraceCall();
			#endif
			}
		break;

		case EPrintfTRefByValueTDesC8:
			{
			logged = OstPrintf(context,  KDesCFormatted8,2);//aTraceData.iDesC8);
			if(aTraceData.iHasProgramCounter == EAddProgramCounter)
				aTraceData.iPc = PcOfPreviousTraceCall();					
			}
		break;
		
		case ETrace:
		case EUtfKernelTrace:
			{
			logged = OstTrace(context, aTraceData.iTraceId);
			if(aTraceData.iHasProgramCounter == EAddProgramCounter)
				aTraceData.iPc = PcOfPreviousTraceCall();
			}
		break;
		
		case ETraceTUint32:
		case EKernelTraceTUint32:
			{
			logged = OstTrace(context, aTraceData.iTraceId, aTraceData.iUint32[0]);
			if(aTraceData.iHasProgramCounter == EAddProgramCounter)
				aTraceData.iPc = PcOfPreviousTraceCall() - 1; //dont know why its an uneven offset, but it seems static anyway
			}
		break;
		
		case ETraceTUint8:
		case EKernelTraceTUint8:
			{			
			logged = OstTrace(context, aTraceData.iTraceId, (TUint8)aTraceData.iUint32[0]);
			if(aTraceData.iHasProgramCounter == EAddProgramCounter)
				aTraceData.iPc = PcOfPreviousTraceCall() - 1; //dont know why its an uneven offset, but it seems static anyway
			}
		break;
			
		case ETraceTUint16:
		case EKernelTraceTUint16:
			{			
			logged = OstTrace(context, aTraceData.iTraceId, (TUint16)aTraceData.iUint32[0]);
			if(aTraceData.iHasProgramCounter == EAddProgramCounter)
				aTraceData.iPc = PcOfPreviousTraceCall() - 1; //dont know why its an uneven offset, but it seems static anyway
			}
		break;
			
		case ETraceTDesC8:
		case EKernelTraceTDesC8:
			{			
			logged = OstTrace(context, aTraceData.iTraceId, (const TDesC8&) aTraceData.iDesC8);
			if(aTraceData.iHasProgramCounter == EAddProgramCounter)
				aTraceData.iPc = PcOfPreviousTraceCall();
			}
		break;
			
		case ETraceTDesC16:			
			#ifndef __KERNEL_MODE__
			{
			logged = OstTrace(context, aTraceData.iTraceId, (const TDesC16&) aTraceData.iDesC16);
			if(aTraceData.iHasProgramCounter == EAddProgramCounter)
				aTraceData.iPc = PcOfPreviousTraceCall();
			}
			#endif			
		break;		
			
		case ETraceTUint32TUint32:
		case EKernelTraceTUint32TUint32:
			{
			logged = OstTrace(context, aTraceData.iTraceId, aTraceData.iUint32[0], aTraceData.iUint32[1]);
			if(aTraceData.iHasProgramCounter == EAddProgramCounter)
				aTraceData.iPc = PcOfPreviousTraceCall() - 1; //dont know why its an uneven offset, but it seems static anyway
			}
		break;
		
		case ETraceTAnySize:
		case EKernelTraceTAnySize:
			{
			logged = OstTrace(context, aTraceData.iTraceId, aTraceData.iDesC8.Ptr(), aTraceData.iDesC8.Size());
			if(aTraceData.iHasProgramCounter == EAddProgramCounter)
				aTraceData.iPc = PcOfPreviousTraceCall() - 1; //dont know why its an uneven offset, but it seems static anyway
			}
		break;
		
        case ETraceTemplated:
        case EKernelTraceTemplated:
            {
            logged = OstTrace(context, aTraceData.iTraceId, aTraceData.iTemplated);
            if(aTraceData.iHasProgramCounter == EAddProgramCounter)
                aTraceData.iPc = PcOfPreviousTraceCall() - 1; //dont know why its an uneven offset, but it seems static anyway
            }
        break;		

		
		case EIsTraceActive:
		case EKernelIsTraceActive:
			{
			logged = IsTraceActive(context);
			if(aTraceData.iHasProgramCounter == EAddProgramCounter)
				aTraceData.iPc = PcOfPreviousTraceCall();
			}
		break;

		case EContextTClassification:
		case EKernelContextTClassification:
			{
			logged = OstTrace(TTraceContext(aTraceData.iGroupId), aTraceData.iTraceId);
			if(aTraceData.iHasProgramCounter == EAddProgramCounter)
				aTraceData.iPc = PcOfPreviousTraceCall();
			}
		break;
		case EContextTClassificationTHasContextIdTHasProgramCounter:
		case EKernelContextTClassificationTHasContextIdTHasProgramCounter:
			{
			logged = OstTrace(TTraceContext(aTraceData.iGroupId, aTraceData.iHasThreadId, aTraceData.iHasProgramCounter), aTraceData.iTraceId);
			if(aTraceData.iHasProgramCounter == EAddProgramCounter)
				aTraceData.iPc = PcOfPreviousTraceCall();
			}
		break;
		case EContextTComponentIdTClassification:
		case EKernelContextTComponentIdTClassification:
			{
			logged = OstTrace(TTraceContext(aTraceData.iComponentId, aTraceData.iGroupId), aTraceData.iTraceId);
			if(aTraceData.iHasProgramCounter == EAddProgramCounter)
				aTraceData.iPc = PcOfPreviousTraceCall();
			}
		break;
		case EContextTComponentIdTClassificationTHasContextIdTHasProgramCounter:
		case EKernelContextTComponentIdTClassificationTHasContextIdTHasProgramCounter:
			{
			logged = OstTrace(TTraceContext(aTraceData.iComponentId, aTraceData.iGroupId, aTraceData.iHasThreadId, aTraceData.iHasProgramCounter), aTraceData.iTraceId);
			if(aTraceData.iHasProgramCounter == EAddProgramCounter)
				aTraceData.iPc = PcOfPreviousTraceCall();
			}
		break;
		case EComponentId:
		case EKernelComponentId:
			{
			if(context.ComponentId() != aTraceData.iComponentId)
				error = KErrCorrupt;	
			logged = OstTrace(context, aTraceData.iTraceId);
			if(aTraceData.iHasProgramCounter == EAddProgramCounter)
				aTraceData.iPc = PcOfPreviousTraceCall();
			}
		break;
		case EClassification:
		case EKernelClassification:
			{
			if(context.Classification() != aTraceData.iGroupId)
				error = KErrCorrupt;
			logged = OstTrace(context, aTraceData.iTraceId);
			if(aTraceData.iHasProgramCounter == EAddProgramCounter)
				aTraceData.iPc = PcOfPreviousTraceCall();
			}
		break;
		case EUtfIncludeContextId:
		case EKernelIncludeContextId:
			{
			if(context.HasThreadIdentification() != aTraceData.iHasThreadId)
				error = KErrCorrupt;
			logged = OstTrace(context, aTraceData.iTraceId);
			if(aTraceData.iHasProgramCounter == EAddProgramCounter)
				aTraceData.iPc = PcOfPreviousTraceCall();
			}
		break;
		case EUtfIncludePc:	
		case EKernelIncludePc:	
			{
			if(context.HasProgramCounter() != aTraceData.iHasProgramCounter)
				error = KErrCorrupt;
			logged = OstTrace(context, aTraceData.iTraceId);
			if(aTraceData.iHasProgramCounter == EAddProgramCounter)
				aTraceData.iPc = PcOfPreviousTraceCall();
			}
		break;
		case EDefaultComponentId:
		case EKernelDefaultComponentId:
			{			
			TTraceContext contexts(aTraceData.iGroupId);
			if(contexts.ComponentId() != aTraceData.iComponentId)
				error = KErrCorrupt;
			logged = OstTrace(contexts, aTraceData.iTraceId);
			if(aTraceData.iHasProgramCounter == EAddProgramCounter)
				aTraceData.iPc = PcOfPreviousTraceCall();
			}
		break;
		case EUid3:
		case EKernelUid3:
			{ 
			if(context.DefaultComponentId() != KComponentId)
				error = KErrCorrupt;
			logged = OstTrace(context, aTraceData.iTraceId);
			if(aTraceData.iHasProgramCounter == EAddProgramCounter)
				aTraceData.iPc = PcOfPreviousTraceCall();
			}
		break;
		case EPerformanceBaseline:
		case EPerformanceKernelBaseline:
			{
			//just don't do anything except all things common
			if(aTraceData.iHasProgramCounter == EAddProgramCounter)
				aTraceData.iPc = PcOfPreviousTraceCall();
			}
		break;
		case EReferenceBaseline:
		case EReferenceInlinedEmpty:
		case EReferenceExportedEmpty:
		case EReferenceKernelInlinedEmpty:
		case EReferenceKernelExportedEmpty:
		case EReferenceInlined:
		case EReferenceExported:
		case EReferenceKernelInlined:
		case EReferenceKernelExported:
		case EReferenceTraceNormalAny:
		case EReferenceTraceNormal2Data:
		case EReferenceTraceTraceIdAny:
		case EReferenceTraceTraceId2Data:
		case EReferenceUserSide:
		case EReferenceKernelSide:
			{
			logged = WouldBeLogged(aTraceData);
			switch(aTraceData.iApiUsed)
				{
				case EReferenceInlinedEmpty:
				case EReferenceKernelInlinedEmpty:
					InlinedEmpty();
				break;
				case EReferenceInlined:
				case EReferenceKernelInlined:
					Inlined();
				break;
				case EReferenceExportedEmpty:
				case EReferenceKernelExportedEmpty:
					ExportedEmpty();
				break;
				case EReferenceExported:
				case EReferenceKernelExported:
					Exported();
				break;
				case EReferenceTraceNormalAny:
					logged = TraceNormalAny(&aTraceData.iDesC8, sizeof(aTraceData.iDesC8));
				break;
				case EReferenceTraceNormal2Data:
					logged = TraceNormal2Data(aTraceData.iUint32[0], aTraceData.iUint32[1]);
				break;
				case EReferenceTraceTraceIdAny:
					logged = TraceFormatIdAny(&aTraceData.iDesC8, sizeof(aTraceData.iDesC8));
				break;
				case EReferenceTraceTraceId2Data:
					logged = TraceFormatId2Data(aTraceData.iUint32[0], aTraceData.iUint32[1]);
				break;
				default:
				break;
				}
			}
		break;
		case EReferenceForLoop:
		case EReferenceKernelForLoop:
			{
			for(TInt i = 0; i < 1000;)
				i++;
			}
		break;
		case BTraceFilteredContext8: //equivalent of ETraceNormal with context, no pc
		case BTraceKernelFilteredContext8:
			BTraceFilteredContext8(aTraceData.iGroupId, 0,aTraceData.iComponentId,aTraceData.iUint32[0]);
		break;
		case BTraceFilteredContextPc12:
		case BTraceKernelFilteredContextPc12:
		BTraceFilteredContextPc12(aTraceData.iGroupId, 0,aTraceData.iComponentId, aTraceData.iUint32[0], aTraceData.iUint32[1]);
		break;
		case BTraceFilteredContextPcBig:
		case BTraceKernelFilteredContextPcBig:
			BTraceFilteredContextPcBig(aTraceData.iGroupId, 0,aTraceData.iComponentId, aTraceData.iDesC8.Ptr(), aTraceData.iDesC8.Size());
		break;
		default:
			error = KErrNotSupported;
		break;
		}
	if(!logged && (error == KErrNone))
		error = KInfoTraceNotOutput;
	
	return error;
	//returns KErrNone if logged, KErrNotLogged if not logged, KErrOther if any other problem...
	}


TBool TApiCaller::WouldBeLogged(TTraceConfigs& aTraceData)
	{
	if(aTraceData.iTracingBuiltIn)
		{
		if(aTraceData.iLoggingEnabled)
			{
			return ETrue;
			}
		}
	return EFalse;
	}


