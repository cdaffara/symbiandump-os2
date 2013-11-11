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

#include <u32std.h>
#include <d32dbms.h>
#include "D32REC.H"
#include "D32TABLE.H"
#include "D32SQL.H"
#include <s32mem.h>

/**
@internalComponent
*/
enum TTablePanic
	{
	EDbInvalidColumn,
	EDbWrongType,
	EDbInUpdate,
	EDbNotInUpdate,
	EDbInvalidRow,
	EDbRowNotRead,
	EDbReadOnly,
	EDbTableOpen,
	EDbNotEvaluated,
	EDbStreamOpen,
	EDbRowSetConstraintMismatch,
	EDbBeginNestedTransaction,
	EDbUpdatesPendingOnCommit,
	EDbUpdatesPendingOnRollback,
	EDbNoCurrentTransaction,
	EDbStreamsPendingOnCommit,
	EDbStreamsPendingOnRollback,
	EDbInvalidBookmark
	};

/**
@internalComponent
*/
GLREF_C void Panic( TTablePanic aPanic );

class Validate
/**
@internalComponent
*/
	{
public:
	static void NameL( const TDesC& aName );
	static void ColSetL( const CDbColSet& aColSet );
	static void KeyL( const CDbKey& aKey, const HDbColumnSet& aColumns );
private:
	static void UniqueNameL( TDesC const** aNames, TInt aCount, const TDesC& aName );
	};

class CDbDataSource : public CBase
/**
@internalComponent
*/
	{
public:
	enum TGoto { ESuccess, ENoRow, EExhausted, ESynchFailure };
	enum TSynch { ESynch, ENoSynch };
	enum TDelete { EDeletedAtNext, EDeletedAtEnd, EDeletedInLimbo };
	enum TWrite { EReplace, EAppend };
protected:
	enum { EWorkToIterate = 1, EWorkToRead = 16 };
public:
	virtual RDbRow* RowBuffer() = 0;
	virtual TDbColumn Column( TDbColNo aColNo ) = 0;
	virtual void Reset() = 0;
	virtual TBool EvaluateL( TInt& aWork, TDbRecordId& aRecordId, TBool& aAtRow ) = 0;
	virtual TBool Unevaluated() = 0;
	virtual TInt CountL() = 0;
	virtual TGoto GotoL( TInt& aWork, TDbPosition aPosition, TDbRecordId& aRecordId ) = 0;
	TGoto GotoL( TDbPosition aPosition, TDbRecordId& aRecordId );
	virtual TBool GotoL( TDbRecordId aRecordId ) = 0;
	virtual void ReadRowL( TDbRecordId aRecordId ) = 0;
	virtual void NewRowL( TDbRecordId aCopyRecord ) = 0;
	virtual void PrepareToWriteRowL( TWrite aWrite ) = 0;
	virtual TDbRecordId WriteRowL( TWrite aWrite, TSynch aSynch = ESynch ) = 0;
	virtual TDelete DeleteRowL( TDbRecordId& aRecordId, TSynch aSynch = ESynch ) = 0;
	virtual TInt ColumnCount() const = 0;
	virtual const TDbColumnDef& ColumnDef( TDbColNo aCol ) const = 0;
//
	virtual void SetIndexL( const TDesC* anIndex ) = 0;
	virtual TBool SeekL( const TDbLookupKey& aKey, RDbTable::TComparison aComparison, TDbRecordId& aRecordId ) = 0;
	virtual CSqlSearchCondition* ParseConstraintLC( const TDesC& aCondition ) = 0;
protected:
	CDbDataSource() {};
	};

NONSHARABLE_CLASS(CDbTableSource) : public CDbDataSource
/**
@internalComponent
*/
	{
public:
	~CDbTableSource();
	CDbTableSource();
	void Construct( CDbTable* aTable );
	inline void SetIterator( CDbRecordIter* anIter );
	void ReverseIteratorL();
	inline const RDbTableRow& Row();
	inline CDbTable& Table();
//
	RDbRow* RowBuffer();
	TDbColumn Column( TDbColNo aColNo );
	void Reset();
	TBool EvaluateL( TInt& aWork, TDbRecordId& aRecordId, TBool& aAtRow );
	TBool Unevaluated();
	TInt CountL();
	TGoto GotoL( TInt& aWork, TDbPosition aPosition, TDbRecordId& aRecordId );
	TBool GotoL( TDbRecordId aRecordId );
	void NewRowL( TDbRecordId aCopyRecord );
	void PrepareToWriteRowL( TWrite aWrite );
	TDbRecordId WriteRowL( TWrite aWrite, TSynch aSynch );
	void ReadRowL( TDbRecordId aRecordId );
	TDelete DeleteRowL( TDbRecordId& aRecordId, TSynch aSynch );
	TInt ColumnCount() const;
	const TDbColumnDef& ColumnDef( TDbColNo aCol ) const;
	void SetIndexL( const TDesC* anIndex );
	TBool SeekL( const TDbLookupKey& aKey, RDbTable::TComparison aComparison, TDbRecordId& aRecordId );
	CSqlSearchCondition* ParseConstraintLC( const TDesC& aCondition );
private:
	TBool SynchL( TDbRecordId aRecordId );
	void OpenIteratorL();
private:
	CDbRecordIter* iIter;
	TDbGenerationMark iIterMark;
	RDbTableRow iRow;
	TBool iReset;
	};

NONSHARABLE_CLASS(CDbDataStage) : public CDbDataSource
/**
@internalComponent
*/
	{
public:
	~CDbDataStage();
	inline void SetSource( CDbDataSource* aSource );
protected:
	CDbDataStage() {};
	inline CDbDataSource& Source();
	inline const CDbDataSource& Source() const;
	RDbRow* RowBuffer();
	TDbColumn Column( TDbColNo aColNo );
	void Reset();
	TBool EvaluateL( TInt& aWork, TDbRecordId& aRecordId, TBool& aAtRow );
	TBool Unevaluated();
	TInt CountL();
	TGoto GotoL( TInt& aWork, TDbPosition aPosition, TDbRecordId& aRecordId );
	TBool GotoL( TDbRecordId aRecordId );
	void ReadRowL( TDbRecordId aRecordId );
	void NewRowL( TDbRecordId aCopyRecord );
	void PrepareToWriteRowL( TWrite aWrite );
	TDbRecordId WriteRowL( TWrite aWrite, TSynch aSynch );
	TDelete DeleteRowL( TDbRecordId& aRecordId, TSynch aSynch );
	TInt ColumnCount() const;
	const TDbColumnDef& ColumnDef( TDbColNo aCol ) const;
	void SetIndexL( const TDesC* anIndex );
	TBool SeekL( const TDbLookupKey& aKey, RDbTable::TComparison aComparison, TDbRecordId& aRecordId );
	CSqlSearchCondition* ParseConstraintLC( const TDesC& aCondition );
private:
	CDbDataSource* iSource;
	};

NONSHARABLE_CLASS(CDbRestrictStage) : public CDbDataStage
/**
@internalComponent
*/
	{
public:
	CDbRestrictStage( TDbTextComparison aComparison );
	~CDbRestrictStage();
	inline void SetRestriction( CSqlSearchCondition* aRestriction );
private:
	inline CSqlSearchCondition& Restriction() const;
	TBool FilterRowL( TDbRecordId aRecordId );
//
	TInt CountL();
	TGoto GotoL( TInt& aWork, TDbPosition aPosition, TDbRecordId& aRecordId );
	TBool GotoL( TDbRecordId aRecordId );
	TDelete DeleteRowL( TDbRecordId& aRecordId, TSynch aSynch );
private:
	CSqlSearchCondition* iRestriction;
	const TTextOps& iTextOp;
	};

class HDbColumnMap;

NONSHARABLE_CLASS(CDbProjectStage) : public CDbDataStage
/**
implement restricted column selection
@internalComponent
*/
	{
public:
	CDbProjectStage();
	~CDbProjectStage();
	void ConstructL( const RSqlColumnList& aSelect, const HDbColumnSet& aColumns );
private:
	RDbRow* RowBuffer();
	TDbColumn Column( TDbColNo aColNo );
	TInt ColumnCount() const;
	const TDbColumnDef& ColumnDef( TDbColNo aCol ) const;
private:
	HDbColumnMap* iMap;
	};

NONSHARABLE_CLASS(CDbBasicWindowStage) : public CDbDataStage
/**
@internalComponent
*/
	{
private:
	enum { EWindowArrayGranularity = 32 };	// 128 byte segments
public:
	CDbBasicWindowStage( const TDbWindow& aWindow );
protected:
// from CDbDataStage
	void Reset();
	TBool EvaluateL( TInt& aWork, TDbRecordId& aRecordId, TBool& aAtRow );
	TInt CountL();
	TGoto GotoL( TInt& aWork, TDbPosition aPosition, TDbRecordId& aRecordId );
	TBool GotoL( TDbRecordId aRecordId );
	void ReadRowL( TDbRecordId aRecordId );
	TDbRecordId WriteRowL( TWrite aWrite, TSynch aSynch );
	TDelete DeleteRowL( TDbRecordId& aRecordId, TSynch aSynch );
private:
	TBool GetRecord( TDbRecordId& aRecordId );
	TInt Find( TDbRecordId aRecordId, TInt& aPos );
	virtual TBool DoEvaluateL( TInt& aWork ) = 0;
protected:
	TDbWindow iWindow;		// the window shape
	CArrayFixSeg<TDbRecordId> iRecords;	// the window itself
	TInt iPos;				// the iterator over the window
	};

NONSHARABLE_CLASS(CDbWindowStage) : public CDbBasicWindowStage
/**
@internalComponent
*/
	{
private:
	enum TView { EBeginning, EMiddle, EEnd, EAll };
	enum TIterPos { EAtBeginning, EAtEnd };
public:
	CDbWindowStage( const TDbWindow& aWindow );
protected:
	void Reset();
	TBool DoEvaluateL( TInt& aWork );
	TBool Unevaluated();
private:
	TInt WhatToEvaluate();
	TDbPosition ResetIterToBeginningL();
	TDbPosition ResetIterToEndL();
	TDbPosition SetIteratorL( TInt anEval );
	void ExtendAtBeginningL( TInt aRecords, TDbPosition aFirst, TInt& aWork );
	void ExtendAtEndL( TInt aRecords, TDbPosition aFirst, TInt& aWork );
private:
	TIterPos iIterPos;		// where the underlying iterator is in our window
	TView iView;			// what part of the underlying set we see through the window
	};

NONSHARABLE_CLASS(CDbReorderWindowStage) : public CDbBasicWindowStage
/**
@internalComponent
*/
	{
private:
	enum TState { EReset, EEvaluate, EComplete, EFailed };
	enum { EGranularity = 32 };
public:
	CDbReorderWindowStage();
	~CDbReorderWindowStage();
protected:
	void Reset();
	TBool DoEvaluateL( TInt& aWork );
	TBool Unevaluated();
private:
	TBool ReadL( TInt& aWork, TDbPosition aNext );
private:
	TState iState;
	RArray<TUint> iRows;
	};

NONSHARABLE_CLASS(CDbOrderByStage) : public CDbBasicWindowStage
/**
@internalComponent
*/
	{
private:
	class HOrdering;
	class CKeys;
	enum TState { EReset, EEvaluate, ESort, EAdd, EComplete, EFailed };
public:
	CDbOrderByStage( const RDbTableRow& aRow );
	~CDbOrderByStage();
	void ConstructL( const CDbKey& aOrderBy );
protected:
	void Reset();
	TBool DoEvaluateL( TInt& aWork );
	TBool Unevaluated();
private:
	inline const RDbTableRow& Row();
	inline CKeys& Keys();
	inline const HOrdering& Order();
//
	TBool ReadL( TInt& aWork, TDbPosition aNext );
private:
	const RDbTableRow& iRow;
	TState iState;
	HOrdering* iOrder;
	CKeys* iKeys;
	};

class RDbAccessPlan
/**
@internalComponent
*/
	{
public:
	inline RDbAccessPlan( CSqlQuery* aQuery, TDbTextComparison aComparison );
	inline RDbAccessPlan();
	void BuildLC( CDbTableDatabase& aDatabase, RDbRowSet::TAccess aAccess, const TDbWindow& aWindow );
	void BuildLC( CDbTableDatabase& aDatabase, const TDesC& aTable, RDbRowSet::TAccess aAccess );
	inline CDbTable& Table();
	inline CDbDataSource& Source();
	inline CDbDataSource* Adopt();
private:
	class TPlan
		{
	public:
		enum 
			{
			EOrder = 0x1, ERestrict = 0x2, EReorder = 0x4,
			EIndex = 0x8, EBounded = 0x10,
			EWindow = 0x20, EReverse = 0x40,
			EMask = 0x1f
			};
		enum TType 
			{
			EPlanA = 0, EPlanB = 0, EPlanC = 0, EPlanD = 0, EPlanEF = 1, EPlanGH = 7, EPlanIJ = 2,
			EPlanKL = 5, EPlanMN = 4, EPlanOP = 3, EPlanQR = 6, EPlanST = 8
			};
	public:
		inline TPlan();
		inline TBool operator!=( const TPlan& aPlan ) const;
		TType Type() const;
		static TInt OrderByPlan( const TPlan& aLeft, const TPlan& aRight );
	public:
		TUint iFlags;
		const CDbTableIndexDef* iIndex;
		CSqlCompPredicate* iLower;
		CSqlCompPredicate* iUpper;		
		};	
private:
	CDbTableSource* TableLC( CDbTableDatabase& aDatabase, const TDesC& aTable );
	void Insert( CDbDataStage* aStage );
	CDbRecordIter* IteratorL( const TPlan& aPlan );
	CDbRecordIter* BoundedIteratorL( const TPlan& aPlan );
	void RestrictionL();
	void OrderByL( const RDbTableRow& aRowBuf );
	void ProjectionL();
	void WindowL( const TPlan& aPlan, const TDbWindow& aWindow );
	static void Cleanup( TAny* aPtr );
	TBool IsBoundedIteratorL( TPlan& aPlan, const CDbTableIndexDef* aIndex );
	TBool IsIndexIteratorL( TPlan& aPlan, const CDbTableIndexDef* aIndex );
	void EvaluateWindowStage( TPlan& aPlan );
	void EvaluateReorderStage( TPlan& aPlan, const CDbTableIndexDef* aIndex );
	void EvaluatePlansL();
	void ChoosePlanL( TPlan& aPlan );
	void PrepareQueryL( CDbTableSource* aSource );
	TInt GetTightestRestrictionL( TPlan& aPlan, TPlan::TType aType );
	TInt GetSmallestKeySize( TPlan& aPlan, TPlan::TType aType );
	TInt IndexSpanL( const TPlan& aPlan );
	void ReducePlans();
	void CreateTableIteratorPlan( TPlan& aPlan );
	TUint FindMatchL( const CDbTableIndexDef* aIndex );
	TBool TextKeyL( const CDbKey& anOrder );
private:
	enum { EGranularity = 4 };
	enum { EMatch = 0x1, EReverse = 0x2, ETruncated = 0x4 };
	//
	class TBounds;
	class CDbCompPredicateList;
private:
	RArray<TPlan> iPlans;
	CDbDataSource* iSource;
	CDbTable* iTable;
	TDbTextComparison iComparison;
	CSqlQuery* iQuery;
	TBool iWindow;
	RDbRowSet::TAccess iAccess;
	};

class RDbAccessPlan::TBounds
	{
public:
	TBounds( const TPlan& aPlan );
private:
	void GetLookupKey( const CSqlCompPredicate& aCompPredicate, TDbLookupKey& aLookup );
	void SetLowerBounds();
	void SetUpperBounds();
public:
	CSqlCompPredicate* iLowerPred;
	CSqlCompPredicate* iUpperPred;		
	TDbLookupKey iLowerKey;
	TDbLookupKey iUpperKey;
	TDbLookupKey* iLower;
	TDbLookupKey* iUpper;
	TUint iInclusion;
	};

class RDbAccessPlan::CDbCompPredicateList : public CArrayFixFlat<CSqlCompPredicate*>
	{
public:
	enum
		{
		EAnd = 0x1,
		ELess = 0x2,
		ELessEqual = 0x4,
		EEqual = 0x8,
		EGreaterEqual = 0x10,
		EGreater = 0x20,
		ECompPred = ELess | ELessEqual | EEqual | EGreaterEqual | EGreater,
		ENone = 0
		};
public:
	static CDbCompPredicateList* NewLC( CSqlQuery& aQuery, TDbTextComparison aComparison, const CDbTableDef& aTableDef );
	CSqlCompPredicate* CompPredicate( TDbColNo aColNo, TUint aType = ECompPred );
	TUint Type( CSqlSearchCondition::TType aType ) const;
	inline TBool IsRestriction();
private:
	inline CDbCompPredicateList( const CDbTableDef& aTableDef, TDbTextComparison aComparison );
	void ConstructL( CSqlSearchCondition& aSeachCondition );
	TBool IsIndexed( const TDesC& aColumnName );
private:
	enum { EGranularity = 4 };
private:
	const CDbTableDef& iTableDef;
	TDbTextComparison iComparison;
	TBool iRestriction;
	};

class CDbTable::TValid
/**
@internalComponent
*/
	{
public:
	TValid(CDbTable& aTable);
	TBool Reset();
//
	inline operator TAny*() const;
	void CheckL() const;
//
	inline RDbTransaction& Transaction() const;
	inline CDbTable& Table() const;
private:
	CDbTable& iTable;
	TDbGenerationMark iRollback;
	};

NONSHARABLE_CLASS(CDbTableCursor) : public CDbCursor
/**
@internalComponent
*/
	{
public:
	static CDbTableCursor* NewL( RDbAccessPlan& aPlan, RDbRowSet::TAccess aAccess );
//
	inline CDbBlobSpace& BlobsL() const;
	inline void AddSink();
	inline void ReleaseSink();
	inline void AddSource();
	inline void ReleaseSource();
	void AddBlobSource();
	void ReleaseBlobSource();
private:
	CDbTableCursor( RDbAccessPlan& aPlan, RDbRowSet::TAccess anAccess );
	~CDbTableCursor();
//
	void Reset();
	TBool EvaluateL();
//	void Evaluate( TRequestStatus& aStatus );
	TBool Unevaluated();
//
	void SetIndexL( const TDesC* anIndex );
	TBool SeekL( const TDbLookupKey& aKey, RDbTable::TComparison aComparison );
	CDbRowConstraint* OpenConstraintL( const TDbQuery& aCriteria );
	TBool MatchL( CDbRowConstraint& aConstraint );
//
	TInt CountL( RDbRowSet::TAccuracy aAccuracy );
	TBool AtBeginning();
	TBool AtEnd();
	TBool AtRow();
	TBool GotoL( RDbRowSet::TPosition aPosition );
	void Bookmark( TDbBookmark::TMark& aMark );
	void GotoL( const TDbBookmark::TMark& aMark );
	void GetL();
	void InsertL( TInsert aClearRow );
	void UpdateL();
	void Cancel();
	void PutL();
	void DeleteL();
//
	TInt ColumnCount();
	TDbColType ColumnType( TDbColNo aCol );
	void ColumnDef( TDbCol& aCol, TDbColNo aColNo );
//
	RDbRow* RowBuffer();
	TDbColumnC ColumnC( TDbColNo aCol );	// non-writeable row buffer
	TDbColumn Column( TDbColNo aCol );		// writeable row buffer
	void SetNullL( TDbColNo aCol );
	TInt ColumnSize( TDbColNo aCol );
	MStreamBuf* ColumnSourceL( TDbColNo aCol );
	MStreamBuf* ColumnSinkL( TDbColNo aCol );
//
	TDbColType Type( TDbColNo aCol ) const;
	void ReplaceBlobL( TDbColumn& aColumn );
	inline TBool InUpdate() const;
	inline RDbTransaction& Transaction();
	void CheckStateL() const;
	void CheckReadL() const;
	void CheckUpdateL() const;
	void AssertValidRow() const;
	void AssertNotInUpdate() const;
	void AssertInUpdate() const;
	void AssertNoStreams() const;
private:
	enum TState
		{
		ERowBeginning,
		ERowOK,
		ERowEnd,
		ERowInvalid,
		ERowDeletedAtNext,
		ERowDeletedAtEnd,
		ERowInLimbo
		};	// keep these in same order as CDbDataSource::TDelete
	enum
		{
		EUpdatable = 0x01,
		EReadable = 0x02,
		ERead = 0x04,
		EUpdating = 0x08,
		EInserting = 0x10,
		EDirty = 0x20,
		EWriteBuf = 0x40
		};
	enum { EMaxReadBuf = 255 };
	class HColumns;
	class CConstraint;
	class HMemBuf;
	class HWriteBuf;
	class HReadBuf;
	class HHeapBuf;
private:
	TUint8 iState;
	TUint8 iFlags;
	TUint8 iReadBuf;
	CDbTable::TValid iValid;
	CDbDataSource* iDataSource;
	TDbRecordId iRecord;
	HColumns* iColumns;
	};


NONSHARABLE_CLASS(CDbTableCursor::HWriteBuf) : public TDesBuf
	{
public:
	static HWriteBuf* NewL( CDbTableCursor& aCursor, const TDbColumn& aColumn, TDbColType aType );
private:
	inline HWriteBuf( CDbTableCursor& aCursor, const TDbColumn& aColumn, TDbColType aType );
	virtual inline ~HWriteBuf();
	void ConstructL();
//
	inline TBool IsBinary() const;
	void FlipL();
// for MStreamBuf
	void DoRelease();
	void DoSynchL();
	TInt DoReadL( TAny* aPtr, TInt aMaxLength );
	TStreamTransfer DoReadL( MStreamInput& aInput, TStreamTransfer aTransfer );
	void DoWriteL( const TAny* aPtr, TInt aLength );
	TStreamTransfer DoWriteL( MStreamOutput& aOutput, TStreamTransfer aTransfer );
	TStreamPos DoSeekL( TMark aMark, TStreamLocation aLocation, TInt aOffset );
private:
	CDbTableCursor& iCursor;
	TDbColumn iColumn;
	TDbColType iType;
	TDbBlob* iBlob;
	TPtr8 iInlineBuf;
	TDbBlobId iBlobId;
	TInt iSize;
	MStreamBuf* iOverflowBuf;
	};

NONSHARABLE_CLASS(CDbTableCursor::HMemBuf) : public TMemBuf
	{
public:
	static HMemBuf* NewL( CDbTableCursor& aCursor, const TDesC8& aDes );
protected:
	HMemBuf( CDbTableCursor& aCursor );
	virtual inline ~HMemBuf();
private:
	void DoRelease();
private:
	CDbTableCursor& iCursor;
	};

class CDbTableCursor::HHeapBuf : public CDbTableCursor::HMemBuf
	{
public:
	enum { EMaxBlobBuffer = 0x600 };	// 1.5K to match KDbsStreamBufSize
public:
	static HHeapBuf* NewL( CDbTableCursor& aCursor, const TDbBlob& aBlob, TDbColType aType );
private:
	inline HHeapBuf( CDbTableCursor& aCursor );
private:
	TUint8 iBuf[1];
	};

NONSHARABLE_CLASS(CDbTableCursor::HReadBuf) : public MStreamBuf
	{
public:
	static HReadBuf* NewLC( CDbTableCursor& aCursor );
	inline void Set( MStreamBuf* aHost );
private:
	inline HReadBuf( CDbTableCursor& aCursor );
	virtual inline ~HReadBuf();
// from MStreamBuf
	void DoRelease();
	TInt DoReadL( TAny* aPtr, TInt aMaxLength );
	TStreamTransfer DoReadL( MStreamInput& aInput, TStreamTransfer aTransfer );
	TStreamPos DoSeekL( TMark aMark, TStreamLocation aLocation, TInt aOffset );
private:
	CDbTableCursor& iCursor;
	MStreamBuf* iHost;
	};

NONSHARABLE_CLASS(CDbTableDatabase::CIncremental) : public CDbSyncIncremental
/**
@internalComponent
*/
	{
protected:
	enum { ELastStep = 1 };
	enum TState { ERunning = 0, EFailed, ECommitted };
public:
	inline void Construct( CStepper* aStepper );
protected:
	CIncremental( RDbTransaction& aTransaction );
	~CIncremental();
	inline RDbTransaction& Transaction();
	inline CDbTableDatabase& Database();
	inline TBool IsCommitted() const;
private:
	TBool NextL( TInt& aStep );
	virtual TInt DoNextL( TInt aStep );
	virtual void DoLastL() =0;
private:
	RDbTransaction& iTransaction;
	CStepper* iStepper;
	TState iState;
	};

NONSHARABLE_CLASS(CDbTableDatabase::CIncrementalDDL) : public CDbTableDatabase::CIncremental
/**
@internalComponent
*/
	{
protected:
	CIncrementalDDL( RDbTransaction& aTransaction );
	~CIncrementalDDL();
// from CIncremental
	void DoLastL();
	};

class CDbTableDatabase::CCreateIndex : public CDbTableDatabase::CIncrementalDDL
/**
@internalComponent
*/
	{
public:
	static CCreateIndex* NewLC( RDbTransaction& aTransaction );
	TInt ConstructL( const CDbTableDef& aTable, const CDbTableIndexDef& anIndex );
private:
	inline CCreateIndex( RDbTransaction& aTransaction );
	};

NONSHARABLE_CLASS(CDbTableDatabase::CDropIndex) : public CDbTableDatabase::CIncrementalDDL
/**
@internalComponent
*/
	{
public:
	static CDropIndex* NewL( RDbTransaction& aTransaction, const CDbTableDef& aTable, CDbTableIndexDef* anIndex, TInt& aStep );
	~CDropIndex();
private:
	inline CDropIndex( RDbTransaction& aTransaction );
private:
	CDbTableIndexDef* iDef;
	};

NONSHARABLE_CLASS(CDbTableDatabase::CDropTable) : public CDbTableDatabase::CIncrementalDDL
/**
@internalComponent
*/
	{
public:
	static CDropTable* NewL( RDbTransaction& aTransaction, CDbTableDef* aTable, TInt& aStep );
	~CDropTable();
private:
	inline CDropTable( RDbTransaction& aTransaction );
private:
	CDbTableDef* iDef;
	};

NONSHARABLE_CLASS(CDbTableDatabase::CAlterTable) : public CDbTableDatabase::CIncrementalDDL
/**
@internalComponent
*/
	{
public:
	static CAlterTable* NewL( RDbTransaction& aTransaction, CDbTableDef& aTable, const CDbColSet& aNewDef, TInt& aStep );
	~CAlterTable();
private:
	inline CAlterTable( RDbTransaction& aTransaction, CDbTableDef& aDef );
	void ConstructL( const CDbColSet& aNewDef, TInt& aStep );
// from CIncremental
	void DoLastL();
private:
	CDbTableDef& iDef;
	HDbColumnSet* iNewSet;
	};

NONSHARABLE_CLASS(CDbTableDatabase::CUtility) : public CDbTableDatabase::CIncremental
/**
@internalComponent
*/
	{
public:
	static CUtility* NewL( RDbTransaction& aTransaction, CDbDatabase::TUtility aType, TInt& aStep );
	~CUtility();
private:
	inline CUtility( RDbTransaction& aTransaction, CDbDatabase::TUtility aType );
private:
	void DoLastL();
	};

NONSHARABLE_CLASS(CDbIncrementalDML) : public CDbSyncIncremental
/**
@internalComponent
*/
	{
public:
	static CDbIncrementalDML* NewL( CSqlModifyStatement& aStatement, CDbTableDatabase& aDatabase, TDbTextComparison aComparison );
	~CDbIncrementalDML();
private:
	enum TState { EInitialising = 0, EEvaluating, EUpdating, EDeleting, ECommitted, EFailed };
private:
	inline CDbIncrementalDML( RDbAccessPlan& aPlan );
	inline RDbTransaction& Transaction();
	inline TBool IsUpdate() const;
	inline void	SetState( TState aState );
// from CDbSyncIncremental
	TBool NextL( TInt& aRows );
private:
	TState iState;
	CDbTable& iTable;
	CDbDataSource* iSource;
	TDbRecordId iRecord;
	CSqlValues* iValues;
	};

NONSHARABLE_CLASS(CDbTableDatabase::CInterface) : public CDbDatabase
/**
the interface implementation class for the Table database framework
@internalComponent
*/
	{
public:
	CInterface(CDbTableDatabase& aDatabase);
private:
	~CInterface();
	inline CDbTableDatabase& Database();
	void PrepareDDLL();
//	from the interface framework
	TInt Destroy();
	TInt Begin();
	TInt Commit();
	void Rollback();
	TInt Property( TProperty aProperty );
	void CreateTableL( const TDesC& aName, const CDbColSet& aColSet, const CDbKey* aPrimaryKey );
	void TablesL( CDbTableNames& aNames );
	void ColumnsL( CDbColSet& aColSet, const TDesC& aName );
	void IndexesL( CDbIndexNames& aNames, const TDesC& aTable );
	void KeysL( CDbKey& aKey, const TDesC& aName, const TDesC& aTable );
	CDbNotifier* OpenNotifierL();
	CDbCursor* PrepareViewL( const TDbQuery& aQuery, const TDbWindow& aWindow, RDbRowSet::TAccess anAccess );
	CDbCursor* OpenTableL( const TDesC& aName, RDbRowSet::TAccess anAccess );
	CDbIncremental* OpenDropTableL( const TDesC& aTable, TInt& aStep );
	CDbIncremental* OpenAlterTableL( const TDesC& aTable, const CDbColSet& aNewDef, TInt& aStep );
	CDbIncremental* OpenCreateIndexL( const TDesC& aName, const TDesC& aTable, const CDbKey& aKey, TInt& aStep );
	CDbIncremental* OpenDropIndexL( const TDesC& aName, const TDesC& aTable, TInt& aStep );
	CDbIncremental* OpenUtilityL( CDbDatabase::TUtility aType, TInt& aStep );
	CDbIncremental* OpenExecuteL( const TDesC& aSql, TDbTextComparison aComparison, TInt& aInit );
private:
	CDbTableDatabase& iDatabase;
	};

NONSHARABLE_CLASS(CDbTableDatabase::CSource) : public CDbSource
/**
@internalComponent
*/
	{
public:
	CSource( CDbTableDatabase& aDatabase );
private:
	~CSource();
	inline CDbTableDatabase& Database();
	// from the framework
	CDbDatabase* AuthenticateL();
	CDbNotifier* OpenNotifierL();
private:
	CDbTableDatabase& iDatabase;
	};

#include "UT_STD.INL"
