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

#include "te_d32perfdrv.h"

#ifndef __KERNEL_MODE__

TInt RUptUTrace::Open()
	{
	//this loads the driver in by using the filename 
	//of the driver (adding the ldd extension if missing)
	TInt r = User::LoadLogicalDevice(Name());
	if(r!=KErrNone && r!=KErrAlreadyExists)
		return r;
	//this uses the name of the logical device 
	//(the name of the LDD factory object, not the 
	//filename of the LDD DLL)

#ifdef TE_UPT_TRACE_ENABLED
	r = DoCreate(KUptUTraceDDEnabled, TVersion(), KNullUnit, NULL, NULL, EOwnerThread);
#else
	r = DoCreate(KUptUTraceDDDisabled, TVersion(), KNullUnit, NULL, NULL, EOwnerThread);
#endif
		
	return r;
	};

TInt RUptUTrace::TestUptUtrace(const TApiRunConfig& aApiRunConfig, TApiRunResults& aApiRunResults)
	{
	TRequestStatus status;
	status = KRequestPending;
	DoRequest(ETestUTrace, status, (TAny*)&aApiRunConfig,(TAny*)&aApiRunResults);
	User::WaitForRequest(status);
	if(!aApiRunResults.iError)
		aApiRunResults.iError = status.Int();
	return aApiRunResults.iError;
	}

TInt RUptUTrace::TimeUptUtrace(const TApiRunConfig& aApiRunConfig, TApiRunResults& aApiRunResults)
	{
	TRequestStatus status;
	status = KRequestPending;
	DoRequest(ETimeUTrace, status, (TAny*)&aApiRunConfig,(TAny*)&aApiRunResults);
	User::WaitForRequest(status);
	if(!aApiRunResults.iError)
		aApiRunResults.iError = status.Int();
	return aApiRunResults.iError;
	}

TInt RUptUTrace::GetTraceWordPtr(const TApiRunConfig& aApiRunConfig, TApiRunResults& aApiRunResults)
	{
	TRequestStatus status;
	status=KRequestPending;
	DoRequest(EGetTraceWordPtr, status, (TAny*) &aApiRunConfig, (TAny*) &aApiRunResults);
	User::WaitForRequest(status);
	if(!aApiRunResults.iError)
		aApiRunResults.iError=status.Int();
	return aApiRunResults.iError;	
	}

TBool RUptUTrace::SanityTestTimer(TUint32& aTestTime)
	{
	return DoSanityTimerTest(ESanityTestTimer, aTestTime);	
	}

TBool RUptUTrace::SanityUtraceTestTimer(TUint32& aTestTime)
	{
	return DoSanityTimerTest(ESanityUtraceTimer, aTestTime);	
	}

TBool RUptUTrace::SanityTestLongTimer(TUint32& aTestTime)
	{
	return DoSanityTimerTest(ESanityTestLongTimer, aTestTime);		
	}
	
TBool RUptUTrace::DoSanityTimerTest(const TControl aTest, TUint32& aTestTime)
	{
	TRequestStatus status;
	status = KRequestPending;
	TSanityResults results;
	DoRequest(aTest, status, (TAny*)&results);
	User::WaitForRequest(status);
	aTestTime = results.iTime;
	return results.iPass;	
	}

TInt RUptUTrace::Close()
	{
	RBusLogicalChannel::Close();
	return User::FreeLogicalDevice(RUptUTrace::Name());
	}

#endif

//eof


