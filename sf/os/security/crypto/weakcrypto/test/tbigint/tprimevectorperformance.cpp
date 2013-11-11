/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include "tprimevectorperformance.h"
#include "t_input.h"
#include "t_output.h"
#include <bigint.h>

_LIT(KPrimeVectorFormat, "\tPrime Vector Primality Test Time: %f us/iteration (%i iterations in %f us)\r\n");

CTestAction* CPrimeVectorPerformance::NewL(RFs& aFs, CConsoleBase& aConsole, 
	Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CPrimeVectorPerformance::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CPrimeVectorPerformance::NewLC(RFs& aFs, CConsoleBase& aConsole, 
	Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CPrimeVectorPerformance* self = new(ELeave) CPrimeVectorPerformance(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CPrimeVectorPerformance::~CPrimeVectorPerformance()
	{
	delete iBody;
	}

CPrimeVectorPerformance::CPrimeVectorPerformance(RFs& aFs, CConsoleBase& aConsole, 
	Output& aOut) : CTestAction(aConsole, aOut), iFs(aFs)
	{
	}

void CPrimeVectorPerformance::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	iBody = HBufC8::NewL(aTestActionSpec.iActionBody.Length());
	iBody->Des().Copy(aTestActionSpec.iActionBody);

	HBufC8* buf = Input::ParseElementHexL(*iBody, _L8("<prime>"));
	CleanupStack::PushL(buf);
	iPrime = RInteger::NewL(*buf);
	CleanupStack::PopAndDestroy(buf);
	iIterations = Input::ParseIntElement(*iBody, _L8("<iterations>"), _L8("</iterations>"));	
	}

void CPrimeVectorPerformance::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EAction;
	}

void CPrimeVectorPerformance::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	iFinished = ETrue;
	User::RequestComplete(status, KErrNone);
	}

void CPrimeVectorPerformance::DoReportAction(void)
	{
	}

void CPrimeVectorPerformance::DoCheckResult(TInt)
	{
	}

void CPrimeVectorPerformance::PerformAction(TRequestStatus& aStatus)
	{
	__UHEAP_MARK;
	TRequestStatus* status = &aStatus;
	TTime start, end;
	TTimeIntervalSeconds diff(0);
	const TTimeIntervalSeconds iterationTime(iIterations);
	TUint iterations = 0;	
	
	iResult = ETrue;

	start.UniversalTime();
	while (diff < iterationTime)
		{
		iResult = iPrime.IsPrimeL();
		if(!iResult)
			break;
		iterations++;
		end.UniversalTime();
		end.SecondsFrom(start, diff);
		}
	end.UniversalTime();

	TTimeIntervalMicroSeconds time = end.MicroSecondsFrom(start);
	TReal rate = I64REAL(time.Int64()) / iterations;
	TReal rtime = I64REAL(time.Int64());

	HBufC* realbuf = HBufC::NewLC(128);
	TPtr buf = realbuf->Des();
	buf.Format(KPrimeVectorFormat, rate, iterations, rtime);
	iOut.writeString(buf);
	iConsole.Printf(_L("."));
	CleanupStack::PopAndDestroy(realbuf);

	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EPostrequisite;
	__UHEAP_MARKEND;
	}

