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
 @file te_apirunconfig.cpp
 @internalTechnology
*/
#include "te_apirunconfig.h"
#include "te_instrumentation_enum.h"
#include "te_dataparameters.h"

void TRunConfigurer::Init(TApiRunResults& aResults)
	{
	aResults.iApiTraceTime = 0;
	aResults.iTimeCount = 0;
	aResults.iApiStackUsage = -1;
	aResults.iApiHeapUsage = -1; //not used yet
	aResults.iTraceEnabled = EFalse;
	aResults.iError = KErrNone;
	}

void TRunConfigurer::Init(TApiRunConfig& aConfig)
	{
	TRunConfigurer::SetRunParams(aConfig, 0, ENoThreadIdentification, ENoProgramCounter);
	TRunConfigurer::SetRunConfigs(aConfig, ETrue, ETrue,ETrue, ETrue, EFalse, EFalse, EFalse, EFalse, EFalse);
	}

void TRunConfigurer::SetRunParams(TApiRunConfig& aConfig, TInt aApiId, THasThreadIdentification aHasContext, THasProgramCounter aHasProgramCounter)
	{
	aConfig.iApiId = aApiId; 
	aConfig.iHasContext = aHasContext; 
	aConfig.iHasProgramCounter = aHasProgramCounter;

	switch(aConfig.iApiId)
		{	
		case EUptTraceTUint32:
		case EUptKernelTraceTUint32:
			aConfig.iSizeOfUptData=sizeof(TUint32);
		break;
		
		case EUptTraceTUint32TUint32:
		case EUptKernelTraceTUint32TUint32:
			aConfig.iSizeOfUptData=2*sizeof(TUint32);
		break;
		
		case EUptTraceTemplatedSmallStruct:
		case EUptKernelTraceTemplatedSmallStruct:
			aConfig.iSizeOfUptData=KSmallSize;
		break;
		
		case EUptTraceTemplatedBigStruct:
		case EUptKernelTraceTemplatedBigStruct:
		case EUptPrintTDesC8_big:
		case EUptTraceTDesC8_big:
			aConfig.iSizeOfUptData = KBigSize8;
		break;
		
		case EUptPrintTDesC8_huge:
		case EUptTraceTDesC8_huge:
		case EUptTraceTemplatedHugeStruct:
		case EUptKernelTraceTemplatedHugeStruct:
			aConfig.iSizeOfUptData = KHugeSize8;
		break;

		case EUptPrintTDesC16_big:
		case EUptTraceTDesC16_big:
			aConfig.iSizeOfUptData = KBigSize16;	
		break;
		
		case EUptPrintTDesC16_huge:
		case EUptTraceTDesC16_huge:
			aConfig.iSizeOfUptData = KHugeSize16;				
		break;
			
		case EUptPrintfTRefByValueTDesC16:
#ifndef __KERNEL_MODE__
			
			aConfig.iSizeOfUptData=sizeof(TDesC16);
#endif
		break;
		
		case EUptPrintfchar:
		case EUptKernelPrintfchar:
			aConfig.iSizeOfUptData=sizeof(char);
		break;
		default:
			/*//zero EVP
			EUptTrace:
			EUptKernelTrace
			EUptKernelWouldBeTracedNow
			UPTTraceTest,
			UPTKernelTraceTest,
			UPTKernelNegativeFunctionalityTest,
			UPTKernelMultipleFunctionalityTest,
			BTraceFiltered4,
			BTraceFiltered8,
			BTraceFiltered12,
			BTraceFilteredBig,
			BTrace0,
			BTrace4,
			BTrace8,
			BTrace12,
			UPTBaseline,
			UPTKernelBaseline,
			UPTFooUserTime,
			UPTFooKernelTime,
			*/
			aConfig.iSizeOfUptData=0;
		break;
		}
	
#ifdef LOOP_EVP	
	//add in extra data size allocation if looping over the data size as a run config option
	//this is not fully implemented yet as really only applies to the TAny API IDs
	//waste of time to run for all of the id's really
#endif	
	}

void TRunConfigurer::SetRunConfigs(TApiRunConfig& aConfig, TBool aDoTestClassificationFiltering, TBool aDoTestModuleIdFiltering, TBool aDoTestClassificationFilterMatch, TBool aDoTestModuleIdFilterMatch, TBool aDoMinExecution, TBool aDoTypicalExecution, TBool aDoTrace, TBool aDoStack, TBool aDoFlush)
	{
	aConfig.iDoTestClassificationFiltering = aDoTestClassificationFiltering;
	aConfig.iDoTestModuleIdFiltering = aDoTestModuleIdFiltering;
	aConfig.iDoTestClassificationFilterMatch = aDoTestClassificationFilterMatch;
	aConfig.iDoTestModuleIdFilterMatch = aDoTestModuleIdFilterMatch;
	aConfig.iDoMinExecution = aDoMinExecution;
	aConfig.iDoTypicalExecution = aDoTypicalExecution; 
	aConfig.iDoTrace = aDoTrace;
	aConfig.iDoStack = aDoStack; 
	aConfig.iDoFlush = aDoFlush;
	}

TApiTestResult::TApiTestResult()
	{
	TRunConfigurer::Init(iRunConfigs);
	TRunConfigurer::Init(iRunResults);
	
	iMinimumExecutionTime = -1;
	iTypicalExecutionTime = -1;
	iApiSize = -1;
	}


