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
//

#if !defined(__D32TABLE_H__)
#define __D32TABLE_H__
#if !defined(__D32DBAS_H__)
#include "D32DBAS.H"
#endif
#if !defined(__D32REC_H__)
#include "D32REC.H"
#endif
#if !defined(__D32CACHE_H__)
#include "D32CACHE.H"
#endif

// classes defined in this file
class TDbGenerationMark;
class TDbColumnDef;
class HDbColumnSet;
class CDbTableIndexDef;
class RDbIndexes;
class RDbTableSchema;
class CDbTableDef;
class RDbTableRow;
class CDbTable;
class RDbTables;
class RDbTransaction;
class CDbTableDatabase;

// classes also referenced
class CDbTableSource;
class CDbBlobCleanup;

/**
@internalComponent
*/
const TInt KDbTableMaxIndexes = 32;

/**
@internalComponent
*/
class CDbTableIndexDef : public CBase
	{
	friend class RDbIndexes;
public:
	IMPORT_C CDbTableIndexDef();
	IMPORT_C ~CDbTableIndexDef();
	IMPORT_C void ConstructL( const TDesC& aName );
	inline const TDesC& Name() const;
	inline const CDbKey& Key() const;
	inline CDbKey& Key();
private:
	TSglQueLink iLink;
	HBufC* iName;
	CDbKey iKey;
	};

/**
@internalComponent
*/
class RDbIndexes
	{
	friend class CDbTableDef;
public:
	inline void Add( CDbTableIndexDef* aDef );
	inline void Remove( CDbTableIndexDef* aDef );
	inline const TSglQueBase& AsQue() const;
	CDbTableIndexDef* Find( const TDesC& aName ) const;
	CDbTableIndexDef& FindL( const TDesC& aName ) const;
	IMPORT_C TInt Count() const;
private:
	inline RDbIndexes();
	void Close();
private:
	TSglQue<CDbTableIndexDef> iRep;
	};

/**
@internalComponent
*/
class TDbColumnDef
	{
public:
	IMPORT_C void SetL( const TDbCol& aCol );
	IMPORT_C void SetL( const TDbColumnDef& aCol );
	void AsTDbCol( TDbCol& aColumn ) const;
	inline TDbColType Type() const;
private:
// don't allow copies
	void operator=( const TDbColumnDef& );
public:
	// flags used by AlterTable
	enum { EDropped = 0x1, EChangedType = 0x2, EChangedLen = 0x4, EAdded = 0x8 };
public:
	HBufC* iName;
	TInt iMaxLength;
	TUint8 iType;
	TUint8 iAttributes;
	TUint8 iFlags;
	TUint8 iReserved;
	};

/**
@internalComponent
*/
class HDbColumnSet
	{
private:
	enum { ELongColumns = 0x1, EAutoIncrement = 0x2 };
public:
	typedef TDbColumnDef* TIterator;
	typedef const TDbColumnDef* TIteratorC;
public:
	static HDbColumnSet* NewL( TInt aCount );
	~HDbColumnSet();
//
	inline TIterator Begin();
	void Complete();
	IMPORT_C TInt Count() const;
	inline TIteratorC Begin() const;
	inline TIteratorC End() const;
	inline const TDbColumnDef& operator[]( TDbColNo anIndex ) const;
	IMPORT_C TIteratorC ColumnL( const TDesC& aColumn ) const;
	inline TIterator ColumnL( const TDesC& aColumn );
	IMPORT_C TDbColNo ColNoL( const TDesC& aColumn ) const;
	inline TBool HasLongColumns() const;
	inline TBool HasAutoIncrement() const;
private:
	HDbColumnSet( TInt aCount );
	const TIteratorC* IndexL() const;
private:
	TUint iAttributes;
	TIteratorC* iIndex;
	const TDbColumnDef* const iEnd;
	TDbColumnDef iColumns[1];		// at least one
	};

/**
@internalComponent
*/
class CDbTableDef : public CBase
	{
	friend class RDbTableSchema;
public:
	IMPORT_C ~CDbTableDef();
	inline const TDesC& Name() const;
	inline HDbColumnSet& Columns();
	inline const HDbColumnSet& Columns() const;
	inline RDbIndexes& Indexes();
	inline const RDbIndexes& Indexes() const;
	IMPORT_C virtual void Changed();
//
	inline const CDbTableIndexDef* Key( const TDesC& aColumn ) const;
	inline TBool IsIndexed( const TDesC& aColumn ) const;
//
	virtual void AlteredColumnSetL( HDbColumnSet& aSet, const CDbColSet& aChange, const CDbColSet& aAdd ) = 0;
	void ExchangeColumnSet( HDbColumnSet* aSet );
protected:
	IMPORT_C CDbTableDef();
	IMPORT_C void ConstructL( const TDesC& aName, TInt aColumnCount );
private:
	const CDbTableIndexDef* FindKey( const TDesC& aColumn, TBool aFirstColumn ) const;
private:
	TSglQueLink iLink;
	HBufC* iName;
	HDbColumnSet* iColumns;
	RDbIndexes iIndexes;
	};

/**
@internalComponent
*/
class RDbTableSchema
	{
	friend class CDbTableDatabase;
public:
	inline void Loaded();
	inline operator TSglQueBase&();
	inline void Add( CDbTableDef* aDef );
//
	inline TBool IsEmpty() const;
	CDbTableDef* Find( const TDesC& aTable );
	CDbTableDef& FindL( const TDesC& aTable );
private:
	inline RDbTableSchema();
	void Close();
//
	inline TBool IsLoaded() const;
	void Discard();
	inline void Remove( CDbTableDef* aDef );
private:
	TSglQue<CDbTableDef> iRep;
	TBool iLoaded;
	};

/**
@internalComponent
*/
typedef TUint TDbGeneration;

/**
@internalComponent
*/
class TDbGenerationMark
	{
public:
	inline void Construct( const TDbGeneration& aGeneration );
	inline void Mark();
	inline TBool Changed() const;
private:
	const TDbGeneration* iGeneration;
	TDbGeneration iMark;
	};

/**
@internalComponent
*/
class CDbTable : public CBase
	{
	friend class RDbTables;
public:
	class TValid;
	friend class TValid;
	enum { EUnavailableSpan = -1, EFullIndexSpan = 100 };
protected:
	IMPORT_C CDbTable( CDbTableDatabase& aDatabase, const CDbTableDef& aDef );
	IMPORT_C ~CDbTable();
public:
	void Open();
	void Close();
	void FlushL();
	void Abandon();
	void Release();
	void Idle();
//
	inline TBool IsActive() const;
	inline TBool IsIdle() const;
	inline const CDbTableDef& Def() const;
	inline CDbTableDatabase& Database();
//
	inline const TDbGeneration& Generation() const;
	TBool ExistsL( TDbRecordId aRecordId );
	void NewRowL( RDbRow& aRow );
	void ReadRowL( RDbRow& aRow, TDbRecordId aRecordId );
	void PrepareReplaceL( const RDbTableRow& aRow, TDbRecordId aRecordId );
	void ReplaceRowL( RDbTableRow& aRow, TDbRecordId aRecordId );
	void PrepareAppendL( const RDbTableRow& aRow );
	TDbRecordId AppendRowL( const RDbTableRow& aRow );
	void DeleteRowL( RDbTableRow& aRow, TDbRecordId aRecordId );
	void DuplicateBlobsL( RDbRow& aRow );
	IMPORT_C void DiscardBlobsL( RDbRow& aRow );
//
	IMPORT_C CDbRecordSpace& RecordsL();
	IMPORT_C CDbRecordIndex& IndexL( const CDbTableIndexDef& anIndex );
	IMPORT_C CDbBlobSpace* BlobsL();
	virtual CDbRecordIndex* RecordIndexL( const CDbTableIndexDef& anIndex ) = 0;
//
	CDbRecordIter* IteratorL();
	IMPORT_C virtual TInt IndexSpanL( const CDbTableIndexDef& aIndex, TUint aInclusion, const TDbLookupKey* aLowerBound, const TDbLookupKey* aUpperBound );
	CDbRecordIter* IteratorL( const CDbTableIndexDef& aIndex, TUint aInclusion = 0, const TDbLookupKey* aLowerBound = 0, const TDbLookupKey* aUpperBound = 0 );
	CDbRecordIter* IteratorL( const TDesC& aIndex );
private:
	typedef void ( *TBlobFuncL )( CDbBlobSpace& aBlobStore, TDbBlob& aBlob, TDbColType aType, CDbBlobCleanup* aCleanup );
private:
	inline void Discard();
	void Disconnect();
	inline TBool InUse() const;
	inline RDbCache& Cache();
//
	void ApplyToBlobsL( RDbRow& aRow, TBlobFuncL aFuncL, CDbBlobCleanup* aCleanup = 0 );
	void ApplyToComponentsL( void ( *anOperationL )( CDbRecordBase* ) );
	void EnsureIndexesL();
	void ValidateL( const RDbRow& aRow );
	void CheckInliningL( RDbRow& aRow );
	void DoReplaceRowL( const RDbRow& aRow, TDbRecordId aRecordId );
//
	virtual CDbRecordSpace* RecordSpaceL() = 0;
	virtual CDbBlobSpace* BlobSpaceL() = 0;
//
	virtual void CopyToRowL( RDbRow& aRow, const TDesC8& aRecord ) = 0;
	virtual TInt RecordLength( const RDbRow& aRow ) = 0;
	virtual void CopyFromRow( TUint8* aRecord, const RDbRow& aRow ) = 0;
private:
	enum { EIdle = -1, ECached = -2 };
private:
	TSglQueLink iLink;
	TInt iRef;
	CDbTableDatabase* iDatabase;
	const CDbTableDef* iDef;
	TDbGeneration iGeneration;
	CDbRecordSpace* iRecords;
	CDbBlobSpace* iBlobs;
	CDbRecordIndex* iIndexes[KDbTableMaxIndexes];
	CDbRecordIndex** iIndexesEnd;
	TUint32 iUpdateMap;
	};

/**
@internalComponent
*/
class RDbTableRow : public RDbRow
	{
public:
	inline RDbTableRow();
	void Open( CDbTable* aTable );
	void Close();
//
	inline CDbTable& Table() const;
	IMPORT_C void ReadL( TDbRecordId aRecordId );
	void NewL( TDbRecordId aCopyRecord );
	void PrepareAppendL();
	TDbRecordId AppendL();
	void PrepareReplaceL();
	TDbRecordId ReplaceL();
	void DeleteL( TDbRecordId aRecordId );
private:
	TDbGenerationMark iMark;
	TDbRecordId iRecord;
	CDbTable* iTable;
	};

/**
implementation class
@internalComponent
*/
class RDbTables		
	{
public:
	inline RDbTables();
	void Close();
//
	inline TBool IsEmpty() const;
	inline void Add( CDbTable& aTable );
	inline void Remove( CDbTable& aTable );
	CDbTable* Find( const TDesC& aTable );
	inline operator TSglQueBase&();
private:
	TSglQue<CDbTable> iRep;
	};

/**
@internalComponent
*/
enum TDbLockType { EDbReadLock = 0, EDbXReadLock, EDbWriteLock, EDbCompactLock, EDbRecoveryLock, EDbSchemaLock };

/**
implementation class
@internalComponent
*/
class RDbTransaction
	{
private:
	class CNotifier;
	friend class CNotifier;
public:
	inline RDbTransaction();
	inline void Open( CDbTableDatabase& aDatabase );
	void Close();
//
	inline const TDbGeneration& RollbackGeneration() const;
	inline CDbTableDatabase& Database();
//
	void BeginL( const CDbObject& aObject );
	void CommitL( const CDbObject& aObject );
	void Rollback( const CDbObject& aObject );
//
	void ReadPrepareL( const CDbObject& aObject );
	void ReadBegin( const CDbObject& aObject );
	void ReadRelease( const CDbObject& aObject );
//
	void DMLCheckL();
	void DMLPrepareL( const CDbObject& aObject );
	void DMLBegin();
	void DMLTouch();
	void DMLBeginLC();
	void DMLCommitL();
	void DMLRollback();
//
	void DDLPrepareL( const CDbObject& aObject );
	void DDLBegin();
	void DDLBeginLC();
	void DDLCommitL();
	void DDLRollback();
//
	void UtilityPrepareL( const CDbObject& aObject );
	void UtilityBegin( CDbDatabase::TUtility aType );
	void UtilityCommitL();
	void UtilityRollback();
//
	inline TBool IsLocked() const;
	inline void ReadyL() const;
	TBool InTransaction( const CDbObject& aObject );
//
	CDbNotifier* NotifierL();
#ifdef _ASSERTIONS
	void _Invariant() const;
#endif
private:
	typedef const CDbContext* THolder;
	struct TLock
		{
		THolder iHolder;
		TUint iState;
		};
	enum { ELockListGranularity = 4 };
	enum { ETransactionLock = 0x80000000 };
	enum { EState = 0x7f, EFailed = 0x80 };
	enum { EMaxLock = 255 };
private:
	inline TDbLockType LockState() const;
	void PrepareSLockL( const CDbObject& aObject, TUint aInitState );
	void PrepareXLockL( const CDbObject& aObject );
	void Unlock( RDbNotifier::TEvent aEvent );
	void Unlock( TLock& aLock );
	TLock* GetLock( const CDbObject& aObject );
	void Event( RDbNotifier::TEvent aEvent );
//
	void DoCommitL();
	void DoRollback();
	static void DMLAbandon( TAny* aPtr );
	static void DDLAbandon( TAny* aPtr );
private:
	CDbTableDatabase* iDatabase;
	TUint8 iAction;
	TUint8 iLockState;
	TUint8 iLockCount;
	TUint8 iMaxLock;
	TInt iUpdaters;
	TLock iPrimary;
	TLock* iSharers;
	TDbGeneration iRollback;
	CNotifier* iNotifier;
	};

/**
@internalComponent
*/
class CDbTableDatabase : public CBase
	{
private:
	class CIncremental;
	class CIncrementalDDL;
	class CCreateIndex;
	class CDropIndex;
	class CDropTable;
	class CAlterTable;
	class CUtility;
	class CInterface;
	class CSource;
//
	friend class RDbTransaction;
	friend class CInterface;
	friend class CSource;
	friend class CDbTable;
	friend class CIncremental;
public:
	/**
	@internalComponent
	*/
	class CStepper : public CBase
		{
	protected:
		CStepper() {}
	public:
		virtual TInt StepL( TInt aStep ) = 0;
		};

	/**
	@internalComponent
	*/
	class CMultiStepper : public CStepper
		{
	private:
		struct SStepper
			{
			CStepper* iStepper;
			TInt iStep;
			};
	public:
		~CMultiStepper();
		IMPORT_C static CMultiStepper* NewL( TInt aStepperCount );
		IMPORT_C void AddStepper( CStepper* aStepper, TInt aSteps );
		IMPORT_C TInt TotalSteps();
		TInt StepL( TInt aStep );
	private:
		inline CMultiStepper( TInt aStepperCount );
	private:
		SStepper* iStepper;
		SStepper* iEnd;
		SStepper iSteppers[1];		// one or more
		};

	/**
	@internalComponent
	*/
	class CBuildIndex : public CStepper
		{
	private:
		enum { ERecordsPerStep = 16 };
	public:
		IMPORT_C static CBuildIndex* NewL( CDbTableDatabase& aDatabase, const CDbTableDef& aTable, const CDbTableIndexDef& anIndex );
		~CBuildIndex();
		IMPORT_C static TInt Steps( TInt aCardinality );
		inline CDbRecordIndex& Index();
		IMPORT_C TInt StepsL();
		// from CStepper
		TInt StepL( TInt aStep );
	private:
		CBuildIndex();
	private:
		CDbTableSource* iSource;
		TDbPosition iNext;
		CDbRecordIndex* iIndex;
		};
protected:
	IMPORT_C CDbTableDatabase();
	IMPORT_C ~CDbTableDatabase();
public:
	inline RDbTransaction& Transaction();
//
	IMPORT_C CDbDatabase* InterfaceL();
	IMPORT_C CDbSource* SourceL();
//
	inline RDbTableSchema& Schema();
	IMPORT_C RDbTableSchema& SchemaL();
//
	CDbTableSource* TableSourceL( const TDesC& aTableName );
	void Release( const CDbTableDef& aDef );
// framework functions
	virtual CStepper* TableAlterL( CDbTableDef& aTable, const HDbColumnSet& aNewSet, TInt& aStep ) = 0;
	virtual CStepper* RecordDiscarderL( const CDbTableDef& aTable, TInt& aStep ) = 0;
	virtual CStepper* IndexDiscarderL( const CDbTableDef& aTable, const CDbTableIndexDef& anIndex, TInt& aStep ) = 0;
	IMPORT_C virtual CStepper* UtilityL( CDbDatabase::TUtility aType, TInt& aStep );
protected:
// framework
	IMPORT_C virtual TInt Property( CDbDatabase::TProperty aProperty );
private:
//framework
	virtual void AuthenticateL() = 0;
	IMPORT_C virtual void Idle();
	virtual void LoadSchemaL() = 0;
	virtual void SynchL( TDbLockType aLock ) = 0;
	virtual void Revert( TDbLockType aLock ) = 0;
	virtual CDbTable* TableL( const CDbTableDef& aDef ) = 0;
	virtual CDbTableDef* CreateTableL( const TDesC& aName, const CDbColSet& aColSet, const CDbKey* aPrimaryKey ) = 0;
	virtual CDbTableIndexDef* CreateIndexL( const CDbTableDef& aTable, const TDesC& aName, const CDbKey& aKey ) = 0;
	virtual void DestroyL() = 0;
//
	inline void Open();
	void Close();
	inline void AddTable( CDbTable& aTable );
	void RemoveTable( CDbTable& aTable );
//
	void CheckIdle();
	void FlushL( TDbLockType aLock );
	void Abandon( TDbLockType aLock );
	void Release();

	void DoCreateTableL( const TDesC& aName, const CDbColSet& aColSet, const CDbKey* aPrimaryKey );
	CIncremental* OpenCreateIndexL( const TDesC& aName, const TDesC& aTable, const CDbKey& aKey, TInt& aStep );
	CIncremental* OpenDropIndexL( const TDesC& aName, const TDesC& aTable, TInt& aStep );
	CIncremental* OpenDropTableL( const TDesC& aTable, TInt& aStep );
	CIncremental* OpenAlterTableL( const TDesC& aTable, const CDbColSet& aNewDef, TInt& aStep );
	CDbCursor* PrepareViewL( const TDbQuery& aQuery, const TDbWindow& aWindow, RDbRowSet::TAccess anAccess );
	CDbCursor* PrepareTableL( const TDesC& aTable, RDbRowSet::TAccess anAccess );
// reserved virtual function space
	IMPORT_C virtual void Reserved_1();
	IMPORT_C virtual void Reserved_2();
private:
	TInt iRef;
	RDbCache iCache;
	RDbTransaction iTransaction;
	RDbTableSchema iSchema;
	RDbTables iTables;
	};

#include "D32TABLE.INL"
#endif
