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


#include <unifiedkeystore.h>
#include <e32base.h>
#include "t_keystore_actions.h"
#include "t_keystore_defs.h"
#include "t_input.h"
#include "t_output.h"

/////////////////////////////////////////////////////////////////////////////////
// CSetClientUid
/////////////////////////////////////////////////////////////////////////////////

CTestAction* CSetClientUid::NewL(RFs& aFs, 
							 CConsoleBase& aConsole, 
							 Output& aOut,
							 const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CSetClientUid::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CTestAction* CSetClientUid::NewLC(RFs& aFs,
							  CConsoleBase& aConsole, 
							  Output& aOut,
							  const TTestActionSpec& aTestActionSpec)
	{
	CSetClientUid* self = new (ELeave) CSetClientUid(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CSetClientUid::~CSetClientUid()
	{
	}

CSetClientUid::CSetClientUid(RFs& aFs, CConsoleBase& aConsole, Output& aOut) :
	CKeyStoreTestAction(aFs, aConsole, aOut), iState(EMain)
	{
	}

void CSetClientUid::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CKeyStoreTestAction::ConstructL(aTestActionSpec);

	TPtrC8 data = Input::ParseElement(aTestActionSpec.iActionBody, KUidStart);
	if (data.Length() == 0)
		User::Leave(KErrNotFound);
	
	TLex8 lex(data);
	lex.Val(iUid.iUid);
	}

void CSetClientUid::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{
		case EMain:
			{
			RProcess thisProcess;
			thisProcess.Open(thisProcess.Id());
			CleanupClosePushL(thisProcess);
			TUidType type = thisProcess.Type();
			TUidType newType(type[0], type[1], iUid);
			thisProcess.SetType(newType);
			CleanupStack::PopAndDestroy(); // thisProcess
			
			iState = EFinished;
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, KErrNone);
			break;
			}

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
			
			iActionState = EPostrequisite;
			}
			break;
		}
	}

void CSetClientUid::PerformCancel()
	{
    // nothing to cancel
	}

void CSetClientUid::Reset()
	{
	iState = EMain;
	}

void CSetClientUid::DoReportAction()
	{
	iOut.writeString(_L("Setting users..."));
	iOut.writeNewLine();
	}


void CSetClientUid::DoCheckResult(TInt aError)
	{
	if (iFinished)
		{
		if (aError == KErrNone)
			{
			_LIT(KSuccessful, "Client UID set successfully\n");
			iConsole.Write(KSuccessful);
			iOut.writeString(KSuccessful);
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		else
			{
			if (aError!=iExpectedResult)
				{
				_LIT(KFailed, "!!!Set UID  failure!!!\n");
				iConsole.Write(KFailed);
				iOut.writeString(KFailed);
				}
			else
				{
				_LIT(KFailed, "Set UID failed, but expected\n");
				iConsole.Write(KFailed);
				iOut.writeString(KFailed);
				}

			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		}
	}


