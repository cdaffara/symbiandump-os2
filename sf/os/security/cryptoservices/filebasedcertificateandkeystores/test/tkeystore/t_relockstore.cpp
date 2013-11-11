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
// CRelockStore
/////////////////////////////////////////////////////////////////////////////////

CTestAction* CRelockStore::NewL(RFs& aFs, 
							 CConsoleBase& aConsole, 
							 Output& aOut,
							 const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CRelockStore::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CTestAction* CRelockStore::NewLC(RFs& aFs,
							  CConsoleBase& aConsole, 
							  Output& aOut,
							  const TTestActionSpec& aTestActionSpec)
	{
	CRelockStore* self = new (ELeave) CRelockStore(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CRelockStore::~CRelockStore()
	{
	}

CRelockStore::CRelockStore(RFs& aFs, CConsoleBase& aConsole, Output& aOut) :
	CKeyStoreTestAction(aFs, aConsole, aOut), iState(EMain)
	{
	}

void CRelockStore::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CKeyStoreTestAction::ConstructL(aTestActionSpec);
	}

void CRelockStore::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{
		case EMain:
			{						
			CUnifiedKeyStore& keystore = *CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);

			iState = EFinished;
			keystore.Relock(aStatus);
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

void CRelockStore::PerformCancel()
	{
    CUnifiedKeyStore* keystore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
    ASSERT(keystore);
    if (iState == EFinished)
        {
        keystore->CancelImportKey();
        }
	}

void CRelockStore::Reset()
	{
	iState = EMain;
	}

void CRelockStore::DoReportAction()
	{
	}

void CRelockStore::DoCheckResult(TInt aError)
	{
	if (iFinished)
		{
		if (aError == KErrNone)
			{
			_LIT(KSuccessful, "Relocked successfully\n");
			iConsole.Write(KSuccessful);
			iOut.writeString(KSuccessful);
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		else
			{
			if (aError!=iExpectedResult)
				{
				_LIT(KFailed, "!!!Relock failure!!!\n");
				iConsole.Write(KFailed);
				iOut.writeString(KFailed);
				}
			else
				{
				_LIT(KFailed, "Relock failed, but expected\n");
				iConsole.Write(KFailed);
				iOut.writeString(KFailed);
				}

			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		}
	}
