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
// UTrace Unit Tests.
// Created: 03/04/2008
//



/**
 @file te_apienums.h
 @internalTechnology
 @prototype
*/
#ifndef TE_APIENUMS_H_
#define TE_APIENUMS_H_


enum TTraceApiUsed
	{
	ENone = 0,

//User side
	//trace
	EPrintTDesC8,
	EPrintTDesC16,
	EPrintfchar,
	EPrintfTRefByValueTDesC16,
	EPrintfTRefByValueTDesC8, //5
	ETrace,
	ETraceTUint8,
	ETraceTUint16,
	ETraceTDesC8,
	ETraceTDesC16,//10
	ETraceTUint32, 
	ETraceTUint32TUint32,
	ETraceTAnySize,
	ETraceTemplated,
	EIsTraceActive,//15
	
	
	//context
	EContextTClassification,
	EContextTClassificationTHasContextIdTHasProgramCounter,
	EContextTComponentIdTClassification,
	EContextTComponentIdTClassificationTHasContextIdTHasProgramCounter,
	EComponentId,//20
	EClassification,
	EUtfIncludeContextId, 
	EUtfIncludePc,	
	EDefaultComponentId,
	EUid3,//25

	//other
	EReferenceUserSide,
	EReferenceInlinedEmpty,
	EReferenceInlined,
	EReferenceExportedEmpty,
	EReferenceExported,//30
	EReferenceTraceNormalAny,
	EReferenceTraceNormal2Data,
	EReferenceTraceTraceIdAny,
	EReferenceTraceTraceId2Data, 
	EReferenceBaseline,//35

	//self test
	EReferenceForLoop, 
	
	//Performance related
	EPerformanceBaseline,
	
	//btrace	
	BTraceFilteredContext8,//equivalent of ETraceNormal with context, no pc
	BTraceFilteredContextPcBig,
	BTraceFilteredContextPc12,//40
	BTraceKernelFilteredContext8,
	BTraceKernelFilteredContextPcBig, 
	BTraceKernelFilteredContextPc12,
	
//Kernel side
	//traces
	EKernelPrintDesC8,
	EKernelPrintfchar,//45
	EUtfKernelTrace, //needed a different name as EKernelTrace was overloaded... should do something about the namespace instead, but can't be bothered.
	EKernelTraceTUint8,
	EKernelTraceTUint16,
	EKernelTraceTDesC8,
	EKernelTraceTUint32,//50
	EKernelTraceTUint32TUint32,
	EKernelTraceTAnySize,
	EKernelTraceTemplated,
	EKernelIsTraceActive,
	
	//context
	EKernelContextTClassification,	//55
	EKernelContextTClassificationTHasContextIdTHasProgramCounter,
	EKernelContextTComponentIdTClassification,
	EKernelContextTComponentIdTClassificationTHasContextIdTHasProgramCounter,
	EKernelComponentId,
	EKernelClassification,//60
	EKernelIncludeContextId,
	EKernelIncludePc,
	EKernelDefaultComponentId,
	EKernelUid3,

	//Performance related
	EPerformanceKernelBaseline,//65

	//self test
	EReferenceKernelForLoop,

	//other
	EReferenceKernelSide,
	EReferenceKernelInlinedEmpty,
	EReferenceKernelInlined,
	EReferenceKernelExportedEmpty,//70
	EReferenceKernelExported,
	};

static const TInt KAPIMaxNumberOfTraceApis = 25;
static const TInt KAPIMaxNumberOfCalls = 71;

#endif /*TE_APIENUMS_H_*/
