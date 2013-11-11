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

#include "t_message.h"
#include "t_input.h"
#include "t_testactionspec.h"
#include "t_output.h"

_LIT8(KMessageStart, "<message>");
_LIT8(KMessageEnd, "</message>");

CTestActionMessage* CTestActionMessage::NewL(CConsoleBase& aConsole, 
											 Output& aOut,
											 const TTestActionSpec& aTestActionSpec)
	{
	CTestActionMessage* self = CTestActionMessage::NewLC(aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CTestActionMessage* CTestActionMessage::NewLC(CConsoleBase& aConsole, 
											  Output& aOut,
											  const TTestActionSpec& aTestActionSpec)
	{
	CTestActionMessage* self = new(ELeave) CTestActionMessage(aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CTestActionMessage::~CTestActionMessage()
	{
	delete iMessage;
	}

void CTestActionMessage::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	iActionState = EAction;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	};

void CTestActionMessage::PerformAction(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iActionState = EPostrequisite;
	}

void CTestActionMessage::DoPerformPostrequisite(TRequestStatus& aStatus) 
	{	
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iFinished = ETrue;
	iResult = ETrue;
	};

void CTestActionMessage::PerformMessage()
	{
	__ASSERT_DEBUG(EFalse, User::Panic(_L("CTestActionMessage::PerformMessage()"), 1));
	}

void CTestActionMessage::Reset()
	{
	__ASSERT_DEBUG(EFalse, User::Panic(_L("CTestActionMessage::Reset()"), 1));
	}

CTestActionMessage::CTestActionMessage(CConsoleBase& aConsole, 
									   Output& aOut)
: CTestAction(aConsole, aOut) 
	{
	}

void CTestActionMessage::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	TInt pos = 0;
	TInt err = KErrNone;
	HBufC8* aBody = HBufC8::NewLC(aTestActionSpec.iActionBody.Length());
	aBody->Des().Copy(aTestActionSpec.iActionBody);
	TPtrC8 pMessage = Input::ParseElement(*aBody, KMessageStart, KMessageEnd, pos, err);
	User::LeaveIfError(err);
	iMessage = pMessage.AllocL();
	CleanupStack::PopAndDestroy();
	}

void CTestActionMessage::DoReportAction()
	{
	iOut.writeString(*iMessage);
	iOut.writeNewLine();
	iOut.writeNewLine();
	}

void CTestActionMessage::DoCheckResult(TInt /*aError*/)
	{
	}
