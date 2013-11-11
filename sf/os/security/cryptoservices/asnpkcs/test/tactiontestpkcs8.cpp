/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* tactiondecodepkcs5.cpp
*
*/


#include "tactiontestpkcs8.h"
#include "t_input.h"
#include "t_output.h"
#include <asnpkcs.h>
#include <pbedata.h>
#include <pbe.h>
#include <asn1enc.h>

_LIT8(KFilenameStart, "<filename>");
_LIT8(KMatchesStart, "<matches>");

_LIT8(KOutcomeNoMatch, "noMatch");
_LIT8(KOutcomeMatchesPKCS8, "pkcs8");
_LIT8(KOutcomeMatchesEncryptedPKCS8, "encryptedPkcs8");

_LIT(KFilenameBase, "\\tkeystore\\data\\");

CTestAction* CActionTestPKCS8::NewL(RFs& aFs,
									CConsoleBase& aConsole,
									Output& aOut, 
									const TTestActionSpec& aTestActionSpec)
	{
	CActionTestPKCS8* self = new(ELeave) CActionTestPKCS8(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CActionTestPKCS8::~CActionTestPKCS8()
	{
	delete iInput;
	}

CActionTestPKCS8::CActionTestPKCS8(RFs& aFs, 
								   CConsoleBase& aConsole,
								   Output& aOut)
								 
: CTestAction(aConsole, aOut), iFs(aFs)
	{
	}

void CActionTestPKCS8::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	ReadInputFileL(Input::ParseElement(aTestActionSpec.iActionBody, KFilenameStart));
	SetExpectedOutcomeL(Input::ParseElement(aTestActionSpec.iActionBody, KMatchesStart));	
	}

void CActionTestPKCS8::ReadInputFileL(const TDesC8& aFilename)
	{
	if (aFilename == KNullDesC8)
		{
		User::Leave(KErrArgument);		
		}

	TFileName tempFilename;
	tempFilename.Copy(aFilename); // convert from 8 -> 16 bit descriptor
	
	TDriveUnit sysDrive = RFs::GetSystemDrive();
	TDriveName sysDriveName (sysDrive.Name());
	tempFilename.Insert(0,sysDriveName);
	tempFilename.Insert(2,KFilenameBase);
		
	RFile file;
	User::LeaveIfError(file.Open(iFs, tempFilename, EFileRead));
	CleanupClosePushL(file);
 	TInt size = 0;
	User::LeaveIfError(file.Size(size));

	iInput = HBufC8::NewMaxL(size);
	TPtr8 ptr = iInput->Des();
	User::LeaveIfError(file.Read(ptr));

	CleanupStack::PopAndDestroy(&file);
	}

void CActionTestPKCS8::SetExpectedOutcomeL(const TDesC8& aOutcome)
	{
	if (aOutcome == KOutcomeNoMatch)
		{
		iExpectedOutcome = ENoMatch;
		}
	else if (aOutcome == KOutcomeMatchesPKCS8)
		{
		iExpectedOutcome = EMatchesPKCS8;
		}
	else if (aOutcome == KOutcomeMatchesEncryptedPKCS8)
		{
		iExpectedOutcome = EMatchesEncryptedPKCS8;
		}
	else
		{
		User::Leave(KErrArgument);
		}
	}

void CActionTestPKCS8::DoReportAction(void)
	{
	}

void CActionTestPKCS8::DoCheckResult(TInt)
	{
	}

void CActionTestPKCS8::PerformAction(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	iResult = EFalse;

	TBool matchesPKCS8 = TASN1DecPKCS8::IsPKCS8Data(*iInput);
	TBool matchesEncryptedPKCS8 = TASN1DecPKCS8::IsEncryptedPKCS8Data(*iInput);

	if (matchesPKCS8 && matchesEncryptedPKCS8)
		{
		iOut.writeString(_L("!! Data matches both cleartext and encrypted pkcs8\n"));
		User::Leave(KErrGeneral);
		}

	TOutcome outcome = ENoMatch;
	
	if (matchesPKCS8)
		{
		iOut.writeString(_L("Data matches cleartext pkcs8\n"));
		outcome = EMatchesPKCS8;
		}
	else if (matchesEncryptedPKCS8)
		{
		iOut.writeString(_L("Data matches encrypted pkcs8\n"));
		outcome = EMatchesEncryptedPKCS8;
		}
	else
		{
		iOut.writeString(_L("Data doesn't match anything\n"));
		}

	if(outcome == iExpectedOutcome)
		{
		iResult = ETrue;
		}

	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EPostrequisite;
	}
