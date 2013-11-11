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
// Name		: CTe_multiparttrace.h
// Version	 : 1.0
// CCTe_multiparttrace declaration
//



#ifndef _CTE_MULTIPARTTRACE_H
#define _CTE_MULTIPARTTRACE_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <test/testexecutestepbase.h>
#include <e32math.h>
#include <uloggerclient.h>
#include "te_suite_step_base.h"
#include "te_suite_defs.h"

_LIT(KMultiTraceTestStep, "CMultiTraceTestStep");


/**
 *  CCTe_multiparttrace
 * 
 */
class CCTe_multiparttrace : public CTe_UTraceSuiteStepBase
	{
public:
	enum TTraceMethod
		{
		ETraceUsingPrintF = 0,
		ETraceUsingTraceBig,
		};
	
	// Constructor and destructor
	CCTe_multiparttrace();
	~CCTe_multiparttrace();

	//from CTe_ULoggerMCLSuiteStepBase
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	TInt GenerateTraceDataL(TTraceMethod aTraceMethod, TUint aMaxDataSize, TUint8 aFillPattern);
	TInt CheckDataL(TTraceMethod aTraceMethod, TUint aMaxDataSize, TUint8 aFillPattern);
	TInt ValidateTraceDataL(const TUint8* aDataPtr, TUint aDataSize, TUint aTraceRecords, TUint8 aFillPattern);
	TInt ValidateTracePayloadL(const TUint8* aDataPtr, TUint aDataSize, TInt aTotalTraceSize, TBool aMultiPart, TUint8 aExpectedFirstVa, TBool aFirstTrace, TBool aLastTrace);

	//data
	Ulogger::RULogger iUlogger;
	};

#endif // CTE_MULTIPARTTRACE_H
