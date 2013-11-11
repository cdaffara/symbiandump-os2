// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
*/
inline CSqlDatabaseImpl::CSqlDatabaseImpl()
	{
	}
	
/**
Attaches a secure or non-secure database.

Implements RSqlDatabase::Attach().

@param aDbFileName The name of the file that hosts the database. If this is
                   a secure database, then the format of the name must be:
                   \<drive\>:\<[SID]database file name excluding the path\>.
                   If this is a private or shared non-secure database, then aDbFileName has to be the full 
                   database file name. "[SID]" refers to SID of the application which created the attached database.
@param aDbName Logical database name. It must be unique (per database connection). This is the name which can
               be used for accessing tables in the attached database. The syntax is "database-name.table-name".

@return KErrNone, the operation has completed successfully;
        KErrNoMemory, an out of memory condition has occurred;
        KErrBadName, the file name is invalid - it has either a zero length or it is the name of a directory;
        KErrNotReady, the drive does not exist or is not ready;
        KErrInUse, the file is already open;
        KErrNotFound, database file not found;
        KErrGeneral, missing or invalid security policies (if the database to be opened is a secure database);
        KErrPermissionDenied, the caller does not satisfy the relevant database security policies;
        KErrNotSupported, incompatible SQL security version (if the database to be opened is a secure database).
                      Note that database specific errors categorised as ESqlDbError, and
                      other system-wide error codes may also be returned.
                      
@see RSqlDatabase
@see RSqlDatabase::Attach()
*/
inline TInt CSqlDatabaseImpl::Attach(const TDesC& aDbFileName, const TDesC& aDbName)
	{
	return Session().Attach(aDbFileName, aDbName);
	}
	
/**
Detaches previously attached database.

Implements RSqlDatabase::Detach().

@param aDbName Logical database name. The logical name of the database to be detached.

@return KErrNone, the operation has completed successfully;
        KErrNotFound, no attached database with aDbName name;
                      Note that database specific errors categorised as ESqlDbError, and
                      other system-wide error codes may also be returned.

@see RSqlDatabase
@see RSqlDatabase::Detach()
*/
inline TInt CSqlDatabaseImpl::Detach(const TDesC& aDbName)
	{
	return Session().Detach(aDbName);
	}

/**
Copies a database file to the specified location.

Implements RSqlDatabase::Copy().

@param aSrcDbFileName Source database file name.
					  If this is a secure database, then the format of the name must be:
					  \<drive\>:\<[SID]database file name excluding the path\>.
					  If this is a non-secure database, then aDbFileName has to be the full database file name.
					  "[SID]" refers to SID of the application which created the database.
@param aDestDbFileName Destination database file name.
					  If this is a secure database, then the format of the name must be:
					  \<drive\>:\<[SID]database file name excluding the path\>.
					  If this is a non-secure database, then aDbFileName has to be the full database file name.
					  "[SID]" refers to SID of the application which performs the copying operation.

The allowed copying operations are:
- secure to secure database. Only the application created the database is allowed to copy it.
- non-secure to non-secure database. No restrictions apply to this operation.

@return KErrNone, the operation completed has successfully;
        KErrBadName, the file name is invalid - it has either a zero length or it is the name of a directory;
        KErrNotReady, the drive does not exist or is not ready;
        KErrInUse, the file is already open;
        KErrNotFound, database file not found;
        KErrPermissionDenied, the SID of the calling application does not match the SID of source or destination database.
                      Note that other system-wide error codes may also be returned.

@see RSqlDatabase
@see RSqlDatabase::Copy()
*/
inline TInt CSqlDatabaseImpl::Copy(const TDesC& aSrcDbFileName, const TDesC& aDestDbFileName)
	{
	return RSqlDbSession::CopyDatabase(aSrcDbFileName, aDestDbFileName);
	}
	
/**
Deletes the specified database file.

Implements RSqlDatabase::Delete().

@param aDbFileName The name of the database file.
				   If this is a secure database, then the format of the name must be:
				   \<drive\>:\<[SID]database file name excluding the path\>.
				   If this is a private or shared non-secure database, then aDbFileName has to be the 
				   full database file name. "[SID]" refers to SID of the application which created the database.

@return KErrNone, the operation has completed successfully;
        KErrBadName, the file name is invalid - it has either a zero length or it is the name of a directory;
        KErrNotReady, the drive does not exist or is not ready;
        KErrInUse, the database file is in use;
        KErrNotFound, the database file cannot be found;
        KErrAccessDenied, access to the database file is denied (e.g. it might be a read-only file);
        KErrPermissionDenied, the SID of the calling application does not match the SID of the database;
                      Note that other system-wide error codes may also be returned.

@see RSqlDatabase
@see RSqlDatabase::Delete()
*/
inline TInt CSqlDatabaseImpl::Delete(const TDesC& aDbFileName)
	{
	return RSqlDbSession::DeleteDatabase(aDbFileName);
	}
	
/**
Sets the transaction isolation level for the database.

Implements RSqlDatabase::SetIsolationLevel().

@param aIsolationLevel The isolation level to be set.
					   Allowed isolation level values are:
					   - RSqlDatabase::EReadUncommitted;
					   - RSqlDatabase::ESerializable;
                       
@return KErrNone, if the operation has completed successfully;
		KErrNotSupported, invalid (not supported) transaction isolation level;

@see RSqlDatabase
@see RSqlDatabase::SetIsolationLevel()
*/
inline TInt CSqlDatabaseImpl::SetIsolationLevel(RSqlDatabase::TIsolationLevel aIsolationLevel)
	{
	return Session().SetIsolationLevel(aIsolationLevel);
	}
	
/**
Executes one or more 16-bit DDL/DML SQL statements.

Implements RSqlDatabase::Exec().

@param aSqlStmt A string of 16-bit wide characters containing one or more DDL/DML SQL statements;
                each statement is separated by a ';' character.

@return >=0, The operation has completed successfully. The number of database rows that were 
			 changed/inserted/deleted by the most recently completed DDL/DML sql statement.
			 Exception: If the executed statement is "DELETE FROM <table>", then the function returns 0 
			 if the operation has completed successfully (disregarding the number of the deleted rows);
        KErrNoMemory, an out of memory condition has occured;
        KSqlErrGeneral, a syntax error has occurred - text describing the problem
                        can be obtained by calling 	RSqlDatabase::LastErrorMessage();
        KErrPermissionDenied, the caller does not satisfy the relevant database security policies.
                      Note that database specific errors categorised as ESqlDbError, and
                      other system-wide error codes may also be returned.

@see RSqlDatabase
@see RSqlDatabase::Exec()
*/
inline TInt CSqlDatabaseImpl::Exec(const TDesC16& aSqlStmt)
	{
	return Session().Exec(aSqlStmt);
	}
	
/**
Executes one or more 8-bit DDL/DML SQL statements.

Implements RSqlDatabase::Exec().

@param aSqlStmt A string of 8-bit wide characters containing one or more DDL/DML SQL statements;
                each statement is separated by a ';' character.

@return >=0, The operation has completed successfully. The number of database rows that were 
			 changed/inserted/deleted by the most recently completed DDL/DML sql statement.
			 Exception: If the executed statement is "DELETE FROM <table>", then the function returns 0 
			 if the operation has completed successfully (disregarding the number of the deleted rows);
        KErrNoMemory, an out of memory condition has occured;
        KSqlErrGeneral, a syntax error has occurred - text describing the problem
                        can be obtained by calling 	RSqlDatabase::LastErrorMessage();
        KErrPermissionDenied, the caller does not satisfy the relevant database security policies.
                      Note that database specific errors categorised as ESqlDbError, and
                      other system-wide error codes may also be returned.

@see RSqlDatabase
@see RSqlDatabase::Exec()
*/
inline TInt CSqlDatabaseImpl::Exec(const TDesC8& aSqlStmt)
	{
	return Session().Exec(aSqlStmt);
	}

/**
Executes one or more 16-bit DDL/DML SQL statements asynchronously.

Implements RSqlDatabase::Exec().

@param aSqlStmt A string of 16-bit wide characters containing one or more DDL/DML SQL statements;
                each statement is separated by a ';' character.
@param aStatus Completion status of asynchronous request, one of the following:
@code
			- >=0, The operation has completed successfully. The number of database rows that were 
			   	   changed/inserted/deleted by the most recently completed DDL/DML sql statement.
			 	Exception: If the executed statement is "DELETE FROM <table>", then the function returns 0 
			 	if the operation has completed successfully (disregarding the number of the deleted rows);
        	- KErrNoMemory,  an out of memory condition has occured;
        	- KSqlErrGeneral, a syntax error has occurred - text describing the problem
                        can be obtained by calling 	RSqlDatabase::LastErrorMessage();
        	- KErrPermissionDenied, the caller does not satisfy the relevant database security policies;
                      Note that aStatus may be set with database specific errors categorised as ESqlDbError, 
                      and other system-wide error codes.
@endcode

@see RSqlDatabase
@see RSqlDatabase::Exec()
*/
inline void CSqlDatabaseImpl::Exec(const TDesC16& aSqlStmt, TRequestStatus& aStatus)
	{
	Session().Exec(aSqlStmt, aStatus);
	}
	
/**
Executes one or more 8-bit DDL/DML SQL statements asynchronously.

Implements RSqlDatabase::Exec().

@param aSqlStmt A string of 8-bit wide characters containing one or more DDL/DML SQL statements;
                each statement is separated by a ';' character.
@param aStatus Completion status of asynchronous request, one of the following:
@code
			- >=0, The operation has completed successfully. The number of database rows that were 
			   	   changed/inserted/deleted by the most recently completed DDL/DML sql statement.
			 	Exception: If the executed statement is "DELETE FROM <table>", then the function returns 0 
			 	if the operation has completed successfully (disregarding the number of the deleted rows);
        	- KErrNoMemory,  an out of memory condition has occured;
        	- KSqlErrGeneral, a syntax error has occurred - text describing the problem
                        can be obtained by calling 	RSqlDatabase::LastErrorMessage();
        	- KErrPermissionDenied, the caller does not satisfy the relevant database security policies;
                      Note that aStatus may be set with database specific errors categorised as ESqlDbError, 
                      and other system-wide error codes.
@endcode

@see RSqlDatabase
@see RSqlDatabase::Exec()
*/
inline void CSqlDatabaseImpl::Exec(const TDesC8& aSqlStmt, TRequestStatus& aStatus)
	{
	Session().Exec(aSqlStmt, aStatus);
	}
	
/**
Retrieves a reference to the textual description of the error returned by the
most recent call.

Implements RSqlDatabase::LastErrorMessage().

@return A non-modifiable pointer descriptor representing the most recent error
        message. Note that message may be NULL, i.e. the descriptor may have
        zero length.

@see RSqlDatabase
@see RSqlDatabase::LastErrorMessage()
*/
inline TPtrC CSqlDatabaseImpl::LastErrorMessage()
	{
	return Session().LastErrorMessage();
	}

/**
Returns the ROWID of the most recent successful INSERT into the database 
from this database connection.

Implements RSqlDatabase::LastInsertedRowId().

@return >0, the ROWID of the most recent successful INSERT into the database
			from this database connection;
		0, 	if no successful INSERTs have ever occurred from this database connection
		<0, if one of the system-wide error codes is returned

@see RSqlDatabase::LastInsertedRowId()
*/
inline TInt64 CSqlDatabaseImpl::LastInsertedRowId()
	{
	return Session().LastInsertedRowId();
	}

/**
@return A reference to RSqlDbSession instance.
*/
inline RSqlDbSession& CSqlDatabaseImpl::Session()
	{
	return iDbSession;
	}

/**
Sends a command to the server to Execute a SELECT query which is expected to return a single row consisting of
a single column value and copies that value to the place refered by aRes parameter.

@param aSqlStmt 16-bit SELECT sql query
@param aType    The expected column type
@param aRes     Output parameter. Refers to the place where the result must be copied

@return KErrNone, if the function completes successfully,
		Positive value, The column value length, in case if the receiving buffer 
						is not big enough. This return result is possible only with text or binary columns.
		          
@leave  The function may leave with database specific errors categorised as ESqlDbError and
  		other system-wide error codes.
*/
inline TInt CSqlDatabaseImpl::ExecScalarFullSelectL(const TDesC16& aSqlStmt, TSqlColumnType aType, TDes8& aRes)
	{
	TInt rc = Session().ExecScalarFullSelect(aSqlStmt, aType, aRes);
	__SQLLEAVE_IF_ERROR(rc);
	return rc;
	}

/**
Sends a command to the server to Execute a SELECT query which is expected to return a single row consisting of
a single column value and copies that value to the place refered by aRes parameter.

@param aSqlStmt 8-bit SELECT sql query
@param aType    The expected column type
@param aRes     Output parameter. Refers to the place where the result must be copied

@return KErrNone, if the function completes successfully,
		Positive value, The column value length, in case if the receiving buffer 
						is not big enough. This return result is possible only with text or binary columns.
		          
@leave  The function may leave with database specific errors categorised as ESqlDbError and
  		other system-wide error codes.
*/
inline TInt CSqlDatabaseImpl::ExecScalarFullSelectL(const TDesC8& aSqlStmt, TSqlColumnType aType, TDes8& aRes)
	{
	TInt rc = Session().ExecScalarFullSelect(aSqlStmt, aType, aRes);
	__SQLLEAVE_IF_ERROR(rc);
	return rc;
	}

/**
@return True, if the database is in transaction, false otherwise.
*/
inline TBool CSqlDatabaseImpl::InTransaction()
	{
	return 	Session().SendReceive(ESqlSrvDbInTransaction) != 0;
	}

/**
@return >= 0, 		  the operation has completed successfully. The number is the size of the main
					  database file,
        KErrNoMemory, an out of memory condition has occurred;
        KErrTooBig,   the database is too big and the size cannot fit into 32-bit signed integer;
                      Note that database specific errors categorised as ESqlDbError, and
                      other system-wide error codes may also be returned.
*/
inline TInt CSqlDatabaseImpl::Size()
	{
	return Session().SendReceive(ESqlSrvDbSize);
	}

/**
Returns the database file size and free space, in bytes.

@param aSize An output parameter. If the call was successfull the aSize object will contain information
			 about the database size and database free space.
@param aDbName The attached database name or KNullDesC for the main database

@return KErrNone, The operation has completed succesfully;
                  Note that database specific errors categorised as ESqlDbError, and
                  other system-wide error codes may also be returned.

Usage of the IPC call arguments:
Arg 0: [in/out]	Points to a RSqldatabase::TSize object, where the database size and free space values
			    will be copied.
Arg 1: [out]	The database name length in characters
Arg 2: [out]	The attached database name or KNullDesC for the main database
*/
inline TInt CSqlDatabaseImpl::Size(RSqlDatabase::TSize& aSize, const TDesC& aDbName)
	{
	TPtr8 ptr(reinterpret_cast <TUint8*> (&aSize), sizeof(aSize));
	return Session().SendReceive(ESqlSrvDbSize2, TIpcArgs(&ptr, aDbName.Length(), &aDbName));
	}
