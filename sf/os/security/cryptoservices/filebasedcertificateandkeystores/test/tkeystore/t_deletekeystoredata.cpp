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

#include <e32uid.h>
#include "t_keystore_actions.h"
#include "t_keystore_defs.h"
#include "t_input.h"

/////////////////////////////////////////////////////////////////////////////////
// CDeleteKeyStoreData
/////////////////////////////////////////////////////////////////////////////////

CTestAction* CDeleteKeyStoreData::NewL(RFs& aFs, 
									   CConsoleBase& aConsole, 
									   Output& aOut,
									   const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CDeleteKeyStoreData::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CTestAction* CDeleteKeyStoreData::NewLC(RFs& aFs,
										CConsoleBase& aConsole, 
										Output& aOut,
										const TTestActionSpec& aTestActionSpec)
	{
	CDeleteKeyStoreData* self = new(ELeave) CDeleteKeyStoreData(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CDeleteKeyStoreData::~CDeleteKeyStoreData()
	{
	}

CDeleteKeyStoreData::CDeleteKeyStoreData(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
	: CKeyStoreTestAction(aFs, aConsole, aOut), iState(EDelete)
	{
	}

void CDeleteKeyStoreData::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CKeyStoreTestAction::ConstructL(aTestActionSpec);
	}

void CDeleteKeyStoreData::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{
		case EDelete:
			{
			TDriveUnit sysDrive (RFs::GetSystemDrive());
			TBuf<128> keystoreDataPath (sysDrive.Name());
			keystoreDataPath.Append(KKeystoreDataPath);
			
			TInt err = iFs.Delete(keystoreDataPath);
			if (err == KErrNotFound)
				err = KErrNone;
			iState = EFinished;
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, err);
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
			iActionState = EPostrequisite;
			}
			break;
		}
	}

void CDeleteKeyStoreData::PerformCancel()
	{
    // nothing to cancel
	}

void CDeleteKeyStoreData::Reset()
	{
	iState = EDelete;
	}

void CDeleteKeyStoreData::DoReportAction()
	{
	_LIT(KDeleting, "Deleting keystore data...");
	iOut.writeString(KDeleting);
	iOut.writeNewLine();
	}

void CDeleteKeyStoreData::DoCheckResult(TInt aError)
	{
	if (iFinished)
		{
		if (aError == KErrNone)
			{
			_LIT(KSuccess, "\tKeystore data deleted successfully\n");
			iConsole.Write(KSuccess);
			iOut.writeString(KSuccess);
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		else
			{
			_LIT(KFailed, "\tStore delete failed\n");
			iConsole.Write(KFailed);
			iOut.writeString(KFailed);
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		}
	}
