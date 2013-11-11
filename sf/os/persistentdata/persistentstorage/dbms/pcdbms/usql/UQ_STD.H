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
#include "D32SQL.H"
#include "D32TABLE.H"
#include <e32math.h>
#include <s32mem.h>
#include "D32Assert.h"

//Forward declarations
class TSqlParser2;

// Classes defined
class TSqlToken;
class TSqlLexer;
class TSqlParser;
template <class T,class D> class TMatcher;
template <class T,class D> class TDesMatcher;
template <class T,class D> class TStreamMatcher;
template <class T,class D> class HMatcherPattern;
class CSqlCreateTableStatement;
class CSqlDropTableStatement;
class CSqlAlterTableStatement;
class CSqlCreateIndexStatement;
class CSqlDropIndexStatement;

/**
parser classes
@internalComponent
*/
enum TSqlTokenType
	{
	ESqlNoToken=0,
//
	ESqlEos,
//
	ESqlIdentifier,
	ESqlLiteralInt,
	ESqlLiteralReal,
	ESqlLiteralTime,
	ESqlLiteralText,
	ESqlLiteralBlob,
//
	ESqlAsterisk,
	ESqlComma,
	ESqlLeftBracket,
	ESqlRightBracket,
	ESqlLess,
	ESqlLessEqual,
	ESqlEqual,
	ESqlGreaterEqual,
	ESqlGreater,
	ESqlNotEqual
	};

/**
@internalComponent
*/
enum TSqlKeyword
	{
	ESqlNotKeyword=-1,
//
#define KEYWORD(s) ESqlKeyword_##s
#include "UQ_KEYWD.H"
#undef KEYWORD
	};

class TSqlToken
/**
@internalComponent
*/
	{
	friend class TSqlLexer;
public:
	inline TBool operator==(TSqlTokenType aType) const;
	inline TBool operator!=(TSqlTokenType aType) const;
//
	inline void SetError(TInt aError);
	inline TInt Error() const;
//
	inline TSqlTokenType Type() const;
	inline const RSqlLiteral& Literal() const;
	inline RSqlLiteral& Literal();
private:
	inline TSqlTokenType operator=(TSqlTokenType aType);
private:
	TInt iType;
	RSqlLiteral iLiteral;
	};

class TSqlLexer
/**
@internalComponent
*/
	{
public:
	TSqlLexer(const TDesC& aSql);
	inline TSqlTokenType NextToken(TSqlToken& aToken);
	static TSqlKeyword Keyword(const TSqlToken& aToken);
	static inline TBool IsKeyword(TSqlKeyword aKeyword,const TSqlToken& aToken);
	const TText* Next() const;
	void Set(const TText* aNext);	
private:
	TSqlTokenType GetIdentifier(TSqlToken& aToken);
	TSqlTokenType GetString(TSqlToken& aToken);
	TInt GetInteger(TInt64& aValue);
	TSqlTokenType GetNumber(TSqlToken& aToken);
	TSqlTokenType GetDate(TSqlToken& aToken);
	TSqlTokenType GetNextToken(TSqlToken& aToken);
	TSqlTokenType GetBlob(TSqlToken& aToken);
//
	static inline TSqlTokenType SqlError(TInt aError=KErrArgument);
	static TInt CompareKeyword(TInt aKeyword,const RSqlLiteral& aIdentifier);
private:
	const TText* iNext;
	const TText* iEnd;
	};

class TSqlParser
/**
@internalComponent
*/
	{
	friend class TSqlParser2;
public:
	TSqlParser(const TDesC& aSql);
//
	CSqlQuery* QueryLC();
	CSqlSearchCondition* SearchConditionLC();
	CSqlDDLStatement* DDLStatementLC();
	CSqlDMLStatement* DMLStatementLC();
	Sql::TStatementType Type();
	TInt PatternFilter(const TDesC& aPattern,const TChar aEscape, TText *aNewPatternBuffer );
private:
	TSqlTokenType NextToken();
	CSqlSearchCondition* SqlError();
	inline TInt Error() const;
	static TSqlTokenType SqlErrorL();
//
	TSqlTokenType Parse(TSqlKeyword aKeyword);
	TSqlTokenType ParseL(TSqlTokenType aToken);
	TSqlTokenType ParseL(TSqlKeyword aKeyword);
	TSqlKeyword Keyword();
	TSqlTokenType RightBracketL();
	TSqlTokenType IdentifierL(TPtrC& aIdentifier);
	TPtrC IdentifierL();
	void EndL();
//
	TSqlTokenType ColumnNameL(RSqlColumnList& aList);
	TSqlTokenType ColumnListL(RSqlColumnList& aList);
	CSqlSearchCondition* SearchCondition(TInt aNot);
	CSqlSearchCondition* BooleanTerm(TInt aNot);
	CSqlSearchCondition* BooleanFactor(TInt aNot);
	CSqlSearchCondition* BooleanPrimary(TInt aNot);
	CSqlSearchCondition* Predicate(TInt aNot);
	void SortSpecificationL(CDbKey& aKey);
	CDbKey* SortSpecificationL();
	void DoQueryL(CSqlQuery& aQuery);
	CSqlSearchCondition* SearchConditionL();
//
	TSqlTokenType AddColumnSpecL(TDbCol& aDef);
	CSqlDDLStatement* CreateTableLC();
	CSqlDDLStatement* DropTableLC();
	CSqlDDLStatement* AlterTableLC();
	CSqlDDLStatement* CreateIndexLC(TBool aUnique);
	CSqlDDLStatement* DropIndexLC();
//
	TSqlTokenType ColumnValueL(CSqlValues& aValues);
	CSqlDMLStatement* InsertStatementLC();
	CSqlDMLStatement* UpdateStatementLC();
	CSqlDMLStatement* DeleteStatementLC();
private:
	TSqlLexer iSql;
	TSqlToken iToken;
	};


/**
LIKE evaluation support
@internalComponent
*/
enum TSegmentType {ENull,ESuccess,EStop,EBeginning,EEnd,EMiddle,ESkip,EEscape,EMask=0xf,EWild=0x80};
/**
@internalComponent
*/
const TInt KPatternGranularity=0x20;

// If this value is changed class documentation must be updated.
// Namely, RDbDatabase and TDbQuery.
const TInt KMaxSegmentLength=0xff;

template <class T,class D>
class HMatcherPattern
/**
@internalComponent
*/
	{
	friend class TMatcher<T,D>;
	typedef HMatcherPattern<T,D> TThis;
public:
	static HMatcherPattern<T,D>* NewL(const D& aPattern,TBool aEscape = EFalse);
	TInt MatchL(const D& aDes,const TTextOps& aTextOp) const;
	TInt MatchL(MStreamBuf& aBuf,TInt aLength,const TTextOps& aTextOp) const;
private:
	static TInt Construct(HMatcherPattern<T,D>*& aCell,const D& aPattern,TBool aEscape = EFalse);
	static inline HMatcherPattern<T,D>* Realloc(HMatcherPattern<T,D>* aThis,TInt aSize);
private:
	T iPattern[1];
	};

template <class T,class D>
class TMatcher
/**
@internalComponent
*/
	{
public:
	TBool MatchL(const HMatcherPattern<T,D>& aPattern,const TTextOps& aTextOp);
private:
	virtual const T* UnderflowL(const T*& aEnd,TInt aRetain=0) =0;
	};

template <class T,class D>
class TDesMatcher : public TMatcher<T,D>
/**
@internalComponent
*/
	{
public:
	inline TDesMatcher(const D& aDes);
private:
	const T* UnderflowL(const T*& aEnd,TInt aRetain=0);
private:
	const T* iPtr;
	const T* iEnd;
	};

template <class T,class D>
class TStreamMatcher : public TMatcher<T,D>
/**
@internalComponent
*/
	{
public:
	inline TStreamMatcher(MStreamBuf& aStreamBuf,TInt aLength);
private:
	const T* UnderflowL(const T*& aEnd,TInt aRetain=0);
private:
	enum {EBufSize=0x200};
private:
	RReadStream iStream;
	TInt iRemain;
	T iBuffer[EBufSize];
	const T* iEnd;
	};

NONSHARABLE_CLASS(CSqlCreateTableStatement) : public CSqlDDLStatement
/**
DDL statements
@internalComponent
*/
	{
public:
private:
	CDbIncremental* ExecuteL(CDbDatabase& aDatabase,TDbTextComparison aComparison,TInt& aSteps);
public:
	TPtrC iName;
	CDbColSet iColumns;
	};

NONSHARABLE_CLASS(CSqlDropTableStatement) : public CSqlDDLStatement
/**
@internalComponent
*/
	{
private:
	CDbIncremental* ExecuteL(CDbDatabase& aDatabase,TDbTextComparison aComparison,TInt& aSteps);
public:
	TPtrC iName;
	};

NONSHARABLE_CLASS(CSqlAlterTableStatement) : public CSqlDDLStatement
/**
@internalComponent
*/
	{
public:
	~CSqlAlterTableStatement();
private:
	CDbIncremental* ExecuteL(CDbDatabase& aDatabase,TDbTextComparison aComparison,TInt& aSteps);
public:
	TPtrC iName;
	CDbColSet iAdd;
	RSqlColumnList iDrop;
	};

NONSHARABLE_CLASS(CSqlCreateIndexStatement) : public CSqlDDLStatement
/**
@internalComponent
*/
	{
private:
	CDbIncremental* ExecuteL(CDbDatabase& aDatabase,TDbTextComparison aComparison,TInt& aSteps);
public:
	TPtrC iName;
	TPtrC iTable;
	CDbKey iKey;
	};

NONSHARABLE_CLASS(CSqlDropIndexStatement) : public CSqlDDLStatement
/**
@internalComponent
*/
	{
private:
	CDbIncremental* ExecuteL(CDbDatabase& aDatabase,TDbTextComparison aComparison,TInt& aSteps);
public:
	TPtrC iName;
	TPtrC iTable;
	};

#include "UQ_STD.INL"
