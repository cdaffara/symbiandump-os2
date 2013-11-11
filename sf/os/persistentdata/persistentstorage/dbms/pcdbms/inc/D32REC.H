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

#ifndef __D32REC_H__
#define __D32REC_H__
#ifndef __E32BASE_H__
#include <e32base.h>
#endif
#ifndef __D32DBMS_H__
#include <d32dbms.h>
#endif
#ifndef __D32DBAS__
#include "D32DBAS.H"
#endif

// classes used in this header
class RDbTableRow;
class MStreamBuf;

// classes defined in this header
class TDbRecordId;
class CDbRecordBase;
class CDbRecordIter;
class CDbRecordSpace;
class CDbBlobSpace;
class CDbRecordIndex;

/**
@internalComponent
*/
enum TDbPosition { EDbFirst=RDbRowSet::EFirst,EDbNext=RDbRowSet::ENext,EDbPrevious=RDbRowSet::EPrevious,EDbLast=RDbRowSet::ELast };

/**
@internalComponent
*/
const TUint32 KDbNullRecordIdValue=0;
class TDbRecordId
/**
@internalComponent
*/
	{
public:
	TDbRecordId() {}
	inline TDbRecordId(TUint32 aValue);
	inline TDbRecordId& operator=(TUint32 aValue);
//
	inline TBool operator==(TDbRecordId anId) const;
	inline TBool operator!=(TDbRecordId anId) const;
//
	inline TUint32 Value() const;
private:
	TUint32 iValue;
	};
#if defined(__NO_CLASS_CONSTS__)
/**
@internalComponent
*/
#define KDbNullRecordId TDbRecordId(KDbNullRecordIdValue)
#else
/**
@internalComponent
*/
const TDbRecordId KDbNullRecordId=TDbRecordId(KDbNullRecordIdValue);
#endif

class CDbRecordBase : public CBase
/**
@internalComponent
*/
	{
private:
	enum {EOpen=0x1,EModified=0x2,EBroken=0x4};
protected:
	CDbRecordBase() {}
public:
	inline TBool OpenL();
	IMPORT_C void FlushL();
	inline void Abandon();
	inline TBool IsBroken() const;
//
	static void DoFlushL(CDbRecordBase* aRec);
	static void DoDelete(CDbRecordBase* aRec);
	static void DoAbandon(CDbRecordBase* aRec);
protected:
	IMPORT_C void TouchL();
	inline void MarkIntact();
private:
	IMPORT_C TBool DoRestoreL();
	IMPORT_C virtual TBool RestoreL();
	IMPORT_C virtual void AboutToModifyL();
	IMPORT_C virtual void SynchL();
private:
	TUint iFlags;
	};

class CDbRecordIter : public CBase
/**
@internalComponent
*/
	{
public:
	enum TDeleted {EAtRow,ENoRow,ENotSupported};
protected:
	IMPORT_C CDbRecordIter(CDbRecordBase& aHost);
public:
	IMPORT_C ~CDbRecordIter();
	inline CDbRecordBase& Host() const;
// pure virtual
	virtual TInt Count() const =0;
	virtual TDbRecordId CurrentL() =0;
	virtual TBool GotoL(TDbPosition aPosition) =0;
	virtual TBool GotoL(TDbRecordId aRecordId,RDbTableRow& aBuffer) =0;	// re-synchronise
	virtual TBool SeekL(const TDbLookupKey& aKey,RDbTable::TComparison aComparison) =0;
	inline TDeleted DeletedL(TDbPosition aPosition,TDbRecordId aRecordId);					// goto next id
	inline TDeleted DeletedL(TDbPosition aPosition,TDbRecordId aRecordId,const RDbTableRow& aRow);	// goto next id
//
	virtual TDeleted DoDeletedL(TDbPosition aPosition,TDbRecordId aRecordId,const RDbTableRow* aRow) =0;
private:
	CDbRecordBase& iHost;
	};

class CDbRecordSpace : public CDbRecordBase
/**
@internalComponent
*/
	{
protected:
	CDbRecordSpace() {}
public:
	virtual TBool ExistsL(TDbRecordId aRecordId) =0;
	virtual TPtrC8 ReadL(TDbRecordId aRecordId) const =0;
	TUint8* ReplaceL(TDbRecordId aRecordId,TInt aRecordSize);
	virtual TUint AutoIncrementL() =0;
	TUint8* NewL(TInt aRecordSize);
	virtual TDbRecordId AppendL() =0;
	void EraseL(TDbRecordId aRecordId);
	virtual CDbRecordIter* IteratorL() =0;
private:
	virtual TUint8* DoReplaceL(TDbRecordId aRecordId,TInt aRecordSize) =0;
	virtual TUint8* DoNewL(TInt aRecordSize) =0;
	virtual void DoEraseL(TDbRecordId aRecordId) =0;
	};

class CDbBlobSpace : public CDbRecordBase
/**
@internalComponent
*/
	{
protected:
	CDbBlobSpace() {}
public:
	virtual MStreamBuf* ReadL(TDbBlobId aBlobId,TDbColType aType) const =0;
	IMPORT_C MStreamBuf* ReadLC(TDbBlobId aBlobId,TDbColType aType) const;
	IMPORT_C MStreamBuf* CreateL(TDbBlobId &aBlobId,TDbColType aType);
	IMPORT_C TDbBlobId CreateL(TDbColType aType,const TAny* aBuf,TInt aLength);
	IMPORT_C void DeleteL(TDbBlobId aBlobId);
	IMPORT_C TInt Delete(TDbBlobId aBlobId);
	inline TInt InlineLimit() const;
protected:
	inline void SetInlineLimit(TInt anInlineLimit);
private:
	virtual MStreamBuf* DoCreateL(TDbBlobId &aBlobId,TDbColType aType) =0;
	virtual void DoDeleteL(TDbBlobId aBlobId) =0;
private:
	TInt iInlineLimit;
	};

class CDbRecordIndex : public CDbRecordBase
/**
@internalComponent
*/
	{
public:
	enum TFind {ENoMatch,EKeyMatch,EEntryMatch};
	enum TInclusion {EIncludeLower=0x1,EIncludeUpper=0x2,EIncludeBoth=0x3};
protected:
	CDbRecordIndex() {}
public:
	virtual TFind FindL(TDbRecordId aRecordId,const RDbTableRow& aRow) =0;
	TBool InsertL(TDbRecordId aRecordId,const RDbTableRow& aRow);
	void DeleteL(TDbRecordId aRecordId,const RDbTableRow& aRow);
	virtual CDbRecordIter* IteratorL(TUint aInclusion=0,const TDbLookupKey* aLowerBound=0,const TDbLookupKey* aUpperBound=0) =0;
private:
	virtual TBool DoInsertL(TDbRecordId aRecordId,const RDbTableRow& aRow) =0;
	virtual void DoDeleteL(TDbRecordId aRecordId,const RDbTableRow& aRow) =0;
	};

#include "D32REC.INL"
#endif
