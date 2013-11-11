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

#include "tcancel.h"
#include "t_output.h"
#include "t_testhandler.h"
#include "t_testrunner.h"

CCancelStart* CCancelStart::NewL(CConsoleBase& aConsole, 
								 Output& aOut,
								 const TTestActionSpec& aTestActionSpec)
	{
	CCancelStart* self = CCancelStart::NewLC(aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CCancelStart* CCancelStart::NewLC(CConsoleBase& aConsole, 
								  Output& aOut,
								  const TTestActionSpec& aTestActionSpec)
	{
	CCancelStart* self = new(ELeave) CCancelStart(aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CCancelStart::~CCancelStart()
	{
	}

void CCancelStart::PerformAction(TRequestStatus& aStatus)
	{
    iTestHandler->SetTestRunnerL(new (ELeave) CCancelTestRunner(iOut));
    
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iResult = ETrue;
	iFinished = ETrue;
	iActionState = EPostrequisite;
	}

CCancelStart::CCancelStart(CConsoleBase& aConsole, 
						   Output& aOut)
    : CTestAction(aConsole, aOut)
	{
	}

void CCancelStart::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	}

void CCancelStart::DoReportAction()
	{
	iOut.writeString(_L("Starting cancellation test sequence..."));
	iOut.writeNewLine();
	iOut.writeNewLine();
	}

void CCancelStart::DoCheckResult(TInt /*aError*/)
	{
	}

CCancelStop* CCancelStop::NewL(CConsoleBase& aConsole, 
							   Output& aOut,
							   const TTestActionSpec& aTestActionSpec)
	{
	CCancelStop* self = CCancelStop::NewLC(aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CCancelStop* CCancelStop::NewLC(CConsoleBase& aConsole, 
								Output& aOut,
								const TTestActionSpec& aTestActionSpec)
	{
	CCancelStop* self = new(ELeave) CCancelStop(aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CCancelStop::~CCancelStop()
	{
	}

void CCancelStop::PerformAction(TRequestStatus& aStatus)
	{
    iTestHandler->SetTestRunnerL(NULL);
    
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iResult = ETrue;
	iFinished = ETrue;
	iActionState = EPostrequisite;
	}

CCancelStop::CCancelStop(CConsoleBase& aConsole,
						 Output& aOut)

    : CTestAction(aConsole, aOut)
	{
	}

void CCancelStop::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	}

void CCancelStop::DoReportAction()
	{
	iOut.writeString(_L("Stopping cancel sequence..."));
	iOut.writeNewLine();
	iOut.writeNewLine();
	}

void CCancelStop::DoCheckResult(TInt /*aError*/)
	{
	}
