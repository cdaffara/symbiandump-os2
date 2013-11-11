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
//

#include <e32debug.h>
#include <hal.h>
#include <sqldb.h>
#include "sqlite3.h"
#include "SqlSrvStatementUtil.h"
#include "SqlAssert.h"
#include "SqlSrvUtil.h"
#include "SqlUtil.h"
#include "SqliteSymbian.h"		//sqlite3SymbianLastOsError()
#include "OstTraceDefinitions.h"
#include "SqlSrvResourceProfiler.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "SqlSrvStatementUtilTraces.h"
#endif
#include "SqlTraceDef.h"

//The database names in all statements are quoted to avoid the "sql injection" threat.
_LIT8(KPageCountPragma, "PRAGMA \"%S\".page_count\x0");
_LIT8(KPageSizePragma,  "PRAGMA \"%S\".page_size\x0");
_LIT8(KCacheSizePragma,  "PRAGMA \"%S\".cache_size\x0");
_LIT8(KEncodingPragma,  "PRAGMA \"%S\".encoding\x0");
_LIT8(KFreePageCountPragma, "PRAGMA \"%S\".freelist_count\x0");
_LIT8(KVacuumModePragma, "PRAGMA \"%S\".auto_vacuum\x0");
_LIT8(KIncrementalVacuumPragma, "PRAGMA \"%S\".incremental_vacuum(%d)\x0");

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Calls sqlite3_open16() to create or open database file with aFileNameZ.
//aFileNameZ is UTF16 encoded, zero-terminated.
//The function returns system-wide errors or database specific errors.
//The function may panic with code 7 in _DEBUG mode - internal error: the created database handle is NULL.
TInt CreateDbHandle16(const TDesC& aFileNameZ, sqlite3*& aDbHandle)
	{
	(void)sqlite3SymbianLastOsError();//clear last OS error
	TInt err = sqlite3_open16(aFileNameZ.Ptr(), &aDbHandle);
	__ASSERT_DEBUG(err == SQLITE_OK ? aDbHandle != NULL : ETrue, __SQLPANIC2(ESqlPanicInternalError));
	if(err == SQLITE_OK)
		{
		(void)sqlite3_extended_result_codes(aDbHandle, 0);
		}
	//Get the return error code now, because the next "if" may destroy it.
	TInt rc = ::Sql2OsErrCode(err, sqlite3SymbianLastOsError());
	if(err != SQLITE_OK)
		{//Yes, it is possible error code != SQLITE_OK and aDbHandle != NULL.
		CloseDbHandle(aDbHandle);
		aDbHandle = NULL;
		}
	return rc;
	}

//Calls sqlite3_open() to create or open database file with aFileNameZ.
//aFileNameZ is UTF8 encoded, zero-terminated.
//The function returns system-wide errors or database specific errors.
//The function may panic with code 7 in _DEBUG mode - internal error: the created database handle is NULL.
TInt CreateDbHandle8(const TDesC8& aFileNameZ, sqlite3*& aDbHandle)
	{
	(void)sqlite3SymbianLastOsError();//clear last OS error
	TInt err = sqlite3_open((const char *) aFileNameZ.Ptr(), &aDbHandle);
	__ASSERT_DEBUG(err == SQLITE_OK ? aDbHandle != NULL : ETrue, __SQLPANIC2(ESqlPanicInternalError));
	if(err == SQLITE_OK)
		{
		(void)sqlite3_extended_result_codes(aDbHandle, 0);
		}
	//Get the return error code now, because the next "if" may destroy it.
	TInt rc = ::Sql2OsErrCode(err, sqlite3SymbianLastOsError());
	if(err != SQLITE_OK)
		{//Yes, it is possible error code != SQLITE_OK and aDbHandle != NULL.
		CloseDbHandle(aDbHandle);
		aDbHandle = NULL;
		}
	return rc;
	}
	
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////                     16-bit and 8-bit SQL statements execution upon completion                        ////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//16-bit SQL statement execution.
//
//aSql - zero-terminated string
//
//Prepares the supplied as an argument (aSql) 16-bit SQL statement and executes it.
//If aSql argument contains more than one SQL statements, separated with ';', then
//the function panics in _DEBUG mode (panic code 7).
//
//The function panics in debug mode (panic code 2) if aDbHandle is NULL.
//
//If the function completes successfully, it returns SQLITE_ROW or SQLITE_DONE.
//If the function fails then it returns one of the SQLITE error codes.
static TInt DoSingleStmtExec16(sqlite3 *aDbHandle, const TDesC16& aSql)
	{
    __ASSERT_DEBUG(aDbHandle != NULL, __SQLPANIC2(ESqlPanicInvalidObj));
    __ASSERT_DEBUG(aSql.Length() > 0 ? (TInt)aSql[aSql.Length() - 1] == 0 : ETrue, __SQLPANIC2(ESqlPanicBadArgument));
	sqlite3_stmt* stmtHandle = NULL;
	const void* stmtTail = NULL;
    //sqlite3_prepare16_v2() expects parameter #3 to be one of the following:
    // - byte length of the sql statement (parameter #2), excluding terminating zero;
    // - negative value - the sql statement (parameter #2) is zero-terminated;
	TInt err = sqlite3_prepare16_v2(aDbHandle, aSql.Ptr(), aSql.Length() * sizeof(TUint16) - sizeof(TUint16), &stmtHandle, &stmtTail);
	__ASSERT_DEBUG(err == SQLITE_OK ? !stmtTail || User::StringLength((const TUint16*)stmtTail) == 0 : !stmtHandle, __SQLPANIC2(ESqlPanicInternalError));
	if(stmtHandle)	//stmtHandle can be NULL for statements like this: ";".
		{
		if(err == SQLITE_OK)
			{
			while((err = sqlite3_step(stmtHandle)) == SQLITE_ROW)
				{
				}
			if(err == SQLITE_ERROR)	//It may be "out of memory" problem
				{
				err = sqlite3_reset(stmtHandle);
				__ASSERT_DEBUG(err != SQLITE_OK, __SQLPANIC2(ESqlPanicInternalError));
				}
			}
		(void)sqlite3_finalize(stmtHandle);//sqlite3_finalize() fails only if an invalid statement handle is passed.
		}
	return err;
	}

/**
This function searches aString argument for ';' occurences.
Every time when it finds a ';' character, the function places a zero character right after the ';' and
tests the just created, zero-terminated substring if it is a comlpete SQL statement.

If it is a SQL statement, the function replaces the found ';' character with zero and returns the just created
zero-terminated substring.Also the function modifies aString argument to point right after the found
SQL string. If it is not SQL statement, the function will continue the searching.

If there is no ';' inside aString argument, the function returns the same string as a return result and
modifies aString argument - sets it to TPtr(NULL, 0, 0).

The function expects aString argument to be zero-terminated.

@internalComponent
*/
TPtrC GetFirstSqlStmt(TPtr& aString)
	{
	const TChar KDelimitier(';');
	TPtr originalStr(aString);
	TPtr str(const_cast <TUint16*> (aString.Ptr()), aString.Length(), aString.Length());
	TInt afterDelimitierPos = 0;
	TInt pos;
	while((pos = str.Locate(KDelimitier) + 1) > 0 && pos < str.Length())
		{
		//There is a possibility that the string, which terminates with the found ';' character, is a SQL statement.
		//Zero terminate the string placing a zero right after the ';' character and test it using sqlite3_complete16().
		//If it is not a SQL string, restore the original character and continue searching.
		afterDelimitierPos += pos;
		TChar ch = aString[afterDelimitierPos];
		aString[afterDelimitierPos] = 0;
		TInt res = sqlite3_complete16(aString.Ptr());
		aString[afterDelimitierPos] = ch;
		if(res)
			{
			str.Set(const_cast <TUint16*> (aString.Ptr()), afterDelimitierPos, afterDelimitierPos);	
			//Replace the found ';' character with 0.
			str[afterDelimitierPos - 1] = 0;
			aString.Set(const_cast <TUint16*> (aString.Ptr()) + afterDelimitierPos, aString.Length() - afterDelimitierPos, aString.Length() - afterDelimitierPos);
			return str;
			}
		str.Set(const_cast <TUint16*> (str.Ptr()) + pos, str.Length() - pos, str.Length() - pos);	
		}
	//aString argument does not contain valid SQL statement or there is no ';' character inside aString.
	//Set aString to TPtr(NULL, 0, 0) and return the original string.
	aString.Set(NULL, 0, 0);
	str.Set(originalStr);
	return str;
	}

/**
Executes one or more 16-bit SQL statements. SQL statements of any kind can be executed, but the 
method won't return any record(s) if the SQL statement type is "SELECT".
If the SQL statement(s) contains one or more parameters, the method will execute it giving the parameters
default NULL values.

@param aDbHandle Database handle. Not NULL.
@param aSqlStmt String containing one or more 16-bit SQL statements, separated with ';'. Zero-terminated string.
				Note: The ExecL() call can modify the content of aSqlStmt argument.

@return KErrNone, Operation completed successfully;
		KErrNoMemory, Out of memory;
		KSqlErrGeneral, Syntax error. A text message describing the problem can be obtained calling
						RSqlDatabase::LastErrorMessage();
						Other system-wide error codes or SQL errors of ESqlDbError type.

@panic SqlDb 4 In _DEBUG mode if aSqlStmt is not zero-terminated string
@panic SqlDb 7 In _DEBUG mode if aDbHandle is NULL

@internalComponent
*/
TInt DbExecStmt16(sqlite3* aDbHandle, TDes16& aSqlStmt)
	{
    __SQLTRACE_INTERNALSEXPR(TPtrC sqlprnptr(aSqlStmt.Left(aSqlStmt.Length() - 1)));
    SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, DBEXECSTMT16_ENTRY, "Entry;0x%X;DbExecStmt16;sql=%S", (TUint)aDbHandle, __SQLPRNSTR(sqlprnptr)));
	
	__ASSERT_DEBUG(aDbHandle != NULL, __SQLPANIC2(ESqlPanicInternalError));
	__ASSERT_DEBUG(aSqlStmt.Length() > 0 ? (TInt)aSqlStmt[aSqlStmt.Length() - 1] == 0: ETrue, __SQLPANIC2(ESqlPanicBadArgument));

	(void)sqlite3SymbianLastOsError();//clear last OS error
	
	TInt err = SQLITE_DONE;
	//Execute SQL statement(s)
	//16-bit SQL string - no sqlite3_exec16() function, so the execution is made with 
	//sqlite3_prepare16_v2() and sqlite3_step() functions.
	TPtr16 sql(const_cast <TUint16*> (aSqlStmt.Ptr()), aSqlStmt.Length(), aSqlStmt.Length());
	TPtrC firstSqlStmt(KNullDesC);
	while(err == SQLITE_DONE && sql.Length() > 1) //"> 1" because it is a zero terminated string
		{
		firstSqlStmt.Set(GetFirstSqlStmt(sql));
        SQLPROFILER_SQL16_PRINT((TUint)aDbHandle, firstSqlStmt.Left(firstSqlStmt.Length() - 1), EFalse);
		err = ::DoSingleStmtExec16(aDbHandle, firstSqlStmt);
		}
		
	err = ::Sql2OsErrCode(err, sqlite3SymbianLastOsError());
	if(err == KSqlAtEnd)
		{
		err = KErrNone;	
		}
	SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, DBEXECSTMT16_EXIT, "Exit;0x%X;DbExecStmt16;err=%d", (TUint)aDbHandle, err));
	return err;
	}

/**
Executes one or more 8-bit SQL statements. SQL statements of any kind can be executed, but the 
method won't return any record(s) if the SQL statement type is "SELECT".
If the SQL statement(s) contains one or more parameters, the method will execute it giving the parameters
default NULL values.

@param aDbHandle Database handle. Not NULL.
@param aSqlStmt String containing one or more 8-bit SQL statements, separated with ';'. Zero-terminated string.

@return KErrNone, Operation completed successfully;
		KErrNoMemory, Out of memory;
		KSqlErrGeneral, Syntax error. A text message describing the problem can be obtained calling
						RSqlDatabase::LastErrorMessage();
		Other system-wide error codes or SQL errors of ESqlDbError type.

@panic SqlDb 4 In _DEBUG mode if aSqlStmt is not zero-terminated string
@panic SqlDb 7 In _DEBUG mode if aDbHandle is NULL

@internalComponent
*/
TInt DbExecStmt8(sqlite3* aDbHandle, const TDesC8& aSqlStmt)
	{
    __SQLTRACE_INTERNALSEXPR(TPtrC8 sqlprnptr(aSqlStmt.Left(aSqlStmt.Length() - 1)));
	__SQLTRACE_INTERNALSVAR(TBuf<100> des16prnbuf);
    SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, DBEXECSTMT8_ENTRY, "Entry;0x%X;DbExecStmt8;sql=%s", (TUint)aDbHandle, __SQLPRNSTR8(sqlprnptr, des16prnbuf)));
    
	__ASSERT_DEBUG(aDbHandle != NULL, __SQLPANIC2(ESqlPanicInternalError));
	__ASSERT_DEBUG(aSqlStmt.Length() > 0 ? (TInt)aSqlStmt[aSqlStmt.Length() - 1] == 0: ETrue, __SQLPANIC2(ESqlPanicBadArgument));

    SQLPROFILER_SQL8_PRINT((TUint)aDbHandle, aSqlStmt.Left(aSqlStmt.Length() - 1), EFalse);
	
	(void)sqlite3SymbianLastOsError();//clear last OS error
	
	TInt err = sqlite3_exec(aDbHandle, reinterpret_cast <const char*> (aSqlStmt.Ptr()), NULL, NULL, NULL);

	err = ::Sql2OsErrCode(err, sqlite3SymbianLastOsError());
	__ASSERT_DEBUG(err != KSqlAtEnd, __SQLPANIC2(ESqlPanicInternalError));
	SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, DBEXECSTMT8_EXIT, "Exit;0x%X;DbExecStmt8;err=%d", (TUint)aDbHandle, err));
	return err;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////                     16-bit and 8-bit SQL statement preparation                                      /////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Prepares 16-bit SQL statement, returning in aStmtHandle the statement handle and
//setting aHasTail to true, if aStmt contains more than one sql statements.
//aStmt - zero-terminated string.
//Returns one of SQLITE error codes.
static TInt DoPrepareStmt16(sqlite3* aDbHandle, const TDesC& aStmt, sqlite3_stmt** aStmtHandle, TBool& aHasTail)
	{
	const void* stmtTail = NULL;
    //sqlite3_prepare16_v2() expects parameter #3 to be one of the following:
    // - byte length of the sql statement (parameter #2), excluding terminating zero;
    // - negative value - the sql statement (parameter #2) is zero-terminated;
	TInt err = sqlite3_prepare16_v2(aDbHandle, aStmt.Ptr(), aStmt.Length() * sizeof(TUint16) - sizeof(TUint16), aStmtHandle, &stmtTail);
	aHasTail = stmtTail && static_cast <const TUint16*> (stmtTail)[0] != 0;
	__ASSERT_DEBUG(err != SQLITE_OK ? !(*aStmtHandle) : ETrue, __SQLPANIC2(ESqlPanicInternalError));
	//(*aStmtHandle) is NULL for ";" statements, when err == SQLITE_OK. Since the server should not panic
	//that situation is handled later (not inside the assert above)
	return err;
	}
	
//Prepares 8-bit SQL statement, returning in aStmtHandle the statement handle and
//setting aHasTail to true, if aStmt contains more than one sql statements.
//aStmt - zero-terminated string.
//Returns one of SQLITE error codes.
static TInt DoPrepareStmt8(sqlite3* aDbHandle, const TUint8* aStmt, sqlite3_stmt** aStmtHandle, TBool& aHasTail)
	{
	const char* stmtTail = NULL;
    //sqlite3_prepare_v2() expects parameter #3 to be one of the following:
    // - byte length of the sql statement (parameter #2), excluding terminating zero;
    // - negative value - the sql statement (parameter #2) is zero-terminated;
	TInt err = sqlite3_prepare_v2(aDbHandle, reinterpret_cast <const char*> (aStmt), -1, aStmtHandle, &stmtTail);
	aHasTail = stmtTail && stmtTail[0] != 0;
	__ASSERT_DEBUG(err != SQLITE_OK ? !(*aStmtHandle) : ETrue, __SQLPANIC2(ESqlPanicInternalError));
	//(*aStmtHandle) is NULL for ";" statements, when err == SQLITE_OK. Since the server should not panic
	//that situation is handled later (not inside the assert above)
	return err;
	}

//This function accepts as arguments the SQLITE error code, 
//"aHasTail" boolean flag set to true if the SQL string contains more than one SQL statement 
//and the statement handle.
//
//It checks the arguments and returns an error if:
// - aSqliteError != SQLITE_OK;
// - aHasTail is true (possibly more than one SQL statement, separated with ";");
// - aStmtHandle is NULL;
//
static TInt ProcessPrepareError(TInt aSqliteError, TBool aHasTail, sqlite3_stmt*& aStmtHandle)
	{
	if(aSqliteError != SQLITE_OK)
		{
		return ::Sql2OsErrCode(aSqliteError, sqlite3SymbianLastOsError());
		}
	else if(aHasTail || !aStmtHandle)
		{//Case 1:
		 // More than one SQL statement or the SQL string is "" or ";;;" or ";   ;; ;". 
		 // Report it as an error, because there is no statement handle.
		 //Case 2:
		 // Non-null aHasTail. In this case the SQL string contains more than one SQL statement.
		 // The statement handle is not null. The statement has to be finialized before reporting the error.
		(void)FinalizeStmtHandle(aStmtHandle);
		aStmtHandle = NULL;
		return KErrArgument;
		}
	return KErrNone;
	}

//This function accepts as arguments the SQLITE error code, 
//"aHasTail" boolean flag set to true if the SQL string contains more than one SQL statement 
//and the statement handle.
//
//It checks the arguments and leaves if:
// - aSqliteError != SQLITE_OK;
// - aHasTail is true (possibly more than one SQL statement, separated with ";");
// - aStmtHandle is NULL;
//
static void LeaveIfPrepareErrorL(TInt aSqliteError, TBool aHasTail, sqlite3_stmt* aStmtHandle)
	{
	__SQLLEAVE_IF_ERROR2(ProcessPrepareError(aSqliteError, aHasTail, aStmtHandle));
	}

/**
Prepares 16-bit aSqlStmt SQL statement.

@param aSqlStmt - zero-terminated string.

@leave KErrNoMemory, if there is no memory;
	   KErrArgument, if the SQL string contains more than one SQL statements;
	   One of the error codes in [KSqlErrGeneral..KSqlErrNotDb] range.

@return The prepared SQL statement handle.

@internalComponent
*/
sqlite3_stmt* StmtPrepare16L(sqlite3* aDbHandle, const TDesC16& aSqlStmt)
	{
    __SQLTRACE_INTERNALSEXPR(TPtrC sqlprnptr(aSqlStmt.Left(aSqlStmt.Length() - 1)));
    SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, STMTPREPARE16L_ENTRY, "Entry;0x%X;StmtPrepare16L;sql=%S", (TUint)aDbHandle, __SQLPRNSTR(sqlprnptr)));	
	(void)sqlite3SymbianLastOsError();//clear last OS error
	TBool hasTail = EFalse;
	sqlite3_stmt* stmtHandle = NULL;
	TInt err = DoPrepareStmt16(aDbHandle, aSqlStmt, &stmtHandle, hasTail);
	LeaveIfPrepareErrorL(err, hasTail, stmtHandle);
	SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, STMTPREPARE16L_EXIT, "Exit;0x%X;StmtPrepare16L;stmtHandle=0x%X", (TUint)aDbHandle, (TUint)stmtHandle)); 
	return stmtHandle;
	}

/**
Prepares 8-bit aSqlStmt SQL statement.

@param aSqlStmt - zero-terminated string.

@leave KErrNoMemory, if there is no memory;
	   KErrArgument, if the SQL string contains more than one SQL statements;
	   One of the error codes in [KSqlErrGeneral..KSqlErrNotDb] range.

@return The prepared SQL statement handle.

@internalComponent
*/
TInt StmtPrepare8(sqlite3* aDbHandle, const TDesC8& aSqlStmt, sqlite3_stmt*& aStmtHandle)
	{
    __SQLTRACE_INTERNALSEXPR(TPtrC8 sqlprnptr(aSqlStmt.Left(aSqlStmt.Length() - 1)));
	__SQLTRACE_INTERNALSVAR(TBuf<100> des16prnbuf);
    SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, STMTPREPARE8_ENTRY, "Entry;0x%X;StmtPrepare8;sql=%s", (TUint)aDbHandle, __SQLPRNSTR8(sqlprnptr, des16prnbuf))); 
	(void)sqlite3SymbianLastOsError();//clear last OS error
	TBool hasTail = EFalse;
	TInt err = DoPrepareStmt8(aDbHandle, aSqlStmt.Ptr(), &aStmtHandle, hasTail);
	err = ProcessPrepareError(err, hasTail, aStmtHandle);
	SQL_TRACE_INTERNALS(OstTraceExt3(TRACE_INTERNALS, STMTPREPARE8_EXIT, "Exit;0x%X;StmtPrepare8;aStmtHandle=0x%X;err=%d", (TUint)aDbHandle, (TUint)aStmtHandle, err)); 
	return err;
	}

/**
Prepares 8-bit aSqlStmt SQL statement.

@param aSqlStmt - zero-terminated string.

@leave KErrNoMemory, if there is no memory;
	   KErrArgument, if the SQL string contains more than one SQL statements;
	   One of the error codes in [KSqlErrGeneral..KSqlErrNotDb] range.

@return The prepared SQL statement handle.

@internalComponent
*/
sqlite3_stmt* StmtPrepare8L(sqlite3* aDbHandle, const TDesC8& aSqlStmt)
	{
    __SQLTRACE_INTERNALSEXPR(TPtrC8 sqlprnptr(aSqlStmt.Left(aSqlStmt.Length() - 1)));
	__SQLTRACE_INTERNALSVAR(TBuf<100> des16prnbuf);
    SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, STMTPREPARE8L_ENTRY, "Entry;0x%X;StmtPrepare8L;sql=%s", (TUint)aDbHandle, __SQLPRNSTR8(sqlprnptr, des16prnbuf))); 
	(void)sqlite3SymbianLastOsError();//clear last OS error
	TBool hasTail = EFalse;
	sqlite3_stmt* stmtHandle = NULL;
	TInt err = DoPrepareStmt8(aDbHandle, aSqlStmt.Ptr(), &stmtHandle, hasTail);
	LeaveIfPrepareErrorL(err, hasTail, stmtHandle);
	SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, STMTPREPARE8L_EXIT, "Exit;0x%X;StmtPrepare8L;stmtHandle=0x%X", (TUint)aDbHandle, (TUint)stmtHandle)); 
	return stmtHandle;
	}

/**
Executes upon completion the prepared SQL statement.

@param aStmtHandle Prepared statement handle

@return KSqlErrStmtExpired, Statement expired (if new functions or collating sequences are 
							registered or if an authorizer function is added or changed);
		KErrNoMemory, Out of memory. The statement will be reset;
		KErrNone, The reset operation completed successfully.							

@panic SqlDb 2 In _DEBUG mode. Invalid (NULL) statement handle.
@panic SqlDb 7 In _DEBUG mode. SQLITE internal error. (SQLITE_ERROR, followed by a sqlite3_reset(), which returns SQLITE_OK)

@internalComponent
*/	
TInt StmtExec(sqlite3_stmt* aStmtHandle)
	{
	SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, STMTEXEC_ENTRY, "Entry;0x%X;StmtExec;aStmtHandle=0x%X", (TUint)sqlite3_db_handle(aStmtHandle), (TUint)aStmtHandle));
	__ASSERT_DEBUG(aStmtHandle != NULL, __SQLPANIC2(ESqlPanicInvalidObj));
	
	(void)sqlite3SymbianLastOsError();//clear last OS error

	TInt err;
	while((err = sqlite3_step(aStmtHandle)) == SQLITE_ROW)
		{
		}
		
	if(err == SQLITE_ERROR)	//It may be "out of memory" problem
		{
		err = sqlite3_reset(aStmtHandle);
		__ASSERT_DEBUG(err != SQLITE_OK, __SQLPANIC2(ESqlPanicInternalError));
		}
		
	err = ::Sql2OsErrCode(err, sqlite3SymbianLastOsError());
	if(err == KSqlAtEnd)
		{
		err = KErrNone;	
		}

	SQL_TRACE_INTERNALS(OstTraceExt3(TRACE_INTERNALS, STMTEXEC_EXIT, "Exit;0x%X;StmtExec;aStmtHandle=0x%X;err=%d", (TUint)sqlite3_db_handle(aStmtHandle), (TUint)aStmtHandle, err));
	return err;
	}

/**
Executes the SQL statement moving it to the next row if available.

@return KSqlErrStmtExpired Statement expired (if new functions or collating sequences are 
							registered or if an authorizer function is added or changed)
@return KSqlAtRow, The next record data is ready for processing by the caller;
		KSqlAtEnd, No more record data;
		KSqlErrBusy, Database file is locked;
		KSqlErrGeneral, Run-time error. Next() should not be called anymore;
		KSqlErrMisuse, Next() called after KSqlAtEnd or KSqlErrGeneral returned by the previous Next() call;
		KErrNoMemory, Out of memory. The statement will be reset.

@panic SqlDb 2 In _DEBUG mode. Invalid (NULL) statement handle.
@panic SqlDb 7 In _DEBUG mode. SQLITE internal error. (SQLITE_ERROR, followed by a sqlite3_reset(), which returns SQLITE_OK)

@internalComponent
*/	
TInt StmtNext(sqlite3_stmt* aStmtHandle)
	{
	SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, STMTNEXT_ENTRY, "Entry;0x%X;StmtNext;aStmtHandle=0x%X", (TUint)sqlite3_db_handle(aStmtHandle), (TUint)aStmtHandle));
	__ASSERT_DEBUG(aStmtHandle != NULL, __SQLPANIC2(ESqlPanicInvalidObj));
	
	(void)sqlite3SymbianLastOsError();//clear last OS error
	
	TInt err = sqlite3_step(aStmtHandle);
	if(err == SQLITE_ERROR)	//It may be "out of memory" problem
		{
		err = sqlite3_reset(aStmtHandle);
		__ASSERT_DEBUG(err != SQLITE_OK, __SQLPANIC2(ESqlPanicInternalError));
		}
	err = ::Sql2OsErrCode(err, sqlite3SymbianLastOsError());
	SQL_TRACE_INTERNALS(OstTraceExt3(TRACE_INTERNALS, STMTNEXT_EXIT, "Exit;0x%X;StmtNext;aStmtHandle=0x%X;err=%d", (TUint)sqlite3_db_handle(aStmtHandle), (TUint)aStmtHandle, err));
	return err;
	}

/**
Resets the prepared SQL statement to its initial state and makes it ready to be executed again.
Any SQL statement parameters that had values bound to them, retain their values.

@return KSqlErrStmtExpired, Statement expired (if new functions or collating sequences are 
							registered or if an authorizer function is added or changed);
		KErrNone, The reset operation completed successfully.							

@panic SqlDb 2 In _DEBUG mode. Invalid (NULL) statement handle.

@internalComponent
*/	
TInt StmtReset(sqlite3_stmt* aStmtHandle)
	{
	SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, STMTRESET_ENTRY, "Entry;0x%X;StmtReset;aStmtHandle=0x%X", (TUint)sqlite3_db_handle(aStmtHandle), (TUint)aStmtHandle));
	__ASSERT_DEBUG(aStmtHandle != NULL, __SQLPANIC2(ESqlPanicInvalidObj));
	
	(void)sqlite3SymbianLastOsError();//clear last OS error
	
	TInt err = sqlite3_reset(aStmtHandle);
	err = ::Sql2OsErrCode(err, sqlite3SymbianLastOsError());
	SQL_TRACE_INTERNALS(OstTraceExt3(TRACE_INTERNALS, STMTRESET_EXIT, "Exit;0x%X;StmtReset;aStmtHandle=0x%X;err=%d", (TUint)sqlite3_db_handle(aStmtHandle), (TUint)aStmtHandle, err));
	return err;
	}

/**
Prepares and executes PRAGMA statement and moves the statement cursor on the first row.

@param aDbHandle Database handle
@param aDbName Attached database name or KNullDesC for the main database
@param aPragmaSql Pragma sql statement
@param aStmtHandle An output parameter where the statement handle will be stored

@panic SqlDb 7 In _DEBUG mode if aDbHandle is NULL
@panic SqlDb 4 In _DEBUG mode if aPragmaSql length is 0 or if the statement is not zero-terminated

@return KErrNone,     Operation completed successfully;
		KErrNoMemory, Out of memory;
					  Other system-wide error codes or SQL errors of ESqlDbError type.

@internalComponent
*/
static TInt PreRetrievePragmaValue(sqlite3* aDbHandle, const TDesC& aDbName, const TDesC8& aPragmaSql, sqlite3_stmt*& aStmtHandle)
	{
    __SQLTRACE_INTERNALSEXPR(TPtrC8 sqlprnptr(aPragmaSql.Left(aPragmaSql.Length() - 1)));
	__SQLTRACE_INTERNALSVAR(TBuf<100> des16prnbuf);
    SQL_TRACE_INTERNALS(OstTraceExt3(TRACE_INTERNALS, PRERETRIEVEPRAGMAVALUE_ENTRY, "Entry;0x%X;PreRetrievePragmaValue;aDbName=%S;aPragmaSql=%s", (TUint)aDbHandle, __SQLPRNSTR(aDbName), __SQLPRNSTR8(sqlprnptr, des16prnbuf)));
	__ASSERT_DEBUG(aDbHandle != NULL, __SQLPANIC2(ESqlPanicInternalError));
	__ASSERT_DEBUG(aPragmaSql.Length() > 0, __SQLPANIC2(ESqlPanicBadArgument));
	__ASSERT_DEBUG(aPragmaSql[aPragmaSql.Length() - 1] == 0, __SQLPANIC2(ESqlPanicBadArgument));
	TBuf8<KMaxFileName> dbName;
	if(!UTF16ToUTF8(aDbName, dbName))
		{
		SQL_TRACE_INTERNALS(OstTrace1(TRACE_INTERNALS, PRERETRIEVEPRAGMAVALUE_EXIT1, "Exit;0x%X;PreRetrievePragmaValue;err=KErrGeneral", (TUint)aDbHandle));
		return KErrGeneral;
		}
	TBuf8<KMaxFileName + 64> sql;//64 characters is enough for the longest PRAGMA statement
	if(dbName == KNullDesC8)
		{
		sql.Format(aPragmaSql, &KMainDb8);
		}
	else
		{
		sql.Format(aPragmaSql, &dbName);
		}
	aStmtHandle = NULL;
	TInt err = ::StmtPrepare8(aDbHandle, sql, aStmtHandle);
	if(err == KErrNone)
		{
		__ASSERT_DEBUG(aStmtHandle != NULL, __SQLPANIC2(ESqlPanicInvalidObj));
		err = ::StmtNext(aStmtHandle);
		}
	SQL_TRACE_INTERNALS(OstTraceExt3(TRACE_INTERNALS, PRERETRIEVEPRAGMAVALUE_EXIT2, "Exit;0x%X;PreRetrievePragmaValue;aStmtHandle=0x%X;err=%d", (TUint)aDbHandle, (TUint)aStmtHandle, err));
	return err;
	}

/**
Prepares and executes PRAGMA statement and retrieves the value of column 0 (the pragma value).

@param aDbHandle Database handle
@param aDbName Attached database name or KNullDesC for the main database
@param aPragmaSql Pragma sql statement
@param aPragmaValue An output parameter where the pragma value will be stored

@panic SqlDb 7 In _DEBUG mode if aDbHandle is NULL
@panic SqlDb 4 In _DEBUG mode if aPragmaSql length is 0 or if the statement is not zero-terminated

@return KErrNone,     Operation completed successfully;
		KErrNoMemory, Out of memory;
					  Other system-wide error codes or SQL errors of ESqlDbError type.

@internalComponent
*/
static TInt RetrievePragmaValue(sqlite3* aDbHandle, const TDesC& aDbName, const TDesC8& aPragmaSql, TInt& aPragmaValue)
	{
	__ASSERT_DEBUG(aDbHandle != NULL, __SQLPANIC2(ESqlPanicInternalError));
	__ASSERT_DEBUG(aPragmaSql.Length() > 0, __SQLPANIC2(ESqlPanicBadArgument));
	__ASSERT_DEBUG(aPragmaSql[aPragmaSql.Length() - 1] == 0, __SQLPANIC2(ESqlPanicBadArgument));
	sqlite3_stmt* stmtHandle = NULL;
	TInt err = PreRetrievePragmaValue(aDbHandle, aDbName, aPragmaSql, stmtHandle);
	if(err == KSqlAtRow)
		{
		aPragmaValue = sqlite3_column_int(stmtHandle, 0);
		err = aPragmaValue >= 0 ? KErrNone : KErrCorrupt;
		}
	(void)FinalizeStmtHandle(stmtHandle);//sqlite3_finalize() fails only if an invalid statement handle is passed.
	return err;
	}

/**
Prepares and executes PRAGMA statement and retrieves the value of column 0 (the pragma value) as text.

@param aDbHandle Database handle
@param aDbName Attached database name or KNullDesC for the main database
@param aPragmaSql Pragma sql statement
@param aPragmaValue An output parameter where the pragma value will be stored (as text)

@panic SqlDb 7 In _DEBUG mode if aDbHandle is NULL
@panic SqlDb 4 In _DEBUG mode if aPragmaSql length is 0 or if the statement is not zero-terminated

@return KErrNone,     Operation completed successfully;
		KErrNoMemory, Out of memory;
					  Other system-wide error codes or SQL errors of ESqlDbError type.

@internalComponent
*/
static TInt RetrievePragmaValue(sqlite3* aDbHandle, const TDesC& aDbName, const TDesC8& aPragmaSql, TDes8& aPragmaValue)
	{
	__ASSERT_DEBUG(aDbHandle != NULL, __SQLPANIC2(ESqlPanicInternalError));
	__ASSERT_DEBUG(aPragmaSql.Length() > 0, __SQLPANIC2(ESqlPanicBadArgument));
	__ASSERT_DEBUG(aPragmaSql[aPragmaSql.Length() - 1] == 0, __SQLPANIC2(ESqlPanicBadArgument));
	sqlite3_stmt* stmtHandle = NULL;
	TInt err = PreRetrievePragmaValue(aDbHandle, aDbName, aPragmaSql, stmtHandle);
	if(err == KSqlAtRow)
		{
		TPtrC8 ptr(sqlite3_column_text(stmtHandle, 0));
		aPragmaValue.Copy(ptr);
		err = KErrNone;
		}
	(void)FinalizeStmtHandle(stmtHandle);//sqlite3_finalize() fails only if an invalid statement handle is passed.
	return err;
	}

/**
Retrieves the database pages count.

@param aDbHandle Database handle
@param aDbName Attached database name or KNullDesC for the main database
@param aPageCount An output parameter where the database pages count will be stored

@return KErrNone,     Operation completed successfully;
		KErrNoMemory, Out of memory;
					  Other system-wide error codes or SQL errors of ESqlDbError type.

@panic SqlDb 7 In _DEBUG mode if aDbHandle is NULL

@internalComponent
*/
TInt DbPageCount(sqlite3* aDbHandle, const TDesC& aDbName, TInt& aPageCount)
	{
	__ASSERT_DEBUG(aDbHandle != NULL, __SQLPANIC2(ESqlPanicInternalError));
	return RetrievePragmaValue(aDbHandle, aDbName, KPageCountPragma, aPageCount);
	}

/**
Retrieves the database page size.

@param aDbHandle Database handle
@param aDbName Attached database name or KNullDesC for the main database
@param aPageSize An output parameter where the page size will be stored

@return KErrNone,     Operation completed successfully;
		KErrNoMemory, Out of memory;
					  Other system-wide error codes or SQL errors of ESqlDbError type.

@panic SqlDb 7 In _DEBUG mode if aDbHandle is NULL

@internalComponent
*/
TInt DbPageSize(sqlite3* aDbHandle, const TDesC& aDbName, TInt& aPageSize)
	{
	__ASSERT_DEBUG(aDbHandle != NULL, __SQLPANIC2(ESqlPanicInternalError));
	return RetrievePragmaValue(aDbHandle, aDbName, KPageSizePragma, aPageSize);
	}

/**
Retrieves the database cache size in pages.

@param aDbHandle Database handle
@param aDbName Attached database name or KNullDesC for the main database
@param aCacheSize An output parameter where the cache size will be stored

@return KErrNone,     Operation completed successfully;
		KErrNoMemory, Out of memory;
					  Other system-wide error codes or SQL errors of ESqlDbError type.

@panic SqlDb 7 In _DEBUG mode if aDbHandle is NULL

@internalComponent
*/
TInt DbCacheSize(sqlite3* aDbHandle, const TDesC& aDbName, TInt& aCacheSize)
	{
	__ASSERT_DEBUG(aDbHandle != NULL, __SQLPANIC2(ESqlPanicInternalError));
	return RetrievePragmaValue(aDbHandle, aDbName, KCacheSizePragma, aCacheSize);
	}

/**
Retrieves the database encoding.

@param aDbHandle Database handle
@param aDbName Attached database name or KNullDesC for the main database
@param aEncoding An output parameter where the encoding type will be stored (as text)

@return KErrNone,     Operation completed successfully;
		KErrNoMemory, Out of memory;
					  Other system-wide error codes or SQL errors of ESqlDbError type.

@panic SqlDb 7 In _DEBUG mode if aDbHandle is NULL

@internalComponent
*/
TInt DbEncoding(sqlite3* aDbHandle, const TDesC& aDbName, TDes8& aEncoding)
	{
	__ASSERT_DEBUG(aDbHandle != NULL, __SQLPANIC2(ESqlPanicInternalError));
	return RetrievePragmaValue(aDbHandle, aDbName, KEncodingPragma, aEncoding);
	}

/**
Retrieves the database free pages count.

@param aDbHandle Database handle
@param aDbName Attached database name or KNullDesC for the main database
@param aPageCount An output parameter where the free pages count will be stored

@return KErrNone,     Operation completed successfully;
		KErrNoMemory, Out of memory;
					  Other system-wide error codes or SQL errors of ESqlDbError type.

@panic SqlDb 7 In _DEBUG mode if aDbHandle is NULL

@internalComponent
*/
TInt DbFreePageCount(sqlite3* aDbHandle, const TDesC& aDbName, TInt& aPageCount)
	{
	__ASSERT_DEBUG(aDbHandle != NULL, __SQLPANIC2(ESqlPanicInternalError));
	return RetrievePragmaValue(aDbHandle, aDbName, KFreePageCountPragma, aPageCount);
	}

/**
Retrieves the current vacuum mode of the database.

@param aDbHandle Database handle
@param aDbName Attached database name or KNullDesC for the main database
@param aVacuumMode An output parameter where the current vacuum mode will be stored

@return KErrNone,     Operation completed successfully;
		KErrNoMemory, Out of memory;
					  Other system-wide error codes or SQL errors of ESqlDbError type.

@panic SqlDb 7 In _DEBUG mode if aDbHandle is NULL

@internalComponent
*/
TInt DbVacuumMode(sqlite3* aDbHandle, const TDesC& aDbName, TInt& aVacuumMode)
	{
	__ASSERT_DEBUG(aDbHandle != NULL, __SQLPANIC2(ESqlPanicInternalError));
	return RetrievePragmaValue(aDbHandle, aDbName, KVacuumModePragma, aVacuumMode);
	}

static TBool IsCompactTimeLimitReached(TUint32 aStartTicks, TUint32 aCurrTicks, TInt aMaxTime)
	{
	__ASSERT_DEBUG(aMaxTime > 0, __SQLPANIC2(ESqlPanicBadArgument));
	TInt64 tickDiff64 = (TInt64)aCurrTicks - (TInt64)aStartTicks;
	if(tickDiff64 < 0)
		{
		tickDiff64 = KMaxTUint32 + tickDiff64 + 1;
		}
	static TInt freq = 0;
	TInt err = KErrNone;
	if(freq == 0)
		{
		err = HAL::Get(HAL::EFastCounterFrequency, freq);
		SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, ISCOMPACTTIMELIMITREACHED, "0;IsCompactTimeLimitReached;fast counter frequency=%d;err=%d", freq, err));
		}
	if(err == KErrNone && freq > 0)
		{
		const TInt KMicroSecIn1Sec = 1000000;
		const TInt KMicroSecIn1Ms = 1000;
		TInt64 usDiff64 = (tickDiff64 * KMicroSecIn1Sec) / freq;
		if(usDiff64 > aMaxTime * KMicroSecIn1Ms)
			{
			return ETrue;	
			}
		}
	return EFalse;
	}

/**
Compacts the database.

@param aDbHandle 			Database handle.
@param aPageCount 			Count of the free database pages to be removed from the file.
@param aProcessedPageCount	Output parameter. How many pages actually have been removed from the file.
@param aMaxTime				The max allowed time in milliseconds for the compact operation.
							If aMaxTime is zero, then aPageCount pages will be removed regardless the time.

@return KErrNone,     Operation completed successfully;
					  Other system-wide error codes or SQL errors of ESqlDbError type.

@panic SqlDb 4 In _DEBUG mode if aPageCount is negative
@panic SqlDb 4 In _DEBUG mode if aMaxTime is negative
@panic SqlDb 7 In _DEBUG mode if aDbHandle is NULL

@internalComponent
*/
TInt DbCompact(sqlite3* aDbHandle, const TDesC& aDbName, TInt aPageCount, TInt& aProcessedPageCount, TInt aMaxTime)
	{
	SQL_TRACE_INTERNALS(OstTraceExt4(TRACE_INTERNALS, DBCOMPACT_ENTRY, "Entry;0x%X;DbCompact;aDbName=%S;aPageCount=%d;aMaxTime=%d", (TUint)aDbHandle, __SQLPRNSTR(aDbName), aPageCount, aMaxTime));
	__ASSERT_DEBUG(aPageCount >= 0, __SQLPANIC2(ESqlPanicBadArgument));
	__ASSERT_DEBUG(aMaxTime >= 0, __SQLPANIC2(ESqlPanicBadArgument));
	__ASSERT_DEBUG(aDbHandle != NULL, __SQLPANIC2(ESqlPanicInternalError));
	TBuf8<KMaxFileName> dbName;
	if(!UTF16ToUTF8(aDbName, dbName))
		{
		SQL_TRACE_INTERNALS(OstTrace1(TRACE_INTERNALS, DBCOMPACT_EXIT1, "Exit;0x%X;DbCompact;err=KErrGeneral", (TUint)aDbHandle));
		return KErrGeneral;
		}
	TBuf8<KMaxFileName + sizeof(KIncrementalVacuumPragma) + 1> sql;
	if(dbName == KNullDesC8)
		{
		sql.Format(KIncrementalVacuumPragma, &KMainDb8, aPageCount);
		}
	else
		{
		sql.Format(KIncrementalVacuumPragma, &dbName, aPageCount);
		}
	//Currently there is no way to check how many pages have been compacted without executing a "PRAGMA freelist_count"	
	//statement, if sqlite3_exec() is used. 
	//So, instead of calling sqlite3_exec(), the function prepares and executes the "PRAGMA incremental_vacuum(N)"
	//statement using sqlite3_step() and counts the steps, because each step compacts one page.
	(void)sqlite3SymbianLastOsError();//clear last OS error
	sqlite3_stmt* stmtHandle = NULL;
	const char* stmtTail = NULL;
	aProcessedPageCount = 0;
	//sqlite3_prepare_v2() expects parameter #3 to be one of the following:
	// - byte length of the sql statement (parameter #2), excluding terminating zero;
	// - negative value - the sql statement (parameter #2) is zero-terminated;
	TInt err = sqlite3_prepare_v2(aDbHandle, (const char*)sql.Ptr(), sql.Length() - sizeof(TUint8), &stmtHandle, &stmtTail);
	__ASSERT_DEBUG(err == SQLITE_OK ? !stmtTail || User::StringLength((const TUint8*)stmtTail) == 0 : !stmtHandle, __SQLPANIC2(ESqlPanicInternalError));
	if(stmtHandle)	//stmtHandle can be NULL for statements like this: ";".
		{
		if(err == SQLITE_OK)
			{
			TUint32 startTicks = 0;
			if(aMaxTime > 0)
				{
				startTicks = User::FastCounter();
				}
			
			while((err = sqlite3_step(stmtHandle)) == SQLITE_ROW)
                {
                ++aProcessedPageCount;
                if(aMaxTime > 0 && IsCompactTimeLimitReached(startTicks, User::FastCounter(), aMaxTime))
                    {
                    err = SQLITE_DONE;//The statement execution did not complete because of the time limit
                    break;  
                    }
                }
			
			if(err == SQLITE_ERROR)  //It may be "out of memory" problem
                {
                err = sqlite3_reset(stmtHandle);
                __ASSERT_DEBUG(err != SQLITE_OK, __SQLPANIC2(ESqlPanicInternalError));
                }
			}
		(void)sqlite3_finalize(stmtHandle);//sqlite3_finalize() fails only if an invalid statement handle is passed.
		}
	err = ::Sql2OsErrCode(err, sqlite3SymbianLastOsError());
	if(err == KSqlAtEnd)
		{
		err = KErrNone;	
		}
	SQL_TRACE_INTERNALS(OstTraceExt3(TRACE_INTERNALS, DBCOMPACT_EXIT2, "Exit;0x%X;DbCompact;aProcessedPageCount=%d;err=%d", (TUint)aDbHandle, aProcessedPageCount, err));
	return err;
	}

/**
Finalizes the statement handle.
Although the function can return an error, it is ok not to check the returned error,
because sqlite3_finalize() fails only if invalid statement handle is passed as an argument.

@return KErrNone,     Operation completed successfully;
					  Other system-wide error codes or SQL errors of ESqlDbError type.

@internalComponent
*/
TInt FinalizeStmtHandle(sqlite3_stmt* aStmtHandle)
	{
	SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, FINALIZESTMTHANDLE_ENTRY, "Entry;0x%X;FinalizeStmtHandle;aStmtHandle=0x%X", (TUint)sqlite3_db_handle(aStmtHandle), (TUint)aStmtHandle));
	TInt err = KErrNone;
	if(aStmtHandle)
		{
		(void)sqlite3SymbianLastOsError();//clear last OS error
		err = sqlite3_finalize(aStmtHandle);
		err = ::Sql2OsErrCode(err, sqlite3SymbianLastOsError());
		}
	SQL_TRACE_INTERNALS(OstTrace1(TRACE_INTERNALS, FINALIZESTMTHANDLE_EXIT, "Exit;0;FinalizeStmtHandle;err=%d", err));
	return err;
	}
