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
*
*/


#include "tactiondecodepkcs5.h"
#include "t_input.h"
#include <asnpkcs.h>
#include <pbedata.h>
#include <pbe.h>
#include <asn1enc.h>

_LIT8(KInputStart, "<input>");


CTestAction* CActionDecodePKCS5::NewL(RFs& aFs,
									   CConsoleBase& aConsole,
									   Output& aOut, 
									   const TTestActionSpec& aTestActionSpec)
	{
	CActionDecodePKCS5* self = new(ELeave) CActionDecodePKCS5(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CActionDecodePKCS5::~CActionDecodePKCS5()
	{
	delete iInput;
	delete iOutput;
	}

CActionDecodePKCS5::CActionDecodePKCS5(RFs& aFs, 
								 CConsoleBase& aConsole,
								 Output& aOut)
								 
: CTestAction(aConsole, aOut), iFs(aFs)
	{
	}

void CActionDecodePKCS5::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);

	iInput = Input::ParseElementHexL(aTestActionSpec.iActionBody, KInputStart);
	iOutput = HBufC8::NewL(iInput->Length()+100); //so we detect errors in
	//encoding not panics in descriptors
	}

void CActionDecodePKCS5::DoReportAction(void)
	{
	}

void CActionDecodePKCS5::DoCheckResult(TInt)
	{
	}

void CActionDecodePKCS5::PerformAction(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	iResult = EFalse;
	
	CPBEncryptParms* parms = TASN1DecPKCS5::DecodeDERL(*iInput);
	CleanupStack::PushL(parms);
	CASN1EncSequence* seq = TASN1EncPKCS5::EncodeDERL(*parms);
	CleanupStack::PushL(seq);

	iOutput->Des().SetLength(seq->LengthDER());
	TUint length=0;
	TPtr8 iOutputTemp = iOutput->Des();
	seq->WriteDERL(iOutputTemp, length);

	if(*iOutput == *iInput)
		{
		iResult = ETrue;
		}

	CleanupStack::PopAndDestroy(2); //seq, parms
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EPostrequisite;
	}
