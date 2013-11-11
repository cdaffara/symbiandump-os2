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
// Client incremental class
// 
//

#include "UD_STD.H"

// Class RDbIncremental

/** Releases the resources used by the incremental operations object. If the operation 
is not yet complete, then the operation is abandoned and the database is rolled 
back to its state before the operation started. */
EXPORT_C void RDbIncremental::Close()
	{
	iState.Close();
	}

/** Performs the next step in the incremental operation, returning when the step 
is complete.

@param aStep Initially, contains the value returned from any of the initiating 
functions or the last call to perform an operational step. On return, contains 
a value which is less than or equal to the initial value. If it equals 0, 
then the operation has completed successfully and the incremental object should 
be closed.
@return KErrNone if successful, or one of the DBMS database error codes. If 
this indicates an error, then the incremental object should be closed and 
the operation aborted. */
EXPORT_C TInt RDbIncremental::Next(TInt& aStep)
	{
	__ASSERT_ALWAYS(aStep>0,Panic(EDbInvalidIncrementalStep));
	TRAPD(r,iState->NextL(aStep));
	return r;
	}

/** Performs the next step in the incremental operation, returning immediately 
and signalling the request status when the step is complete. 

This function is most effectively used when the incremental operation is packaged 
as an active object.

Note that the step parameter is packaged to enable this API to work for asynchronous 
calls in client/server implementations of DBMS.

@param aStep Initially, contains the value returned from any of the initiating 
functions or the last call to perform an operational step. On return, contains 
a value which is less than or equal to the initial value. If it equals 0, 
then the operation has completed successfully and the incremental object should 
be closed.
@param aStatus The request status used to contain completion information for 
the function. On completion, contains the completion status or one of the 
DBMS database error codes. If this indicates an error, then the incremental 
object should be closed and the operation aborted.
@see TPckgBuf */
EXPORT_C void RDbIncremental::Next(TPckgBuf<TInt>& aStep,TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(aStep.operator()()>0,Panic(EDbInvalidIncrementalStep));
	iState->Next(aStep,aStatus);
	}

LOCAL_C TInt Utility(RDbHandle<CDbIncremental>& aInc
						 ,RDbHandle<CDbDatabase>& aDb,TInt& aStep
						 ,CDbDatabase::TUtility aType)
	{
	TRAPD(r,aInc=aDb->UtilityL(aType,aStep));
	return r;
	}

/** Initiates a database recovery operation.

This is the incremental version of RDbDatabase::Recover().
Recover() will try to rebuild database indexes if they are broken.
If the database data is corrupted, it cannot be recovered.

@param aDatabase The database to recover.
@param aStep On return, contains the initial step count for the incremental 
operation. This value should be passed in to subsequent calls to NextL().

@return KErrNone if successful, or one of the DBMS database error codes. If 
recovery fails with either KErrNoMemory or KErrDiskFull, then recovery may 
be attempted again when more memory or disk space is available.

@see RDbDatabase::Recover() 

@capability Note For a secure shared database, the caller must satisfy the schema 
            access policy for the database.
*/
EXPORT_C TInt RDbIncremental::Recover(RDbDatabase& aDatabase,TInt& aStep)
	{
	return Utility(iState,aDatabase.iDatabase,aStep,CDbDatabase::ERecover);
	}

/** Initiates the operation of calculating and updating database statistics.

This is the incremental form of RDbDatabase::UpdateStats()

@param aDatabase The database whose statistics are to be updated.
@param aStep On return, contains the initial step count for the incremental 
operation. This value should be passed in to subsequent calls to Next() to 
continue the operation.
@return KErrNone if successful, otherwise another of the system-wide error 
codes.
@see RDbDatabase::UpdateStats() 

@capability Note For a secure shared database, the caller must satisfy the schema 
            access policy for the database.
*/
EXPORT_C TInt RDbIncremental::UpdateStats(RDbDatabase& aDatabase,TInt& aStep)
	{
	return Utility(iState,aDatabase.iDatabase,aStep,CDbDatabase::EStats);
	}

/** Initiates the operation of compacting a database. This is the incremental form 
of RDbDatabase::Compact().

@param aDatabase The database to compact.
@param aStep On return, contains the initial step count for the incremental 
operation. This value should be passed in to subsequent calls to Next() to 
continue the operation.
@return KErrNone if successful, otherwise another of the system-wide error 
codes.
@see RDbDatabase::Compact() 

@capability Note For a secure shared database, the caller must satisfy the schema 
            access policy for the database.
*/
EXPORT_C TInt RDbIncremental::Compact(RDbDatabase& aDatabase,TInt& aStep)
	{
	return Utility(iState,aDatabase.iDatabase,aStep,CDbDatabase::ECompact);
	}

/** Initiates a table discard operation on a database. All indexes belonging to 
the table are also discarded as part of this operation.

This is the incremental version of RDbDatabase::DropTable().

@param aDatabase The database from which to drop the table.
@param aTable The name of the table to drop.
@param aStep On return, contains the initial step count for the incremental 
operation. This value should be passed in to subsequent calls to NextL().
@return KErrNone if successful, or one of the DBMS database error codes. The 
Store database always returns KErrNotSupported for this function.
@see RDbDatabase::DropTable() 

@capability Note For a secure shared database, the caller must satisfy the schema 
            access policy for the database.
*/
EXPORT_C TInt RDbIncremental::DropTable(RDbDatabase& aDatabase,const TDesC& aTable,TInt& aStep)
	{
	TRAPD(r,iState=aDatabase.iDatabase->DropTableL(aTable,aStep));
	return r;
	}

/** Initiates a table alteration operation on a database. This is the incremental 
form of RDbDatabase::AlterTable().

@param aDatabase The database which has the table to be altered.
@param aTable The name of the table which is to be altered.
@param aNewDef A column set describing the new definition for the table.
@param aStep On return, contains the initial step count for the incremental 
operation. This value should be passed in to subsequent calls to NextL().
@return KErrNone if successful, or one of the DBMS database error codes. Specifically, 
the function returns: KErrNotFound, if the table does not exist in the database. 
KErrBadName if a column name is invalid. KErrArgument if the new column set 
is empty, or there are duplicate column names, or if a column's maximum length 
is non-positive but not KDbUndefinedLength, or a non-numeric column has the 
auto-increment attribute, or an indexed column has been dropped, or a column 
has changed its type or attributes, or a not-null or auto-increment column 
has been added to a table which is not empty. KErrNotSupported if a column 
type is out of the recognised range, or an unknown attribute bit is set, or 
the maximum length for a Text8, Text16 or Binary column is more than 255. 
KErrTooBig if the resulting record size can be larger than 8200 bytes.
@see RDbDatabase::AlterTable() 

@capability Note For a secure shared database, the caller must satisfy the schema 
            access policy for the database.
*/
EXPORT_C TInt RDbIncremental::AlterTable(RDbDatabase& aDatabase,const TDesC& aTable,const CDbColSet& aNewDef,TInt& aStep)
	{
	TRAPD(r,iState=aDatabase.iDatabase->AlterTableL(aTable,aNewDef,aStep));
	return r;
	}

/** Initiates an index creation operation on a database. This is the incremental 
form of RDbDatabase::CreateIndex().

@param aDatabase The database on which to create the index.
@param aName A name for the created index.
@param aTable The name of the table on which to create the index.
@param aKey The key for the new index.
@param aStep On return, contains the initial step count for the incremental 
operation. This value should be passed in to subsequent calls to NextL().
@return KErrNone if successful, or one of the DBMS database error codes. Specifically, 
the function returns: KErrNotFound if the table does not exist in the database 
or a key column is not found in the table. KErrAlreadyExists if an index of 
the same name already exists on table or a duplicate key is present when building 
an index. Note that it is not possible to tell the difference between the 
possible causes of this error if index creation is not carried out incrementally. 
KErrBadName if an index or column name is invalid. KErrArgument if the key 
has no key columns or a fixed width column has a truncation length specified, 
or an invalid truncation length has been specified for a key column, or a 
LongText8 or LongText16 key column has no truncation length specified, or 
the key contains a Binary or LongBinary column. KErrNotSupported if a truncated 
key column is not the last one. KErrTooBig if the resulting key size is too 
big.
@see RDbDatabase::CreateIndex() 

@capability Note For a secure shared database, the caller must satisfy the schema 
            access policy for the database.
*/
EXPORT_C TInt RDbIncremental::CreateIndex(RDbDatabase& aDatabase,const TDesC& aName,const TDesC& aTable,const CDbKey& aKey,TInt& aStep)
	{
	TRAPD(r,iState=aDatabase.iDatabase->CreateIndexL(aName,aTable,aKey,aStep));
	return r;
	}

/** Initiates an index discard operation on the database. This is the incremental 
form of RDbDatabase::DropIndex().

@param aDatabase The database from which to drop the index.
@param aName The name of the index to drop.
@param aTable The name of the table which has the index.
@param aStep On return, contains the initial step count for the incremental 
operation. This value should be passed in to subsequent calls to NextL().
@return KErrNone if successful, or one of the DBMS database error codes. Specifically, 
the function returns KErrNotFound if the table or index does not exist
@see RDbDatabase::DropIndex() 

@capability Note For a secure shared database, the caller must satisfy the schema 
            access policy for the database.
*/
EXPORT_C TInt RDbIncremental::DropIndex(RDbDatabase& aDatabase,const TDesC& aName,const TDesC& aTable,TInt& aStep)
	{
	TRAPD(r,iState=aDatabase.iDatabase->DropIndexL(aName,aTable,aStep));
	return r;
	}

//
// Incremental SQL Data definition execution
//
LOCAL_C CDbIncremental* ExecuteDDLL(CDbDatabase& aDatabase,const TDesC& aSql,TDbTextComparison aComparison,TInt& aStep)
	{
	CDbIncremental* inc=aDatabase.ExecuteL(aSql,aComparison,aStep);
	if ((inc==NULL)!=(aStep==0))
		{
		CDbObject::Destroy(inc);
		__LEAVE(KErrArgument);
		}
	return inc;
	}

/** Initiates the execution of a DDL (SQL schema update) statement on the database, 
specifing additional comparison operations for some SQL statements.

This is the incremental form of RDbDatabase::Execute().

Note that to begin executing a DML (SQL data update) statement incrementally, 
use the RDbUpdate class.

@param aDatabase The database on which the DDL (SQL schema update) statement 
is to execute.
@param aSql The DDL SQL statement to be executed on the database.
@param aComparison This argument is used in the execution of some SQL statements, 
and is ignored in all other SQL statements. Specifically: in CREATE INDEX 
statements, it specifies the comparison operation used for text columns in 
the index key. In UPDATE and DELETE statements, it specifies the comparison 
operation used to evaluate the WHERE clause. 
@param aStep On return, contains the initial step count for the incremental 
operation. This value should be passed in to subsequent calls to Next() to 
continue the operation.
@return KErrNone if successful, otherwise another of the system-wide error 
codes.
@see RDbDatabase::Execute()
@see RDbUpdate 

@capability Note For a secure shared database, the caller must satisfy:
            - the schema access policy for the database, if the SQL statement is 
			  CREATE/DROP/ALTER; 
            - the write access policy for the table in the SQL, if the SQL statement is 
			  INSERT/UPDATE/DELETE; 
*/
EXPORT_C TInt RDbIncremental::Execute(RDbDatabase& aDatabase,const TDesC& aSql,TDbTextComparison aComparison,TInt& aStep)
	{
	TRAPD(r,iState=ExecuteDDLL(*aDatabase.iDatabase,aSql,aComparison,aStep));
	return r;
	}

// Class RDbUpdate

//
// Incremental SQL Data definition execution
//
LOCAL_C CDbIncremental* ExecuteDMLL(CDbDatabase& aDatabase,const TDesC& aSql,TDbTextComparison aComparison,TInt& aRows)
	{
	CDbIncremental* inc=aDatabase.ExecuteL(aSql,aComparison,aRows);
	if ((inc==NULL)==(aRows==0))
		{
		CDbObject::Destroy(inc);
		__LEAVE(KErrArgument);
		}
	return inc;
	}

/** Initiates the incremental execution of a DML (SQL data update) statement on 
the database. This is similar to RDbDatabase::Execute().

Note that to begin executing a DDL (SQL schema update) statement incrementally, 
use the RDbIncremental class.

@param aDatabase The database on which the DML (SQL data update) statement 
is to execute.
@param aSql A reference to a descriptor containing the DML statement to be 
executed.
@param aComparison This argument is only used in the execution of some SQL 
statements. By default the comparison is EDbCompareNormal. For more information 
see RDbDatabase::Execute().
@return KErrNone, if the operation is complete or 1, if the operation requires 
further incremental execution or another of the system-wide error codes.
@see RDbIncremental
@see RDbDatabase::Execute() 

@capability Note For a secure shared database, the caller must satisfy:
            - the schema access policy for the database, if the SQL statement is 
			  CREATE/DROP/ALTER; 
            - the write access policy for the table in the SQL, if the SQL statement is 
			  INSERT/UPDATE/DELETE; 
*/
EXPORT_C TInt RDbUpdate::Execute(RDbDatabase& aDatabase,const TDesC& aSql,TDbTextComparison aComparison)
	{
	TRAPD(r,iUpdate=ExecuteDMLL(*aDatabase.iDatabase,aSql,aComparison,iRows()));
	return r;
	}

/** Releases the resources used by this incremental operation object. If the operation 
is not yet complete, then the operation is abandoned and the database is rolled 
back to its state before the operation started. */
EXPORT_C void RDbUpdate::Close()
	{
	iUpdate.Close();
	}

/** Performs the next step in the incremental execution of the DML (SQL data update) 
statement synchronously. The function returns when the step is complete.

Note that if the incremental step fails, then the incremental object should 
be closed and the operation abandoned.

@return KErrNone if execution of the DML statement is complete or 1 if another 
step in the execution of the DML statement is needed. or another of the system-wide 
error codes is returned if the incremental step fails. */
EXPORT_C TInt RDbUpdate::Next()
	{
	TRAPD(r,r=iUpdate->NextL(iRows()));
	return r;
	}

/** Performs the next step in the incremental execution of the DML (SQL data update) 
statement asynchronously.

The function returns immediately and signals when the step is complete.

This function is most effectively used when the incremental operation is packaged 
as an active object.

Note that if the incremental step fails, then the incremental object should 
be closed, and the operation abandoned.

@param aStatus The request status used to contain completion information for 
the operation. On completion, it contains:KErrNone, if execution of the DML 
statement is complete or 1, if another step in the execution of the DML statement 
is needed. or another of the system-wide error codes, if the incremental step 
fails. */
EXPORT_C void RDbUpdate::Next(TRequestStatus& aStatus)
	{
	iUpdate->Next(iRows,aStatus);
	}

/** Returns the number of rows currently affected by the execution of the DML (SQL 
data update) statement on the database.

Once execution of the DML statement is complete, the value returned is the 
final total number of rows affected.

@return The current/final number of rows affected by the execution of the 
DML statement. */
EXPORT_C TInt RDbUpdate::RowCount() const
	{
	return CONST_CAST(TPckgBuf<TInt>&,iRows)();
	}

// Class CDbSyncIncremental

//
// Implement the asynchronous step in terms of the synchronous form
//
EXPORT_C void CDbSyncIncremental::Next(TPckgBuf<TInt>& aStep,TRequestStatus& aStatus)
	{
	TRequestStatus* pStatus=&aStatus;
	TRAPD(r,r=NextL(aStep.operator()()));	// MSVC issue!!! cannot use aStep() directly
	User::RequestComplete(pStatus,r);
	}

// Class CDbAsyncIncremental

//
// Implement the synchronous step in terms of the asynchronous form
//
EXPORT_C TBool CDbAsyncIncremental::NextL(TInt& aStep)
	{
	TRequestStatus status;
	TPckgBuf<TInt> step=aStep;
	Next(step,status);
	User::WaitForRequest(status);
	aStep=step();
	return __LEAVE_IF_ERROR(status.Int());
	}

