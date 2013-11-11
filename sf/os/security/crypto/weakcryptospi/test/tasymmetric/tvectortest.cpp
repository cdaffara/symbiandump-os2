/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "tvectortest.h"
#include <e32twin.h>
#include "t_input.h"
#include <asymmetric.h>
#include <random.h>
#include <padding.h>
#include "tvectorutils.h"
#include "performancetest.h"
#include <securityerr.h>

_LIT8(KKeyBitsStart, "<keybits>");
_LIT8(KKeyBitsEnd, "</keybits>");
_LIT8(KResultStart, "<result>");
_LIT8(KResultEnd, "</result>");

////////////////////////////////////////////////////////////////////////////////
// CVectorTest 
////////////////////////////////////////////////////////////////////////////////

CVectorTest::CVectorTest(CConsoleBase& aConsole, Output& aOut)
    : CTestAction(aConsole, aOut), iPerfTestIterations(100)
    {
    }

CVectorTest::~CVectorTest()
    {
    }

void CVectorTest::ConstructL(const TTestActionSpec& aTestActionSpec)
{
	CTestAction::ConstructL(aTestActionSpec);

	if (CPerformance::PerformanceTester()->IsTestingPerformance())
	{//	Number of test iterations
		TPtrC8 itsPtr = Input::ParseElement(aTestActionSpec.iActionBody, KIterationsStart, KIterationsEnd);
		if (itsPtr!=KNullDesC8)
		{
			TLex8 lex;
			lex.Assign(itsPtr);
			User::LeaveIfError(lex.Val(iPerfTestIterations));
			if (iPerfTestIterations > KMaxIterations)
				User::Panic(_L("AsymmetricPerformance.exe"), KErrArgument);
		}
	}

	TPtrC8 keyBitsPtr = Input::ParseElement(aTestActionSpec.iActionBody, KKeyBitsStart, KKeyBitsEnd); 
	if (keyBitsPtr!=KNullDesC8)
	{
		TLex8 lex;
		lex.Assign(keyBitsPtr);
		User::LeaveIfError(lex.Val(iKeyBits));
	}

    TPtrC8 resultIn = Input::ParseElement(aTestActionSpec.iActionResult, KResultStart, KResultEnd);
    iExpectedResult = VectorUtils::ParseBoolL(resultIn);
}

void CVectorTest::PerformAction(TRequestStatus& aStatus)
{
	TRequestStatus* status = &aStatus;
	
	if (CPerformance::PerformanceTester()->IsTestingPerformance())
	{
		iConsole.Printf(_L(">"));	//	Indicates start of test
		TRAP(iActionResult, DoPerformanceTestActionL());
	}
	else
	{
		TRAP(iActionResult, DoPerformActionL());
	}
	
	if (iActionResult==KErrNoMemory)
		{
		User::Leave(iActionResult);	//	For OOM testing
		}
	if (iActionResult==KErrKeyNotWeakEnough)
		{
		iResult = ETrue;
		iConsole.Printf(_L("Crypto libraries returned KErrKeyNotWeakEnough!  Passing test automatically.\n\r"));
		iOut.writeString(_L("Crypto libraries returned KErrKeyNotWeakEnough!  Passing test automatically.\n\r"));
		}

	User::RequestComplete(status, iActionResult);

	iActionState = CTestAction::EPostrequisite;

}

void CVectorTest::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EAction;
	}

void CVectorTest::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	iFinished = ETrue;
	User::RequestComplete(status, KErrNone);
	}

void CVectorTest::DoReportAction(void)
	{
	}

void CVectorTest::DoCheckResult(TInt /*aError*/)
	{
//	If using Keith's fixed up testframework for testing failures, iResult will 
//	have already been corrected for a deliberate fail result.
//	If not using Keith's testframework iResult is still a fail result at this
//	point, so now's the time to adjust for deliberate failure.

	if (!iResult)
		iResult = (iResult && iExpectedResult) || (!iResult && !iExpectedResult);

	if (iResult)
		iConsole.Printf(_L("."));
	else
		iConsole.Printf(_L("X"));
	}
