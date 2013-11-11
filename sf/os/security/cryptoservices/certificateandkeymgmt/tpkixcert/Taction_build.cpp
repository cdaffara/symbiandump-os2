/*
* Copyright (c) 1998-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "taction_build.h"
#include "t_inputextra.h"

_LIT(KRoot, "build\\CA3_root.crt");
_LIT(KCA2, "build\\CA2.crt");
_LIT(KCA1, "build\\CA1.crt");
_LIT(KEE, "build\\EE.crt");

_LIT(KRootStart, "<root>");
_LIT(KRootEnd, "</root>");
_LIT(KCertificateStart, "<certificate>");
_LIT(KCertificateEnd, "</certificate>");
_LIT(KLabelStart, "<label>");
_LIT(KLabelEnd, "</label>");
_LIT(KExtraStart, "<extra>");
_LIT(KExtraEnd, "</extra>");
_LIT(KInitStart, "<init>");
_LIT(KInitEnd, "</init>");
_LIT(KAddCallStart, "<addcall>");
_LIT(KAddCallEnd, "</addcall>");

_LIT(KAddCertStart, "<addcert>");
_LIT(KAddCertEnd, "</addcert>");

_LIT(KCertPath, "\\pkixtestdata\\");

CAddCall* CAddCall::NewL(const TDesC& aBuf)
	{
	CAddCall* self = CAddCall::NewLC(aBuf);
	CleanupStack::Pop();
	return self;
	}

CAddCall* CAddCall::NewLC(const TDesC& aBuf)
	{
	CAddCall* self = new(ELeave) CAddCall;
	CleanupStack::PushL(self);
	self->ConstructL(aBuf);
	return self;
	}

CAddCall::~CAddCall()
	{
	delete iCertsToAdd;
	}

CAddCall::CAddCall()
	{
	}

void CAddCall::ConstructL(const TDesC& aBuf)
	{
	iCertsToAdd = new(ELeave) CDesCArrayFlat (1);
	TInt pos = 0;
	while(AddCert(aBuf, KAddCertStart, KAddCertEnd, pos, *iCertsToAdd))
		{
		}	
	}

TBool CAddCall::AddCert(const TDesC& aBuf, const TDesC& aStart, const TDesC& aEnd, TInt& aPos, CDesCArray& aCerts)
	{
	TPtrC certBuf = Input::ParseElement(aBuf, aStart, aEnd, aPos);
	if (certBuf != KNullDesC)
		{
		aCerts.AppendL(certBuf);
		return ETrue;
		}
		
	return EFalse;
	} 

CTestAction* CTestActionBuild::NewL(RFs& aFs,
										 CConsoleBase& aConsole,
										 Output& aOut, 
										 const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CTestActionBuild::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CTestAction* CTestActionBuild::NewLC(RFs& aFs,
										  CConsoleBase& aConsole,
										  Output& aOut,
										  const TTestActionSpec& aTestActionSpec)
	{
	CTestActionBuild* self = new(ELeave) CTestActionBuild(aFs, aConsole, 
		aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CTestActionBuild::~CTestActionBuild()
	{
	delete iUnifiedCertStore;
	delete iCertUtils;
	delete iChain;
	delete iExtraCertsFileName;
	delete iExtraCertsLabel;
	delete iInitCertsFileName;
	delete iInitCertsLabel;
	iAddCalls.ResetAndDestroy();
	iAddCalls.Close();
	delete iValidationResult;
	}

CTestActionBuild::CTestActionBuild(RFs& aFs, 
								   CConsoleBase& aConsole,
								   Output& aOut)
: CTestAction(aConsole, aOut), iFs(aFs)
	{
	}

void CTestActionBuild::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);

	TInt pos = 0;
	TInt err = KErrNone;
	HBufC* aBody = HBufC::NewLC(aTestActionSpec.iActionBody.Length());
	aBody->Des().Copy(aTestActionSpec.iActionBody);
	TPtrC rootCert(Input::ParseElement(*aBody,
		KRootStart, KRootEnd, pos, err));
	TInt dummyPos = 0;
	iRootCertFileName = Input::ParseElement(rootCert, KCertificateStart,
		KCertificateEnd, dummyPos);
	iRootCertLabel = Input::ParseElement(rootCert, KLabelStart,
		KLabelEnd, dummyPos);

	iExtraCertsFileName = new(ELeave) CDesCArrayFlat(1);
	iExtraCertsLabel = new(ELeave) CDesCArrayFlat(1);
	while(AddCert(*aBody, KExtraStart, KExtraEnd, pos, *iExtraCertsFileName, 
		*iExtraCertsLabel))
		{
		}	

	iInitCertsFileName = new(ELeave) CDesCArrayFlat(1);
	iInitCertsLabel = new(ELeave) CDesCArrayFlat(1);
	while(AddCert(*aBody, KInitStart, KInitEnd, pos, *iInitCertsFileName,
		*iInitCertsLabel))
		{
		}

	while(AddAddCallsL(*aBody, pos))
		{
		}

	iValidationResult = CPKIXValidationResult::NewL();
	
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TDriveName driveName(sysDrive.Name());
	iCertPath.Copy(driveName);
	iCertPath.Append(KCertPath);

	CleanupStack::PopAndDestroy(aBody);
	}

void CTestActionBuild::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	iActionState = EAction;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CTestActionBuild::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CTestActionBuild::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{
		case EInitCertStoreManager1:
			__ASSERT_DEBUG(!iUnifiedCertStore, User::Panic(_L("CPKIXCertTest"), 1));
			iUnifiedCertStore = CUnifiedCertStore::NewL(iFs, ETrue);	// We open the store for writing
			iUnifiedCertStore->Initialize(aStatus);
			iState = ERemoveCertsBeforeTest;
			break;

		case ERemoveCertsBeforeTest:
			// A new iCertUtils is created and destroyed for each test because
			// we need to close before validating as the chain won't be able to
			// open the store manager if it is already open for write elsewhere
			__ASSERT_DEBUG(!iCertUtils, User::Panic(_L("CPKIXCertTest"), 1));
			iCertUtils = CCertUtils::NewL(iFs);
			iCertUtils->RemoveCertsL(*iUnifiedCertStore, aStatus);
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
			iCertUtils->AddCert(iRootCertLabel, EX509Certificate, ECACertificate,
				1, iCertPath, iRootCertFileName, *iUnifiedCertStore, aStatus);//1 is trusted for our use
			iState = EAddIntermediateCerts;
			}
			break;

		case EAddIntermediateCerts:
			iCertUtils->AddCACertsL(*iExtraCertsFileName,
				*iExtraCertsLabel, EX509Certificate,
				2, iCertPath, *iUnifiedCertStore, aStatus);	// 2 not trusted for our use
			iState = EDoBuildTestStart;
			break;

		case EDoBuildTestStart:
			{
			// We have to close the store manager because it is open for write
			// and CPKIXCertChain won't be able to open it if we don't close it
			// iCertUtils muts also be deleted since it uses this store manager
			delete iCertUtils;
			iCertUtils = 0;
			delete iUnifiedCertStore;
			iUnifiedCertStore = 0;

			__ASSERT_DEBUG(!iChain, User::Panic(_L("CPKIXCertTest"), 1));

			HBufC8* initCerts = 
				InputExtra::ReadFilesLC(*iInitCertsFileName, iCertPath, iFs);
			TUid testUid = TUid::Uid(1);
			iChain = CPKIXCertChain::NewL(iFs, *initCerts, testUid);
			CleanupStack::PopAndDestroy(initCerts);	// initCerts

			TInt addCount = iAddCalls.Count();	
			for (TInt i = 0; i < addCount; i++)
				{
				const CAddCall* addCall = iAddCalls[i];
				HBufC8* addCerts = InputExtra::ReadFilesLC(*(addCall->iCertsToAdd),
					iCertPath, iFs);
				// the root of the chain is set when the certificate chain is getting created through the CPKIXCertChainAO
				// via CPKIXChainBuilder. If the chain has the root certificate then the iChainHasRoot is set which is returned
				// through this method. If the corresponding certificate does not have the issuer certificate in the chain then 
				// the extra certificates provided in the test script would be added for completing the chain.
				if (!(iChain->ChainHasRoot()))
					{
					iChain->AddCertL(*addCerts);
					}
				CleanupStack::PopAndDestroy();	// addCerts
				}

			iTime.HomeTime();

			iChain->ValidateL(*iValidationResult, iTime, aStatus);

			iState = EDoBuildTestFinished;
			}
			break;

		case EDoBuildTestFinished:
			{
			iResult = CompareChainsL(*iChain);
			delete iChain;
			iChain = 0;
			iState = EInitCertStoreManager2;
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, KErrNone);
			}
			break;

		case EInitCertStoreManager2:
			__ASSERT_DEBUG(!iChain, User::Panic(_L("CPKIXCertTest"), 1));
			//TRAP(err, HandleEDoBuildTests_SingleTest_InitCertStoreManager2L());
			iUnifiedCertStore = CUnifiedCertStore::NewL(iFs, ETrue);	// We open the store for writing
			iUnifiedCertStore->Initialize(aStatus);
			iState = ERemoveCertsAfterTest;
			break;
			
		case ERemoveCertsAfterTest:
			__ASSERT_DEBUG(!iCertUtils, User::Panic(_L("CPKIXCertTest"), 1));
			iCertUtils = CCertUtils::NewL(iFs);
			iCertUtils->RemoveCertsL(*iUnifiedCertStore, aStatus);
			iState = EEnd;
			break;

		case EEnd:
			{
			delete iCertUtils;
			iCertUtils = 0;
			delete iUnifiedCertStore;
			iUnifiedCertStore = 0;
			TRequestStatus* status = &aStatus;
			iFinished = ETrue;
			User::RequestComplete(status, KErrNone);
			}
			break;
		}
	}

TBool CTestActionBuild::TestResult(TInt /*aError*/)
	{
	return 0;
	}
	
void CTestActionBuild::PerformCancel()
	{
	}
	
void CTestActionBuild::Reset()
	{
	}

void CTestActionBuild::DoReportAction()
	{
	iConsole.Printf(_L("u"));
	}

void CTestActionBuild::DoCheckResult(TInt /*aError*/)
	{
	}

TBool CTestActionBuild::AddCert(const TDesC& aBuf, 
								const TDesC& aStart,
								const TDesC& aEnd, TInt& aPos, 
								CDesCArray& aCertsFileName,
								CDesCArray& aCertsLabel)
	{
	TPtrC certBuf = Input::ParseElement(aBuf, aStart, aEnd, aPos);
	TInt dummyPos = 0;
	if (certBuf != KNullDesC)
		{
		aCertsFileName.AppendL(Input::ParseElement(certBuf, KCertificateStart,
			KCertificateEnd, dummyPos));
		aCertsLabel.AppendL(Input::ParseElement(certBuf, KLabelStart,
			KLabelEnd, dummyPos));
		return ETrue;
		}
	return EFalse;
	} 

TBool CTestActionBuild::AddAddCallsL(const TDesC& aBuf, TInt& aPos)
	{
	TPtrC addCallsBuf = Input::ParseElement(aBuf, KAddCallStart, KAddCallEnd, aPos);
	if (addCallsBuf != KNullDesC)
		{
		CAddCall* addCall = CAddCall::NewLC(addCallsBuf);		
		iAddCalls.AppendL(addCall);
		CleanupStack::Pop(addCall);
		return ETrue;
		}
	return EFalse;
	}

TBool CTestActionBuild::CompareChainsL(const CPKIXCertChain& aChain)
	{
	if (aChain.Count() != 4)
		{
		return EFalse;
		}
	if (	(!CompareCertL(aChain.Cert(0), KEE))		||
			(!CompareCertL(aChain.Cert(1), KCA1))	||
			(!CompareCertL(aChain.Cert(2), KCA2))	||
			(!CompareCertL(aChain.Cert(3), KRoot))	)
		{
		return EFalse;
		}
	else
		{
		return ETrue;
		}
	}

TBool CTestActionBuild::CompareCertL(const CX509Certificate& aCert, const TDesC& aFilename)
	{
	HBufC8* correct = Input::ReadFileLC(aFilename, iCertPath, iFs);
	TBool res = (correct->Des() == aCert.Encoding());
	CleanupStack::PopAndDestroy();
	return res;
	}


