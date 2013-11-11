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


#include "unifiedcertstore.h"
#include "unifiedcertstoreworkingvars.h"
#include "CCheckedCertStore.h"
#include <certificateapps.h>
#include <x509cert.h>
#include <wtlscert.h>
#include <hash.h>
#include <ecom/ecom.h>

_LIT(KUCSPanic, "CUnifiedCertStore"); 
#define assert(x) __ASSERT_ALWAYS((x), User::Panic(KUCSPanic, 1));

/////////////////////////////////////////////////////////////////////////////////////
//CUnifiedCertStore
/////////////////////////////////////////////////////////////////////////////////////

EXPORT_C CUnifiedCertStore* CUnifiedCertStore::NewL(RFs& aFs, TBool aOpenForWrite)
	{
	CUnifiedCertStore* self = CUnifiedCertStore::NewLC(aFs, aOpenForWrite);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CUnifiedCertStore* CUnifiedCertStore::NewLC(RFs& aFs, TBool aOpenForWrite)
	{
	CUnifiedCertStore* self = new(ELeave) CUnifiedCertStore(aFs, aOpenForWrite);
	CleanupStack::PushL(self);
	RArray<TInt> orderingFilter;
	self->ConstructL(orderingFilter);
	return self;
	}

EXPORT_C CUnifiedCertStore::~CUnifiedCertStore()
	{
	Cancel();

	assert(!iWorkingVars);

	TInt end = iReadOnlyCertStores.Count();
	TInt i;
	for (i = 0; i < end; i++)
		{
		iReadOnlyCertStores[i]->Release();
		}
	iReadOnlyCertStores.Close();

	end = iWritableCertStores.Count();
	for (i = 0; i < end; i++)
		{
		iWritableCertStores[i]->Release();
		}
	iWritableCertStores.Close();

	// The elements are already released by the two loops above
	iCertStores.Close();
	
	// release resources allocated to order attributes list
	iOrderAttributes.Close();
    
    DestroyTemporaryMembers();
	iHardwareTypeUids.Close();
	REComSession::FinalClose();
	}


EXPORT_C void CUnifiedCertStore::Initialize(TRequestStatus& aStatus)
	{
	BeginAsyncOp(aStatus, EInitializeGetTokenList);
	TRAPD(err, InitializeL());
	if (err != KErrNone)
		{
		Complete(err);
		}
	}

void CUnifiedCertStore::InitializeL()
	{
	AllocWorkingVarsL();
	
	// We want the list of all token types that support a readable or writable 
	// certstore interface
	RArray<TUid> uidArray;
	CleanupClosePushL(uidArray);
		
	User::LeaveIfError(uidArray.Append(TUid::Uid(KInterfaceWritableCertStore)));
	
	TCTFindTokenTypesByInterface filter(uidArray.Array());
	CCTTokenTypeInfo::ListL(iWorkingVars->iWritableTokenTypes, filter);

	uidArray.Reset();

	User::LeaveIfError(uidArray.Append(TUid::Uid(KInterfaceCertStore)));

	RCPointerArray<CCTTokenTypeInfo> tokenTypes;
	CleanupClosePushL(tokenTypes);

	TCTFindTokenTypesByInterface filter2(uidArray.Array());
	CCTTokenTypeInfo::ListL(tokenTypes, filter2);
	
  	// Check whether client specified order list has attributes in it
  	if(iOrderAttributes.Count() > 0)
  	    {
  	    ApplyOrderingL(tokenTypes);
  	    }

	// Make a note of all hardware token types
	TInt i = 0;
	TInt end = tokenTypes.Count();
	for (; i < end; i++)
		{
		TCTTokenTypeAttribute software;
		software.iUID = KCTSoftware;
		TInt find = tokenTypes[i]->Attributes().Find(software);
		// In the case (TInt)ETrue == KThirdPartyCertStore == 1  
		if (find != KErrNotFound && tokenTypes[i]->Attributes()[find].iVal != 
			(TInt)ETrue && tokenTypes[i]->Attributes()[find].iVal != KManufactureCertStore)
			{
			// This is a hardware type. Add its UID to the list.
			User::LeaveIfError(iHardwareTypeUids.Append(tokenTypes[i]->Type()));
			}
		}

	i = 0;
	while (i < end)
		{
		TInt j = 0;
		TInt jEnd = iWorkingVars->iWritableTokenTypes.Count();
		while (j < jEnd)
			{
			if (iWorkingVars->iWritableTokenTypes[j]->Type() == tokenTypes[i]->Type())
				{
				break;
				}
			j++;
			}
		if (j == jEnd)
			{
			User::LeaveIfError(iWorkingVars->iReadOnlyTokenTypes.Append(tokenTypes[i]));
			tokenTypes.Remove(i);
			end--;
			}
		else
			{
			i++;
			}
		}

	CleanupStack::PopAndDestroy(2);	// uidArray, tokenTypes
	
	iWorkingVars->iIndex = -1;

	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

EXPORT_C void CUnifiedCertStore::CancelInitialize()
	{
	if (iState == EInitializeGetTokenList ||
		iState == EInitializeGetToken ||
		iState == EInitializeGetWritableInterface ||
		iState == EInitializeGetReadableInterface ||
		iState == EInitializeGetReadableInterfaceFinished ||
		iState == EInitializeFinished)
		{
		Cancel();
		}	
	}

void CUnifiedCertStore::List(RMPointerArray<CCTCertInfo>& aCertInfos,
							 const CCertAttributeFilter& aFilter,
							 TRequestStatus& aStatus)
	{
	BeginAsyncOp(aStatus, EList);	
	TRAPD(err, ListL(aCertInfos, aFilter));
	if (err != KErrNone)
		{
		Complete(err);
		}
	}

void CUnifiedCertStore::ListL(RMPointerArray<CCTCertInfo>& aCertInfos,
							  const CCertAttributeFilter& aFilter)
	{
	if (!iIsInitialized)
		{
		User::Leave(KErrNotReady);
		}

	AllocWorkingVarsL();
	iWorkingVars->iCertInfos = &aCertInfos;
	iWorkingVars->iFilter = &aFilter;
	iWorkingVars->iCertIndex = aCertInfos.Count();
	iIndex = -1;
	
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}

void CUnifiedCertStore::CancelList()
	{
	if (iState == EList ||
		iState == ERetrieveForList)
		{
		Cancel();
		}
	}

EXPORT_C void CUnifiedCertStore::List(RMPointerArray<CCTCertInfo>& aCertInfos,
									  const CCertAttributeFilter& aFilter, 
									  const TDesC8& aIssuer, 
									  TRequestStatus& aStatus)
	{
	RPointerArray<const TDesC8> array;
	if (array.Append(&aIssuer) != KErrNone)
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrNoMemory);
		}
	else
		{
		List(aCertInfos, aFilter, array, aStatus);
		array.Close();
		}
	}

EXPORT_C void CUnifiedCertStore::List(RMPointerArray<CCTCertInfo>& aCertInfos,
									  const CCertAttributeFilter& aFilter, 
									  RPointerArray<const TDesC8> aIssuers, 
									  TRequestStatus& aStatus)
	{
	BeginAsyncOp(aStatus, EList);	
	TRAPD(err, ListL(aCertInfos, aFilter, aIssuers));
	if (err != KErrNone)
		{
		Complete(err);
		}
	}

void CUnifiedCertStore::ListL(RMPointerArray<CCTCertInfo>& aCertInfos,
							  const CCertAttributeFilter& aFilter, 
							  RPointerArray<const TDesC8> aIssuers)
	{
	// Obscure special case: If aIssuers has no elements, we should
	// return nothing.
	if (aIssuers.Count() == 0)
		{
		Complete(KErrNone);
		return;
		}

	AllocWorkingVarsL();
	iWorkingVars->iCertInfos = &aCertInfos;
	iWorkingVars->iFilter = &aFilter;
	iWorkingVars->iCertIndex = aCertInfos.Count();
	
	TInt count = aIssuers.Count();
	for (TInt i = 0 ; i < count ; ++i)
		{
		User::LeaveIfError(iWorkingVars->iIssuerNames.Append(aIssuers[i]));
		}
	
	iIndex = -1;
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);	
	}

EXPORT_C void CUnifiedCertStore::Retrieve(const CCTCertInfo& aCertInfo, 
								 CCertificate*& aCert,
								 TRequestStatus& aStatus)
	{
	BeginAsyncOp(aStatus, ERetrieve);	
	TRAPD(err, RetrieveL(aCertInfo, aCert));
	if (err != KErrNone)
		{
		Complete(err);
		}
	}

void CUnifiedCertStore::RetrieveL(const CCTCertInfo& aCertInfo, 
								  CCertificate*& aCert)
	{
	FindCertStoreL(aCertInfo.Handle());

    if (aCertInfo.CertificateFormat() != EX509Certificate &&
        aCertInfo.CertificateFormat() != EWTLSCertificate)
        {
        User::Leave(KErrNotSupported);
        }

	AllocWorkingVarsL();
	iWorkingVars->iCertDesC = HBufC8::NewMaxL(aCertInfo.Size());
	iWorkingVars->iReturnedCert = &aCert;
	iWorkingVars->iCertType = aCertInfo.CertificateFormat();
	iWorkingVars->iCertDes.Set(iWorkingVars->iCertDesC->Des());
	iCurrentCertStore->Retrieve(aCertInfo, iWorkingVars->iCertDes, iStatus);
	SetActive();
    }

void CUnifiedCertStore::GetCert(CCTCertInfo*& aCertInfo, 
								const TCTTokenObjectHandle& aHandle, 
								TRequestStatus& aStatus)
	{
	BeginAsyncOp(aStatus, EGetCert);
	TRAPD(err, GetCertL(aCertInfo, aHandle));
	if (err != KErrNone)
		{
		Complete(err);
		}
	}

void CUnifiedCertStore::GetCertL(CCTCertInfo*& aCertInfo,
								 const TCTTokenObjectHandle& aHandle)
	{
	FindCertStoreL(aHandle);
	iCurrentCertStore->GetCert(aCertInfo, aHandle, iStatus);
	SetActive();
	}

void CUnifiedCertStore::CancelGetCert()
	{
	if (iState == EGetCert)
		{
		Cancel();
		}
	}

void CUnifiedCertStore::Applications(const CCTCertInfo& aCertInfo,
									 RArray<TUid>& aApplications,
									 TRequestStatus& aStatus)
	{
	BeginAsyncOp(aStatus, EApplications);
	TRAPD(err, ApplicationsL(aCertInfo, aApplications));
	if (err != KErrNone)
		{
		Complete(err);
		}
	}

void CUnifiedCertStore::ApplicationsL(const CCTCertInfo& aCertInfo,
									 RArray<TUid>& aApplications)
	{
	FindCertStoreL(aCertInfo.Handle());
	iCurrentCertStore->Applications(aCertInfo, aApplications, iStatus);
	SetActive();
	}

void CUnifiedCertStore::CancelApplications()
	{
	if (iState == EApplications)
		{
		Cancel();
		}
	}

void CUnifiedCertStore::IsApplicable(const CCTCertInfo& aCertInfo,
									 TUid aApplication, 
									 TBool& aIsApplicable, 
									 TRequestStatus& aStatus)
	{
	BeginAsyncOp(aStatus, EIsApplicable);
	TRAPD(err, IsApplicableL(aCertInfo, aApplication, aIsApplicable));
	if (err != KErrNone)
		{
		Complete(err);
		}
	}

void CUnifiedCertStore::IsApplicableL(const CCTCertInfo& aCertInfo,
									  TUid aApplication, 
									  TBool& aIsApplicable)
	{
	FindCertStoreL(aCertInfo.Handle());
	iCurrentCertStore->IsApplicable(aCertInfo, aApplication, aIsApplicable, iStatus);
	SetActive();
	}

void CUnifiedCertStore::CancelIsApplicable()
	{
	if (iState == EIsApplicable)
		{
		Cancel();
		}
	}

void CUnifiedCertStore::Trusted(const CCTCertInfo& aCertInfo, 
								TBool& aTrusted, 
								TRequestStatus& aStatus)
	{
	BeginAsyncOp(aStatus, ETrusted);
	TRAPD(err, TrustedL(aCertInfo, aTrusted));
	if (err != KErrNone)
		{
		Complete(err);
		}
	}

void CUnifiedCertStore::TrustedL(const CCTCertInfo& aCertInfo, 
								 TBool& aTrusted)
	{
	FindCertStoreL(aCertInfo.Handle());
	iCurrentCertStore->Trusted(aCertInfo, aTrusted, iStatus);
	SetActive();
	}

void CUnifiedCertStore::CancelTrusted()
	{
	if (iState == ETrusted)
		{
		Cancel();
		}
	}

void CUnifiedCertStore::Retrieve(const CCTCertInfo& aCertInfo,
								 TDes8& aEncodedCert,
								 TRequestStatus& aStatus)
	{
	BeginAsyncOp(aStatus, ERetrieveData);
	TRAPD(err, RetrieveDataL(aCertInfo, aEncodedCert));
	if (err != KErrNone)
		{
		Complete(err);
		}
	}

void CUnifiedCertStore::RetrieveDataL(const CCTCertInfo& aCertInfo,
									  TDes8& aEncodedCert)
	{
	FindCertStoreL(aCertInfo.Handle());
	iCurrentCertStore->Retrieve(aCertInfo, aEncodedCert, iStatus);	
	SetActive();
	}

void CUnifiedCertStore::CancelRetrieve()
	{
	if (iState == ERetrieveData)
		{
		Cancel();
		}
	}

EXPORT_C void CUnifiedCertStore::Remove(const CCTCertInfo& aCertInfo,
										TRequestStatus& aStatus)
	{
	BeginAsyncOp(aStatus, ERemove);
	TRAPD(err, RemoveL(aCertInfo));
	if (err != KErrNone)
		{
		Complete(err);
		}
	}

void CUnifiedCertStore::RemoveL(const CCTCertInfo& aCertInfo)
	{
	FindWritableCertStoreL(aCertInfo.Handle());
	iCurrentWritableCertStore->Remove(aCertInfo, iStatus);	
	SetActive();
	}

EXPORT_C void CUnifiedCertStore::CancelRemove()
	{
	if (iState == ERemove)
		{
		Cancel();
		}
	}

EXPORT_C void CUnifiedCertStore::SetApplicability(const CCTCertInfo& aCertInfo, 
												  const RArray<TUid>& aApplications,
												  TRequestStatus& aStatus)
	{
	BeginAsyncOp(aStatus, ESetApplicability);	
	TRAPD(err, SetApplicabilityL(aCertInfo, aApplications));
	if (err != KErrNone)
		{
		Complete(err);
		}
	}

void CUnifiedCertStore::SetApplicabilityL(const CCTCertInfo& aCertInfo, 
										  const RArray<TUid>& aApplications)
	{
	FindWritableCertStoreL(aCertInfo.Handle());
	
	// Search for duplicates in the array of application
	// complete with KErrArgument if there are any duplicates
	if(aApplications.Count() > 1)
		{
		TInt i=0, j=1;

		for(i=0; i<aApplications.Count()-1; i++ )
			{
			for(j=i+1; j<aApplications.Count(); j++)
				{
				if(aApplications[i] == aApplications[j])
					{
					User::Leave(KErrArgument);
					}
				}		  			
			}
		}

	// Check requested applications actaully exist
	CCertificateAppInfoManager* appInfoManager = CCertificateAppInfoManager::NewLC(iFs, EFalse);
	const RArray<TCertificateAppInfo>& applications = appInfoManager->Applications();

	for (TInt i = 0 ; i < aApplications.Count() ; ++i)
		{
		TInt j = 0;
		for ( ; j < applications.Count() ; ++j)
			{
			if (aApplications[i] == applications[j].Id())
				{
				break;
				}
			}
		if (j == applications.Count())
			{
			User::Leave(KErrArgument);
			}
		}
	CleanupStack::PopAndDestroy(appInfoManager);

	iCurrentWritableCertStore->SetApplicability(aCertInfo, aApplications, iStatus);
	SetActive();
	}

EXPORT_C void CUnifiedCertStore::CancelSetApplicability()
	{
	if (iState == ESetApplicability)
		{
		Cancel();
		}
	}

EXPORT_C void CUnifiedCertStore::SetTrust(const CCTCertInfo& aCertInfo, 
										  TBool aTrusted,
										  TRequestStatus& aStatus)
	{
	BeginAsyncOp(aStatus, ESetTrust);	
	TRAPD(err, SetTrustL(aCertInfo, aTrusted));
	if (err != KErrNone)
		{
		Complete(err);
		}
	}

void CUnifiedCertStore::SetTrustL(const CCTCertInfo& aCertInfo, TBool aTrusted)
	{
	FindWritableCertStoreL(aCertInfo.Handle());
	iCurrentWritableCertStore->SetTrust(aCertInfo, aTrusted, iStatus);
	SetActive();
	}

EXPORT_C void CUnifiedCertStore::CancelSetTrust()
	{
	if (iState == ESetTrust)
		{
		Cancel();
		}
	}

/**
 * Get the certstore containing a given certificate.
 * 
 * Returns the certstore containing the cert referenced in certinfo or NULL if
 * not found.
 */
MCTCertStore* CUnifiedCertStore::GetCertStore(const TCTTokenObjectHandle& aHandle)
	{
	TInt count = iCertStores.Count();
	for (TInt i = 0; i < count; i++)
		{
		MCTCertStore* certstore = iCertStores[i];
		MCTToken& token = certstore->Token();
		if (token.Handle() == aHandle.iTokenHandle)
			{
			return certstore;
			}
		}
	return NULL;
	}

/**
 * Set iCurrentCertStore to the certstore containing a given certificate, or
 * leave if it could not be found.  The handle is the handle of the *certinfo*,
 * *NOT* the token.
 */
void CUnifiedCertStore::FindCertStoreL(const TCTTokenObjectHandle& aHandle)
	{
	assert(!iCurrentCertStore);
	assert(!iCurrentWritableCertStore);
		
	if (!iIsInitialized)
		{
		User::Leave(KErrNotReady);
		}

	iCurrentCertStore = GetCertStore(aHandle);
	
	if (!iCurrentCertStore)
		{
		User::Leave(KErrNotFound);
		}
	}

/**
 * Set iCurrentWritableCertStore to the writable certstore containing a given
 * certificate, or leave if it could not be found.  The handle is the handle of
 * the *certinfo*, *NOT* the token.
 */
void CUnifiedCertStore::FindWritableCertStoreL(const TCTTokenObjectHandle& aHandle)
	{
	assert(!iCurrentCertStore);
	assert(!iCurrentWritableCertStore);
		
	if (!iIsInitialized)
		{
		User::Leave(KErrNotReady);
		}

	if (!iOpenedForWrite)
		{
		User::Leave(KErrAccessDenied);
		}

	iCurrentWritableCertStore = NULL;
	TInt count = iWritableCertStores.Count();
	for (TInt i = 0; i < count; i++)
		{
		MCTWritableCertStore* certstore = iWritableCertStores[i];
		MCTToken& token = certstore->Token();
		if (token.Handle() == aHandle.iTokenHandle)
			{
			iCurrentWritableCertStore = certstore;
			break;
			}
		}

	if (!iCurrentWritableCertStore)
		{
		User::Leave(KErrNotFound);
		}
	}

EXPORT_C TInt CUnifiedCertStore::CertStoreCount() const
	{
	return iCertStores.Count();
	}

EXPORT_C MCTCertStore& CUnifiedCertStore::CertStore(TInt aIndex)
	{
	assert(aIndex < iCertStores.Count());
	return *iCertStores[aIndex];
	}

EXPORT_C TInt CUnifiedCertStore::WritableCertStoreCount() const
	{
	return iWritableCertStores.Count();
	}

EXPORT_C MCTWritableCertStore& CUnifiedCertStore::WritableCertStore(TInt aIndex)
	{
	assert(aIndex < iWritableCertStores.Count());
	return *iWritableCertStores[aIndex];
	}

EXPORT_C TInt CUnifiedCertStore::ReadOnlyCertStoreCount() const
	{
	return iReadOnlyCertStores.Count();
	}

EXPORT_C MCTCertStore& CUnifiedCertStore::ReadOnlyCertStore(TInt aIndex)
	{
	assert(aIndex < iReadOnlyCertStores.Count());
	return *iReadOnlyCertStores[aIndex];
	}

CUnifiedCertStore::CUnifiedCertStore(RFs& aFs, TBool aOpenForWrite)
	: CActive(EPriorityNormal), iFs(aFs), iOpenedForWrite(aOpenForWrite), iOrderAttributes()
	{
	CActiveScheduler::Add(this);
	assert(IsAdded());
	}

void CUnifiedCertStore::ConstructL(RArray<TInt>& aOrderFilter)
	{

 	for (TInt i=0;i<aOrderFilter.Count();i++)
 		{
 		User::LeaveIfError(iOrderAttributes.Append(aOrderFilter[i]));	
 		}
	}

void CUnifiedCertStore::RunL()
	{
	if ((iState != EInitializeGetReadableInterface) &&
		(iState != EInitializeGetReadableInterfaceFinished) &&
		(iState != EInitializeGetToken)) // We don't want to leave if we're in this state
										 // since we want to enumerate all tokens, see below
		{
		User::LeaveIfError(iStatus.Int());
		}

	switch (iState)
		{
		case EInitializeGetTokenList:
			// We need to try to get a list of Tokens for each of the Token Types
			iWorkingVars->iIndex++;
			TInt end;
			if (!iCurrentlyDoingReadOnly)
				{
				end = iWorkingVars->iWritableTokenTypes.Count();
				}
			else
				{
				end = iWorkingVars->iReadOnlyTokenTypes.Count();
				}
			if (iWorkingVars->iIndex < end)
				{
				assert(!iTokenType);
				TInt createRes = KErrNone;
				if (iCurrentlyDoingReadOnly)
					{
					TRAP(createRes, iTokenType = MCTTokenType::NewL(*iWorkingVars->iReadOnlyTokenTypes[iWorkingVars->iIndex], iFs));
					}
				else
					{
					TRAP(createRes, iTokenType = MCTTokenType::NewL(*iWorkingVars->iWritableTokenTypes[iWorkingVars->iIndex], iFs));
					}

				if (KErrNoMemory==createRes)
					{
					// Leave if there's no memory, so OOM tests work
					User::Leave(createRes);
					}
				else if (KErrNone!=createRes)
					{
					//	ECOM couldn't load that token type, don't give up, try the next...
					TRequestStatus* stat = &iStatus;
					User::RequestComplete(stat, KErrNone);
					}
				else
					{
					assert(iTokens.Count() == 0);
					iTokenType->List(iTokens, iStatus);
					iIndexTokens = -1;
					iState = EInitializeGetToken;
					}				   
				}
			else if (!iCurrentlyDoingReadOnly)
				{
				iCurrentlyDoingReadOnly = ETrue;
				iWorkingVars->iIndex = -1;
				TRequestStatus* status = &iStatus;
				User::RequestComplete(status, KErrNone);
				}
			else
				{
				iState = EInitializeFinished;
				TRequestStatus* status = &iStatus;
				User::RequestComplete(status, KErrNone);
				}
			SetActive();
			break;

		case EInitializeGetToken:
			if (iStatus.Int() == KErrHardwareNotAvailable) 
				{
				// If the hardware corresponding to this
				// TokenType has been removed then just skip it
				// but DO NOT leave!
				++iIndexTokens;
				iState = EInitializeGetToken;
				TRequestStatus* status = &iStatus;
				User::RequestComplete(status,KErrNone);
				}
            else 
				{
				User::LeaveIfError(iStatus.Int());

				// iIndexTokens is initialized at EInitializeGetTokenList
				++iIndexTokens;

				//	We need to try to get a certstore interface (readable or
				//	writable) for each of the Tokens in iTokens		
				if (iIndexTokens < iTokens.Count())
					{
                    assert(!iToken);
					iTokenType->OpenToken(*iTokens[iIndexTokens], iToken, iStatus);
					if ((iOpenedForWrite) && !iCurrentlyDoingReadOnly)
						{
						iState = EInitializeGetWritableInterface;
						}
					else
						{
						iState = EInitializeGetReadableInterface;
						}
					}
				else
					{
					// We don't need the iTokenType anymore
					iTokenType->Release();
					iTokenType = 0;
					// We don't need the list of Tokens anymore
					iTokens.Close();
					iState = EInitializeGetTokenList;
					TRequestStatus* status = &iStatus;
					User::RequestComplete(status, KErrNone);
					}
				}
			SetActive();
			break;

		case EInitializeGetWritableInterface:
			{
			User::LeaveIfError(iStatus.Int());
			// First we try to get a writable interface to the store, if
			// that doesn't work we will try to get a readable interface
            assert(iToken);
            assert(!iTokenInterface);
			TUid uid = { KInterfaceWritableCertStore };
			iToken->GetInterface(uid, iTokenInterface, iStatus);
			iState = EInitializeGetReadableInterface;
			SetActive();
			}
			break;

		case EInitializeGetReadableInterface:
			// We check if we managed to get a writable interface
			if (iStatus == KErrNoMemory)
				{
				User::Leave(KErrNoMemory);
				}
            
			if (!iCurrentlyDoingReadOnly && iOpenedForWrite && (iStatus == KErrNone))
				{
				assert(iTokenInterface);

				//	Drop the interface into a "writable checking" object
				CCheckedCertStore* interf = 
					CCheckedCertStore::NewCheckedWritableCertStoreL(iTokenInterface, iPSCertstoreChangeProperty);

                CleanupReleasePushL(*interf);
				iTokenInterface = 0;
				
				User::LeaveIfError(iWritableCertStores.Append(interf));
				CleanupStack::Pop();
				
				User::LeaveIfError(iCertStores.Append(interf));	
				
				// We don't need the Token anymore
				iToken->Release();
				iToken = 0;
				iState = EInitializeGetToken;
				TRequestStatus* status = &iStatus;
				User::RequestComplete(status, KErrNone);
				}
			else 
				{
				// We do the check only if we were not trying to get a Writeable Interface
				// before, if we trying to get a writeable interface before, we know that we
				// have a valid iToken.
				if ((iCurrentlyDoingReadOnly || !iOpenedForWrite) && (iStatus != KErrNone))
					{
					User::Leave(iStatus.Int());
					}
				else
					{
					assert(iToken);
                    assert(!iTokenInterface);
					TUid uid = { KInterfaceCertStore };
					iToken->GetInterface(uid, iTokenInterface, iStatus);
					iState = EInitializeGetReadableInterfaceFinished;
					}
				}
			SetActive();
			break;

		case EInitializeGetReadableInterfaceFinished:
			{
			if (iStatus == KErrNoMemory)
				{
				User::Leave(KErrNoMemory);
				}
            
			if (iStatus == KErrNone)
				{
				assert(iTokenInterface);

				//	Drop the interface into a "read only checking" object
                CCheckedCertStore* interf = 
						CCheckedCertStore::NewCheckedCertStoreL(iTokenInterface, iPSCertstoreChangeProperty);
				
				CleanupReleasePushL(*interf);
				iTokenInterface = 0;

				User::LeaveIfError(iReadOnlyCertStores.Append(interf));
				CleanupStack::Pop(interf);
                
				User::LeaveIfError(iCertStores.Append(interf));
				}			
				
			// We don't need the Token anymore
			iToken->Release();
			iToken = 0;
			
			iStatus = KErrNone;
			iState = EInitializeGetToken;
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, iStatus.Int());
			SetActive();
            }
			break;

		case EInitializeFinished:
            assert(!iTokenType);
            assert(!iToken);
            assert(!iTokenInterface);
			iIsInitialized = ETrue;
			Complete(iStatus.Int());
			break;

		case EList:
			// iIndex has been initialized in List
			iIndex++;
			iCurrentCertStore = NULL;
			if (iIndex < iCertStores.Count())
				{
				iCurrentCertStore = iCertStores[iIndex];
				iCurrentCertStore->List(*iWorkingVars->iCertInfos, *iWorkingVars->iFilter, iStatus);
				iWorkingVars->iCertIndex = 0;
				SetActive();
				}
			else if (iWorkingVars->iIssuerNames.Count() > 0)
				{
				// We have an issuer name. We now remove all certs
				// that don't match that issuer.

				// If this is the first time in here, we need to parse
				// and hash all the issuer names.
				if (iWorkingVars->iParsedIssuerNames.Count() == 0)
					{
					CSHA1* sha1 = CSHA1::NewL();
					CleanupStack::PushL(sha1);
					TInt count = iWorkingVars->iIssuerNames.Count();
					for (TInt i = 0; i < count; i++)
						{
						CX500DistinguishedName* dn = 
							CX500DistinguishedName::NewLC(*iWorkingVars->
														  iIssuerNames[i]);
						User::LeaveIfError(
							iWorkingVars->iParsedIssuerNames.Append(dn));
						CleanupStack::Pop(dn);
						TPtrC8 hash=sha1->Hash(*iWorkingVars->iIssuerNames[i]);
						User::LeaveIfError(
							iWorkingVars->iHashedIssuerNames.Append(
								hash.AllocLC()));
						CleanupStack::Pop();
						}
					CleanupStack::PopAndDestroy();
					}

				while (iWorkingVars->iCertIndex <
					   iWorkingVars->iCertInfos->Count())
					{
					CCTCertInfo* info = 
						(*iWorkingVars->iCertInfos)[iWorkingVars->iCertIndex];
					TCompareResults res = CompareCertInfoDN(info);
					if (res == EYes)
						{
						// It matches. leave it for the next one.
						iWorkingVars->iCertIndex++;
						}
					else if (res == ENo)
						{
						// It doesn't match. Remove it and try the next one.
						info->Release();
						iWorkingVars->iCertInfos->
							Remove(iWorkingVars->iCertIndex);
						}
					else // res == EMaybe
						{
						// Need to load the cert and properly compare the DNs.
						iCurrentCertStore = GetCertStore(info->Handle());
						assert(iCurrentCertStore);
						
						iWorkingVars->iCertDesC=HBufC8::NewMaxL(info->Size());
						iWorkingVars->iCertType = info->CertificateFormat();
						iState = ERetrieveForList;
						iWorkingVars->iCertDes.Set(iWorkingVars->iCertDesC->Des());
						iCurrentCertStore->Retrieve(*info, iWorkingVars->iCertDes, iStatus);
						SetActive();
						return;
						}
					}
				Complete(KErrNone);
				}
			else
				{
				Complete(KErrNone);
				}
			break;

	case ERetrieve:
		{
		switch (iWorkingVars->iCertType)
			{
            case EX509Certificate:
                {
                TPtr8 theCert(iWorkingVars->iCertDesC->Des());
                *(iWorkingVars->iReturnedCert) = CX509Certificate::NewL(theCert);
                }
                break;
            case EWTLSCertificate:
                {
                TPtr8 theCert(iWorkingVars->iCertDesC->Des());
                *(iWorkingVars->iReturnedCert) = CWTLSCertificate::NewL(theCert);			
                }
                break;
            default:
                assert(EFalse);
                break;
			}
		Complete(KErrNone);
		}
		break;

	case ERetrieveForList:
		{
		TPtr8 theCert(iWorkingVars->iCertDesC->Des());
		CX509Certificate* cert=CX509Certificate::NewLC(theCert);
		if (MatchL(cert->IssuerName()))
			{
			// It matches. leave it for the next one.
			iWorkingVars->iCertIndex++;
			}
		else
			{
			// It doesn't match. Remove it and try the next one.
			(*iWorkingVars->iCertInfos)[iWorkingVars->iCertIndex]->Release();
			iWorkingVars->iCertInfos->Remove(iWorkingVars->iCertIndex);
			}
		CleanupStack::PopAndDestroy(cert);
		delete iWorkingVars->iCertDesC;
		iWorkingVars->iCertDesC = 0;
		iState = EList;
		SetActive();
		TRequestStatus* status = & iStatus;
		User::RequestComplete(status, KErrNone);
		break;
		}
		
 	case ERemove:
 	case ESetApplicability:
 	case ESetTrust:
	case EGetCert:
	case EApplications:
	case EIsApplicable:
	case ETrusted:
	case ERetrieveData:
		Complete(KErrNone);
		break;

	default:
        User::Panic(KUCSPanic, 1);
		break;
		}
	}

TInt CUnifiedCertStore::RunError(TInt aError)
	{
	Complete(aError);
	return KErrNone;
	}

void CUnifiedCertStore::DoCancel()
	{
	// If the current state is the last state involved in handling a request, we
	// check to see if we have already been completed - in this case we can
	// simply complete the client with iStatus (this may be KErrNone).  If we
	// have not we cancel the outstanding request and pass the resulting iStatus
	// back to the client - this too may indicate a successful completion if the
	// cancel arrived after the request was executed.
	//
	// For more complex cases, where there are more states to go through before
	// we finish servicing the client request, we cancel any outstanding
	// request, and return KErrCancel to the client.

	switch (iState)
		{
		case EInitializeFinished:
		case ERetrieve:
		case EGetCert:
		case EApplications:
		case EIsApplicable:
		case ETrusted:
		case ERetrieveData:
		case ERemove:
		case ESetApplicability:
		case ESetTrust:		
			if (iStatus == KRequestPending)
				{
				// Attempt to cancel outstanding request and pass status back to
				// client
				CancelOutstandingRequest();
				Complete(iStatus.Int());
				}
			else
				{
				// We've already been completed - call RunL() to process results
				// and complete client
				TRAPD(err, RunL());
				if (err != KErrNone)
					{
					RunError(err);
					}
				}
			break;
			
		default:
			CancelOutstandingRequest();
			Complete(KErrCancel);
			break;
		}
	}

void CUnifiedCertStore::CancelOutstandingRequest()
	{
	switch (iState)
		{
		case EInitializeGetTokenList:
		case EInitializeGetToken:
		case EInitializeGetWritableInterface:
		case EInitializeGetReadableInterface:
		case EInitializeGetReadableInterfaceFinished:
		case EInitializeFinished:
			// Don't have to cancel initialisation stuff - this happens when we
			// release the objects in DestroyTemporaryMembers().
			iStatus = KErrCancel;
			break;

		case EList:
			if (iCurrentCertStore)
				{
				iCurrentCertStore->CancelList();
				}
			break;
			
		case ERetrieve:
		case ERetrieveForList:
		case ERetrieveData:
			assert(iCurrentCertStore);
			iCurrentCertStore->CancelRetrieve();
			break;
			
		case EGetCert:
			assert(iCurrentCertStore);
			iCurrentCertStore->CancelGetCert();
			break;
			
		case EApplications:
			assert(iCurrentCertStore);
			iCurrentCertStore->CancelApplications();
			break;
			
		case EIsApplicable:
			assert(iCurrentCertStore);
			iCurrentCertStore->CancelIsApplicable();
			break;
			
		case ETrusted:
			assert(iCurrentCertStore);
			iCurrentCertStore->CancelTrusted();
			break;
			
		case ERemove:
			assert(iCurrentWritableCertStore);
			iCurrentWritableCertStore->CancelRemove();
			break;
			
		case ESetApplicability:
			assert(iCurrentWritableCertStore);
			iCurrentWritableCertStore->CancelSetApplicability();
			break;
			
		case ESetTrust:
			assert(iCurrentWritableCertStore);
			iCurrentWritableCertStore->CancelSetTrust();
			break;
			
		default:
			User::Panic(KUCSPanic, 1);
			break;
		}
	}

TBool CUnifiedCertStore::MatchL(const CX500DistinguishedName& aName) const
	{
	// Return true if the supplied DN is the same as any of the supplied DNs.
	TInt count = iWorkingVars->iIssuerNames.Count();
	for (TInt i = 0; i < count; i++)
		{
		if (aName.ExactMatchL(*iWorkingVars->iParsedIssuerNames[i]))
			return ETrue;
		}
	return EFalse;
	}

void CUnifiedCertStore::AllocWorkingVarsL()
	{
	assert(!iWorkingVars);
	iWorkingVars = new (ELeave) CUnifiedCertStoreWorkingVars;
	}

void CUnifiedCertStore::BeginAsyncOp(TRequestStatus& aStatus, TState aState)
	{
	assert(iState == EIdle);
	assert(!iClientStatus);
	
	iClientStatus = &aStatus;
	*iClientStatus = KRequestPending;
	iState = aState;
	}

void CUnifiedCertStore::Complete(TInt aError)
	{
	assert(iClientStatus);
	User::RequestComplete(iClientStatus, aError);	
	DestroyTemporaryMembers();
	iState = EIdle;
	}

void CUnifiedCertStore::DestroyTemporaryMembers()
	{
	if (!iIsInitialized)
		{
		TInt end = iReadOnlyCertStores.Count();
		TInt i;
		for (i = 0; i < end; i++)
			{
			iReadOnlyCertStores[i]->Release();
			}
		iReadOnlyCertStores.Close();

		end = iWritableCertStores.Count();
		for (i = 0; i < end; i++)
			{
			iWritableCertStores[i]->Release();
			}
		iWritableCertStores.Close();

		// The elements are already released by the two loops above
		iCertStores.Close();
		}

    if (iTokenType)
		{
		iTokenType->Release();
		iTokenType = 0;
		}

	if (iToken)
		{
		iToken->Release();
		iToken = 0;
		}

	if (iTokenInterface)
		{
		iTokenInterface->Release();
		iTokenInterface = 0;
		}

	iTokens.Close();

	delete iWorkingVars;
	iWorkingVars = 0;

	iCurrentCertStore = NULL;
	iCurrentWritableCertStore = NULL;
	}

CUnifiedCertStore::TCompareResults 
CUnifiedCertStore::CompareCertInfoDN(const CCTCertInfo* aCertInfo)
	{
	if (aCertInfo->IssuerHash() && 
		(aCertInfo->CertificateFormat() == EX509CertificateUrl ||
			iHardwareTypeUids.Find(aCertInfo->Token().TokenType().Type()) != 
		 KErrNotFound))
		{
		TInt count = iWorkingVars->iHashedIssuerNames.Count();
		for (TInt i = 0; i < count; i++)
			{
			if (*aCertInfo->IssuerHash()==*iWorkingVars->iHashedIssuerNames[i])
				return EYes;
			}
		return ENo;
		}
	if (aCertInfo->CertificateFormat() != EX509Certificate)
		return ENo;
	return EMaybe;
	}

EXPORT_C CUnifiedCertStore* CUnifiedCertStore::NewL(RFs& aFs, 
                                                     TBool aOpenForWrite,
                                                     RArray<TInt>& aOrderFilter)
 	{
 	CUnifiedCertStore* self = CUnifiedCertStore::NewLC(aFs, 
 	                                                   aOpenForWrite, 
 	                                                   aOrderFilter);
 	CleanupStack::Pop(self);
 	return self;
 	}

EXPORT_C CUnifiedCertStore* CUnifiedCertStore::NewLC(RFs& aFs, 
                                                      TBool aOpenForWrite,
                                                      RArray<TInt>& aOrderFilter)
 	{
 	CUnifiedCertStore* self = new(ELeave) CUnifiedCertStore(aFs, 
 	                                                        aOpenForWrite);
 	CleanupStack::PushL(self);
 	self->ConstructL(aOrderFilter);
 	return self;
 	}

void CUnifiedCertStore::FilterTokenTypesL(RCPointerArray<CCTTokenTypeInfo>& aSearchTokenTypes,
                                         RCPointerArray<CCTTokenTypeInfo>& aTempTokenTypes,
										 TInt aOrderAttribute)
	{
	//We allow aOrderAttribute=KUnknownHardwareCertStore here to keep DC.
	//assert(aOrderAttribute);
	
	// Get number of token types
	TInt tokenTypesCount = aSearchTokenTypes.Count(); 
	
	// loop through token types
	for(TInt tokenTypesLoop = tokenTypesCount-1; tokenTypesLoop >= 0; tokenTypesLoop--)
		{
		// get the list of attributes supported by this token type.
		// Note: The attribute list consists of values such as
		// KCTSoftware defined in TCTTokenTypeAttribute.h
		const RArray<TCTTokenTypeAttribute>& attributesList = 
			aSearchTokenTypes[tokenTypesLoop]->Attributes();
		
		// Get the number of attributes in the attribute list.
		// The number of attributes will match the ECOM resource
		// file definition. E.g. see 101f5015.rss for the software
		// implementation of certstore. 
		TInt attributeCount = attributesList.Count();
		
		// Check each attribute in the attribute list
		for(TInt attribLoop = 0; attribLoop < attributeCount; attribLoop++)
			{
			// Check whether attribute in the list matches an order attribute
			// E.g. KCTSoftware
			if(attributesList[attribLoop].iUID == KCTSoftware
				&& attributesList[attribLoop].iVal == aOrderAttribute)
				{
				// Found the attribute of interest. Add token type to the temp container. 
				User::LeaveIfError(aTempTokenTypes.Append(aSearchTokenTypes[tokenTypesLoop]));
				// Remove from the Searchlist.
				aSearchTokenTypes.Remove(tokenTypesLoop);
				// No need to examine the other attributes, so break loop
				break;
				}
			}
		}                      
	}
 	
void CUnifiedCertStore::ApplyOrderingL(RCPointerArray<CCTTokenTypeInfo>& aTokenTypes)
	{
	// Number of attributes in ordering filter
	TInt numOrderAttributes=iOrderAttributes.Count();
	assert(numOrderAttributes>0);
	
	// Contains writable tokens types
	RCPointerArray<CCTTokenTypeInfo> tempWritableTokenTypes;
	CleanupClosePushL(tempWritableTokenTypes);
	
	// Contains read-only tokens types
	RCPointerArray<CCTTokenTypeInfo> tempReadOnlyTokenTypes;
	CleanupClosePushL(tempReadOnlyTokenTypes);
		
	// For each order attribute, order the token types
	for(TInt attributeLoop = 0; attributeLoop < numOrderAttributes; attributeLoop++)
		{
		// Get ordering attribute Uid from Order filter
		TInt orderAttribute = iOrderAttributes[attributeLoop];
		
		// Order for writable token types
		FilterTokenTypesL(iWorkingVars->iWritableTokenTypes,
			tempWritableTokenTypes,
			orderAttribute);
		
		// Order for read-only token types
		FilterTokenTypesL(aTokenTypes,
			tempReadOnlyTokenTypes,
			orderAttribute);
		}
		
	// release and close the resources so can refill container, and get rid of 
	// the TokenType which have been filtered out.
	TInt tokenTypesCount = iWorkingVars->iWritableTokenTypes.Count();
	TInt i;	
	for(i = tokenTypesCount-1; i >= 0 ;i--)
		{
		if (iWorkingVars->iWritableTokenTypes[i])
			{
			CCTTokenTypeInfo* ptr=iWorkingVars->iWritableTokenTypes[i];
			iWorkingVars->iWritableTokenTypes.Remove(i);
			delete ptr;			
			}
		}
	iWorkingVars->iWritableTokenTypes.Reset();
	
	// release and close the resources so can refill container, and get rid of 
	// the TokenType which have been filtered out.
	tokenTypesCount = aTokenTypes.Count();
	for(i = tokenTypesCount-1; i >= 0 ;i--)
		{
		if (aTokenTypes[i])
			{
			CCTTokenTypeInfo* ptr=aTokenTypes[i];
			aTokenTypes.Remove(i);
			delete ptr;			
			}
		}
	aTokenTypes.Reset();
	
	// Assign contents of temp token types to containers. 
	// Note: temp tokens types are ordered according to user specification
	tokenTypesCount = tempWritableTokenTypes.Count();
	for(i = 0; i < tokenTypesCount; i++)
		{
		User::LeaveIfError(iWorkingVars->iWritableTokenTypes.Append(tempWritableTokenTypes[i]));
		tempWritableTokenTypes[i] = NULL; 
		}
		
	tokenTypesCount = tempReadOnlyTokenTypes.Count();
	for(i = 0; i < tokenTypesCount; i++)
		{
		User::LeaveIfError(aTokenTypes.Append(tempReadOnlyTokenTypes[i]));
		tempReadOnlyTokenTypes[i] = NULL;
		}
	
	CleanupStack::PopAndDestroy(2);	// tempReadOnlyTokenTypes, tempWritableTokenTypes, 
	}

