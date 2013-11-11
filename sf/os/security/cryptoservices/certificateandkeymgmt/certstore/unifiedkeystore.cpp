/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "unifiedkeystore.h"
#include <ecom/ecom.h>
#include <random.h>
#include <pbedata.h>
#include <asnpkcs.h>
#include "mctcertappinterface.h"
#include <mctkeystoreuids.h>

_LIT(KUnifiedKeyStore, "UnifiedKeyStore");

/////////////////////////////////////////////////////////////////////////////////////
//CUnifiedKeyStore
/////////////////////////////////////////////////////////////////////////////////////

EXPORT_C CUnifiedKeyStore* CUnifiedKeyStore::NewL(RFs& aFs)
	{
	CUnifiedKeyStore* self = CUnifiedKeyStore::NewLC(aFs);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CUnifiedKeyStore* CUnifiedKeyStore::NewLC(RFs& aFs)
	{
	CUnifiedKeyStore* self = new(ELeave) CUnifiedKeyStore(aFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CUnifiedKeyStore::~CUnifiedKeyStore()
	{
	Cancel();
	Cleanup();

	iKeyStoresHolder.ResetAndDestroy();
	iKeyStoresHolder.Close();

	REComSession::FinalClose();
	}

void CUnifiedKeyStore::DoInitializeL()
{//	We want the list of all token types that support a keystore interface
	RArray<TUid> uidArray;
	CleanupClosePushL(uidArray);
	
	User::LeaveIfError(uidArray.Append(TUid::Uid(KInterfaceKeyStore)));

	TCTFindTokenTypesByInterface filter(uidArray.Array());
	CCTTokenTypeInfo::ListL(iTokenTypes, filter);
	
	CleanupStack::PopAndDestroy();	// uidArray
}

EXPORT_C void CUnifiedKeyStore::Initialize(TRequestStatus& aStatus)
{// The following assertion checks that we didn't call Initialize twice
	__ASSERT_DEBUG((iKeyStoresHolder.Count()==0), User::Panic(KUnifiedKeyStore, EUnexpectedInitialise));

	TRAPD(err, DoInitializeL());
	if (err != KErrNone)
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, err);
		return;
		}
	
	iIndexTokenTypes = -1;
	StartAsyncOperation(EInitializeGetTokenList, aStatus);

	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
}

EXPORT_C void CUnifiedKeyStore::CancelInitialize()
	{
	if (iState == EInitializeGetTokenList ||
		iState == EInitializeGetToken ||
		iState == EInitialiseGetKeyManagerInterface ||
		iState == EInitializeGetKeyUserInterface ||
		iState == EInitializeGetKeyUserInterfaceFinished ||
		iState == EInitializeFinished)
		{
		Cancel();
		}
	}

EXPORT_C void CUnifiedKeyStore::CreateKey(TInt aKeyStoreIndex, TKeyUsagePKCS15 aUsage,TUint aSize, 
										  const TDesC& aLabel, CCTKeyInfo::EKeyAlgorithm aAlgorithm, 
										  TInt aAccessType, TTime aStartDate, TTime aEndDate, 
										  CCTKeyInfo*& aKeyInfoOut, TRequestStatus& aStatus)
	{
	StartAsyncOperation(ECreateKey, aStatus);
	TRAPD(err, PrepareToCreateKeyL(aKeyStoreIndex, aUsage, aSize, aLabel, aAlgorithm, aAccessType,
								   aStartDate, aEndDate, aStatus));
	if (KErrNone != err)
		{
		Complete(err);
		return;
		}
	
	iKeyInfoOut = &aKeyInfoOut;
	aKeyInfoOut = NULL;
	iKeyStoreManager->CreateKey(iKeyInfo, iStatus);
	SetActive();
	}

EXPORT_C void CUnifiedKeyStore::CancelCreateKey()
	{
	if (iState == ECreateKey)
		{			
		Cancel();
		}
	}

EXPORT_C void CUnifiedKeyStore::ImportKey(TInt aKeyStoreIndex, const TDesC8& aKeyData,
										  TKeyUsagePKCS15 aUsage, const TDesC& aLabel, 
										  TInt aAccessType, TTime aStartDate, TTime aEndDate, 
										  CCTKeyInfo*& aKeyInfoOut, TRequestStatus& aStatus)
	{
	TBool isEncrypted = TASN1DecPKCS8::IsEncryptedPKCS8Data(aKeyData);
	StartAsyncOperation(isEncrypted ? EImportKeyEncrypted : EImportKey, aStatus);

	ASSERT(!iKeyData);
	iKeyData = aKeyData.Alloc();
	if (!iKeyData)	//	OOM or some other catastrophe
		{
		Complete(KErrNoMemory);
		return;
		}
	
	TRAPD(err, PrepareToCreateKeyL(aKeyStoreIndex, aUsage, 0, aLabel, CCTKeyInfo::EInvalidAlgorithm, aAccessType,
								   aStartDate, aEndDate, aStatus));
	if (KErrNone != err)
		{
		Complete(err);
		return;
		}

	iKeyInfoOut = &aKeyInfoOut;
	aKeyInfoOut = NULL;

	if (isEncrypted)
		{
		iKeyStoreManager->ImportEncryptedKey(*iKeyData, iKeyInfo, iStatus);
		}
	else
		{
		iKeyStoreManager->ImportKey(*iKeyData, iKeyInfo, iStatus);
		}
	SetActive();
	}

EXPORT_C void CUnifiedKeyStore::CancelImportKey()
	{
	if (iState == EImportKey ||
		iState == EImportKeyEncrypted)
		{
		Cancel();
		}
	}

void CUnifiedKeyStore::PrepareToCreateKeyL(	TInt aKeyStoreIndex,
											TKeyUsagePKCS15 aUsage, TUint aSize, 
											const TDesC& aLabel, 
											CCTKeyInfo::EKeyAlgorithm aAlgorithm,
											TInt aAccessType, 
											TTime aStartTime, TTime aEndTime,
											TRequestStatus& aStatus)
	{
	ASSERT(!iKeyStoreManager);
	
	//	These values are filled in by the server when the key is created
	TKeyIdentifier keyID;
	keyID.MaxSize();
	keyID.FillZ(keyID.MaxSize());
	TInt keyHandle = 0;

	// Get the secure ID of the current process
	RProcess thisProcess;
	User::LeaveIfError(thisProcess.Open(thisProcess.Id()));
	TSecureId creatorId = thisProcess.SecureId();
	thisProcess.Close();

	// Default management policy: resict to creating process
	TSecurityPolicy managementPolicy(creatorId);

	// Default use policy: also resict to creating process
	TSecurityPolicy usePolicy(creatorId);

	HBufC* label = aLabel.AllocLC();

	// Panics if keystore manager index invalid
	MCTKeyStoreManager& keystore = KeyStoreManager(aKeyStoreIndex);

	iKeyInfo = CCTKeyInfo::NewL(keyID, aUsage, aSize, NULL, label, keystore.Token(),
								keyHandle, usePolicy, managementPolicy, aAlgorithm,
								aAccessType, ETrue, aStartTime, aEndTime);
	CleanupStack::Pop(label);

	iKeyStoreManager = &keystore;
	iOriginalRequestStatus = &aStatus;
	aStatus = KRequestPending;
	}

//	************************************************************************
//	MKeyStore
//	************************************************************************

void CUnifiedKeyStore::List(RMPointerArray<CCTKeyInfo>& aKeys, 
							const TCTKeyAttributeFilter& aFilter, 
							TRequestStatus& aStatus)
	{
	StartAsyncOperation(EList, aStatus);

	iKeyInfos = &aKeys;

	delete iFilter;
	iFilter = new TCTKeyAttributeFilter(aFilter);
	if (!iFilter)
		{
		Complete(KErrNoMemory);
		return;
		}
	
	iIndex = -1;

	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
}

void CUnifiedKeyStore::CancelList()
	{
	if (iState == EList)
		{
		Cancel();
		}
	}

void CUnifiedKeyStore::GetKeyInfo(TCTTokenObjectHandle aHandle, 
										   CCTKeyInfo*& aKeyInfo,
										   TRequestStatus& aStatus)
	{
	StartAsyncOperation(EGetKeyInfo, aStatus);
	
	ASSERT(!iKeyStore);	
	iKeyStore = FindKeyStore(aHandle);
	if (!iKeyStore)
		{
		Complete(KErrNotFound);
		return;
		}

	iKeyStore->GetKeyInfo(aHandle, aKeyInfo, iStatus);
	SetActive();
	}

void CUnifiedKeyStore::CancelGetKeyInfo()
	{
	if (iState == EGetKeyInfo)
		{
		Cancel();
		}
	}

// Implementation for most of the Open() method
TBool CUnifiedKeyStore::DoOpen(const TCTTokenObjectHandle& aHandle, 
							   TRequestStatus& aStatus)
	{
	StartAsyncOperation(EOpen, aStatus);
	
	ASSERT(!iKeyStore);	
	iKeyStore = FindKeyStore(aHandle);
	if (!iKeyStore)
		{
		Complete(KErrNotFound);
		return EFalse;
		} 

	SetActive();
	return ETrue;
	}

void CUnifiedKeyStore::Open(const TCTTokenObjectHandle& aHandle, 
							MRSASigner*& aSigner,
							TRequestStatus& aStatus)
	{
	if (DoOpen(aHandle, aStatus))
		{
		iKeyStore->Open(aHandle, aSigner, iStatus);
		}
	}

void CUnifiedKeyStore::Open(const TCTTokenObjectHandle& aHandle, 
							MDSASigner*& aSigner, 
							TRequestStatus& aStatus)
	{
	if (DoOpen(aHandle, aStatus))
		{			
		iKeyStore->Open(aHandle, aSigner, iStatus);
		}
	}

void CUnifiedKeyStore::Open(const TCTTokenObjectHandle& aHandle, 
							MCTDecryptor*& aDecryptor,
							TRequestStatus& aStatus)
	{
	if (DoOpen(aHandle, aStatus))
		{
		iKeyStore->Open(aHandle, aDecryptor, iStatus);
		}
	}

void CUnifiedKeyStore::Open(const TCTTokenObjectHandle& aHandle, 
							MCTDH*& aDH, TRequestStatus& aStatus)
	{	
	if (DoOpen(aHandle, aStatus))
		{
		iKeyStore->Open(aHandle, aDH, iStatus);
		}
	}

void CUnifiedKeyStore::CancelOpen()
	{
	if (iState == EOpen)
		{
		Cancel();
		}
	}

/** Returns the public key in DER-encoded ASN-1 */
void CUnifiedKeyStore::ExportPublic(const TCTTokenObjectHandle& aHandle,
									HBufC8*& aPublicKey,
									TRequestStatus& aStatus)
	{
	StartAsyncOperation(EExportPublic, aStatus);

	iKeyStore = FindKeyStore(aHandle);
	if (!iKeyStore)
		{
		Complete(KErrNotFound);
		return;
		}
		
	iKeyStore->ExportPublic(aHandle, aPublicKey, iStatus);
	SetActive();
	}

void CUnifiedKeyStore::CancelExportPublic()
	{
	if (iState == EExportPublic)
		{
		Cancel();
		}
	}

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
void CUnifiedKeyStore::Open(const TCTTokenObjectHandle& aHandle,
                            CryptoSpi::CSigner*& aSigner,
                            TRequestStatus& aStatus)
    {
    if (DoOpen(aHandle, aStatus))
        {
        iKeyStore->Open(aHandle, aSigner, iStatus);
        }
    }

void CUnifiedKeyStore::Open(const TCTTokenObjectHandle& aHandle,
                            CryptoSpi:: CAsymmetricCipher*& asymmetricCipherObj,
                            TRequestStatus& aStatus)
    {
    if (DoOpen(aHandle, aStatus))
        {
        iKeyStore->Open(aHandle, asymmetricCipherObj, iStatus);
        }
    }

void CUnifiedKeyStore::Decrypt(const TCTTokenObjectHandle& aHandle,
                               const TDesC8& aCiphertext,
                               HBufC8*& aPlaintextPtr,
                               TRequestStatus& aStatus)
    {
    if (DoOpen(aHandle, aStatus))
        {
        iKeyStore->Decrypt(aHandle, aCiphertext, aPlaintextPtr, iStatus);
        }
    }

void CUnifiedKeyStore::Sign(const TCTTokenObjectHandle& aHandle,
                            const TDesC8& aPlaintext,
                            CryptoSpi::CCryptoParams*& aSignature,
                            TRequestStatus& aStatus)
    {
    if (DoOpen(aHandle, aStatus))
        {
        iKeyStore->Sign(aHandle, aPlaintext, aSignature, iStatus);
        }
    }
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

//	************************************************************************
//	MKeyStoreManager
//	************************************************************************
	
EXPORT_C void CUnifiedKeyStore::ExportKey(TCTTokenObjectHandle aHandle, 
										  HBufC8*& aKey, TRequestStatus& aStatus)
	{
	StartAsyncOperation(EExportKey, aStatus);

	ASSERT(!iKeyStoreManager);
	iKeyStoreManager = FindKeyStoreManager(aHandle);
	if (!iKeyStoreManager)
		{
		Complete(KErrNotFound);
		return;
		} 

	iKeyStoreManager->ExportKey(aHandle, aKey, iStatus);
	SetActive();
	}

EXPORT_C void CUnifiedKeyStore::CancelExportKey()
	{
	if (iState == EExportKey)
		{
		Cancel();
		}
	} 

EXPORT_C void CUnifiedKeyStore::ExportEncryptedKey(TCTTokenObjectHandle aHandle, 
												   const CPBEncryptParms& aEncryptParams,
												   HBufC8*& aKey, TRequestStatus& aStatus)
	{
	StartAsyncOperation(EExportEncryptedKey, aStatus);
	
	ASSERT(!iKeyStoreManager);	
	iKeyStoreManager = FindKeyStoreManager(aHandle);
	if (!iKeyStoreManager)
		{
		Complete(KErrNotFound);
		return;
		} 
		
	iKeyStoreManager->ExportEncryptedKey(aHandle, aEncryptParams, aKey, iStatus);
	SetActive();
	}

EXPORT_C void CUnifiedKeyStore::CancelExportEncryptedKey()
	{
	if (iState == EExportEncryptedKey)
		{
		Cancel();
		}	
	}

EXPORT_C void CUnifiedKeyStore::DeleteKey(TCTTokenObjectHandle aHandle, 
										  TRequestStatus& aStatus)
	{
	StartAsyncOperation(EDeleteKey, aStatus);

	iKeyStoreManager = FindKeyStoreManager(aHandle);
	if (!iKeyStoreManager)
		{
		Complete(KErrNotFound);
		return;
		}
	
	iKeyStoreManager->DeleteKey(aHandle, iStatus);
	SetActive();
	}

EXPORT_C void CUnifiedKeyStore::CancelDeleteKey()
	{
	if (iState == EDeleteKey)
		{
		Cancel();
		}
	}

EXPORT_C void CUnifiedKeyStore::SetUsePolicy(TCTTokenObjectHandle aHandle, 
											 const TSecurityPolicy& aPolicy,
											 TRequestStatus& aStatus)
	{
	StartAsyncOperation(ESetUsePolicy, aStatus);

	iKeyStoreManager = FindKeyStoreManager(aHandle);
	if (!iKeyStoreManager)
		{
		Complete(KErrNotFound);
		return;
		}
	
	iKeyStoreManager->SetUsePolicy(aHandle, aPolicy, iStatus);
	SetActive();
	}

EXPORT_C void CUnifiedKeyStore::CancelSetUsePolicy()
	{
	if (iState == ESetUsePolicy)
		{
		Cancel();
		}
	}

EXPORT_C void CUnifiedKeyStore::SetManagementPolicy(TCTTokenObjectHandle aHandle, 
													const TSecurityPolicy& aPolicy,
													TRequestStatus& aStatus)
	{
	StartAsyncOperation(ESetManagementPolicy, aStatus);

	iKeyStoreManager = FindKeyStoreManager(aHandle);
	if (!iKeyStoreManager)
		{
		Complete(KErrNotFound);
		return;
		}
	
	iKeyStoreManager->SetManagementPolicy(aHandle, aPolicy, iStatus);
	SetActive();
	}

EXPORT_C void CUnifiedKeyStore::CancelSetManagementPolicy()
	{
	if (iState == ESetManagementPolicy)
		{
		Cancel();
		}
	}

EXPORT_C void CUnifiedKeyStore::SetPassphraseTimeout(TInt aTimeout, 
													 TRequestStatus& aStatus)
	{
	StartAsyncOperation(ESetPassphraseTimeout, aStatus);

	iIndex = -1;
	iNewTimeout = aTimeout;
	SetActive();
	
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}

EXPORT_C void CUnifiedKeyStore::CancelSetPassphraseTimeout()
	{
	if (iState == ESetPassphraseTimeout)
		{
		Cancel();
		}
	}

EXPORT_C void CUnifiedKeyStore::Relock(TRequestStatus& aStatus)
	{
	StartAsyncOperation(ERelock, aStatus);

	iIndex = -1;
	SetActive();
	
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}

EXPORT_C void CUnifiedKeyStore::CancelRelock()
	{
	if (iState == ERelock)
		{
		Cancel();
		}
	}

//	************************************************************************
//	Other exports
//	************************************************************************

EXPORT_C TInt CUnifiedKeyStore::KeyStoreCount() const
{
	return (iKeyStoresHolder.Count());
}

EXPORT_C MCTKeyStore& CUnifiedKeyStore::KeyStore(TInt aIndex)
{
	__ASSERT_ALWAYS(aIndex >= 0 && aIndex < iKeyStoresHolder.Count(),
					User::Panic(KUnifiedKeyStore, EArrayAccessOutOfBounds));
	
	MCTKeyStore* keyStore = static_cast<MCTKeyStore*>(iKeyStoresHolder[aIndex]->KeyStore());
	return (*keyStore);
}

EXPORT_C TInt CUnifiedKeyStore::KeyStoreManagerCount() const
	{
	TInt result = 0;
	for (TInt i = 0 ; i < iKeyStoresHolder.Count() ; ++i)
		{
		if (iKeyStoresHolder[i]->IsKeyManager())
			{
			++result;
			}
		}
	return result;
	}

EXPORT_C MCTKeyStoreManager& CUnifiedKeyStore::KeyStoreManager(TInt aIndex)
	{
	__ASSERT_ALWAYS(aIndex >= 0, User::Panic(KUnifiedKeyStore, EArrayAccessOutOfBounds));
	TInt managerIndex = 0;
	MCTKeyStoreManager* result = NULL;
	for (TInt i = 0 ; i < iKeyStoresHolder.Count() ; ++i)
		{
		if (iKeyStoresHolder[i]->IsKeyManager())
			{
			if (managerIndex == aIndex)
				{
				result = static_cast<MCTKeyStoreManager*>(iKeyStoresHolder[i]->KeyStore());
				break;
				}
			++managerIndex;
			}
		}
	__ASSERT_ALWAYS(result != NULL, User::Panic(KUnifiedKeyStore, EArrayAccessOutOfBounds));
	return *result;
	}

#ifdef SYMBIAN_AUTH_SERVER
	
EXPORT_C void CUnifiedKeyStore::CreateKey(	TInt aKeyStoreIndex, TKeyUsagePKCS15 aUsage,TUint aSize, 
								const TDesC& aLabel, CCTKeyInfo::EKeyAlgorithm aAlgorithm, 
								TInt aAccessType, TTime aStartDate, TTime aEndDate, 
								const TDesC& aAuthenticationString, TInt aFreshness,
								CCTKeyInfo*& aKeyInfoOut, TRequestStatus& aStatus)
		{
		
		StartAsyncOperation(ECreateKey, aStatus);
		TRAPD(err, PrepareToCreateKeyL(aKeyStoreIndex, aUsage, aSize, aLabel, aAlgorithm, aAccessType,
									   aStartDate, aEndDate, aStatus));
		if (KErrNone != err)
			{
			Complete(err);
			return;
			}
		
		iKeyInfoOut = &aKeyInfoOut;
		aKeyInfoOut = NULL;
		iKeyStoreManager->CreateKey(aAuthenticationString, aFreshness, iKeyInfo, iStatus);
		SetActive();
		
		}


EXPORT_C void CUnifiedKeyStore::ImportKey(	TInt aKeyStoreIndex, const TDesC8& aKeyData,
								TKeyUsagePKCS15 aUsage, const TDesC& aLabel, 
								TInt aAccessType, TTime aStartDate, TTime aEndDate, 
								const TDesC& aAuthenticationString, TInt aFreshness,
								CCTKeyInfo*& aKeyInfoOut, TRequestStatus& aStatus)
		{
		TBool isEncrypted = TASN1DecPKCS8::IsEncryptedPKCS8Data(aKeyData);
		StartAsyncOperation(isEncrypted ? EImportKeyEncrypted : EImportKey, aStatus);

		ASSERT(!iKeyData);
		iKeyData = aKeyData.Alloc();
		if (!iKeyData)	//	OOM or some other catastrophe
			{
			Complete(KErrNoMemory);
			return;
			}
		
		TRAPD(err, PrepareToCreateKeyL(aKeyStoreIndex, aUsage, 0, aLabel, CCTKeyInfo::EInvalidAlgorithm, aAccessType,
									   aStartDate, aEndDate, aStatus));
		if (KErrNone != err)
			{
			Complete(err);
			return;
			}

		iKeyInfoOut = &aKeyInfoOut;
		aKeyInfoOut = NULL;

		if (isEncrypted)
			{
			iKeyStoreManager->ImportEncryptedKey(*iKeyData, aAuthenticationString, aFreshness, iKeyInfo, iStatus);
			}
		else
			{
			iKeyStoreManager->ImportKey(*iKeyData, aAuthenticationString, aFreshness, iKeyInfo, iStatus);
			}
		SetActive();
		}

EXPORT_C void CUnifiedKeyStore::SetAuthenticationPolicy(	const TCTTokenObjectHandle aHandle, 
															const TDesC& aAuthenticationString,
															TInt aFreshness,					
															TRequestStatus& aStatus)
	{
	StartAsyncOperation(ESetAuthenticationPolicy, aStatus);
		
	ASSERT(!iKeyStoreManager);	
	iKeyStoreManager = FindKeyStoreManager(aHandle);
	if (!iKeyStoreManager)
		{
		Complete(KErrNotFound);
		return;
		} 
		
	iKeyStoreManager->SetAuthenticationPolicy(aHandle, aAuthenticationString, aFreshness, iStatus);
	SetActive();
		
	}

EXPORT_C void CUnifiedKeyStore::GetAuthenticationPolicy(	const TCTTokenObjectHandle aHandle, 
															HBufC*& aAuthenticationString,
															TInt& aFreshness,					
															TRequestStatus& aStatus)
	{
	StartAsyncOperation(EGetAuthenticationPolicy, aStatus);
		
	ASSERT(!iKeyStoreManager);	
	iKeyStoreManager = FindKeyStoreManager(aHandle);
	if (!iKeyStoreManager)
		{
		Complete(KErrNotFound);
		return;
		} 
		
	iKeyStoreManager->GetAuthenticationPolicy(aHandle, aAuthenticationString, aFreshness, iStatus);
	SetActive();
		
	}

#endif // SYMBIAN_AUTH_SERVER

CUnifiedKeyStore::CUnifiedKeyStore(RFs& aFs)
	:	CActive(EPriorityNormal), iFs(aFs), iState(EIdle)
{//	Currently defaults to always try for key store manager interface
//	This may change (add parameter to NewL for required interface)
	iRequestUid.iUid = KInterfaceKeyStoreManager;
	CActiveScheduler::Add(this);
}

void CUnifiedKeyStore::ConstructL()
{}

void CUnifiedKeyStore::StartAsyncOperation(TState aState, TRequestStatus& aStatus)
	{
	ASSERT(iState == EIdle);
	ASSERT(iOriginalRequestStatus == NULL);
	iState = aState;
	iOriginalRequestStatus = &aStatus;
	aStatus = KRequestPending;
	}

MCTKeyStore* CUnifiedKeyStore::FindKeyStore(const TCTTokenObjectHandle& aHandle)
	{
	for (TInt index = 0 ; index < iKeyStoresHolder.Count() ; ++index)
		{
		MCTTokenInterface* store = iKeyStoresHolder[index]->KeyStore();
		ASSERT(store);
		if (store->Token().Handle() == aHandle.iTokenHandle)
			{
			return static_cast<MCTKeyStoreManager*>(store);
			}
		}
	return NULL;
	}
	
MCTKeyStoreManager* CUnifiedKeyStore::FindKeyStoreManager(const TCTTokenObjectHandle& aHandle)
	{
	for (TInt index = 0 ; index < iKeyStoresHolder.Count() ; ++index)
		{
		MCTTokenInterface* store = iKeyStoresHolder[index]->KeyStore();
		ASSERT(store);
		if (store->Token().Handle() == aHandle.iTokenHandle && iKeyStoresHolder[index]->IsKeyManager())
			{
			return static_cast<MCTKeyStoreManager*>(store);
			}
		}
	return NULL;
	}
	
void CUnifiedKeyStore::RunL()
{
	if (EInitializeGetKeyUserInterfaceFinished != iState &&
		EInitializeGetKeyUserInterface != iState && 
		EInitializeGetToken != iState)
	{
		User::LeaveIfError(iStatus.Int());
	}

	switch (iState)
	{
		case EInitializeGetTokenList:
		{//	Try to get a list of Tokens for each of the Token Types
			iIndexTokenTypes++;
			if (iIndexTokenTypes < iTokenTypes.Count())
			{
				__ASSERT_DEBUG(!iTokenType, User::Panic(KUnifiedKeyStore, EArrayAccessOutOfBounds));
				iTokenType = MCTTokenType::NewL(*iTokenTypes[iIndexTokenTypes], iFs);
				__ASSERT_DEBUG(iTokens.Count()==0, User::Panic(KUnifiedKeyStore, ETokensArrayAlreadyInUse));
				iTokenType->List(iTokens, iStatus);
				iIndexTokens = -1;
				iState = EInitializeGetToken;
			}
			else
			{// We don't need the list of Token Types anymore
				iTokenTypes.ResetAndDestroy();
				iTokenTypes.Close();
				iState = EInitializeFinished;
				TRequestStatus* status = &iStatus;
				User::RequestComplete(status, KErrNone);
			}
			SetActive();
			break;
		}
		case EInitializeGetToken:
		{
			if (iStatus.Int() == KErrHardwareNotAvailable)
				{
				// If the hardware corresponding to this
				// TokenType has been removed then just skip it
				// but DO NOT leave!
				++iIndexTokens;
				iState = EInitializeGetToken;
				TRequestStatus* status = &iStatus;
				User::RequestComplete(status, KErrNone);
				}
            else
				{
				User::LeaveIfError(iStatus.Int());
    			iIndexTokens++;		
		
				if (iIndexTokens < iTokens.Count())
					{
					iTokenType->OpenToken(*iTokens[iIndexTokens], iToken, iStatus);
					iRequestUid.iUid = KInterfaceKeyStoreManager;
					iState = EInitialiseGetKeyManagerInterface;
					}
				else
					{// Don't need the iTokenType anymore
					iTokenType->Release();
					iTokenType = 0;

					iTokens.Close();	// Don't need the list of Tokens anymore
					iState = EInitializeGetTokenList;
					TRequestStatus* status = &iStatus;
					User::RequestComplete(status, KErrNone);
					}
				}
			SetActive();
			break;
		}
		case EInitialiseGetKeyManagerInterface:
		{// First try to get a manager interface to the store, if
		//	unsuccessful, try once to get a user interface
			if (iToken)
			{
				iRequestUid.iUid = KInterfaceKeyStoreManager;			
				iToken->GetInterface(iRequestUid, iTokenInterface, iStatus);
				iState = EInitializeGetKeyUserInterface;
				SetActive();			
			}
			else
			{//	No token
				User::Leave(KErrNotReady);
			}
			break;
		}
		case EInitializeGetKeyUserInterface:
		{//	Did we get a manager interface?
			if (iStatus==KErrNoMemory)
			{
				User::Leave(KErrNoMemory);
			}

			if (iRequestUid.iUid==KInterfaceKeyStoreManager)
			{
				if (KErrNone==iStatus.Int())
				{//	Success! Store it and finish up
					CKeyStoreIF* keyStore = new (ELeave) CKeyStoreIF(iTokenInterface, ETrue);
					CleanupStack::PushL(keyStore);
					User::LeaveIfError(iKeyStoresHolder.Append(keyStore));
					CleanupStack::Pop(keyStore);

					iTokenInterface = 0;
					iToken->Release();
					iToken = 0;
					iState = EInitializeGetToken;
					TRequestStatus* status = &iStatus;
					User::RequestComplete(status, KErrNone);
				}
				else
				{//	No luck getting a manager, so try getting a user
					iRequestUid.iUid = KInterfaceKeyStore;			
					iToken->GetInterface(iRequestUid, iTokenInterface, iStatus);
					iState = EInitializeGetKeyUserInterfaceFinished;					
				}
			}
			else if (iRequestUid.iUid==KInterfaceKeyStore) 
			{//	We were trying for user IF								
				if (iStatus==KErrNone)
				{
					if (iToken)
					{
						iRequestUid.iUid = KInterfaceKeyStore;			
						iToken->GetInterface(iRequestUid, iTokenInterface, iStatus);
						iState = EInitializeGetKeyUserInterfaceFinished;	
					}
					else
					{
						User::Leave(KErrNotReady);
					}
				}
				else
				{//	Couldn't even get a user IF
					User::Leave(iStatus.Int());
				}				
			}

			SetActive();
			break;
		}
		case EInitializeGetKeyUserInterfaceFinished:
		{
			if (iStatus==KErrNone)
			{
				CKeyStoreIF* keyStore = new (ELeave) CKeyStoreIF(iTokenInterface, EFalse);
				CleanupStack::PushL(keyStore);
				User::LeaveIfError(iKeyStoresHolder.Append(keyStore));
				CleanupStack::Pop(keyStore);

				iTokenInterface = 0;
				iToken->Release();
				iToken = 0;
				iState = EInitializeGetToken;
				TRequestStatus* status = &iStatus;
				User::RequestComplete(status, KErrNone);
			}
			else if (iStatus == KErrNoMemory)
			{
				User::Leave(KErrNoMemory);
			}
			else
			{
				iState = EInitializeGetToken;
				TRequestStatus* status = &iStatus;
				User::RequestComplete(status,iStatus.Int());
			}
			
			SetActive();
			break;
		}
		case EInitializeFinished:
			Complete(KErrNone);
			break;
			
		case EList:
		{//	iIndex has been initialized in List function
			++iIndex;
			if (iIndex < iKeyStoresHolder.Count())
				{
				iKeyStore = static_cast<MCTKeyStore*>(iKeyStoresHolder[iIndex]->KeyStore());
				ASSERT(iKeyStore);
				iKeyStore->List(*iKeyInfos, *iFilter, iStatus);
				SetActive();
				}
			else
				{
				Complete(KErrNone);
				}
			break;
		}
		
	    case EGetKeyInfo:
			Complete(KErrNone);
			break;
			
		case ECreateKey:
			*iKeyInfoOut = iKeyInfo;
			iKeyInfo = NULL; // Release ownership
			Complete(KErrNone);
			break;
			
		case EImportKey:
		case EImportKeyEncrypted:
			*iKeyInfoOut = iKeyInfo;
			iKeyInfo = NULL; // Release ownership
			Complete(KErrNone);
			break;
			
		case EExportKey:
		case EExportEncryptedKey:
			Complete(KErrNone);
			break;		

	    case ERelock:
			++iIndex;
			
			// Find next key store manager
			while (iIndex < iKeyStoresHolder.Count() && !iKeyStoresHolder[iIndex]->IsKeyManager())
				++iIndex;
			
			if (iIndex < iKeyStoresHolder.Count())
				{
				iKeyStoreManager = static_cast<MCTKeyStoreManager*>(iKeyStoresHolder[iIndex]->KeyStore());
				ASSERT(iKeyStoreManager);
				iKeyStoreManager->Relock(iStatus);
				SetActive();
				}
			else
				{
				Complete(KErrNone);
				}
			break;

	    case ESetPassphraseTimeout:
			++iIndex;
			
			// Find next key store manager
			while (iIndex < iKeyStoresHolder.Count() && !iKeyStoresHolder[iIndex]->IsKeyManager())
				++iIndex;
			
			if (iIndex < iKeyStoresHolder.Count())
				{
				iKeyStoreManager = static_cast<MCTKeyStoreManager*>(iKeyStoresHolder[iIndex]->KeyStore());
				ASSERT(iKeyStoreManager);
				iKeyStoreManager->SetPassphraseTimeout(iNewTimeout, iStatus);
				SetActive();
				}
			else
				{
				Complete(KErrNone);
				}
			break;

	    case EOpen:
		case EExportPublic:
	    case EDeleteKey:
	    case ESetUsePolicy:
	    case ESetManagementPolicy:
	    case EGetAuthenticationPolicy:
	    case ESetAuthenticationPolicy:
			Complete(KErrNone);
			break;
		default:
			User::Panic(KUnifiedKeyStore, EUnrecognisedState);
			break;
	}
}

TInt CUnifiedKeyStore::RunError(TInt aError)
	{
	Complete(aError); 
	return KErrNone;
	}

void CUnifiedKeyStore::DoCancel()
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
		case EGetKeyInfo:
		case ECreateKey:
		case EImportKey:
		case EImportKeyEncrypted:
		case EExportKey:
		case EExportEncryptedKey:
	    case EOpen:
		case EExportPublic:
	    case EDeleteKey:
	    case ESetUsePolicy:
	    case ESetManagementPolicy:
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

void CUnifiedKeyStore::CancelOutstandingRequest()
	{
	switch (iState)
		{
		case EInitializeGetTokenList:
		case EInitializeGetToken:
		case EInitialiseGetKeyManagerInterface:
		case EInitializeGetKeyUserInterface:
		case EInitializeGetKeyUserInterfaceFinished:
		case EInitializeFinished:
			// Don't have to cancel initialisation stuff - this happens when we
			// release the objects in Cleanup().
			iStatus = KErrCancel;
			break;

		case EList:
			if (iKeyStore)
				{
				iKeyStore->CancelList();
				}
			break;

		case EGetKeyInfo:
			ASSERT(iKeyStore);
			iKeyStore->CancelGetKeyInfo();
			break;

		case EOpen:
			ASSERT(iKeyStore);
			iKeyStore->CancelOpen();
			break;

		case EExportPublic:
			ASSERT(iKeyStore);
			iKeyStore->CancelExportPublic();
			break;			

		case ECreateKey:
			ASSERT(iKeyStoreManager);
			iKeyStoreManager->CancelCreateKey();
			break;

		case EImportKey:
		case EImportKeyEncrypted:
			ASSERT(iKeyStoreManager);
			iKeyStoreManager->CancelImportKey();
			break;
			
		case EExportKey:
		case EExportEncryptedKey:
			ASSERT(iKeyStoreManager);
			iKeyStoreManager->CancelExportKey();
			break;
			
		case EDeleteKey:
			ASSERT(iKeyStoreManager);
			iKeyStoreManager->CancelDeleteKey();
			break;

		case ERelock:
			ASSERT(iKeyStoreManager);
			iKeyStoreManager->CancelRelock();
			break;

		case ESetPassphraseTimeout:
			ASSERT(iKeyStoreManager);
			iKeyStoreManager->CancelSetPassphraseTimeout();
			break;
		
	    case ESetUsePolicy:
			ASSERT(iKeyStoreManager);
			iKeyStoreManager->CancelSetUsePolicy();
			break;

	    case ESetManagementPolicy:
			ASSERT(iKeyStoreManager);
			iKeyStoreManager->CancelSetManagementPolicy();
			break;

		default:
			User::Panic(KUnifiedKeyStore, EUnrecognisedState);
			break;
		}
	}


void CUnifiedKeyStore::Complete(TInt aError)
	{
	Cleanup();
	if (iOriginalRequestStatus)
		{
		User::RequestComplete(iOriginalRequestStatus, aError);
		}
	}

void CUnifiedKeyStore::Cleanup()
	{
	// If we have a key info, we want to release it
	if (iKeyInfo)
	{
		iKeyInfo->Release();
		iKeyInfo = NULL;
	}

	delete iKeyData;
	iKeyData = NULL;

	delete iFilter;
	iFilter = NULL;

	delete iPbeParams;
	iPbeParams = NULL;

	iTokenTypes.Close();

	if (iTokenType)
		{
		iTokenType->Release();
		iTokenType = 0;
		}

	iTokens.Close();

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
	
	iKeyInfoOut = NULL;
	iKeyStore = NULL;
	iKeyStoreManager = NULL;
	
	iState = EIdle;
	}

CUnifiedKeyStore::CKeyStoreIF::CKeyStoreIF(MCTTokenInterface* aKeyStore, TBool aIsKeyManager)
:	iKeyStore(aKeyStore), iIsKeyManager(aIsKeyManager)
{}

CUnifiedKeyStore::CKeyStoreIF::~CKeyStoreIF()
{
	if (iKeyStore)
	{
		iKeyStore->Release();
		iKeyStore = NULL;
	}
}

