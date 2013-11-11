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

#include "t_certstoreactionshandle.h"
#include "t_input.h"
#include "t_certstoredefs.h"
#include "t_certstoreout.h"
#include <ccertattributefilter.h>

CTestAction* CCertStoreHandle::NewL(RFs& aFs, CConsoleBase& aConsole,
									Output& aOut, 
									const TTestActionSpec& aTestActionSpec)
	{
	CCertStoreHandle* self = new(ELeave) CCertStoreHandle(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}


CCertStoreHandle::~CCertStoreHandle()
	{
	delete iFilter;
	iCertInfos.Close();
	}

CCertStoreHandle::CCertStoreHandle(RFs& aFs, CConsoleBase& aConsole,	
								   Output& aOut)
: CCertStoreTestAction(aFs, aConsole, aOut), iState(EList)
	{
	}

void CCertStoreHandle::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CCertStoreTestAction::ConstructL(aTestActionSpec);
	// Creates a default filter
	iFilter = CCertAttributeFilter::NewL();

	TInt pos = 0;
	TInt err = KErrNone;
	iLabel.Copy(Input::ParseElement(aTestActionSpec.iActionBody, KCertLabelStart, KCertLabelEnd, pos, err));
	
	// set the expected result
	HBufC* result = HBufC::NewLC(aTestActionSpec.iActionResult.Length());
	TPtr(result->Des()).Copy(aTestActionSpec.iActionResult);
	Input::GetExpectedResultL(Input::ParseElement(*result, KReturnStart, KReturnEnd), iExpectedResult);
	CleanupStack::PopAndDestroy(result);
	
	}

void CCertStoreHandle::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{
	case EList:
		// Get the list of all certificates
		CertStore().List(iCertInfos, *iFilter, aStatus);
		iState = EHandle;
		break;

	case EHandle:
		{
		// Find the certificate we want
		TInt end =  iCertInfos.Count();
		for (TInt i = 0; i < end; i++)
			{
			if (iCertInfos[i]->Label() == iLabel)
				{
				iHandle = iCertInfos[i]->Handle();
				break;
				}
			}
		if (aStatus == iExpectedResult)
			{
			iResult = ETrue;
			}
		else
			{
			iResult = EFalse;
			}
		if (aStatus == KErrNoMemory)
			{	
			}
		else
			{
			iFinished = ETrue;
			}
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrNone);
		break;
		}
	
	// The initial state is set to EList and the default case would
	// never be reached.
	default:
		iResult = EFalse;
		iFinished = ETrue;
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrNotSupported);
		break;
		}
	}

void CCertStoreHandle::PerformCancel()
	{
	}

void CCertStoreHandle::Reset()
	{
	__ASSERT_DEBUG(EFalse, User::Panic(_L("CCertStoreHandle::Reset()"), 1));
	}

void CCertStoreHandle::DoReportAction()
	{
	iOut.writeString(_L("\tLabel = "));
	iOut.writeString(iLabel);
	iOut.writeNewLine();
	}	

void CCertStoreHandle::DoCheckResult(TInt aError)
	{
	if (iFinished)
		{
		if (aError == KErrNone)
			{
			iOut.writeHex(iHandle.iTokenHandle.iTokenTypeUid.iUid);
			iOut.writeHex(iHandle.iTokenHandle.iTokenId);
			iOut.writeHex(iHandle.iObjectId);
			}
		}
	}
