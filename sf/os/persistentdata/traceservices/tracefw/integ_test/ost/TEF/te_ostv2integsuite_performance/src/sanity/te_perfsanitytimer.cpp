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
// UTrace Performance Tests Kernel-Side Timer.
//



/**
 @file te_perfsanitytimer.cpp
 @internalTechnology
 @prototype
*/
#include "te_perfsanitytimer.h"
#include "te_apirunconfig.h"
#ifndef __KERNEL_MODE__
#include "te_instrumentation_enum.h"
#include "te_instrumentationpoints.h"

#endif
#include "te_dataparameters.h"

TBool TTestTimer::TestUserTimerL(TUint32& aTestTime)
	{
	return DoTestTraceL(ESanityFoo, aTestTime);
	}

TBool TTestTimer::TestUserLongTimerL(TUint32& aTestTime)
	{
	return DoTestTraceL(ESanityFooLong, aTestTime);
	}


TBool TTestTimer::TestKernelTimerL(TUint32& aTestTime)
	{
	return DoTestTraceL(ESanityFoo, aTestTime);
	}

TBool TTestTimer::TestKernelLongTimerL(TUint32& aTestTime)
	{
	return DoTestTraceL(ESanityFooLong, aTestTime);
	}

TBool TTestTimer::TestUTraceUserTimerL(TUint32& aTestTime)
	{
	return DoTestTraceL(EUtraceUsr, aTestTime);
	}

TBool TTestTimer::TestUTraceKernelTimerL(TUint32& aTestTime)
	{
	return DoTestTraceL(EUtraceKrn, aTestTime);
	}


TBool TTestTimer::DoTestTraceL(const TTestMethodType aMethod, TUint32& aTestTime)
	{
	aTestTime = 0;

	//Want to set this up before we run the timers not to affect them
	TApiRunConfig apiRunConfig;
	TRunConfigurer::Init(apiRunConfig);
	if(aMethod == EUtraceUsr)
		{
		#ifndef __KERNEL_MODE__
		apiRunConfig.iApiId = UPTTraceTest;
		#endif
		}
	if(aMethod == EUtraceKrn)
		{
		#ifndef __KERNEL_MODE__
		apiRunConfig.iApiId = UPTKernelTraceTest;
		#endif
		}
	apiRunConfig.iDoMinExecution = ETrue;
	TApiRunResults apiRunResults;
	TRunConfigurer::Init(apiRunResults);

	TUint32 callResults[KNumberOfTraceCallIterations];
	for(TInt i = 0; i < KNumberOfTraceCallIterations; i++)
		{

		TInt error = iTimer.StartBackgroundTimer();
		if(!error)
			{
			while(!iTimer.IsTimerExpired())
				{
				switch(aMethod)
				{
				case ESanityFoo:
				SanityFoo();
				break;
				case ESanityFooLong:
				SanityFooLong();
				break;
				case EUtraceUsr:
				case EUtraceKrn:
				#ifndef __KERNEL_MODE__
				TUptTraceCalls::DoSendTraceL(apiRunConfig, apiRunResults);
				#endif
				break;
				}
				iTimer.IncreaseCount();
				}
			iTimer.FinalCount();
			aTestTime = iTimer.FinalTime();//in nanosec
			}
		callResults[i] = aTestTime;
		}
	TUint32 total = 0;
	for(TInt i = 0; i < KNumberOfTraceCallIterations; i++)
		total += callResults[i];
	aTestTime = total / KNumberOfTraceCallIterations; //average


	return VerifyTime(aTestTime);
	}

TBool TTestTimer::VerifyTime(TUint32 aTime)
	{
	if(aTime > KMinFooTime && aTime < KMaxFooTime)
		return ETrue;
	else
		return EFalse;
	}


