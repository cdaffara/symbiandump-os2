// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Ost Performance Tests API Test Object
//


/**
 @file te_perfapicall.h
 @internalTechnology
 @prototype
*/

#ifndef TE_UPTAPICALLTESTS_H
#define TE_UPTAPICALLTESTS_H

#ifndef __KERNEL_MODE__
#include <e32base.h>
#include <f32file.h>
#endif
#include <opensystemtrace.h>
#include "te_tracecontrolcmds.h"
#include "te_apirunconfig.h"


class CApiCallTest: public CBase
	{
public:
	CApiCallTest();
	~CApiCallTest();
	//could get rid of this now if you want...
	TInt DoTestL(const TApiRunConfig& aApiRunConfig, TApiTestResult& aApiTestResult);
	TInt CachedTraceTimeL(TApiRunConfig& aApiRunConfig, TApiTestResult& aApiTestResult);
	TInt NonCachedTraceTimeL(TApiRunConfig& aApiRunConfig, TApiTestResult& aApiTestResult);
	TInt StackUsageL(TApiRunConfig& aApiRunConfig, TApiTestResult& aApiTestResult);

	TInt Test();
private:
//performance test functions
//e.g.
//	ApiTestResult fred;
//	fred.minexecutintime = 009;
//	return fred;
	TInt FlushTheCache();
	//could really tidy these up to just 2routines now - leave for now tho

	//TInt TraceBaselineTime(const TApiRunConfig& aApiRunConfig, TUint64& aBaselineTime, TBool aDoTrace);
	//TInt TraceTime(const TApiRunConfig& aApiRunConfig, TUint64& aTime, const TUint64& aBaselineTime, TBool aDoTrace);
	TInt CachedTraceBaselineTimeL(TApiRunConfig& aApiRunConfig, TInt32& aMinimumExecutionBaselineTime);
	TInt CachedTraceTimeL(TApiRunConfig& aApiRunConfig, TInt32& aMinimumExecutionTime, TInt32& aMinimumExecutionBaselineTime);
	TInt NonCachedTraceBaselineTimeL(TApiRunConfig& aApiRunConfig, TInt32& aTypicalExecutionBaselineTime);
	TInt NonCachedTraceTimeL(TApiRunConfig& aApiRunConfig, TInt32& aTypicalExecutionTime, TInt32& aTypicalExecutionBaselineTime);

	TInt StackUsageL(TApiRunConfig& aApiRunConfig, TInt& aApiStackUsage);

	TInt HeapUsage(TInt& aApiHeapUsage);
	TInt DoTheApiCall();
	TInt DoTheCommentedApiCall();

	//any tests for the private functions
	TInt TestStackUsage();

//	TApiTestResult iApiTestResult;
	};

#endif
