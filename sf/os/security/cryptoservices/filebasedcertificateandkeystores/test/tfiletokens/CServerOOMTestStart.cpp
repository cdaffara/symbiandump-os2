/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "t_filetokens.h"
#include "t_output.h"
#include "t_testhandler.h"
#include "CServerOOMTestRunner.h"

EXPORT_C CTestAction* CServerOOMTestStart::NewL(RFs& /*aFs*/,
												CConsoleBase& aConsole, 
												Output& aOut,
												const TTestActionSpec& aTestActionSpec)
	{
	CServerOOMTestStart* self = new(ELeave) CServerOOMTestStart(aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CServerOOMTestStart::~CServerOOMTestStart()
	{
	}

void CServerOOMTestStart::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	iActionState = EAction;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CServerOOMTestStart::PerformAction(TRequestStatus& aStatus)
	{
    iTestHandler->SetTestRunnerL(new (ELeave) CServerOOMTestRunner(iOut));
    
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iResult = ETrue;
	iFinished = ETrue;
	iActionState = EPostrequisite;
	}

void CServerOOMTestStart::DoPerformPostrequisite(TRequestStatus& aStatus)
	{	
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CServerOOMTestStart::PerformCancel()
	{
	}

void CServerOOMTestStart::Reset()
	{
	}

CServerOOMTestStart::CServerOOMTestStart(CConsoleBase& aConsole, 
										 Output& aOut)
    : CTestAction(aConsole, aOut)
	{
	}

void CServerOOMTestStart::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	}

void CServerOOMTestStart::DoReportAction()
	{
	iOut.writeString(_L("Starting memory failure sequence..."));
	iOut.writeNewLine();
	iOut.writeNewLine();
	}

void CServerOOMTestStart::DoCheckResult(TInt /*aError*/)
	{
	}
