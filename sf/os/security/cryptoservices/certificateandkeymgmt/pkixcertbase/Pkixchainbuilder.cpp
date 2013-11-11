/*
* Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "pkixchainbuilder.h"

CPKIXChainBuilder* CPKIXChainBuilder::NewL()
	{
	CPKIXChainBuilder* s = CPKIXChainBuilder::NewLC();
	CleanupStack::Pop(s);
	return s;
	}

CPKIXChainBuilder* CPKIXChainBuilder::NewLC()
	{
	CPKIXChainBuilder* s = new (ELeave) CPKIXChainBuilder;
	CleanupStack::PushL(s);
	s->ConstructL();
	return s;
	}

CPKIXChainBuilder::~CPKIXChainBuilder()
	{
	Cancel();
	iSources.Close();
	iCandidates.ResetAndDestroy();
	iCandidates.Close();
	}

void CPKIXChainBuilder::AddSourceL(MPKIXCertSource* aSource)
	{
	User::LeaveIfError(iSources.Append(aSource));
	}

void CPKIXChainBuilder::AddIssuer(TInt& aNumberOfCertsAdded,
								  TBool& aResult, 
								  CArrayPtrFlat<CX509Certificate>& aChain, 
								  TRequestStatus& aStatus)
	{
	iOriginalRequestStatus = &aStatus;
	aStatus = KRequestPending;
	iResult = &aResult;
	iChain = &aChain;
	iNumberOfCertsAdded = &aNumberOfCertsAdded;

	iSubject = aChain[aChain.Count()-1];
	__ASSERT_DEBUG(iSubject, User::Panic(_L("CPKICCertChainAO"), 1));
	__ASSERT_DEBUG(!iCandidates.Count(), User::Panic(_L("CPKICCertChainAO"), 1));

	iIndex = -1;
	iState = EAddCandidate;
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

CPKIXChainBuilder::CPKIXChainBuilder()
: CActive(EPriorityNormal)
	{
	CActiveScheduler::Add(this);
	}

void CPKIXChainBuilder::ConstructL()
	{
	}

TBool CPKIXChainBuilder::ResolveIssuersL(CArrayPtr<CX509Certificate>& aChain,
										 const RPointerArray<CX509Certificate>& aCandidates) const
	{
	//*this function attempts to figure out which certificate in aCandidates is the issuer of
	//the last cert in the aChain, and adds a *copy* of the best guess to aChain
	//*it assumes that the names match already
	//*if it establishes that none are any good it returns EFalse
	TInt count = aCandidates.Count();
	if (count == 0)
		{
		return EFalse;
		}
	if (count == 1)
		{
		CX509Certificate* cert = CX509Certificate::NewLC(*aCandidates[0]);
		aChain.AppendL(cert);
		(*iNumberOfCertsAdded)++;
		CleanupStack::Pop(cert);
		return ETrue;
		}

	const CX509Certificate* current = aChain[aChain.Count() - 1];
			//1)  look for SKI/AKI to distinguish
	const CX509CertExtension* akiExt = current->Extension(KAuthorityKeyId);
	if (akiExt)
		{
		const CX509AuthorityKeyIdExt* aki = CX509AuthorityKeyIdExt::NewLC(akiExt->Data());
		TPtrC8 authorityKeyId = aki->KeyId();
		if (authorityKeyId != KNullDesC8)
			{
			for (TInt i = 0; i < count; i++)
				{
				const CX509CertExtension* skiExt = (aCandidates[i])->Extension(KSubjectKeyId);
				if (skiExt)
					{
					const CX509SubjectKeyIdExt* ski = CX509SubjectKeyIdExt::NewLC(skiExt->Data());
					if (authorityKeyId == ski->KeyId())
						{
						CX509Certificate* issuer = CX509Certificate::NewLC(*aCandidates[i]);
						aChain.AppendL(issuer);
						(*iNumberOfCertsAdded)++;
						CleanupStack::Pop();//issuer
						CleanupStack::PopAndDestroy(2);//aki, ski
						return ETrue;
						}
					else
						{
						CleanupStack::PopAndDestroy();//ski
						}
					}
				}
			}
		//ok, we haven't got a key ID for the issuer, so try for a serial number instead...
		else
			{
			TPtrC8 authoritySerialNo = aki->AuthorityCertSerialNumber();
			for (TInt i = 0; i < count; i++)
				{
				const CX509Certificate* candidate = aCandidates[i];
				if (authoritySerialNo == candidate->SerialNumber())
					{
					CX509Certificate* issuer = CX509Certificate::NewLC(*candidate);
					aChain.AppendL(issuer);
					(*iNumberOfCertsAdded)++;
					CleanupStack::Pop();//issuer
					CleanupStack::PopAndDestroy();//aki
					return ETrue;
					}
				}
			}
		CleanupStack::PopAndDestroy();//aki
		}
	
	// If more then one subject matching candidates are found and they doesn't contain SKI/AKI,
	// consider the most recently imported/added candidate as the issuer.
	for(TInt index = count - 1; index >= 0; --index)
		{
		TTime currentTime;
		// if secure time is not available then fall back to the insecure version.
		if(currentTime.UniversalTimeSecure() == KErrNoSecureTime)
			{
			currentTime.UniversalTime();
			}

		CX509Certificate* cert = CX509Certificate::NewLC(*aCandidates[index]);
		// SKI/AKI are optional in versions lower than 3. So, relax the candidate selection rules only for version 1 & 2
		// If the recent candidate is not valid enough(cert expired), consider it's previous valid candidate.
		if((cert->Version() < 3) && (cert->ValidityPeriod().Valid(currentTime)))
			{				
			aChain.AppendL(cert);
			++(*iNumberOfCertsAdded);
			CleanupStack::Pop(cert);
			return ETrue;				
			}
		else
			{
			CleanupStack::PopAndDestroy(cert);
			continue;
			}			
		}

	return EFalse;
	}


void CPKIXChainBuilder::RunL()
	{
	User::LeaveIfError(iStatus.Int());

	switch (iState)
		{
	case EAddCandidate:
		iIndex++;
		if (iIndex < iSources.Count())
			{
			iSources[iIndex]->CandidatesL(*iSubject, iCandidates, iStatus);
			}
		else
			{
			iState = EFinished;
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrNone);
			}
		SetActive();
		break;

	case EFinished:
		iState = EIdle;
		*iResult = ResolveIssuersL(*iChain, iCandidates);
		iCandidates.ResetAndDestroy();
		User::RequestComplete(iOriginalRequestStatus, KErrNone);
		break;

	default:
		User::Panic(_L("CPKIXChainBuilder"), 1);
		break;
		}
	}

void CPKIXChainBuilder::DoCancel()
	{
	int i = 0;
	int end = iSources.Count();
	while (i < end)
		{
		iSources[i]->CancelCandidates();
		i++;
		}
	iCandidates.ResetAndDestroy();

	User::RequestComplete(iOriginalRequestStatus, KErrCancel);

	iState = EIdle;
	}

TInt CPKIXChainBuilder::RunError(TInt aError)
	{
	iState = EIdle;
	iCandidates.ResetAndDestroy();
	User::RequestComplete(iOriginalRequestStatus, aError);
	return KErrNone;
	}
