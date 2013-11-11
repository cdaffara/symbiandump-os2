/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "CKeyDataManager.h"
#include "fsdatatypes.h"
#include "fstokencliserv.h"
#include "fstokenutil.h"
#include "keystorepassphrase.h"

_LIT(KKeyStoreFilename,"keys.dat");

#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
#include <e32property.h>
#include <authserver/aspubsubdefs.h>
#else
const TInt KDefaultPassphraseTimeout = 30;
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

//	*********************************************************************
//	Key store data manager - maintains array of objects representing keys
//	*********************************************************************

/*static*/ CFileKeyDataManager* CFileKeyDataManager::NewL()
	{
	CFileKeyDataManager* self = new (ELeave) CFileKeyDataManager();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CFileKeyDataManager::~CFileKeyDataManager()
	{
	if (iFileStore)
		{
		CompactStore();
		delete iFileStore;
		}

	iFile.Close(); // May already have been closed by store
	iFs.Close();
		
	iKeys.ResetAndDestroy();
	iKeys.Close();
	#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	iIdentityId.Close();
	#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	}

CFileKeyDataManager::CFileKeyDataManager() :
	iRootStreamId(KNullStreamId),
	iInfoStreamId(KNullStreamId)
#ifndef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	,iPassStreamId(KNullStreamId),
	iTimeoutStreamId(KNullStreamId)
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	{
	}

void CFileKeyDataManager::ConstructL()
	{
	
	User::LeaveIfError(iFs.Connect());
	OpenStoreL();

	RStoreReadStream lookupStream;
	lookupStream.OpenLC(*iFileStore, iInfoStreamId);

	#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	User::LeaveIfError(iIdentityId.Attach(	AuthServer::KAuthServerSecureId,
											AuthServer::KUidAuthServerAuthChangeEvent));
	#else
	iPassStreamId = (TStreamId) lookupStream.ReadUint32L();
	iTimeoutStreamId = (TStreamId) lookupStream.ReadUint32L();
	#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

	TInt count = lookupStream.ReadInt32L();
	for (TInt index = 0; index < count; index++)
		{
		CFileKeyData* keyData = CFileKeyData::NewL(lookupStream);
		CleanupStack::PushL(keyData);

		if (keyData->Handle() > iKeyIdentifier)
			iKeyIdentifier = keyData->Handle();

		iKeys.AppendL(keyData);
		CleanupStack::Pop(keyData);
		}
	
	CleanupStack::PopAndDestroy(&lookupStream);

#ifndef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	ReadPassphraseTimeoutL();
#endif //SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
	}

CPassphraseManager* CFileKeyDataManager::CreatePassphraseManagerLC()
	{
	CPassphraseManager* result = CPassphraseManager::NewL(*iFileStore);
	CleanupStack::PushL(result);
	return result;
	}

void CFileKeyDataManager::OpenStoreL()
	{
	//	Tries to locate a key store file on the default drive and then from ROM
	//	If it cannot find one, tries to create a file with permanent file store
	//	inside it In all cases, should initialise iFileStore unless it cannot
	//	create the file/store/streams
	
	__ASSERT_DEBUG(!iFileStore, PanicServer(EPanicStoreInitialised));

	TFileName fullPath;
	FileUtils::MakePrivateFilenameL(iFs, KKeyStoreFilename, fullPath);

	FileUtils::EnsurePathL(iFs, fullPath);
	TRAPD(result, OpenStoreInFileL(fullPath));

	if (result == KErrInUse) 
		{		
		// Cannot access the file now. Abort server startup rather than wiping the keystore.
		User::Leave(result); 
		}

	if (result != KErrNone)
		{		
		// Not yet opened a valid store, either no file to be found, or no valid
		// store in it. Copy the original one stored in the ROM.
		delete iFileStore;
		iFileStore = NULL;
		
		TFileName romPath;
		FileUtils::MakePrivateROMFilenameL(iFs, KKeyStoreFilename, romPath);

		if (result != KErrNotFound)
			{
			// Wipe the keystore if we can't open it (it's corrupt anyway)
			User::LeaveIfError(iFs.Delete(fullPath));
			}

		// Copy data from rom and open it	
		TRAPD(err,
			  FileUtils::CopyL(iFs, romPath, fullPath);
			  OpenStoreInFileL(fullPath)
			);
		
		if (KErrNone != err)
			{
			// We tried to copy the keystore from ROM. For some reason this
			// failed and we still cannot open the file. Create a new one from
			// scratch.
			CreateStoreInFileL(fullPath);
			}
		}

	__ASSERT_DEBUG(iFileStore, PanicServer(EPanicStoreInitialised));
	__ASSERT_DEBUG((KNullStreamId!=iRootStreamId), PanicServer(EPanicRootStreamNotReady));
	__ASSERT_DEBUG((KNullStreamId!=iInfoStreamId), PanicServer(EPanicManagerStreamNotReady));
	}

void CFileKeyDataManager::CreateStoreInFileL(const TDesC& aFile)
	{
	TInt r = iFs.MkDirAll(aFile);
	if ( (r!=KErrNone) && (r!=KErrAlreadyExists) )
		User::Leave(r);

	iFileStore = CPermanentFileStore::ReplaceL(iFs, aFile, EFileRead | EFileWrite | EFileShareExclusive);
	iFileStore->SetTypeL(KPermanentFileStoreLayoutUid);

	TCleanupItem cleanupStore(RevertStore, iFileStore);
	CleanupStack::PushL(cleanupStore);
	
#ifndef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	// Create timeout stream with default timeout
	RStoreWriteStream timeoutStream;
	iTimeoutStreamId = timeoutStream.CreateLC(*iFileStore);
	timeoutStream.WriteUint32L(KDefaultPassphraseTimeout);
	timeoutStream.CommitL();
	CleanupStack::PopAndDestroy(&timeoutStream);
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
	// Create info stream - Currently no passphrase created, and no keys
	RStoreWriteStream infoStream;
	iInfoStreamId = infoStream.CreateLC(*iFileStore);
	
#ifndef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	infoStream.WriteUint32L(KNullStreamId.Value());
	infoStream.WriteUint32L(iTimeoutStreamId.Value());
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
	infoStream.WriteUint32L(0); // Write key count of zero
	infoStream.CommitL();
	CleanupStack::PopAndDestroy(&infoStream);

	// Create root stream - just contains id of info stream
	RStoreWriteStream rootStream;
	iRootStreamId = rootStream.CreateLC(*iFileStore);
	iFileStore->SetRootL(iRootStreamId);

	rootStream.WriteUint32L(iInfoStreamId.Value());		
	rootStream.CommitL();
	CleanupStack::PopAndDestroy(&rootStream);

	WriteKeysToStoreL();
	
	iFileStore->CommitL();
	CleanupStack::Pop(); // cleanupStore
	}

void CFileKeyDataManager::OpenStoreInFileL(const TDesC& aFile)
	{
	// Make sure the file isn't write protected
	User::LeaveIfError(iFs.SetAtt(aFile, 0, KEntryAttReadOnly));
	
	User::LeaveIfError(iFile.Open(iFs, aFile, EFileRead | EFileWrite | EFileShareExclusive));
	
	iFileStore = CPermanentFileStore::FromL(iFile);		

	// Get the salt, root and manager TStreamIds
	iRootStreamId = iFileStore->Root();
	if (iRootStreamId == KNullStreamId)
		{
		User::Leave(KErrCorrupt);
		}
	
	RStoreReadStream rootStream;
	rootStream.OpenLC(*iFileStore, iRootStreamId);
	iInfoStreamId = (TStreamId)(rootStream.ReadUint32L());
	CleanupStack::PopAndDestroy(&rootStream);
	}

// Methods dealing with atomic updates to key data file ////////////////////////

// This is a cleanup item that reverts the store
void CFileKeyDataManager::RevertStore(TAny* aStore)
{
	CPermanentFileStore* store = reinterpret_cast<CPermanentFileStore*>(aStore);
	TRAP_IGNORE(store->RevertL());
	// We're ignoring the leave code from this becuase there's no way we can
	// handle this sensibly.  This shouldn't be a problem in practice - this
	// will leave if for example the file store is on removable which is
	// unexpectedly remove, and this is never the case for us.
}

// Rewrites the info stream (ie the array of key data info) to the store
void CFileKeyDataManager::WriteKeysToStoreL()
	{
	RStoreWriteStream lookupStream;
	lookupStream.ReplaceLC(*iFileStore, iInfoStreamId);

	#ifndef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
		lookupStream.WriteUint32L(iPassStreamId.Value());
		lookupStream.WriteUint32L(iTimeoutStreamId.Value());
	#endif //SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
	TInt keyCount = iKeys.Count();
	lookupStream.WriteInt32L(keyCount);

	for (TInt index = 0; index < keyCount; index++)
		{
		const CFileKeyData* key = iKeys[index];
		key->ExternalizeL(lookupStream);
		}

	lookupStream.CommitL();
	CleanupStack::PopAndDestroy(&lookupStream);
	}

/**
 * Add a key to the store.  Assumes that the key data streams (info, public key
 * and private key) have already been written.
 */
void CFileKeyDataManager::AddL(const CFileKeyData* aKeyData)
	{
	ASSERT(aKeyData);
#ifndef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
		ASSERT(aKeyData->PassphraseStreamId() != KNullStreamId);
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

	// Add the key to to the array, rewrite the infostream and 
	// ONLY THEN commit the store
	User::LeaveIfError(iKeys.Append(aKeyData));

#ifndef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	TStreamId oldDefaultPassphraseId;

	// Set the default passphrase id if this is the first key
	oldDefaultPassphraseId = iPassStreamId;
	if (iKeys.Count() == 1)
		{
		iPassStreamId = aKeyData->PassphraseStreamId();
		}
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
	TRAPD(err,UpdateStoreL());
	
	if (err != KErrNone)
		{
		iKeys.Remove(iKeys.Count() - 1);
		#ifndef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
			iPassStreamId = oldDefaultPassphraseId;
		#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
		User::Leave(err);
		}
	}

void CFileKeyDataManager::UpdateStoreL()
	{
	WriteKeysToStoreL();

	// Release ownership of key data and reset default passphrase id if store
	// can't be written
	TCleanupItem cleanupStore(RevertStore, iFileStore);
	CleanupStack::PushL(cleanupStore);

	iFileStore->CommitL();
	
	CleanupStack::Pop(); // cleanupStore
	}

/**
 * "Transaction safe" key removal - only removes the key in memory and file if
 * all operations are successful.
 */
void CFileKeyDataManager::RemoveL(TInt aObjectId)
	{
	TInt index;
	const CFileKeyData* key = NULL;
	for (index = 0 ; index < iKeys.Count() ; ++index)
		{
		if (iKeys[index]->Handle() == aObjectId)
			{
			key = iKeys[index];
			break;
			}
		}
	
	if (!key)
		{
		User::Leave(KErrNotFound);
		}

	TCleanupItem cleanupStore(RevertStore, iFileStore);
	CleanupStack::PushL(cleanupStore);	

	iFileStore->DeleteL(key->PrivateDataStreamId());
	iFileStore->DeleteL(key->PublicDataStreamId());
	iFileStore->DeleteL(key->InfoDataStreamId());

#ifndef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	// Remove the passphrase if it's the last key
	TStreamId oldPassphraseId = iPassStreamId;
	if (Count() == 1)
		{
		iFileStore->DeleteL(iPassStreamId);
		iPassStreamId = KNullStreamId;
		}
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	
	// Remove the key
	iKeys.Remove(index);
	
	TRAPD(res, WriteKeysToStoreL());

	if (res != KErrNone)
		{
#ifndef SYMBIAN_KEYSTORE_USE_AUTH_SERVER		
		iPassStreamId = oldPassphraseId;
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
		User::LeaveIfError(iKeys.Append(key)); // Put it back, shouldn't leave
		User::Leave(res);
		}
	else 
		{
		delete key;   // Cannot leave from the point it's removed to here, so no cleanup stack!
		}		
	iFileStore->CommitL();

	CleanupStack::Pop(); // cleanupStore
	CompactStore();
}

#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
TBool CFileKeyDataManager::IsKeyAlreadyInStore(const TDesC& aKeyLabel, AuthServer::TIdentityId aIdentity) const
	{//	Check each key in the store to determine if aKeyLabel already exists
	TInt keyCount = iKeys.Count();
	TBool isInStore = EFalse;
	for (TInt index = 0; index < keyCount; ++index)
		{
		const TDesC& keyLabel = iKeys[index]->Label();
		if (keyLabel.Compare(aKeyLabel)==0 && (iKeys[index]->Identity() == aIdentity))
			{
			isInStore = ETrue;
			break;
			}
		}
	return (isInStore);
	}

#else
TBool CFileKeyDataManager::IsKeyAlreadyInStore(const TDesC& aKeyLabel) const
{//	Check each key in the store to determine if aKeyLabel already exists
	TInt keyCount = iKeys.Count();
	TBool isInStore = EFalse;
	for (TInt index = 0; index < keyCount; index++)
	{
		const TDesC& keyLabel = iKeys[index]->Label();
		if (keyLabel.Compare(aKeyLabel)==0)
		{
			isInStore = ETrue;
			break;
		}
	}

	return (isInStore);
}

#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

TInt CFileKeyDataManager::Count() const
	{
	return iKeys.Count();
	}	

const CFileKeyData* CFileKeyDataManager::operator[](TInt aIndex) const
	{
	return iKeys[aIndex];
	}

const CFileKeyData* CFileKeyDataManager::Lookup(TInt aObjectId) const
	{
	TInt count = Count();
	for (TInt i = 0; i < count; ++i)
		{
		if ((*this)[i]->Handle() == aObjectId)
			{
			return (*this)[i];
			}
		}
	return NULL;
	}

//	*********************************************************************
//	Management of file and store therein
//	*********************************************************************

#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER

const CFileKeyData* CFileKeyDataManager::CreateKeyDataLC(const TDesC& aLabel, AuthServer::TIdentityId aIdentityId)
	{
	TInt objectId = ++iKeyIdentifier;
	TStreamId infoData = CreateWriteStreamL();
	TStreamId publicKeyData = CreateWriteStreamL();
	TStreamId privateKeyData = CreateWriteStreamL();
	return CFileKeyData::NewLC(objectId, aLabel, infoData, publicKeyData, privateKeyData, aIdentityId);
	}

#else
const CFileKeyData* CFileKeyDataManager::CreateKeyDataLC(const TDesC& aLabel, TStreamId aPassStreamId)
	{
	ASSERT(aPassStreamId != KNullStreamId);
	TInt objectId = ++iKeyIdentifier;
	TStreamId infoData = CreateWriteStreamL();
	TStreamId publicKeyData = CreateWriteStreamL();
	TStreamId privateKeyData = CreateWriteStreamL();
	return CFileKeyData::NewLC(objectId, aLabel, infoData, aPassStreamId, publicKeyData, privateKeyData);
	}

#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

//	Creates a new write stream in the store (which it then closes)
//	Returns the TStreamId associated with it
TStreamId CFileKeyDataManager::CreateWriteStreamL()
	{
	__ASSERT_DEBUG(iFileStore, PanicServer(EPanicStoreInitialised));
	if (!iFileStore)
		User::Leave(KErrNotReady);

	RStoreWriteStream newStream;
	TStreamId result = newStream.CreateLC(*iFileStore);
	if (KNullStreamId == result)
		User::Leave(KErrBadHandle);

	newStream.CommitL();
	CleanupStack::PopAndDestroy(&newStream);

	return result;
	}

CKeyInfo* CFileKeyDataManager::ReadKeyInfoLC(const CFileKeyData& aKeyData) const
	{
	__ASSERT_ALWAYS(iFileStore, PanicServer(EPanicStoreInitialised));
	RStoreReadStream stream;
	stream.OpenLC(*iFileStore, aKeyData.InfoDataStreamId());
	CKeyInfo* info = CKeyInfo::NewL(stream);
	
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	ReadAuthDetailsL(stream, *info);
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	CleanupStack::PopAndDestroy(&stream);
	info->CleanupPushL();
	if (info->Handle() != aKeyData.Handle())
		{
		User::Leave(KErrCorrupt); // is this appropriate?
		}
	return info;
	}

void CFileKeyDataManager::WriteKeyInfoL(const CFileKeyData& aKeyData, const CKeyInfo& aKeyInfo)
	{
	RStoreWriteStream infoStream;
	OpenInfoDataStreamLC(aKeyData, infoStream);
	infoStream << aKeyInfo;
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	WriteAuthDetailsL(infoStream, aKeyInfo);
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	infoStream.CommitL();
	CleanupStack::PopAndDestroy(&infoStream);
	}

#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
void CFileKeyDataManager::WriteAuthDetailsL( RStoreWriteStream& aInfoStream, const CKeyInfo& aKeyInfo )
	{
	aInfoStream.WriteInt32L(aKeyInfo.Identity());
	aInfoStream << aKeyInfo.AuthExpression();
  	aInfoStream.WriteInt32L(aKeyInfo.Freshness());
	}

void CFileKeyDataManager::ReadAuthDetailsL( RStoreReadStream& aInfoStream, CKeyInfo& aKeyInfo ) const
	{
	aKeyInfo.SetIdentity(aInfoStream.ReadInt32L());
	HBufC* expression = HBufC::NewLC(aInfoStream, KMaxTInt);
	aKeyInfo.SetAuthExpressionL(*expression);
	aKeyInfo.SetFreshness(aInfoStream.ReadInt32L());
	CleanupStack::PopAndDestroy(expression);
	}
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

void CFileKeyDataManager::SafeWriteKeyInfoL(const CFileKeyData& aKeyData, const CKeyInfo& aKeyInfo)
	{
	TCleanupItem cleanupStore(RevertStore, iFileStore);
	CleanupStack::PushL(cleanupStore);	

	WriteKeyInfoL(aKeyData, aKeyInfo);
	iFileStore->CommitL();
	
	CleanupStack::Pop(); // cleanupStore	
	}

void CFileKeyDataManager::OpenInfoDataStreamLC(const CFileKeyData& aKeyData, RStoreWriteStream& aStream)
	{
	__ASSERT_ALWAYS(iFileStore, PanicServer(EPanicStoreInitialised));
	aStream.ReplaceLC(*iFileStore, aKeyData.InfoDataStreamId());
	}

void CFileKeyDataManager::OpenPublicDataStreamLC(const CFileKeyData& aKeyData, RStoreWriteStream& aStream)
	{
	__ASSERT_ALWAYS(iFileStore, PanicServer(EPanicStoreInitialised));
	aStream.ReplaceLC(*iFileStore, aKeyData.PublicDataStreamId());
	}

void CFileKeyDataManager::OpenPublicDataStreamLC(const CFileKeyData& aKeyData, RStoreReadStream& aStream) const
	{
	__ASSERT_ALWAYS(iFileStore, PanicServer(EPanicStoreInitialised));
	aStream.OpenLC(*iFileStore, aKeyData.PublicDataStreamId());
	}

#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER

void CFileKeyDataManager::OpenPrivateDataStreamLC(const CFileKeyData& aKeyData, RStoreWriteStream& aStream) 
	{
	__ASSERT_DEBUG(iFileStore, PanicServer(EPanicStoreInitialised));
	aStream.ReplaceLC(*iFileStore, aKeyData.PrivateDataStreamId());
	}

void CFileKeyDataManager::OpenPrivateDataStreamLC(const CFileKeyData& aKeyData, RStoreReadStream& aStream) const
	{
	__ASSERT_DEBUG(iFileStore, PanicServer(EPanicStoreInitialised));
	aStream.OpenLC(*iFileStore, aKeyData.PrivateDataStreamId());
	}

#else

void CFileKeyDataManager::OpenPrivateDataStreamLC(const CFileKeyData& aKeyData, CPassphrase& aPassphrase,
												  RStoreReadStream& aStream) 
	{
	__ASSERT_DEBUG(iFileStore, PanicServer(EPanicStoreInitialised));
	aStream.OpenLC(aPassphrase.Store(), aKeyData.PrivateDataStreamId());
	}

void CFileKeyDataManager::OpenPrivateDataStreamLC(const CFileKeyData& aKeyData, CPassphrase& aPassphrase,
												  RStoreWriteStream& aStream)
	{
	__ASSERT_DEBUG(iFileStore, PanicServer(EPanicStoreInitialised));
	aStream.ReplaceLC(aPassphrase.Store(), aKeyData.PrivateDataStreamId());
	}


#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

#ifndef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
TInt CFileKeyDataManager::GetPassphraseTimeout() const
	{
	return iTimeout;
	}

void CFileKeyDataManager::SetPassphraseTimeoutL(TInt aTimeout)
	{
	TInt oldTimeout = iTimeout;

	iTimeout = aTimeout;
	TRAPD(err, WritePassphraseTimeoutL(); iFileStore->CommitL());

	if (err != KErrNone)
		{
		iTimeout = oldTimeout;
		iFileStore->RevertL(); // shouldn't leave
		User::Leave(err);
		}
	}

void CFileKeyDataManager::ReadPassphraseTimeoutL()
	{
	ASSERT(iTimeout == 0); // Only called from ConstructL()
	
	RStoreReadStream stream;
	stream.OpenLC(*iFileStore, iTimeoutStreamId);
	iTimeout = stream.ReadInt32L();
	CleanupStack::PopAndDestroy(&stream);
	}

void CFileKeyDataManager::WritePassphraseTimeoutL()
	{
	RStoreWriteStream stream;
	stream.ReplaceLC(*iFileStore, iTimeoutStreamId);
	stream.WriteUint32L(iTimeout);	
	stream.CommitL();
	CleanupStack::PopAndDestroy(&stream);
	}

TStreamId CFileKeyDataManager::DefaultPassphraseId() const
	{
	ASSERT((iPassStreamId == KNullStreamId) == (Count() == 0));
	return iPassStreamId;
	}

#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

/**
 * Attempt to compact the store - it doesn't matter if these calls leave, it
 * will only mean that the store takes up more space than necessary.
 */
void CFileKeyDataManager::CompactStore()
	{
	ASSERT(iFileStore);
	TRAP_IGNORE(iFileStore->ReclaimL(); iFileStore->CompactL());
	}

#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
TUint32 CFileKeyDataManager::CachedIdentity()
	{
	TInt value = 0;
	iIdentityId.Get(value);
	return value;
	}
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

// CFileKeyData ////////////////////////////////////////////////////////////////

#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
CFileKeyData* CFileKeyData::NewLC(	TInt aObjectId, const TDesC& aLabel, TStreamId aInfoData,
								  TStreamId aPublicData, TStreamId aPrivateData,
								  AuthServer::TIdentityId aIdentityId)
	{
	CFileKeyData* self = new (ELeave) CFileKeyData(aObjectId, aInfoData, aPublicData, aPrivateData, aIdentityId);
	CleanupStack::PushL(self);
	self->ConstructL(aLabel);
	return self;
	}
#else
CFileKeyData* CFileKeyData::NewLC(TInt aObjectId, const TDesC& aLabel, TStreamId aInfoData,
								  TStreamId aPassphraseId, TStreamId aPublicData, TStreamId aPrivateData)
	{
	CFileKeyData* self = new (ELeave) CFileKeyData(aObjectId, aInfoData, aPassphraseId, aPublicData, aPrivateData);
	CleanupStack::PushL(self);
	self->ConstructL(aLabel);
	return self;
	}

#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

CFileKeyData* CFileKeyData::NewL(RStoreReadStream& aReadStream)
	{
	CFileKeyData* self = new (ELeave) CFileKeyData();
	CleanupStack::PushL(self);
	self->InternalizeL(aReadStream);
	CleanupStack::Pop(self);
	return (self);
	}

CFileKeyData::~CFileKeyData()
	{
	delete iLabel;
	}

#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
CFileKeyData::CFileKeyData(TInt aObjectId, TStreamId aInfoData,  
						   TStreamId aPublicData, TStreamId aPrivateData,
						   AuthServer::TIdentityId aIdentityId) :
	iObjectId(aObjectId), iInfoData(aInfoData), 
	iPublicKeyData(aPublicData), iPrivateKeyData(aPrivateData),
	iIdentityId(aIdentityId)
	{
	ASSERT(iObjectId);
	ASSERT(iInfoData != KNullStreamId);
	ASSERT(iPublicKeyData != KNullStreamId);
	ASSERT(iPrivateKeyData != KNullStreamId);
	ASSERT(iIdentityId);
	}
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
#ifdef KEYTOOL
CFileKeyData::CFileKeyData(TInt aObjectId, TStreamId aInfoData,  
						   TStreamId aPublicData, TStreamId aPrivateData,
						   AuthServer::TIdentityId aIdentityId) :
	iObjectId(aObjectId), iInfoData(aInfoData), 
	iPublicKeyData(aPublicData), iPrivateKeyData(aPrivateData),
	iIdentityId(aIdentityId)
	{
	ASSERT(iObjectId);
	ASSERT(iInfoData != KNullStreamId);
	ASSERT(iPublicKeyData != KNullStreamId);
	ASSERT(iPrivateKeyData != KNullStreamId);
	ASSERT(iIdentityId);
	}
#endif // KEYTOOL

CFileKeyData::CFileKeyData(TInt aObjectId, TStreamId aInfoData, TStreamId aPassphraseId, 
						   TStreamId aPublicData, TStreamId aPrivateData) :
	iObjectId(aObjectId), iInfoData(aInfoData), iPassphraseId(aPassphraseId),
	iPublicKeyData(aPublicData), iPrivateKeyData(aPrivateData)
	{
	ASSERT(iObjectId);
	ASSERT(iInfoData != KNullStreamId);
	ASSERT(iPassphraseId != KNullStreamId);
	ASSERT(iPublicKeyData != KNullStreamId);
	ASSERT(iPrivateKeyData != KNullStreamId);
	}

CFileKeyData::CFileKeyData()
	{
	}

void CFileKeyData::ConstructL(const TDesC& aLabel)
	{
	TInt labelLen = aLabel.Length();
	iLabel = HBufC::NewMaxL(labelLen);
	TPtr theLabel(iLabel->Des());
	theLabel.FillZ();
	theLabel.Copy(aLabel);
	}

void CFileKeyData::InternalizeL(RReadStream& aReadStream)
{
	iObjectId = aReadStream.ReadInt32L();
	iInfoData.InternalizeL(aReadStream);
#ifndef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	iPassphraseId.InternalizeL(aReadStream);
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	iPublicKeyData.InternalizeL(aReadStream);
	iPrivateKeyData.InternalizeL(aReadStream);
	
	TInt labelLen = aReadStream.ReadInt32L();
	iLabel = HBufC::NewMaxL(labelLen);
	TPtr theLabel((TUint16*)iLabel->Ptr(), labelLen, labelLen);
	theLabel.FillZ(labelLen);
	aReadStream.ReadL(theLabel);
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	iIdentityId = aReadStream.ReadInt32L();
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
}

void CFileKeyData::ExternalizeL(RWriteStream& aWriteStream) const
{
	aWriteStream.WriteInt32L(iObjectId);
	iInfoData.ExternalizeL(aWriteStream);
#ifndef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	iPassphraseId.ExternalizeL(aWriteStream);
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	iPublicKeyData.ExternalizeL(aWriteStream);
	iPrivateKeyData.ExternalizeL(aWriteStream);

	TInt labelLen = iLabel->Length();
	aWriteStream.WriteInt32L(labelLen);
	TPtr theLabel(iLabel->Des());
	theLabel.SetLength(labelLen);
	aWriteStream.WriteL(theLabel);
#ifdef SYMBIAN_KEYSTORE_USE_AUTH_SERVER
	aWriteStream.WriteInt32L(iIdentityId);
#endif // SYMBIAN_KEYSTORE_USE_AUTH_SERVER

}

#ifdef KEYTOOL

CFileKeyData* CFileKeyData::CreateOldKeyL(RStoreReadStream& aReadStream)
	{
	CFileKeyData* self = new (ELeave) CFileKeyData();
	CleanupStack::PushL(self);
	self->InternalizeOldKeyL(aReadStream);
	CleanupStack::Pop(self);
	return (self);
	}

void CFileKeyData::InternalizeOldKeyL(RReadStream& aReadStream)
	{
	iObjectId = aReadStream.ReadInt32L();
	iInfoData.InternalizeL(aReadStream);
	iPassphraseId.InternalizeL(aReadStream);
	iPublicKeyData.InternalizeL(aReadStream);
	iPrivateKeyData.InternalizeL(aReadStream);
	
	TInt labelLen = aReadStream.ReadInt32L();
	iLabel = HBufC::NewMaxL(labelLen);
	TPtr theLabel((TUint16*)iLabel->Ptr(), labelLen, labelLen);
	theLabel.FillZ(labelLen);
	aReadStream.ReadL(theLabel);
	}

void CFileKeyData::ExternalizeWithAuthL(RWriteStream& aWriteStream) 
{
	aWriteStream.WriteInt32L(iObjectId);
	iInfoData.ExternalizeL(aWriteStream);
	iPublicKeyData.ExternalizeL(aWriteStream);
	iPrivateKeyData.ExternalizeL(aWriteStream);

	TInt labelLen = iLabel->Length();
	aWriteStream.WriteInt32L(labelLen);
	TPtr theLabel(iLabel->Des());
	theLabel.SetLength(labelLen);
	aWriteStream.WriteL(theLabel);
	aWriteStream.WriteInt32L(iIdentityId);
}

#endif // KEYTOOL
