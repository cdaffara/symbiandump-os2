// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <kernel/kern_priv.h>
#include <platform.h>
#include "d32utrace.h"
#include <e32utrace.h>


//
// Device driver
//

class DUTraceFactory : public DLogicalDevice
	{
public:
	virtual TInt Install();
	virtual void GetCaps(TDes8& aDes) const;
	virtual TInt Create(DLogicalChannelBase*& aChannel);
	};


class DUTraceChannel : public DLogicalChannelBase
	{
public:
	DUTraceChannel();
	virtual ~DUTraceChannel();
	//	Inherited from DObject
	virtual TInt RequestUserHandle(DThread* aThread, TOwnerType aType);
	// Inherited from DLogicalChannelBase
	virtual TInt DoCreate(TInt aUnit, const TDesC8* anInfo, const TVersion& aVer);
	virtual TInt Request(TInt aReqNo, TAny* a1, TAny* a2);
	//
	static void WaitCallback(TAny* aSelf);

	TInt TestTraces(TInt& aFailed);
	TInt KernelBench();
private:
	DThread* iClient;
	TRequestStatus*	iWaitRequest;
	TDfc iWaitDfc;
	};


//
// DBTraceFactory
//

TInt DUTraceFactory::Install()
	{
	return SetName(&RUTrace::Name());
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
	: iWaitDfc(WaitCallback,this,Kern::DfcQue1(),7)
	{
	}

DUTraceChannel::~DUTraceChannel()
	{
	}

TInt DUTraceChannel::DoCreate(TInt /*aUnit*/, const TDesC8* /*aInfo*/, const TVersion& /*aVer*/)
	{
	iClient = &Kern::CurrentThread();
	return KErrNone;
	}


TInt DUTraceChannel::RequestUserHandle(DThread* aThread, TOwnerType aType)
	{
	if (aType!=EOwnerThread || aThread!=iClient)
		return KErrAccessDenied;
	return KErrNone;
	}

void DUTraceChannel::WaitCallback(TAny* aSelf)
	{
	Kern::RequestComplete(((DUTraceChannel*)aSelf)->iClient,((DUTraceChannel*)aSelf)->iWaitRequest,KErrNone);
	}

TInt DUTraceChannel::Request(TInt aReqNo, TAny* a1, TAny* /*a2*/)
	{
	TInt r = KErrNone;
	switch(aReqNo)
		{
	case RUTrace::ETestUTrace:
		{
		r = TestTraces((TInt&)a1);
		return r;
		}
	default:
		break;
		}
	return KErrNotSupported;
	}

TInt DUTraceChannel::TestTraces(TInt& aFailed)
	{
	_LIT(KAnyBuf, "Kernel side, my any buf string size");
	TBufC<16> BufPtr(KAnyBuf);
	aFailed = 0;
	TBool logged = EFalse;
	for(TInt pc = 0; pc <= 1; pc++)
		{
		for(TInt context = 0; context <= 1; context++)
			{
			TUTrace trace(KDebug, KBinary, context,pc);
			logged = trace.Trace();
			if(!logged) aFailed++;
			trace.Trace(333);
			if(!logged) aFailed++;
			trace.Trace(333,444);
			if(!logged) aFailed++;
			trace.Trace(333,444,555);
			if(!logged) aFailed++;
			trace.Trace(BufPtr.Ptr(), BufPtr.Size());
			if(!logged) aFailed++;
			trace.Printf("dfgfdgf");
			if(!logged) aFailed++;

			logged = TUTrace::TracePrimary(KDebug, KBinary, context,pc);
			if(!logged) aFailed++;
			logged = TUTrace::TracePrimary(KDebug, KBinary, context,pc,333);
			if(!logged) aFailed++;
			logged = TUTrace::TracePrimary(KDebug, KBinary, context,pc,333,444);
			if(!logged) aFailed++;
			logged = TUTrace::TracePrimary(KDebug, KBinary, context,pc,333,444,555);
			if(!logged) aFailed++;
			logged = TUTrace::TracePrimary(KDebug, KBinary, context,pc,&KAnyBuf, sizeof(KAnyBuf));
			if(!logged) aFailed++;
			logged = TUTrace::PrintfPrimary(KDebug, context,pc,"dfgfdgf");
			if(!logged) aFailed++;

			TUint32 secondary = 3333;
			logged = TUTrace::Trace(KDebug, secondary, KBinary, context,pc);
			if(!logged) aFailed++;
			logged = TUTrace::Trace(KDebug, secondary, KBinary, context,pc,333);
			if(!logged) aFailed++;
			logged = TUTrace::Trace(KDebug, secondary, KBinary, context,pc,333,444);
			if(!logged) aFailed++;
			logged = TUTrace::Trace(KDebug, secondary, KBinary, context,pc,&KAnyBuf, sizeof(KAnyBuf));
			if(!logged) aFailed++;
			}
		}
	return logged;
	}
TInt DUTraceChannel::KernelBench()
	{
	return 0;
	}


DECLARE_STANDARD_LDD()
{
	return new DUTraceFactory;
}

