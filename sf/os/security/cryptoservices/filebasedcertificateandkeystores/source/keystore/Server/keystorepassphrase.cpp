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


#include "keystorepassphrase.h"
#include "FSDialog.h"
#include "pbedata.h"
#include "fstokenutil.h"
#include <mctauthobject.h>
#include <securityerr.h>

const int KRetryCount = 3;

// TPasswordManager ////////////////////////////////////////////////////////////

/*static*/ void TPasswordManager::GetPassword(FSResources::TStrings aLabel,
											  TPINValue& aValue, 
											  TRequestStatus& aStatus,
											  TBool aRetry)
	{
	TPINParams params;

	params.iPINLabel = FSResources::Strings()[aLabel];
	params.iMinLength = 0;
	params.iMaxLength = KMaxPINLength;

	FSDialog::Dialog()->EnterPIN(params, aRetry, aValue, aStatus);
	}

/*static*/ void TPasswordManager::KeystorePassword(TPINValue& aValue, 
												   TRequestStatus& aStatus,
												   TBool aRetry)
	{
	GetPassword(FSResources::EPinGLabel, aValue, aStatus, aRetry);
	}
	
/*static*/ void TPasswordManager::CreatePassword(TPINValue& aValue, 
												 TRequestStatus& aStatus)
	{
	GetPassword(FSResources::ECreatePinGLabel, aValue, aStatus);
	}
	
/*static*/ void TPasswordManager::ImportPassword(TPINValue& aValue, 
												 TRequestStatus& aStatus)
	{
	GetPassword(FSResources::EImportPassphraseLabel, aValue, aStatus);
	}
	
/*static*/ void TPasswordManager::ExportPassword(TPINValue& aValue, 
												 TRequestStatus& aStatus)
	{
	GetPassword(FSResources::EExportPassphraseLabel, aValue, aStatus);
	}

/*static*/ void TPasswordManager::Cancel()
	{
	FSDialog::Dialog()->Cancel();
	}


// CPassphrase /////////////////////////////////////////////////////////////////

CPassphrase* CPassphrase::NewLC(TInt aTimeout, CStreamStore& aStore,
								TStreamId aStreamId, const TDesC& aPassphrase)
	{
	CPassphrase* self = new (ELeave) CPassphrase(aStore, aTimeout);
	CleanupStack::PushL(self);
	self->ConstructL(aStreamId, aPassphrase);
	return self;
	}

CPassphrase* CPassphrase::NewL(TInt aTimeout, CStreamStore& aStore,
								TStreamId aStreamId, const TDesC& aPassphrase)
	{
	CPassphrase* self = NewLC(aTimeout, aStore, aStreamId, aPassphrase);
	CleanupStack::Pop(self);
	return self;
	}

CPassphrase* CPassphrase::NewLC(TInt aTimeout, CStreamStore& aStore,
								const TDesC& aPassphrase)
	{
	CPassphrase* self = new (ELeave) CPassphrase(aStore, aTimeout);
	CleanupStack::PushL(self);
	self->ConstructL(aPassphrase);
	return self;
	}

CPassphrase* CPassphrase::NewL(TInt aTimeout, CStreamStore& aStore,
								const TDesC& aPassphrase)
	{
	CPassphrase* self = NewLC(aTimeout, aStore, aPassphrase);
	CleanupStack::Pop(self);
	return self;
	}

CPassphrase::CPassphrase(CStreamStore& aStore, TInt aTimeout)
	: iStore(aStore)
	{
	iExpires = (aTimeout != KTimeoutNever);
	if (iExpires)
		{
		iExpiryTime.UniversalTime();
		iExpiryTime += TTimeIntervalSeconds(aTimeout);
		}
	}

void CPassphrase::ConstructL(TStreamId aStreamId, const TDesC& aPassphrase)
	{
	iStreamId = aStreamId;

	RStoreReadStream stream;
	stream.OpenLC(iStore, iStreamId);
	HBufC8* encryptedMasterKey = HBufC8::NewLC(stream, 10000); //some large number
	CPBEncryptionData* encryptionData = CPBEncryptionData::NewLC(stream);
	iSecurity = CPBEncryptSet::NewL(*encryptionData, *encryptedMasterKey, aPassphrase);	
	CleanupStack::PopAndDestroy(3); // encryptionData, encryptedMasterKey, stream
	iSecureStore = CSecureStore::NewL(iStore, *iSecurity);
	}

void CPassphrase::ConstructL(const TDesC& aPassphrase)
	{
	TPBPassword password(aPassphrase);
	iSecurity = CPBEncryptSet::NewL(password, ECipherAES_CBC_256);

	RStoreWriteStream stream;
	iStreamId = stream.CreateLC(iStore);
	stream << iSecurity->EncryptedMasterKey();
	iSecurity->EncryptionData().ExternalizeL(stream);
	stream.CommitL();
	CleanupStack::PopAndDestroy(); // stream
	
	iSecureStore = CSecureStore::NewL(iStore, *iSecurity);
	}

CPassphrase::~CPassphrase()
	{
	delete iSecurity;
	delete iSecureStore;
	}

TInt CPassphrase::TimeRemainingL(const TTime& aCurrentTime) const
	{
	if (iExpires)
		{
		TTimeIntervalSeconds seconds;
		User::LeaveIfError(iExpiryTime.SecondsFrom(aCurrentTime, seconds));
		TInt result = seconds.Int();
		if (result < 0)
			{
			result = 0;
			}
		return result;
		}
	else
		{
		return KTimeoutNever;
		}
	}

void CPassphrase::RevertStore(TAny* aStore)
	{
	//	This is a CleanupItem
	CStreamStore* store = reinterpret_cast<CStreamStore*>(aStore);
	TRAP_IGNORE(store->RevertL());
	}

void CPassphrase::ChangePassphraseL(const TDesC& aNewPassphrase)
	{
	iSecurity->ChangePasswordL(aNewPassphrase);

	TCleanupItem cleanupStore(RevertStore, &iStore);
	CleanupStack::PushL(cleanupStore);

	RStoreWriteStream stream;
	stream.ReplaceLC(iStore, iStreamId);
	stream << iSecurity->EncryptedMasterKey();
	iSecurity->EncryptionData().ExternalizeL(stream);
	stream.CommitL();
	CleanupStack::PopAndDestroy(); // stream
	iStore.CommitL();
	CleanupStack::Pop(); // cleanupStore
	}

CStreamStore& CPassphrase::Store() const
	{
	return *iSecureStore;
	}

TStreamId CPassphrase::StreamId() const
	{
	return iStreamId;
	}

// CPassphraseManager //////////////////////////////////////////////////////////

CPassphraseManager* CPassphraseManager::NewL(CStreamStore& aStore)
	{
	return new (ELeave) CPassphraseManager(aStore);
	}

CPassphraseManager::CPassphraseManager(CStreamStore& aStore)
		: CActive(EPriorityNormal), iStore(aStore)
	{
	CActiveScheduler::Add(this);
	}

CPassphraseManager::~CPassphraseManager()
	{
	Cancel();
	iCache.ResetAndDestroy();
	}

/** Return a cached passphrase if present, otherwise return NULL. */
CPassphrase* CPassphraseManager::GetCachedPassphrase(TStreamId aStreamId)
	{
	for (TInt i = 0 ; i < iCache.Count() ; ++i)
		{
		CPassphrase* p = iCache[i];
		if (p->StreamId() == aStreamId)
			{
			return p;
			}
		}
	
	return NULL;
	}

void CPassphraseManager::GetPassphrase(TStreamId aStreamId,
									   TInt aTimeout,
									   CPassphrase*& aPassphrase, 
									   TRequestStatus& aStatus)
	{
	ASSERT(iTimeout >= KTimeoutNever);
	CPassphrase* p = GetCachedPassphrase(aStreamId);
	if (p)
		{
		aPassphrase = p;
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrNone);
		}
	else
		{
        iRetries = KRetryCount;
		iClientStatus = &aStatus;
		aStatus = KRequestPending;	
		iNewPassphraseStream = aStreamId;
		iTimeout = aTimeout;
		iNewPassphrasePtr = &aPassphrase;
		iState = EGetPassphrase;
		TPasswordManager::KeystorePassword(iNewPassphrase, iStatus);
		SetActive();
		}
	}

void CPassphraseManager::CreatePassphrase(TInt aTimeout,
										  CPassphrase*& aPassphrase, 
										  TRequestStatus& aStatus)
	{
	ASSERT(iTimeout >= -1);
	iClientStatus = &aStatus;
	aStatus = KRequestPending;
	iTimeout = aTimeout;
	iNewPassphrasePtr = &aPassphrase;
	iState = ECreatePassphrase;
	TPasswordManager::CreatePassword(iNewPassphrase, iStatus);
	SetActive();
	}

void CPassphraseManager::ChangePassphrase(TStreamId aStreamId, TRequestStatus& aStatus)
	{	
	iRetries = KRetryCount;
	iClientStatus = &aStatus;
	aStatus = KRequestPending;
	iNewPassphraseStream = aStreamId;
	iState = EChangePassphrase;

	TPINParams params;
	params.iPINLabel = 
		FSResources::Strings()[FSResources::EPinGLabel];
	params.iMinLength = 0;
	params.iMaxLength = KMaxPINLength;
		
	FSDialog::Dialog()->ChangePIN(params, EFalse, iOldPassphrase, iNewPassphrase, iStatus);
	SetActive();
	}

void CPassphraseManager::RunL()
	{
	User::LeaveIfError(iStatus.Int());

	switch (iState)
		{
		case EGetPassphrase:
            {
            TRAPD(err, HandleGetPassphraseL());
            if (err == KErrBadPassphrase && --iRetries > 0)
                {
                TPasswordManager::KeystorePassword(iNewPassphrase, iStatus, ETrue);
                SetActive();
                }
            else
                {
                User::LeaveIfError(err);
                }
            }
			break;
            
		case ECreatePassphrase:
            {
			CPassphrase* phrase = CPassphrase::NewLC(iTimeout, iStore, 
													 iNewPassphrase);
			*iNewPassphrasePtr = phrase;
			User::LeaveIfError(iCache.Append(phrase));
			CleanupStack::Pop(phrase);
            Complete(KErrNone);
            }
			break;
            
		case EChangePassphrase:
            {
			CPassphrase* phrase = NULL;
			TRAPD(err, phrase = CPassphrase::NewL(	0, 
													iStore, 
													iNewPassphraseStream,
													iOldPassphrase));
			
			if(KErrBadPassphrase == err && --iRetries > 0)
				{
				TPINParams params;
				params.iPINLabel = FSResources::Strings()[FSResources::EPinGLabel];
				params.iMinLength = 0;
				params.iMaxLength = KMaxPINLength;
			
				FSDialog::Dialog()->ChangePIN(params, ETrue, iOldPassphrase, iNewPassphrase, iStatus);
				SetActive();
				break;
				} 

			if(KErrNone != err)
				{
				User::Leave(err);
				}
			
			CleanupStack::PushL(phrase);

			phrase->ChangePassphraseL(iNewPassphrase);
			RemoveCachedPassphrases(phrase->StreamId());
			CleanupStack::PopAndDestroy(phrase);
            Complete(KErrNone);
            }
			break;
            
		default:
			PanicServer(EPanicInvalidState);
		}

	}

void CPassphraseManager::HandleGetPassphraseL()
    {
    CPassphrase* phrase = CPassphrase::NewLC(iTimeout, iStore, 
                                             iNewPassphraseStream,
                                             iNewPassphrase);
    *iNewPassphrasePtr = phrase;
    User::LeaveIfError(iCache.Append(phrase));
    CleanupStack::Pop(phrase);
    Complete(KErrNone);
    }

TInt CPassphraseManager::RunError(TInt aError)
	{
    Complete(aError);
	return KErrNone;
	}

void CPassphraseManager::DoCancel()
	{
	TPasswordManager::Cancel();
	RunError(KErrCancel);
	}

void CPassphraseManager::Complete(TInt aError)
    {
	iNewPassphrasePtr = NULL;
	iState = EIdle;
	if (iClientStatus)
		{
		User::RequestComplete(iClientStatus, aError);
		}
    }

void CPassphraseManager::ExpireCacheL()
	{
	TTime timeNow;
	timeNow.UniversalTime();
	
	for (TInt i = iCache.Count() - 1; i >= 0; --i)
		{
		CPassphrase* passphrase = iCache[i];
		if (passphrase->TimeRemainingL(timeNow) == 0)
			{
			iCache.Remove(i);
			delete passphrase;
			}
		}
	}

void CPassphraseManager::RemoveCachedPassphrases(TStreamId aStreamId)
	{
	for (TInt i = iCache.Count() - 1; i >= 0; --i)
		{
		CPassphrase* passphrase = iCache[i];
		if (aStreamId == KNullStreamId || passphrase->StreamId() == aStreamId)
			{
			iCache.Remove(i);
			delete passphrase;
			}
		}
	}

TInt CPassphraseManager::TimeRemainingL(TStreamId aStreamId)
	{
	CPassphrase* passphrase = GetCachedPassphrase(aStreamId);
	if (passphrase)
		{
		TTime timeNow;
		timeNow.UniversalTime();
		return passphrase->TimeRemainingL(timeNow);
		}
	else
		{
		return 0;
		}
	}
