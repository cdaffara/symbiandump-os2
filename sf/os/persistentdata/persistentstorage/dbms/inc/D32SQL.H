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
// SQL parsing engine
// 
//

#ifndef __D32SQL_H__
#define __D32SQL_H__

#ifndef __D32DBMS_H__
#include <d32dbms.h>
#endif

// Other class referenced
class MStreamBuf;
class TDbBlob;
class RDbTableRow;
class TTextOps;
class TSqlParser;
class CDbTable;
class CDbTableDatabase;
class CDbDataSource;
class CDbBlobSpace;
template <class T,class D> class HMatcherPattern;

// Classes defined
class Sql;
class RSqlLiteral;
class CSqlValues;
class RSqlColumnList;
class CSqlSearchCondition;
class CSqlMultiNode;
class CSqlBoundNode;
class CSqlNullPredicate;
class CSqlLiteralNode;
class CSqlCompPredicate;
class CSqlLikePredicate;
class CSqlQuery;
class CSqlDDLStatement;
class CSqlDMLStatement;
class CSqlInsertStatement;
class CSqlModifyStatement;

/**
@internalComponent
*/
typedef HMatcherPattern<TUint8,TDesC8> HMatcherPattern8;
typedef HMatcherPattern<TUint16,TDesC16> HMatcherPattern16;

/**
@internalComponent
*/
class Sql
	{
public:
	enum TStatementType { ENone, EDDL, EDML };
public:
	static CSqlDDLStatement* ParseDDLStatementLC( const TDesC& aSql );
	static CSqlDMLStatement* ParseDMLStatementLC( const TDesC& aSql );
	static CSqlQuery* ParseQueryLC( const TDesC& aSql );
	static CSqlSearchCondition* ParseSearchConditionLC( const TDesC& aSql );
	//
	static TStatementType Type( const TDesC& aSql );
	};

// variant data classes

/**
@internalComponent
*/
class RSqlLiteral
	{
public:
	inline RSqlLiteral();
	void Close();
// assignment
	inline void SetText( const TText* aPtr, const TText* aEnd );
	inline TInt64& SetInt();
	inline TReal64& SetReal();
	inline TTime& SetTime();
// conversions
	void ToInt32L();
	void ToUint32L();
	void ToInt64L();
	void ToReal32L();
	void ToReal64L();
	void ToTimeL();
	void ToText8L();
	void ToText16L();
	void ToPattern8L(TBool aEscape = EFalse);
	void ToPattern16L(TBool aEscape = EFalse);
	TInt CopyText();
// accessors
	inline TBool IsNull() const;
	inline TInt32 Int32() const;
	inline TUint32 Uint32() const;
	inline const TInt64& Int64() const;
	inline const TTime& Time() const;
	inline const TReal32& Real32() const;
	inline const TReal64& Real64() const;
	inline const TText* Ptr() const;
	inline const TText* End() const;
	inline const TPtrC DesC() const;
	inline const HBufC8& Text8() const;
	inline const HBufC16& Text16() const;
	inline const HMatcherPattern8& Pattern8() const;
	inline const HMatcherPattern16& Pattern16() const;
private:
	inline TBool IsBasic() const;
	inline TBool IsAlloc() const;
private:
	enum TType { EInt32, EUint32, EReal32, EInt64, ETime, EReal64, EPtr, ENull, EBuf8, EBuf16, EPattern8, EPattern16 };
private:
	union
		{
		TInt32 iInt32;
		TUint32 iUint32;
		TUnion<TInt64> iInt64;
		TReal32 iReal32;
		TReal64 iReal64;
		TUnion<TTime> iTime;
		struct { const TText* iPtr; const TText* iEnd; } iPtr;
		TAny* iAlloc;
		} iVal;
	TType iType;
	HBufC* iBuffer;
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CSqlValues) : public CBase
	{
public:
	CSqlValues();
	~CSqlValues();
//
	void AddL( const RSqlLiteral& aLiteral );
	void BindL( const CDbDataSource& aSource );
	void WriteL( CDbDataSource& aSource, CDbTable& aTable ) const;
private:
	enum { EGranularity = 8 };
	class TEntry
		{
	public:
		inline TEntry( const RSqlLiteral& aValue );
	public:
		RSqlLiteral iValue;
		TDbColType iType;
		};
private:
	RArray<TEntry> iValues;
	};

/**
@internalComponent
*/
class RSqlColumnList : public RArray<TPtrC>
	{
	enum { EGranularity = 8 };
public:
	inline RSqlColumnList();
	};

// evaluation tree classes

/**
@internalComponent
*/
class CSqlSearchCondition : public CBase
	{
public:
	enum TType
		{
		EAnd, EOr,
		EIsNull, EIsNotNull,
		ELike, ENotLike,
		ELess, ELessEqual, EEqual, EGreaterEqual, EGreater, ENotEqual
		};
public:

	TBool iIsEscape; 
	
	inline TType NodeType() const;
	inline CSqlMultiNode* MultiNode();
	inline CSqlNullPredicate* NullPredicate();
	inline CSqlCompPredicate* CompPredicate();
	inline CSqlLikePredicate* LikePredicate();
//
	virtual void BindL( const RDbTableRow& aSource ) = 0;
	virtual TBool EvaluateL( const TTextOps& aTextOp ) const = 0;
protected:
	CSqlSearchCondition( TType aType );
		
private:
	TType iType;
	
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CSqlMultiNode) : public CSqlSearchCondition
	{
private:
	typedef CSqlSearchCondition* TEntry;
	enum { EGranularity = 4 * sizeof( TEntry ) };
public:
	static CSqlSearchCondition* New( TType aType, CSqlSearchCondition* aLeft, CSqlSearchCondition* aRight );
//
	inline TInt Count() const;
	inline CSqlSearchCondition* SubNode( TInt aIndex ) const;
	inline void SetSubNode( TInt aIndex, CSqlSearchCondition* aSearchCondition ); 
	void Remove( CSqlSearchCondition* aSubNode );
	static CSqlSearchCondition* Reduce( CSqlMultiNode* aNode );
private:
	inline CSqlMultiNode( TType aType );
	~CSqlMultiNode();
	static CSqlMultiNode* New( TType aType, TInt aSize );
	static CSqlMultiNode* Grow( CSqlMultiNode* aNode, TInt aExtraSize );
	static CSqlMultiNode* Concatenate( CSqlMultiNode* aLeft, CSqlMultiNode* aRight );
//
	inline TInt Size() const;
	static inline TInt Size( TInt aLeaves );
	inline TBool IsFull() const;
private:
// from CSqlSearchCondition
	void BindL( const RDbTableRow& aSource );
	TBool EvaluateL( const TTextOps& aTextOp ) const;
private:
	TEntry* iEnd;
	TEntry iLeaves[1];
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CSqlBoundNode) : public CSqlSearchCondition
	{
public:
	const TDesC& ColumnName() const;
	inline TDbColNo ColNo() const;
	inline TDbColType ColType() const;
protected:
	CSqlBoundNode( TType aType, const TDesC& aColumn );
	inline TBool IsBound() const;
	inline TDbColumnC Column() const;
	MStreamBuf& StreamLC( const TDbBlob& aBlob ) const;
// from CSqlSearchCondition
	void BindL( const RDbTableRow& aSource );
private:
	const RDbTableRow* iSource;
	union
		{
		TUnion<TPtrC> iName;
		struct
			{
			TDbColNo iNumber;
			TDbColType iType;
			} iBound;
		} iColumn;
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CSqlNullPredicate) : public CSqlBoundNode
	{
public:
	CSqlNullPredicate( TType aType, const TDesC& aColumn );
private:
// from CSqlSearchCondition
	TBool EvaluateL( const TTextOps& aTextOp ) const;
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CSqlLiteralNode) : public CSqlBoundNode
	{
public:
	inline const RSqlLiteral& Value() const;
	inline RSqlLiteral& Value();
protected:
	CSqlLiteralNode( TType aType, const TDesC& aColumn, const RSqlLiteral& aLiteral );
	~CSqlLiteralNode();
// from CSqlSearchCondition
	void BindL( const RDbTableRow& aSource );
private:
	RSqlLiteral iLiteral;
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CSqlCompPredicate) : public CSqlLiteralNode
	{
public:
	CSqlCompPredicate( TType aType, const TDesC& aColumn, const RSqlLiteral& aLiteral );
private:
	TInt CompareLongText8L( const TDbBlob& aBlob, const TTextOps& aTextOp ) const;
	TInt CompareLongText16L( const TDbBlob& aBlob, const TTextOps& aTextOp ) const;
// from CSqlSearchCondition
	TBool EvaluateL( const TTextOps& aTextOp ) const;
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CSqlLikePredicate) : public CSqlLiteralNode 
	{
public:
	CSqlLikePredicate( TType aType, const TDesC& aColumn, const RSqlLiteral& aPattern );
private:
// from CSqlSearchCondition
	void BindL( const RDbTableRow& aSource );
	TBool EvaluateL( const TTextOps& aTextOp ) const;
private:
	TInt iMatchSize;
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CSqlQuery) : public CBase
	{
	friend class TSqlParser;
public:
	CSqlQuery();
	~CSqlQuery();
//
	inline const TDesC& Table() const;
//
	inline TBool HasColumnList() const;
	inline const RSqlColumnList& ColumnList() const;
//
	inline TBool HasSearchCondition() const;
	inline CSqlSearchCondition& SearchCondition();
	void RemovePredicate( CSqlCompPredicate* aNode );
	inline CSqlSearchCondition* AdoptSearchCondition();
//
	inline TBool HasSortSpecification() const;
	inline CDbKey& SortSpecification();
private:
	CDbKey& SortSpecificationL();
private:
	TPtrC iTable;
	RSqlColumnList iColumns;
	CSqlSearchCondition* iSearchCondition;
	CDbKey* iSortSpecification;
	};

/**
@internalComponent
*/
class CSqlDDLStatement : public CBase
	{
protected:
	inline CSqlDDLStatement() {}
public:
	virtual CDbIncremental* ExecuteL( CDbDatabase& aDatabase, TDbTextComparison aComparison, TInt& aStep ) = 0;
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CSqlDMLStatement) : public CBase
	{
	friend class TSqlParser;
public:
	~CSqlDMLStatement();
//
	inline CSqlQuery& Query();
	inline CSqlValues& Values();
	inline CSqlValues* AdoptValues();
//
	virtual CDbIncremental* ExecuteL( CDbTableDatabase& aDatabase, TDbTextComparison aComparison, TInt& aRows ) = 0;
protected:
	inline CSqlDMLStatement() {}
	inline TBool HasValues() const;
private:
	CSqlValues& ValuesL();
private:
	CSqlQuery iQuery;
	CSqlValues* iValues;
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CSqlInsertStatement) : public CSqlDMLStatement
	{
public:
	static CSqlInsertStatement* NewLC();
private:
	inline CSqlInsertStatement() {}
// from CSqlDMLStatement
	CDbIncremental* ExecuteL( CDbTableDatabase& aDatabase, TDbTextComparison aComparison, TInt& aRows );
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CSqlModifyStatement) : public CSqlDMLStatement
	{
public:
	static CSqlModifyStatement* NewLC();
	inline TBool IsUpdate() const;
private:
	inline CSqlModifyStatement() {}
// from CSqlDMLStatement
	CDbIncremental* ExecuteL( CDbTableDatabase& aDatabase, TDbTextComparison aComparison, TInt& aRows );
	};

/**
TSqlParser2 class uses TSqlParser class functionality only to parse particular SQL string
and extract SQL type and table name.
@internalComponent
*/
class TSqlParser2
	{
public:
	void ParseL(const TDesC& aSql);
	inline Sql::TStatementType StatementType() const;
	inline const TDesC& TableName() const;

private:
	Sql::TStatementType	iStatementType;
	TPtrC				iTableName;

	};

#include "D32SQL.INL"

#endif//__D32SQL_H__
