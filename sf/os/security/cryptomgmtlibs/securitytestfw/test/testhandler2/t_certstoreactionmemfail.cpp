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

#include "t_certstoreactionmemfail.h"
#include "t_output.h"
#include "t_testhandler.h"
#include "t_testrunner.h"

CMemFailStart* CMemFailStart::NewL(CConsoleBase& aConsole, 
								   Output& aOut,
								   const TTestActionSpec& aTestActionSpec)
	{
	CMemFailStart* self = CMemFailStart::NewLC(aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CMemFailStart* CMemFailStart::NewLC(CConsoleBase& aConsole, 
									Output& aOut,
									const TTestActionSpec& aTestActionSpec)
	{
	CMemFailStart* self = new(ELeave) CMemFailStart(aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CMemFailStart::~CMemFailStart()
	{
	}

void CMemFailStart::PerformAction(TRequestStatus& aStatus)
	{
    iTestHandler->SetTestRunnerL(new (ELeave) COOMTestRunner(iOut));
    
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iResult = ETrue;
	iFinished = ETrue;
	iActionState = EPostrequisite;
	}

void CMemFailStart::PerformCancel()
	{
	}

void CMemFailStart::Reset()
	{
	__ASSERT_DEBUG(EFalse, User::Panic(_L("CMemFailStart::Reset()"), 1));
	}

CMemFailStart::CMemFailStart(CConsoleBase& aConsole, 
							 Output& aOut)
    : CTestAction(aConsole, aOut)
	{
	}

void CMemFailStart::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	}

void CMemFailStart::DoReportAction()
	{
	iOut.writeString(_L("Starting memory failure sequence..."));
	iOut.writeNewLine();
	iOut.writeNewLine();
	}

void CMemFailStart::DoCheckResult(TInt /*aError*/)
	{
	}

CMemFailStop* CMemFailStop::NewL(CConsoleBase& aConsole, 
								 Output& aOut,
								 const TTestActionSpec& aTestActionSpec)
	{
	CMemFailStop* self = CMemFailStop::NewLC(aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CMemFailStop* CMemFailStop::NewLC(CConsoleBase& aConsole, 
								  Output& aOut,
								  const TTestActionSpec& aTestActionSpec)
	{
	CMemFailStop* self = new(ELeave) CMemFailStop(aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CMemFailStop::~CMemFailStop()
	{

	}

void CMemFailStop::PerformAction(TRequestStatus& aStatus)
	{
    iTestHandler->SetTestRunnerL(NULL);
    
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iResult = ETrue;
	iFinished = ETrue;
	iActionState = EPostrequisite;
	}

void CMemFailStop::PerformCancel()
	{
	}

void CMemFailStop::Reset()
	{
//	__ASSERT_DEBUG(EFalse, User::Panic(_L("CMemFailStop::Reset()"), 1));
	}

CMemFailStop::CMemFailStop(CConsoleBase& aConsole, 
						   Output& aOut)
    : CTestAction(aConsole, aOut)
	{
	}

void CMemFailStop::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	}

void CMemFailStop::DoReportAction()
	{
	iOut.writeString(_L("Stoping memory failure sequence..."));
	iOut.writeNewLine();
	iOut.writeNewLine();
	}

void CMemFailStop::DoCheckResult(TInt /*aError*/)
	{
	}
