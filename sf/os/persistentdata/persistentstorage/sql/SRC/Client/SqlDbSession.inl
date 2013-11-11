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
// Initializes RSqlDbSession data members with their default values.
// 
//

inline RSqlDbSession::RSqlDbSession() :
	iLastErrorMessage(NULL)
	{
	}

/**
Sends a command to the server to detach previously attached database.

@param aDbName Logical database name. 

@return KErrNone, operation has completed successfully;
		KErrNotFound, no attached database with aDbName name.
                      Note that database specific errors categorised as ESqlDbError, and
                      other system-wide error codes may also be returned.

Usage of the IPC call arguments: 
Arg 0: [out]	Logical database name length.
Arg 1: [out]	Logical database name.
*/
inline TInt RSqlDbSession::Detach(const TDesC& aDbName)
	{
	return SendReceive(ESqlSrvDbDetach, TIpcArgs(aDbName.Length(), &aDbName));
	}

/**
Executes one or more 16-bit SQL statements.

The method sends a message to the SQL server containing one or more 16-bit SQL statements to be executed.
SQL statements of any kind can be executed, but the method won't return any record(s) 
if the SQL statement type is "SELECT".
If the SQL statement(s) contains one or more parameters, these parameters will be set to NULL before the
execution.
If the call fails and the error class is ESqlDbError, a text description of the error can be obtained using
LastErrorMessage() method.

@param aSqlStmt String containing one or more 8/16-bit SQL statements, separated with ';'.

@return >=0, The operation has completed successfully. The number of database rows that were 
			 changed/inserted/deleted by the most recently completed INSERT/UPDATE/DELETE sql statement.
			 Exception: If the executed statement is "DELETE FROM <table>", then the function returns 0 
			 if the operation has completed successfully (disregarding the number of the deleted rows);
		KErrNoMemory, an out of memory condition has occured;
		KSqlErrGeneral, Syntax error. A text message describing the problem can be obtained calling
						LastErrorMessage().

Usage of the IPC call arguments: 
Arg 0: [out]	16-bit character length of SQL statement.
Arg 1: [out]	SQL statement(s).
*/
inline TInt RSqlDbSession::Exec(const TDesC& aSqlStmt)
	{
	return SendReceive(ESqlSrvDbExec16, TIpcArgs(aSqlStmt.Length(), &aSqlStmt));
	}

/**
Executes one or more 8-bit SQL statements.

The method sends a message to the SQL server containing one or more 8-bit SQL statements to be executed.
SQL statements of any kind can be executed, but the method won't return any record(s) 
if the SQL statement type is "SELECT".
If the SQL statement(s) contains one or more parameters, these parameters will be set to NULL before the
execution.
If the call fails and the error class is ESqlDbError, a text description of the error can be obtained using
LastErrorMessage() method.

@param aSqlStmt String containing one or more 8-bit SQL statements, separated with ';'.

@return >=0, The operation has completed successfully. The number of database rows that were 
			 changed/inserted/deleted by the most recently completed INSERT/UPDATE/DELETE sql statement.
			 Exception: If the executed statement is "DELETE FROM <table>", then the function returns 0 
			 if the operation has completed successfully (disregarding the number of the deleted rows);
		KErrNoMemory, an out of memory condition has occured;
		KSqlErrGeneral, Syntax error. A text message describing the problem can be obtained calling
						LastErrorMessage().

Usage of the IPC call arguments: 
Arg 0: [out]	8-bit character length of SQL statement.
Arg 1: [out]	SQL statement(s).
*/
inline TInt RSqlDbSession::Exec(const TDesC8& aSqlStmt)
	{
	return SendReceive(ESqlSrvDbExec8, TIpcArgs(aSqlStmt.Length(), &aSqlStmt));
	}

/**
Executes one or more 16-bit SQL statements asynchronously.

The method sends a message to the SQL server containing one or more 16-bit SQL statements to be executed.
SQL statements of any kind can be executed, but the method won't return any record(s) 
if the SQL statement type is "SELECT".
If the SQL statement(s) contains one or more parameters, these parameters will be set to NULL before the
execution.
If the call fails and the error class is ESqlDbError, a text description of the error can be obtained using
LastErrorMessage() method.

@param aSqlStmt String containing one or more 16-bit SQL statements, separated with ';'.
@param aStatus Completion status of asynchronous request, one of the following:
@code
		- >=0, The operation has completed successfully. The number of database rows that were 
			   changed/inserted/deleted by the most recently completed INSERT/UPDATE/DELETE sql statement.
			 Exception: If the executed statement is "DELETE FROM <table>", then the function returns 0 
			 if the operation has completed successfully (disregarding the number of the deleted rows);
        - KSqlErrStmtExpired, the SQL statement has expired (if new functions or
                            collating sequences have been registered or if an
                            authorizer function has been added or changed);
        - KErrNoMemory, an out of memory condition has occurred - the statement
                      will be reset.
                      Note that aStatus may be set with database specific errors categorised as ESqlDbError, 
                      and other system-wide error codes.
@endcode

Usage of the IPC call arguments: 
Arg 0: [out]	16-bit character length of SQL statement.
Arg 1: [out]	SQL statement(s).
*/
inline void RSqlDbSession::Exec(const TDesC& aSqlStmt, TRequestStatus& aStatus)
	{
	SendReceive(ESqlSrvDbExec16, TIpcArgs(aSqlStmt.Length(), &aSqlStmt), aStatus);
	}

/**
Executes one or more 8-bit SQL statements asynchronously.

The method sends a message to the SQL server containing one or more 8-bit SQL statements to be executed.
SQL statements of any kind can be executed, but the method won't return any record(s) 
if the SQL statement type is "SELECT".
If the SQL statement(s) contains one or more parameters, these parameters will be set to NULL before the
execution.
If the call fails and the error class is ESqlDbError, a text description of the error can be obtained using
LastErrorMessage() method.

@param aSqlStmt String containing one or more 8-bit SQL statements, separated with ';'.
@param aStatus Completion status of asynchronous request, one of the following:
@code
		- >=0, The operation has completed successfully. The number of database rows that were 
			   changed/inserted/deleted by the most recently completed INSERT/UPDATE/DELETE sql statement.
			 Exception: If the executed statement is "DELETE FROM <table>", then the function returns 0 
			 if the operation has completed successfully (disregarding the number of the deleted rows);
        - KSqlErrStmtExpired, the SQL statement has expired (if new functions or
                            collating sequences have been registered or if an
                            authorizer function has been added or changed);
        - KErrNoMemory, an out of memory condition has occurred - the statement
                      will be reset.
                      Note that aStatus may be set with database specific errors categorised as ESqlDbError, 
                      and other system-wide error codes.
@endcode

Usage of the IPC call arguments: 
Arg 0: [out]	8-bit character length of SQL statement.
Arg 1: [out]	SQL statement(s).
*/
inline void RSqlDbSession::Exec(const TDesC8& aSqlStmt, TRequestStatus& aStatus)
	{
	SendReceive(ESqlSrvDbExec8, TIpcArgs(aSqlStmt.Length(), &aSqlStmt), aStatus);
	}

/**
Sets the transaction isolation level.

The method sends a message to the SQL server to set the desired isolation level.
The default isolation level is RSqlDatabase::ESerializable, but can be changed to 
RSqlDatabase::EReadUncommitted for database connections which share the access to the same database file.

@param aIsolationLevel Desired isolation level: RSqlDatabase::ESerializable or RSqlDatabase::EReadUncommitted.
@return KErrNone, the operation has completed successfully.
                  The function may also return some other system-wide error codes.

Usage of the IPC call arguments: 
Arg 0: [out]	Isolation level type.
*/
inline TInt RSqlDbSession::SetIsolationLevel(RSqlDatabase::TIsolationLevel aIsolationLevel)
	{
	return SendReceive(ESqlSrvDbSetIsolationLevel, TIpcArgs(static_cast <TInt> (aIsolationLevel)));
	}

/**
Usage of the IPC call arguments: 
Arg 0: [out]	(16-bit character length of SQL statement) | (expected column value type << 24).
Arg 1: [out]	SQL statement.
Arg 2: [out]	Byte max length of the receiving buffer
Arg 3: [in/out]	The receiving buffer
*/
inline TInt RSqlDbSession::ExecScalarFullSelect(const TDesC& aSqlStmt, TSqlColumnType aType, TDes8& aRes)
	{
	return SendReceive(ESqlSrvDbScalarFullSelect16, TIpcArgs(aSqlStmt.Length() | (aType << 24), &aSqlStmt, aRes.MaxLength(), &aRes));
	}
	
/**
Usage of the IPC call arguments: 
Arg 0: [out]	(8-bit character length of SQL statement) | (expected column value type << 24).
Arg 1: [out]	SQL statement.
Arg 2: [out]	Byte max length of the receiving buffer
Arg 3: [in/out]	The receiving buffer
*/
inline TInt RSqlDbSession::ExecScalarFullSelect(const TDesC8& aSqlStmt, TSqlColumnType aType, TDes8& aRes)
	{
	return SendReceive(ESqlSrvDbScalarFullSelect8, TIpcArgs(aSqlStmt.Length() | (aType << 24), &aSqlStmt, aRes.MaxLength(), &aRes));
	}
	
/**
The method sends a message to the SQL server.

@param aFunction Function code

@return KErrNone or system-wide error codes.
*/
inline TInt RSqlDbSession::SendReceive(TInt aFunction)
	{
	return RSessionBase::SendReceive(aFunction);	
	}

/**
The method sends a message asynchronously to the SQL server.

@param aFunction Function code

@return KErrNone or system-wide error codes.
*/
inline void RSqlDbSession::SendReceive(TInt aFunction, TRequestStatus& aStatus)
	{
	RSessionBase::SendReceive(aFunction, aStatus);
	}

/**
The method sends a message with arguments to the SQL server.

@param aFunction Function code
@param aArgs Message arguments

@return KErrNone or system-wide error codes.
*/
inline TInt RSqlDbSession::SendReceive(TInt aFunction, const TIpcArgs& aArgs)
	{
	return RSessionBase::SendReceive(aFunction, aArgs);	
	}
	
/**
The method sends asynchronously a message with arguments to the SQL server.

@param aFunction Function code
@param aArgs Message arguments
@param aStatus Completion status of asynchronous request
*/
inline void RSqlDbSession::SendReceive(TInt aFunction,const TIpcArgs& aArgs, TRequestStatus& aStatus)
	{
	RSessionBase::SendReceive(aFunction, aArgs, aStatus);	
	}
