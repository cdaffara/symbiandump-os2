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
// e32\drivers\trace\btrace.cpp
//


#include <kern_priv.h>
#include <kernel.h>
#include <platform.h>
#include "te_d32perfdrv.h"
//#include "te_instrumentationpoints.h"
#include "te_perfcacheflush.h"
#include "timer/te_perfkerneltimer.h"
#include "sanity/te_perfsanitytimer.h"




//
// Device driver
//

class DUptUTraceFactory : public DLogicalDevice
	{
public:
	DUptUTraceFactory();
	virtual TInt Install();
	virtual void GetCaps(TDes8& aDes) const;
	virtual TInt Create(DLogicalChannelBase*& aChannel);
	};

/**
 * We need this class to derive from DLogicalChannel rather than DLogicalChannelBase
 * This is for several reasons:
 * We are making calls that take a long time, which means we don't want the
 * user side thread to block while doing this, and we don't want to be interrupted
 * and loose our state. So we solve this by running the device driver as
 * as it's own separate thread in the kernel (DLogicalChannelBase runs a user thread in kernel mode,
 * DLogicalChannel runs a kernel thread called from a user thread).
 * We also solve the long timing issue by making an asynchrounous call by using the DoRequestL call.
 * For this to work we need to create a DfcQ, i.e. a queue of all the incoming calls from the user side.
 * These then get's run in turn.
 */
class DUptUTraceChannel : public DLogicalChannel
	{
public:
	DUptUTraceChannel();
	virtual ~DUptUTraceChannel();
	//	Inherited from DObject
	/**
	 * I think this is similar to RunL when using active objects...
	 * Because we are now using a Dfc queue all calls are routed
	 * through this method.
	 * The aMsg.iValue are these: http://lon-engbuild130:6794/doc_source/EKA2DeviceDriver/WritingANewDeviceDriver/LogicalChannel.guide.html#DD%2dlogical%2dchannel%2emodel1%2ehandling%2dreqs%2eyou1
	 */
	virtual void HandleMsg(TMessageBase* aMsg);
	// Inherited from DLogicalChannel
	virtual TInt DoCreate(TInt aUnit, const TDesC8* anInfo, const TVersion& aVer);
	//Make an asynchronous call...
	virtual TInt DoRequestL(TInt aReqNo, TRequestStatus* aStatus, TAny* a1, TAny* a2);

	void TestUptTraces(const TApiRunConfig& aApiRunConfig, TApiRunResults& aApiRunResults);
	void TimeUptTraces(const TApiRunConfig& aApiRunConfig, TApiRunResults& aApiRunResults);

private:
	//This current thread
	DThread*		iClient;
	//This is a queue of requests (calls) to this device driver
	TDfcQue* 		iDfcQ;

	TKernelTimer	iTimer;
 	//This is used to signal when one of the async calls (to DoRequestL) is finished.
 	TRequestStatus	iStatus;
	};


DECLARE_STANDARD_LDD()
{
	return new DUptUTraceFactory;
}

EXPORT_C DLogicalDevice* CreateLogicalDevice();

//
// DBTraceFactory
//
DUptUTraceFactory::DUptUTraceFactory()
	{
	iVersion = RUptUTrace::VersionRequired();
	}


TInt DUptUTraceFactory::Install()
	{
#ifdef TE_UPT_TRACE_ENABLED
	return SetName(&KUptUTraceDDEnabled);
#else
	return SetName(&KUptUTraceDDDisabled);
#endif
	}

void DUptUTraceFactory::GetCaps(TDes8& aDes) const
	{
	Kern::InfoCopy(aDes,0,0);
	}

TInt DUptUTraceFactory::Create(DLogicalChannelBase*& aChannel)
	{
	aChannel=new DUptUTraceChannel();
	if(!aChannel)
		return KErrNoMemory;
	return KErrNone;
	}


//
// DBTraceChannel
//

DUptUTraceChannel::DUptUTraceChannel()
	{
	iClient = &Kern::CurrentThread();
	((DObject*)iClient)->Open();
	}

DUptUTraceChannel::~DUptUTraceChannel()
	{
	Kern::SafeClose((DObject*&)iClient,NULL);
	}

TInt DUptUTraceChannel::DoCreate(TInt /*aUnit*/, const TDesC8* /*aInfo*/, const TVersion& /*aVer*/)
	{
	//skipping security checks and version control since this is just a test driver
	iDfcQ = Kern::DfcQue0();
	SetDfcQ(iDfcQ);
	iMsgQ.Receive();
	return KErrNone;
	}

void DUptUTraceChannel::HandleMsg(TMessageBase* aMsg)
	{
	TThreadMessage& message = *(TThreadMessage*) aMsg;
	TInt id = message.iValue;
	if(id == (TInt) ECloseMsg)
		{
		//Close DUptUTraceChannel
		message.Complete(KErrNone, EFalse);
		return;
		}
	else if(id == KMaxTInt)
		{
		//DoCancel(message.Int0());
		message.Complete(KErrNone, ETrue);
		return;
		}
	if(id < 0 )  //it was a call for DoRequestL()
		{
		TRequestStatus* status = (TRequestStatus*)message.Ptr0();
		DoRequestL(~id, status, message.Ptr1(), message.Ptr2());
		message.Complete(KErrNone, ETrue);
		}
	else //it was a call for DoControl, we dont implement that now so just fall through
		{
		TRequestStatus* status=(TRequestStatus*)message.Ptr0();
		Kern::RequestComplete(iClient,status,KErrNotSupported);
		message.Complete(KErrNotSupported, ETrue);
		}
	}

TInt DUptUTraceChannel::DoRequestL(TInt aReqNo, TRequestStatus* aStatus, TAny* a1, TAny* a2)
	{
	TInt error = KErrNone;

/**
 * This is the beginning of the new implementation for creating a new thread
 * to run the timers in.
 *
	 NKern::ThreadEnterCS();
	{
		SThreadCreateInfo info;
		#ifdef DEBUG
			TPtrC8 name((TUint8*)get_thread_name(priority));
		#endif
		info.iType = EThreadSupervisor;
		info.iFunction = thread_wrapper;
		info.iPtr = thread;
		info.iSupervisorStack = NULL;
		info.iSupervisorStackSize = 0; // zero means use default value
		info.iInitialThreadPriority = hal_convert_priority(priority);
		info.iTotalSize = sizeof(info);
		#ifdef DEBUG
			info.iName.Set(name);
		#endif
		if (Kern::ThreadCreate(info) != KErrNone)
		{
		     rc = EINVAL;
		       goto Error;
		 }

		thread->func = func;
		thread->arg = arg;
			Kern::ThreadResume(*(DThread *)info.iHandle);
		}
		NKern::ThreadLeaveCS();


	//TInt NKern::ThreadCreate(NThread *aThread, SNThreadCreateInfo &anInfo);
	 *
	 * */
	if(!error)
		{
		switch(aReqNo)
			{
			case RUptUTrace::ETestUTrace:
				{
				TApiRunConfig configData;
			    error = Kern::ThreadRawRead(iClient, a1, (TUint8 *)&configData, sizeof(TApiRunConfig));
				TApiRunResults configResults;
			    error = Kern::ThreadRawRead(iClient, a2, (TUint8 *)&configResults, sizeof(TApiRunResults));
			    if(!error)
			    	TestUptTraces(configData, configResults);
			    if(!error)
			    	error = Kern::ThreadRawWrite(iClient, a2, (TUint8 *)&configResults, sizeof(TApiRunResults));
				}
			break;
			case RUptUTrace::ETimeUTrace:
				{
				TApiRunConfig configData;
			    error = Kern::ThreadRawRead(iClient, a1, (TUint8 *)&configData, sizeof(TApiRunConfig));
				TApiRunResults configResults;
			    error = Kern::ThreadRawRead(iClient, a2, (TUint8 *)&configResults, sizeof(TApiRunResults));
			    if(!error)
			    	{
			    	TimeUptTraces(configData, configResults);
			    	}
			    if(!error)
			    	error = Kern::ThreadRawWrite(iClient, a2, (TUint8 *)&configResults, sizeof(TApiRunResults));
				}
			break;
			case RUptUTrace::ESanityTestTimer:
			case RUptUTrace::ESanityUtraceTimer:
			case RUptUTrace::ESanityTestLongTimer:
				{
				TSanityResults results;
			    error = Kern::ThreadRawRead(iClient, a1, (TUint8 *)&results, sizeof(TSanityResults));
			    if(!error)
			    	{
			    	TTestTimer timer;
			    	if(aReqNo == RUptUTrace::ESanityTestTimer)
			    		results.iPass = timer.TestKernelTimerL(results.iTime);
			    	if(aReqNo == RUptUTrace::ESanityTestLongTimer)
			    		results.iPass = timer.TestKernelLongTimerL(results.iTime);
			    	if(aReqNo == RUptUTrace::ESanityUtraceTimer)
			    		results.iPass = timer.TestUTraceKernelTimerL(results.iTime);
			    	}
			    if(!error)
			    	error = Kern::ThreadRawWrite(iClient, a1, (TUint8 *)&results, sizeof(TSanityResults));
				}
			break;

			default:
				error = KErrNotSupported;
			break;
			}
		}
	Kern::RequestComplete(iClient, aStatus, error);
	return error;
	}

void DUptUTraceChannel::TestUptTraces(const TApiRunConfig& /*aApiRunConfig*/, TApiRunResults& aApiRunResults)
	{
	//TInt error = TUptTraceCalls::DoSendTraceL(aApiRunConfig, aApiRunResults); //not implemented
	TInt error = KErrNotSupported;
	aApiRunResults.iError = error;
	}

void DUptUTraceChannel::TimeUptTraces(const TApiRunConfig& /*aApiRunConfig*/, TApiRunResults& aApiRunResults)
	{
/*	TInt error = iTimer.StartBackgroundTimer(); //not implemented
	if(!error)
		{
	    while(!iTimer.IsTimerExpired())
	   	 {
	   	 if(aApiRunConfig.iDoFlush)
	   		 {
	   		 Te_Flush_The_Cache();
	   		 }
	   	 	aApiRunResults.iError = TUptTraceCalls::DoSendTraceL(aApiRunConfig, aApiRunResults);
	   	 	aApiRunResults.iError = KErrNotSupported;
	   	 	iTimer.IncreaseCount();
	   	 }
	    aApiRunResults.iApiTraceTime = iTimer.FinalTime();
	    aApiRunResults.iTimeCount = iTimer.FinalCount();
		}
		*/
 	aApiRunResults.iError = KErrNotSupported;
	}
