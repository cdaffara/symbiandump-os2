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
* tcertuils.cpp
*
*/


#include "tcertutils.h"
#include <wtlscert.h>
#include <certificateapps.h>
#include "t_input.h"
#include <ccertattributefilter.h>
#include <cctcertinfo.h>
#include <mctwritablecertstore.h>


EXPORT_C CCertUtils* CCertUtils::NewL(RFs& aFs)
	{
	CCertUtils* self = CCertUtils::NewLC(aFs);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CCertUtils* CCertUtils::NewLC(RFs& aFs)
	{
	CCertUtils* self = new(ELeave) CCertUtils(aFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CCertUtils::CCertUtils(RFs& aFs)
: CActive(EPriorityNormal), iFs(aFs)
	{
	CActiveScheduler::Add(this);
	}

EXPORT_C CCertUtils::~CCertUtils()
	{
	Cancel();

	delete iCertificate;
	delete iCreatedUnifiedCertStore;
	
	delete iCertData;
	delete iLabelData;
	delete iSecondCertUtils;

	delete iCAFilter;
	delete iUserFilter;

	iCACertStoreEntries.Close();		// The entries are owned by us
	iUserCertStoreEntries.Close();		// The entries are owned by us
	iTrusters.Close();
	}

/**
This function handles all the asynchronous calls. There is at least 
one state for each of the functions of CCertUtils that requires 
asynchronicity.
*/
void CCertUtils::RunL()
	{
	if (iStatus != KErrNone)
        {
        User::RequestComplete(iOriginalRequestStatus, iStatus.Int());
        return;
        }

	switch (iState)
		{
		// Used for AddCACerts
		case EAddCACerts:
			HandleEAddCACertsL();
			break;

		// Used for AddCert
		case EAddCert:
			TRAPD(err, HandleEAddCACertL());
			if (err != KErrNone)
				{
				iDiagnosticState = EAddCert;
                User::RequestComplete(iOriginalRequestStatus, err);
				}
			break;
		case EAddCACertGetCAEntry:
			HandleEAddCACertGetCAEntry();
			break;
		case EAddCACertSetApplications:
			HandleEAddCACertSetApplicationsL();
			break;
		case EAddCACertSetTrust:
			HandleEAddCACertSetTrust();
			break;
		case EAddCACertFinished:
			HandleEAddCACertFinishedL();
			delete iCreatedUnifiedCertStore;
			iCreatedUnifiedCertStore = 0;
			break;

		// Used for RemoveCerts
		case ERemoveCertsGetCACerts:
			HandleERemoveCertsGetCACerts();
			break;
		case ERemoveCertsCACertsRetrieved:
			HandleERemoveCertsCACertsRetrieved();
			break;
		case ERemoveCertsRemoveCACerts:
			HandleERemoveCertsRemoveCACerts();
			break;
		case ERemoveCertsGetUserCerts:
			HandleERemoveCertsGetUserCerts();
			break;
		case ERemoveCertsUserCertsRetrieved:
			HandleERemoveCertsUserCertsRetrieved();
			break;
		case ERemoveCertsRemoveUserCerts:
			HandleERemoveCertsRemoveUserCerts();
			break;
		case ERemoveCertsFinished:
			HandleERemoveCertsFinished();
			delete iCreatedUnifiedCertStore;
			iCreatedUnifiedCertStore = 0;
			break;

		default:
			break;
		}
	}

TInt CCertUtils::RunError(TInt aError)
    {
	User::RequestComplete(iOriginalRequestStatus, aError);
    return KErrNone;
    }

void CCertUtils::DoCancel()
	{
	if (iSecondCertUtils)
		iSecondCertUtils->Cancel();

	if (iUnifiedCertStore)
		{
		iUnifiedCertStore->Cancel();
		if (iUnifiedCertStore->WritableCertStoreCount() != 0)
			{
			MCTWritableCertStore *store;
			store = &iUnifiedCertStore->WritableCertStore(0);
			store->CancelRemove();
			store->CancelSetApplicability();
			}
		}

	if (iStore)
		iStore->CancelAdd();
	}

EXPORT_C CCertificate* CCertUtils::CertFromFileLC(const TDesC& aFilename, 
												  const TDesC& aPathname, 
												  RFs& aFs,
												  TCertificateFormat aFormat)
	{
	TFileName fullname;
	fullname.Append(aPathname);
	fullname.Append(aFilename);
	HBufC8* certBuf = Input::ReadFileLC(fullname, aFs);
	CCertificate* cert = 0;
	if (aFormat == EX509Certificate)
		{
		cert = CX509Certificate::NewLC(*certBuf);
		}
	else if (aFormat == EWTLSCertificate)
		{
		cert = CWTLSCertificate::NewLC(*certBuf);
		}
	CleanupStack::Pop();//cert
	CleanupStack::PopAndDestroy();//buf
	CleanupStack::PushL(cert);
	return cert;
	}

EXPORT_C CCertificate* CCertUtils::CertFromFileL(const TDesC& aFilename,
												 const TDesC& aPathname,
												 RFs& aFs,
												 TCertificateFormat aFormat)
	{
	CCertificate* cert = CertFromFileLC(aFilename, aPathname, aFs, aFormat);
	CleanupStack::Pop();
	return cert;
	}

EXPORT_C void CCertUtils::AddCertL(const TDesC& aLabel,
								  TCertificateFormat aFormat,
								  TCertificateOwnerType aCertificateOwnerType, 
								  TInt aTrustedUsage, 
								  const TDesC& aCertificatePath, 
								  const TDesC& aCertificateFileName,
								  TRequestStatus& aStatus)
	{
	iOriginalRequestStatus = &aStatus;
	aStatus = KRequestPending;

	// We set up the member variable as required for this function
	iCertificateFileName = &aCertificateFileName;
	iLabel = &aLabel;
	iFormat = aFormat;
	iTrustedUsage = aTrustedUsage;
	iPath = &aCertificatePath;
	iCertificateOwnerType = aCertificateOwnerType;

	if (iCreatedUnifiedCertStore)
		{
		delete iCreatedUnifiedCertStore;
		}

	iCreatedUnifiedCertStore = CUnifiedCertStore::NewL(iFs, ETrue);	// We want to open it for 
																	// writing
	iUnifiedCertStore = iCreatedUnifiedCertStore;
	iCreatedUnifiedCertStore->Initialize(iStatus);
	iState = EAddCert;
	SetActive();
	}

EXPORT_C void CCertUtils::AddCert(const TDesC& aLabel,
								  TCertificateFormat aFormat,
								  TCertificateOwnerType aCertificateOwnerType, 
								  TInt aTrustedUsage, 
								  const TDesC& aCertificatePath, 
								  const TDesC& aCertificateFileName,
								  CUnifiedCertStore& aUnifiedCertStore,
								  TRequestStatus& aStatus)
	{
	iOriginalRequestStatus = &aStatus;
	aStatus = KRequestPending;

	// We set up the member variable as required for this function
	iCertificateFileName = &aCertificateFileName;
	iLabel = &aLabel;
	iFormat = aFormat;
	iTrustedUsage = aTrustedUsage;
	iPath = &aCertificatePath;
	iCertificateOwnerType = aCertificateOwnerType;
	iUnifiedCertStore = &aUnifiedCertStore;
	
	iState = EAddCert;
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}

EXPORT_C void CCertUtils::AddCACertsL(const CDesCArray& aRoots,
									  const CDesCArray& aLabels,
									  TCertificateFormat aFormat,
									  TInt aTrustedUsage, 									
									  const TDesC& aPath,
									  TRequestStatus& aStatus)
	{
	iOriginalRequestStatus = &aStatus;
	aStatus = KRequestPending;

	// We set up the member variable as required for this function
	iRoots = &aRoots;
	iFormat = aFormat;
	iLabels = &aLabels;	
	iTrustedUsage = aTrustedUsage;
	iPath = &aPath;
	
	iIndex = -1;	// -1 because it will be incremented before its first use
	if (!iSecondCertUtils)
		{
		iSecondCertUtils = CCertUtils::NewL(iFs);
		}

	if (iCreatedUnifiedCertStore)
		{
		delete iCreatedUnifiedCertStore;
		}

	iCreatedUnifiedCertStore = CUnifiedCertStore::NewL(iFs, ETrue);	// We want to open it for 
																	// writing
	iUnifiedCertStore = iCreatedUnifiedCertStore;
	iCreatedUnifiedCertStore->Initialize(iStatus);

	iState = EAddCACerts;
	SetActive();
	}

EXPORT_C void CCertUtils::AddCACertsL(const CDesCArray& aRoots,
									  const CDesCArray& aLabels,
									  TCertificateFormat aFormat,
									  TInt aTrustedUsage, 									
									  const TDesC& aPath,
									  CUnifiedCertStore& aUnifiedCertStore, 
									  TRequestStatus& aStatus)
	{
	iOriginalRequestStatus = &aStatus;
	aStatus = KRequestPending;

	// We set up the member variable as required for this function
	iRoots = &aRoots;
	iFormat = aFormat;
	iLabels = &aLabels;
	iTrustedUsage = aTrustedUsage;
	iPath = &aPath;
	iUnifiedCertStore = &aUnifiedCertStore;

	iIndex = -1;	// -1 because it will be incremented before its first use
	if (!iSecondCertUtils)
		{
		iSecondCertUtils = CCertUtils::NewL(iFs);
		}

	iState = EAddCACerts;
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}

EXPORT_C void CCertUtils::RemoveCertsL(CUnifiedCertStore& aUnifiedCertStore,
									   TRequestStatus& aStatus)
	{
	iOriginalRequestStatus = &aStatus;
	aStatus = KRequestPending;	
	iUnifiedCertStore = &aUnifiedCertStore;

	iState = ERemoveCertsGetCACerts;
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

EXPORT_C void CCertUtils::RemoveCertsL(TRequestStatus& aStatus)
	{
	iOriginalRequestStatus = &aStatus;
	aStatus = KRequestPending;	
	iState = ERemoveCertsGetCACerts;
	
	if (iCreatedUnifiedCertStore)
		{
		delete iCreatedUnifiedCertStore;
		}
	
	iCreatedUnifiedCertStore = CUnifiedCertStore::NewL(iFs, ETrue);	// We want to open it for 
																	// writing
	iUnifiedCertStore = iCreatedUnifiedCertStore;
	iCreatedUnifiedCertStore->Initialize(iStatus);
	SetActive();
	}

EXPORT_C void CCertUtils::WriteError(TValidationError aError, Output& aOut)
	{
	aOut.writeString(CCertUtils::MapError(aError));
	}

EXPORT_C TPtrC CCertUtils::MapError(TValidationError aError)
	{

	switch(aError)
		{
		//errors
		case EValidatedOK:
			{
			return (_L("Validated OK"));
			}
		case EChainHasNoRoot:
			{
			return(_L("Chain has no root"));
			}
		case ESignatureInvalid:
			{
			return(_L("Signature invalid"));
			}
		case EDateOutOfRange:
			{
			return(_L("Date out of range"));
			}
		case ENameIsExcluded:
			{
			return(_L("Name is excluded"));
			}
		case ENameNotPermitted:
			{
			return(_L("Name is not permitted"));
			}
		case ECertificateRevoked:
			{
			return(_L("Certificate revoked"));
			}
		case EUnrecognizedCriticalExtension:
			{
			return(_L("Unrecognized Critical Extension"));
			}
		case ENoBasicConstraintInCACert:
			{
			return(_L("CA cert with no Basic Constraint"));
			}
		case ENoAcceptablePolicy:
			{
			return(_L("No acceptable policy"));
			}
		case EPathTooLong:
			{
			return(_L("Path too long"));
			}
		case ENegativePathLengthSpecified:
			{
			return(_L("Negative path length specified"));
			}
		case ENamesDontChain:
			{
			return(_L("Names don't chain"));
			}
		case ERequiredPolicyNotFound:
			{
			return(_L("Required policy not found"));
			}
		case EBadKeyUsage:
			{
			return(_L("Bad key usage"));
			}
		case ENotCACert:
			{
			return(_L("Non-CA cert used as CA cert"));
			}
		//warnings
		case ERootCertNotSelfSigned:
			{
			return(_L("Root cert not self-signed"));
			}
		case ECriticalExtendedKeyUsage:
			{
			return(_L("Critical extended key usage"));
			}
		case ECriticalCertPoliciesWithQualifiers:
			{
			return(_L("Critical cert policies with qualifiers"));
			}
		case ECriticalPolicyMapping:
			{
			return(_L("Critical policy mapping"));
			}
		case ECriticalDeviceId:
			{
			return(_L("Critical Device Id"));
			}
		case ECriticalSid:
			{
			return(_L("Critical Sid"));
			}
		case ECriticalVid:
			{
			return(_L("Critical Vid"));
			}
		case ECriticalCapabilities:
			{
			return(_L("Critical Capabilities"));
			}
		}
	return (_L("Unknown Error"));
	}


EXPORT_C HBufC* CCertUtils::DiagnosticLC() const
	{
	HBufC* result = HBufC::NewLC(600);
	switch (iDiagnosticState)
		{
		case EAddCert:
			result->Des().Append(_L("EAddCACert"));
			result->Des().Append(_L(" : "));
			result->Des().Append(iDiagnosticMessage);
			break;

		default:
			break;
		}

	return result;
	}

EXPORT_C void CCertUtils::AddApplicationL(const TDesC& aName, TUid aUid) const
	{
	CCertificateAppInfoManager* appManager = CCertificateAppInfoManager::NewL(iFs, ETrue);
	CleanupStack::PushL(appManager);

	// Only add the application if it doesn't exist already
	const RArray<TCertificateAppInfo>& apps = appManager->Applications();
	TInt i;
	for (i = 0 ; i < apps.Count() ; ++i)
		{
		if (apps[i].Id() == aUid && apps[i].Name() == aName)
			break;
		}

	if (i == apps.Count())
		{
		appManager->AddL(TCertificateAppInfo(aUid, aName));
		}
	
	CleanupStack::PopAndDestroy(appManager);
	}

EXPORT_C void CCertUtils::RemoveApplicationL(TUid aUid) const
	{
	CCertificateAppInfoManager* appManager = CCertificateAppInfoManager::NewL(iFs, ETrue);
	CleanupStack::PushL(appManager);
	appManager->RemoveL(aUid);
	CleanupStack::PopAndDestroy(appManager);
	}


void CCertUtils::ConstructL()
	{
	iCAFilter = CCertAttributeFilter::NewL();
	iCAFilter->SetOwnerType(ECACertificate);
	iUserFilter = CCertAttributeFilter::NewL();
	iUserFilter->SetOwnerType(EUserCertificate);
	}

void CCertUtils::HandleEAddCACertsL()
	{
	iIndex++;
	if (iIndex < iRoots->Count())
		{
		// We still have some certificates to add
		if (iCertData)
			{	
			delete iCertData;
			iCertData = 0;
			}
		iCertData = iRoots->MdcaPoint(iIndex).AllocL();
		if (iLabelData)
			{
			delete iLabelData;
			iLabelData = 0;
			}
		iLabelData = iLabels->MdcaPoint(iIndex).AllocL();
		iSecondCertUtils->AddCert(*iLabelData, iFormat, ECACertificate,
			iTrustedUsage, *iPath, *iCertData, *iUnifiedCertStore, iStatus);
		SetActive();
		}
	else
		{
		// We have finished adding all the certificates
		delete iSecondCertUtils;
		iSecondCertUtils = 0;
		delete iCreatedUnifiedCertStore;
		iCreatedUnifiedCertStore = 0;
		User::RequestComplete(iOriginalRequestStatus, KErrNone);
		}
	}

void CCertUtils::HandleEAddCACertL()
	{
	// At this stage we should always have an initialized iStoreManager
	__ASSERT_DEBUG(iUnifiedCertStore, User::Panic(_L("TCertUtils"), 1));
	
	// We use the first writable certstore
	iStore = &iUnifiedCertStore->WritableCertStore(0);
		
	__ASSERT_DEBUG(!iCertificate, User::Panic(_L("TCertUtils"), 1));
	iCertificate = 0;
	TRAPD(err, iCertificate =
		CCertUtils::CertFromFileL(*iCertificateFileName, *iPath, iFs, iFormat));
	if (err != KErrNone)
		{
		if (err != KErrNoMemory)
			{
			iDiagnosticMessage.Zero();
			iDiagnosticMessage.Append(_L("CertFromFileL failed ("));
			iDiagnosticMessage.Append(*iCertificateFileName);
			iDiagnosticMessage.Append(_L(")"));
			}
		User::Leave(err);
		}
	iEncoding.Set(iCertificate->Encoding());
	iStore->Add(*iLabel, iFormat, iCertificateOwnerType, 0, 0, iEncoding, iStatus);
	iState = EAddCACertGetCAEntry;
	SetActive();
	}

void CCertUtils::HandleEAddCACertGetCAEntry()
	{
	delete iCertificate;
	iCertificate = 0;
	iCACertStoreEntries.Close();
	iUnifiedCertStore->List(iCACertStoreEntries, *iCAFilter, iStatus);
	iState = EAddCACertSetApplications;
	SetActive();
	}

void CCertUtils::HandleEAddCACertSetApplicationsL()
	{
	CCTCertInfo* entry = 0;
	TInt iEnd = iCACertStoreEntries.Count();
	for (TInt i = 0; i < iEnd; i++)
		{
		if (iCACertStoreEntries[i]->Label() == *iLabel)
			{
			entry = iCACertStoreEntries[i];
			}
		}

	__ASSERT_ALWAYS(entry, User::Panic(_L("TCertUtils"), 1));

	iTrusters.Reset();
	TUid truster = { iTrustedUsage };
	User::LeaveIfError(iTrusters.Append(truster));
	
	iUnifiedCertStore->SetApplicability(*entry, iTrusters, iStatus);
	iState = EAddCACertSetTrust;
	SetActive();
	}

void CCertUtils::HandleEAddCACertSetTrust()
	{
	CCTCertInfo* entry = 0;
	TInt iEnd = iCACertStoreEntries.Count();
	for (TInt i = 0; i < iEnd; i++)
		{
		if (iCACertStoreEntries[i]->Label() == *iLabel)
			{
			entry = iCACertStoreEntries[i];
			}
		}

	__ASSERT_ALWAYS(entry, User::Panic(_L("TCertUtils"), 1));

	iUnifiedCertStore->SetTrust(*entry, ETrue, iStatus);
	iState = EAddCACertFinished;
	SetActive();
	}

void CCertUtils::HandleEAddCACertFinishedL()
	{
	User::RequestComplete(iOriginalRequestStatus, iStatus.Int());
	}

void CCertUtils::HandleERemoveCertsGetCACerts()
	{
	// At this stage we should always have an initialized iStoreManager
	__ASSERT_DEBUG(iUnifiedCertStore, User::Panic(_L("TCertUtils"), 1));

	iCACertStoreEntries.Close();
	iUnifiedCertStore->List(iCACertStoreEntries, *iCAFilter, iStatus);

	iState = ERemoveCertsCACertsRetrieved;
	SetActive();
	}

void CCertUtils::HandleERemoveCertsCACertsRetrieved()
	{
	// This index will be used to keep track of the current entry
	iIndex = -1;

	iState = ERemoveCertsRemoveCACerts;
	TRequestStatus* status = &iStatus;
	SetActive();
	User::RequestComplete(status, KErrNone);
	}

void CCertUtils::HandleERemoveCertsRemoveCACerts()
	{
	iIndex++;
	if (iIndex < iCACertStoreEntries.Count())
		{
		// Remove this certificate if it can be deleted.
		
		CCTCertInfo& cert = *iCACertStoreEntries[iIndex];
		
		// Unfortunately, certs in non-writable stores can still be
		// marked as deletable, so need to check if cert is also in a
		// writable store.
		
		TBool isDeletable = cert.IsDeletable();
		
		TBool inWritableStore = EFalse;
		
		TCTTokenObjectHandle certHandle(cert.Handle());
		TInt writeStoreCount = iUnifiedCertStore->WritableCertStoreCount();
		for (TInt i = 0; i < writeStoreCount; ++i)
			{
			MCTWritableCertStore& wcs = iUnifiedCertStore->WritableCertStore(i);
			if (wcs.Token().Handle() == certHandle.iTokenHandle)
				{
				inWritableStore = ETrue;
				break;
				}
			}
		
		if (isDeletable && inWritableStore)
			{
			iUnifiedCertStore->Remove(cert, iStatus);
			SetActive();
			}
		else
			{
			TRequestStatus* status = &iStatus;
			SetActive();
			User::RequestComplete(status, KErrNone);
			}
		}
	else
		{
		iState = ERemoveCertsGetUserCerts;
		TRequestStatus* status = &iStatus;
		SetActive();
		User::RequestComplete(status, KErrNone);
		}
	}

void CCertUtils::HandleERemoveCertsGetUserCerts()
	{
	// At this stage we should always have an initialized iStoreManager
	__ASSERT_DEBUG(iUnifiedCertStore, User::Panic(_L("TCertUtils"), 1));

	iUserCertStoreEntries.Close();
	iUnifiedCertStore->List(iUserCertStoreEntries, *iUserFilter, iStatus);

	iState = ERemoveCertsUserCertsRetrieved;
	SetActive();
	}

void CCertUtils::HandleERemoveCertsUserCertsRetrieved()
	{
	iIndex = -1;

	iState = ERemoveCertsRemoveUserCerts;
	TRequestStatus* status = &iStatus;
	SetActive();
	User::RequestComplete(status, KErrNone);
	}

void CCertUtils::HandleERemoveCertsRemoveUserCerts()
	{
	// At this stage we should always have an initialized iStoreManager
	__ASSERT_DEBUG(iUnifiedCertStore, User::Panic(_L("TCertUtils"), 1));

	iIndex++;
	if (iIndex < iUserCertStoreEntries.Count())
		{
		iUnifiedCertStore->Remove(*iUserCertStoreEntries[iIndex], iStatus);
		SetActive();
		}
	else
		{
		iState = ERemoveCertsFinished;
		TRequestStatus* status = &iStatus;
		SetActive();
		User::RequestComplete(status, KErrNone);
		}
	}

void CCertUtils::HandleERemoveCertsFinished()
	{
	User::RequestComplete(iOriginalRequestStatus, KErrNone);
	}

