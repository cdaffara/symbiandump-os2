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

// t_metadataactions.cpp
#include <e32std.h>
#include <e32base.h>
#include <s32file.h>
#include "t_swicertstoreactions.h"
#include "t_certstoredefs.h"
#include "t_certstoreout.h"
#include "t_input.h"
#include <swicertstore.h>
#include "testutilclient.h"

TBool operator==(const TCapabilitySet& a, const TCapabilitySet& b)
	{
	return a.HasCapabilities(b) && b.HasCapabilities(a);
	}

// CInitSWICertStoreAction /////////////////////////////////////////////////////

CTestAction* CInitSWICertStoreAction::NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut,
										   const TTestActionSpec& aTestActionSpec)
	{
	CInitSWICertStoreAction* self = new (ELeave) CInitSWICertStoreAction(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CInitSWICertStoreAction::CInitSWICertStoreAction(RFs& aFs, CConsoleBase& aConsole, Output& aOut) :
	CCertStoreTestAction(aFs, aConsole, aOut), iFs(aFs)
	{
	}

void CInitSWICertStoreAction::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CCertStoreTestAction::ConstructL(aTestActionSpec);
	
	TInt err = KErrNone;
	TInt pos = 0;
	TPtrC8 fileName = Input::ParseElement(aTestActionSpec.iActionBody, KFileNameStart, 
		KFileNameEnd, pos, err);
	
	if (!err)
			{
			HBufC* tempFName = HBufC::NewLC(fileName.Length());

			tempFName->Des().Copy(fileName);

			RTestUtilSession testutil;
			User::LeaveIfError(testutil.Connect());
			CleanupClosePushL(testutil);
			
			TDriveUnit sysDrive(RFs::GetSystemDrive());
			TBuf<64> datfile (sysDrive.Name());
			datfile.Append(_L("\\Resource\\SwiCertstore\\dat\\"));
			testutil.MkDirAll(datfile);
			
			datfile.Copy(sysDrive.Name());
			datfile.Append(_L("\\Resource\\SwiCertstore\\dat\\00000001"));
			err = testutil.Copy(tempFName->Des(), datfile);
			CleanupStack::PopAndDestroy(&testutil);
			CleanupStack::PopAndDestroy(tempFName);
			}

	HBufC* result = HBufC::NewLC(aTestActionSpec.iActionResult.Length());
	TPtr(result->Des()).Copy(aTestActionSpec.iActionResult);
	Input::GetExpectedResultL(Input::ParseElement(*result, KReturnStart, KReturnEnd), iExpectedResult);
	CleanupStack::PopAndDestroy(result);
	}

void CInitSWICertStoreAction::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{
		case EInitCertStore:
			{
			iState = EFinished;
			CSWICertStore* swiCertStore = CSWICertStore::NewL(iFs);
			CleanupReleasePushL(*swiCertStore);
			AddCertStoreL(swiCertStore);
			CleanupStack::Pop(swiCertStore);
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, KErrNone);
			}
			break;

		case EFinished:
			{
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, KErrNone);
			iResult = EFalse;
			iFinished = ETrue;
			}
			break;
		}
	}

void CInitSWICertStoreAction::PerformCancel()
	{
	}

void CInitSWICertStoreAction::Reset()
	{
	iState = EInitCertStore;
	}

void CInitSWICertStoreAction::DoReportAction()
	{
	iOut.write(_L("Initialising SWI cert store...\n"));
	}

void CInitSWICertStoreAction::DoCheckResult(TInt aError)
	{
	iResult = (aError == iExpectedResult);
	}

// CSWICertStoreTestAction /////////////////////////////////////////////////////

CSWICertStoreTestAction::CSWICertStoreTestAction(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
	: CCertStoreTestAction(aFs, aConsole, aOut), iState(EGetCert)
	{
	}

void CSWICertStoreTestAction::ConstructL(const TTestActionSpec& aTestActionSpec)
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

	HBufC* result = HBufC::NewLC(aTestActionSpec.iActionResult.Length());
	TPtr(result->Des()).Copy(aTestActionSpec.iActionResult);
	Input::GetExpectedResultL(Input::ParseElement(*result, KReturnStart, KReturnEnd), iExpectedResult);
	CleanupStack::PopAndDestroy(result);

	DoConstructL(aTestActionSpec);
	}

CSWICertStoreTestAction::~CSWICertStoreTestAction()
	{
	iCertInfos.Close();
	delete iFilter;
	}

void CSWICertStoreTestAction::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{
		case EGetCert:
			iState = EPerformAction;
			SWICertStore().List(iCertInfos, *iFilter, aStatus);
			break;
			
		case EPerformAction:
			{
			TInt err = KErrNotFound;
			iState = EFinished;
			
			CCTCertInfo* certInfo = FindCertificate();
			if (certInfo)
				{
				err = KErrNone;
				DoPerformActionL(*certInfo);
				}
			
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, err);
			}	
			break;
			
		case EFinished:
			{
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, aStatus.Int());
			iResult = EFalse; // Must be set in DoCheckResult
            iFinished = ETrue;
			}
			break;
		}
	}

void CSWICertStoreTestAction::PerformCancel()
	{
	switch (iState)
		{
		case EGetCert:
			SWICertStore().CancelList();
			break;

		case EPerformAction:
			DoCancel();
			break;

		default:
			// do nothing
			break;
		}
	}

void CSWICertStoreTestAction::Reset()
	{
    iCertInfos.Close();
    iState = EGetCert;
	}

CCTCertInfo* CSWICertStoreTestAction::FindCertificate()
	{
	TInt count = iCertInfos.Count();
	for (TInt i = 0; i < count; i++)
		{
		CCTCertInfo* info = iCertInfos[i];
		if (info->Label() == iCertificateLabel)
			{
			return info;
			}
		}
	return NULL;
	}

// CGetCapabilitiesAction //////////////////////////////////////////////////////

CTestAction* CGetCapabilitiesAction::NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut,
										  const TTestActionSpec& aTestActionSpec)
	{
	CGetCapabilitiesAction* self = new (ELeave) CGetCapabilitiesAction(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CGetCapabilitiesAction::CGetCapabilitiesAction(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
	: CSWICertStoreTestAction(aFs, aConsole, aOut)
	{
	}

void CGetCapabilitiesAction::DoConstructL(const TTestActionSpec& aTestActionSpec)
	{
	if (iExpectedResult == KErrNone)
		{
		Input::ParseCapabilitySetL(aTestActionSpec.iActionResult, iExpectedCapapbilities);
		}
	}

void CGetCapabilitiesAction::DoReportAction()
	{
	iOut.writeString(_L("Getting certificate capabilities, expecting: "));
	iOut.writeCapabilitySetL(iExpectedCapapbilities);
	iOut.writeNewLine();
	}

void CGetCapabilitiesAction::DoPerformActionL(CCTCertInfo& aCertInfo)
	{
	iCapabilities = SWICertStore().CertMetaInfoL(aCertInfo).iCapabilities;
	}

void CGetCapabilitiesAction::DoCheckResult(TInt aError)
	{
	iResult = (aError == iExpectedResult &&
			   iCapabilities == iExpectedCapapbilities);

	if (aError == KErrNone)
		{
		iOut.writeString(_L("Certificate has capabilities: "));
		iOut.writeCapabilitySetL(iCapabilities);
		}
	}

void CGetCapabilitiesAction::DoCancel()
	{
	}

// CGetMandatoryAction //////////////////////////////////////////////////////

CTestAction* CGetMandatoryAction::NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut,
									   const TTestActionSpec& aTestActionSpec)
	{
	CGetMandatoryAction* self = new (ELeave) CGetMandatoryAction(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CGetMandatoryAction::CGetMandatoryAction(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
	: CSWICertStoreTestAction(aFs, aConsole, aOut)
	{
	}

void CGetMandatoryAction::DoConstructL(const TTestActionSpec& aTestActionSpec)
	{
	if (iExpectedResult == KErrNone)
		{
		iExpectedValue = Input::ParseElementBoolL(aTestActionSpec.iActionResult, KMandatoryStart);
		}
	}

void CGetMandatoryAction::DoReportAction()
	{
	iOut.writeString(_L("Getting certificate mandatory flag, expecting: "));
	iOut.writeBoolL(iExpectedValue);
	iOut.writeNewLine();
	}

void CGetMandatoryAction::DoPerformActionL(CCTCertInfo& aCertInfo)
	{
	iValue = SWICertStore().CertMetaInfoL(aCertInfo).iIsMandatory;
	}

void CGetMandatoryAction::DoCheckResult(TInt aError)
	{
	iResult = (aError == iExpectedResult &&
			   iValue == iExpectedValue);

	if (aError == KErrNone)
		{
		iOut.writeString(_L("Certificate mandatory flag: "));
		iOut.writeBoolL(iValue);
		}
	}

void CGetMandatoryAction::DoCancel()
	{
	}

// CGetSystemUpgradeAction //////////////////////////////////////////////////////

CTestAction* CGetSystemUpgradeAction::NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut,
									   const TTestActionSpec& aTestActionSpec)
	{
	CGetSystemUpgradeAction* self = new (ELeave) CGetSystemUpgradeAction(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CGetSystemUpgradeAction::CGetSystemUpgradeAction(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
	: CSWICertStoreTestAction(aFs, aConsole, aOut)
	{
	}

void CGetSystemUpgradeAction::DoConstructL(const TTestActionSpec& aTestActionSpec)
	{
	if (iExpectedResult == KErrNone)
		{
		iExpectedSystemUpgradeValue = Input::ParseElementBoolL(aTestActionSpec.iActionResult, KSystemUpgradeStart);
		}
	}

void CGetSystemUpgradeAction::DoReportAction()
	{
	iOut.writeString(_L("Getting certificate systemupgrade flag, expecting: "));
	iOut.writeBoolL(iExpectedSystemUpgradeValue);
	iOut.writeNewLine();
	}

void CGetSystemUpgradeAction::DoPerformActionL(CCTCertInfo& aCertInfo)
	{
	iSystemUpgradeValue = SWICertStore().CertMetaInfoL(aCertInfo).iIsSystemUpgrade;
	}

void CGetSystemUpgradeAction::DoCheckResult(TInt aError)
	{
	iResult = (aError == iExpectedResult &&
			   iSystemUpgradeValue == iExpectedSystemUpgradeValue);

	if (aError == KErrNone)
		{
		iOut.writeString(_L("Certificate systemupgrade flag: "));
		iOut.writeBoolL(iSystemUpgradeValue);
		}
	}

void CGetSystemUpgradeAction::DoCancel()
	{
	}

// CGetDeletableAction //////////////////////////////////////////////////////

CTestAction* CGetDeletionAction::NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut,
									   const TTestActionSpec& aTestActionSpec)
	{
	CGetDeletionAction* self = new (ELeave) CGetDeletionAction(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CGetDeletionAction::CGetDeletionAction(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
	: CSWICertStoreTestAction(aFs, aConsole, aOut)
	{
	}

void CGetDeletionAction::DoConstructL(const TTestActionSpec& aTestActionSpec)
	{
	if (iExpectedResult == KErrNone)
		{
		iExpectedDeletableValue = Input::ParseElementBoolL(aTestActionSpec.iActionResult, KDeletionStart);
		}
	}

void CGetDeletionAction::DoReportAction()
	{
	iOut.writeString(_L("Getting certificate deletable flag, expecting: "));
	iOut.writeBoolL(iExpectedDeletableValue);
	iOut.writeNewLine();
	}

void CGetDeletionAction::DoPerformActionL(CCTCertInfo& aCertInfo)
	{
	  iDeletableValue = aCertInfo.IsDeletable();
	}

void CGetDeletionAction::DoCheckResult(TInt aError)
	{
	iResult = (aError == iExpectedResult &&
			   iDeletableValue == iExpectedDeletableValue);

	if (aError == KErrNone)
		{
		iOut.writeString(_L("Certificate Deletable flag: "));
		iOut.writeBoolL(iDeletableValue);
		}
	}

void CGetDeletionAction::DoCancel()
	{
	}


// CMultipleReadersAction /////////////////////////////////////////////////////


CTestAction* CMultipleReadersAction::NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut,
										  const TTestActionSpec& aTestActionSpec)
       {
       CMultipleReadersAction* self = new (ELeave) CMultipleReadersAction(aFs,aConsole, aOut);
       CleanupStack::PushL(self);
       self->ConstructL(aTestActionSpec);
       CleanupStack::Pop(self);
       return self;
       }

CMultipleReadersAction::CMultipleReadersAction(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
       : CSWICertStoreTestAction(aFs, aConsole, aOut)
       {
       }

void CMultipleReadersAction::DoConstructL(const TTestActionSpec& /* aTestActionSpec*/)
       {
       }

void CMultipleReadersAction::DoReportAction()
       {
       iOut.writeString(_L("Checking opening multiple readers works, expecting:"));
       iOut.writeBoolL(true);
       iOut.writeNewLine();
       }

void CMultipleReadersAction::DoPerformActionL(CCTCertInfo& /*aCertInfo*/)
       {
       CSWICertStore* swiCertStore = 0;

       TRAPD(err, swiCertStore = CSWICertStore::NewL(iFs));
       
       if (err == KErrNone && swiCertStore != 0)
               {
               swiCertStore->Release();
               }
         iValue = err;
       }

void CMultipleReadersAction::DoCheckResult(TInt aError)
       {
       iResult = (aError == KErrNone &&
                          iValue == KErrNone);
       }

void CMultipleReadersAction::DoCancel()
       {
       }
