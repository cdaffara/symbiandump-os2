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
// ost Performance Tests ost Call.
//



/**
 @file te_perftracetester.cpp
 @internalTechnology
 @prototype
*/


#include <opensystemtrace.h>
#ifndef __KERNEL_MODE__
#include <e32base.h> //for cleanupstack
#endif
#include "te_perftracetester.h"
#include "te_instrumentationpoints.h"
#include "te_perfcacheflush.h"
#include "te_d32perfdrv.h"
#include "timer/te_perfusertimer.h"




TInt TUptTraceTester::SendTraceL(const TApiRunConfig& aApiRunConfig,  TApiRunResults& aApiRunResults) //pass the configuration here
	{
 	TInt error = KErrNone;
 	if(TUptTraceCalls::IsKernelTrace(aApiRunConfig.iApiId))
 		{
		#ifndef __KERNEL_MODE__
			RUptUTrace utracedriver;
		//update with test on whether compiled of not!!
			error = utracedriver.Open();
			if(!error)
				{
				error = utracedriver.TestUptUtrace(aApiRunConfig, aApiRunResults);
				}
			utracedriver.Close();
		#endif
 		}
 	else
 		{
 		//it's a user side trace, call it directly
		error = TUptTraceCalls::DoSendTraceL(aApiRunConfig, aApiRunResults);
 		}
	return error;
	}

TInt TUptTraceTester::TimeTraceL(const TApiRunConfig& aApiRunConfig,  TApiRunResults& aApiRunResults) //pass the configuration here
	{
	TInt error = KErrNone;
	TUint32 callResults[KNumberOfTraceCallIterations];
	for(TInt i = 0; i < KNumberOfTraceCallIterations; i++)
		{
		error = TimeTraceCallL(aApiRunConfig, aApiRunResults);
		callResults[i] = aApiRunResults.iApiTraceTime;
		}
	TUint32 total = 0;
	for(TInt i = 0; i < KNumberOfTraceCallIterations; i++)
		total += callResults[i];
	aApiRunResults.iApiTraceTime = total / KNumberOfTraceCallIterations; //average
	return error;
	}

TInt TUptTraceTester::TimeTraceCallL(const TApiRunConfig& aApiRunConfig,  TApiRunResults& aApiRunResults)
	{
	TInt error = KErrNone;
 	if(TUptTraceCalls::IsKernelTrace(aApiRunConfig.iApiId))
 		{
		RUptUTrace utracedriver;
		error = utracedriver.Open();
		if(!error)
			{
			error = utracedriver.TimeUptUtrace(aApiRunConfig, aApiRunResults);
			}
		utracedriver.Close();
 		}
 	else
 		{
 		error = TimeUsersideTraceCallsL(aApiRunConfig, aApiRunResults);
 		}

 	return error;
	}

TInt TUptTraceTester::TimeUsersideTraceCallsL(const TApiRunConfig& aApiRunConfig,  TApiRunResults& aApiRunResults) //pass the configuration here
	{
		aApiRunResults.iTimeCount = 0;
		CUptTimer timer;
		TInt error = timer.StartBackgroundTimer();
		if(!error)
			{
			while(!timer.IsTimerExpired())
				{
				if(aApiRunConfig.iDoFlush) //i.e. if we want the non-cache results
					Te_Flush_The_Cache();	//ignoring error? //will be KErrNotSupported for winscw in any case
				error = TUptTraceTester::SendTraceL(aApiRunConfig, aApiRunResults);

				timer.IncreaseCount();
				}
			aApiRunResults.iApiTraceTime = timer.FinalTime();
			aApiRunResults.iTimeCount = timer.FinalCount();
			}

		return error;
	}


TInt TUptTraceTester::GenerateEvpDataField(const TInt aDataSize, TInt8* aData)
	{
	TInt err=KErrNone;

	for(TInt i=0;i<aDataSize/8;i++)
		{
		*aData=0x00;
		aData++;
		}
	for(TInt i=0;i<aDataSize/8;i++)
		aData--;
	return err;
	}


//eof
