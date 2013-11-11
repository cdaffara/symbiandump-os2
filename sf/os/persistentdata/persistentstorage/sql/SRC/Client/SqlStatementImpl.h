// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// NTT DOCOMO, INC - Fix for Bug 1915 "SQL server panics when using long column type strings"
//
// Description:
//

#ifndef __SQLSTATEMENTIMPL_H__
#define __SQLSTATEMENTIMPL_H__

#include "SqlBufIterator.h"	//TSqlBufFlat, TSqlBufRIterator, TSqlBufWIterator
#include "SqlDatabaseImpl.h"//CSqlDatabaseImpl
#include "SqlStmtSession.h"	//RSqlStatementSession

//Forward declarations
class MStreamBuf;
class CSqlStatementImpl;

#ifdef _DEBUG
#define LONGCOL_INVARIANT()	Invariant()
#else
#define LONGCOL_INVARIANT()
#endif

/**
RSqlLongColumnColl class represents a collection of long text/binary column values.
Once a column value is added to the collection, it is guaranteed that the memory address of the 
column value will never change.
The class was designed to solve one specific problem - to exclude the possibility that SQL clients
may get a dangling pointer to a block of memory which already has been deleted.
Without RSqlLongColumnColl class, it may happen for example, when client prepares a SELECT sql
statement, where the record consists of two long binary/text column values.
The client calls RSqlStatement::Next() to move to a record. Since the column values are long, they will
not be transferred on the client side by the Next() call, so the client side row buffer (CSqlStatementImpl::iColumnValueBuf)
will mark them as "not present". 
Then the client calls RSqlStatement::ColumnBinary(0, TPtrC8&) to get a pointer to the first column value.
The row buffer is reallocated, and the column value is retrieved from the server and copied into the buffer, the client's pointer
is set to point to the place in the row buffer, where the column value is.
While keeping the pointer to the first column value, the client calls again RSqlStatement::ColumnBinary(1, TPtrC8&) to retrieve
the second column value. In which case, the row buffer may get reallocated again, so the first pointer will point to a 
block of memory, which has been deleted.
With the implementation and use of RSqlLongColumnColl class, the long column values will be kept in a different place,
RSqlLongColumnColl's collection, where it is guaranteed that the memory address of the column value is constant.

@internalComponent
*/
NONSHARABLE_CLASS(RSqlLongColumnColl)
	{
public:	
	struct TColumnReader
		{
		virtual TInt Read(TInt aColumnIndex, TDes8& aBuf) = 0;
		};
		
public:	
	inline RSqlLongColumnColl();
	inline void Close();
	inline void Reset();
	TInt Append(TColumnReader& aReader, TInt aColumnIndex, TInt aColumnSize);
	inline TPtrC Text(TInt aColumnIndex) const;
	inline TPtrC8 Binary(TInt aColumnIndex) const;
	inline TBool IsPresent(TInt aColumnIndex) const;

private:
	inline TInt FindValue(TInt aColumnIndex) const;
#ifdef _DEBUG
	void Invariant() const;	
#endif
	
private:
	struct TData
		{
		static TBool Compare(const TInt* aIndex, const TData& aData);
		inline TData(TInt aIndex, HBufC8* aData);
		TInt	iIndex;
		HBufC8*	iData;
		};
	enum {KLongColumnCollGranularity = 8};
	RArray<TData>	iValues;
	
	};

template <class DES> TInt SqlCreateStatement(CSqlStatementImpl*& aImpl, CSqlDatabaseImpl& aDatabase, const DES& aSqlStmt);

/**
CSqlStatementImpl implements RSqlStatement functionality.
CSqlStatementImpl can prepare and execute parametrized SQL statements and SQL statements without parameters.
CSqlStatementImpl cannot prepare and execute SQL strings containing more than one SQL statement.
@internalComponent
*/
NONSHARABLE_CLASS(CSqlStatementImpl) : public CBase
	{
	template <class DES> friend TInt SqlCreateStatement(CSqlStatementImpl*& aImpl, CSqlDatabaseImpl& aDatabase, const DES& aSqlStmt);
	
public:
	static inline TInt New(CSqlStatementImpl*& aImpl, CSqlDatabaseImpl& aDatabase, const TDesC16& aSqlStmt);
	static inline TInt New(CSqlStatementImpl*& aImpl, CSqlDatabaseImpl& aDatabase, const TDesC8& aSqlStmt);
	virtual ~CSqlStatementImpl();
	
	inline TBool AtRow() const;
	
	TInt Reset();
	TInt Exec();
	void Exec(TRequestStatus& aStatus);
	TInt Next();
	
	TInt BindNull(TInt aParamIndex);
	TInt BindInt(TInt aParamIndex, TInt aParamValue);
	TInt BindInt64(TInt aParamIndex, TInt64 aParamValue);
	TInt BindReal(TInt aParamIndex, TReal aParamValue);
	TInt BindText(TInt aParamIndex, const TDesC& aParamText);
	TInt BindBinary(TInt aParamIndex, const TDesC8& aParamData);
	TInt BindZeroBlob(TInt aParamIndex, TInt aBlobSize);

	inline TInt ColumnCount() const;
	TSqlColumnType ColumnType(TInt aColumnIndex);
	TInt DeclaredColumnType(TInt aColumnIndex, TSqlColumnType& aColumnType);
	TInt ColumnSize(TInt aColumnIndex);
	TInt ColumnInt(TInt aColumnIndex);
	TInt64 ColumnInt64(TInt aColumnIndex);
	TReal ColumnReal(TInt aColumnIndex);
	TInt ColumnText(TInt aColumnIndex, TPtrC& aPtr);
	TInt ColumnText(TInt aColumnIndex, TDes& aDest);
	TInt ColumnBinary(TInt aColumnIndex, TPtrC8& aPtr);
	TInt ColumnBinary(TInt aColumnIndex, TDes8& aDest);

	TInt ColumnName(TInt aColumnIndex, TPtrC& aNameDest);
	TInt ParameterName(TInt aParamIndex, TPtrC& aNameDest);

	inline TInt ParamIndex(const TDesC& aParamName);
	inline TInt ColumnIndex(const TDesC& aColumnName);
		
	inline MStreamBuf* ColumnSourceL(TInt aColumnIndex);
	inline MStreamBuf* ParamSinkL(TSqlSrvFunction aFunction, TInt aParamIndex);

private:
	inline CSqlStatementImpl();
	template <class DES> TInt Construct(CSqlDatabaseImpl& aDatabase, const DES& aSqlStmt);
	TInt Name2Index(const TDesC& aName, RSqlBufFlat& aNameBufFlat, TInt aCount, TSqlSrvFunction aFunction, TBool& aPresent);
	TInt Index2Name(TInt aIndex, RSqlBufFlat& aNameBufFlat, TInt aCount, TSqlSrvFunction aFunction, TBool& aPresent, TPtrC& aColName);
	TInt CheckNameBufPresent(TBool& aPresent, RSqlBufFlat& aNameBufFlat, TInt aCount, TSqlSrvFunction aFunction);
	
private:
	RSqlStatementSession	iSqlStmtSession;
	TBool					iBound;
	enum TState				{EUnknown, EAtRow};
	TState					iState;
	
	TInt 					iColumnCnt;
	TBool					iColumnNameBufPresent;
	RSqlBufFlat				iColumnNameBuf;
	RSqlBufFlat				iColumnValueBuf;
	TSqlBufRIterator		iColumnValBufIt;
	
	TInt 					iParamCnt;
	TBool					iParamNameBufPresent;
	RSqlBufFlat				iParamNameBuf;
	RSqlBufFlat				iParamValueBuf;
	TSqlBufWIterator		iParamValBufIt;
	
	RSqlLongColumnColl		iLongColumnColl;
	
	RArray<TSqlColumnType>	iDeclaredColumnTypes;	//Array of declared column types for current statement.
	};

#include "SqlStatementImpl.inl"

#endif //__SQLSTATEMENTIMPL_H__
