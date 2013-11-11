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
// SqlDbSession.inl
// 
//

/**
@panic SqlDb 2 In _DEBUG mode if the statement handle is 0 or the database session is NULL,
			   i.e. Prepare() has not yet been called on this RSqlStatementSession object.
			   
@return A reference to the RSqlDbSession instance.
*/
inline RSqlDbSession& RSqlStatementSession::DbSession() const
	{
	__ASSERT_DEBUG(iHandle > 0 && iDbSession != NULL, __SQLPANIC(ESqlPanicInvalidObj));
	return *iDbSession;
	}

/**
Creates unitialized RSqlStatementSession object.
*/
inline RSqlStatementSession::RSqlStatementSession() :
	iHandle(-1),
	iDbSession(NULL)
	{
	}
	
/**
Sends a request to the SQL server to reset the prepared SQL statement.

@return KErrNone The method completed successfully, system-wide error code otherwise.
*/
inline TInt RSqlStatementSession::Reset()
	{
	return DbSession().SendReceive(::MakeMsgCode(ESqlSrvStmtReset, ESqlSrvStatementHandle, iHandle));
	}

/**
Sends a request to the SQL server to execute the prepared SQL statement.


@return >=0, The operation has completed successfully. The number of database rows that were 
			 changed/inserted/deleted by the most recently completed INSERT/UPDATE/DELETE sql statement.
			 Exception: If the executed statement is "DELETE FROM <table>", then the function returns 0 
			 if the operation has completed successfully (disregarding the number of the deleted rows);
	    KSqlErrStmtExpired, statement has expired (if new functions or collating sequences are 
							registered or if an authorizer function is added or changed);
		KErrNoMemory, an out of memory condition has occurred.
                      Note that database specific errors categorised as ESqlDbError, and
                      other system-wide error codes may also be returned.
*/
inline TInt RSqlStatementSession::Exec()
	{
	return DbSession().SendReceive(::MakeMsgCode(ESqlSrvStmtExec, ESqlSrvStatementHandle, iHandle));
	}

/**
Sends a request asynchronously to the SQL server to execute the prepared SQL statement.

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
*/
inline void RSqlStatementSession::Exec(TRequestStatus& aStatus)
	{
	DbSession().SendReceive(::MakeMsgCode(ESqlSrvStmtAsyncExec, ESqlSrvStatementHandle, iHandle), aStatus);
	}

/**
Sends a request to the SQL server to execute the prepared SQL statement.

Usage of the IPC call arguments:
Arg 0: [out]		parameter buffer length in bytes
Arg 1: [out]		parameter buffer

@param aParamBuf A buffer with the parameter values

@return >=0, The operation has completed successfully. The number of database rows that were 
			 changed/inserted/deleted by the most recently completed INSERT/UPDATE/DELETE sql statement.
			 Exception: If the executed statement is "DELETE FROM <table>", then the function returns 0 
			 if the operation has completed successfully (disregarding the number of the deleted rows);
	    KSqlErrStmtExpired, statement has expired (if new functions or collating sequences are 
							registered or if an authorizer function is added or changed);
		KErrNoMemory, an out of memory condition has occurred.
                      Note that database specific errors categorised as ESqlDbError, and
                      other system-wide error codes may also be returned.
*/
inline TInt RSqlStatementSession::BindExec(const RSqlBufFlat& aParamBuf)
	{
	TPtrC8 prmData(aParamBuf.BufDes());
	return DbSession().SendReceive(::MakeMsgCode(ESqlSrvStmtBindExec, ESqlSrvStatementHandle, iHandle), TIpcArgs(prmData.Length(), &prmData));
	}

/**
Sends a request asynchronously to the SQL server to execute the prepared SQL statement.

Usage of the IPC call arguments:
Arg 0: [out]		parameter buffer length in bytes
Arg 1: [out]		parameter buffer

@param aParamBuf A buffer with the parameter values
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
*/
inline void RSqlStatementSession::BindExec(const RSqlBufFlat& aParamBuf, TRequestStatus& aStatus)
	{
	const TDesC8& bufDes = aParamBuf.BufDes();
	DbSession().SendReceive(::MakeMsgCode(ESqlSrvStmtAsyncBindExec, ESqlSrvStatementHandle, iHandle), TIpcArgs(bufDes.Length(), &bufDes), aStatus);
	}

/**
Sends a request to the SQL server to move to the next record which satisfies the 
condition of the prepared SQL statement.
If there is a valid next record, the method transfers the column values from the server.

@param aColumnBuf It references RSqlBufFlat object where the column values will be stored.

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
*/
inline TInt RSqlStatementSession::Next(RSqlBufFlat& aColumnBuf)
	{
	TIpcArgs ipcArgs;
	return DoBindNext(ESqlSrvStmtNext, ipcArgs, aColumnBuf);
	}

/**
Sends a command to the server for retrieving a column data.

Usage of the IPC call arguments:
Arg 0: [out]		column index
Arg 1: [out]		column data buffer length in bytes
Arg 2: [in/out]		column data buffer
*/	
inline TInt RSqlStatementSession::ReadColumnValue(TInt aColumnIndex, TDes8& aBuf)
	{
	return DbSession().SendReceive(::MakeMsgCode(ESqlSrvStmtColumnValue, ESqlSrvStatementHandle, iHandle), TIpcArgs(aColumnIndex, aBuf.MaxLength(), &aBuf));
	}

/**
The method creates a read-only MStreamBuf derived object which allows the column with aColumnIndex index 
to be accessed as a stream of bytes (if the column is a binary column) or characters 
(if the column is a text column) and returns it to the caller.

Usage of the IPC call arguments:
Arg 0: [out]	column index (0 based)
Arg 2: [in]		ipc buffer, column source

The caller is responsible for the destroying of the read-only MStreamBuf derived object.

@param aColumnIndex Column index (starting from 0)
@return A pointer to the created read-only memory MStreamBuf derived object.

@leave KErrNoMemory, an out of memory condition has occured;
*/
inline MStreamBuf* RSqlStatementSession::ColumnSourceL(TInt aColumnIndex)
	{
	TIpcArgs args(aColumnIndex);
	HIpcBuf* columnSource = HIpcBuf::NewL(DbSession(), ::MakeMsgCode(ESqlSrvStmtColumnSource, ESqlSrvStatementHandle, iHandle), args);
	return columnSource;
	}

/**
The method creates an IPC object with buffering capabilities, allowing to stream out the data of the 
parameter with aParameterIndex index and returns this MStreamBuf derived object to the caller.

The caller is responsible for the destroying of the MStreamBuf derived object.

Arg 0: [out]	parameter index (0 based)
Arg 2: [in]		ipc buffer, parameter source

@param aParameterIndex Parameter index (starting from 0)

@return A pointer to the created MStreamBuf derived object.

@leave KErrNoMemory, an out of memory condition has occured;
*/
inline MStreamBuf* RSqlStatementSession::ParamSinkL(TSqlSrvFunction aFunction, TInt aParamIndex)
	{
	TIpcArgs args(aParamIndex);
	HIpcBuf* paramSink = HIpcBuf::NewL(DbSession(), ::MakeMsgCode(aFunction, ESqlSrvStatementHandle, iHandle), args);
	return paramSink;
	}
