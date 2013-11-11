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

#include "t_unifiedcertstorewritablecertstorecount.h"
#include "t_input.h"
#include "t_certstoredefs.h"
#include "t_certstoreout.h"

CTestAction* CUnifiedCertStoreWritableCertStoreCount::NewL(RFs& aFs,
														   CConsoleBase& aConsole,	
														   Output& aOut, 
														   const TTestActionSpec& aTestActionSpec)
	{
	CUnifiedCertStoreWritableCertStoreCount* self =
		new(ELeave) CUnifiedCertStoreWritableCertStoreCount(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CUnifiedCertStoreWritableCertStoreCount::~CUnifiedCertStoreWritableCertStoreCount()
	{
	}

CUnifiedCertStoreWritableCertStoreCount::CUnifiedCertStoreWritableCertStoreCount(RFs& aFs, CConsoleBase& aConsole,	
																				 Output& aOut)
: CUnifiedCertStoreCount(aFs, aConsole, aOut)
	{
	}

void CUnifiedCertStoreWritableCertStoreCount::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CUnifiedCertStoreCount::ConstructL(aTestActionSpec);

	TInt err = KErrNone;
	TInt pos = 0;
	iStoreIndex = Input::ParseElement(aTestActionSpec.iActionBody, KStoreIndexStart, KStoreIndexEnd, pos, err);
	iExpectCount = Input::ParseElement(aTestActionSpec.iActionResult, KNumberOfStoresStart, KNumberOfStoresEnd, pos, err);
	}

void CUnifiedCertStoreWritableCertStoreCount::DoPerformAction()
	{
	TLex8 lexi(iStoreIndex);
	TUid iIndex;
	TInt index = lexi.Val(iIndex.iUid);
	if (index != KErrNone)
		{
		User::Panic(_L("CUnifiedCertStoreCertStoreCount"), 1);
		}
	iCount = UnifiedCertStore(iIndex.iUid).WritableCertStoreCount();

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

void CUnifiedCertStoreWritableCertStoreCount::DoWriteResult()
	{
	iConsole.Printf(_L("\tNumber of writable stores:"));
	iOut.writeString(_L("\tNumber of writable stores: "));
	}
