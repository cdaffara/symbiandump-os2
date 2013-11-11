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


/**
 @file
*/

#include <e32base.h>
#include "t_keystore_actions.h"
#include "t_keystore_defs.h"
#include "t_input.h"

/////////////////////////////////////////////////////////////////////////////////
// CCorrupt
//
// Corrupts a file by writing random stuff into it
/////////////////////////////////////////////////////////////////////////////////

CTestAction* CCorrupt::NewL(RFs& aFs, 
							CConsoleBase& aConsole, 
							Output& aOut,
							const TTestActionSpec& aTestActionSpec)
{
	CTestAction* self = CCorrupt::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
}

CTestAction* CCorrupt::NewLC(RFs& aFs,
							CConsoleBase& aConsole, 
							Output& aOut,
							const TTestActionSpec& aTestActionSpec)
{
	CCorrupt* self = new (ELeave) CCorrupt(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
}

CCorrupt::~CCorrupt()
	{
	delete iCorruptionData;
	}
	
CCorrupt::CCorrupt(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
:	CKeyStoreTestAction(aFs, aConsole, aOut)
{
 	iState = ECorrupting;
}


void CCorrupt::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CKeyStoreTestAction::ConstructL(aTestActionSpec);

	TInt err = KErrNone;
	TInt pos = 0;
	
	iCorruptionData= HBufC8::NewL(256); 
	TPtr8 temp(iCorruptionData->Des());
	temp.Copy(Input::ParseElement(aTestActionSpec.iActionBody, KCorruptData, KCorruptDataEnd, pos, err));
	}
	
void CCorrupt::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{
		case ECorrupting:
			{
			TRAPD(err, DoCorruptL());
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
			iFinished = ETrue;			
			}
		}

	}

void CCorrupt::DoCorruptL()
	{
	RFs theFs;
	User::LeaveIfError(theFs.Connect());
	CleanupClosePushL(theFs);

	RFile file;
	TDriveUnit sysDrive (theFs.GetSystemDrive());
	TBuf<128> keystoreDataPath (sysDrive.Name());
	keystoreDataPath.Append(KKeystoreDataPath);
	
	User::LeaveIfError(file.Open(theFs, keystoreDataPath, EFileWrite|EFileShareAny));
	file.Write(*iCorruptionData);
	file.Close();
	CleanupStack::PopAndDestroy(&theFs);
	}

void CCorrupt::PerformCancel()
	{
    // nothing to cancel
	}


void CCorrupt::DoCheckResult(TInt aError)
	{
	if (iFinished)
		{
		if (aError == KErrNone)
			{
			_LIT(KSuccessful, "Corrupted! \n");
			iConsole.Write(KSuccessful);
			iOut.writeString(KSuccessful);
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		else
			{
			TBuf<100> msg;
			_LIT(KUnSuccessful, "Corrupt didn't do its job... : %d\n");
			msg.Format(KUnSuccessful, aError);
			iConsole.Write(msg);
			iOut.writeString(msg);
			iOut.writeNewLine();
			iOut.writeNewLine();		
			}			
		}
	}


void CCorrupt::DoReportAction()
	{
	_LIT(KCorrupting, "Corrupting...");
	iOut.writeString(KCorrupting);
	TPtr theLabel(iLabel->Des());
	iOut.writeString(theLabel);
	iOut.writeNewLine();
	}
