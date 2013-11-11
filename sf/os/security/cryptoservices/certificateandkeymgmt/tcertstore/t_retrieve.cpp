/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "t_retrieve.h"
#include "t_certstoreout.h"
#include "t_input.h"
#include "t_certstoredefs.h"
#include "t_concurrentcertstore.h"
#include <ccertattributefilter.h>

///////////////////////////////////////////////////////////////////////////////
//CRetrieveCertificate
///////////////////////////////////////////////////////////////////////////////

CTestAction* CRetrieveCertificate::NewL(RFs& aFs,
										CConsoleBase& aConsole,
										Output& aOut,
										const TTestActionSpec& aTestActionSpec)
	{
	CRetrieveCertificate* self = new (ELeave) CRetrieveCertificate(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CRetrieveCertificate::CRetrieveCertificate(RFs& aFs,
										   CConsoleBase& aConsole, 
										   Output& aOut) :
	CCertStoreTestAction(aFs, aConsole, aOut), iDataPtr(NULL, 0),
	iFormat(EUnknownCertificate), iDeletable(EFalse), iTestForDeletable(EFalse)
	{
	}

void CRetrieveCertificate::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CCertStoreTestAction::ConstructL(aTestActionSpec);
	
	iFilter = CCertAttributeFilter::NewL();

	HBufC* result = HBufC::NewLC(aTestActionSpec.iActionResult.Length());
	TPtr(result->Des()).Copy(aTestActionSpec.iActionResult);
	Input::GetExpectedResultL(Input::ParseElement(*result, KReturnStart, KReturnEnd), iExpectedResult);
	CleanupStack::PopAndDestroy(result);

	iLabel.Copy(Input::ParseElement(aTestActionSpec.iActionBody, KCertLabelStart));
	if (iLabel.Length() == 0)
		{
		User::Leave(KErrArgument);
		}

	// check for a possible deletable flag value for the certificate
	TInt err = KErrNone;
	TInt pos = 0;
	const TDesC8& deletableStr = Input::ParseElement(aTestActionSpec.iActionResult, 
														KDeletableStart,
														KDeletableEnd,
														pos,
														err);

	// set the deletable attribute if a value was found for the certificate
	if (err == KErrNone)
		{
		SetDeletable(deletableStr);
		
		// if deletable is found then format value must also be present
		SetFormatL(Input::ParseElement(aTestActionSpec.iActionResult, KCertFormatStart));
		}
	}

CRetrieveCertificate::~CRetrieveCertificate()
	{
	Reset();
	delete iFilter;
	}

void CRetrieveCertificate::SetDeletable(const TDesC8& aDeletableString)
	{
	iTestForDeletable = ETrue;
	if (aDeletableString.Compare(KTrue)==0)
		{
		iDeletable = ETrue;
		}
	else
		{
		iDeletable = EFalse;
		}
	}

void CRetrieveCertificate::SetFormatL(const TDesC8& aFormatString)
	{
	if (aFormatString == KNullDesC8)
		{
		User::Leave(KErrArgument);
		}
	if (aFormatString == KWTLS)
		{
		iFormat = EWTLSCertificate;
		}
	else if (aFormatString == KX509)
		{
		iFormat = EX509Certificate;
		}
	else if (aFormatString == KWTLSURL)
		{
		iFormat = EWTLSCertificateUrl;
		}
	else if (aFormatString == KX509URL)
		{
		iFormat = EX509CertificateUrl;
		}
	else
		{
		iOut.write(_L("Unknown cert format: "));
		iOut.writeString(aFormatString);
		iOut.writeNewLine();		   
		User::Leave(KErrNotSupported);
		}
	}

void CRetrieveCertificate::PerformAction(TRequestStatus& aStatus)
	{
	if (aStatus != KErrNone && iState != ECheckRetrieve)
		{
		iState = EFinished;
		}
	
	switch (iState)
		{
		case EList:
			CertStore().List(iCertInfos, *iFilter, aStatus);
			iState = ERetrieveData;
			break;

		case ERetrieveData:
			{
			for (TInt i = 0 ; i < iCertInfos.Count() ; ++i)
				{
				CCTCertInfo* certInfo = iCertInfos[i];
				if (certInfo->Label() == iLabel)
					{
					iCertInfo = certInfo;
					break;
					}
				}
			
			if (!iCertInfo)
				{
				iState = EFinished;			
				TRequestStatus* status = &aStatus;
				User::RequestComplete(status, KErrNotFound);
				}
			else
				{
				TRAPD(err, iData = HBufC8::NewL(iCertInfo->Size()));
				if (err != KErrNone)
					{
					iState = EFinished;
					TRequestStatus* status = &aStatus;
					User::RequestComplete(status, err);
					}
				else
					{
					iState = EParseData;
					// iDataPtr has to be be a member as it's passed to asyc retrieve operation
					iDataPtr.Set(iData->Des());  
					CertStore().Retrieve(*iCertInfo, iDataPtr, aStatus);
					}
				}
			}
			break;

		case EParseData:
			{
			switch (iCertInfo->CertificateFormat())
				{
				case EX509Certificate:
					{
					CX509Certificate* cert = CX509Certificate::NewLC(*iData);
					X509CertWriter writer(iOut);
					writer.WriteCert(*cert);
					CleanupStack::PopAndDestroy();
					iOut.writeNewLine();
					}
					break;

				case EWTLSCertificate:
					{
					CWTLSCertificate* cert = CWTLSCertificate::NewLC(*iData);
					WTLSCertWriter writer(iOut);
					writer.WriteCert(*cert);
					CleanupStack::PopAndDestroy(cert);
					iOut.writeNewLine();
					}
					break;
				
				default:
					break;
				}
			
			iState = ERetrieveCert;
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, KErrNone);
			}
			break;

		case ERetrieveCert:
			if (CertStoreType() != EUnifiedCertStore)
				{
				iState = EFinished;
				TRequestStatus* status = &aStatus;
				User::RequestComplete(status, KErrNone);
				}
			else
				{
				iState = ECheckRetrieve;
				UnifiedCertStore().Retrieve(*iCertInfo, iCert, aStatus);
				}
			break;

		case ECheckRetrieve:
			{
			TCertificateFormat format = iCertInfo->CertificateFormat();
			TInt expectedErr = KErrNotSupported;
			if (format == EX509Certificate || format == EWTLSCertificate)
				{
				expectedErr = KErrNone;
				}
			TInt err = (aStatus.Int() == expectedErr) ? KErrNone : KErrGeneral;
			iState = EFinished;
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, err);
			}
			break;

		case EFinished:
			{
			iActionState = EPostrequisite;
			iFinished = ETrue;
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, aStatus.Int());
			}
			break;
			
		default:
			User::Invariant();
		}
	}

void CRetrieveCertificate::PerformCancel()
	{
	switch (iState)
		{
		case ERetrieveData:
			CertStore().CancelList();
			break;

		case EParseData:
		case ECheckRetrieve:
			CertStore().CancelRetrieve();
			break;

		default:
			break;
		}
	}

void CRetrieveCertificate::Reset()
	{
	iState = EList;
	iCertInfos.Close();
	delete iCert;
	iCert = NULL;
	delete iData;
	iData = NULL;
	}

void CRetrieveCertificate::DoReportAction()
	{
	iOut.write(_L("Retrieving certificate...\n"));
	iOut.write(_L("\tLabel: %S\n"), &iLabel);
	if (iTestForDeletable)
		{
		iOut.writeString(_L("\tDeletable = "));
		iDeletable ? iOut.writeString(KTrue) : iOut.writeString(KFalse);
		iOut.writeNewLine();
		}
	}

void CRetrieveCertificate::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	iFinished = ETrue;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, aStatus.Int());
	}

TBool CRetrieveCertificate::ValidCertInfo()
	{
	// check iFormat and deletable flag of the cert info object
	return ((iCertInfo->IsDeletable() == iDeletable) &&
		    (iFormat == iCertInfo->CertificateFormat()));
	}
	
void CRetrieveCertificate::DoCheckResult(TInt aError)
	{
	if (iFinished)
		{
		iResult = (aError == iExpectedResult); 
		
		if (iResult)
			{
			if (iTestForDeletable)
				{
				iResult = iResult && ValidCertInfo();
				
				if (iResult)
					{
					iOut.writeString(_L("\tRetrieved certificate info successfully - \n\n"));
					
					}
				}
			else
				{
				iOut.writeString(_L("\tRetrieved certificate successfully\n\n"));
				}
			}
		else
			{
			iOut.writeString(_L("\tRetrieve certificate failed\n\n"));
			//	If running tests with multiple threads, failure may be expected
			// need to add this back in when we get concurrent tests working
			//CConcurrentTester::SanitizeTestResult(iOut, iResult);
			}
		}
	}
