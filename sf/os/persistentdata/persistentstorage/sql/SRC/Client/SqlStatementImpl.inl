// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// ///////////////////////             RSqlLongColumnColl        ////////////////////////////////////////////
// Sets the default granularity of the collection.
// 
//

inline RSqlLongColumnColl::RSqlLongColumnColl() :
	iValues(KLongColumnCollGranularity)
	{
	}

/**
Releases the allocated memory.
*/
inline void RSqlLongColumnColl::Close()
	{
	LONGCOL_INVARIANT();
	Reset();
	iValues.Close();	
	}

/**
Destroys all long column values in the collection, without destroying the collection object.
*/
inline void RSqlLongColumnColl::Reset()
	{
	LONGCOL_INVARIANT();
	for(TInt i=iValues.Count()-1;i>=0;--i)
		{
		delete iValues[i].iData;
		}
	iValues.Reset();
	}

/**
Constructs and returns a TPtrC object to the long column value, identified by aColumnIndex argument.
@param aColumnIndex Column index
@return A TPtrC object to the long column value
@panic SqlDb 4 In _DEBUG mode, aColumnIndex value is negative.
@panic SqlDb 7 There is no long column value with aColumnIndex index.
*/
inline TPtrC RSqlLongColumnColl::Text(TInt aColumnIndex) const
	{
	__ASSERT_DEBUG(aColumnIndex >= 0, __SQLPANIC(ESqlPanicBadArgument));
	LONGCOL_INVARIANT();
	TInt rc = FindValue(aColumnIndex);
	__ASSERT_ALWAYS(rc >= 0, __SQLPANIC(ESqlPanicInternalError));
	TPtrC8 ptr(iValues[rc].iData->Des());
	return TPtrC(reinterpret_cast <const TUint16*> (ptr.Ptr()), ptr.Length() / sizeof(TUint16));
	}

/**
Constructs and returns a TPtrC8 object to the long column value, identified by aColumnIndex argument.
@param aColumnIndex Column index
@return A TPtrC8 object to the long column value
@panic SqlDb 4 In _DEBUG mode, aColumnIndex value is negative.
@panic SqlDb 7 There is no long column value with aColumnIndex index.
*/
inline TPtrC8 RSqlLongColumnColl::Binary(TInt aColumnIndex) const
	{
	__ASSERT_DEBUG(aColumnIndex >= 0, __SQLPANIC(ESqlPanicBadArgument));
	LONGCOL_INVARIANT();
	TInt rc = FindValue(aColumnIndex);
	__ASSERT_ALWAYS(rc >= 0, __SQLPANIC(ESqlPanicInternalError));
	return iValues[rc].iData->Des();
	}

/**
Returns true if there is a long column value in the collection, which index is aColumnIndex.
@param aColumnIndex Column index
@return True if there is a long column value in the collection, which index is aColumnIndex.
@panic SqlDb 4 In _DEBUG mode, aColumnIndex value is negative.
*/
inline TBool RSqlLongColumnColl::IsPresent(TInt aColumnIndex) const
	{
	__ASSERT_DEBUG(aColumnIndex >= 0, __SQLPANIC(ESqlPanicBadArgument));
	LONGCOL_INVARIANT();
	return FindValue(aColumnIndex) >= 0;
	}

/**
The method returns the index in the collection of the long column value, identified by aColumnIndex argument.
@param aColumnIndex Column index
@return The collection index of the long column value, identified by aColumnIndex,
		KErrNotFound otherwise.
@panic SqlDb 4 In _DEBUG mode, aColumnIndex value is negative.
*/
inline TInt RSqlLongColumnColl::FindValue(TInt aColumnIndex) const
	{
	__ASSERT_DEBUG(aColumnIndex >= 0, __SQLPANIC(ESqlPanicBadArgument));
	return iValues.Find(aColumnIndex, &RSqlLongColumnColl::TData::Compare);
	}

/**
Initializes the RSqlLongColumnColl::TData instance with the column index, and a pointer to the column data
@param aIndex Column index
@param aData A HBufC8 pointer to the column data. Cannot be NULL, this is a long column value
@panic SqlDb 4 In _DEBUG mode, aColumnIndex value is negative or aData is NULL.
*/
inline RSqlLongColumnColl::TData::TData(TInt aIndex, HBufC8* aData) :
	iIndex(aIndex),
	iData(aData)
	{
	__ASSERT_DEBUG(aIndex >= 0, __SQLPANIC(ESqlPanicBadArgument));
	__ASSERT_DEBUG(aData != NULL, __SQLPANIC(ESqlPanicBadArgument));
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////           CSqlStatementImpl           ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
/**
Initializes the created CSqlDatabaseImpl object.
Works with 8/16-bit SQL statements.

@param aDatabase A reference to CSqlDatabaseImpl object.
@param aSqlStmt 8/16-bit string containing the SQL statement.

@return KErrNoMemory, an out of memory condition has occurred;
 	    KErrArgument, bad argument, for example - the SQL string contains more than one SQL statements.
                      Note that the function may leave with some database specific errors categorised as 
                      ESqlDbError or other system-wide error codes;
        KErrNone      Operation has completed successfully.

@panic SqlDb 7 In _DEBUG mode, invalid column count.

@see CSqlStatementImpl::New()
*/
template <class DES> TInt CSqlStatementImpl::Construct(CSqlDatabaseImpl& aDatabase, const DES& aSqlStmt)
	{
	TInt err = iSqlStmtSession.Prepare(aDatabase.Session(), aSqlStmt, iColumnCnt, iParamCnt);
	if(err != KErrNone)
		{
		return err;	
		}
	__ASSERT_DEBUG(iColumnCnt >= 0, __SQLPANIC(ESqlPanicInternalError));
    err = iColumnValueBuf.SetCount(iColumnCnt);
    if(err != KErrNone)
        {
        return err;	
        }
    iColumnValBufIt.Set(iColumnValueBuf);
	if(iParamCnt > 0)
		{
		err = iParamValueBuf.SetCount(iParamCnt);
		if(err != KErrNone)
			{
			return err;	
			}
		iParamValBufIt.Set(iParamValueBuf);
		}
	return KErrNone;
	}

/**
Template function, friend of CSqlStatementImpl, that is used for creation of CSqlStatementImpl objects.

@param aImpl A reference to a CSqlStatementImpl pointer. Will be initialized after a successfull CSqlStatementImpl construction.
@param aDatabase A reference to a CSqlDatabaseImpl object.
@param aSqlStmt The SQL statement: 8-bit or 16-bit.

@return KErrNoMemory, an out of memory condition has occurred;
 	    KErrArgument, bad argument, for example - the SQL string contains more than one SQL statements.
                      Note that the function may leave with some database specific errors categorised as 
                      ESqlDbError or other system-wide error codes;
        KErrNone      Operation has completed successfully.

@internalComponent
*/
template <class DES> TInt SqlCreateStatement(CSqlStatementImpl*& aImpl, CSqlDatabaseImpl& aDatabase, const DES& aSqlStmt)
	{
	TInt err = KErrNoMemory;
	aImpl = new CSqlStatementImpl;
	if(aImpl)
		{
		err = aImpl->Construct(aDatabase, aSqlStmt);
		}
	if(err != KErrNone)
		{
		delete aImpl;
		aImpl = NULL;
		}
	return err;
	}

/**
Factory function for creating CSqlStatementImpl objects.
Works with 16-bit SQL statements.

Creates CSqlStatementImpl instance and prepares the supplied SQL statement for execution.

Note that:
- CSqlStatementImpl can prepare and execute both parametrized SQL statements and SQL statements 
  without parameters;
- CSqlStatementImpl cannot prepare and execute SQL strings containing more than one SQL statement;

@param aImpl A reference to CSqlStatementImpl pointer which will be set to point to the created instance
@param aDatabase A reference to CSqlDatabaseImpl object.
@param aSqlStmt 16-bit string containing the SQL statement.

@return KErrNoMemory, an out of memory condition has occurred;
 	    KErrArgument, bad argument, for example - the SQL string contains more than one SQL statements.
                      Note that the function may leave with some database specific errors categorised as 
                      ESqlDbError or other system-wide error codes;
        KErrNone      Operation has completed successfully.

@see RSqlStatement
@see RSqlStatement::Prepare()
*/
inline TInt CSqlStatementImpl::New(CSqlStatementImpl*& aImpl, CSqlDatabaseImpl& aDatabase, const TDesC16& aSqlStmt)
	{
	return SqlCreateStatement(aImpl, aDatabase, aSqlStmt);
	}

/**
Factory function for creating CSqlStatementImpl objects.
Works with 8-bit SQL statements.

Creates CSqlStatementImpl instance and prepares the supplied SQL statement for execution.

Note that:
- CSqlStatementImpl can prepare and execute both parametrized SQL statements and SQL statements 
  without parameters;
- CSqlStatementImpl cannot prepare and execute SQL strings containing more than one SQL statement;

@param aImpl A reference to CSqlStatementImpl pointer which will be set to point to the created instance
@param aDatabase A reference to CSqlDatabaseImpl object.
@param aSqlStmt 8-bit string containing the SQL statement.

@return KErrNoMemory, an out of memory condition has occurred;
 	    KErrArgument, bad argument, for example - the SQL string contains more than one SQL statements.
                      Note that the function may leave with some database specific errors categorised as 
                      ESqlDbError or other system-wide error codes;
        KErrNone      Operation has completed successfully.

@see RSqlStatement
@see RSqlStatement::Prepare()
*/
inline TInt CSqlStatementImpl::New(CSqlStatementImpl*& aImpl, CSqlDatabaseImpl& aDatabase, const TDesC8& aSqlStmt)
	{
	return SqlCreateStatement(aImpl, aDatabase, aSqlStmt);
	}

/**
@return Non-zero if CSqlStatementImpl object points at a valid record, zero otherwise.

@see RSqlStatement::AtRow()
*/
inline TBool CSqlStatementImpl::AtRow() const
	{
	return iState == CSqlStatementImpl::EAtRow;
	}

/**
Implements RSqlStatement::ColumnCount().

@see RSqlStatement::ColumnCount().

@return The column count of the statement
*/	
inline TInt CSqlStatementImpl::ColumnCount() const
	{
	return iColumnCnt;
	}

/**
Gets the index (starting from 0) of the parameter with the given name.

The function does a case insensitive parameter name search.

If the parameter name is ":Prm", then the ":" prefix cannot be omitted when you call ParameterIndex().

This function can be called at any time after the SQL statement has been prepared.

@param aParamName Parameter name

@return the parameter index value, if successful - this is a non-negative integer value;
        KErrNotFound, if no such parameter can be found.
        One of the other system-wide error codes may also be returned.

@see RSqlStatement::ParamIndex()
*/
inline TInt CSqlStatementImpl::ParamIndex(const TDesC& aParamName)
	{
	return Name2Index(aParamName, iParamNameBuf, iParamCnt, ESqlSrvStmtParamNames, iParamNameBufPresent);
	}

/**
Gets the index (starting from 0) of the column with the given name.

The function does a case insensitive column name search.

This function can be called at any time after the SQL statement has been prepared.

@param aColumnName Column name

@return the column index value, if successful - this is a non-negative integer value;
        KErrNotFound, if no such parameter can be found.
        One of the other system-wide error codes may also be returned.

@see RSqlStatement::ColumnIndex()
*/
inline TInt CSqlStatementImpl::ColumnIndex(const TDesC& aColumnName)
	{
	return Name2Index(aColumnName, iColumnNameBuf, iColumnCnt, ESqlSrvStmtColumnNames, iColumnNameBufPresent);
	}

/**
Gives an access to the content of the requested column as a stream of bytes or characters.

The method creates a read-only MStreamBuf derived object which allows the column with aColumnIndex index 
to be accessed as a stream of bytes (if the column is a binary column) or characters 
(if the column is a text column) and returns it to the caller.

The caller is responsible for the destroying of the read-only MStreamBuf derived object.

ColumnSourceL() can be used only after successful Next() call (Next() returned KSqlAtRow),
otherwise the method issues panic 11.

@param aColumnIndex Column index (starting from 0)
@return A pointer to the created read-only memory MStreamBuf derived object.

@leave KErrNoMemory, an out of memory condition has occurred;

@panic SqlDb  5 Column index out of bounds.
@panic SqlDb  7 In _DEBUG mode. aColumnIndex index does not refer to a text or binary column.
@panic SqlDb 11 Statement object not positioned at row.

@see RSqlColumnReadStream
*/
inline MStreamBuf* CSqlStatementImpl::ColumnSourceL(TInt aColumnIndex)
	{
	__ASSERT_ALWAYS((TUint)aColumnIndex < (TUint)iColumnCnt, __SQLPANIC(ESqlPanicBadColumnIndex));
	__ASSERT_ALWAYS(iState == CSqlStatementImpl::EAtRow, __SQLPANIC(ESqlPanicInvalidRow));
	iColumnValBufIt.MoveTo(aColumnIndex);		
	return iColumnValBufIt.IsPresent() ? iColumnValBufIt.StreamL() : iSqlStmtSession.ColumnSourceL(aColumnIndex);
	}

/**
Gives an access to the content of the requested parameter as a stream of bytes or characters.

The method creates an IPC object with buffering capabilities, allowing to stream out the data of the 
parameter with aParameterIndex index and returns this MStreamBuf derived object to the caller.

The caller is responsible for the destroying of the MStreamBuf derived object.

@param aFunction Requested operation
@param aParamIndex Parameter index (starting from 0)

@return A pointer to the created MStreamBuf derived object.

@leave KErrNoMemory, an out of memory condition has occurred;

@panic SqlDb 5 Parameter index out of bounds.

@see RSqlParamWriteStream
*/
inline MStreamBuf* CSqlStatementImpl::ParamSinkL(TSqlSrvFunction aFunction, TInt aParamIndex)
	{
	__ASSERT_ALWAYS((TUint)aParamIndex < (TUint)iParamCnt, __SQLPANIC(ESqlPanicBadColumnIndex));
	return iSqlStmtSession.ParamSinkL(aFunction, aParamIndex);
	}

/**
Sets the internal state of CSqlStatementImpl instance to CSqlStatementImpl::EUnknown.
*/
inline CSqlStatementImpl::CSqlStatementImpl() :
	iBound(ETrue),
	iState(CSqlStatementImpl::EUnknown)
	{
	}
