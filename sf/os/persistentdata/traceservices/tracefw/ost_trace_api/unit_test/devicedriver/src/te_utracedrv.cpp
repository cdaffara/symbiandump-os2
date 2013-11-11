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
 @file te_utracedrv.cpp
 @internalTechnology
*/

#include <kern_priv.h>
#include "te_traceglobals.h"
#include "d32utrace.h"
//#include "te_apicaller.h"
#include "te_kerneltimer.h"


//
// Device driver
//

class DUTraceFactory : public DLogicalDevice
	{
public:
	DUTraceFactory();
	virtual TInt Install();
	virtual void GetCaps(TDes8& aDes) const;
	virtual TInt Create(DLogicalChannelBase*& aChannel);
	};

/**
 * Let the kernel side traces run in the user thread as we don't
 * need any particular kernel side stuff such as stackbases etc.
 * I.e. no need for DFCQs or NKern threads etc.
 */
class DUTraceChannel : public DLogicalChannelBase
	{
public:
	DUTraceChannel();
	virtual ~DUTraceChannel();
	//Device driver stuff
	virtual TInt DoCreate(TInt aUnit, const TDesC8* anInfo, const TVersion& aVer);
	virtual TInt Request(TInt aReqNo, TAny* a1, TAny* a2);

	//our stuff
	TInt TestTraces(TTraceConfigs& aTraceData);
	TInt TimeTraces(TTraceConfigs& aTraceData, TTimerResults& aTimeResults);	

private:
	//This current thread
	DThread*		iClient;

	//This is all about the timer and timing a trace call... 
	//should refactor into its own class, but it's so small so far it doesnt matter too much 
	//This is used to set up a timer such that we can measure how long a trace call takes
	TKernelTimer 	iTimer;
	};


DECLARE_STANDARD_LDD()
{
	return new DUTraceFactory;
}

EXPORT_C DLogicalDevice* CreateLogicalDevice();

//
// DBTraceFactory
//
DUTraceFactory::DUTraceFactory()
	{
	iVersion = RUTrace::VersionRequired();
	}


TInt DUTraceFactory::Install()
	{
#ifdef SYMBIAN_TRACE_SYSTEM_IS_INCLUDED
	#ifdef SYMBIAN_TRACE_EXECUTABLE_IS_INCLUDED
		return SetName(&KUTraceDDEnabled);
	#else
		return SetName(&KUTraceDDDisabled);
	#endif
#else
	#ifdef SYMBIAN_TRACE_EXECUTABLE_IS_INCLUDED
		return SetName(&KUTraceDDSystemEnabled);
	#else
		return SetName(&KUTraceDDSystemDisabled);
	#endif
#endif
	}

void DUTraceFactory::GetCaps(TDes8& aDes) const
	{
	Kern::InfoCopy(aDes,0,0);
	}

TInt DUTraceFactory::Create(DLogicalChannelBase*& aChannel)
	{
	
	aChannel=new DUTraceChannel();
	if(!aChannel)
		return KErrNoMemory;
	return KErrNone;
	}


//
// DBTraceChannel
//

DUTraceChannel::DUTraceChannel()
	{
	iClient = &Kern::CurrentThread();
	((DObject*)iClient)->Open();
	}

DUTraceChannel::~DUTraceChannel()
	{
	Kern::SafeClose((DObject*&)iClient,NULL);
	}

TInt DUTraceChannel::DoCreate(TInt /*aUnit*/, const TDesC8* /*aInfo*/, const TVersion& /*aVer*/)
	{
	//skipping security checks and version control since this is just a test driver
//	iDfcQ = Kern::DfcQue0();
//	SetDfcQ(iDfcQ);
//	iMsgQ.Receive();
	return KErrNone;
	}

TInt DUTraceChannel::Request(TInt aReqNo, TAny* a1, TAny* a2)
	{
	TInt error = KErrNone;
	
	TTraceConfigs* traceConfig = (TTraceConfigs*) a1;
	switch(aReqNo)
		{
		case RUTrace::ETestTrace:
			{
			TInt* result = (TInt*)a2;
			*result = TestTraces(*traceConfig);
			}
		break;
		case RUTrace::ETimeTrace:
			{
			TTimerResults* timerResults = (TTimerResults*) a2;
			error = TimeTraces(*traceConfig, *timerResults);
			}	
		break;
		default:
			error = KErrNotSupported;
		break;
		}
	return error;
	}

TInt DUTraceChannel::TestTraces(TTraceConfigs& /*aTraceData*/)
	{ 
//	return TApiCaller::DoSendTraceL(aTraceData);
	return KErrNotSupported;
	}

TInt DUTraceChannel::TimeTraces(TTraceConfigs& /*aTraceData*/, TTimerResults& /*aTimeResults*/)
	{
/*	TInt error = iTimer.StartBackgroundTimer();
	if(!error)
		{
		while(!iTimer.IsTimerExpired())
		   	 {
//		   	 	TApiCaller::DoSendTraceL(aTraceData);   
		        iTimer.IncreaseCount();
		   	 }
		    aTimeResults.iCount = iTimer.FinalCount();
		    aTimeResults.iTime = iTimer.FinalTime();
		}
//	return error;
 */
	return KErrNotSupported;
	}

