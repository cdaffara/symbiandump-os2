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
// Store/Load database system settings (database settings, security policies)
// 
//

#include <f32file.h>
#include <f32file64.h>
#include <utf.h>				//CnvUtfConverter
#include <sqldb.h>				//RSqlSecurityPolicy enum items
#include "SqlSrvDbSysSettings.h"
#include "SqlUtil.h"			//Sql2OsErrCode()
#include "sqlite3.h"			//SQLITE API
#include "SqliteSymbian.h"		//sqlite3SymbianLastOsError()
#include "SqlSecurityImpl.h"	//CSqlSecurityPolicy
#include "SqlSrvStatementUtil.h"//Global SQL execution functions
#include "SqlSrvCollation.h"	//TSqlCollate
#include "SqlSrvStrings.h"		//System table names
#include "SqlSrvUtil.h"			//Global functions
#include "SqlSrvFileData.h"		//TSqlSrvFileData
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "SqlSrvDbSysSettingsTraces.h"
#endif
#include "SqlTraceDef.h"

extern TBool IsStatementSupported(const TDesC& aStatementIn, const TDesC& aDbName, TDes& aStatementOut);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////        Local const data   ///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum
	{
	ESqlSystemVersion1 = 1,	//Base version
	ESqlSystemVersion2 = 2,	//New field into "Settings" table - "Reserved" (unused, set to 0)
	ESqlSystemVersion3 = 3,	//New field into "Settings" table - "CollationDllName"
				            //Existing field in "Settings" table - "Reserved" - now used to store processed version of db config file
	ESqlSystemVersion4 = 4	//"CompactionMode" field added
	};

//Current version of the system settings - reflects changes in the system tables.
const TInt KSqlSystemVersion = ESqlSystemVersion4;

//The database names in all statements are quoted to avoid the "sql injection" threat.
//(At the moment there is no way to pass an invalid database name for these statements, because the database has to be attached
// first and a parameter binding is used there. So, the quoting is just for safety and against changes in the future)

_LIT(KCreateSettingsSql,"CREATE TABLE \"%S\".symbian_settings(Id INTEGER,Reserved INTEGER,CollationDllName TEXT, CompactionMode INTEGER);\x0");
_LIT(KDropSettingsSql, "DROP TABLE IF EXISTS \"%S\".symbian_settings;\x0");
_LIT(KInsertSettingsSql, "INSERT INTO \"%S\".symbian_settings VALUES(%d, %d,'%S',%d);\x0");
_LIT(KUpdateCollationSettingsSql, "UPDATE \"%S\".symbian_settings SET CollationDllName='%S';\x0");
_LIT(KUpdateFileVersionSettingsSql, "UPDATE \"%S\".symbian_settings SET Reserved=%d;\x0");
_LIT(KGetSettingsSql, "SELECT * FROM \"%S\".symbian_settings;\x0");
_LIT(KGetIndexSql, "SELECT name FROM \"%S\".sqlite_master WHERE type = 'index' AND sql LIKE '%%%S%%'\x0");
_LIT(KSettingsTableCheckSql, "SELECT name from \"%S\".sqlite_master WHERE type = 'table' AND name = 'symbian_settings';\x0");
_LIT(KReindexSql, "REINDEX \"%S\";\x0");

_LIT8(KCreateSecuritySql,"CREATE TABLE symbian_security(Id INTEGER PRIMARY KEY AUTOINCREMENT,ObjectType INTEGER,ObjectName TEXT,PolicyType INTEGER,PolicyData BLOB);\x0");
_LIT8(KInsertSecuritySql, "INSERT INTO symbian_security(ObjectType,ObjectName,PolicyType,PolicyData) VALUES(:V1,:V2,:V3,:V4);\x0");
_LIT8(KGetSecuritySql, "SELECT * FROM symbian_security;\x0");

_LIT8(KBeginTransactionSql, "BEGIN;\x0");
_LIT8(KCommitTransactionSql, "COMMIT;\x0");
_LIT8(KRollbackTransactionSql, "ROLLBACK;\x0");



//KInsertSecuritySql statement - parameter indices
const TInt KObjTypePrmIdx = 1;
const TInt KObjNamePrmIdx = 2;
const TInt KObjPolicyTypePrmIdx = 3;
const TInt KObjPolicyDataPrmIdx = 4;

//Default security policy - object type code and policy type code
const TInt KDefaultObjType = -2;
const TInt KDefaultPolicyType = -1;

//Database security policy - object type code
const TInt KDbObjType = -1;

//KGetSettingsSql sql statement - column indices
const TInt KSysVersionColIdx = 0;
const TInt KConfigFileVersionColIdx = 1;
const TInt KCollationDllNameColIdx = 2;
const TInt KCompactionModeColIdx = 3;

//KGetSecuritySql sql statement - column indices
const TInt KObjTypeColIdx = 1;
const TInt KObjNameColIdx = 2;
const TInt KObjPolicyTypeColIdx = 3;
const TInt KObjPolicyDataColIdx = 4;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////        Local functions   ////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Finalzes the statement handle (aHandle argument). Used in stack cleanup operations.
//Panic SqlDb 4 In _DEBUG mode if aHandle argument is NULL.
static void FinalizeStatementHandle(void* aHandle)
	{
	__ASSERT_DEBUG(aHandle != NULL, __SQLPANIC2(ESqlPanicBadArgument));
	sqlite3_stmt* stmtHandle = static_cast <sqlite3_stmt*> (aHandle);
	(void)sqlite3_finalize(stmtHandle);
	}

//Transaction rollback. Used in stack cleanup operations.
//Panic SqlDb 4 In _DEBUG mode if aDbHandle argument is NULL.
static void RollbackTransaction(void* aDbHandle)
	{
	__ASSERT_DEBUG(aDbHandle != NULL, __SQLPANIC2(ESqlPanicBadArgument));
	(void)::DbExecStmt8(reinterpret_cast <sqlite3*> (aDbHandle), KRollbackTransactionSql);
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////      TSqlDbSysSettings   ////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Initializes TSqlDbSysSettings data members with default values.

@param aDbHandle Database handle. TSqlDbSysSettings does not own aDbHandle argument.

@panic SqlDb 4 In _DEBUG mode if aDbHandle argument is NULL.
*/
TSqlDbSysSettings::TSqlDbSysSettings(sqlite3* aDbHandle) :
	iDbHandle(aDbHandle)
	{
	__ASSERT_DEBUG(iDbHandle != NULL, __SQLPANIC(ESqlPanicBadArgument));
	}

/**
Creates the database security policy table and stores the security policy in the table.

@param aSecurityPolicy   Database security policies container, which data needs to be persisted.

@leave KErrNoMemory, an out of memory condition has occurred.
                  Note that the function may also leave with some other database specific 
                  errors categorised as ESqlDbError, and other system-wide error codes.
@panic SqlDb 2 In _DEBUG mode if iDbHandle is NULL (uninitialized TSqlDbSysSettings object).
*/
void TSqlDbSysSettings::StoreSecurityPolicyL(const CSqlSecurityPolicy& aSecurityPolicyCon)
	{
	__ASSERT_DEBUG(iDbHandle != NULL, __SQLPANIC(ESqlPanicInvalidObj));
	__SQLLEAVE_IF_ERROR(::DbExecStmt8(iDbHandle, KBeginTransactionSql()));
	CleanupStack::PushL(TCleanupItem(&RollbackTransaction, iDbHandle));
	__SQLLEAVE_IF_ERROR(::DbExecStmt8(iDbHandle, KCreateSecuritySql()));
	StoreSecurityPoliciesL(aSecurityPolicyCon);
	__SQLLEAVE_IF_ERROR(::DbExecStmt8(iDbHandle, KCommitTransactionSql()));
	CleanupStack::Pop();//TCleanupItem(&RollbackTransaction, iDbHandle)
	}

/**
Stores the database system settings in the settings table. The settings table is recreated.

@param aDbName Logical database name: "main" for the main database or attached database name,
@param aCollationDllName Collation dll name. It uniquely identifies the current collation method in use.
                         If the default collation method changes later then the database will be reindexed 
                         and the new collation dll name will replace the existing one in the settings table.
@param aDbConfigFileVersion Current config file version or KSqlNullDbConfigFileVersion
@param aCompactionMode Database compaction mode, one of TSqlCompactionMode enum item values (except ESqlCompactionNotSet)

@see TSqlCompactionMode

@leave KErrNoMemory, an out of memory condition has occurred.
                  Note that the function may also leave with some other database specific 
                  errors categorised as ESqlDbError, and other system-wide error codes.
@panic SqlDb 2 In _DEBUG mode if iDbHandle is NULL (uninitialized TSqlDbSysSettings object).
@panic SqlDb 4 In _DEBUG mode if aCompactionMode parameter value is invalid.
*/
void TSqlDbSysSettings::StoreSettingsL(const TDesC& aDbName, const TDesC& aCollationDllName, TInt aDbConfigFileVersion, TSqlCompactionMode aCompactionMode)
	{
	__ASSERT_DEBUG(aCompactionMode == ESqlCompactionManual || aCompactionMode == ESqlCompactionBackground || aCompactionMode == ESqlCompactionAuto, __SQLPANIC(ESqlPanicBadArgument));
	__ASSERT_DEBUG(iDbHandle != NULL, __SQLPANIC(ESqlPanicInvalidObj));
	HBufC* buf = HBufC::NewLC(Max((TInt)sizeof(KDropSettingsSql), 
							  Max((TInt)sizeof(KCreateSettingsSql), (TInt)sizeof(KInsertSettingsSql))) + 
							  aDbName.Length() + aCollationDllName.Length() + 10);
	TPtr sql = buf->Des();
	//Begin transaction
	__SQLLEAVE_IF_ERROR(::DbExecStmt8(iDbHandle, KBeginTransactionSql()));
	CleanupStack::PushL(TCleanupItem(&RollbackTransaction, iDbHandle));	
	//Recreate the "settings" table and store the default collation there (the DLL name)
	sql.Format(KDropSettingsSql(), &aDbName);
	__SQLLEAVE_IF_ERROR(::DbExecStmt16(iDbHandle, sql));
	sql.Format(KCreateSettingsSql, &aDbName);
	__SQLLEAVE_IF_ERROR(::DbExecStmt16(iDbHandle, sql));
	sql.Format(KInsertSettingsSql(), &aDbName, KSqlSystemVersion, aDbConfigFileVersion, &aCollationDllName, aCompactionMode);
	__SQLLEAVE_IF_ERROR(::DbExecStmt16(iDbHandle, sql));
	//Commit transaction
	__SQLLEAVE_IF_ERROR(::DbExecStmt8(iDbHandle, KCommitTransactionSql()));
	CleanupStack::Pop();//TCleanupItem(&RollbackTransaction, iDbHandle)
	CleanupStack::PopAndDestroy(buf);
	}

/**
Read security policies from security policy table.

The method does not guarantee that either the security policies will be read from the 
security table and stored in aSecurityPolicy parameter or aSecurityPolicy argument 
stays unchanged in a case of failure.

@param aSecurityPolicyCon Security policies container which needs to be initializeed with 
						  the database security policies.

@leave KErrNoMemory, an out of memory condition has occurred;
	   KErrGeneral, missing or invalid data in the system tables;
                  Note that the function may also leave with some other database specific 
                  errors categorised as ESqlDbError, and other system-wide error codes.
@panic SqlDb 2 In _DEBUG mode if iDbHandle is NULL (uninitialized TSqlDbSysSettings object).
*/
void TSqlDbSysSettings::LoadSecurityPolicyL(CSqlSecurityPolicy& aSecurityPolicyCon)
	{
	__ASSERT_DEBUG(iDbHandle != NULL, __SQLPANIC(ESqlPanicInvalidObj));
	//Even if the version of the system settings is bigger than the current one (KSqlSystemVersion constant),
	//I think that all future modifications of the system tables shall not affect the already existing
	//fields. So it is correct to think that all information available in version 1 should be available 
	//(and accessible) in all future versions of the system settings.
	//Note: no attempt shall be made here to modify the system tables structure! There may be more than one 
	//      connection to the database being processed!
	//
	//Prepare statement handle
	sqlite3_stmt* stmtHandle = ::StmtPrepare8L(iDbHandle, KGetSecuritySql());
	CleanupStack::PushL(TCleanupItem(&FinalizeStatementHandle, stmtHandle));
	//Read the security policies
	TBool defaultPolicySet = EFalse;
	TInt dbPolicySetFlag = 0;
	TInt err;
	while((err = ::StmtNext(stmtHandle)) == KSqlAtRow)
		{
		TInt objType = KDefaultObjType - 1;
		TPtrC objName(KNullDesC);
		TInt policyType = RSqlSecurityPolicy::ESchemaPolicy - 1;
		TSecurityPolicy policy = ReadCurrSecurityPolicyL(stmtHandle, objType, objName, policyType);
		switch(objType)
			{
			case KDefaultObjType:
				if(defaultPolicySet)
					{
					__SQLLEAVE(KErrGeneral);//two "default policy" records in the table
					}
				StoreDefaultSecurityPolicy(aSecurityPolicyCon, policy, dbPolicySetFlag);
				defaultPolicySet = ETrue;
				break;
			case KDbObjType:
				StoreDbSecurityPolicyL(aSecurityPolicyCon, policyType, policy, dbPolicySetFlag);
				break;
			case RSqlSecurityPolicy::ETable:
				StoreDbObjSecurityPolicyL(aSecurityPolicyCon, objType, objName, policyType, policy);
				break;
			default:
				__SQLLEAVE(KErrGeneral);
				break;
			}//end of "switch(aObjType)"
		}
	CleanupStack::PopAndDestroy();//cleanupItem (statement handle)
	__SQLLEAVE_IF_ERROR(err);
	if(!defaultPolicySet)
		{
		__SQLLEAVE(KErrGeneral);//no default policy
		}
	}
	
/**
Loads the database system settings from the settings table.
If the settings table does not exist then it is created with the default settings
(the stored collation dll name will be empty and the stored database configuration 
file version will be 0).

@param aDbName Logical database name: "main" for the main database or attached database name,
@param aCollationDllName Output parameter, will contain the stored collation dll name,
@param aDbConfigFileVersion Output parameter, will contain the stored database config file version.
@param aCompactionMode Output parameter. Database compaction mode (one of TSqlCompactionMode enum item values except ESqlCompactionNotSet).
					   Note that the input value of this parameter might be ESqlCompactionManual if the this is a legacy database,
					   created not by the SQL server.

@see TSqlCompactionMode

@leave KErrNoMemory, an out of memory condition has occurred.
       Note that the function may also leave with some other database specific 
       errors categorised as ESqlDbError, and other system-wide error codes..
@panic SqlDb 2 In _DEBUG mode if iDbHandle is NULL (uninitialized TSqlDbSysSettings object).
@panic SqlDb 7 In _DEBUG mode if the stored compaction mode is invalid.
*/	
void TSqlDbSysSettings::LoadSettingsL(const TDesC& aDbName, TDes& aCollationDllName, TInt& aDbConfigFileVersion, TSqlCompactionMode& aCompactionMode)
	{
	__ASSERT_DEBUG(iDbHandle != NULL, __SQLPANIC(ESqlPanicInvalidObj));

	aCollationDllName.Zero();
	aDbConfigFileVersion = KSqlNullDbConfigFileVersion;	
	if(aCompactionMode == ESqlCompactionNotSet)
		{
		aCompactionMode = KSqlDefaultCompactionMode;
		}
		
	//If the system settings table does not exist then create it now.
	//For a database created by the SQL Server this will only occur 
	//when the database is being created in an application's private data cage -
	//as part of this create call it is now being opened by the server.
	//An externally created database is likely to not contain the settings table 
	//and adding it here makes the database 'SQL Server-compatible'
	if(!SettingsTableExistsL(aDbName))
		{
		StoreSettingsL(aDbName, aCollationDllName, KSqlNullDbConfigFileVersion, aCompactionMode); // store empty collation dll name, then reindexing will occur
		}
	else
		{
		//Get the settings from the existing table
		TInt settingsVersion = 0;
		GetSettingsL(aDbName, aCollationDllName, aDbConfigFileVersion, settingsVersion, aCompactionMode);
		if(settingsVersion < KSqlSystemVersion)
			{
			//Recreate the settings table using the last version number format (this is what the old code did during reindexing)
			StoreSettingsL(aDbName, aCollationDllName, aDbConfigFileVersion, aCompactionMode); // store empty collation dll name, then reindexing will occur
			}
		}
	__ASSERT_DEBUG(aCompactionMode == ESqlCompactionManual || aCompactionMode == ESqlCompactionBackground || aCompactionMode == ESqlCompactionAuto, __SQLPANIC(ESqlPanicInternalError));
	}
	
/**
Retrieves the database system settings from the settings table.

@param aDbName Logical database name: "main" for the main database or attached database name,
@param aCollationDllName Output parameter, will contain the stored collation dll name,
@param aDbConfigFileVersion Output parameter, will contain the stored database config file version,
@param aSettingsVersion Output parameter, will contain the version of the settings table.
@param aCompactionMode Output parameter. Database compaction mode (one of TSqlCompactionMode enum item values except ESqlCompactionNotSet).

@see TSqlCompactionMode

@leave KErrGeneral, either unable to retrieve the data from the settings table or the 
					stored table version or config file version is invalid or the stored compaction mode is invalid.
	   KErrOverflow, aCollationDllName is not large enough to store the name of the 
	   				 collation dll that is stored in the settings table.
       KErrNoMemory, an out of memory condition has occurred.
	   Note that the function may also leave with other system-wide error codes or SQL
	   errors of ESqlDbError type
@panic SqlDb 2 In _DEBUG mode if iDbHandle is NULL (uninitialized TSqlDbSysSettings object).
*/
void TSqlDbSysSettings::GetSettingsL(const TDesC& aDbName, TDes& aCollationDllName, TInt& aDbConfigFileVersion, 
									 TInt& aSettingsVersion, TSqlCompactionMode& aCompactionMode)
	{
	__ASSERT_DEBUG(iDbHandle != NULL, __SQLPANIC(ESqlPanicInvalidObj));

	HBufC* buf = HBufC::NewLC(sizeof(KGetSettingsSql) + aDbName.Length());
	TPtr sql = buf->Des();
			
	//Prepare statement handle
	sql.Format(KGetSettingsSql(), &aDbName);
	sqlite3_stmt* stmtHandle = ::StmtPrepare16L(iDbHandle, sql);
	CleanupStack::PushL(TCleanupItem(&FinalizeStatementHandle, stmtHandle));
	//Move to the first record
	TInt err = ::StmtNext(stmtHandle);
	__SQLLEAVE_IF_ERROR(err);
	//Check that it is a valid row. The error is checked on the previous line. 
	//The "if" bellow will check whether there is a valid record or not.
	if(err != KSqlAtRow)
		{
		__SQLLEAVE(KErrGeneral);
		}
	//Get the system settings version 
	aSettingsVersion = sqlite3_column_int(stmtHandle, KSysVersionColIdx);
	if(aSettingsVersion < ESqlSystemVersion1)
		{
		__SQLLEAVE(KErrGeneral);
		}
	if(aSettingsVersion > ESqlSystemVersion2)
		{
		//The "ConfigFileVersion" column exists and is used to store the most recent
		//version of the database's config file (if there is one) that has 
		//been successfully processed
		aDbConfigFileVersion = sqlite3_column_int(stmtHandle, KConfigFileVersionColIdx);
		if(aDbConfigFileVersion < KSqlNullDbConfigFileVersion)
			{
			__SQLLEAVE(KErrGeneral);
			}
			
		//The "CollationDllName" column exists and its value can be read.
        //The column type might be different than SQLITE_TEXT - malformed database.
        if(sqlite3_column_type(stmtHandle, KCollationDllNameColIdx) != SQLITE_TEXT)
            {
            __SQLLEAVE(KErrGeneral);   
            }
		const void* ptr = sqlite3_column_text16(stmtHandle, KCollationDllNameColIdx);
        //Null column value - this might be an indication of an "out of memory" problem, if the column text  
        //is in UTF8 format. (sqlite3_column_text16() may allocate memory for UTF8->UTF16 conversion)
		__SQLLEAVE_IF_NULL(const_cast<void*>(ptr));
        TPtrC16 src(reinterpret_cast <const TUint16*> (ptr));
        if(src.Length() > aCollationDllName.MaxLength())
            {
            __SQLLEAVE(KErrOverflow);	
            }
        aCollationDllName.Copy(src);
		}
	if(aSettingsVersion > ESqlSystemVersion3)
		{
		aCompactionMode = static_cast <TSqlCompactionMode> (sqlite3_column_int(stmtHandle, KCompactionModeColIdx));
		if(aCompactionMode != ESqlCompactionManual && aCompactionMode != ESqlCompactionBackground && aCompactionMode != ESqlCompactionAuto)
			{
			__SQLLEAVE(KErrGeneral);
			}
		}
	CleanupStack::PopAndDestroy();//TCleanupItem(&FinalizeStatementHandle, stmtHandle)	
	CleanupStack::PopAndDestroy(buf);
	}
	
/**
Deletes and recreates all database indexes which use user-defined collation methods, from scratch.
Also updates the database settings table with the name of the current collation dll.
This is all performed as a single atomic transaction.

@param aDbName Logical database name: "main" for the main database or attached database name,
@param aCurrentCollationDllName The name of the current collation dll.

@leave The function may leave with system-wide error codes or SQL errors of ESqlDbError type

@panic SqlDb 2 In _DEBUG mode if iDbHandle is NULL (uninitialized TSqlDbSysSettings object).
*/
void TSqlDbSysSettings::ReindexDatabaseL(const TDesC& aDbName, const TDesC& aCurrentCollationDllName)
	{	
	__ASSERT_DEBUG(iDbHandle != NULL, __SQLPANIC(ESqlPanicInvalidObj));

	//Allocate memory for the SQL statements
	HBufC* buf = HBufC::NewLC(Max((TInt)sizeof(KUpdateCollationSettingsSql), (TInt)sizeof(KReindexSql)) + 
							  aDbName.Length() + aCurrentCollationDllName.Length());
	TPtr sql = buf->Des();
	
	//Begin transaction
	__SQLLEAVE_IF_ERROR(::DbExecStmt8(iDbHandle, KBeginTransactionSql()));
	CleanupStack::PushL(TCleanupItem(&RollbackTransaction, iDbHandle));	
	
	const TSqlCollationUtil collationUtil(iDbHandle);
	//Issue a "REINDEX" command for each collation that has at least one index using it
	for(TInt i = collationUtil.CollationCount()-1; i >= 0 ;--i)
		{
		TPtrC collationName = collationUtil.CollationName(i);
		if(IndexExistsL(aDbName, collationName))
			{
			sql.Format(KReindexSql(), &collationName);
			__SQLLEAVE_IF_ERROR(::DbExecStmt16(iDbHandle, sql));
			}
		}
		
	//Update the settings table to store the current collation DLL name
	sql.Format(KUpdateCollationSettingsSql(), &aDbName, &aCurrentCollationDllName);
	__SQLLEAVE_IF_ERROR(::DbExecStmt16(iDbHandle, sql));

	//Commit transaction
	__SQLLEAVE_IF_ERROR(::DbExecStmt8(iDbHandle, KCommitTransactionSql()));
	CleanupStack::Pop(); //TCleanupItem(&RollbackTransaction, iDbHandle)
	CleanupStack::PopAndDestroy(buf);
	}

/**
Performs any necessary configuration file updates to the database if a 
configuration file exists for the database that has not yet been processed.
If a configuration file is processed then the database settings table will 
be updated with the version of the configuration file that was processed.

@param aStoredDbConfigFileVersion The configuration file version that is
								  currently stored in the settings table
@param aFileData The database file data

@param aDbName Logical database name: "main" for the main database or attached database name

@leave One of the system-wide error codes or SQL errors of ESqlDbError type

@panic SqlDb 2 In _DEBUG mode if iDbHandle is NULL (uninitialized TSqlDbSysSettings object)
*/
void TSqlDbSysSettings::ConfigureDatabaseL(TInt aStoredDbConfigFileVersion, const TSqlSrvFileData& aFileData, 
										   const TDesC& aDbName)
	{	
	SQL_TRACE_INTERNALS(OstTrace1(TRACE_INTERNALS, TSQLDBSYSSETTINGS_CONFIGUREDATABASEL_ENTRY, "Entry;0x%X;TSqlDbSysSettings::ConfigureDatabaseL", (TUint)this));    
	
	__ASSERT_DEBUG(iDbHandle != NULL, __SQLPANIC(ESqlPanicInvalidObj));

	if(!aFileData.IsSecureFileNameFmt())
		{
		//As a first implementation, config files will only be supported for 
		//shared, secure databases - not for private, secure databases or public databases
		SQL_TRACE_INTERNALS(OstTrace1(TRACE_INTERNALS, TSQLDBSYSSETTINGS_CONFIGUREDATABASEL_EXIT1, "Exit;0x%X;TSqlDbSysSettings::ConfigureDatabaseL;Not a secure db", (TUint)this));    
		return;	
		}
		
	//Determine whether there is a configuration file 
	//for this database that has not yet been processed
	const CDbConfigFiles* dbConfigFiles = aFileData.DbConfigFiles();
	if(dbConfigFiles)
		{
		//Get the database file name and extension to search for a corresponding config file
		TPtrC dbName = aFileData.FileName();
		TParse parse;
		__SQLLEAVE_IF_ERROR(parse.Set(dbName, NULL, NULL));
		TPtrC dbFileNameAndExt = parse.NameAndExt();
		HBufC* matchingConfigFile = dbConfigFiles->FindConfigFile(dbFileNameAndExt);
		if(matchingConfigFile)
			{
			//There exists a config file for this database - now check whether its
			//version is greater than the version stored in the settings table and 
			//only process the file if it is
			TFileName configFileName;
			configFileName.Copy(matchingConfigFile->Des());
			TInt offset = configFileName.LocateReverse('.') ;
			if(KErrNotFound != offset)
				{
				TInt lengthOfExt = configFileName.Length() - offset - 1;
				TPtrC versionNum = configFileName.Right(lengthOfExt);				
				TLex lex(versionNum);
				TInt fileVersion = 0;
				if(KErrNone == lex.Val(fileVersion))
					{
					if(fileVersion > aStoredDbConfigFileVersion)
						{
						//The latest version of the configuration file has not yet been processed, so do it now
						SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, TSQLDBSYSSETTINGS_CONFIGUREDATABASEL1, "0x%X;TSqlDbSysSettings::ConfigureDatabaseL;Config file '%S'", (TUint)this, __SQLPRNSTR(configFileName)));	
						ExecuteConfigurationUpdateL(aFileData, configFileName, fileVersion, aDbName);
						SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, TSQLDBSYSSETTINGS_CONFIGUREDATABASEL2, "0x%X;TSqlDbSysSettings::ConfigureDatabaseL;Config file '%S' was processed, no errors", (TUint)this, __SQLPRNSTR(configFileName)));	
						}
					else
						{
						SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, TSQLDBSYSSETTINGS_CONFIGUREDATABASEL3, "0x%X;TSqlDbSysSettings::ConfigureDatabaseL;Config file '%S' as this or a later version has already been processed", (TUint)this, __SQLPRNSTR(configFileName)));	
						}
					}
				else
					{
					//Invalid config file name extension - it cannot be converted to an integer
					__SQLLEAVE(KErrCorrupt);
					}
				}
			else
				{
				//Invalid config file name - it doesn't contain the character '.'
				__SQLLEAVE(KErrCorrupt);
				}
			}
		else
			{
			SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, TSQLDBSYSSETTINGS_CONFIGUREDATABASEL4, "0x%X;TSqlDbSysSettings::ConfigureDatabaseL;No config file found for database '%S'", (TUint)this, __SQLPRNSTR(dbFileNameAndExt)));	
			}
		}		
	SQL_TRACE_INTERNALS(OstTrace1(TRACE_INTERNALS, TSQLDBSYSSETTINGS_CONFIGUREDATABASEL_EXIT2, "Exit;0x%X;TSqlDbSysSettings::ConfigureDatabaseL", (TUint)this));    
	}
	
/**
Executes the supported operations specified in the given database 
configuration file and updates the settings table to store the 
version of this configuration file.

@param aFileData The database file data
@param aMatchingConfigFile The configuration file that is to be processed
@param aDbConfigFileVersion The configuration file version
@param aDbName Logical database name: "main" for the main database or attached database name

@leave KErrNoMemory, if an out of memory condition occurs.
       One of the other system-wide error codes if the configuration
       file fails to be opened or read.
       One of the SQL errors of ESqlDbError type if the update to the
       database settings table fails

@panic SqlDb 2 In _DEBUG mode if iDbHandle is NULL (uninitialized TSqlDbSysSettings object).
*/
void TSqlDbSysSettings::ExecuteConfigurationUpdateL(const TSqlSrvFileData& aFileData, 
													const TDesC& aMatchingConfigFile,
													TInt aDbConfigFileVersion,
													const TDesC& aDbName)
	{
	__ASSERT_DEBUG(iDbHandle != NULL, __SQLPANIC(ESqlPanicInvalidObj));
														
	//Execute the specified database config file operations that are supported
#ifdef SYSLIBS_TEST
	TDriveUnit drive = EDriveC;
#else
	TDriveUnit drive = EDriveZ;
#endif			
	TFileName configFilePath;
	TDriveName drvName = drive.Name();
	configFilePath.Append(drvName);
	configFilePath.Append(aFileData.PrivatePath());
	configFilePath.Append(aMatchingConfigFile);
	//If this method leaves then either the config file could not be 
	//opened or read or an out of memory condition occured. Either way
	//another attempt will be made to process the config file when the
	//database is next opened
	DoExecuteDbConfigFileOpsL(aFileData.Fs(), configFilePath, aDbName);
												
	//Now update the settings table to store the current version of the database config file.
	//If this fails then another attempt will be made to process the config file and update
	//the settings table when the database is next opened
	TBuf<sizeof(KUpdateFileVersionSettingsSql) + KMaxFileName + 10> buf;
	buf.Format(KUpdateFileVersionSettingsSql(), &aDbName, aDbConfigFileVersion);
	__SQLLEAVE_IF_ERROR(::DbExecStmt16(iDbHandle, buf));
	}

/**
Opens the specified database configuration file and executes those operations that 
are specified in it which are currently supported (currently only 'CREATE INDEX' 
operations are supported and will be executed).

@param aFs The server's file server session
@param aConfigFilePath The configuration file path
@param aDbName Logical database name: "main" for the main database or attached database name

@leave KErrNoMemory, if an out of memory condition occurs.
       One of the other system-wide error codes if the configuration
       file fails to be opened or read

@panic SqlDb 2 In _DEBUG mode if iDbHandle is NULL (uninitialized TSqlDbSysSettings object).
*/
void TSqlDbSysSettings::DoExecuteDbConfigFileOpsL(RFs& aFs, const TDesC& aConfigFilePath, const TDesC& aDbName)
	{
	__ASSERT_DEBUG(iDbHandle != NULL, __SQLPANIC(ESqlPanicInvalidObj));
	
	//Open the config file and read it into a buffer
	RFile64 file;
	__SQLLEAVE_IF_ERROR(file.Open(aFs, aConfigFilePath, EFileRead)); 
	CleanupClosePushL(file);
	TInt64 size = 0;
	__SQLLEAVE_IF_ERROR(file.Size(size));
	if(size == 0)
		{
		//Config file is empty so just return
		SQL_TRACE_INTERNALS(OstTraceExt1(TRACE_INTERNALS, TSQLDBSYSSETTINGS_DOEXECUTEDBCONFIGFILEOPSL, "0;TSqlDbSysSettings::DoExecuteDbConfigFileOpsL();Config file %S is empty", __SQLPRNSTR(aConfigFilePath)));	
		CleanupStack::PopAndDestroy(); // file
		return;
		}
	HBufC8* buffer = HBufC8::NewLC(size);
	TPtr8 bufPtr = buffer->Des();
	__SQLLEAVE_IF_ERROR(file.Read(bufPtr));

	//Convert buffer to Unicode for processing (the configuration file is expected to be ASCII or UTF-8)
	HBufC16* buf16 = CnvUtfConverter::ConvertToUnicodeFromUtf8L(bufPtr);
	CleanupStack::PushL(buf16);			
	TPtrC16 ptr16 = buf16->Des();
	
	//Skip the BOM (byte ordering mark) at the start if there is one
	if((ptr16.Locate(TChar(0xFEFF)) == 0) || (ptr16.Locate(TChar(0xFFFE)) == 0))
		{
		ptr16.Set(ptr16.Mid(1));	
		}
	 
	//Parse the file contents and execute the specified
	//config statements that are supported
	ParseFileL(ptr16, aDbName);

	CleanupStack::PopAndDestroy(3); // buf16, buffer, file
	}
	
/**
Parses the configuration file buffer, reading each SQL statement
and processing it.

@param aBuffer Buffer containing the configuration file contents
@param aDbName Logical database name: "main" for the main database or attached database name

@leave KErrNoMemory, if an out of memory condition occurs
*/
void TSqlDbSysSettings::ParseFileL(const TDesC& aBuffer, const TDesC& aDbName)
	{
	TLex fileParser(aBuffer);
	//While not end of file
	while (!fileParser.Eos())
		{
		//Get the next of the SQL statements, which are seperated by semicolons
		fileParser.SkipSpaceAndMark(); // skip any preceding whitespace before the next statement
		while ((!fileParser.Eos()) && (fileParser.Peek() != ';'))
			{
			fileParser.Inc(); // continue to next character
			}
		TPtrC stmt = fileParser.MarkedToken(); // extract the marked token
		fileParser.Inc();  // to skip the terminating ';' for next iteration
	
		//Process this statement
		ProcessStatementL(stmt, aDbName);
		}
	}

/**
Process an SQL statement contained in the configuration file
and executes it if it is a supported statement.
Currently only 'CREATE INDEX' statements are supported.
The statement can contain comments that are understood by SQLite
- these can be in the form of an SQL comment or a 'C' comment, 
as defined at http://www.sqlite.org/lang_comment.html.

@param aStmt An SQL statement string, stripped of any whitespace 
			 at the beginning of it
@param aDbName Logical database name: "main" for the main database or attached database name

@leave KErrNoMemory, if an out of memory condition occurs
*/
void TSqlDbSysSettings::ProcessStatementL(const TDesC& aStmt, const TDesC& aDbName)
	{	
	SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, TSQLDBSYSSETTINGS_PROCESSSTATEMENTL_ENTRY, "Entry;0x%X;TSqlDbSysSettings::ProcessStatementL;Processing statement '%S'", (TUint)this, __SQLPRNSTR(aStmt)));	
	
	//If the statement only contained whitespace then just return
	if(aStmt.Length() == 0)
		{
		SQL_TRACE_INTERNALS(OstTrace1(TRACE_INTERNALS, TSQLDBSYSSETTINGS_PROCESSSTATEMENTL_EXIT1, "Exit;0x%X;TSqlDbSysSettings::ProcessStatementL;The statement ignored because contains only whitespace", (TUint)this));	
		return;	
		}
		
	//Check that the statement does not contain an unsupported comment style
	if(KErrNotFound != aStmt.Find(_L("//")))
		{
		//The statement contains '//' which is an unsupported comment style, but rather
		//than leave here and cause the full file to fail, we just ignore this statement
		SQL_TRACE_INTERNALS(OstTrace1(TRACE_INTERNALS, TSQLDBSYSSETTINGS_PROCESSSTATEMENTL_EXIT2, "Exit;0x%X;TSqlDbSysSettings::ProcessStatementL;The statement ignored because contains invalid comment style", (TUint)this));	
		return;
		}
		
	HBufC* stmtBuf = HBufC::NewLC(aStmt.Length() + 1 + aDbName.Length() + 1);
	TPtr stmtPtr = stmtBuf->Des();
	//If the 'aStmt' string is an SQL statement that is supported then execute it
	if(::IsStatementSupported(aStmt, aDbName, stmtPtr))
		{
		TInt err = ::DbExecStmt16(iDbHandle, stmtPtr);
		if(KErrNone == err)
			{
			SQL_TRACE_INTERNALS(OstTrace1(TRACE_INTERNALS, TSQLDBSYSSETTINGS_PROCESSSTATEMENTL1, "0x%X;TSqlDbSysSettings::ProcessStatementL;Successfully executed statement", (TUint)this));		
			}
		else
			{
			SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, TSQLDBSYSSETTINGS_PROCESSSTATEMENTL2, "0x%X;TSqlDbSysSettings::ProcessStatementL;Failed to execute the statement;err=%d", (TUint)this, err));		
			if(err == KErrNoMemory)
				{
				__SQLLEAVE(err);	
				}
			}
		}
	else
		{
		SQL_TRACE_INTERNALS(OstTrace1(TRACE_INTERNALS, TSQLDBSYSSETTINGS_PROCESSSTATEMENTL3, "0x%X;TSqlDbSysSettings::ProcessStatementL;Non-supported statement, will be ignored", (TUint)this));		
		}
	CleanupStack::PopAndDestroy(); // stmtBuf
	SQL_TRACE_INTERNALS(OstTrace1(TRACE_INTERNALS, TSQLDBSYSSETTINGS_PROCESSSTATEMENTL_EXIT3, "Exit;0x%X;TSqlDbSysSettings::ProcessStatementL", (TUint)this));  
	}


/**
Stores the security policies into the security policies table.

@param aSecurityPolicyCon Security policies container, which data needs to be persisted.

@leave KErrNoMemory, an out of memory condition has occurred;
	   KErrArgument, system table name found in the list of the tables, which need special protection;
                  	 Note that the function may also leave with some other database specific 
                  	 errors categorised as ESqlDbError, and other system-wide error codes.

@panic SqlDb 4 In _DEBUG mode if aSecurityPolicy argument is NULL.
*/
void TSqlDbSysSettings::StoreSecurityPoliciesL(const CSqlSecurityPolicy& aSecurityPolicyCon)
	{
	//Prepare the INSERT sql statement
	sqlite3_stmt* stmtHandle = ::StmtPrepare8L(iDbHandle, KInsertSecuritySql());
	CleanupStack::PushL(TCleanupItem(&FinalizeStatementHandle, stmtHandle));
	//Store default security policy into the table
	StoreSecurityPolicyL(stmtHandle, KDefaultObjType, KNullDesC, KDefaultPolicyType, aSecurityPolicyCon.DefaultPolicy());
	//Store database security policies into the table
	StoreSecurityPolicyL(stmtHandle, KDbObjType, KNullDesC, RSqlSecurityPolicy::ESchemaPolicy, aSecurityPolicyCon.DbPolicy(RSqlSecurityPolicy::ESchemaPolicy));
	StoreSecurityPolicyL(stmtHandle, KDbObjType, KNullDesC, RSqlSecurityPolicy::EReadPolicy, aSecurityPolicyCon.DbPolicy(RSqlSecurityPolicy::EReadPolicy));
	StoreSecurityPolicyL(stmtHandle, KDbObjType, KNullDesC, RSqlSecurityPolicy::EWritePolicy, aSecurityPolicyCon.DbPolicy(RSqlSecurityPolicy::EWritePolicy));
	//Store database objects security policies into the table
	TSqlSecurityPolicyIterator it(aSecurityPolicyCon);
	RSqlSecurityPolicy::TObjectType objectType;
	TPtrC objectName;
	RSqlSecurityPolicy::TPolicyType policyType;
	TSecurityPolicy policy;
	while(it.Next(objectType, objectName, policyType, policy))
		{
		if(objectType == RSqlSecurityPolicy::ETable && ::IsSystemTableName(objectName))
	   		{
	   		//Clients are not allowed to put system table names in the RSqlSecurityPolicy container.
			__SQLLEAVE(KErrArgument);		   			
	   		}
		StoreSecurityPolicyL(stmtHandle, objectType, objectName, policyType, policy);
		}
	CleanupStack::PopAndDestroy();//cleanupItem (statement handle)
	}

/**
Stores a security policy object in the security policies table.

@param aStmtHandle Statement handle.
@param aObjType Database object type: default security policies, database security policies, table security policies
@param aObjName Database object name. It is expected to be a non-empty string only for tables.
@param aPolicyType Security policy type: schema, write, read.
@param aPolicy Security policy object.

@leave KErrNoMemory, an out of memory condition has occurred;
                  	 Note that the function may also leave with some other database specific 
                  	 errors categorised as ESqlDbError, and other system-wide error codes.

@panic SqlDb 4 In _DEBUG mode if aHandle argument is NULL.
*/
void TSqlDbSysSettings::StoreSecurityPolicyL(sqlite3_stmt* aStmtHandle, TInt aObjType, const TDesC& aObjName, 
											 TInt aPolicyType, const TSecurityPolicy& aPolicy)
	{
	__ASSERT_DEBUG(aStmtHandle != NULL, __SQLPANIC(ESqlPanicBadArgument));
	__SQLLEAVE_IF_ERROR(::StmtReset(aStmtHandle));
	__SQLLEAVE_IF_ERROR(BindSecurityPolicyPrm(aStmtHandle, aObjType, aObjName, aPolicyType, aPolicy));
	__SQLLEAVE_IF_ERROR(::StmtExec(aStmtHandle));
	}

/**
Binds the parameter values for the insert SQL statement for security policies table.

@param aStmtHandle Statement handle.
@param aObjType Database object type: default security policy, database security policies, table security policies
@param aObjName Database object name. It is expected to be a non-empty string only for tables.
@param aPolicyType Security policy type: schema, write, read.
@param aPolicy Security policy object.

@see TSqlDbSysSettings::StoreSecurityPolicyL()

@panic SqlDb 4 In _DEBUG mode if aHandle argument is NULL.
*/
TInt TSqlDbSysSettings::BindSecurityPolicyPrm(sqlite3_stmt* aStmtHandle, TInt aObjType, const TDesC& aObjName, 
												  TInt aPolicyType, const TSecurityPolicy& aPolicy)
	{
	(void)sqlite3SymbianLastOsError();//clear last OS error
	TInt err = sqlite3_bind_int(aStmtHandle, KObjTypePrmIdx, aObjType);
	if(err == SQLITE_OK)
		{
		err = sqlite3_bind_text16(aStmtHandle, KObjNamePrmIdx, aObjName.Ptr(), aObjName.Length() * sizeof(TText), SQLITE_STATIC);
		if(err == SQLITE_OK)
			{
			err = sqlite3_bind_int(aStmtHandle, KObjPolicyTypePrmIdx, aPolicyType);
			if(err == SQLITE_OK)
				{
				TPtrC8 policyData(aPolicy.Package());
				err = sqlite3_bind_blob(aStmtHandle, KObjPolicyDataPrmIdx, policyData.Ptr(), policyData.Length(), SQLITE_STATIC);
				}
			}
		}
	return ::Sql2OsErrCode(err, sqlite3SymbianLastOsError());
	}

/**
Reads a record from security policies table.

@param aStmtHandle Statement handle. It cannot be NULL.
@param aSecurityPolicy Security policies container.
@param aObjType Output parameter, will be initialized with the database object type: KDefaultObjType,
				KDbObjType, RSqlSecurityPolicy::ETable.
@param aObjName Output parameter, database object name (for example, table name), which is protected by the
				current security policy.
@param aPolicyType Output parameter, will be initialized with the database policy type: RSqlSecurityPolicy::EReadPolicy,
				   RSqlSecurityPolicy::EWritePolicy, RSqlSecurityPolicy::ESchemaPolicy.
@return The created security policy object.

@leave KErrGeneral, invalid security policy data;
	   KErrNoMemory, Out of memory.
*/
TSecurityPolicy TSqlDbSysSettings::ReadCurrSecurityPolicyL(sqlite3_stmt* aStmtHandle, TInt& aObjType, 
														   TPtrC& aObjName, TInt& aPolicyType)
	{
	__ASSERT_DEBUG(aStmtHandle != NULL, __SQLPANIC(ESqlPanicBadArgument));
	aObjType = sqlite3_column_int(aStmtHandle, KObjTypeColIdx);
    //The "ObjectName" column type might be different than SQLITE_TEXT - malformed database.
    if(sqlite3_column_type(aStmtHandle, KObjNameColIdx) != SQLITE_TEXT)
        {
        __SQLLEAVE(KErrGeneral);   
        }
    const void* text = sqlite3_column_text16(aStmtHandle, KObjNameColIdx);
    //Null column value - this might be an indication of an "out of memory" problem, if the column text  
    //is in UTF8 format. (sqlite3_column_text16() may allocate memory for UTF8->UTF16 conversion)
    __SQLLEAVE_IF_NULL(const_cast<void*>(text));
	TInt len = (TUint)sqlite3_column_bytes16(aStmtHandle, KObjNameColIdx) / sizeof(TUint16);
	aObjName.Set(reinterpret_cast <const TUint16*> (text), len);
	aPolicyType = sqlite3_column_int(aStmtHandle, KObjPolicyTypeColIdx);
	len = sqlite3_column_bytes(aStmtHandle, KObjPolicyDataColIdx);
	if(len != sizeof(TSecurityPolicy))
		{
		//Check if the error is "out of memory" (which may happen when retrieving text column data
		//and the column encoding is different, in which case  the column text has to be converted 
		//and a new block of memory has to be allocated for the conversion).
		TInt err2 = ::StmtReset(aStmtHandle);
		__SQLLEAVE(err2 == KErrNoMemory ? KErrNoMemory : KErrGeneral);
		}
	const void* data = sqlite3_column_blob(aStmtHandle, KObjPolicyDataColIdx);
	TSecurityPolicy policy;
	policy.Set(TPtrC8(reinterpret_cast <const TUint8*> (data), len));
	return policy;
	}

/**
Stores the default security policy into aSecurityPolicyCon container.
Initialises all database security policies not set yet with the default security policy.

@param aSecurityPolicyCon Security policies container.
@param aPolicy Default security policy object
@param aDbPolicySetFlag Bit flag. Keeps information which database security policies are set and which aren't.

@see CSqlSecurityPolicy::SetDbPolicy()
*/
void TSqlDbSysSettings::StoreDefaultSecurityPolicy(CSqlSecurityPolicy& aSecurityPolicyCon, 
												   const TSecurityPolicy& aPolicy, TInt aDbPolicySetFlag)
	{
	aSecurityPolicyCon.SetDefaultPolicy(aPolicy);
	if(!(aDbPolicySetFlag & (1 << RSqlSecurityPolicy::ESchemaPolicy)))
		{
		aSecurityPolicyCon.SetDbPolicy(RSqlSecurityPolicy::ESchemaPolicy, aPolicy);
		}
	if(!(aDbPolicySetFlag & (1 << RSqlSecurityPolicy::EReadPolicy)))
		{
		aSecurityPolicyCon.SetDbPolicy(RSqlSecurityPolicy::EReadPolicy, aPolicy);
		}
	if(!(aDbPolicySetFlag & (1 << RSqlSecurityPolicy::EWritePolicy)))
		{
		aSecurityPolicyCon.SetDbPolicy(RSqlSecurityPolicy::EWritePolicy, aPolicy);
		}
	}

/**
Stores a database security policy into aSecurityPolicyCon container.

@param aSecurityPolicyCon Security policies container.
@param aPolicyType Database policy type: RSqlSecurityPolicy::EReadPolicy,
				   RSqlSecurityPolicy::EWritePolicy, RSqlSecurityPolicy::ESchemaPolicy.
@param aPolicy Database security policy object
@param aDbPolicySetFlag Bit flag. Keeps information which database security policies are set and which aren't.
						The parameter value may change if some of the database policies are set by
						StoreDbSecurityPolicyL().

@leave KErrGeneral Invalid security policy type or the corresponding database security policy has been set already.
*/
void TSqlDbSysSettings::StoreDbSecurityPolicyL(CSqlSecurityPolicy& aSecurityPolicyCon, TInt aPolicyType,
											   const TSecurityPolicy& aPolicy, TInt& aDbPolicySetFlag)
	{
	if(aPolicyType < (TInt)RSqlSecurityPolicy::ESchemaPolicy || aPolicyType > (TInt)RSqlSecurityPolicy::EWritePolicy)
		{
		__SQLLEAVE(KErrGeneral);
		}
	if(aDbPolicySetFlag & (1 << aPolicyType))
		{
		__SQLLEAVE(KErrGeneral);
		}
	aSecurityPolicyCon.SetDbPolicy(static_cast <RSqlSecurityPolicy::TPolicyType> (aPolicyType), aPolicy);
	aDbPolicySetFlag |= (1 << aPolicyType);
	}

/**
Stores a database object security policy into aSecurityPolicyCon container.

@param aSecurityPolicyCon Security policies container.
@param aObjType Database object type: RSqlSecurityPolicy::ETable.
@param aObjName Database object name (for example, table name), which is protected by the current security policy.
@param aPolicyType Database policy type: RSqlSecurityPolicy::EReadPolicy,
				   RSqlSecurityPolicy::EWritePolicy, RSqlSecurityPolicy::ESchemaPolicy.
@param aPolicy Database security policy object
@param aDbPolicySetFlag Bit flag. Keeps information which database security policies are set and which aren't.
						The parameter value may change if some of the database policies are set by
						StoreDbSecurityPolicyL().

@leave KErrGeneral Invalid security policy type or invalid database object name length.
*/
void TSqlDbSysSettings::StoreDbObjSecurityPolicyL(CSqlSecurityPolicy& aSecurityPolicyCon, 
							   					  TInt aObjType, const TDesC& aObjName,
							   					  TInt aPolicyType, const TSecurityPolicy& aPolicy)
	{
	if(aPolicyType < (TInt)RSqlSecurityPolicy::EReadPolicy || aPolicyType > (TInt)RSqlSecurityPolicy::EWritePolicy)
		{
		__SQLLEAVE(KErrGeneral);
		}
	if(aObjName.Length() < 1)
		{
		__SQLLEAVE(KErrGeneral);
		}
	__SQLLEAVE_IF_ERROR(aSecurityPolicyCon.SetPolicy(static_cast<RSqlSecurityPolicy::TObjectType> (aObjType), 
							aObjName, static_cast <RSqlSecurityPolicy::TPolicyType> (aPolicyType), aPolicy));
	}

/**
Returns true if settigns table exists. 
(Non-secure databases prior version 3 don't have settings table)

@param aDbName Logical database name: "main" for the main database or attached database name.
*/
TBool TSqlDbSysSettings::SettingsTableExistsL(const TDesC& aDbName)
	{
	HBufC* buf = HBufC::NewLC(sizeof(KSettingsTableCheckSql) + aDbName.Length());
	TPtr sql = buf->Des();
	sql.Format(KSettingsTableCheckSql(), &aDbName);
	sqlite3_stmt* stmtHandle = ::StmtPrepare16L(iDbHandle, sql);
	CleanupStack::PushL(TCleanupItem(&FinalizeStatementHandle, stmtHandle));
	TInt rc = ::StmtNext(stmtHandle);
	CleanupStack::PopAndDestroy();//cleanupItem (statement handle)
	CleanupStack::PopAndDestroy(buf);
	__SQLLEAVE_IF_ERROR(rc);
	return rc == KSqlAtRow;
	}

/**
Returns true if at least one index using aCollationName exists.

@param aDbName Logical database name: "main" for the main database or attached database name,
@param aCollationName SQLITE collation name.
*/
TBool TSqlDbSysSettings::IndexExistsL(const TDesC& aDbName, const TDesC& aCollationName)
	{
	//KGetIndexSql will contain '%<CollationName>%' search pattern, while Symbian OS search patterns use '*' symbol by default.
	//This time the '%' is OK because the serach is performed in the SQLITE master table and before installing the
	//user-defined Like() function.
	HBufC* buf = HBufC::NewLC(sizeof(KGetIndexSql) + aDbName.Length() + aCollationName.Length());
	TPtr sql = buf->Des();
	sql.Format(KGetIndexSql(), &aDbName, &aCollationName);
	sqlite3_stmt* stmtHandle = ::StmtPrepare16L(iDbHandle, sql);
	CleanupStack::PushL(TCleanupItem(&FinalizeStatementHandle, stmtHandle));
	TInt rc = ::StmtNext(stmtHandle);
	CleanupStack::PopAndDestroy(2);//buf, cleanupItem (statement handle)
	__SQLLEAVE_IF_ERROR(rc);
	return rc == KSqlAtRow;
	}
