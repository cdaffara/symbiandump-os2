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
// CSetTimeout
/////////////////////////////////////////////////////////////////////////////////

CTestAction* CSetTimeout::NewL(RFs& aFs, 
							 CConsoleBase& aConsole, 
							 Output& aOut,
							 const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CSetTimeout::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CTestAction* CSetTimeout::NewLC(RFs& aFs,
							  CConsoleBase& aConsole, 
							  Output& aOut,
							  const TTestActionSpec& aTestActionSpec)
	{
	CSetTimeout* self = new (ELeave) CSetTimeout(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CSetTimeout::~CSetTimeout()
	{
	}

CSetTimeout::CSetTimeout(RFs& aFs, CConsoleBase& aConsole, Output& aOut) :
	CKeyStoreTestAction(aFs, aConsole, aOut), iState(EMain)
	{
	}

void CSetTimeout::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CKeyStoreTestAction::ConstructL(aTestActionSpec);

	TPtrC8 data = Input::ParseElement(aTestActionSpec.iActionBody, KTimeoutStart);
	if (data.Length() == 0)
		User::Leave(KErrNotFound);
	TLex8 lex(data);
	lex.Val(iNewTimeout);
	}

void CSetTimeout::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{
		case EMain:
			{						
			CUnifiedKeyStore& keystore = *CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);

			iState = EFinished;
			keystore.SetPassphraseTimeout(iNewTimeout, aStatus);
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

void CSetTimeout::PerformCancel()
	{
    CUnifiedKeyStore* keystore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
    ASSERT(keystore);
    if (iState == EFinished)
        {
        keystore->CancelSetPassphraseTimeout();
        }
	}

void CSetTimeout::Reset()
	{
	iState = EMain;
	}

void CSetTimeout::DoReportAction()
	{
	iOut.writeString(_L("Setting timeout..."));
	iOut.writeNewLine();
	}


void CSetTimeout::DoCheckResult(TInt aError)
	{
	if (iFinished)
		{
		if (aError == KErrNone)
			{
			_LIT(KSuccessful, "Timeout set successfully\n");
			iConsole.Write(KSuccessful);
			iOut.writeString(KSuccessful);
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		else
			{
			if (aError!=iExpectedResult)
				{
				_LIT(KFailed, "!!!Set timeout failure!!!\n");
				iConsole.Write(KFailed);
				iOut.writeString(KFailed);
				}
			else
				{
				_LIT(KFailed, "Set timeout failed, but expected\n");
				iConsole.Write(KFailed);
				iOut.writeString(KFailed);
				}

			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		}
	}
