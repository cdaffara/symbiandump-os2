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


#include "tprimegenperformance.h"
#include "t_input.h"
#include "t_output.h"
#include <bigint.h>

_LIT(KPrimeGenFormat, "\tPrime Generation Time: %f us/iteration (%i iterations in %f us)\r\n");

const TUint KMaxIterations=100;

CTestAction* CPrimeGenPerformance::NewL(RFs& aFs, CConsoleBase& aConsole, 
	Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CPrimeGenPerformance::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CPrimeGenPerformance::NewLC(RFs& aFs, CConsoleBase& aConsole, 
	Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CPrimeGenPerformance* self = new(ELeave) CPrimeGenPerformance(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CPrimeGenPerformance::~CPrimeGenPerformance()
	{
	delete iBody;
	}

CPrimeGenPerformance::CPrimeGenPerformance(RFs& aFs, CConsoleBase& aConsole, 
	Output& aOut) : CTestAction(aConsole, aOut), iFs(aFs)
	{
	}

void CPrimeGenPerformance::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	iBody = HBufC8::NewL(aTestActionSpec.iActionBody.Length());
	iBody->Des().Copy(aTestActionSpec.iActionBody);

	iBits = Input::ParseIntElement(*iBody, _L8("<bits>"), _L8("</bits>"));
	iIterations = Input::ParseIntElement(*iBody, _L8("<iterations>"), _L8("</iterations>"));	
	__ASSERT_ALWAYS(iIterations < KMaxIterations, User::Panic(_L("Iterations too large"), 1));
	}

void CPrimeGenPerformance::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EAction;
	}

void CPrimeGenPerformance::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	iFinished = ETrue;
	User::RequestComplete(status, KErrNone);
	}

void CPrimeGenPerformance::DoReportAction(void)
	{
	}

void CPrimeGenPerformance::DoCheckResult(TInt)
	{
	}

void CPrimeGenPerformance::PerformAction(TRequestStatus& aStatus)
	{
	__UHEAP_MARK;
	TRequestStatus* status = &aStatus;
	TTime start, end;
	TTimeIntervalSeconds diff(0);
	const TTimeIntervalSeconds iterationTime(iIterations);
	TUint iterations=0;
	
	RInteger prime;

	start.UniversalTime();
	while (diff < iterationTime)
		{
		prime = RInteger::NewPrimeL(iBits, TInteger::ETopBitSet);
		prime.Close();
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

	buf.Format(KPrimeGenFormat, rate, iterations, rtime);
	iOut.writeString(buf);
	iConsole.Printf(_L("."));
	CleanupStack::PopAndDestroy(realbuf);

	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EPostrequisite;
	iResult = ETrue;
	__UHEAP_MARKEND;
	}

