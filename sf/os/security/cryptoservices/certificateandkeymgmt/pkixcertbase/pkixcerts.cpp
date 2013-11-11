/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "pkixcerts.h"
#include <ccertattributefilter.h>
#include <cctcertinfo.h>

// CPKIXCertSource
////////////////////

MPKIXCertSource::~MPKIXCertSource()
	{
	}

// This function returns ETrue iff the issuer altname in aSubjectCert matches the
// subject altname in aIssuerCert
TBool MPKIXCertSource::AltNameMatchL(const CX509Certificate& aSubjectCert, 
									 const CX509Certificate& aIssuerCert) const 
	{
	TBool res = EFalse;
	const CX509CertExtension* subjectExt = aSubjectCert.Extension(KIssuerAltName);
	const CX509CertExtension* issuerExt = aIssuerCert.Extension(KSubjectAltName);
	if ((subjectExt) && (issuerExt))
		{
		const CX509AltNameExt* issuerAltName = CX509AltNameExt::NewLC(subjectExt->Data());
		const CX509AltNameExt* subjectAltName = CX509AltNameExt::NewLC(issuerExt->Data());
		if (subjectAltName->Match(*issuerAltName))
			{
			res = ETrue;
			}
		CleanupStack::PopAndDestroy(2);//subjectAltName, issuerAltName
		}
	return res;
	}

// CPKIXCertsFromStore
////////////////////////

CPKIXCertsFromStore* CPKIXCertsFromStore::NewL(MCertStore& aCertStore)
	{
	CPKIXCertsFromStore* self = CPKIXCertsFromStore::NewLC(aCertStore);
	CleanupStack::Pop(self);
	return self;
	}

CPKIXCertsFromStore* CPKIXCertsFromStore::NewLC(MCertStore& aCertStore)
	{
	CPKIXCertsFromStore* self = new(ELeave) CPKIXCertsFromStore(aCertStore);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CPKIXCertsFromStore* CPKIXCertsFromStore::NewL(MCertStore& aCertStore, TUid aClient)
	{
	CPKIXCertsFromStore* self = CPKIXCertsFromStore::NewLC(aCertStore, aClient);
	CleanupStack::Pop(self);
	return self;
	}

CPKIXCertsFromStore* CPKIXCertsFromStore::NewLC(MCertStore& aCertStore, TUid aClient)
	{
	CPKIXCertsFromStore* self = new(ELeave) CPKIXCertsFromStore(aCertStore, aClient);
	CleanupStack::PushL(self);
	self->ConstructL(aClient);
	return self;
	}

void CPKIXCertsFromStore::Initialize(TRequestStatus& aStatus)
	{
	// In the case of a WIM, we don't have trust settings,
	// the WIM store will ignore the filter and return all certificates
	aStatus = KRequestPending;
	iOriginalRequestStatus = &aStatus;
	iState = EInitialize;
	iCertStore.List(iCertInfos, *iFilter, iStatus);
	SetActive();
	}

void CPKIXCertsFromStore::CandidatesL(const CX509Certificate& aSubject,
									 RPointerArray<CX509Certificate>& aCandidates, 
									 TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	iOriginalRequestStatus = &aStatus;

	iRootName = &aSubject.IssuerName();
	iSubject = &aSubject;
	iCandidates = &aCandidates;
	iEntriesIndex = -1;
	
	iState = ECheckTrusted;
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

void CPKIXCertsFromStore::CancelCandidates()
	{
	Cancel();
	}

void CPKIXCertsFromStore::Release()
	{
	delete this;
	}

CPKIXCertsFromStore::~CPKIXCertsFromStore()
	{
	Cancel();

	iCertInfos.Close();

	delete iFilter;
	
	delete iCertData;
	delete iCertPtr;
	}

//private functions
CPKIXCertsFromStore::CPKIXCertsFromStore(MCertStore& aCertStore)
	: CActive(EPriorityNormal), iCertStore(aCertStore)
	{
	CActiveScheduler::Add(this);
	}

CPKIXCertsFromStore::CPKIXCertsFromStore(MCertStore& aCertStore, TUid aClient)
: CActive(EPriorityNormal), iClient(aClient), iCertStore(aCertStore)
	{
	CActiveScheduler::Add(this);
	}

void CPKIXCertsFromStore::ConstructL()
	{
	iFilter = CCertAttributeFilter::NewL();
	iFilter->SetFormat(EX509Certificate);
	iFilter->SetOwnerType(ECACertificate);
	}

void CPKIXCertsFromStore::ConstructL(TUid aClient)
	{
	iFilter = CCertAttributeFilter::NewL();
	iFilter->SetUid(aClient);
	iFilter->SetFormat(EX509Certificate);
	iFilter->SetOwnerType(ECACertificate);
	}

void CPKIXCertsFromStore::RunL()
	{
	User::LeaveIfError(iStatus.Int());

	switch (iState)
		{
		case EInitialize:
			iState = EIdle;
			User::RequestComplete(iOriginalRequestStatus, iStatus.Int());
			break;

		case ECheckTrusted:
			HandleECheckTrusted();
			break;

		case EGetCertificate:
			HandleEGetCertificateL();
			break;

		case EAddCandidate:
			HandleEAddCandidateL();
			break;

		case EEnd:
			iState = EIdle;
			User::RequestComplete(iOriginalRequestStatus, KErrNone);
			break;

		default:
			__ASSERT_ALWAYS(0, User::Panic(_L("CPKIXCertsFromStore"), 1));
			break;
			}
	}

TInt CPKIXCertsFromStore::RunError(TInt aError)
	{
	User::RequestComplete(iOriginalRequestStatus, aError);
	return KErrNone;
	}

void CPKIXCertsFromStore::DoCancel()
	{
	switch(iState)
		{
		case EInitialize:
			iCertStore.CancelList();
			break;

		case EGetCertificate:
			iCertStore.CancelTrusted();
			break;

		case EAddCandidate:
			iCertStore.CancelRetrieve();
			break;

		case ECheckTrusted:
		case EEnd:
			// nothing to do
			break;

		default:
			__ASSERT_ALWAYS(0, User::Panic(_L("CPKIXCertsFromStore"), 1));
			break;
		}
	
	User::RequestComplete(iOriginalRequestStatus, KErrCancel);
	iState = EIdle;
	}

void CPKIXCertsFromStore::HandleECheckTrusted()
	{// iEntriesIndex has been initialized to -1 by Candidates
	iEntriesIndex++;
	if (iEntriesIndex < iCertInfos.Count())
		{
		const CCTCertInfo* entry = iCertInfos[iEntriesIndex];

	//	Fix for DEF017139  "PKIXCert ignores trust"
	//	Check the certificate is trusted and discard it if not
		iCertStore.Trusted(*entry, iIsTrusted, iStatus);
		iState = EGetCertificate;
		}
	else
		{
		iState = EEnd;
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		}
	SetActive();
	}

void CPKIXCertsFromStore::HandleEGetCertificateL()
{
	if (iIsTrusted)
	{//	Fine to trust, go ahead
		const CCTCertInfo* entry = iCertInfos[iEntriesIndex];
		__ASSERT_DEBUG(!iCertData, User::Panic(_L("CPKIXCertsFromStore"), 1));
		iCertData = HBufC8::NewL(entry->Size());
		__ASSERT_DEBUG(!iCertPtr, User::Panic(_L("CPKIXCertsFromStore"), 1));
		iCertPtr = new(ELeave) TPtr8(iCertData->Des());
		iCertStore.Retrieve(*entry, *iCertPtr, iStatus);
		iState = EAddCandidate;
	}
	else
	{//	Not trusted, check next for trust
		iState = ECheckTrusted;
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
	}
		
	SetActive();
}


TBool CPKIXCertsFromStore::IsDuplicateL(const CX509Certificate& aCandidate)
	{
	
	TInt candidatesCount = iCandidates->Count();
	for (TInt i = 0; i < candidatesCount; i++)
		{
		
		// Certificate is a duplicate iff
		// a) The public keys have the same hash, and
		// b) The serial numbers are identical
		
		CX509Certificate* cert = (*iCandidates)[i];
		if (cert->KeyIdentifierL() == aCandidate.KeyIdentifierL()
			&& cert->SerialNumber() == aCandidate.SerialNumber())
			{
			return ETrue;
			}
		
		}
	
	return EFalse;
	
	}

void CPKIXCertsFromStore::HandleEAddCandidateL()
	{
	CX509Certificate *candidate = CX509Certificate::NewLC(*iCertData);
	delete iCertData;
	iCertData = 0;
	delete iCertPtr;
	iCertPtr = 0;

	if (iRootName->Count() > 0)
		{
		if (candidate->SubjectName().ExactMatchL(*iRootName) && !IsDuplicateL(*candidate))
			{
			User::LeaveIfError(iCandidates->Append(candidate));
			CleanupStack::Pop();	// candidate
			}
		else
			{
			CleanupStack::PopAndDestroy();	// candidate
			}
		}
	else
		{
		const CX500DistinguishedName& candidateName = candidate->SubjectName();
		if ((candidateName.Count() == 0) && (AltNameMatchL(*iSubject, *candidate)))
			{
			User::LeaveIfError(iCandidates->Append(candidate));
			CleanupStack::Pop();	// candidate
			}
		else
			{
			CleanupStack::PopAndDestroy();	// candidate
			}
		}

	iState = ECheckTrusted;
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

//CPKIXCertsFromClient
//public functions
CPKIXCertsFromClient* CPKIXCertsFromClient::NewL(const RPointerArray<CX509Certificate>& aCerts)
	{
	CPKIXCertsFromClient* self = new(ELeave) CPKIXCertsFromClient(aCerts);
	return self;
	}

CPKIXCertsFromClient* CPKIXCertsFromClient::NewLC(const RPointerArray<CX509Certificate>& aCerts)
	{
	CPKIXCertsFromClient* self = new(ELeave) CPKIXCertsFromClient(aCerts);
	CleanupStack::PushL(self);
	return self;
	}

void CPKIXCertsFromClient::Release()
	{
	delete this;
	}

CPKIXCertsFromClient::~CPKIXCertsFromClient()
	{
	}

void CPKIXCertsFromClient::CandidatesL(const CX509Certificate& aSubject,
									   RPointerArray<CX509Certificate>& aCandidates, 
									   TRequestStatus& aStatus)
	{
	// There is no need for this to be asynchronous but it is because the base class
	// wants this to be

	const CX500DistinguishedName& rootName = aSubject.IssuerName();
	TInt count = iCerts.Count();
	const CX509Certificate* candidate = NULL;
	if (rootName.Count() > 0)
		{
		for (TInt i = 0; i < count; i++)
			{
			candidate = iCerts[i];
			if (candidate->SubjectName().ExactMatchL(rootName))
				{
				CX509Certificate* cert = CX509Certificate::NewLC(*candidate);
				User::LeaveIfError(aCandidates.Append(cert));
				CleanupStack::Pop();
				}
			}
		}
	else
		{
		for (TInt i = 0; i < count; i++)
			{
			candidate = iCerts[i];
			const CX500DistinguishedName& candidateName = candidate->SubjectName();
			if ((candidateName.Count() ==0) && (AltNameMatchL(aSubject, *candidate)))
				{
				CX509Certificate* cert = CX509Certificate::NewLC(*candidate);
				User::LeaveIfError(aCandidates.Append(cert));
				CleanupStack::Pop();
				}
			}
		}

	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CPKIXCertsFromClient::CancelCandidates()
	{
	// Nothing to do because the function completes immediately
	}

//private functions
CPKIXCertsFromClient::CPKIXCertsFromClient(const RPointerArray<CX509Certificate>& aCerts)
	:iCerts(aCerts)
	{
	}
