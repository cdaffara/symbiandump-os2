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


#include "t_certstoreactionslistcertificates.h"
#include "t_certstoreout.h"
#include "t_input.h"
#include "t_certstoredefs.h"
#include "t_concurrentcertstore.h"
#include "t_filter.h"
#include <ccertattributefilter.h>

///////////////////////////////////////////////////////////////////////////////
//CListCertificates
///////////////////////////////////////////////////////////////////////////////

CTestAction* CListCertificates::NewL(RFs& aFs,
									 CConsoleBase& aConsole,
									 Output& aOut,
									 const TTestActionSpec& aTestActionSpec)
	{
	CListCertificates* self = new(ELeave) CListCertificates(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CListCertificates::CListCertificates(RFs& aFs,
									 CConsoleBase& aConsole, 
									 Output& aOut) :
	CCertStoreTestAction(aFs, aConsole, aOut), iState(EList)
	{
	}

void CListCertificates::ConstructL(	Output& aOut,
									const TTestActionSpec& aTestActionSpec)
	{
	CCertStoreTestAction::ConstructL(aTestActionSpec);
	
	iFilter = CFilter::NewL(aOut, Input::ParseElement(aTestActionSpec.iActionBody, KFilterStart, KFilterEnd));

	// Set expected result
	HBufC* result = HBufC::NewLC(aTestActionSpec.iActionResult.Length());
	TPtr(result->Des()).Copy(aTestActionSpec.iActionResult);
	Input::GetExpectedResultL(Input::ParseElement(*result, KReturnStart, KReturnEnd), iExpectedResult);
	CleanupStack::PopAndDestroy(result);

	// Set expected cert labels
	TInt pos = 0;
	TInt err = KErrNone;
	while(err == KErrNone)
		{
		TPtrC8 pExpCALabel = Input::ParseElement(aTestActionSpec.iActionResult, KCCTCertInfoStart, KCCTCertInfoEnd, pos, err);
		if (err == KErrNone)
			{
			AppendCCTCertInfoL(pExpCALabel);
			}
		}

	// Set allowed issuer DNs
	err = KErrNone;
	pos = 0;
	while (err == KErrNone)
		{
		TPtrC8 dn = Input::ParseElement(aTestActionSpec.iActionBody, KIssuerDNStart, KIssuerDNEnd, pos, err);
		if (err == KErrNone)
			AddDNL(dn);	
		}
	}

CListCertificates::~CListCertificates()
	{
	iCertInfos.Close();
	iExpectedLabels.Close();
	delete iFilter;
	iDNs.ResetAndDestroy();
	iDNs.Close();
	}


void CListCertificates::AddDNL(const TDesC8& aDNBuf)
	{
	if (aDNBuf.Length())
		{
		RFs fs;
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		
		TFileName fn;
		fn.Copy(aDNBuf);
		RFile file;
		file.Open(fs, fn, EFileRead);
		CleanupClosePushL(file);
		
		TInt size;
		file.Size(size);
		
		HBufC8* cert = HBufC8::NewMaxLC(size);
		TPtr8 ptr(cert->Des());
		file.Read(ptr);
		
		CX509Certificate* parsedCert = CX509Certificate::NewLC(*cert);
		HBufC8* issuer = parsedCert->DataElementEncoding(CX509Certificate::EIssuerName)->AllocLC();
		User::LeaveIfError(iDNs.Append(issuer));
		
		CleanupStack::Pop(issuer);
		CleanupStack::PopAndDestroy(4, &fs);
		}
	}

void CListCertificates::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{
		case EList:
			if (iDNs.Count() == 1)
				{
				UnifiedCertStore().List(iCertInfos, *iFilter, *iDNs[0], aStatus);
				}
			else if (iDNs.Count() > 1)
				{
				UnifiedCertStore().List(iCertInfos, *iFilter, iDNs, aStatus);
				}
			else
				{
				CertStore().List(iCertInfos, *iFilter, aStatus);
				}
			iState = EFinished;
			break;

		case EFinished:
			{
			// Check certificates have unique handles
			if (aStatus == KErrNone)
				{
				for (TInt i = 0 ; i < iCertInfos.Count() ; ++i)
					{
					for (TInt j = i + 1 ; j < iCertInfos.Count() ; ++j)
						{
						if (iCertInfos[i]->Handle() == iCertInfos[j]->Handle())
							{
							aStatus = KErrBadHandle;
							break;
							}
						}
					}
				}

			if (aStatus == iExpectedResult)
				{
				TestAction();
				}
			else
				{
				iResult = EFalse;
				}
			
			iActionState = EPostrequisite;
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, aStatus.Int());
			}
			break;
				
		default:
			User::Invariant();
			break;
		}
	}

void CListCertificates::TestAction()
	{
	TInt count = iCertInfos.Count();
	if (count == iExpectedLabels.Count())
		{
		TInt i;
		for (i = 0; i < count; i++)
			{
			TName expected;
			TName retrieved;
			expected.Copy(iExpectedLabels[i].iName.Ptr());
			retrieved.Copy(iCertInfos[i]->Label());
			if (expected.Compare(retrieved))
				{
				break;
				}

			// this fails the test if any of the certs are marked as not
			// deletable
			if (iCertInfos[i]->IsDeletable() != !iExpectedLabels[i].iReadOnly)
				break;
			}
		if (i == count)
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

void CListCertificates::Reset()
	{
	iState = EList;
	iCertInfos.Close();
	}

void CListCertificates::PerformCancel()
	{
	if (iState == EFinished)
		{
		CertStore().CancelList();
		}
	}

void CListCertificates::DoReportAction()
	{
	if (iFilter->iOwnerType == ECACertificate)
		{
		iOut.writeString(_L("Listing CA certificates...\n"));
		}
	else if (iFilter->iOwnerType == EUserCertificate)
		{
		iOut.writeString(_L("Listing User certificates...\n"));
		}
	else if (iFilter->iOwnerType == EPeerCertificate)
		{
		iOut.writeString(_L("Listing Peer certificates...\n"));
		}
	else
		{
		iOut.writeString(_L("Listing certificates of unknown type...\n"));
		}
	
	iOut.writeString(_L("\tFilter settings:\n")); 
	
	if (iFilter->iUidIsSet)
		{
		iOut.write(_L("\t\tUID = %08x\n"), iFilter->iUid.iUid);
		}
	else
		{
		iOut.write(_L("\t\tNo filtering on UID\n"));
		}

	if (iFilter->iSubjectKeyIdIsSet)
		{
		iOut.writeString(_L("\t\tKey identifier = "));
		iOut.writeOctetString(iFilter->iSubjectKeyId);
		iOut.writeNewLine();
		}
	else
		{
		iOut.writeString(_L("\t\tNo filtering on key ID\n"));
		}

	iOut.writeString(_L("\t\tFormats = "));	
	if (iFilter->iFormatIsSet)
		{
		switch (iFilter->iFormat)
			{
			case EWTLSCertificate:
				{
				iOut.writeString(_L("WTLS"));	
				break;
				}
			case EX509Certificate:
				{
				iOut.writeString(_L("X.509"));	
				break;
				}
			case EX509CertificateUrl:
				{
				iOut.writeString(_L("X.509 URL"));	
				break;
				}
			default:
				{
				iOut.writeString(_L("Unknown"));	
				break;
				}
			}
		}
	else
		{
		iOut.writeString(_L("All formats"));	
		}
	iOut.writeNewLine();

	iOut.writeString(_L("\t\tUsages = "));
	if (iFilter->iKeyUsage == EX509UsageAll)
		{
		iOut.writeString(_L("\t\t\tany\n"));
		}
	else
		{
		iOut.writeNewLine();
		if (iFilter->iKeyUsage & EX509UsageDigitalSignature)
			{
			iOut.writeString(_L("\t\t\tsignature\n"));
			}
		if (iFilter->iKeyUsage & EX509UsageNonRepudiation)
			{
			iOut.writeString(_L("\t\t\tnon-repudiation\n"));	
			}
		if (iFilter->iKeyUsage & EX509UsageDataEncipherment)
			{
			iOut.writeString(_L("\t\t\tdecryption\n"));	
			}
		if (iFilter->iKeyUsage & EX509UsageKeyEncipherment)
			{
			iOut.writeString(_L("\t\t\tkey encipher\n"));	
			}
		iOut.writeNewLine();
		}

	iOut.writeString(_L("\tExpected labels :\n"));
	TInt icount = iExpectedLabels.Count();
	for (TInt i = 0; i < icount; i++)
		{
		iOut.write(_L("\t\t%S\n"), &iExpectedLabels[i]);
		}
	}

void CListCertificates::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	iFinished = ETrue;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, aStatus.Int());
	}

void CListCertificates::AppendCCTCertInfoL(const TDesC8& aCCTCertInfoBuf)
	{
	TCert tempCA;
	tempCA.iName.FillZ(tempCA.iName.MaxLength());
	TInt pos = 0;
	TInt err = KErrNone;
	// get the expected certificate label
	tempCA.iName.Copy(Input::ParseElement(aCCTCertInfoBuf, KCertLabelStart, KCertLabelEnd, pos, err));
	
	// Figure out whether we expect this certificate to be readonly (ie. not deletable)
	// If no <ReadOnly> section is specified the default is not readonly (ie. deletable)
	TPtrC8 readOnly = Input::ParseElement(aCCTCertInfoBuf, KReadOnlyStart, KReadOnlyEnd, pos, err);
	if(err == KErrNone && readOnly.FindF(KTrue) != KErrNotFound)
		{
		tempCA.iReadOnly = ETrue;
		}
	else
		{
		tempCA.iReadOnly = EFalse;
		}
	User::LeaveIfError(iExpectedLabels.Append(tempCA));	
	}

void CListCertificates::DoCheckResult(TInt /*aError*/)
	{
	if (iFinished)
		{
		TRAPD(err, DoWriteResultL());
		if (err != KErrNone)
			{
			iOut.writeString(_L("\tDoWriteResultL failed"));
			}
		
		if (iResult)
			{
			iOut.writeString(_L("\tList certificates succeeded\n\n"));
			}
		else
			{
			iOut.writeString(_L("\tlist certificates failed\n\n"));
			
			//	If running tests with multiple threads, failure may be expected
			//  need to add this back in when we get concurrent tests working
			//CConcurrentTester::SanitizeTestResult(iOut, iResult);
			}
		iOut.writeNewLine();
		}
	}

void CListCertificates::DoWriteResultL()
	{
	iOut.writeString(_L("\t Certificates: \n"));

	for(TInt i = 0 ; i < iCertInfos.Count() ; ++i)
		{
		CCTCertInfo* entry = iCertInfos[i];
		
		iOut.write(_L("\t\tCert label = %S\n"), &entry->Label());
		
		iOut.writeString(_L("\t\tCert Format = "));
		switch (entry->CertificateFormat())
			{
			case EX509Certificate:
				{
				iOut.writeString(_L("X.509"));
				break;
				}
			case EWTLSCertificate:
				{
				iOut.writeString(_L("WTLS"));
				break;
				}
			case EX509CertificateUrl:
				{
				iOut.writeString(_L("X.509 URL"));
				break;
				}
			default:
				{
				iOut.writeString(_L("Unknown"));
				break;
				}
			}
		iOut.writeNewLine();
		
		iOut.writeString(_L("\t\tSubject key = "));		
		iOut.writeOctetString(entry->SubjectKeyId());
		iOut.writeNewLine();
		}
	}
