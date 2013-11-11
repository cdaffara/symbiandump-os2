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
// CSetUsers
/////////////////////////////////////////////////////////////////////////////////

CTestAction* CSetUsers::NewL(RFs& aFs, 
							 CConsoleBase& aConsole, 
							 Output& aOut,
							 const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CSetUsers::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CTestAction* CSetUsers::NewLC(RFs& aFs,
							  CConsoleBase& aConsole, 
							  Output& aOut,
							  const TTestActionSpec& aTestActionSpec)
	{
	CSetUsers* self = new (ELeave) CSetUsers(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CSetUsers::~CSetUsers()
	{
	iUsers.Close();
	iKeys.Close();
	}

CSetUsers::CSetUsers(RFs& aFs, CConsoleBase& aConsole, Output& aOut) :
	CKeyStoreTestAction(aFs, aConsole, aOut), iState(EListKeys)
	{
	}

void CSetUsers::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CKeyStoreTestAction::ConstructL(aTestActionSpec);

	// Set users
	TInt err = KErrNone;
	TInt pos = 0;
	while (AddUserL(Input::ParseElement(aTestActionSpec.iActionBody, KUserStart, KUserEnd, pos, err)))
		/* do nothing */;
	}

TBool CSetUsers::AddUserL(const TDesC8& aData)
	{
	if (aData.Length() == 0)
		{
		return EFalse;
		}
	
	TLex8 lex(aData);
	TUid uid;
	lex.Val(uid.iUid);
	User::LeaveIfError(iUsers.Append(uid));

	return ETrue;
	}

void CSetUsers::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{
		case EListKeys:
			{
			// Currently uses first store
			CUnifiedKeyStore& keystore = *CSharedKeyStores::TheUnifiedKeyStores().operator[](0);

			TCTKeyAttributeFilter filter;
			keystore.List(iKeys, filter, aStatus);			
			iState = EMain;
			break;
			}

		case EMain:
			{			
			iState = EFinished;
			TRequestStatus* status = &aStatus;
			
			if (iKeys.Count() == 0)
				{
				User::RequestComplete(status, KErrNotFound);
				return;
				}

			const CCTKeyInfo* key = NULL;
			for (TInt index = 0 ; index < iKeys.Count() ; ++index)
				{
				const CCTKeyInfo* k = iKeys[index];
				if (k->Label() == *iLabel)
					{
					key = k;
					break;
					}
				}

			if (!key)
				{
				iOut.writeString(_L("Key not found: "));
				iOut.writeString(*iLabel);
				iOut.writeNewLine();
				User::RequestComplete(status, KErrNotFound);
				return;
				}


			// Currently uses first store
			CUnifiedKeyStore& keystore = *CSharedKeyStores::TheUnifiedKeyStores().operator[](0);

			keystore.SetUsers(*key, iUsers, aStatus);
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

void CSetUsers::PerformCancel()
	{
    CUnifiedKeyStore* keystore = CSharedKeyStores::TheUnifiedKeyStores().operator[](0);
    ASSERT(keystore);
    
    switch (iState)
        {
        case EMain:
            keystore->CancelList();
            break;

        case EFinished:
            keystore->CancelSetUsers();
            break;
            
        default:
            break;
        }
	}

void CSetUsers::Reset()
	{
	iState = EListKeys;
	iKeys.Close();
	}

void CSetUsers::DoReportAction()
	{
	iOut.writeString(_L("Setting users..."));
	iOut.writeNewLine();
	}


void CSetUsers::DoCheckResult(TInt aError)
	{
	if (iFinished)
		{
		if (aError == KErrNone)
			{
			_LIT(KSuccessful, "Users set successfully\n");
			iConsole.Write(KSuccessful);
			iOut.writeString(KSuccessful);
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		else
			{
			if (aError!=iExpectedResult)
				{
				_LIT(KFailed, "!!!Set users failure!!!\n");
				iConsole.Write(KFailed);
				iOut.writeString(KFailed);
				}
			else
				{
				_LIT(KFailed, "Set users failed, but expected\n");
				iConsole.Write(KFailed);
				iOut.writeString(KFailed);
				}

			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		}
	}


