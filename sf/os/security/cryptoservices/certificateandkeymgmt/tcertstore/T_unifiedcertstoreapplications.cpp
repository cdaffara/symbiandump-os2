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

#include "T_unifiedcertstoreapplications.h"
#include "t_input.h"
#include "t_certstoredefs.h"
#include "t_certstoreout.h"
#include <ccertattributefilter.h>

CTestAction* CUnifiedCertStoreApplications::NewL(RFs& aFs,
												 CConsoleBase& aConsole,	
												 Output& aOut, 
												 const TTestActionSpec& aTestActionSpec)
	{
	CUnifiedCertStoreApplications* self = 
		new(ELeave) CUnifiedCertStoreApplications(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CUnifiedCertStoreApplications::~CUnifiedCertStoreApplications()
	{
	iCertInfos.Close();
	iExpectedApplications.Reset();
	delete iFilter;
	iApplications.Close();
	}

CUnifiedCertStoreApplications::CUnifiedCertStoreApplications(RFs& aFs, CConsoleBase& aConsole,	
															 Output& aOut)
: CCertStoreTestAction(aFs, aConsole, aOut), iState(EGetCert)
	{
	}

void CUnifiedCertStoreApplications::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CCertStoreTestAction::ConstructL(aTestActionSpec);
	iFilter = CCertAttributeFilter::NewL();
	TInt err = KErrNone;
	TInt pos = 0;
	
	TPtrC8 pLabel = Input::ParseElement(aTestActionSpec.iActionBody, KCertLabelStart, KCertLabelEnd, pos, err);
	if (err != KErrNone)
		{
		User::Leave(err);
		}
	iCertificateLabel.Copy(pLabel);

	// Set expected result

	pos = 0;
	
	HBufC* result = HBufC::NewLC(aTestActionSpec.iActionResult.Length());
	TPtr(result->Des()).Copy(aTestActionSpec.iActionResult);
	Input::GetExpectedResultL(Input::ParseElement(*result, KReturnStart, KReturnEnd), iExpectedResult);
	CleanupStack::PopAndDestroy(result);
	
	const TDesC8& uidsString = Input::ParseElement(aTestActionSpec.iActionResult, KUIDStart, KUIDEnd,
		pos, err);
	TLex8 lex(uidsString);

	err = KErrNone;
	while (err == KErrNone)
		{
		TUid uid;
		err = lex.Val(uid.iUid);
		if (err == KErrNone)
			{	
			lex.SkipSpace();
			User::LeaveIfError(iExpectedApplications.Append(uid));
			}
		}
	}

void CUnifiedCertStoreApplications::GetApplications(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	TInt ix = KErrNotFound;
	TInt count = iCertInfos.Count();
	for (TInt i = 0; i < count; i++)
		{
		if (iCertInfos[i]->Label() == iCertificateLabel)
			{
			ix = i;
			break;
			}
		}
	if (ix == KErrNotFound)
		{
		User::RequestComplete(status, ix);
		}	
	else
		{
		iCertInfoForApplications = iCertInfos[ix];
		CertStore().Applications(*iCertInfoForApplications, iApplications, aStatus);
		}
	}

void CUnifiedCertStoreApplications::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{
		case EGetCert:
			iState = EGettingApplications;
			//get the certs
			CertStore().List(iCertInfos, *iFilter, aStatus);
			break;
			
		case EGettingApplications:
			iState = EFinished;
			//get the certificates associated applications
			GetApplications(aStatus);
			break;
			
		case EFinished:
			{
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, aStatus.Int());
			if (aStatus == iExpectedResult)
				{
				// Check that we have the expected uids
				TInt iEnd = iApplications.Count();
				if (iEnd == iExpectedApplications.Count())
					{
					TInt i = 0;
					for (i = 0; i < iEnd; i++)
						{
						if ((iApplications)[i] != iExpectedApplications[i])
							{
							break;
							}
						}
					if (i == iEnd)
						{
						iResult = ETrue;
						}
					else
						{
						iResult = EFalse;
						}
					}
				else
					{
					iResult = EFalse;
					}
				}
			else
				{
				iResult = EFalse;
				}
            
            iFinished = ETrue;
			}
			break;
		}
	}

void CUnifiedCertStoreApplications::PerformCancel()
	{
	switch (iState)
		{
		case EGettingApplications:
			CertStore().CancelList();
			break;

		case EFinished:
			CertStore().CancelApplications();
			break;

		default:
			break;
		}
	}

void CUnifiedCertStoreApplications::Reset()
	{
    iCertInfos.Close();
    iState = EGetCert;
	}

void CUnifiedCertStoreApplications::DoReportAction()
	{
	iOut.writeString(_L("Getting applications..."));
	iOut.writeNewLine();
	iOut.writeString(_L("\tExpected applications :"));
	TInt iEnd = iExpectedApplications.Count();
	for (TInt i = 0; i < iEnd; i++)
		{
		iOut.writeString(_L(" "));
		iOut.writeNum(iExpectedApplications[i].iUid);
		}
	iOut.writeNewLine();
	iOut.writeNewLine();
	}	

void CUnifiedCertStoreApplications::DoCheckResult(TInt /*aError*/)
	{
	if (iFinished)
		{
		iConsole.Printf(_L("\tApplications : "));
		iOut.writeString(_L("\tApplications : "));
		TInt count = iApplications.Count();
		for (TInt i = 0; i < count; i++)
			{
			iConsole.Printf(_L("%D "), (iApplications)[i]);
			iOut.writeNum((iApplications)[i].iUid);
			iOut.writeString(_L(" "));
			}
		iOut.writeNewLine();
		if (iResult)
			{
			iConsole.Printf(_L("\n\tApplications retrieved successfully\n"));
			iOut.writeString(_L("\tApplications retrieved successfully"));
			iOut.writeNewLine();
			}
		else
			{
			iConsole.Printf(_L("\n\tApplications retrieved failed\n"));
			iOut.writeString(_L("\tApplications retrieved failed"));
			iOut.writeNewLine();
			}
		iOut.writeNewLine();
		}
	}
