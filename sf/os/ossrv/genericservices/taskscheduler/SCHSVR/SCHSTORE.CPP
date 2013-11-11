// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

// User includes
#include "SCHSTORE.H"
#include "SchLogger.h"
#include "SCHEDULE.H"
#include "SCHCLI.H"
#include "SCHEXEC.H"

#define UNUSED_VAR(a) a = a

// Constants
const TInt KMaxChangesBeforeCompact = 5;
//


//
// -----> SchBackupManagerUtils (header)
//

void SchBackupManagerUtils::Panic(TSchStorePanic aPanic)
	{
	_LIT(KSchStorePanic, "SchStore");
	User::Panic(KSchStorePanic, aPanic);
	}

//
// -----> CSchBackupManager (header)
//

CSchBackupManager::CSchBackupManager(RFs& aFsSession)
:	CActive(EPriorityIdle), iFsSession(aFsSession)
	{
	// construct backup filename
	iBackupFileName.Copy(KSchsvrBackupFileName);
	iBackupFileName[0] = 'A' + static_cast<TInt>(RFs::GetSystemDrive()); 

	CActiveScheduler::Add(this);
	}

CSchBackupManager::~CSchBackupManager()
	{
	// This will delete the store and close the compactor
	Cancel();
	//
	delete iScheduleIndex;
	delete iClientIndex;
	}

void CSchBackupManager::ConstructL()
	{
	iScheduleIndex = CSchScheduleIndex::NewL();
	iClientIndex = CSchClientIndex::NewL(iFsSession);
	}

/**
If this is called during a restore, we want to make sure we want to append
to existing structure not always create new object and just append to the queue
for example we want to append task to existing client if they have the same name
and priority not create another one.
*/
void CSchBackupManager::RestoreL(TPriQue<CClientProxy>& aClients, 
								TSglQue<CSchedule>& aTimeSchedules,
								CSchLogManager& aSchLogManager,TBool aBUR)
	{
	LOGSTRING("CSchBackupManager::RestoreL");

	// Open store
	CPermanentFileStore* store = OpenOrCreateBackupStoreLC();

	// Restore root stream (two pointers to other streams)
	RStoreReadStream stream;
	stream.OpenLC(*store, store->Root());
	stream >> iIndexStreamSchedules;
	stream >> iIndexStreamClients;
	CleanupStack::PopAndDestroy(); // stream

	// Restore clients
	iClientIndex->RestoreClientsL(aClients, *store, iIndexStreamClients, aSchLogManager,aBUR);

	// Restore schedules
	iScheduleIndex->RestoreSchedulesL(aTimeSchedules, *store, iIndexStreamSchedules);

	// Cleanup store.
	CleanupStack::PopAndDestroy(store);
	}

void CSchBackupManager::PerformStoreOperationL(TSchBackupOperation aAction, const CSchedule& aSchedule)
//
//	Perform an schedule-related operation.
//
	{
	LOGSTRING3("CSchBackupManager::PerformStoreOperationL - Schedule: %S (%d)", &aSchedule.Name(), aSchedule.Id());
	
	if(aSchedule.Persists())
		{
		// Cancel any compaction that may be going on
		Cancel();

		// Perform the operation
		CStreamStore* store = OpenOrCreateBackupStoreLC();
		CleanupRevertPushLC(*store);

		switch(aAction)
			{
		case ESchBackupOperationAdd:
			LOGSTRING("CSchBackupManager::PerformStoreOperationL - OpAdd");
			iScheduleIndex->AddL(iIndexStreamSchedules, *store, aSchedule);
			break;
		case ESchBackupOperationEdit:
			LOGSTRING("CSchBackupManager::PerformStoreOperationL - OpEdit");
			iScheduleIndex->EditL(iIndexStreamSchedules, *store, aSchedule);
			break;
		case ESchBackupOperationDelete:
			LOGSTRING("CSchBackupManager::PerformStoreOperationL - OpDelete");
			iScheduleIndex->DeleteL(iIndexStreamSchedules, *store, aSchedule);
			break;
		default:
			__ASSERT_DEBUG(EFalse, User::Invariant());
			User::Leave(KErrNotSupported);
			}	
		// Save changes to store
		store->CommitL();

		CleanupStack::Pop(); // Store reversion cleanup item
		CleanupStack::PopAndDestroy(store);

		// Indicate the store has changed and attempt to compact 
		// the store, but only if the required number of store 
		// changes has been met
		StoreChangedL();
		}
	}

void CSchBackupManager::PerformStoreOperationL(TSchBackupOperation aAction, const CClientProxy& aClient)
//
//	Perform an schedule-related operation.
//
	{
	LOGSTRING2("CSchBackupManager::PerformStoreOperationL - Client: %S", &aClient.ExecutorFileName());

	// Cancel any compaction that may be going on
	Cancel();

	// Perform the operation
	CStreamStore* store = OpenOrCreateBackupStoreLC();
	CleanupRevertPushLC(*store);

	switch(aAction)
		{
	case ESchBackupOperationAdd:
		LOGSTRING("CSchBackupManager::PerformStoreOperationL - OpAdd");
		iClientIndex->AddL(iIndexStreamClients, *store, aClient);
		break;
	case ESchBackupOperationEdit:
		LOGSTRING("CSchBackupManager::PerformStoreOperationL - OpEdit");
		iClientIndex->EditL(iIndexStreamClients, *store, aClient);
		break;
	case ESchBackupOperationDelete:
		LOGSTRING("CSchBackupManager::PerformStoreOperationL - OpDelete");
		iClientIndex->DeleteL(iIndexStreamClients, *store, aClient);
		break;
	default:
		__ASSERT_DEBUG(EFalse, User::Invariant());
		User::Leave(KErrNotSupported);
		}

	// Save changes to store
	store->CommitL();

	CleanupStack::Pop(); // Store reversion cleanup item
	CleanupStack::PopAndDestroy(store);

	// Indicate the store has changed and attempt to compact 
	// the store, but only if the required number of store 
	// changes has been met
	StoreChangedL();
	}

void CSchBackupManager::RunL()
//
//	Perform a store compaction step
//
	{
	LOGSTRING("CSchBackupManager::RunL - Performing compaction step");

	// Is there any more processing required?
	if	(iStoreReclaimerCount() > 0)
		{
		// Yes, so start next step
		iStoreReclaimer.Next(iStoreReclaimerCount, iStatus);
		SetActive();
		LOGSTRING("CSchBackupManager::RunL - Requesting another compaction step");
		}
	else
		{
		// No, we've finised. Clean up previously allocated
		// resources
		iStoreReclaimer.Close();
		//
		TRAPD(errNotRef, iStoreOpenForCompaction->CommitL());
        UNUSED_VAR(errNotRef);
		delete iStoreOpenForCompaction;
		iStoreOpenForCompaction = NULL;

		// Set this to zero again...
		ResetStoreChanges();
		LOGSTRING("CSchBackupManager::RunL - Compaction complete");
		}
	}

void CSchBackupManager::DoCancel()
//
//	Cancel's any asynchronous store compaction
//
	{
	LOGSTRING("CSchBackupManager::RunL - Cancelling compaction");

	iStoreReclaimer.Release();
	iStoreReclaimer.Close();
	//
	TRAPD(errNotRef, iStoreOpenForCompaction->CommitL());
    UNUSED_VAR(errNotRef);
	delete iStoreOpenForCompaction;
	iStoreOpenForCompaction = NULL;
	}

void CSchBackupManager::StoreChangedL()
	{
	__ASSERT_DEBUG(!IsActive() && !iStoreOpenForCompaction, User::Invariant());

	if	(RecordStoreChange() >= KMaxChangesBeforeCompact)
		{
		// Open the store
		CStreamStore* store = OpenBackupStoreLC();

		// Prepare for compaction
		TInt count;
		iStoreReclaimer.CompactL(*store, count);
		iStoreReclaimerCount = count;

		// Safe to do this now
		iStoreOpenForCompaction = store;
		CleanupStack::Pop();

		// Start asynchronous compaction...
		iStoreReclaimer.Next(iStoreReclaimerCount, iStatus);
		SetActive();
		}
	}

CPermanentFileStore* CSchBackupManager::OpenBackupStoreLC()
	{
	return CPermanentFileStore::OpenLC(iFsSession, iBackupFileName, EFileWrite);
	}

CPermanentFileStore* CSchBackupManager::OpenOrCreateBackupStoreLC()
	{
	CPermanentFileStore* store = NULL;
	TInt error = KErrNone;
	TRAP(error, 
		store = CPermanentFileStore::OpenL(iFsSession, iBackupFileName, EFileWrite);
		);
	if	(error < KErrNone)
		{
		if	(error == KErrNotFound)
			{
			CreateEmptyBackupL();
			store = CPermanentFileStore::OpenL(iFsSession, iBackupFileName, EFileWrite);
			}
		else
			{
			User::Leave(error);
			}
		}
	LOGSTRING("CSchBackupManager::OpenOrCreateBackupStoreLC - store opened");
	CleanupStack::PushL(store);
	return store;
	}

void CSchBackupManager::CleanupRevertPushLC(CStreamStore& aStore)
	{
	CleanupStack::PushL(TCleanupItem(RevertStore, &aStore));
	}

void CSchBackupManager::RevertStore(TAny* aStore)
//
//	The Cleanup Item callback function which is used to rever the store
//	should a leave occur.
//
	{
	CStreamStore* store = reinterpret_cast<CStreamStore*>(aStore);
	store->Revert();
	LOGSTRING("CSchBackupManager::RevertStore - store reverted");
	}

/**
Creates an initialised empty store.
The creation process is performed as an atomic operation.
If the operation fails somewhere at the middle, CreateEmptyBackupL()
will cleanup after itself - the store file will be deleted,
iIndexStreamSchedules and iIndexStreamClients stream IDs will be reinitialised
with invalid values.
*/
void CSchBackupManager::CreateEmptyBackupL()
	{
	TRAPD(err, DoCreateEmptyBackupL());
	if(err != KErrNone)
		{
		//Cleanup & leave
		// If unable to delete file record the fact
		TInt err2 = iFsSession.Delete(iBackupFileName);
		if (err2 != KErrNone)
			{
			LOGSTRING2("CSchBackupManager::CreateEmptyBackupL - File delete error = %d", err2);
			}
		iIndexStreamSchedules = iIndexStreamClients = KNullStreamId;
		User::Leave(err);
		}
	}

/**
Creates an initialised empty store.
*/
void CSchBackupManager::DoCreateEmptyBackupL()
	{
	LOGSTRING("CSchBackupManager::CreateEmptyBackupL - trying to create new store");
	CPermanentFileStore* store = CPermanentFileStore::ReplaceLC(iFsSession, iBackupFileName, EFileWrite);
	store->SetTypeL(KPermanentFileStoreLayoutUid);	

	// Create emtpy schedule index stream
	CSchScheduleIndex* indexSchedule = CSchScheduleIndex::NewL();
	CleanupStack::PushL(indexSchedule);
	iIndexStreamSchedules = indexSchedule->CreateEmptyIndexL(*store);
	CleanupStack::PopAndDestroy(indexSchedule);

	// Create emtpy client index stream
	CSchClientIndex* indexClient = CSchClientIndex::NewL(iFsSession);
	CleanupStack::PushL(indexClient);
	iIndexStreamClients = indexClient->CreateEmptyIndexL(*store);
	CleanupStack::PopAndDestroy(indexClient);

	// Write root stream
	WriteRootStreamL(*store);

	// Finalise
	store->CommitL();							
	CleanupStack::PopAndDestroy(store);
	LOGSTRING("CSchBackupManager::CreateEmptyBackupL - new store created");
	}
	
void CSchBackupManager::WriteRootStreamL(CStreamStore& aStore)
//
//	Write the root stream which contains the two stream id's
//
	{
	LOGSTRING("CSchBackupManager::WriteRootStreamL - trying to write root stream");
	RStoreWriteStream stream;
	TStreamId id = stream.CreateLC(aStore);	
	
	// This writes a stream id - it doesn't write the actual
	// dictionary since this is written after every operation.
	stream << iIndexStreamSchedules;
	stream << iIndexStreamClients;

	// 
	stream.CommitL();						
    CleanupStack::PopAndDestroy();// outstream
	static_cast<CPermanentFileStore&>(aStore).SetRootL(id);
	LOGSTRING("CSchBackupManager::WriteRootStreamL - root stream written");
	}

//
// -----> CSchScheduleIndex (header)
//

CSchScheduleIndex::CSchScheduleIndex()
	{
	}

CSchScheduleIndex* CSchScheduleIndex::NewL()
	{
	CSchScheduleIndex* self = new(ELeave) CSchScheduleIndex;
	return self;
	}

void CSchScheduleIndex::RestoreSchedulesL(TSglQue<CSchedule>& aTimeSchedules, 
										CStreamStore& aStore, 
										TStreamId aDictionaryStreamId)
	{
	CSchScheduleDictionary* dictionary = DictionaryLC(aStore, aDictionaryStreamId);

	// Restore every schedule in the dictionary
	const TInt count = dictionary->Count();
	LOGSTRING2("CSchScheduleIndex::RestoreSchedulesL - read %d dictionary entries", count);

	for(TInt i=0; i<count; i++)
		{
		// Get stream
		TStreamId stream = dictionary->AtIndex(i);

		// Restore schedule from stream
		CSchedule* schedule = CSchedule::NewL(static_cast<CFileStore&>(aStore), stream);

		// Save schedule
		aTimeSchedules.AddLast(*schedule); // takes ownership

		LOGSTRING3("CSchScheduleIndex::RestoreSchedulesL - restored schedule: %S, %d", &schedule->Name(), schedule->Id());
		}
	CleanupStack::PopAndDestroy(dictionary);
	}

TStreamId CSchScheduleIndex::CreateEmptyIndexL(CStreamStore& aStore) const
	{
	CSchScheduleDictionary* dictionary = CSchScheduleDictionary::NewLC();
	RStoreWriteStream stream;
	TStreamId id = stream.CreateLC(aStore);
	stream << *dictionary;
	stream.CommitL();
	CleanupStack::PopAndDestroy(2); // stream, dictionary
	return id;
	}

void CSchScheduleIndex::AddL(TStreamId aIndexStream, CStreamStore& aStore, const CSchedule& aSchedule)
//
//	Saves the specified schedule to the store and adds a new entry to the
//	dictionary.
//
	{
	LOGSTRING2("CSchScheduleIndex::AddL - adding a new schedule (id is %d)", aSchedule.Id());
	RStoreWriteStream stream;
	TStreamId id = stream.CreateLC(aStore);
	stream << aSchedule;
	stream.CommitL();
	CleanupStack::PopAndDestroy(); // stream

	// Read the dictionary and update an entry
	CSchScheduleDictionary* dictionary = DictionaryLC(aStore, aIndexStream);
	dictionary->AssignL(aSchedule.Id(), id);

	// Store the dictionary
	StoreDictionaryL(aStore, *dictionary, aIndexStream);
	CleanupStack::PopAndDestroy(dictionary);
	LOGSTRING("CSchScheduleIndex::AddL - new schedule added okay");
	}

void CSchScheduleIndex::EditL(TStreamId aIndexStream, CStreamStore& aStore, const CSchedule& aSchedule)
//
//	Replace an existing stream with the contents of aSchedule. 
//
	{
	// Locate the existing stream (to be replaced)
	CSchScheduleDictionary* dictionary = DictionaryLC(aStore, aIndexStream);
	LOGSTRING2("CSchScheduleIndex::EditL - editing schedule with id of %d", aSchedule.Id());
	TStreamId id = dictionary->At(aSchedule.Id());
	CleanupStack::PopAndDestroy(dictionary);
	if	(id == KNullStreamId)
		{
		// Wasn't found, add it instead...
		LOGSTRING("CSchScheduleIndex::EditL - schedule wasn't found, adding new entry to the store");
		AddL(aIndexStream, aStore, aSchedule);
		}
	else
		{
		// Replace - the original stream is orphaned but the new
		// stream retains the old id.
		LOGSTRING("CSchScheduleIndex::EditL - replacing original stream");
		RStoreWriteStream stream;
		stream.ReplaceLC(aStore, id);
		stream << aSchedule;
		stream.CommitL();
		CleanupStack::PopAndDestroy(); // stream
		}
	}

void CSchScheduleIndex::DeleteL(TStreamId aIndexStream, CStreamStore& aStore, const CSchedule& aSchedule)
//
//	Remove an existing schedule from the store.
//
	{
	// Locate the existing stream (to be deleted)
	CSchScheduleDictionary* dictionary = DictionaryLC(aStore, aIndexStream);
	LOGSTRING2("CSchScheduleIndex::DeleteL - deleting schedule with id of %d", aSchedule.Id());
	TStreamId id = dictionary->At(aSchedule.Id());
	if	(id == KNullStreamId)
		{
		LOGSTRING("CSchScheduleIndex::DeleteL - schedule wasn't found! Would panic in debug");
		__ASSERT_DEBUG(EFalse, SchBackupManagerUtils::Panic(SchBackupManagerUtils::ESchBackupManagerScheduleStreamToDeleteNotFound));
		}
	else
		{
		// Remove the stream
		aStore.DeleteL(id);

		// Save changes to store - we have to do this here since
		// we must assure that the store is fully updated before
		// we remove this stream from the dictionary (in order to 
		// maintain integrity).
		aStore.CommitL();

		// Remove entry from the stream dictionary
		dictionary->Remove(aSchedule.Id());
		StoreDictionaryL(aStore, *dictionary, aIndexStream);
		}
	CleanupStack::PopAndDestroy(dictionary);
	}

CSchScheduleIndex::CSchScheduleDictionary* CSchScheduleIndex::DictionaryLC(CStreamStore& aStore, TStreamId aIndexStream)
	{
	LOGSTRING("CSchScheduleIndex::DictionaryLC - restoring dictionary");
	CSchScheduleDictionary* dictionary = CSchScheduleDictionary::NewLC();
	RStoreReadStream stream;
	stream.OpenLC(aStore, aIndexStream);
	stream >> *dictionary;
	CleanupStack::PopAndDestroy(); // stream
	LOGSTRING("CSchScheduleIndex::DictionaryLC - dictionary restored");
	return dictionary;
	}

void CSchScheduleIndex::StoreDictionaryL(CStreamStore& aStore, const CSchScheduleDictionary& aDictionary, TStreamId aStreamToReplace)
	{
	LOGSTRING("CSchScheduleIndex::DictionaryLC - storing dictionary");
	RStoreWriteStream stream;
	stream.ReplaceLC(aStore, aStreamToReplace);
	stream << aDictionary;
	stream.CommitL();
	CleanupStack::PopAndDestroy(); // stream
	LOGSTRING("CSchScheduleIndex::DictionaryLC - dictionary stored");
	}

//
// -----> CSchScheduleDictionary (header)
//

CSchScheduleIndex::CSchScheduleDictionary::CSchScheduleDictionary()
	{
	}

CSchScheduleIndex::CSchScheduleDictionary::~CSchScheduleDictionary()
	{
	delete iMappings;
	}

void CSchScheduleIndex::CSchScheduleDictionary::ConstructL()
	{
	const TInt KGranularity = 3;
	iMappings = new(ELeave) CArrayFixSeg<TSchScheduleMapplet>(KGranularity);
	}

CSchScheduleIndex::CSchScheduleDictionary* CSchScheduleIndex::CSchScheduleDictionary::NewLC()
	{
	CSchScheduleDictionary* self = new(ELeave) CSchScheduleDictionary();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CSchScheduleIndex::CSchScheduleDictionary::AssignL(TInt aKey, TStreamId aId)
	{
	if	(aId == KNullStreamId)
		{
		Remove(aKey); // default associated stream is null
		return;
		}
	//
	TSchScheduleMapplet entry(aKey, KNullStreamId);
	TKeyArrayFix key(TSchScheduleMapplet::KeyOffset(), ECmpTInt32);
	TInt i;
	if	(iMappings->FindIsq(entry, key, i) == 0)
		{
		iMappings->At(i).SetStream(aId);
		return;
		}
	//
	entry.SetStream(aId);
	iMappings->InsertIsqL(entry, key);
	}

void CSchScheduleIndex::CSchScheduleDictionary::Remove(TInt aKey)
	{
	TSchScheduleMapplet entry(aKey, KNullStreamId);
	TKeyArrayFix key(TSchScheduleMapplet::KeyOffset(), ECmpTInt32);
	TInt i;
	if	(iMappings->FindIsq(entry, key, i) == 0)
		iMappings->Delete(i);
	}

TInt CSchScheduleIndex::CSchScheduleDictionary::Count() const
	{
	return iMappings->Count();
	}

TStreamId CSchScheduleIndex::CSchScheduleDictionary::At(TInt aKey) const
	{
	TSchScheduleMapplet entry(aKey, KNullStreamId);
	TKeyArrayFix key(TSchScheduleMapplet::KeyOffset(), ECmpTInt32);
	TInt i;
	if	(iMappings->FindIsq(entry, key, i) != 0)
		return KNullStreamId;
	//
	return iMappings->At(i).Stream();
	}

TStreamId CSchScheduleIndex::CSchScheduleDictionary::AtIndex(TInt aIndex) const
	{
	return iMappings->At(aIndex).Stream();
	}

void CSchScheduleIndex::CSchScheduleDictionary::InternalizeL(RReadStream& aStream)
	{
	aStream >> *iMappings;
	}

void CSchScheduleIndex::CSchScheduleDictionary::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << *iMappings;
	}

//
// -----> CSchClientIndex (header)
//

CSchClientIndex::CSchClientIndex(RFs& aFsSession)
:	iFsSession(aFsSession)
	{
	}

CSchClientIndex* CSchClientIndex::NewL(RFs& aFsSession)
	{
	return new(ELeave) CSchClientIndex(aFsSession);
	}

void CSchClientIndex::AppendClientToListL(TPriQue<CClientProxy>& aClients, CClientProxy* aClient)
	{
	CClientProxy* currentClient;
	TDblQueIter<CClientProxy> clientIter(aClients);
	clientIter.SetToFirst();
	while ((currentClient = clientIter++) != NULL)
		{	
		//if match on same client name and priority	
		if (currentClient->IsEqual(aClient->ExecutorFileName(),aClient->Priority()))
			{
			//transfer all the tasks in aClient to this client
			aClient->TransferTasks(*currentClient);
			//now that we have transferred all the task ownership, we can safely delete the source client
			delete aClient;
			return;
			}
		}
	//since that there is no matching one just add the aClient directly
	aClients.Add(*aClient);	
	}

void CSchClientIndex::RestoreClientsL(TPriQue<CClientProxy>& aClients, 
									CStreamStore& aStore, 
									TStreamId aIndexStream,
									CSchLogManager& aSchLogManager,TBool aBUR)
	{
	CSchClientDictionary* dictionary = DictionaryLC(aStore, aIndexStream);

	// Restore every schedule in the dictionary
	const TInt count = dictionary->Count();
	LOGSTRING2("CSchClientIndex::RestoreClientsL - read %d dictionary entries", count);

	for(TInt i=0; i<count; i++)
		{
		// Get stream
		TStreamId streamId = dictionary->AtIndex(i);

		RStoreReadStream stream;
		stream.OpenLC(aStore, streamId);

		// Restore client
		CClientProxy* client = CClientProxy::NewL(iFsSession,stream,aSchLogManager);
		CleanupStack::PopAndDestroy(); // stream

		// Save schedule
		// only when this is called through restore we need to append to existing client which
		// might already contain other transient tasks
		if (aBUR)
			{
			AppendClientToListL(aClients,client);
			}
		else
			{
			aClients.Add(*client); // takes ownership				
			}
		LOGSTRING2("CSchClientIndex::RestoreClientsL - restored client: %S", &client->ExecutorFileName());
		}
	CleanupStack::PopAndDestroy(dictionary);
	}

TStreamId CSchClientIndex::CreateEmptyIndexL(CStreamStore& aStore) const
	{
	CSchClientDictionary* dictionary = CSchClientDictionary::NewLC();
	RStoreWriteStream stream;
	TStreamId id = stream.CreateLC(aStore);
	stream << *dictionary;
	stream.CommitL();
	CleanupStack::PopAndDestroy(2); // stream, dictionary
	return id;
	}

void CSchClientIndex::AddL(TStreamId aIndexStream, CStreamStore& aStore, const CClientProxy& aClient)
	{
	RStoreWriteStream stream;
	TStreamId id = stream.CreateLC(aStore);
	stream << aClient;
	stream.CommitL();
	CleanupStack::PopAndDestroy(); // stream

	// Read the dictionary and update an entry
	CSchClientDictionary* dictionary = DictionaryLC(aStore, aIndexStream);
	dictionary->AssignL(aClient.ExecutorFileName(), id);

	// Store the dictionary
	StoreDictionaryL(aStore, *dictionary, aIndexStream);
	CleanupStack::PopAndDestroy(dictionary);
	}

void CSchClientIndex::EditL(TStreamId aIndexStream, CStreamStore& aStore, const CClientProxy& aClient)
	{
	// Locate the existing stream (to be replaced)
	CSchClientDictionary* dictionary = DictionaryLC(aStore, aIndexStream);
	TStreamId id = dictionary->AtL(aClient.ExecutorFileName());
	CleanupStack::PopAndDestroy(dictionary);
	if	(id == KNullStreamId)
		{
		// Wasn't found, add it instead...
		AddL(aIndexStream, aStore, aClient);
		}
	else
		{
		// Replace - the original stream is orphaned but the new
		// stream retains the old id.
		RStoreWriteStream stream;
		stream.ReplaceLC(aStore, id);
		stream << aClient;
		stream.CommitL();
		CleanupStack::PopAndDestroy(); // stream
		}
	}

void CSchClientIndex::DeleteL(TStreamId aIndexStream, CStreamStore& aStore, const CClientProxy& aClient)
	{
	// Locate the existing stream (to be deleted)
	CSchClientDictionary* dictionary = DictionaryLC(aStore, aIndexStream);
	TStreamId id = dictionary->AtL(aClient.ExecutorFileName());
	__ASSERT_ALWAYS(id != KNullStreamId, SchBackupManagerUtils::Panic(SchBackupManagerUtils::ESchBackupManagerScheduleStreamToDeleteNotFound));

	// Remove the stream
	aStore.DeleteL(id);

	// Save changes to store - we have to do this here since
	// we must assure that the store is fully updated before
	// we remove this stream from the dictionary (in order to 
	// maintain integrity).
	aStore.CommitL();

	// Remove entry from the stream dictionary
	dictionary->RemoveL(aClient.ExecutorFileName());
	StoreDictionaryL(aStore, *dictionary, aIndexStream);
	CleanupStack::PopAndDestroy(dictionary);
	}

CSchClientIndex::CSchClientDictionary* CSchClientIndex::DictionaryLC(CStreamStore& aStore, TStreamId aIndexStream)
	{
	LOGSTRING("CSchClientIndex::DictionaryLC - restoring dictionary");
	CSchClientDictionary* dictionary = CSchClientDictionary::NewLC();
	RStoreReadStream stream;
	stream.OpenLC(aStore, aIndexStream);
	stream >> *dictionary;
	CleanupStack::PopAndDestroy(); // stream
	LOGSTRING("CSchClientIndex::DictionaryLC - dictionary restored");
	return dictionary;
	}

void CSchClientIndex::StoreDictionaryL(CStreamStore& aStore, const CSchClientDictionary& aDictionary, TStreamId aStreamToReplace)
	{
	LOGSTRING("CSchClientIndex::DictionaryLC - storing dictionary");
	RStoreWriteStream stream;
	stream.ReplaceLC(aStore, aStreamToReplace);
	stream << aDictionary;
	stream.CommitL();
	CleanupStack::PopAndDestroy(); // stream
	LOGSTRING("CSchClientIndex::DictionaryLC - dictionary stored");
	}

//
// -----> TKeyArrayPtr (header)
//
NONSHARABLE_CLASS(TKeyArrayMapping) : public TKeyArrayFix
{
public:
	inline TKeyArrayMapping(TInt aOffset) : TKeyArrayFix(aOffset, TKeyCmpText()) {iHBufType=ETrue;}
	//
	virtual TAny* At(TInt aIndex) const;
	virtual TInt Compare(TInt aLeft,TInt aRight) const;	

private:
	TBool iHBufType;
	};

TAny* TKeyArrayMapping::At(TInt aIndex) const
	{
	if	(aIndex==KIndexPtr)
		{
		const CSchClientIndex::CSchClientMapplet** ppItem = (const CSchClientIndex::CSchClientMapplet**) iPtr;
		const CSchClientIndex::CSchClientMapplet* pItem = *ppItem;
		return (TAny*) pItem;
		}
	else
		{
		TInt baseOffset = aIndex * sizeof(const CSchClientIndex::CSchClientMapplet*);
		const TUint8* pItemUncast = iBase->Ptr(baseOffset).Ptr();
		const CSchClientIndex::CSchClientMapplet** ppItem = (const CSchClientIndex::CSchClientMapplet**) pItemUncast;
		const CSchClientIndex::CSchClientMapplet* pItem = *ppItem;
		return (TAny*) pItem;
		}
	}

TInt TKeyArrayMapping::Compare(TInt aLeft,TInt aRight) const
	{
	if	(iHBufType)
		{
		const CSchClientIndex::CSchClientMapplet* left = (const CSchClientIndex::CSchClientMapplet*) At(aLeft);
		const CSchClientIndex::CSchClientMapplet* right = (const CSchClientIndex::CSchClientMapplet*) At(aRight);

		return (left->Key().Compare(right->Key()));
		}
	else
		User::Invariant();
	return KErrNotFound;
	}

//
// -----> CSchClientIndex::CSchClientMapplet (header)
//

CSchClientIndex::CSchClientMapplet::~CSchClientMapplet()
	{
	delete iKey;
	}

CSchClientIndex::CSchClientMapplet* CSchClientIndex::CSchClientMapplet::NewLC(RReadStream& aStream)
	{
	CSchClientMapplet* self = new(ELeave) CSchClientMapplet;
	CleanupStack::PushL(self);
	aStream >> *self;
	return self;
	}

CSchClientIndex::CSchClientMapplet* CSchClientIndex::CSchClientMapplet::NewLC(const TDesC& aKey, TStreamId aStream)
	{
	CSchClientMapplet* self = new(ELeave) CSchClientMapplet;
	CleanupStack::PushL(self);
	self->iKey = aKey.AllocL();
	self->iStream = aStream;
	return self;
	}

void CSchClientIndex::CSchClientMapplet::InternalizeL(RReadStream& aStream)
	{
	HBufC* key = HBufC::NewLC(aStream, KMaxTInt);
	delete iKey;
	iKey = key;
	CleanupStack::Pop(); // key
	aStream >> iStream;
	}

void CSchClientIndex::CSchClientMapplet::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << *iKey;
	aStream << iStream;
	}


//
// -----> CSchClientIndex::CSchClientDictionary (header)
//

CSchClientIndex::CSchClientDictionary::CSchClientDictionary()
	{
	}

CSchClientIndex::CSchClientDictionary::~CSchClientDictionary()
	{	
	if(iMappings)
		{
		for(int index = 0; index < iMappings->Count();index++)
			{
			delete iMappings->At(index);
			}
		delete iMappings;	
		}
	}

void CSchClientIndex::CSchClientDictionary::ConstructL()
	{
	const TInt KGranularity = 2;
	iMappings = new(ELeave) CArrayPtrSeg<CSchClientMapplet>(KGranularity);
	}

CSchClientIndex::CSchClientDictionary* CSchClientIndex::CSchClientDictionary::NewLC()
	{
	CSchClientDictionary* self = new(ELeave) CSchClientDictionary();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CSchClientIndex::CSchClientDictionary::AssignL(const TDesC& aKey, TStreamId aId)
	{
	if	(aId == KNullStreamId)
		{
		RemoveL(aKey); // default associated stream is null
		return;
		}

	CSchClientMapplet* entry = CSchClientMapplet::NewLC(aKey, KNullStreamId);
	TKeyArrayMapping key(CSchClientMapplet::KeyOffset());
	TInt i;
	if	(iMappings->FindIsq(entry, key, i) == 0)
		{
		iMappings->At(i)->SetStream(aId);
		CleanupStack::PopAndDestroy(); // entry
		return;
		}

	entry->SetStream(aId);
	iMappings->InsertIsqL(entry, key);
	CleanupStack::Pop(); // entry
	}

void CSchClientIndex::CSchClientDictionary::RemoveL(const TDesC& aKey)
	{
	CSchClientMapplet* entry = CSchClientMapplet::NewLC(aKey, KNullStreamId);
	TKeyArrayMapping key(CSchClientMapplet::KeyOffset());
	TInt i;
	if	(iMappings->FindIsq(entry, key, i) == 0)
		{
		delete iMappings->At(i);
		iMappings->Delete(i);
		}
	CleanupStack::PopAndDestroy(); // entry
	}

TInt CSchClientIndex::CSchClientDictionary::Count() const
	{
	return iMappings->Count();
	}

TStreamId CSchClientIndex::CSchClientDictionary::AtL(const TDesC& aKey) const
	{
	CSchClientMapplet* entry = CSchClientMapplet::NewLC(aKey, KNullStreamId);
	TKeyArrayMapping key(CSchClientMapplet::KeyOffset());
	TInt i;
	TInt found = iMappings->FindIsq(entry, key, i);
	CleanupStack::PopAndDestroy(); // entry
	if	(found != 0)
		return KNullStreamId;
	return iMappings->At(i)->Stream();
	}

TStreamId CSchClientIndex::CSchClientDictionary::AtIndex(TInt aIndex) const
	{
	return iMappings->At(aIndex)->Stream();
	}

void CSchClientIndex::CSchClientDictionary::InternalizeL(RReadStream& aStream)
	{
	const TInt count = aStream.ReadInt32L();
	for(TInt i=0; i<count; i++)
		{
		CSchClientMapplet* mapplet = CSchClientMapplet::NewLC(aStream);
		iMappings->AppendL(mapplet);
		CleanupStack::Pop(); // mapplet
		}
	}

void CSchClientIndex::CSchClientDictionary::ExternalizeL(RWriteStream& aStream) const
	{
	const TInt count = iMappings->Count();
	aStream.WriteInt32L(count);
	for(TInt i=0; i<count; i++)
		aStream << *iMappings->At(i);
	}
