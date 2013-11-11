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



#include "ValidateTest.h"
#include "t_input.h"
#include <ecom/ecom.h>

_LIT(KPath, "");
_LIT(KChainStart, "<chain>");
_LIT(KDateIssuedStart, "<dateissued>");
_LIT(KIOStart, "<io>");

CTestAction* CValidateTest::NewL(RFs& aFs, CConsoleBase& aConsole, 
		Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CValidateTest::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CTestAction* CValidateTest::NewLC(RFs& aFs, CConsoleBase& aConsole, 
		Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CValidateTest* self = new(ELeave) CValidateTest(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}


void CValidateTest::TestValidation(TRequestStatus& aStatus)
	{
	iOriginalRequestStatus = &aStatus;
	aStatus = KRequestPending;

	iState = EInit;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

CValidateTest::CValidateTest(RFs& aFs, 
								 CConsoleBase& aConsole,
								 Output& aOut)
: CTestAction(aConsole, aOut), iFs(aFs)
	{
	}

void CValidateTest::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	iValidationResult = CWTLSValidationResult::NewL();
	HBufC* aBody = HBufC::NewLC(aTestActionSpec.iActionBody.Length());
	aBody->Des().Copy(aTestActionSpec.iActionBody);
	// creates the test chan validation object
	iTestChain = CTestChainValidate::NewL();
	TPtrC chainBuf = Input::ParseElement(*aBody, KChainStart);
	
	iTestChain->AddChainL(chainBuf);
	TPtrC ioDateIssued = Input::ParseElement(*aBody, KDateIssuedStart);
	iTestChain->AddDateIssued(ioDateIssued);
	TPtrC ioBuf = Input::ParseElement(*aBody, KIOStart);
	iTestChain->AddIOL(ioBuf);

	CleanupStack::PopAndDestroy(aBody);
	}

CValidateTest::~CValidateTest()
	{
	delete iTestChain;
	delete iCertUtils;
	delete iValidationResult;
	delete iChain;
	REComSession::FinalClose();
	}

void CValidateTest::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	TInt err = KErrNone;
	
	switch (iState)
		{
		case EInit:
			iResult = ETrue;
			__ASSERT_DEBUG(!iCertUtils, User::Panic(_L("TWTLSCertTest"), 1));
			iCertUtils = CCertUtils::NewL(iFs);
			TRAP(err, HandleEInitL(aStatus));
			break;

		case EValidationStoreInitStoreManager1:
			TRAP(err, HandleEValidationInitStoreManager1L(aStatus));
			iActionState = EAction;
			break;		
		case EValidationStoreDepopulateStore1:
		case EValidationStorePopulateStoreRoots:
		case EValidationStorePopulateStoreExtras:
		case EValidationStoreValidate:
		case EValidationStoreValidated:
		case EValidationStoreInitStoreManager2:
		case EValidationStoreDepopulateStore2:
		case EValidationStoreEnd:
		case EValidationSuppliedInit:
		case EValidationSuppliedValidate:
		case EValidationSuppliedValidated:
		case EFinished:
			break;	//	Nothing to do, for compiler
		}
	}

void CValidateTest::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TInt err = KErrNone;

	switch (iState)
		{
	case EInit:
	case EValidationStoreInitStoreManager1:
	case EValidationStoreDepopulateStore1:
	case EValidationStorePopulateStoreRoots:
	case EValidationStorePopulateStoreExtras:
	case EValidationStoreValidate:
	case EValidationStoreValidated:
	case EValidationStoreInitStoreManager2:
	case EValidationStoreDepopulateStore2:
	case EValidationStoreEnd:
		break;	//	Nothing to do, for compiler
	case EValidationSuppliedInit:
			{
			iConsole.Printf(_L("started with supplied certs...\n"));
			iOut.writeString(_L("started with supplied certs..."));
			iOut.writeNewLine();

			iState = EValidationSuppliedValidate;
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, KErrNone);
			}
			break;

		case EValidationSuppliedValidate:
			{
			HBufC8* encodedCerts = ReadFilesLC(*iTestChain->iServerCerts);

			if(!encodedCerts)
				{
				TRequestStatus* status = &aStatus;
				iFinished = ETrue;
				User::RequestComplete(status, KErrNone);
				}
			else
				{
				TInt certCount = iTestChain->iRootCerts->MdcaCount();
				CArrayPtrFlat<CWTLSCertificate>* roots = 
					new(ELeave) CArrayPtrFlat<CWTLSCertificate>(1);
				TCleanupItem cleanup(CleanupCertArray, roots);
				CleanupStack::PushL(cleanup);
				for (TInt i = 0; i < certCount; i++)
					{
					// build the root certificates array including all the candidates.
					HBufC8* encCert = 
						ReadFileLC(iTestChain->iRootCerts->MdcaPoint(i));
					CWTLSCertificate* cert = CWTLSCertificate::NewLC(encCert->Des());
					roots->AppendL(cert);
					CleanupStack::Pop();	// cert
					CleanupStack::PopAndDestroy();	// encCert
					}

				__ASSERT_DEBUG(!iChain, User::Panic(_L("CValidateTest"), 1));
				iChain = CWTLSCertChain::NewL(iFs, *encodedCerts, *roots);

				CleanupStack::PopAndDestroy(2);	// encodedCerts, roots

				TDateTime dt(2000, EJuly, 0, 0, 0, 0, 0);
				if(iTestChain->iDateIssued == 1)
					{
					dt.SetYear(2002);
					}
				iTime = dt;
				
				iChain->ValidateL(*iValidationResult, iTime, aStatus);
				iState = EValidationSuppliedValidated;
				}
			}
			break;

		case EValidationSuppliedValidated:
			{
			delete iChain;
			iChain = 0;
			TWTLSValidationStatus* expectedStatus = iTestChain->iError;
			const TWTLSValidationStatus& actualStatus = iValidationResult->Error();

			iOut.writeString(_L("Expected Error = "));
			WriteError(expectedStatus->iReason);
			iOut.writeNewLine();

			iOut.writeString(_L("Actual Error = "));
			WriteError(actualStatus.iReason);
			iOut.writeNewLine();
					
			TInt wCount = iTestChain->iWarnings->Count();
			iOut.writeString(_L("Expected Warnings = "));
			iOut.writeNewLine();
			for (TInt i = 0; i < wCount; i++)
				{
				TWTLSValidationStatus warning = iTestChain->iWarnings->At(i);
				WriteError(warning.iReason);
				iOut.writeNewLine();
				}

			iOut.writeString(_L("Actual Warnings = "));
			iOut.writeNewLine();
			const CArrayFixFlat<TWTLSValidationStatus>& warnings = 
				iValidationResult->Warnings();
			wCount = warnings.Count();
			for (TInt j = 0; j < wCount; j++)
				{
				TWTLSValidationStatus warning = warnings.At(j);
				WriteError(warning.iReason);
				iOut.writeNewLine();
				}
			iOut.writeNewLine();

			if(expectedStatus->iReason != actualStatus.iReason)
				{
				iConsole.Printf(_L("FAILED!!!!\n"));
				iOut.writeString(_L("FAILED!!!!"));
				iOut.writeNewLine();
				iResult = EFalse;
				}
		
			// End of validatewith supplied
			if (err != KErrNone)
				{
				iOut.writeString(_L("Failed: leave code = "));
				iOut.writeNum(err);
				iOut.writeNewLine();
				}

			iState = EFinished;
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, KErrNone);
			}
			break;

		case EFinished:
			{
			TTime end;
			end.HomeTime();
			TTimeIntervalMicroSeconds intervalMS = end.MicroSecondsFrom(iStart);
			iConsole.Printf(_L("Time taken = %d milliseconds\n"), (intervalMS.Int64()/1000));
	
			TRequestStatus* status = &aStatus;
			iFinished = ETrue;
			User::RequestComplete(status, KErrNone);
			}
			break;
		}
	}

void CValidateTest::PerformAction(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;

	switch (iState)
		{
		case EValidationStoreDepopulateStore1:
			{
			iCertUtils->RemoveCertsL(aStatus);
			iState = EValidationStorePopulateStoreRoots;
			break;
			}

		case EValidationStorePopulateStoreRoots:
			{
			TUid uid = { 1 };
			TRAP_IGNORE(iCertUtils->RemoveApplicationL(uid));
			iCertUtils->AddApplicationL(_L("testwtls"), uid);
			iCertUtils->AddCACertsL(*iTestChain->iRootCerts, 
				*iTestChain->iRootLabels, EWTLSCertificate, 1, KPath, aStatus);
			iState = EValidationStorePopulateStoreExtras;
			break;
			}

		case EValidationStorePopulateStoreExtras:
			{
			iCertUtils->AddCACertsL(*iTestChain->iExtraCerts, 
				*iTestChain->iExtraLabels, EWTLSCertificate, 2, KPath, aStatus);
			iState = EValidationStoreValidate;
			break;
			}

		case EValidationStoreValidate:
			{
			HBufC8* encodedCerts = ReadFilesLC(*iTestChain->iServerCerts);

			TUid testUid = TUid::Uid(1);

			if(!encodedCerts)
				{
				TRequestStatus* status = &aStatus;
				iFinished = ETrue;
				User::RequestComplete(status, KErrNone);
				}
			else
				{
				__ASSERT_DEBUG(!iChain, User::Panic(_L("CValidateTest"), 1));
				iChain = CWTLSCertChain::NewL(iFs, *encodedCerts, testUid);
				CleanupStack::PopAndDestroy();	// encodedCerts

				TDateTime dt(2000, EJuly, 0, 0, 0, 0, 0);
				if(iTestChain->iDateIssued == 1)
					{
					dt.SetYear(2002);
					}
				iTime = dt;

				TRAP_IGNORE(iChain->ValidateL(*iValidationResult, iTime, aStatus));
				iState = EValidationStoreValidated;
				};
			break;
			}
			
		case EValidationStoreValidated:
			{
			TInt count = iChain->Count();
			if (count > 0)
				{
				iOut.writeString(_L("EE certificate = "));
				const CWTLSCertificate& eeCert = iChain->Cert(0);
				HBufC* eeSubject = eeCert.SubjectL();
				CleanupStack::PushL(eeSubject);
				iOut.writeString(*eeSubject);
				CleanupStack::PopAndDestroy();
				iOut.writeNewLine();
				for (TInt i = 1; i < count - 1; i++)
					{
					iOut.writeString(_L("Intermediate certificate = "));
					const CWTLSCertificate& iCert = iChain->Cert(i);
					HBufC* subject = iCert.SubjectL();
					CleanupStack::PushL(subject);
					iOut.writeString(*subject);
					CleanupStack::PopAndDestroy();
					iOut.writeNewLine();	
					}
				iOut.writeString(_L("Root certificate = "));
				const CWTLSCertificate& rCert = iChain->Cert(count-1);
				HBufC* rSubject = rCert.SubjectL();
				CleanupStack::PushL(rSubject);
				iOut.writeString(*rSubject);
				CleanupStack::PopAndDestroy();
				iOut.writeNewLine();	
				}

			TWTLSValidationStatus* expectedStatus = iTestChain->iError;
			const TWTLSValidationStatus& actualStatus = iValidationResult->Error();

			iOut.writeString(_L("Expected Error = "));
			WriteError(expectedStatus->iReason);
			iOut.writeNewLine();

			iOut.writeString(_L("Actual Error = "));
			WriteError(actualStatus.iReason);
			iOut.writeNewLine();
			
			TInt wCount = iTestChain->iWarnings->Count();
			iOut.writeString(_L("Expected Warnings = "));
			iOut.writeNewLine();
			for (TInt i = 0; i < wCount; i++)
				{
				TWTLSValidationStatus warning = iTestChain->iWarnings->At(i);
				WriteError(warning.iReason);
				iOut.writeNewLine();
				}

			iOut.writeString(_L("Actual Warnings = "));
			iOut.writeNewLine();
			const CArrayFixFlat<TWTLSValidationStatus>& warnings = 
				iValidationResult->Warnings();
			wCount = warnings.Count();
			for (TInt j = 0; j < wCount; j++)
				{
				TWTLSValidationStatus warning = warnings.At(j);
				WriteError(warning.iReason);
				iOut.writeNewLine();
				}
			iOut.writeNewLine();
			
			if(expectedStatus->iReason != actualStatus.iReason)
				{
				iConsole.Printf(_L("FAILED!!!!\n"));
				iOut.writeString(_L("FAILED!!!!"));
				iOut.writeNewLine();
				iResult = EFalse;
				}

			// End of DoValidate
			iState = EValidationStoreInitStoreManager2;				
			User::RequestComplete(status, KErrNone);
			delete iChain;
			iChain = 0;
			break;
			}

		case EValidationStoreInitStoreManager2:
			{
			User::RequestComplete(status, KErrNone);
			iState = EValidationStoreDepopulateStore2;
			break;
			}

		case EValidationStoreDepopulateStore2:
			{
			iCertUtils->RemoveCertsL(aStatus);
			TUid uid = { 1 };
			iCertUtils->RemoveApplicationL(uid);
			iState = EValidationStoreEnd;
			break;
			}

		case EValidationStoreEnd:
			{
			iState = EValidationSuppliedInit;
			iActionState = EPostrequisite;
			User::RequestComplete(status, KErrNone);
			break;
			}
		case EInit:
		case EValidationStoreInitStoreManager1:
		case EValidationSuppliedInit:
		case EValidationSuppliedValidate:
		case EValidationSuppliedValidated:
		case EFinished:
			break;	//	Nothing to do, for compiler
		}
	}

HBufC8* CValidateTest::ReadFileLC(const TDesC& aFilename)
	{
	RFile file;
	User::LeaveIfError(file.Open(iFs, aFilename, EFileRead));
	CleanupClosePushL(file);
	TInt size;
	file.Size(size);
	CleanupStack::PopAndDestroy(1);//fileClose

	HBufC8* res = HBufC8::NewLC(size);
	TPtr8 p(res->Des());
	p.SetLength(size);

	RFileReadStream stream;
	User::LeaveIfError(stream.Open(iFs, aFilename, EFileStream));
	CleanupClosePushL(stream);
	stream.ReadL(p, size);
	CleanupStack::PopAndDestroy();//streamClose...bleurgh
	return res;
	}


HBufC8* CValidateTest::ReadFilesLC(CDesCArray& aServerCerts)
	{
	TInt count = aServerCerts.MdcaCount();
	TInt totalSize = 0;
	TInt i;

	for (i = 0; i < count; i++)
		{
		TPtrC filename = aServerCerts.MdcaPoint(i);
		RFile file;
		TRAPD(err, file.Open(iFs, filename, EFileRead));
		if(err != KErrNone)
			{
			HBufC *failedToLoad = filename.AllocLC();
			SetScriptError(EFileNotFound, failedToLoad->Des());
			CleanupStack::PopAndDestroy(2);//fsclose, fileClose	
			return(NULL);
			};
		CleanupClosePushL(file);
		TInt size;
		file.Size(size);
		CleanupStack::PopAndDestroy(1);	//	fileClose
		totalSize += size;
		}

	HBufC8* res = HBufC8::NewLC(totalSize);
	TPtr8 pRes = res->Des();
	for (i = 0; i < count; i++)
		{
		HBufC8* cert = ReadFileLC(aServerCerts.MdcaPoint(i));
		pRes.Append(cert->Des());
		CleanupStack::PopAndDestroy();//cert
		}
	return res;
	}

void CValidateTest::WriteError(TValidationError aError)
	{
	switch(aError)
		{
		//errors
		case EValidatedOK:
			{
			iOut.writeString(_L("Validated OK"));
			break;
			}
		case EChainHasNoRoot:
			{
			iOut.writeString(_L("No trusted root found"));
			break;
			}
		case ESignatureInvalid:
			{
			iOut.writeString(_L("Signature invalid"));
			break;
			}
		case EDateOutOfRange:
			{
			iOut.writeString(_L("Date out of range"));
			break;
			}
		case ENamesDontChain:
			{
			iOut.writeString(_L("Names don't chain"));
			break;
			}
		case ENotCACert:
			{
			iOut.writeString(_L("Cert not authorised to sign other certs"));
			break;
			}

		default:
			{
			iOut.writeString(_L("Unknown"));
			break;
			}
		}
	}

void CValidateTest::CleanupCertArray(TAny* aCertArray)
	{
	CArrayPtrFlat<CWTLSCertificate>* certs = REINTERPRET_CAST(CArrayPtrFlat<CWTLSCertificate>*, aCertArray);
	certs->ResetAndDestroy();
	delete certs;
	}

void CValidateTest::HandleEInitL(TRequestStatus& aStatus)
	{
	iConsole.Printf(_L("Chain validation tests...\n"));
	iStart.HomeTime();

	iConsole.Printf(_L("started with store...\n"));
	iOut.writeString(_L("started with store..."));
	iOut.writeNewLine();

	iState = EValidationStoreInitStoreManager1;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CValidateTest::HandleEValidationInitStoreManager1L(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iState = EValidationStoreDepopulateStore1;
	}


void CValidateTest::DoReportAction()
	{
	}

void CValidateTest::DoCheckResult(TInt /*aError*/)
	{
	}

