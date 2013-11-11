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

/**
 
 Retrieves a reference to the textual description of the error returned by the
 most recent call to any of the functions:
 - CSqlSrvDatabase::ExecL();
 - CSqlSrvStatement::NewLC();
 - CSqlSrvStatement::Next();
 - CSqlSrvStatement::Reset();
 - CSqlSrvStatement::Exec();
 
 Note that the function can only return a reference to text for
 database-specific type errors, i.e. those errors that are categorised as of
 type ESqlDbError.
 
 @return A non-modifiable pointer descriptor representing the most recent error
 message. Note that message may be NULL, i.e. the descriptor may have
 zero length.
 
 @see CSqlSrvDatabase::ExecL()
 @see CSqlSrvStatement::NewLC()
 @see CSqlSrvStatement::Next()
 @see CSqlSrvStatement::Reset()
 @see CSqlSrvStatement::Exec()
*/
inline TPtrC CSqlSrvDatabase::LastErrorMessage() const
	{
	const void* errMsg = sqlite3_errmsg16(iDbHandle);//"errMsg" - zero terminated string
	TPtrC msg(reinterpret_cast <const TUint16*> (errMsg));//terminating zero removed
	return msg;
	}
	
/**
Executes one or more 16-bit SQL statements.

SQL statements of any kind can be executed, but note the following points:
- the function does not return any records if the SQL statement type is "SELECT".
- if an SQL statement contains one or more parameters, then the function will execute it,
giving the parameters default NULL values.

If the call to this function fails because of a database-specific type error 
(i.e. the error is categorised as of type ESqlDbError), then a textual description of
the error can be obtained calling CSqlSrvDatabase::LastErrorMessage().

@param aSqlStmt A zero-terminated string descriptor of 16-bit wide characters containing one or more 
                SQL statements. Each statement is separated by a ';' character.
                Note that the ExecL() call can modify the content of aSqlStmt parameter.

@leave  KErrNoMemory, an out of memory condition has occured;
        KSqlErrGeneral, a syntax error has occurred - text describing the problem
                        can be obtained by calling 	CSqlSrvDatabase::LastErrorMessage();
        KErrPermissionDenied, the caller does not satisfy the relevant database security policies.
                      Note that the function may also leave with some other database specific 
                      errors categorised as ESqlDbError, and other system-wide error codes.
                      
@see CSqlSrvDatabase::LastErrorMessage()
*/
inline void CSqlSrvDatabase::ExecL(TDes16& aSqlStmt)
	{
	__SQLLEAVE_IF_ERROR(::DbExecStmt16(iDbHandle, aSqlStmt));
	}
	
/**
Executes one or more 8-bit SQL statements.

SQL statements of any kind can be executed, but note the following points:
- the function does not return any records if the SQL statement type is "SELECT".
- if an SQL statement contains one or more parameters, then the function will execute it,
giving the parameters default NULL values.

If the call to this function fails because of a database-specific type error 
(i.e. the error is categorised as of type ESqlDbError), then a textual description of
the error can be obtained calling CSqlSrvDatabase::LastErrorMessage().

@param aSqlStmt A zero-terminated string descriptor of 8-bit wide characters containing one or more 
                SQL statements. Each statement is separated by a ';' character.

@leave  KErrNoMemory, an out of memory condition has occured;
        KSqlErrGeneral, a syntax error has occurred - text describing the problem
                        can be obtained by calling 	CSqlSrvDatabase::LastErrorMessage();
        KErrPermissionDenied, the caller does not satisfy the relevant database security policies.
                      Note that the function may also leave with some other database specific 
                      errors categorised as ESqlDbError, and other system-wide error codes.
                      
@see CSqlSrvDatabase::LastErrorMessage()
*/
inline void CSqlSrvDatabase::ExecL(const TDesC8& aSqlStmt)
	{
	__SQLLEAVE_IF_ERROR(::DbExecStmt8(iDbHandle, aSqlStmt));
	}
	
/**
Sets the transaction isolation level for the database.

A transaction isolation level defines the way in which a transaction
interacts with other transactions that may be in progress concurrently.

Transaction isolation levels are defined by the values of the
RSqlDatabase::TIsolationLevel enum.

The default isolation level is RSqlDatabase::ESerializable

Note that the isolation level is not a persistent property of the database.
It is set to the default value, i.e. RSqlDatabase::ESerializable,
whenever the database is created or opened.

@param aLevel The isolation level to be set.

@leave KErrNotSupported, invalid (not supported) transaction isolation level.
						 Current implementation of CSqlSrvDatabase::SetIsolationLevelL()
						 does not support RSqlDatabase::EReadCommitted and RSqlDatabase::ERepeatableRead
						 transaction isolation levels.
*/
inline void CSqlSrvDatabase::SetIsolationLevelL(RSqlDatabase::TIsolationLevel aLevel)
	{
	if(aLevel != RSqlDatabase::EReadUncommitted && aLevel != RSqlDatabase::ESerializable)
		{
		__SQLLEAVE(KErrNotSupported);
		}
	iAuthorizerDisabled = ETrue;
	TInt err = ::DbExecStmt8(iDbHandle, aLevel == RSqlDatabase::EReadUncommitted ? KReadUncommittedPragma() : KSerializablePragma());
	iAuthorizerDisabled = EFalse;
	__SQLLEAVE_IF_ERROR(err);
	}
	
/**
@return sqlite3 handle

@panic SqlDb 7 In _DEBUG mode. iDbHandle is NULL.
*/
inline sqlite3* CSqlSrvDatabase::RawDbHandle() const
	{
	__ASSERT_DEBUG(iDbHandle != NULL, __SQLPANIC(ESqlPanicInternalError));
	return iDbHandle;	
	}

/**
@return A pointer to the database security policies object.

Note that there may be no security policies in force for this database (if it is not a secure database).

@see CSqlSecurityPolicy
*/	
inline const CSqlSecurityPolicy* CSqlSrvDatabase::SecurityPolicy() const
	{
	return iSecurityPolicy;	
	}

/**
Installs user defined collations.

At the moment 5 user defined collations with the following names are installed:
- CompareC0 - 16-bit strings collated comaprison at level 0;
- CompareC1 - 16-bit strings collated comaprison at level 1;
- CompareC2 - 16-bit strings collated comaprison at level 2;
- CompareC3 - 16-bit strings collated comaprison at level 3;
- CompareF  - 16-bit strings folded comaprison;

These user defined collations can be used in the following cases:

- as column constraint in "CREATE TABLE" SQL statements. For example:
@code
  CREATE TABLE A(Col1 TEXT COLLATE CompareC1)
@endcode
In this case every time when Col1 values have to be compared, the SQL server will use CompareC1 collation.

- as column constraint in "CREATE INDEX" SQL statements. For example:
@code
  CREATE INDEX I ON A(Col1 COLLATE CompareC2)
@endcode
In this case SQL server will use CompareC2 collation to compare Col1 values when using the index.

- In "ORDER BY" clause of "SELECT" SQL statements. For example:
@code
  SELECT * FROM A ORDER BY Col1 COLLATE CompareC3
@endcode
In this case SQL server will use CompareC3 collation to compare Col1 values when building the result set.

This function is part of CSqlSrvDatabase instance initialization.

@leave The function may leave with some database specific errors categorised as ESqlDbError.

@see TSqlCollate
@see TSqlCollate::InstallCollationsL()
*/
inline void CSqlSrvDatabase::InstallCollationsL()
	{
	TSqlCollationUtil collationUtil(iDbHandle);
	collationUtil.InstallCollationsL();
	}

/**
Stores the initial settings in the system settings table of a database. 
The initial settings are the specified collation dll name and version 0
of the database configuration file.
The collation dll name is used to uniquely identify the collation 
method which is used by the collation comparison methods: 
TDesC16::CompareC(), TDesC16::MatchC(), TDesC16::FindC().
This function is used when a new database is created.

@param aCollationDllName Collation dll name. It uniquely identifies the current collation method in use.
                         If the default collation method changes later then the database will be reindexed 
                         and the new collation dll name will replace the existing one in the settings table.
@param aDbConfigFileVersion Current config file version or KSqlNullDbConfigFileVersion
@param aCompactionMode Database compaction mode, one of TSqlCompactionMode enum item values (except ESqlCompactionNotSet)

@see TSqlCompactionMode

@panic SqlDb 4 In _DEBUG mode if aCompactionMode parameter value is invalid.
*/
inline void CSqlSrvDatabase::StoreSettingsL(const TDesC& aCollationDllName, TInt aDbConfigFileVersion, TSqlCompactionMode aCompactionMode)
	{
	__ASSERT_DEBUG(aCompactionMode == ESqlCompactionManual || aCompactionMode == ESqlCompactionBackground || aCompactionMode == ESqlCompactionAuto, __SQLPANIC(ESqlPanicBadArgument));
#if !defined(__SQL_DISABLE_SYMBIAN_SETTINGS_TABLE__)		
	TSqlDbSysSettings dbSettings(iDbHandle);
	dbSettings.StoreSettingsL(KMainDb16, aCollationDllName, aDbConfigFileVersion, aCompactionMode);
#else
	aCollationDllName.Ptr(); // to avoid compile-time warning
#endif // !(__SQL_DISABLE_SYMBIAN_SETTINGS_TABLE__)		
	}

/**
This function returns the number of database rows that were updated/inserted/deleted
by the most recently completed INSERT, UPDATE, or DELETE statement.
*/
inline TInt CSqlSrvDatabase::LastChangesCount()
	{
	return sqlite3_changes(iDbHandle);
	}

/**
@return Returns the ROWID of the most recent successful INSERT into the database from this database connection.
        If no successful inserts have ever occurred on this database connection, zero is returned.
*/
inline TInt64 CSqlSrvDatabase::LastInsertedRowId()
	{
	return sqlite3_last_insert_rowid(iDbHandle);
	}

//Check that the caller has at least one of {Read, Write, Schema} policies and leave with 
//KErrPermissionDenied if it is not true.
//The "schema" policy check should be evaluated last in order to prevent too many platsec warnings reported in the log files.
//(It is very likely that the caller has either "read", "write" or both database security policies)
inline void CSqlSrvDatabase::BasicSecurityPolicyCheckL(const CSqlSecurityPolicy& aSecurityPolicy)
	{
	MSqlPolicyInspector& inspector = ::SqlServer().SecurityInspector();
	if(!(inspector.Check(aSecurityPolicy.DbPolicy(RSqlSecurityPolicy::EReadPolicy)) ||
	     inspector.Check(aSecurityPolicy.DbPolicy(RSqlSecurityPolicy::EWritePolicy))||
	     inspector.Check(aSecurityPolicy.DbPolicy(RSqlSecurityPolicy::ESchemaPolicy))))
	     	{
	     	__SQLLEAVE(KErrPermissionDenied);		
	     	}
	}

/**
Current database connection transaction state.

@return True, the current database connection is in transaction, false otherwise.
*/
inline TBool CSqlSrvDatabase::InTransaction() const
	{
	return !sqlite3_get_autocommit(iDbHandle);		
	}

