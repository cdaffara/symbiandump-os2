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


#include <swicertstore.h>
#include "CSWICertStoreImpl.h"
#include "CSWICertStoreToken.h"
#include "CSWICertStoreTokenType.h"

_LIT(KSWICertStorePanic, "CSWICertStore");

EXPORT_C CCTTokenType* CSWICertStore::CreateTokenTypeL()
	{
	// This method is just here so we don't have to declare
	// CSWICertStoreTokenType in the exported header
	return CSWICertStoreTokenType::NewL();
	}

EXPORT_C CSWICertStore* CSWICertStore::NewL(RFs& aFs)
	{
	// This method relies on the fact that we know the implementation of the swi
	// cert store token type is synchronous

	// Create token type
	MCTTokenType* tokenType = CSWICertStoreTokenType::NewL(aFs);
	CleanupReleasePushL(*tokenType);

	// Open token
	TRequestStatus stat;
	MCTToken* token = NULL;
	TUid tokenTypeUid = { KSWICertStoreTokenTypeUid };
	TCTTokenHandle tokenHandle(tokenTypeUid, CSWICertStoreTokenType::ESWICertStore);
	tokenType->OpenToken(tokenHandle, token, stat);
	User::WaitForRequest(stat);
	User::LeaveIfError(stat.Int());
	CleanupReleasePushL(*token);

	// Get interface
	MCTTokenInterface* tokenIf = NULL;
	token->GetInterface(TUid::Uid(KInterfaceCertStore), tokenIf, stat);
	User::WaitForRequest(stat);
	User::LeaveIfError(stat.Int());
	
	__ASSERT_ALWAYS(tokenIf, User::Panic(KSWICertStorePanic, 1));

	// Release token and token type
	CleanupStack::PopAndDestroy(2, tokenType);

	return static_cast<CSWICertStore*>(tokenIf);
	}

CSWICertStore* CSWICertStore::NewL(CSWICertStoreToken& aToken, RFs& aFs)
	{
	CSWICertStore* self = new (ELeave) CSWICertStore(aToken);
	CleanupStack::PushL(self);
	self->ConstructL(aFs);
	CleanupStack::Pop(self);
	return self;
	}

CSWICertStore::CSWICertStore(CSWICertStoreToken& aToken) :
	iToken(aToken)
	{
	}

void CSWICertStore::ConstructL(RFs& aFs)
	{
	iImpl = CSWICertStoreImpl::NewL(iToken, aFs);
	}

void CSWICertStore::DoRelease()
	{
	if (iToken.ReleaseInterface())
		{
		delete this;
		}
	}

CSWICertStore::~CSWICertStore()
	{
	delete iImpl;
	}

MCTToken& CSWICertStore::Token()
	{
	return iToken;
	}

void CSWICertStore::List(RMPointerArray<CCTCertInfo>& aCerts,
						 const CCertAttributeFilter& aFilter,
						 TRequestStatus& aStatus)
	{
	TRAPD(err, iImpl->ListL(aCerts, aFilter));
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CSWICertStore::CancelList()
	{
	}

void CSWICertStore::GetCert(CCTCertInfo*& aCertInfo,
							const TCTTokenObjectHandle& aHandle, 
							TRequestStatus& aStatus)
	{
	TRAPD(err, aCertInfo = iImpl->GetCertL(aHandle));
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}
		
void CSWICertStore::CancelGetCert()
	{
	}

void CSWICertStore::Applications(const CCTCertInfo& aCertInfo,
								 RArray<TUid>& aApplications,
								 TRequestStatus& aStatus)
	{
	TRAPD(err, iImpl->ApplicationsL(aCertInfo.Handle(), aApplications));
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CSWICertStore::CancelApplications()
	{
	}

void CSWICertStore::IsApplicable(const CCTCertInfo& aCertInfo,
								 TUid aApplication, 
								 TBool& aIsApplicable,
								 TRequestStatus& aStatus)
	{
	TRAPD(err, aIsApplicable = iImpl->IsApplicableL(aCertInfo.Handle(), aApplication));
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CSWICertStore::CancelIsApplicable()
	{
	}

void CSWICertStore::Trusted(const CCTCertInfo& aCertInfo,
							TBool& aTrusted, 
							TRequestStatus& aStatus)
	{
	TRAPD(err, aTrusted = iImpl->TrustedL(aCertInfo.Handle()));
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CSWICertStore::CancelTrusted()
	{
	}

void CSWICertStore::Retrieve(const CCTCertInfo& aCertInfo,
							 TDes8& aEncodedCert, 
							 TRequestStatus& aStatus)
	{
	TRAPD(err, iImpl->RetrieveL(aCertInfo.Handle(), aEncodedCert));
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CSWICertStore::CancelRetrieve()
	{
	}

EXPORT_C const TCertMetaInfo& CSWICertStore::CertMetaInfoL(const CCTCertInfo& aCertInfo)
	{
	return iImpl->CertMetaInfoL(aCertInfo.Handle());
	}


