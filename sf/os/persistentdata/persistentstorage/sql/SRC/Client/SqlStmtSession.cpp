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
// NTT DOCOMO, INC - Fix for Bug 1915 "SQL server panics when using long column type strings"
//
// Description:
//

#include <s32mem.h>
#include "SqlStmtSession.h"		//RSqlStatementSession
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "SqlStmtSessionTraces.h"
#endif
#include "SqlTraceDef.h"

/**
 Sends a request to the SQL server to prepare 16-bit aSqlStmt statement.
 
 Usage of the IPC call arguments:
 Arg 0: [in/out]        data buffer for the column and parameter count.
 Arg 1: [out]       statement length in characters
 Arg 2: [out]       16-bit statement
 
 @param aDbSession A reference to RSqlDbSession instance.
 @param aSqlStmt 16-bit SQL statement.
 @param aColumnCount Output parameter. Statement column count.
 @param aParamCount Output parameter. Statement parameter count.
 
 @return KErrNoMemory, an out of memory condition has occured;
 KErrArgument, bad argument, for example - the SQL string contains more than one SQL statements.
 Note that the function may return some database specific errors categorised as 
 ESqlDbError or other system-wide error codes;
 KErrNone      The operation has completed successfully. 
 
 @panic SqlDb 7 In _DEBUG mode if the statement handle is 0.
*/
TInt RSqlStatementSession::Prepare(RSqlDbSession& aDbSession, const TDesC& aSqlStmt, 
                                          TInt& aColumnCount, TInt& aParamCount)
    {
    iDbSession = &aDbSession;
    TSqlIpcData data;
    TPckg<TSqlIpcData> pckg(data);
    TUint stmtLen = aSqlStmt.Length();
    iHandle = iDbSession->SendReceive(ESqlSrvStmtPrepare16, TIpcArgs(&pckg, stmtLen, &aSqlStmt));
    __ASSERT_DEBUG(iHandle != 0, __SQLPANIC(ESqlPanicInternalError));   
    aColumnCount = static_cast <TInt> (data.iPrm1);
    aParamCount = static_cast <TInt> (data.iPrm2);
    SQL_TRACE_SESSION(OstTraceExt2(TRACE_INTERNALS, RSQLSTATEMENTSESSION_PREPARE16, "0x%X;RSqlStatementSession::Prepare-16;iHandle=%d", (TUint)this, iHandle));
    return iHandle > 0 ? KErrNone : iHandle;
    }

/**
Sends a request to the SQL server to prepare 8-bit aSqlStmt statement.

Usage of the IPC call arguments:
Arg 0: [in/out]     data buffer for the column and parameter count.
Arg 1: [out]        statement length in characters
Arg 2: [out]        8-bit statement

@param aDbSession A reference to RSqlDbSession instance.
@param aSqlStmt 8-bit SQL statement.
@param aColumnCount Output parameter. Statement column count.
@param aParamCount Output parameter. Statement parameter count.

@return KErrNoMemory, an out of memory condition has occured;
        KErrArgument, bad argument, for example - the SQL string contains more than one SQL statements.
                      Note that the function may return some database specific errors categorised as 
                      ESqlDbError or other system-wide error codes;
        KErrNone      The operation has completed successfully. 

@panic SqlDb 7 In _DEBUG mode if the statement handle is 0.
*/
TInt RSqlStatementSession::Prepare(RSqlDbSession& aDbSession, const TDesC8& aSqlStmt, 
                                          TInt& aColumnCount, TInt& aParamCount)
    {
    iDbSession = &aDbSession;
    TSqlIpcData data;
    TPckg<TSqlIpcData> pckg(data);
    TUint stmtLen = aSqlStmt.Length();
    iHandle = iDbSession->SendReceive(ESqlSrvStmtPrepare8, TIpcArgs(&pckg, stmtLen, &aSqlStmt));
    __ASSERT_DEBUG(iHandle != 0, __SQLPANIC(ESqlPanicInternalError));
    aColumnCount = static_cast <TInt> (data.iPrm1);
    aParamCount = static_cast <TInt> (data.iPrm2);
    SQL_TRACE_SESSION(OstTraceExt2(TRACE_INTERNALS, RSQLSTATEMENTSESSION_PREPARE8, "0x%X;RSqlStatementSession::Prepare-8;iHandle=%d", (TUint)this, iHandle));
    return iHandle > 0 ? KErrNone : iHandle;
    }

/**
Sends a request to the server to close the statement handle.
Closes the session object.
*/
void RSqlStatementSession::Close()
	{
	SQL_TRACE_SESSION(OstTraceExt2(TRACE_INTERNALS, RSQLSTATEMENTSESSION_CLOSE, "0x%X;RSqlStatementSession::Close;iHandle=%d", (TUint)this, iHandle));
	if(iDbSession && iHandle > 0)
		{
		(void)iDbSession->SendReceive(::MakeMsgCode(ESqlSrvStmtClose, ESqlSrvStatementHandle, iHandle));
		}
	iDbSession = NULL;
	iHandle = -1;
	}

/**
Binds the statement parameters and sends a request to the SQL server to move to the next record which satisfies the 
condition of the prepared SQL statement.
If there is a valid next record, the method transfers the column values from the server.

@param aParamBuf  It references RSqlBufFlat object where the parameter values are stored.
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
TInt RSqlStatementSession::BindNext(const RSqlBufFlat& aParamBuf, RSqlBufFlat& aColumnBuf)
	{
	TPtrC8 prmData(aParamBuf.BufDes());
	TIpcArgs ipcArgs;
	ipcArgs.Set(0, prmData.Length());
	ipcArgs.Set(1, &prmData);
	return DoBindNext(ESqlSrvStmtBindNext, ipcArgs, aColumnBuf);
	}

/**
Implements RSqlStatementSession::Next() and RSqlStatementSession::BindNext().
Sends a "Next" command to the server combined with optional "Bind" command.
In a single IPC call the statement parameters will be bound and the current row columns - returned.
If the client side flat buffer is not big enough, a second IPC call will be made after reallocating the buffer.

Usage of the IPC call arguments:
Arg 0: [out]		parameter buffer length in bytes
Arg 1: [out]		parameter buffer
Arg 2: [out]		column buffer length in bytes
Arg 3: [in/out]		column buffer

@see RSqlStatementSession::Next()
@see RSqlStatementSession::BindNext()
*/
TInt RSqlStatementSession::DoBindNext(TSqlSrvFunction aFunction, TIpcArgs& aIpcArgs, RSqlBufFlat& aColumnBuf)
	{
	aColumnBuf.Reset();
	aIpcArgs.Set(2, aColumnBuf.MaxSize());
	aIpcArgs.Set(3, &aColumnBuf.BufPtr());
	TInt err = DbSession().SendReceive(::MakeMsgCode(aFunction, ESqlSrvStatementHandle, iHandle), aIpcArgs);
	if(err > KSqlClientBufOverflowCode)
		{
		err = Retry(aColumnBuf, err - KSqlClientBufOverflowCode, ESqlColumnValuesBuf);
		if(err == KErrNone)
			{
			err = KSqlAtRow;	
			}
		}
	return err;
	}

/**
Sends a command to the server for retrieving parameter names or column names.

Usage of the IPC call arguments:
Arg 0: [out]		buffer length in bytes
Arg 1: [in/out]		buffer
*/	
TInt RSqlStatementSession::GetNames(TSqlSrvFunction aFunction, RSqlBufFlat& aNameBuf)
	{
	aNameBuf.Reset();
	TPtr8& ptr = aNameBuf.BufPtr();
	TInt err = DbSession().SendReceive(::MakeMsgCode(aFunction, ESqlSrvStatementHandle, iHandle), TIpcArgs(ptr.MaxLength(), &ptr));
	if(err > KSqlClientBufOverflowCode)
		{
		err = Retry(aNameBuf, err - KSqlClientBufOverflowCode, aFunction == ESqlSrvStmtColumnNames ? ESqlColumnNamesBuf : ESqlParamNamesBuf);
		}
	return err;
	}

/**
Sends a command to the server for retrieving specified data (aWhat parameter).

Usage of the IPC call arguments:
Arg 0: [out]		The type of the data to be retrieved
Arg 1: [in/out]		Data buffer
*/	
TInt RSqlStatementSession::Retry(RSqlBufFlat& aBufFlat, TInt aSize, TSqlBufFlatType aWhat)
	{
	aBufFlat.Reset();
	TInt err = aBufFlat.ReAlloc(aSize);
	if(err == KErrNone)
		{
		TPtr8& ptr = aBufFlat.BufPtr();
		err = DbSession().SendReceive(::MakeMsgCode(ESqlSrvStmtBufFlat, ESqlSrvStatementHandle, iHandle), TIpcArgs(aWhat, &ptr));
		}
	return err;	
	}

/**
Sends a command to the server for retrieving the declared types of columns

Usage of the IPC call arguments:
Arg 0: [out]		buffer length in bytes
Arg 1: [in/out]		buffer
*/	 
TInt RSqlStatementSession::GetDeclColumnTypes(RSqlBufFlat& aDeclColumnTypeBuf)
	{
	aDeclColumnTypeBuf.Reset();
	TPtr8& ptr = aDeclColumnTypeBuf.BufPtr();
	TInt err = DbSession().SendReceive(::MakeMsgCode(ESqlSrvStmtDeclColumnTypes, ESqlSrvStatementHandle, iHandle), TIpcArgs(ptr.MaxLength(), &ptr));	
	if(err > KSqlClientBufOverflowCode)
		{
		err = Retry(aDeclColumnTypeBuf, err - KSqlClientBufOverflowCode, ESqlDeclColumnTypesBuf);
		}
	return err;	
	}
