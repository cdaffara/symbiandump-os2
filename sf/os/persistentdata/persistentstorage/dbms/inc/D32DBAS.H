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

#if !defined(__D32DBAS_H__)
#define __D32DBAS_H__
#if !defined(__D32DBMS_H__)
#include <d32dbms.h>
#endif

// other classes referenced
class RFs;
class CDbContext;

// classes defined
class TTextOps;
class TDbBlob;
class TDbCell;
class TDbColumnC;
class TDbColumn;
class RDbRow;
class CDbObject;
class CDbRowConstraint;
class CDbCursor;
class CDbNotifier;
class CDbIncremental;
class CDbSyncIncremental;
class CDbAsyncIncremental;
class CDbDatabase;
class CDbSource;
class TDbDriver;
class TDbFormat;

/**
@internalComponent
*/
const TInt KDbMaxFormatName=32;
const TInt KDbMaxDatabaseName=KMaxFileName;

/**
@internalComponent
*/
const TInt KDbMaxInlineBlobSize=255;

/**
@internalComponent
*/
typedef TUint32 TDbBlobId;

/**
@internalComponent
*/
const TDbBlobId KDbNullBlobId=0;

/**
@internalComponent
*/
class Dbms
	{
public:
	IMPORT_C static void Panic(const TDesC& aCategory,TInt aCode);
	};

/**
@internalComponent
*/
class TTextOps
	{
public:
	IMPORT_C static const TTextOps& Ops(TDbTextComparison aType=EDbCompareNormal);
//
	inline TUint Fold(TUint aChar) const;
	IMPORT_C TInt Compare(const TDesC8& aLeft,const TDesC8& aRight) const;
	IMPORT_C TInt Compare(const TDesC16& aLeft,const TDesC16& aRight) const;
	inline TInt Compare(const TText8* aLeftPtr,TInt aLeftLen,const TText8* aRightPtr,TInt aRightLen) const;
	inline TInt Compare(const TText16* aLeftPtr,TInt aLeftLen,const TText16* aRightPtr,TInt aRightLen) const;
	inline TInt Match(const TDesC8& aDes,const TDesC8& aPattern) const;
	inline TInt Match(const TDesC16& aDes,const TDesC16& aPattern) const;
	IMPORT_C TInt Match(const TText8* aPtr,TInt aLen,const TText8* aPatternPtr,TInt aPatternLen) const;
	IMPORT_C TInt Match(const TText16* aPtr,TInt aLen,const TText16* aPatternPtr,TInt aPatternLen) const;
	IMPORT_C TInt Find(const TDesC8& aDes,const TDesC8& aSub) const;
	IMPORT_C TInt Find(const TDesC16& aDes,const TDesC16& aSub) const;
	IMPORT_C TInt Find(const TText8* aPtr,TInt aLen,const TText8* aSubPtr,TInt aSubLen) const;
	IMPORT_C TInt Find(const TText16* aPtr,TInt aLen,const TText16* aSubPtr,TInt aSubLen) const;
	TInt Order(const TDesC16& aLeft, const TDesC16& aRight) const;
	inline TInt Order(const TText16* aLeftPtr, TInt aLeftLen, const TText16* aRightPtr, TInt aRightLen) const;
public:
	TUint (*iFold)(TUint aChar);
	TInt (*iCompare8)(const TText8*,TInt,const TText8*,TInt);
	TInt (*iMatch8)(const TDesC8&,const TDesC8&);
	TInt (*iFind8)(const TDesC8&,const TText8*,TInt);
	TInt (*iCompare16)(const TText16*,TInt,const TText16*,TInt);
	TInt (*iMatch16)(const TDesC16&,const TDesC16&);
	TInt (*iFind16)(const TDesC16&,const TText16*,TInt);
	TInt (*iOrder16)(const TText16*,  TInt, const TText16*, TInt);
	};

/**
@internalComponent
*/
class TDbBlob
	{
public:
	inline TDbBlob();
	inline TDbBlob(TDbBlobId anId,TInt aSize=KDbUndefinedLength);
	inline TDbBlob(const TUint8* aPtr,TInt aSize);
	inline TUint8* InlineBuffer();
	inline void SetId(TDbBlobId anId);
	inline void SetSize(TInt aSize);
//
	inline TBool IsInline() const;
	inline TInt Size() const;
	inline const TUint8* Data() const;
	inline TPtrC8 PtrC8() const;
	inline TPtrC16 PtrC16() const;
	inline TDbBlobId Id() const;
//
	inline TInt CellSize() const;
	static inline TInt InlineSize(TInt aSize);
	static inline TInt RefSize();
private:
	TDbBlobId iId;
	TInt iSize;
	TUint8 iInline[KDbMaxInlineBlobSize];
	};

/**
@internalComponent
*/
class TDbCell
	{
public:
	static inline TInt Size(TInt aLength);
	inline TInt Size() const;			// total cell size
	inline TDbCell* Next();				// pointer to next cell
	inline const TDbCell* Next() const;	// pointer to next cell
	inline TInt Length() const;			// data size
	inline void SetLength(TInt aLength);
	inline TAny* Data();				// data pointer
	inline const TAny* Data() const;	// data pointer
private:
	TInt iLength;
	TUint8 iBuf[1];
	};

/**
@internalComponent
*/
class TDbColumnC
	{
	friend class TDbColumn;
public:
	inline TDbColumnC(const RDbRow& aRow,TDbColNo aCol);
	inline TDbColumnC(const TDbColumn& aCol);
	inline TInt Size() const;
	inline TBool IsNull() const;
	inline TInt8 Int8() const;
	inline TInt16 Int16() const;
	inline TInt32 Int32() const;
	inline const TInt64& Int64() const;
	inline TUint8 Uint8() const;
	inline TUint16 Uint16() const;
	inline TUint32 Uint32() const;
	inline TReal32 Real32() const;
	inline const TReal64& Real64() const;
	inline TPtrC8 PtrC8() const;
	inline TPtrC16 PtrC16() const;
	inline const TTime& Time() const;
	inline const TDbBlob& Blob() const;
private:
	const TDbCell* iCell;
	};

/**
@internalComponent
*/
class TDbColumn
	{
	friend class TDbColumnC;
public:
	inline TDbColumn();
	inline TDbColumn(RDbRow& aRow,TDbColNo aCol);
	inline void SetNull();
	inline void SetL(TInt32 aValue);
	IMPORT_C void SetL(TUint32 aValue);
	IMPORT_C void SetL(TInt64 aValue);
	IMPORT_C void SetL(TReal32 aValue) __SOFTFP;
	IMPORT_C void SetL(TReal64 aValue) __SOFTFP;
	IMPORT_C void SetL(const TDesC8& aValue);
	IMPORT_C void SetL(const TDesC16& aValue);
	inline void SetL(const TTime& aValue);
	IMPORT_C void SetBlobL(TDbBlobId aBlobId,TInt aSize=KDbUndefinedLength);
	IMPORT_C void SetBlobL(const TUint8* aData,TInt aSize);
//
	IMPORT_C TDbBlob& InitBlobL();
	IMPORT_C void CommitBlob(const TDbBlob& aBlob);
//
	inline void SetL(const TDbCell* aCell);
	inline void SetL(const TDbColumnC& aColumn);
private:
	inline RDbRow& Row() const;
	IMPORT_C void SetL(const TAny* aPtr,TInt aSize);
private:
	RDbRow* iRow;
	TDbColNo iColumn;
	};

/**
@internalComponent
*/
class RDbRow
	{
	friend class TDbColumn;
public:
	IMPORT_C RDbRow();
	inline RDbRow(TAny* aBuf,TInt aMaxSize);
	inline RDbRow(TAny* aBuf,TInt aSize,TInt aMaxSize);
	inline void Open(TAny* aBuf,TInt aMaxSize);
	IMPORT_C void Open(TAny* aBuf,TInt aSize,TInt aMaxSize);
	IMPORT_C void CreateL(TInt aMaxSize);
	void PushL();
	IMPORT_C void Close();
	inline void Reset();
//
	inline TDbCell* First();
	inline const TDbCell* First() const;
	inline const TDbCell* Last() const;
	inline const TDbCell* End() const;
	IMPORT_C void SetSize(TInt aSize);
	inline TInt Size() const;
	inline TInt MaxSize() const;
	IMPORT_C void GrowL(TInt aMaxSize);
// for use by data sources
	static inline TInt Diff(const TDbCell* aBegin,const TDbCell* aEnd);
	IMPORT_C const TDbCell* ColCell(TDbColNo aCol) const;
private:
	IMPORT_C TAny* SetColumnWidthL(TDbColNo aCol,TInt aWidth);
	TDbCell* Column(TDbColNo aCol) const;
	TInt ReallocL(TInt aMaxSize);
	void ExtendL(TInt aAdjust);	// change the row buffer length
//
	inline TBool Owned() const;
	inline TInt Column() const;
	inline void SetCache(TDbCell* aCell,TInt aColumn);
private:
	enum {EOwned=0x80000000};
	enum {EGranularity=0x100};
private:
	TDbCell* iFirst;	// first column
	TDbCell* iLast;		// pointer past last cell
	TDbCell* iEnd;		// pointer past end of available buffer
	TDbCell* iCell;		// cached cell pointer
	TUint iColumn;		// cached column number/ownership flag
	};


/**
@internalComponent
*/
class CDbObject : public CBase
	{
	friend class CDbContext;
public:
	static void Destroy(CDbObject* aObject);	// context-safe deletion
	CDbObject* Attach(CDbObject* aObject);
	inline const CDbContext* Context() const;
//
	void PushL();
protected:
	inline CDbObject() {}
private:
	CDbContext* iContext;
	};

/**
@internalComponent
*/
class CDbRowConstraint : public CDbObject
	{
protected:
	CDbRowConstraint() {}
	};

/**
@internalComponent
*/
class CDbCursor : public CDbObject
	{
public:
	enum TInsert {EClear,ECopy};
protected:
	CDbCursor() {}
public:
// Pure virtual
	virtual void Reset() =0;
	virtual TBool EvaluateL()=0;
	TInt Evaluate();
	virtual void Evaluate(TRequestStatus& aStatus)=0;
	virtual TBool Unevaluated()=0;
//
	virtual void SetIndexL(const TDesC* anIndex)=0;
	virtual TBool SeekL(const TDbLookupKey& aKey,RDbTable::TComparison aComparison) =0;
	CDbRowConstraint* ConstraintL(const TDbQuery& aCriteria);
	virtual TBool MatchL(CDbRowConstraint& aConstraint) =0;
	IMPORT_C virtual TInt FindL(RDbRowSet::TDirection aDirection,const TDbQuery& aCriteria);
//
	virtual TBool AtBeginning()=0;
	virtual TBool AtEnd()=0;
	virtual TBool AtRow()=0;
	IMPORT_C virtual TInt CountL(RDbRowSet::TAccuracy aAccuracy);
	virtual TBool GotoL(RDbRowSet::TPosition aPosition)=0;
	virtual void Bookmark(TDbBookmark::TMark& aMark) =0;
	virtual void GotoL(const TDbBookmark::TMark& aMark) =0;
	virtual void GetL()=0;
	virtual void InsertL(TInsert aClearRow)=0;
	virtual void UpdateL()=0;
	virtual void Cancel()=0;
	virtual void PutL()=0;
	virtual void DeleteL()=0;
// column validation
	virtual TInt ColumnCount()=0;
	IMPORT_C virtual void ColumnsL(CDbColSet& aColSet);
	virtual void ColumnDef(TDbCol& aCol,TDbColNo aColNo)=0;
	virtual TDbColType ColumnType(TDbColNo aCol)=0;
// Column Accessors
	virtual TDbColumnC ColumnC(TDbColNo aCol)=0;	// non-writeable column
	virtual TDbColumn Column(TDbColNo aCol)=0;		// writeable column
	virtual void SetNullL(TDbColNo aCol)=0;
	virtual TInt ColumnSize(TDbColNo aCol)=0;
// Optimized whole row access for server
	virtual RDbRow* RowBuffer() =0;
// Blob functions
	virtual MStreamBuf* ColumnSourceL(TDbColNo aCol)=0;
	virtual MStreamBuf* ColumnSinkL(TDbColNo aCol)=0;
private:
	virtual CDbRowConstraint* OpenConstraintL(const TDbQuery& aCriteria) =0;
// Reserved virtuals
	IMPORT_C virtual void Reserved_1();
	IMPORT_C virtual void Reserved_2();
	};

/**
@internalComponent
*/
class CDbIncremental : public CDbObject
	{
protected:
	CDbIncremental() {}
public:
	virtual TBool NextL(TInt& aStep) =0;
	virtual void Next(TPckgBuf<TInt>& aStep,TRequestStatus& aStatus) =0;
	};

/**
@internalComponent
*/
class CDbSyncIncremental : public CDbIncremental
	{
protected:
	CDbSyncIncremental() {}
private:
	IMPORT_C void Next(TPckgBuf<TInt>& aStep,TRequestStatus& aStatus);
	};

/**
@internalComponent
*/
class CDbAsyncIncremental : public CDbIncremental
	{
protected:
	CDbAsyncIncremental() {}
private:
	IMPORT_C TBool NextL(TInt& aStep);
	};

/**
@internalComponent
*/
class CDbDatabase : public CDbObject
	{
protected:
	CDbDatabase() {}
public:
	enum TUtility {EStats=1,ECompact,ERecover};
	enum TProperty {EUpdateStats=-EStats,ECompactable=-ECompact,EIsDamaged=-ERecover,
					EInTransaction=0,ESize,EUsage};
public:
// Pure virtual
	virtual TInt Destroy()=0;
	virtual TInt Begin()=0;
	virtual TInt Commit()=0;
	virtual void Rollback()=0;
	virtual TInt Property(TProperty aProperty) =0;
	virtual void CreateTableL(const TDesC& aName,const CDbColSet& aColSet,const CDbKey* aPrimaryKey)=0;
	CDbNotifier* NotifierL();
	CDbIncremental* UtilityL(TUtility aType,TInt& aStep);
	CDbIncremental* DropTableL(const TDesC& aTable,TInt& aStep);
	CDbIncremental* AlterTableL(const TDesC& aTable,const CDbColSet& aNewDef,TInt& aStep);
	CDbIncremental* CreateIndexL(const TDesC& aName,const TDesC& aTable,const CDbKey& aKey,TInt& aStep);
	CDbIncremental* DropIndexL(const TDesC& aName,const TDesC& aTable,TInt& aStep);
	CDbIncremental* ExecuteL(const TDesC& aSql,TDbTextComparison aComparison,TInt& aInit);
	CDbCursor* ViewL(const TDbQuery& aQuery,const TDbWindow& aWindow,RDbRowSet::TAccess anAccess);
	CDbCursor* TableL(const TDesC& aName,RDbRowSet::TAccess anAccess);
	virtual void TablesL(CDbTableNames& aNames)=0;
	virtual void ColumnsL(CDbColSet& aColSet,const TDesC& aName)=0;
	virtual void IndexesL(CDbIndexNames& aNames,const TDesC& aTable)=0;
	virtual void KeysL(CDbKey& aKey,const TDesC& aName,const TDesC& aTable)=0;
private:
	virtual CDbNotifier* OpenNotifierL() =0;
	virtual CDbIncremental* OpenUtilityL(TUtility aType,TInt& aStep) =0;
	virtual CDbIncremental* OpenDropTableL(const TDesC& aTable,TInt& aStep) =0;
	virtual CDbIncremental* OpenAlterTableL(const TDesC& aTable,const CDbColSet& aNewDef,TInt& aStep) =0;
	virtual CDbIncremental* OpenCreateIndexL(const TDesC& aName,const TDesC& aTable,const CDbKey& aKey,TInt& aStep) =0;
	virtual CDbIncremental* OpenDropIndexL(const TDesC& aName,const TDesC& aTable,TInt& aStep) =0;
	virtual CDbCursor* PrepareViewL(const TDbQuery& aQuery,const TDbWindow& aWindow,RDbRowSet::TAccess anAccess) =0;
	virtual CDbCursor* OpenTableL(const TDesC& aName,RDbRowSet::TAccess anAccess) =0;
	virtual CDbIncremental* OpenExecuteL(const TDesC& aSql,TDbTextComparison aComparison,TInt& aInit) =0;
// Reserved virtuals
	IMPORT_C virtual void Reserved_1();
	IMPORT_C virtual void Reserved_2();
	};

/**
@internalComponent
*/
class CDbNotifier : public CDbObject
	{
public:
	enum TType {EUnlock=-1,EChange=-2};
protected:
	CDbNotifier() {}
public:
// pure virtual
	virtual void Notify(TType aEvent,TRequestStatus& aStatus) =0;
	virtual void Cancel() =0;
	};


/**
CDbSource class declaration - SYMBIAN_REMOVE_TRIVIAL_ENCRYPTION version
@internalComponent
*/
class CDbSource : public CDbObject
	{
protected:
    CDbSource() 
    	{
    	}
public:
	CDbDatabase* OpenL();
	virtual CDbDatabase* AuthenticateL() = 0;
	CDbNotifier* NotifierL();
private:
	virtual CDbNotifier* OpenNotifierL() =0;
	};


/**
@internalComponent
*/
class TDbDriver
	{
public:
	TInt iCount;
	const TDbFormat* iFormats;
	};

/**
@internalComponent
*/
class TDbFormat
	{
public:
	enum TCreate {ECreate,EReplace};
	enum TOpen {EReadWrite,EReadOnly};
	typedef CDbDatabase* (*FCreateL)(RFs& aFs, const TDesC& aName, TCreate aMode,
                                     const TUidType& aType);
	typedef CDbSource* (*FOpenL)(RFs& aFs,const TDesC& aName,TOpen aMode);
public:
	const TText* iName;
	FCreateL CreateL;
	FOpenL OpenL;
	TInt32 iUid[KMaxCheckedUid];
	};

/**
@internalComponent
*/
#define KDbmsDriverUidValue KDbmsDriverUidValue16
#define KDbmsDriverUid KDbmsDriverUid16

/**
@internalComponent
*/
const TInt32 KDbmsDriverUidValue8=0x10000EBC;
const TUid KDbmsDriverUid8={KDbmsDriverUidValue8};
const TInt32 KDbmsDriverUidValue16=0x10003B0F;
const TUid KDbmsDriverUid16={KDbmsDriverUidValue16};

#include "D32DBAS.INL"
#endif
