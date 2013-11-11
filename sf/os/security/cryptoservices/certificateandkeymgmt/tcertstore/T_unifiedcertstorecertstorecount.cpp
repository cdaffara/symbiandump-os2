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

#include "t_unifiedcertstorecertstorecount.h"
#include "t_input.h"
#include "t_certstoredefs.h"
#include "t_certstoreout.h"

CTestAction* CUnifiedCertStoreCertStoreCount::NewL(RFs& aFs,
												   CConsoleBase& aConsole,
												   Output& aOut, 
												   const TTestActionSpec& aTestActionSpec)
	{
	CUnifiedCertStoreCertStoreCount* self =
		new(ELeave) CUnifiedCertStoreCertStoreCount(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CUnifiedCertStoreCertStoreCount::~CUnifiedCertStoreCertStoreCount()
	{
	}

CUnifiedCertStoreCertStoreCount::CUnifiedCertStoreCertStoreCount(RFs& aFs, CConsoleBase& aConsole,	
																 Output& aOut)
: CUnifiedCertStoreCount(aFs, aConsole, aOut)
	{
	}

void CUnifiedCertStoreCertStoreCount::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	TInt err = KErrNone;
	TInt pos = 0;
	CUnifiedCertStoreCount::ConstructL(aTestActionSpec);
	iStoreIndex = Input::ParseElement(aTestActionSpec.iActionBody, KStoreIndexStart, KStoreIndexEnd, pos, err);
	iExpectCount = Input::ParseElement(aTestActionSpec.iActionResult, KNumberOfStoresStart, KNumberOfStoresEnd, pos, err);
	}

void CUnifiedCertStoreCertStoreCount::DoPerformAction()
	{
	TLex8 lexi(iStoreIndex);
	TUid iIndex;
	TInt index = lexi.Val(iIndex.iUid);
	if (index != KErrNone)
		{
		User::Panic(_L("CUnifiedCertStoreCertStoreCount"), 1);
		}
	iCount = UnifiedCertStore(iIndex.iUid).CertStoreCount();

	TLex8 lex(iExpectCount);
	TUid uid;
	TInt iu = lex.Val(uid.iUid);
	if (iu != KErrNone)
		{
		User::Panic(_L("CUnifiedCertStoreCertStoreCount"), 1);
		}
	if (iCount == uid.iUid)
		{
		iResult = ETrue;
		}
	else
		{
		iResult = EFalse;
		}
	}

void CUnifiedCertStoreCertStoreCount::DoWriteResult()
	{
	iConsole.Printf(_L("\tNumber of stores (read-only + writable) :"));
	iOut.writeString(_L("\tNumber of stores (read-only + writable) : "));
	}
