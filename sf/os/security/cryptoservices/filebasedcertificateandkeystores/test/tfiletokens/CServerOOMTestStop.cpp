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

EXPORT_C CTestAction* CServerOOMTestStop::NewL(RFs& /*aFs*/,
											   CConsoleBase& aConsole, 
											   Output& aOut,
											   const TTestActionSpec& aTestActionSpec)
	{
	CServerOOMTestStop* self = new(ELeave) CServerOOMTestStop(aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CServerOOMTestStop::~CServerOOMTestStop()
	{
	}

void CServerOOMTestStop::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	iActionState = EAction;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CServerOOMTestStop::PerformAction(TRequestStatus& aStatus)
	{
    iTestHandler->SetTestRunnerL(NULL);
    
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iResult = ETrue;
	iFinished = ETrue;
	iActionState = EPostrequisite;
	}

void CServerOOMTestStop::DoPerformPostrequisite(TRequestStatus& aStatus)
	{	
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CServerOOMTestStop::PerformCancel()
	{
	}

void CServerOOMTestStop::Reset()
	{
	}

CServerOOMTestStop::CServerOOMTestStop(CConsoleBase& aConsole, 
									   Output& aOut)
    : CTestAction(aConsole, aOut)
	{
	}

void CServerOOMTestStop::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	}

void CServerOOMTestStop::DoReportAction()
	{
	iOut.writeString(_L("Stoping memory failure sequence..."));
	iOut.writeNewLine();
	iOut.writeNewLine();
	}

void CServerOOMTestStop::DoCheckResult(TInt /*aError*/)
	{
	}
