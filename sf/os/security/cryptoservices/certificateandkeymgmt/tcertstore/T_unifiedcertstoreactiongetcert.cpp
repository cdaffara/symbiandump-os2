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

#include "t_unifiedcertstoreactiongetcert.h"
#include "t_input.h"
#include "t_certstoredefs.h"
#include "t_certstoreout.h"

CTestAction* CUnifiedCertStoreGetCert::NewL(RFs& aFs, CConsoleBase& aConsole,
											Output& aOut, 
											const TTestActionSpec& aTestActionSpec)

	{
	CUnifiedCertStoreGetCert* self = new(ELeave) CUnifiedCertStoreGetCert(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}


CUnifiedCertStoreGetCert::~CUnifiedCertStoreGetCert()
	{
	if (iCertInfo)
		iCertInfo->Release();
	}

CUnifiedCertStoreGetCert::CUnifiedCertStoreGetCert(RFs& aFs, CConsoleBase& aConsole,	
												   Output& aOut)
: CCertStoreTestAction(aFs, aConsole, aOut),
iState(EGetCert)
	{
	}

void CUnifiedCertStoreGetCert::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CCertStoreTestAction::ConstructL(aTestActionSpec);
	TInt pos = 0;
	TInt err = KErrNone;
	TLex8 lex(Input::ParseElement(aTestActionSpec.iActionBody, KTokenTypeStart, KTokenTypeEnd, pos, err));
	if (err != KErrNone)
		{
		User::Leave(err);
		}
	TUid uid;
	lex.Val(uid.iUid); // check for error

	TInt token;
	lex = TLex8(Input::ParseElement(aTestActionSpec.iActionBody, KTokenStart, KTokenEnd, pos, err));
	lex.Val(token);

	TInt object;
	lex = TLex8(Input::ParseElement(aTestActionSpec.iActionBody, KObjectStart, KObjectEnd, pos, err));
	lex.Val(object);

	iHandle = TCTTokenObjectHandle(TCTTokenHandle(uid, token), object);

	pos = 0;

	HBufC* result = HBufC::NewLC(aTestActionSpec.iActionResult.Length());
	TPtr(result->Des()).Copy(aTestActionSpec.iActionResult);
	Input::GetExpectedResultL(Input::ParseElement(*result, KReturnStart, KReturnEnd), iExpectedResult);
	CleanupStack::PopAndDestroy(result);
	}

void CUnifiedCertStoreGetCert::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{
	case EGetCert:
		CertStore().GetCert(iCertInfo, iHandle, aStatus);
		iState = EFinished;
		break;

	case EFinished:
		{
		if (aStatus == iExpectedResult)
			{
			iResult = ETrue;
			}
		else
			{
			iResult = EFalse;
			}
		iFinished = ETrue;
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrNone);
		break;
		}
	
	// The initial state is set to EGetCert and the default case would
	// never be reached.
	default:
		iResult = EFalse;
		iFinished = ETrue;
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrNotSupported);
		break;
		}
	}

void CUnifiedCertStoreGetCert::PerformCancel()
	{
	}

void CUnifiedCertStoreGetCert::Reset()
	{
	__ASSERT_DEBUG(EFalse, User::Panic(_L("CUnifiedCertStoreGetCert::Reset()"), 1));
	}

void CUnifiedCertStoreGetCert::DoReportAction()
	{
	//iOut.writeString(_L("Setting trust settings..."));
	iOut.writeHex(iHandle.iTokenHandle.iTokenTypeUid.iUid);
	//iOut.writeNewLine();
	//iOut.writeString(_L("\tLabel = "));
//	iOut.writeString(iLabel);
//	iOut.writeNewLine();
	//iOut.writeString(_L("\tTrusters = "));
	//TInt count = iTrusters->Count();
	//for (TInt i = 0; i < count; i++)
	//	{
	//	iOut.writeNum((*iTrusters)[i].iUid);
	//	iOut.writeString(_L(" "));
	//	}
	//iOut.writeNewLine();
	//iOut.writeNewLine();
	}	

void CUnifiedCertStoreGetCert::DoCheckResult(TInt /*aError*/)
	{
	}
