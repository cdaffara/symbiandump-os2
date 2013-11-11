// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// SCHSTORE
// 
//

#ifndef __SCHSTORE_H__
#define __SCHSTORE_H__

// System includes
#include <s32file.h>

// Classes referenced
class CSchScheduleIndex;
class CSchClientIndex;
class CClientProxy;
class CSchLogManager;
class CSchedule;


/**
@internalComponent
*/
class SchBackupManagerUtils
	{
public:
	enum TSchStorePanic
		{
		ESchBackupManagerScheduleStreamToReplaceNotFound,
		ESchBackupManagerScheduleStreamToDeleteNotFound,
		//
		ESchBackupManagerLast
		};

public:
	static void Panic(TSchStorePanic aPanic);
	};


/**
Schedule server backup file name.
@internalComponent
*/
_LIT(KSchsvrBackupFileName, "_:\\Private\\10005399\\SchedulesBackup.dat");
/**
Buffer descriptor to hold complete backup file name.
@internalComponent
*/
typedef TBuf<40> TBackupFileName;



/**
@internalComponent
*/
NONSHARABLE_CLASS(CSchBackupManager) : public CActive
	{
public:
	enum TSchBackupOperation
		{
		ESchBackupOperationAdd,
		ESchBackupOperationEdit,
		ESchBackupOperationDelete
		};

public:
	CSchBackupManager(RFs& aFsSession);
	~CSchBackupManager();
	void ConstructL();

public:
	void RestoreL(TPriQue<CClientProxy>& aClients, 
				TSglQue<CSchedule>& aTimeSchedules,
				CSchLogManager& aSchLogManager,TBool aBUR=EFalse);
	void CreateEmptyBackupL();
	//
	void PerformStoreOperationL(TSchBackupOperation aAction, const CSchedule& aSchedule);
	void PerformStoreOperationL(TSchBackupOperation aAction, const CClientProxy& aClient);

private:
	// The backup manager need to compact the store every now and then. It does
	// this as an idle operation and therefore needs to be CActive-based.
	void RunL();
	void DoCancel();
	void DoCreateEmptyBackupL();

private:
	void StoreChangedL();
	inline TInt RecordStoreChange() { return ++iStoreChangeCount; }
	inline void ResetStoreChanges() { iStoreChangeCount = 0; }

private:
	CPermanentFileStore* OpenBackupStoreLC();
	CPermanentFileStore* OpenOrCreateBackupStoreLC();
	//
	void CleanupRevertPushLC(CStreamStore& aStore);
	static void RevertStore(TAny* aStore);
	//
	void WriteRootStreamL(CStreamStore& aStore);

private:

	RFs& iFsSession;
	
	// The number of times the store has changed since it was last
	// compacted.
	TInt iStoreChangeCount;

	// Used to compact the store.
	RStoreReclaim iStoreReclaimer;

	// Used internally by the reclaimer to judge how much work is left to do.
	TPckgBuf<TInt> iStoreReclaimerCount;

	// A pointer to the store that has been opened for compaction purposes.
	// During normal operation of the backup manager, this pointer will
	// be NULL. However, when performing compaction, the store must be
	// open at all times and since processing is carried out as a series of
	// active object steps, we need to anchor it to the class.
	CStreamStore* iStoreOpenForCompaction;

	// The index to the schedule stream
	TStreamId iIndexStreamSchedules;

	// The index to the client stream
	TStreamId iIndexStreamClients;
	
	// The two pseudo-dictionary classes that manage mappings between
	// (clients and streams) and (schedules and streams).
	CSchClientIndex* iClientIndex;
	CSchScheduleIndex* iScheduleIndex;
	
	// backup file name
	TBackupFileName iBackupFileName;
	};




/**
@internalComponent
*/
class CSchScheduleIndex : public CBase
	{
public:
	static CSchScheduleIndex* NewL();

private:
	CSchScheduleIndex();

public:
	void RestoreSchedulesL(TSglQue<CSchedule>& aTimeSchedules, 
						CStreamStore& aStore, 
						TStreamId aDictionaryStreamId);
	TStreamId CreateEmptyIndexL(CStreamStore& aStore) const;
	//
	void AddL(TStreamId aIndexStream, CStreamStore& aStore, const CSchedule& aSchedule);
	void EditL(TStreamId aIndexStream, CStreamStore& aStore, const CSchedule& aSchedule);
	void DeleteL(TStreamId aIndexStream, CStreamStore& aStore, const CSchedule& aSchedule);

public:
	class TSchScheduleMapplet
		{
	public:
		inline TSchScheduleMapplet() { }
		inline TSchScheduleMapplet(TInt aKey, TStreamId aStream) : iKey(aKey), iStream(aStream) { }

	public:
		void InternalizeL(RReadStream& aStream) { iKey = aStream.ReadInt32L(); aStream >> iStream; }
		void ExternalizeL(RWriteStream& aStream) const { aStream.WriteInt32L(iKey); aStream << iStream; }

	public:
		inline void SetStream(TStreamId aStream) { iStream = aStream; }
		inline TStreamId Stream() const { return iStream; }

	public:
		inline static TInt KeyOffset() { return _FOFF(TSchScheduleMapplet, iKey); }

	private:
		TInt iKey;
		TStreamId iStream;
		};
	
private:

	NONSHARABLE_CLASS(CSchScheduleDictionary) : public CBase
		{
	public:
		static CSchScheduleDictionary* NewLC();
		~CSchScheduleDictionary();

	private:
		CSchScheduleDictionary();
		void ConstructL();

	public:
		void AssignL(TInt aKey, TStreamId aId);
		void Remove(TInt aKey);
		TInt Count() const;
		//
		TStreamId At(TInt aKey) const;
		TStreamId AtIndex(TInt aIndex) const;
		//
		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;

	private:
		CArrayFix<TSchScheduleMapplet>* iMappings;
		};

private:
	CSchScheduleDictionary* DictionaryLC(CStreamStore& aStore, TStreamId aIndexStream);
	void StoreDictionaryL(CStreamStore& aStore, const CSchScheduleDictionary& aDictionary, TStreamId aStreamToReplace);
	};


/**
@internalComponent
*/
class CSchClientIndex : public CBase
	{
public:
	static CSchClientIndex* NewL(RFs& aFsSession);
	
private:
	CSchClientIndex(RFs& aFsSession);

public:
	void RestoreClientsL(TPriQue<CClientProxy>& aClients, 
							CStreamStore& aStore, 
							TStreamId aIndexStream,
							CSchLogManager& aSchLogManager,TBool aBUR=EFalse);
	TStreamId CreateEmptyIndexL(CStreamStore& aStore) const;
	void AppendClientToListL(TPriQue<CClientProxy>& aClients, CClientProxy* aClient);
public:
	void AddL(TStreamId aIndexStream, CStreamStore& aStore, const CClientProxy& aClient);
	void EditL(TStreamId aIndexStream, CStreamStore& aStore, const CClientProxy& aClient);
	void DeleteL(TStreamId aIndexStream, CStreamStore& aStore, const CClientProxy& aClient);

public:
	NONSHARABLE_CLASS(CSchClientMapplet) : public CBase
		{
	public:
		static CSchClientMapplet* NewLC(RReadStream& aStream);
		static CSchClientMapplet* NewLC(const TDesC& aKey, TStreamId aStream);
		~CSchClientMapplet();

	public:
		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;

	public:
		inline const TDesC& Key() const { return *iKey; }
		inline TStreamId Stream() const { return iStream; }
		//
		inline void SetStream(TStreamId aStream) { iStream = aStream; }
		//
		inline static TInt KeyOffset() { return _FOFF(CSchClientMapplet, iStream); }

	private:
		HBufC* iKey;
		TStreamId iStream;
		};

private:
	NONSHARABLE_CLASS(CSchClientDictionary) : public CBase
		{
	public:
		static CSchClientDictionary* NewLC();
		~CSchClientDictionary();

	private:
		CSchClientDictionary();
		void ConstructL();

	public:
		void AssignL(const TDesC& aKey, TStreamId aId);
		void RemoveL(const TDesC& aKey);
		TInt Count() const;
		TStreamId AtL(const TDesC& aKey) const;
		TStreamId AtIndex(TInt aIndex) const;

	public:
		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;

	private:
		CArrayPtr<CSchClientMapplet>* iMappings;
		};

private:
	CSchClientDictionary* DictionaryLC(CStreamStore& aStore, TStreamId aIndexStream);
	void StoreDictionaryL(CStreamStore& aStore, const CSchClientDictionary& aDictionary, TStreamId aStreamToReplace);

private:
	RFs& iFsSession;
	};


#endif
