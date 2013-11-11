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


#include "t_sleep.h"
#include "t_input.h"
#include "t_output.h"
#include <e32base.h>

/////////////////////////////////////////////////////////////////////////////////
// CSleep
/////////////////////////////////////////////////////////////////////////////////

_LIT8(KSecondsStart, "<seconds>");

CTestAction* CSleep::NewL(CConsoleBase& aConsole, 
						  Output& aOut,
						  const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CSleep::NewLC(aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CTestAction* CSleep::NewLC(CConsoleBase& aConsole, 
						   Output& aOut,
						   const TTestActionSpec& aTestActionSpec)
	{
	CSleep* self = new (ELeave) CSleep(aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CSleep::~CSleep()
	{
	iTimer.Close();
	}
	
CSleep::CSleep(CConsoleBase& aConsole, Output& aOut) :
	CTestAction(aConsole, aOut)
	{
 	iState = ESleeping;
	}

void CSleep::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);

	iExpectedResult = KErrNone;
	TLex8 lex(Input::ParseElement(aTestActionSpec.iActionBody, KSecondsStart));
	lex.Val(iSeconds);
	User::LeaveIfError(iTimer.CreateLocal());	
	}
	
void CSleep::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{
		case ESleeping:
			{
			iState = EFinished;
			iTimer.After(aStatus, iSeconds*1000*1000);
			}
			break;
		case EFinished:
			{
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, aStatus.Int());
			if (aStatus == iExpectedResult)
				{
				iResult = ETrue;
				}
			else
				{
				iResult = EFalse;
				}
			iFinished = ETrue;			
			}
		}

	}

void CSleep::PerformCancel()
	{
    if (iState == ESleeping)
        {
        iTimer.Cancel();
        }
	}

void CSleep::DoCheckResult(TInt aError)
	{
	if (iFinished)
		{
		if (aError == KErrNone)
			{
			iOut.write(_L("Slept well \n\n"));
			}
		else
			{
			iOut.write(_L("Troubled sleep... : %d\n\n"), aError);
			}			
		}
	}

void CSleep::DoReportAction()
	{
	iOut.write(_L("Sleeping for %d seconds...\n"), iSeconds);
	}
