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


#include "tactionvalidate.h"
#include "t_inputextra.h"

_LIT(KChainStart, "<chain>");
_LIT(KChainEnd, "</chain>");
_LIT(KIOStart, "<io>");
_LIT(KIOEnd, "</io>");
_LIT(KCertPath, "\\pkixtestdata\\");

CActionValidate::~CActionValidate()
	{
	if (iPolicies)
		{
		iPolicies->ResetAndDestroy();
		delete iPolicies;
		iPolicies = 0;
		}

	delete iValidationResult;
	delete iChain;

	delete iCertUtils;

	delete iTestChain;
	delete iTestIO;
	}

CActionValidate::CActionValidate(RFs& aFs, 
								 CConsoleBase& aConsole,
								 Output& aOut)
: CTestAction(aConsole, aOut), iFs(aFs)
	{
	}


void CActionValidate::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	HBufC* aBody = HBufC::NewLC(aTestActionSpec.iActionBody.Length());
	aBody->Des().Copy(aTestActionSpec.iActionBody);
	TInt pos = 0;
	TInt err = KErrNone;
	TPtrC chainBuf = Input::ParseElement(*aBody, KChainStart, KChainEnd, pos, err);
	iTestChain = CTestChain::NewL(chainBuf);
	if (!AddParametersL(*aBody, pos))
		{
		// There must be at least one IO thing
		User::Leave(KErrNotFound);
		}
	while(AddParametersL(*aBody, pos))
		{
		}

	iValidationResult = CPKIXValidationResult::NewL();
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TDriveName driveName(sysDrive.Name());
	iCertPath.Copy(driveName);
	iCertPath.Append(KCertPath);

	CleanupStack::PopAndDestroy(aBody);
	}

void CActionValidate::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{
		case EDoValidateTestStart:
			{
			__ASSERT_DEBUG(!iChain, User::Panic(_L("CPKIXCertTest"), 1));
			TRAPD(err, CreateChainL());
			iState = EDoValidateTestValidate;
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, err);
			break;
			}

		case EDoValidateTestValidate:
			{
			// 1) write the overall result we expect
			iOut.writeSpaces(4);
			iOut.writeString(_L("Expected result = "));
			iOut.writeString(iTestIO->iError);
			iOut.writeNewLine();

			// 2) now do the validation

			iTime.UniversalTime();
	
			if (iTestIO->iPolicyInput->Count() > 0)
				{
				__ASSERT_DEBUG(!iPolicies, User::Panic(_L("CPKIXCertTest"), 1));
				__ASSERT_DEBUG(iChain, User::Panic(_L("CPKIXCertTest"), 1));

				iPolicies = new (ELeave) CArrayPtrFlat<HBufC> (1);
				TInt count = iTestIO->iPolicyInput->Count();
				for (TInt i = 0; i < count; i++)
					{
					TPtrC policy = iTestIO->iPolicyInput->MdcaPoint(i);
					HBufC* pBuf = policy.AllocL();
					CleanupStack::PushL(pBuf);
					iPolicies->AppendL(pBuf);
					CleanupStack::Pop();
					}
					
				iChain->ValidateL(*iValidationResult, iTime, *iPolicies, aStatus);	
				}
			else
				{
				iChain->ValidateL(*iValidationResult, iTime, aStatus);
				}
			iState = EDoValidateTestValidated;
			}
			break;

		case EDoValidateTestValidated:
			{
			// 3) write the overall result
			iOut.writeSpaces(4);
			iOut.writeString(_L("Actual result = "));
			CCertUtils::WriteError(iValidationResult->Error().iReason, iOut);
			TBuf<128> iActualResult = CCertUtils::MapError(iValidationResult->Error().iReason);
			iOut.writeNewLine();
			iOut.writeNewLine();

			// 4) write the policy info
			TBool checkingPolicies = 
				((iTestIO->iIPoliciesSet) || (iTestIO->iOPoliciesSet));
			if (checkingPolicies)
				{
				if (iTestIO->iIPoliciesSet)
					{
					// 4.1) policies we supplied
					iOut.writeSpaces(4);
					iOut.writeString(_L("Supplied policy set: "));
					iOut.writeNewLine();
					WritePolicies(*(iTestIO->iPolicyInput));
					}
				if (iTestIO->iOPoliciesSet)
					{
					// 4.2) policies we expect
					iOut.writeSpaces(4);
					iOut.writeString(_L("Expected user-constrained policy set: "));
					iOut.writeNewLine();
					WritePolicies(*(iTestIO->iExpectedPolicyOutput));
					}

				// 4.3) policies we collected
				iOut.writeSpaces(4);
				iOut.writeString(_L("Actual user-constrained policy set: "));
				iOut.writeNewLine();
				const CArrayPtrFlat<CX509CertPolicyInfo>& policies = iValidationResult->Policies();
				TInt actualPolicyCount = policies.Count();
				for (TInt j = 0; j < actualPolicyCount; j++)
					{
					iOut.writeSpaces(8);
					iOut.writeString(policies.At(j)->Id());
					iOut.writeNewLine();	
					}
				iOut.writeNewLine();	

			//check expected user-constrained policy set == actual user-constrained policy set
				TBool policyOutputCorrect = ETrue;
				CDesCArray* expectedPolicies = iTestIO->iExpectedPolicyOutput;
				TInt expectedPolicyCount = expectedPolicies->MdcaCount();
				if (expectedPolicyCount == actualPolicyCount)
					{
					for (TInt k = 0; k < expectedPolicyCount; k++)
						{
						TPtrC expectedPolicy = expectedPolicies->MdcaPoint(k);
						TBool policyFound = EFalse;
						for (TInt l = 0; l < actualPolicyCount; l++)
							{
							TPtrC actualPolicy = policies.At(l)->Id();
							if (actualPolicy == expectedPolicy)
								{
								policyFound = ETrue;
								break;
								}
							}
						if (!policyFound)
							{
							policyOutputCorrect = EFalse;
							}	
						}
					}
				else
					{
					policyOutputCorrect = EFalse;
					}
				iResult = (iTestIO->iError == iActualResult) && (policyOutputCorrect);
				}
			else	//! checking policies 
				{
				iResult = (iTestIO->iError == iActualResult);
				}
			iState = EDoValidateTestFinished;
			if (iPolicies)
				{
				iPolicies->ResetAndDestroy();
				delete iPolicies;
				iPolicies = 0;
				}
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, KErrNone);
			}
			break;

		case EDoValidateTestFinished:
			{
			__ASSERT_DEBUG(!iPolicies, User::Panic(_L("CPKIXCertTest"), 1));
			delete iChain;
			iChain = 0;
			iState = ERemoveCertsAfterTest;
			iActionState = EPostrequisite;
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, KErrNone);
			}
			break;
		default:
			break;
		}
	}

TBool CActionValidate::TestResult(TInt /*aError*/)
	{
	return 0;
	}
	
void CActionValidate::PerformCancel()
	{
	delete iChain;
	iChain = 0;
	}

void CActionValidate::AfterOOMFailure()
	{
	if (iPolicies)
		{
		iPolicies->ResetAndDestroy();
		delete iPolicies;
		iPolicies = 0;
		}
	}
	
void CActionValidate::Reset()
	{
	iState = EDoValidateTestStart;
	if (iPolicies)
		{
		iPolicies->ResetAndDestroy();
		delete iPolicies;
		iPolicies = 0;
		}
	delete iChain;
	iChain = 0;
	}

void CActionValidate::DoReportAction()
	{
	iConsole.Printf(_L("u"));
	}

void CActionValidate::DoCheckResult(TInt /*aError*/)
	{
	}

TBool CActionValidate::AddParametersL(const TDesC& aBuf, TInt& aPos)
	{
	TPtrC ioBuf = Input::ParseElement(aBuf, KIOStart, KIOEnd, aPos);
	if (ioBuf != KNullDesC)
		{
		iTestIO = CTestParameters::NewL(ioBuf);
		return ETrue;
		}
	return EFalse;
	}

void CActionValidate::WritePolicies(const CDesCArray& aPolicySet)
	{
	TInt count = aPolicySet.Count();
	for (TInt i = 0; i < count; i++)
		{
		iOut.writeSpaces(8);
		iOut.writeString(aPolicySet.MdcaPoint(i));
		iOut.writeNewLine();
		}
	}

/*
validate using the cert store as a source of root certificates
*/

CTestAction* CActionValidateWithStore::NewL(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CActionValidateWithStore::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CTestAction* CActionValidateWithStore::NewLC(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CActionValidateWithStore* self = new(ELeave) CActionValidateWithStore(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CActionValidateWithStore::CActionValidateWithStore(RFs& aFs, CConsoleBase& aConsole,Output& aOut)
	:CActionValidate(aFs, aConsole, aOut)
	{
	}

void CActionValidateWithStore::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	switch (iState)
		{
		case ERemoveCertsBeforeTest:
			__ASSERT_DEBUG(!iCertUtils, User::Panic(_L("CPKIXCertTest"), 1));
			iCertUtils = CCertUtils::NewL(iFs);
			iCertUtils->RemoveCertsL(aStatus);
			iState = EAddRoot;
			break;

		case EAddRoot:
			{
			TUid uid = { 1 };
			TRAPD(err, iCertUtils->RemoveApplicationL(uid));
			iCertUtils->AddApplicationL(_L("testpkix"), uid);
			TUid uid2 = { 2 };
			TRAP(err, iCertUtils->RemoveApplicationL(uid2));
			iCertUtils->AddApplicationL(_L("testpkix"), uid2);
			iCertUtils->AddCertL(iTestChain->iRootCertLabel,
				EX509Certificate, ECACertificate, 1, iCertPath,
				iTestChain->iRootCertFileName, 
				aStatus);	// 1 is trusted for our use
			iState = EAddIntermediateCerts;
			break;
			}

		case EAddIntermediateCerts:
			{
			iCertUtils->AddCACertsL(*(iTestChain->iIntermediateCertsFileName), 
				*(iTestChain->iIntermediateCertsLabel),
				EX509Certificate, 2, iCertPath, 
				aStatus);	//2 not trusted for our use
			iState = EDoValidateTestStart;
			iActionState = EAction;
			break;
			}
		default:
			break;
		}
	}

void CActionValidateWithStore::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	switch (iState)
		{
		case ERemoveCertsAfterTest:
			iCertUtils->RemoveCertsL(aStatus);
			iState = EEnd;
			break;

		case EEnd:
			{
			delete iCertUtils;
			iCertUtils = 0;
			TRequestStatus* status = &aStatus;
			iFinished = ETrue;
			User::RequestComplete(status, KErrNone);
			}
			break;
		default:
			break;
		}
	}


void CActionValidateWithStore::CreateChainL()
	{
	HBufC8* eeCert = 0;
			
	TRAPD(err, 
		eeCert = Input::ReadFileL(iTestChain->iEECertFileName,
		iCertPath, iFs));
	if (err != KErrNone)
		{
		iConsole.Printf(_L("Error : couldn't open file "));
		iConsole.Printf(iTestChain->iEECertFileName);
		iConsole.Printf(_L("\n"));
		iOut.writeString(_L("Error : couldn't open file "));
		iOut.writeString(iTestChain->iEECertFileName);
		iOut.writeNewLine();
		User::Leave(err);
		}
	CleanupStack::PushL(eeCert);
	TUid testUid = TUid::Uid(1);
	iChain = CPKIXCertChain::NewL(iFs, *eeCert, testUid);
	CleanupStack::PopAndDestroy(eeCert); 
	}

/*
validate using a set of candidates root certs supplied by the client
*/
CTestAction* CActionValidateWithSuppliedCerts::NewL(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CActionValidateWithSuppliedCerts::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;	
	}

CTestAction* CActionValidateWithSuppliedCerts::NewLC(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CActionValidateWithSuppliedCerts* self = new(ELeave) CActionValidateWithSuppliedCerts(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CActionValidateWithSuppliedCerts::CActionValidateWithSuppliedCerts(RFs& aFs, CConsoleBase& aConsole,Output& aOut)
	:CActionValidate(aFs, aConsole, aOut)
	{
	}

void CActionValidateWithSuppliedCerts::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CActionValidate::ConstructL(aTestActionSpec);
	iRootCerts = new(ELeave) RPointerArray<CX509Certificate>;
	}

CActionValidateWithSuppliedCerts::~CActionValidateWithSuppliedCerts()
	{
	if (iRootCerts)
		{
		iRootCerts->ResetAndDestroy();
		delete iRootCerts;
		}
	delete iEndEntityAndIntermediateCerts;
	}

void CActionValidateWithSuppliedCerts::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
//initialise the big descriptor containing ee cert followed by all intermediate certs
//and the array of candidate root certs

	HBufC8* eeCert = Input::ReadFileLC(iTestChain->iEECertFileName, iCertPath, iFs);
	HBufC8* inter = InputExtra::ReadFilesLC(*(iTestChain->iIntermediateCertsFileName), iCertPath, iFs);
	TInt totalSize = (eeCert->Size()) + (inter->Size());
	iEndEntityAndIntermediateCerts = HBufC8::NewL(totalSize);
	TPtr8 pRes = iEndEntityAndIntermediateCerts->Des();
	pRes.Append(*eeCert);
	pRes.Append(*inter);
	CleanupStack::PopAndDestroy(2);//eeCert, inter

	HBufC8* rootBuf = Input::ReadFileLC(iTestChain->iRootCertFileName, iCertPath, iFs);
	CX509Certificate* root = CX509Certificate::NewL(*rootBuf);
	CleanupStack::PopAndDestroy(rootBuf);
	CleanupStack::PushL(root);
	
	User::LeaveIfError(iRootCerts->Append(root));
	CleanupStack::Pop(root);//

	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iState = EDoValidateTestStart;
	iActionState = EAction;
	}

void CActionValidateWithSuppliedCerts::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	delete iCertUtils;
	iCertUtils = NULL;
	TRequestStatus* status = &aStatus;
	iFinished = ETrue;
	User::RequestComplete(status, KErrNone);
	}

void CActionValidateWithSuppliedCerts::CreateChainL()
	{
//create chain object
	iChain = CPKIXCertChain::NewL(iFs, *iEndEntityAndIntermediateCerts, *iRootCerts);
	}
