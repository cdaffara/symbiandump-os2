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
// CSetTime
/////////////////////////////////////////////////////////////////////////////////

CTestAction* CSetTime::NewL(RFs& aFs, 
							 CConsoleBase& aConsole, 
							 Output& aOut,
							 const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CSetTime::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CTestAction* CSetTime::NewLC(RFs& aFs,
							  CConsoleBase& aConsole, 
							  Output& aOut,
							  const TTestActionSpec& aTestActionSpec)
	{
	CSetTime* self = new (ELeave) CSetTime(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CSetTime::~CSetTime()
	{
	}

CSetTime::CSetTime(RFs& aFs, CConsoleBase& aConsole, Output& aOut) :
	CKeyStoreTestAction(aFs, aConsole, aOut), iState(EMain)
	{
	}

void CSetTime::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CKeyStoreTestAction::ConstructL(aTestActionSpec);

	TPtrC8 data = Input::ParseElement(aTestActionSpec.iActionBody, KHomeTimeStart);
	SetNewTimeL(data);
	}

void CSetTime::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{
		case EMain:
			{
			User::LeaveIfError(User::SetHomeTime(iNewTime));
			
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

void CSetTime::PerformCancel()
	{
    // nothing to cancel
	}

void CSetTime::Reset()
	{
	iState = EMain;
	}

void CSetTime::DoReportAction()
	{
	iOut.writeString(_L("Setting time..."));
	iOut.writeNewLine();
	}

void CSetTime::DoCheckResult(TInt aError)
	{
	if (iFinished)
		{
		if (aError == KErrNone)
			{
			_LIT(KSuccessful, "Time set successfully\n");
			iConsole.Write(KSuccessful);
			iOut.writeString(KSuccessful);
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		else
			{
			if (aError!=iExpectedResult)
				{
				_LIT(KFailed, "!!!Set time failure!!!\n");
				iConsole.Write(KFailed);
				iOut.writeString(KFailed);
				}
			else
				{
				_LIT(KFailed, "Set time failed, but expected\n");
				iConsole.Write(KFailed);
				iOut.writeString(KFailed);
				}

			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		}
	}

void CSetTime::SetNewTimeL(const TDesC8& aData)
	{
	HBufC* buf = HBufC::NewLC(aData.Length());
	TPtr ptr = buf->Des();
	ptr.Copy(aData);	
	User::LeaveIfError(iNewTime.Parse(*buf));
	CleanupStack::PopAndDestroy(buf);
	}
