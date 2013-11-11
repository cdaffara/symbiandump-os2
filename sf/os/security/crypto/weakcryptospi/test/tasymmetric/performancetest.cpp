/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
*/

#include "performancetest.h"
#include "t_input.h"
#include "tvectorutils.h"

_LIT8(KPerformanceTestStart, "<KPerformanceTesting>");
_LIT8(KPerformanceTestEnd, "</KPerformanceTesting>");

/*static*/ CPerformance* CPerformance::iPerformanceTester = NULL;

/*static*/ CPerformance* CPerformance::NewL()
{
	if (!iPerformanceTester)
	{
		iPerformanceTester = new (ELeave) CPerformance();
	}

	return (iPerformanceTester);
}

/*static*/ CPerformance* CPerformance::PerformanceTester()
{
	ASSERT(iPerformanceTester);
	return (iPerformanceTester);
}

/*static*/ void CPerformance::ClosePerformanceTester()
{
	delete iPerformanceTester;
	iPerformanceTester = NULL;
}


CPerformance::CPerformance()
:	iTestingPerformance(EFalse)	//	Default to no performance testing
{}

CPerformance::~CPerformance()
{}

////////////////////////////////////////////////////////////////////////
//	Use this class as a place holder to turn performance testing on/off
////////////////////////////////////////////////////////////////////////
	
CTestAction* CPerformanceTest::NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec)
{
	CTestAction* me = CPerformanceTest::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(me);
	return (me);
}

CTestAction* CPerformanceTest::NewLC(RFs& /*aFs*/, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec)
{
	CPerformanceTest* me = new (ELeave) CPerformanceTest(aConsole, aOut);
	CleanupStack::PushL(me);
	me->ConstructL(aTestActionSpec);
	return (static_cast<CTestAction*>(me));
}


CPerformanceTest::CPerformanceTest(CConsoleBase& aConsole, Output& aOut)
    : CTestAction(aConsole, aOut)
    {
    }

CPerformanceTest::~CPerformanceTest()
    {
    }

void CPerformanceTest::ConstructL(const TTestActionSpec& aTestActionSpec)
{
	CTestAction::ConstructL(aTestActionSpec);

//	Switch on performance testing?    
	TPtrC8 enablePtr = Input::ParseElement(aTestActionSpec.iActionBody, KPerformanceTestStart, KPerformanceTestEnd);
    TBool enablePerformance = VectorUtils::ParseBoolL(enablePtr);
	CPerformance::PerformanceTester()->SetTestingPerformance(enablePerformance);
	
	iExpectedResult = KErrNone;
}

void CPerformanceTest::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EAction;
	}

void CPerformanceTest::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	iFinished = ETrue;
	User::RequestComplete(status, KErrNone);
	}

void CPerformanceTest::PerformAction(TRequestStatus& aStatus)
{
	iResult = ETrue;
	iFinished = ETrue;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);	
}

