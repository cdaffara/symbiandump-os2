/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "tmontgomeryperformance.h"
#include "t_input.h"
#include "t_output.h"
#include <bigint.h>
#include "../../source/bigint/mont.h"

_LIT(KPerfConstFormat, "\tConstruction Time: %f us/iteration (%i iterations in %f us)\r\n");
_LIT(KPerfMultiplyFormat, "\tMultiply Time: %f us/iteration (%i iterations in %f us)\r\n");
_LIT(KPerfSquareFormat, "\tSquare Time: %f us/iteration (%i iterations in %f us)\r\n");
_LIT(KPerfExpFormat, "\tExponentiate Time: %f us/iteration (%i iterations in %f us)\r\n");
_LIT(KPerfReduceFormat, "\tReduce Time: %f us/iteration (%i iterations in %f us)\r\n");

CTestAction* CMontgomeryPerformance::NewL(RFs& aFs, CConsoleBase& aConsole, 
	Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CMontgomeryPerformance::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CMontgomeryPerformance::NewLC(RFs& aFs, CConsoleBase& aConsole, 
	Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CMontgomeryPerformance* self = new(ELeave) CMontgomeryPerformance(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CMontgomeryPerformance::~CMontgomeryPerformance()
	{
	delete iBody;
	iA.Close();
	iB.Close();
	iModulus.Close();
	}

CMontgomeryPerformance::CMontgomeryPerformance(RFs& aFs, CConsoleBase& aConsole, 
	Output& aOut) : CTestAction(aConsole, aOut), iFs(aFs)
	{
	}

void CMontgomeryPerformance::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	iBody = HBufC8::NewL(aTestActionSpec.iActionBody.Length());
	iBody->Des().Copy(aTestActionSpec.iActionBody);

	iIterations = Input::ParseIntElement(*iBody, _L8("<iterations>"), _L8("</iterations>"));

	TUint bits = Input::ParseIntElement(*iBody, _L8("<bits>"), _L8("</bits>"));
	
	do 
		{
		if(*((TUint*)&iModulus)) //if the size is zero, ie iModulus hasn't been initialised so don't clean it up
			{
			iModulus.Close();
			}
		iModulus = RInteger::NewRandomL(bits);
		}
	while(iModulus.IsEven());

	iA = RInteger::NewRandomL(TInteger::One(), iModulus);
	iB = RInteger::NewRandomL(TInteger::One(), iModulus);
	}

void CMontgomeryPerformance::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EAction;
	}

void CMontgomeryPerformance::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	iFinished = ETrue;
	User::RequestComplete(status, KErrNone);
	}

void CMontgomeryPerformance::DoReportAction(void)
	{
	}

void CMontgomeryPerformance::DoCheckResult(TInt)
	{
	}

void CMontgomeryPerformance::PerformAction(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	iResult = ETrue;

	CMontgomeryStructure* montConst;

	TUint iterations = 0;

	TTime start, end;
	TTimeIntervalSeconds diff(0);
	const TTimeIntervalSeconds iterationTime(iIterations);
	
	start.UniversalTime();
	while (diff < iterationTime)
		{
		montConst = CMontgomeryStructure::NewL(iModulus);
		delete montConst;
		iterations++;
		end.UniversalTime();
		end.SecondsFrom(start, diff);
		}
	end.UniversalTime();

	TTimeIntervalMicroSeconds time = end.MicroSecondsFrom(start);
	TReal rate = I64REAL(time.Int64()) / (iterations);
	TReal rtime = I64REAL(time.Int64());
    HBufC* realbuf = HBufC::NewLC(128);
	TPtr buf = realbuf->Des();
	buf.Format(KPerfConstFormat, rate, iterations, rtime);
	iOut.writeString(buf);
	iConsole.Printf(_L("."));

	CMontgomeryStructure* mont = CMontgomeryStructure::NewLC(iModulus); 

//Multiply
	diff  = 0;
	iterations = 0;
	start.UniversalTime();
	while (diff < iterationTime)
		{
		mont->MultiplyL(iA, iB);
		iterations++;
		end.UniversalTime();
		end.SecondsFrom(start, diff);
		}
	end.UniversalTime();

	time = end.MicroSecondsFrom(start);
	rate = I64REAL(time.Int64()) / iterations;
    rtime = I64REAL(time.Int64());
	buf.Zero();
	buf.Format(KPerfMultiplyFormat, rate, iterations, rtime);
	iOut.writeString(buf);
	iConsole.Printf(_L("."));

//Square
	diff  = 0;
	iterations = 0;
	start.UniversalTime();
	while (diff < iterationTime)
		{
		mont->SquareL(iA);
		iterations++;
		end.UniversalTime();
		end.SecondsFrom(start, diff);
		}
	end.UniversalTime();

	time = end.MicroSecondsFrom(start);
	rate = I64REAL(time.Int64()) / iterations;
    rtime = I64REAL(time.Int64());
	buf.Zero();
	buf.Format(KPerfSquareFormat, rate, iterations, rtime);
	iOut.writeString(buf);
	iConsole.Printf(_L("."));

//Exponentiate
	diff  = 0;
	iterations = 0;
	start.UniversalTime();
	while (diff < iterationTime)
		{
		mont->ExponentiateL(iA, iB);
		iterations++;
		end.UniversalTime();
		end.SecondsFrom(start, diff);
		}
	end.UniversalTime();

	time = end.MicroSecondsFrom(start);
	rate = I64REAL(time.Int64()) / iterations;
    rtime = I64REAL(time.Int64());

	buf.Zero();
	buf.Format(KPerfExpFormat, rate, iterations, rtime);
	iOut.writeString(buf);
	iConsole.Printf(_L("."));

//Reduce
	RInteger top = iA.TimesL(iB);
	CleanupStack::PushL(top);

	diff  = 0;
	iterations = 0;
	start.UniversalTime();
	while (diff < iterationTime)
		{
		mont->ReduceL(top);
		iterations++;
		end.UniversalTime();
		end.SecondsFrom(start, diff);
		}
	end.UniversalTime();

	CleanupStack::PopAndDestroy(&top);

	time = end.MicroSecondsFrom(start);
	rate = I64REAL(time.Int64()) / iterations;
    rtime = I64REAL(time.Int64());
	buf.Zero();
	buf.Format(KPerfReduceFormat, rate, iterations, rtime);
	iOut.writeString(buf);
	iConsole.Printf(_L("."));
	CleanupStack::PopAndDestroy(mont);
	CleanupStack::PopAndDestroy(realbuf);

	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EPostrequisite;
	}

