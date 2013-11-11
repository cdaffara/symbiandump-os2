// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// NTT DOCOMO, INC - Fix for defect 1915 "SQL server panics when using long column type strings"
//
// Description:
//

#include "SqlStatementImpl.h" 	//CSqlStatementImpl
#include "SqlDatabaseImpl.h"	//CSqlDatabaseImpl::Session()

//Constants

_LIT(KTextKWD, 	"TEXT");
_LIT(KCharKWD, 	"CHAR");
_LIT(KClobKWD, 	"CLOB");

_LIT(KBinaryKWD,"BINARY");
_LIT(KBlobKWD,	"BLOB");

_LIT(KRealKWD, 	"REAL");
_LIT(KFloatKWD, "FLOAT");
_LIT(KDoubleKWD,"DOUBLE");

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////             RSqlLongColumnColl        ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
This "comparison" function is used for searching a particular column value, identified by aIndex, in the long column 
values collection.
@param aIndex An integer pointer to long column index value
@param aData A reference to particular elemt of the long column values collection
@return True of the index of aData item is the same as *aIndex, false otherwise
@panic SqlDb 4 In _DEBUG mode, aIndex is NULL.
@panic SqlDb 4 In _DEBUG mode, *aIndex value is negative.
*/
TBool RSqlLongColumnColl::TData::Compare(const TInt* aIndex, const RSqlLongColumnColl::TData& aData)
	{
	__ASSERT_DEBUG(aIndex != NULL, __SQLPANIC2(ESqlPanicBadArgument));
	__ASSERT_DEBUG(*aIndex >= 0, __SQLPANIC2(ESqlPanicBadArgument));
	return *aIndex == aData.iIndex;
	}

/**
Reads a long column value, identified by aColumnIndex parameter, from the server and stores the value in the collection.
@param aReader A RSqlLongColumnColl::TColumnReader object, which performs the "read column value" operation.
@param aColumnIndex Column index
@param aColumnSize The column size in bytes
@panic SqlDb 4 In _DEBUG mode, aColumnIndex value is negative.
@panic SqlDb 4 In _DEBUG mode, aColumnSize is less than KSqlMaxDesLen (not a long column value)
@return KErrNone The call has completed successfully;
		KErrNoMemory Out of memory;
        Note that other system-wide or database specific error codes may also be returned.
*/
TInt RSqlLongColumnColl::Append(RSqlLongColumnColl::TColumnReader& aReader, TInt aColumnIndex, TInt aColumnSize)
	{
	__ASSERT_DEBUG(aColumnIndex >= 0, __SQLPANIC(ESqlPanicBadArgument));
	__ASSERT_DEBUG(aColumnSize >= KSqlMaxDesLen, __SQLPANIC(ESqlPanicBadArgument));
	LONGCOL_INVARIANT();
	HBufC8* colBuf = HBufC8::New(aColumnSize);
	if(!colBuf)
		{
		return KErrNoMemory;	
		}
	TPtr8 ptr = colBuf->Des();
	TInt err = aReader.Read(aColumnIndex, ptr);
	if(err == KErrNone)
		{
		err = iValues.Append(RSqlLongColumnColl::TData(aColumnIndex, colBuf));	
		}
	if(err != KErrNone)
		{
		delete colBuf;	
		}
	LONGCOL_INVARIANT();
	return err;
	}
	
#ifdef _DEBUG
/**
RSqlLongColumnColl invariant.
The collection cannot have two column values with the same column index.
The collection cannot have NULL column value, or negative column index.
*/
void RSqlLongColumnColl::Invariant() const
	{
	for(TInt i=iValues.Count()-1;i>=0;--i)
		{
		const RSqlLongColumnColl::TData& data = iValues[i];
		__ASSERT_DEBUG(data.iIndex >= 0, __SQLPANIC(ESqlPanicInternalError));
		__ASSERT_DEBUG(data.iData != NULL, __SQLPANIC(ESqlPanicInternalError));
		for(TInt j=i-1;j>=0;--j)
			{
			__ASSERT_DEBUG(data.iIndex != iValues[j].iIndex, __SQLPANIC(ESqlPanicInternalError));
			}
		}
	}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////           CSqlStatementImpl           ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
RSqlLongColumnColl::TColumnReader derived class, which implements the pure virtual RSqlLongColumnColl::TColumnReader::Read().
The class is used for retrieving long column values from the server.
@internalComponent
*/
NONSHARABLE_STRUCT(TSqlLongColumnReader) : public RSqlLongColumnColl::TColumnReader
	{
	TSqlLongColumnReader(RSqlStatementSession& aStmtSession) :
		iStmtSession(aStmtSession)
		{
		}
	virtual TInt Read(TInt aColumnIndex, TDes8& aBuf)
		{
		return iStmtSession.ReadColumnValue(aColumnIndex, aBuf);
		}
	RSqlStatementSession	iStmtSession;
	};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////           CSqlStatementImpl           ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Frees the allocated memory and other resources.
*/
CSqlStatementImpl::~CSqlStatementImpl()
	{
	iDeclaredColumnTypes.Close();
	iLongColumnColl.Close();
	iParamValueBuf.Close();
	iParamNameBuf.Close();
	iColumnValueBuf.Close();
	iColumnNameBuf.Close();
	iSqlStmtSession.Close();
	}



/**
Resets the prepared SQL statement back to its initial state and makes it ready for re-execution.
The parameters of the SQL statement retain their values.
If CSqlStatementImpl object processes parametrized SQL statement, the parameter values can be bound after 
the Reset() call.

The method sets the internal object's state to CSqlStatementImpl::EUnknown. The column data accessors
cannot be used until the next successful Next() call.

@return KSqlErrStmtExpired, statement expired (if new functions or collating sequences are 
							registered or if an authorizer function is added or changed);
		KErrNone, the operation has completed successfully.

@see RSqlStatement::Reset()
*/
TInt CSqlStatementImpl::Reset()
	{
	iState = CSqlStatementImpl::EUnknown;
	iLongColumnColl.Reset();
	if(iParamCnt > 0)
		{
		iParamValueBuf.ResetAndMinimize();
		}
	return iSqlStmtSession.Reset();
	}

/**
Executes the prepared DDL/DML SQL statement. 

If the SQL statement contains parameters, their values will be bound right before the execution.

The method sets the internal object's state to CSqlStatementImpl::EUnknown. The column data accessors
cannot be used until the next successful Next() call.

@return >=0, The operation has completed successfully. The number of database rows that were 
			 changed/inserted/deleted by the most recently completed DDL/DML sql statement.
			 Exception: If the executed statement is "DELETE FROM <table>", then the function returns 0 
			 if the operation has completed successfully (disregarding the number of the deleted rows);
	    KSqlErrStmtExpired, statement has expired (if new functions or collating sequences are 
							registered or if an authorizer function is added or changed);
		KErrNoMemory, an out of memory condition has occurred.
                      Note that database specific errors categorised as ESqlDbError, and
                      other system-wide error codes may also be returned.

@see RSqlStatement::Exec()
*/
TInt CSqlStatementImpl::Exec()
	{
	iState = CSqlStatementImpl::EUnknown;
	TInt rc = 0;
	if(iBound)
		{
		rc = iSqlStmtSession.Exec();	
		}
	else
		{
		rc = iSqlStmtSession.BindExec(iParamValueBuf);
		iBound = ETrue;
		iParamValueBuf.ResetAndMinimize();
		}
	return rc;
	}

/**
Executes the prepared DDL/DML SQL statement asynchronously. 

If the SQL statement contains parameters, their values will be bound right before the execution.

The method sets the internal object's state to CSqlStatementImpl::EUnknown. The column data accessors
cannot be used until the next successful Next() call.

@param aStatus Completion status of asynchronous request, one of the following:
		 >=0, The operation has completed successfully. The number of database rows that were 
		   	   changed/inserted/deleted by the most recently completed DDL/DML sql statement.
			 Exception: If the executed statement is "DELETE FROM <table>", then the function returns 0 
			 if the operation has completed successfully (disregarding the number of the deleted rows);
         KSqlErrStmtExpired, the SQL statement has expired (if new functions or
                            collating sequences have been registered or if an
                            authorizer function has been added or changed);
         KErrNoMemory, an out of memory condition has occurred - the statement
                      will be reset.
                      Note that aStatus may be set with database specific errors categorised as ESqlDbError, 
                      and other system-wide error codes.


@see RSqlStatement::Exec()
*/
void CSqlStatementImpl::Exec(TRequestStatus& aStatus)
	{
	iState = CSqlStatementImpl::EUnknown;
	if(iBound)
		{
		iSqlStmtSession.Exec(aStatus);
		}
	else
		{
		iSqlStmtSession.BindExec(iParamValueBuf, aStatus);
		iBound = ETrue;
		//Here: it is not possible to reset iParamValueBuf for "safety", because this is an asynchronous call
		//      and the buffer content has to be preserved until the operation completes.
		}
	}

/**
Retrieves a record.

If the prepared SQL statement is a "SELECT" statement, and is expected to
return a set of records, then this function can be used to retrieve that record data.

If the SQL statement contains parameters, then their values must be bound before
this function is called.

If the call to this function completes successfully, i.e. it returns
with KSqlAtRow, then this CSqlStatementImpl object contains the record data, and 
this data will remain valid for access until another call is made to any
CSqlStatementImpl function.

The record data can be accessed using the following functions:
- CSqlStatementImpl::ColumnType()
- CSqlStatementImpl::ColumnSize()
- CSqlStatementImpl::Column<Type>()

@return KSqlAtRow,      the record data is ready for processing by the caller;
        KSqlAtEnd,      there is no more record data;
        KSqlErrBusy,    the database file is locked;
        KErrNoMemory,   an out of memory condition has occurred - the statement
                        will be reset;
        KSqlErrGeneral, a run-time error has occured - this function must not
                        be called again;        
        KSqlErrMisuse,  this function has been called after a previous call
                        returned KSqlAtEnd or KSqlErrGeneral.
        KSqlErrStmtExpired, the SQL statement has expired (if new functions or
                            collating sequences have been registered or if an
                            authorizer function has been added or changed);

@see RSqlStatement::Next()
*/
TInt CSqlStatementImpl::Next()
	{
	iLongColumnColl.Reset();
	iColumnValueBuf.ResetAndMinimize();
	iState = CSqlStatementImpl::EUnknown;
	TInt err = KErrNone;
	if(iBound)
		{
		err = iSqlStmtSession.Next(iColumnValueBuf);
		}
	else
		{
		err = iSqlStmtSession.BindNext(iParamValueBuf, iColumnValueBuf);
		iBound = ETrue;
		iParamValueBuf.ResetAndMinimize();
		}
	iColumnValBufIt.Set(iColumnValueBuf);
	if(err == KSqlAtRow)
		{
		iState = CSqlStatementImpl::EAtRow;
		}
	return err;
	}

/**
Implements RSqlStatement::BindNull().

@panic SqlDb 5 Parameter index out of bounds.

@see RSqlStatement::BindNull()
*/	
TInt CSqlStatementImpl::BindNull(TInt aParamIndex)
	{
	__ASSERT_ALWAYS((TUint)aParamIndex < (TUint)iParamCnt, __SQLPANIC(ESqlPanicBadColumnIndex));
	iBound = EFalse;
	iParamValBufIt.MoveTo(aParamIndex);
	iParamValBufIt.SetNull();
	return KErrNone;
	}
	
/**
Implements RSqlStatement::BindInt().

@panic SqlDb 5 Parameter index out of bounds.

@see RSqlStatement::BindInt()
*/	
TInt CSqlStatementImpl::BindInt(TInt aParamIndex, TInt aParamValue)
	{
	__ASSERT_ALWAYS((TUint)aParamIndex < (TUint)iParamCnt, __SQLPANIC(ESqlPanicBadColumnIndex));
	iBound = EFalse;
	iParamValBufIt.MoveTo(aParamIndex);
	return iParamValBufIt.SetInt(aParamValue);
	}
	
/**
Implements RSqlStatement::BindInt64().

@panic SqlDb 5 Parameter index out of bounds.

@see RSqlStatement::BindInt64()
*/	
TInt CSqlStatementImpl::BindInt64(TInt aParamIndex, TInt64 aParamValue)
	{
	__ASSERT_ALWAYS((TUint)aParamIndex < (TUint)iParamCnt, __SQLPANIC(ESqlPanicBadColumnIndex));
	iBound = EFalse;
	iParamValBufIt.MoveTo(aParamIndex);
	return iParamValBufIt.SetInt64(aParamValue);
	}
	
/**
Implements RSqlStatement::BindReal().

@panic SqlDb 5 Parameter index out of bounds.

@see RSqlStatement::BindReal()
*/	
TInt CSqlStatementImpl::BindReal(TInt aParamIndex, TReal aParamValue)
	{
	__ASSERT_ALWAYS((TUint)aParamIndex < (TUint)iParamCnt, __SQLPANIC(ESqlPanicBadColumnIndex));
	iBound = EFalse;
	iParamValBufIt.MoveTo(aParamIndex);
	return iParamValBufIt.SetReal(aParamValue);
	}
	
/**
Implements RSqlStatement::BindText().

@panic SqlDb 5 Parameter index out of bounds.

@see RSqlStatement::BindText()
*/	
TInt CSqlStatementImpl::BindText(TInt aParamIndex, const TDesC& aParamText)
	{
	__ASSERT_ALWAYS((TUint)aParamIndex < (TUint)iParamCnt, __SQLPANIC(ESqlPanicBadColumnIndex));
	iBound = EFalse;
	iParamValBufIt.MoveTo(aParamIndex);
	return iParamValBufIt.SetText(aParamText);
	}
	
/**
Implements RSqlStatement::BindBinary().

@panic SqlDb 5 Parameter index out of bounds.

@see RSqlStatement::BindBinary()
*/	
TInt CSqlStatementImpl::BindBinary(TInt aParamIndex, const TDesC8& aParamData)
	{
	__ASSERT_ALWAYS((TUint)aParamIndex < (TUint)iParamCnt, __SQLPANIC(ESqlPanicBadColumnIndex));
	iBound = EFalse;
	iParamValBufIt.MoveTo(aParamIndex);
	return iParamValBufIt.SetBinary(aParamData);
	}

/**
Implements RSqlStatement::BindZeroBlob().

@panic SqlDb 5 Parameter index out of bounds.

@see RSqlStatement::BindZeroBlob()
*/	
TInt CSqlStatementImpl::BindZeroBlob(TInt aParamIndex, TInt aBlobSize)
	{
	__ASSERT_ALWAYS((TUint)aParamIndex < (TUint)iParamCnt, __SQLPANIC(ESqlPanicBadColumnIndex));
	iBound = EFalse;
	iParamValBufIt.MoveTo(aParamIndex);
	return iParamValBufIt.SetZeroBlob(aBlobSize);
	}

/**
Implements RSqlStatement::ColumnType().

@panic SqlDb 5 Column index out of bounds.
@panic SqlDb 11 Statement cursor not positioned on a row.

@see RSqlStatement::ColumnType().
*/	
TSqlColumnType CSqlStatementImpl::ColumnType(TInt aColumnIndex)
	{
	__ASSERT_ALWAYS((TUint)aColumnIndex < (TUint)iColumnCnt, __SQLPANIC(ESqlPanicBadColumnIndex));
	__ASSERT_ALWAYS(iState == CSqlStatementImpl::EAtRow, __SQLPANIC(ESqlPanicInvalidRow));
	iColumnValBufIt.MoveTo(aColumnIndex);		
	return static_cast <TSqlColumnType> (iColumnValBufIt.Type());
	}

/**
Implements RSqlStatement::DeclaredColumnType().

@param aColumnIndex The index value identifying the column. This is 0 for the first column.
@param aColumnType	Output parameter. Upon completion this will contain the type of the column.

@return KErrNone, the operation completed successfully;
        KErrNoMemory, an out of memory condition has occurred.
                  One of the other system-wide error codes may also be returned.

@panic SqlDb 5 Column index out of bounds.

@see RSqlStatement::DeclaredColumnType().
*/
TInt CSqlStatementImpl::DeclaredColumnType(TInt aColumnIndex, TSqlColumnType& aColumnType)
	{
	__ASSERT_ALWAYS((TUint)aColumnIndex < (TUint)iColumnCnt, __SQLPANIC(ESqlPanicBadColumnIndex));
	if(iDeclaredColumnTypes.Count() == 0) //initialise iDeclaredColumnTypes array if necessary
		{
		RSqlBufFlat declaredColumnTypeBuf;
		TInt err = declaredColumnTypeBuf.SetCount(iColumnCnt);
		if(err != KErrNone)
			{
			declaredColumnTypeBuf.Close();
			return err;
			}
		
		//Resize buffer to minimise the chance that two IPC calls are required to get all the column type name data.
		//Allocates enough space to contain the header cells (already created by calling SetCount()) and a data buffer
		//which assumes an average column type name length of 20 characters plus 8-byte alignment padding
		const TInt KBufSizePerColumn = 48;
		TInt newSize = declaredColumnTypeBuf.Size() + iColumnCnt*KBufSizePerColumn;

		err = declaredColumnTypeBuf.ReAlloc(newSize);
		if(err != KErrNone)
			{
			declaredColumnTypeBuf.Close();
			return err;	
			}
		
		err = iSqlStmtSession.GetDeclColumnTypes(declaredColumnTypeBuf);
		if(err != KErrNone)
			{
			declaredColumnTypeBuf.Close();
			return err;	
			}

		err = iDeclaredColumnTypes.Reserve(iColumnCnt);//We know what the array size should be - iColumnCnt
		if(err != KErrNone)
			{
			declaredColumnTypeBuf.Close();
			return err;
			}	

		//Iterate over the column type names buffer and map each column type name to one of the TSqlColumnType enum item values.
		TSqlBufRIterator declColumnTypeBufIt;
		declColumnTypeBufIt.Set(declaredColumnTypeBuf);
		TInt colIdx = 0;
		while(declColumnTypeBufIt.Next())
			{
			TPtrC colTypeName(declColumnTypeBufIt.Text());
			TSqlColumnType colType = ESqlInt;
			if(colTypeName.FindF(KCharKWD) >= 0 || colTypeName.FindF(KTextKWD) >= 0 || colTypeName.FindF(KClobKWD) >= 0)
				{
				colType = ESqlText;
				}
			else if(colTypeName.FindF(KBinaryKWD) >= 0 || colTypeName.FindF(KBlobKWD) >= 0)
				{
				colType = ESqlBinary;
				}
			else if(colTypeName.FindF(KRealKWD) >= 0 || colTypeName.FindF(KFloatKWD) >= 0 || colTypeName.FindF(KDoubleKWD) >= 0)
				{
				colType = ESqlReal;
				}
			err = iDeclaredColumnTypes.Append(colType);
			__ASSERT_DEBUG(err == KErrNone, __SQLPANIC(ESqlPanicInternalError));//memory for the array elements has been reserved already
			++colIdx;
			} //end of - while(declColumnTypeBufIt.Next())
		__ASSERT_DEBUG(colIdx == iColumnCnt, __SQLPANIC(ESqlPanicInternalError));
		declaredColumnTypeBuf.Close();
		} //end of - if(iDeclaredColumnTypes.Count() == 0 && iColumnCnt > 0)
	aColumnType = iDeclaredColumnTypes[aColumnIndex];
	return KErrNone;
	}
	
/**
Implements RSqlStatement::ColumnSize().

@panic SqlDb 5 Column index out of bounds.
@panic SqlDb 11 Statement cursor not positioned on a row

@see RSqlStatement::ColumnSize().
*/	
TInt CSqlStatementImpl::ColumnSize(TInt aColumnIndex)
	{
	__ASSERT_ALWAYS((TUint)aColumnIndex < (TUint)iColumnCnt, __SQLPANIC(ESqlPanicBadColumnIndex));
	__ASSERT_ALWAYS(iState == CSqlStatementImpl::EAtRow, __SQLPANIC(ESqlPanicInvalidRow));
	iColumnValBufIt.MoveTo(aColumnIndex);		
	return iColumnValBufIt.Size();
	}

/**
Implements RSqlStatement::ColumnInt().

@panic SqlDb 5 Column index out of bounds.
@panic SqlDb 11 Statement cursor not positioned on a row

@see RSqlStatement::ColumnInt().
*/	
TInt CSqlStatementImpl::ColumnInt(TInt aColumnIndex)
	{
	__ASSERT_ALWAYS((TUint)aColumnIndex < (TUint)iColumnCnt, __SQLPANIC(ESqlPanicBadColumnIndex));
	__ASSERT_ALWAYS(iState == CSqlStatementImpl::EAtRow, __SQLPANIC(ESqlPanicInvalidRow));
	iColumnValBufIt.MoveTo(aColumnIndex);		
	__ASSERT_DEBUG(iColumnValBufIt.IsPresent(), __SQLPANIC(ESqlPanicValueNotPresent));
	return iColumnValBufIt.Int();
	}
	
/**
Implements RSqlStatement::ColumnInt64().

@panic SqlDb 5 Column index out of bounds.
@panic SqlDb 11 Statement cursor not positioned on a row

@see RSqlStatement::ColumnInt64().
*/	
TInt64 CSqlStatementImpl::ColumnInt64(TInt aColumnIndex)
	{
	__ASSERT_ALWAYS((TUint)aColumnIndex < (TUint)iColumnCnt, __SQLPANIC(ESqlPanicBadColumnIndex));
	__ASSERT_ALWAYS(iState == CSqlStatementImpl::EAtRow, __SQLPANIC(ESqlPanicInvalidRow));
	iColumnValBufIt.MoveTo(aColumnIndex);		
	__ASSERT_DEBUG(iColumnValBufIt.IsPresent(), __SQLPANIC(ESqlPanicValueNotPresent));
	return iColumnValBufIt.Int64();
	}
	
/**
Implements RSqlStatement::ColumnReal().

@panic SqlDb 5 Column index out of bounds.
@panic SqlDb 11 Statement cursor not positioned on a row

@see RSqlStatement::ColumnReal().
*/	
TReal CSqlStatementImpl::ColumnReal(TInt aColumnIndex)
	{
	__ASSERT_ALWAYS((TUint)aColumnIndex < (TUint)iColumnCnt, __SQLPANIC(ESqlPanicBadColumnIndex));
	__ASSERT_ALWAYS(iState == CSqlStatementImpl::EAtRow, __SQLPANIC(ESqlPanicInvalidRow));
	iColumnValBufIt.MoveTo(aColumnIndex);		
	__ASSERT_DEBUG(iColumnValBufIt.IsPresent(), __SQLPANIC(ESqlPanicValueNotPresent));
	return iColumnValBufIt.Real();
	}

/**
Return a text (16 bit) descriptor to a text column identified by aColumnIndex.

@param aColumnIndex Column index
@param aPtr An output parameter which will be set to point to the column data.

@return KErrNone, if the function completes successfully,
                  otherwise one of the other system-wide error codes.

@panic SqlDb 5 Column index out of bounds.
@panic SqlDb 11 Statement cursor not positioned on a row
*/	
TInt CSqlStatementImpl::ColumnText(TInt aColumnIndex, TPtrC& aPtr)
	{
	__ASSERT_ALWAYS((TUint)aColumnIndex < (TUint)iColumnCnt, __SQLPANIC(ESqlPanicBadColumnIndex));
	__ASSERT_ALWAYS(iState == CSqlStatementImpl::EAtRow, __SQLPANIC(ESqlPanicInvalidRow));
	iColumnValBufIt.MoveTo(aColumnIndex);		
	if(iColumnValBufIt.IsPresent())
		{
		aPtr.Set(iColumnValBufIt.Text());
		return KErrNone;
		}
	//The text column value has not been transferred to the client side if its length is >= KSqlMaxDesLen characters.
	//In this case an additional call to the server is made to get the column value.
	//The column value is stored in a separate collection (iLongColumnColl), because if iColumnValueBuf gets reallocated,
	//the client can get a dangling pointer to some of the located in iColumnValueBuf text/binary column values.
	if(iColumnValBufIt.Type() != ESqlText)
		{
		aPtr.Set(KNullDesC);
		return KErrNone;
		}
	if(!iLongColumnColl.IsPresent(aColumnIndex))
		{
		TSqlLongColumnReader colReader(iSqlStmtSession);
		TInt err = iLongColumnColl.Append(colReader, aColumnIndex, iColumnValBufIt.Size() * sizeof(TUint16));
		if(err != KErrNone)
			{
			return err;	
			}
		}
	aPtr.Set(iLongColumnColl.Text(aColumnIndex));
	return KErrNone;
	}

/**
Copies the content of a text column, identified by aColumnIndex, to the place refered by aDest parameter.

If the destination buffer is not big enough, the function will copy as much data as possible and will
return KErrOverflow.

@param aColumnIndex Column index
@param aDest Refers to the place where the column data will be copied.

@return KErrNone, if the function completes successfully,
                  otherwise one of the other system-wide error codes.

@panic SqlDb 5 Column index out of bounds.
@panic SqlDb 11 Statement cursor not positioned on a row
*/	
TInt CSqlStatementImpl::ColumnText(TInt aColumnIndex, TDes& aDest)
	{
	__ASSERT_ALWAYS((TUint)aColumnIndex < (TUint)iColumnCnt, __SQLPANIC(ESqlPanicBadColumnIndex));
	__ASSERT_ALWAYS(iState == CSqlStatementImpl::EAtRow, __SQLPANIC(ESqlPanicInvalidRow));
	iColumnValBufIt.MoveTo(aColumnIndex);		
	TInt err = KErrNone;
	//The text column value has not been transferred to the client side if its length is >= KSqlMaxDesLen characters.
	//In this case an additional call to the server is made to get the column value.
	if(!iColumnValBufIt.IsPresent())
		{
		if(iColumnValBufIt.Type() != ESqlText)
			{
			aDest.Zero();
			return err;
			}
		TPtr8 ptr(reinterpret_cast <TUint8*> (const_cast <TUint16*> (aDest.Ptr())), aDest.MaxLength() * sizeof(TUint16));
		err = iSqlStmtSession.ReadColumnValue(aColumnIndex, ptr);
		switch(err)
		    {
	        case KErrNone:
	        case KErrOverflow:
	            aDest.SetLength(ptr.Length() / sizeof(TUint16));
	            break;
	        default:
	            break;
		    }
		}
	else
		{
		TPtrC src = iColumnValBufIt.Text();
		TInt len = src.Length();
		if(len > aDest.MaxLength())
			{
			len = aDest.MaxLength();
			err = KErrOverflow;
			}
		aDest.Copy(src.Ptr(), len);
		}
	return err;
	}

/**
Return a binary (8 bit) descriptor to a binary column identified by aColumnIndex.

@param aColumnIndex Column index
@param aPtr An output parameter which will be set to point to the column data.

@return KErrNone, if the function completes successfully,
                  otherwise one of the other system-wide error codes.

@panic SqlDb 5 Column index out of bounds.
@panic SqlDb 11 Statement cursor not positioned on a row
*/	
TInt CSqlStatementImpl::ColumnBinary(TInt aColumnIndex, TPtrC8& aPtr)
	{
	__ASSERT_ALWAYS((TUint)aColumnIndex < (TUint)iColumnCnt, __SQLPANIC(ESqlPanicBadColumnIndex));
	__ASSERT_ALWAYS(iState == CSqlStatementImpl::EAtRow, __SQLPANIC(ESqlPanicInvalidRow));
	iColumnValBufIt.MoveTo(aColumnIndex);		
	if(iColumnValBufIt.IsPresent())
		{
		aPtr.Set(iColumnValBufIt.Binary());
		return KErrNone;
		}
	//The binary column value has not been transferred to the client side if its length is >= KSqlMaxDesLen characters.
	//In this case an additional call to the server is made to get the column value.
	//The column value is stored in a separate collection (iLongColumnColl), because if iColumnValueBuf gets reallocated,
	//the client can get a dangling pointer to some of the located in iColumnValueBuf text/binary column values.
	if(iColumnValBufIt.Type() != ESqlBinary)
		{
		aPtr.Set(KNullDesC8);
		return KErrNone;
		}
	if(!iLongColumnColl.IsPresent(aColumnIndex))
		{
		TSqlLongColumnReader colReader(iSqlStmtSession);
		TInt err = iLongColumnColl.Append(colReader, aColumnIndex, iColumnValBufIt.Size());
		if(err != KErrNone)
			{
			return err;	
			}
		}
	aPtr.Set(iLongColumnColl.Binary(aColumnIndex));
	return KErrNone;
	}

/**
Copies the content of a binary column, identified by aColumnIndex, to the place refered by aDest parameter.

If the destination buffer is not big enough, the function will copy as much data as possible and will
return KErrOverflow.

@param aColumnIndex Column index
@param aDest Refers to the place where the column data will be copied.

@return KErrNone, if the function completes successfully,
                  otherwise one of the other system-wide error codes.

@panic SqlDb 5 Column index out of bounds.
@panic SqlDb 11 Statement cursor not positioned on a row
*/	
TInt CSqlStatementImpl::ColumnBinary(TInt aColumnIndex, TDes8& aDest)
	{
	__ASSERT_ALWAYS((TUint)aColumnIndex < (TUint)iColumnCnt, __SQLPANIC(ESqlPanicBadColumnIndex));
	__ASSERT_ALWAYS(iState == CSqlStatementImpl::EAtRow, __SQLPANIC(ESqlPanicInvalidRow));
	iColumnValBufIt.MoveTo(aColumnIndex);		
	TInt err = KErrNone;
	//The binary column value has not been transferred to the client side if its length is >= KSqlMaxDesLen bytes.
	//In this case an additional call to the server is made to get the column value.
	if(!iColumnValBufIt.IsPresent())
		{
		if(iColumnValBufIt.Type() != ESqlBinary)
			{
			aDest.Zero();
			return err;
			}
		err = iSqlStmtSession.ReadColumnValue(aColumnIndex, aDest);
		}
	else
		{
		TPtrC8 src = iColumnValBufIt.Binary();
		TInt len = src.Length();
		if(len > aDest.MaxLength())
			{
			len = aDest.MaxLength();
			err = KErrOverflow;
			}
		aDest.Copy(src.Ptr(), len);
		}
	return err;
	}

/**
 * Obtain the name of a column after performing a query.
 * 
 * @param aColumnIndex Column index
 * @param aNameDest Descriptor which will be set to column name
 * @return KErrNone if successfull or one of the system-wide error codes on error
 */
TInt CSqlStatementImpl::ColumnName(TInt aColumnIndex, TPtrC& aNameDest)
	{
	return Index2Name(aColumnIndex, iColumnNameBuf, iColumnCnt, ESqlSrvStmtColumnNames, iColumnNameBufPresent, aNameDest);
	}

/**
 * Obtain the name of a parameter after preparing a DML query.
 * 
 * @param aParamIndex Parameter index
 * @param aNameDest Descriptor which will be set to column name
 * @return KErrNone if successfull or one of the system-wide error codes on error
 */
TInt CSqlStatementImpl::ParameterName(TInt aParamIndex, TPtrC& aNameDest)
	{
	return Index2Name(aParamIndex, iParamNameBuf, iParamCnt, ESqlSrvStmtParamNames, iParamNameBufPresent, aNameDest);
	}


TInt CSqlStatementImpl::Name2Index(const TDesC& aName, RSqlBufFlat& aNameBufFlat, TInt aCount, TSqlSrvFunction aFunction, TBool& aPresent)
	{
	if(aCount == 0)
		{
		return KErrNotFound;	
		}
	TInt err = CheckNameBufPresent(aPresent, aNameBufFlat, aCount, aFunction);
	if ( err != KErrNone ) 
		{
		return err;
		}
	TInt idx = -1;
	aPresent = ETrue;	
	TSqlBufRIterator nameBufIt;
	nameBufIt.Set(aNameBufFlat);
	while(nameBufIt.Next())
		{
		++idx;
		if(::CompareNoCase16(aName, nameBufIt.Text()) == 0)
			{
			return idx;	
			}
		}
	return KErrNotFound;
	}

                   
TInt CSqlStatementImpl::Index2Name(TInt aIndex, RSqlBufFlat& aNameBufFlat, TInt aCount, TSqlSrvFunction aFunction, TBool& aPresent, TPtrC& aColName)
	{
	if( aCount == 0 || aIndex < 0 || aIndex >= aCount ) 
		{
		return KErrNotFound;	
		}
	TInt err = CheckNameBufPresent(aPresent, aNameBufFlat, aCount, aFunction);
	if ( err != KErrNone ) 
		{
		return err;
		}
	aPresent = ETrue;	
	TSqlBufRIterator nameBufIt;
	nameBufIt.Set(aNameBufFlat);
	nameBufIt.MoveTo(aIndex);
	aColName.Set(nameBufIt.Text());
	return KErrNone;
	}

/**
Ensures that the specified names buffer is present on the client side.
Name buffers are used for transporting parameter and column names.

@return KErrNone if successfull or one of the system-wide error codes on error
@internalComponent
*/
TInt CSqlStatementImpl::CheckNameBufPresent(TBool& aPresent, RSqlBufFlat& aNameBufFlat, TInt aCount, TSqlSrvFunction aFunction)
	{
	if(!aPresent)
		{
		aNameBufFlat.Close();
		TInt err = aNameBufFlat.SetCount(aCount);
		if(err != KErrNone)
			{
			return err;	
			}
		err = iSqlStmtSession.GetNames(aFunction, aNameBufFlat);
		if(err != KErrNone)
			{
			return err;	
			}
		}
	return KErrNone;
	}

