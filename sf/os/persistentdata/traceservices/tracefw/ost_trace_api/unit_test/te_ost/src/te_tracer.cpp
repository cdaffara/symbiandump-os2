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
 @file te_tracer.cpp
 @internalTechnology
*/
#include "d32utrace.h"
#include "te_tracer.h"
#include "te_timer.h"
#include "te_apicaller.h"


TInt TTraceTester::TimeTraceL(TTraceConfigs& aTraceData, TUint32& aTime, TUint32& aCount) //pass the configuration here
	{
	TInt error = KErrNone;
	TUint32 callResults[KNumberOfTraceCallIterations];
	for(TInt i = 0; i < KNumberOfTraceCallIterations; i++)
		{
		error = TimeTraceCallL(aTraceData, aTime, aCount);
		callResults[i] = aTime;
		}
	TUint32 total = 0;
	for(TInt i = 0; i < KNumberOfTraceCallIterations; i++)
		total += callResults[i];
	aTime = total / KNumberOfTraceCallIterations; //average
	return error;
	}


TInt TTraceTester::TimeTraceCallL(TTraceConfigs& aTraceData, TUint32& aTime, TUint32& aCount)
	{
	TInt error = KErrNotSupported;
 	if(TApiCaller::IsKernelTrace(aTraceData.iApiUsed))
 		{
 		// kernel side removed for now
/*			RUTrace utracedriver;
			error = utracedriver.Open();
			if(!error)
				{
				error = utracedriver.TimeTraces(aTraceData, aTime, aCount);
				}
			utracedriver.Close();
			*/
 		}
 	else//it's a user side trace, call it directly
 		{
 		CUtfTimer timer;
 		aCount = 0;
 		TRequestStatus status;
 		error = timer.StartBackgroundTimer(status);
 		if(!error)
 			{
	 		do
	 			{
	 			error = TApiCaller::DoSendTraceL(aTraceData);
	 			aCount++;
#ifdef __WINS__
	 			// under WINSCW this code never completes as status is never KErrNone	 			
				// and as timings are not valid for emulator - we should just exit the loop here
	 			
	 			TRequestStatus* pStatus = &status;
	 			User::RequestComplete(pStatus, KErrNone);
#endif
	 			
	 			}
	 		while(status != KErrNone);	// while (timer is still waiting)
	 		aTime = timer.CalculateTime(aCount);
 			}
 		}	
	return error;
	}


TInt TTraceTester::SendTraceL(TTraceConfigs& aTraceData)
	{
 	TInt error = KErrNotSupported;
 	if(TApiCaller::IsKernelTrace(aTraceData.iApiUsed))
 		{
 		// kernel side removed for now
/*			RUTrace utracedriver;
			error = utracedriver.Open();
			if(!error)
				{
				error = utracedriver.TestTraces(aTraceData);
				}
			utracedriver.Close();
*/ 		}

 	else//it's a user side trace, call it directly
 		{
		error = TApiCaller::DoSendTraceL(aTraceData);
 		}
	return error;
	}
