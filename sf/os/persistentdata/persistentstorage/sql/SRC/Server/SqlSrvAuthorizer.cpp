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
//
// Description:
//

#include "SqlSrvAuthorizer.h"	//MSqlPolicyInspector
#include "SqlSrvMain.h"			//CSqlServer
#include "SqlSrvSecurityMap.h"	//RSqlSecurityMap
#include "SqlSrvDatabase.h"		//CSqlSrvDatabase
#include "SqlSecurityImpl.h"	//CSqlSecurityPolicy
#include "SqlSrvDbSysSettings.h"//TSqlDbSysSettings
#include "SqlSrvUtil.h"			//Global server functions
#include "SqlSrvStatementUtil.h"//Global sql statement related functions
#include "SqlSrvStrings.h"		//KTempDb
#include "sqlite3.h"
#include "SqliteSymbian.h"		//sqlite3SymbianLastOsError()

//This macro is used to suppress "function argument not used" compiler warning.
#define UNUSED_ARG(arg) arg = (arg)

//Array of pragma commands 
const TPtrC8 KPragmaCommands[] = 
	{
	KAutoVacuum(),	KCacheSize(), KCaseSensitiveLike(), KCountChanges(), KDefaultCacheSize(),
	KEmptyResultCallbacks(), KEncoding(), KFullColumnNames(), KFullfsync(), KIncrementalVacuum(), 
	KJournalMode(), KJournalSizeLimit(), KLegacyFileFormat(), KLockingMode(), KPageSize(),
	KMaxPageCount(), KReadUncommitted(), KShortColumnNames(), KSynchronousFlag(), KTempStore(), 
	KTempStoreDirectory(), KDatabaseList(), KForeignKeyList(), KFreelistCount(), KIndexInfo(), 
	KIndexIist(), KPageCount(),KTableInfo(), KSchemaVersion(), KUserVersion(),
	KIntegrityCheck(),KParserTrace(), KVdbeTrace(), KdbeListing()
	};

const TInt KMaxPragmaCommands = sizeof(KPragmaCommands) / sizeof(KPragmaCommands[0]);


//Define the different ways of calling a pragam depending on the following
// 1) If its a secure or non secure database
// 2) If the pragma is called with a parameter (write) or without a parameter (read)
struct TPragmaAccess
	{
	TInt iNonSecureRead; 
	TInt iNonSecureWrite;
	TInt iSecureRead;
	TInt iSecureWrite;
	};

//Table specifying the permissions for each pragma command for secure (shared) and non-secure (public and private)
//databases. For each database permissions for the following situations are specified
//1) With Parameter - e.g "Pragma auto_vacuum = 0"
//2) Without Parameter - e.g "Pragma auto_vacuum" 

//Permissions "without parameters" usually apply to a pragma query (or read)
//Permissions "with parameters" usually apply to pragama set (or write)
//However please note that this is not always the case. e.g "index_info" requires a parameter but is used to query
//(or read) the database and not a pragma set. 
const TPragmaAccess KPermissionsTable[KMaxPragmaCommands] = 
	{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//				NON_SECURE					|				SECURE				|
	//  W/Out Parameter		|With Parameter		|W/Out Parameter|With  Parameter	|Pragma Command 
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		{SQLITE_OK,			SQLITE_IGNORE,		SQLITE_DENY, 	SQLITE_DENY}, 		//0. auto_vacuum
		{SQLITE_OK,			SQLITE_IGNORE,		SQLITE_DENY,	SQLITE_DENY}, 		//1.cache_size
		{SQLITE_OK,			SQLITE_OK,			SQLITE_DENY,	SQLITE_DENY},		//2.case_sensitive_like
		{SQLITE_OK,			SQLITE_OK,			SQLITE_DENY,	SQLITE_DENY}, 		//3.count_changes
		{SQLITE_OK,			SQLITE_IGNORE,		SQLITE_DENY,	SQLITE_DENY}, 		//4.cache_size
		{SQLITE_OK,			SQLITE_IGNORE,		SQLITE_DENY,	SQLITE_DENY}, 		//5.empty_result_callbacks
		{SQLITE_OK,			SQLITE_IGNORE,		SQLITE_DENY,	SQLITE_DENY},  		//6.encoding
		{SQLITE_OK,			SQLITE_OK,			SQLITE_DENY,	SQLITE_DENY},		//7.full_column_names
		{SQLITE_OK,			SQLITE_IGNORE,		SQLITE_DENY,	SQLITE_DENY},		//8.fullfsync
		{SQLITE_IGNORE,		SQLITE_IGNORE,		SQLITE_DENY,	SQLITE_DENY},		//9.incremental_vacuum
		{SQLITE_IGNORE,		SQLITE_IGNORE,		SQLITE_DENY,	SQLITE_DENY},		//10.journal_mode
		{SQLITE_IGNORE,		SQLITE_IGNORE,		SQLITE_DENY,	SQLITE_DENY}, 		//11.journal_size_limit
		{SQLITE_OK,			SQLITE_IGNORE,		SQLITE_DENY,	SQLITE_DENY}, 		//12.legacy_file_format
		{SQLITE_OK,			SQLITE_IGNORE,		SQLITE_DENY,	SQLITE_DENY},		//13.locking_mode
		{SQLITE_OK,			SQLITE_IGNORE,		SQLITE_DENY,	SQLITE_DENY},		//14.page_size
		{SQLITE_IGNORE,		SQLITE_IGNORE,		SQLITE_DENY,	SQLITE_DENY},		//15.max_page_count
		{SQLITE_OK,			SQLITE_IGNORE,		SQLITE_DENY,	SQLITE_DENY},		//16.read_uncommitted
		{SQLITE_OK,			SQLITE_OK,			SQLITE_DENY,	SQLITE_DENY},		//17.short_column_names
		{SQLITE_OK,			SQLITE_IGNORE,		SQLITE_DENY,	SQLITE_DENY},		//18.synchronous
		{SQLITE_OK,			SQLITE_IGNORE,		SQLITE_DENY,	SQLITE_DENY},		//19.temp_store
		{SQLITE_OK,			SQLITE_IGNORE,		SQLITE_DENY,	SQLITE_DENY},		//20.temp_store_directory
		{SQLITE_OK,			SQLITE_OK,			SQLITE_DENY,	SQLITE_DENY},		//21.database_list
		{SQLITE_OK,			SQLITE_OK,			SQLITE_DENY,	SQLITE_DENY},		//22.foreign_key_list
		{SQLITE_IGNORE,		SQLITE_IGNORE,		SQLITE_DENY,	SQLITE_DENY},		//23.freelist_count
		{SQLITE_OK,			SQLITE_OK,			SQLITE_DENY,	SQLITE_DENY},		//24.index_info
		{SQLITE_OK,			SQLITE_OK,			SQLITE_DENY,	SQLITE_DENY},		//25.index_list
		{SQLITE_IGNORE,		SQLITE_IGNORE,		SQLITE_DENY,	SQLITE_DENY},		//26.page_count
		{SQLITE_OK,			SQLITE_OK,			SQLITE_DENY,	SQLITE_DENY},		//27.table_info
		{SQLITE_OK,			SQLITE_IGNORE,		SQLITE_DENY,	SQLITE_DENY},		//28.schema_version
		{SQLITE_OK,			SQLITE_IGNORE,		SQLITE_DENY,	SQLITE_DENY},		//29.user_version
		{SQLITE_OK,			SQLITE_IGNORE,		SQLITE_DENY,	SQLITE_DENY},		//30.integrity_check
		{SQLITE_OK,			SQLITE_IGNORE,		SQLITE_DENY,	SQLITE_DENY},		//31.parser_trace
		{SQLITE_OK,			SQLITE_IGNORE,		SQLITE_DENY,	SQLITE_DENY},		//32.vdbe_trace
		{SQLITE_OK,			SQLITE_IGNORE,		SQLITE_DENY,	SQLITE_DENY},		//33.vdbe_listing
	};


//This const array describes the relation between the database operation type and
//the number of the authorizer argument where the table name is.
//For example:
//- SQLITE_CREATE_TEMP_TABLE operation. The table name is in aDbObjName1 argument, so the array element value is 1.
//- SQLITE_PRAGMA operation. No table name for this operation, so the array element value is 0.
//- SQLITE_CREATE_TEMP_TRIGGER operation. The table name is in aDbObjName2 argument, so the array element value is 2.
const TUint8 KTableNameArgIndex[] =
	{
	/////////////////////////////////////////////////////////////////
	//									  aDbObjName1	  aDbObjName2
	/////////////////////////////////////////////////////////////////
	1,	//SQLITE_COPY                     Table Name      Filename 
	2,	//SQLITE_CREATE_INDEX             Index Name      Table Name      
	1,	//SQLITE_CREATE_TABLE             Table Name      NULL            
	2,	//SQLITE_CREATE_TEMP_INDEX        Index Name      Table Name      
	1,	//SQLITE_CREATE_TEMP_TABLE        Table Name      NULL            
	2,	//SQLITE_CREATE_TEMP_TRIGGER      Trigger Name    Table Name      
	0,	//SQLITE_CREATE_TEMP_VIEW         View Name       NULL            
	2,	//SQLITE_CREATE_TRIGGER           Trigger Name    Table Name      
	0,	//SQLITE_CREATE_VIEW              View Name       NULL            
	1,	//SQLITE_DELETE                   Table Name      NULL            
	2,	//SQLITE_DROP_INDEX               Index Name      Table Name      
	1,	//SQLITE_DROP_TABLE               Table Name      NULL            
	2,	//SQLITE_DROP_TEMP_INDEX          Index Name      Table Name      
	1,	//SQLITE_DROP_TEMP_TABLE          Table Name      NULL            
	2,	//SQLITE_DROP_TEMP_TRIGGER        Trigger Name    Table Name      
	0,	//SQLITE_DROP_TEMP_VIEW           View Name       NULL            
	2,	//SQLITE_DROP_TRIGGER             Trigger Name    Table Name      
	0,	//SQLITE_DROP_VIEW                View Name       NULL            
	1,	//SQLITE_INSERT                   Table Name      NULL            
	0,	//SQLITE_PRAGMA                   Pragma Name     1st arg or NULL 
	1,	//SQLITE_READ                     Table Name      Column Name     
	0,	//SQLITE_SELECT                   NULL            NULL            
	0,	//SQLITE_TRANSACTION              NULL            NULL            
	1,	//SQLITE_UPDATE                   Table Name      Column Name     
	0,	//SQLITE_ATTACH                   Filename        NULL            
	0,	//SQLITE_DETACH                   Database Name   NULL 
	2,	//SQLITE_ALTER_TABLE          	  Database Name   Table Name
	0,	//SQLITE_REINDEX              	  Index Name      NULL
	1,	//SQLITE_ANALYZE              	  Table Name      NULL
	1,	//SQLITE_CREATE_VTABLE			  Table Name	  Module Name	
	1,	//SQLITE_DROP_VTABLE          	  Table Name      Module Name
	0	//SQLITE_FUNCTION				  Function Name   NULL
	};

//The function returns the argument number where the table name is.
inline TInt DbOp2TableNameArgIndex(TInt aDbOpType)
	{
	__ASSERT_DEBUG(aDbOpType > 0 && aDbOpType <= SQLITE_FUNCTION, __SQLPANIC2(ESqlPanicInternalError));
	return KTableNameArgIndex[aDbOpType];
	}

//The function returns the table name, which may be in aDbObjName1 or aDbObjName2, depending on aDbOpType value.
//The return value is "const char" pointer to a zero terminated string.
inline const char* DbOp2TableName(TInt aDbOpType, const char* aDbObjName1, const char* aDbObjName2)
	{
	TInt pos = DbOp2TableNameArgIndex(aDbOpType);
	if(pos == 2)
		{
		__ASSERT_DEBUG(aDbObjName2 != NULL, __SQLPANIC2(ESqlPanicInternalError));
		return aDbObjName2;
		}
	else if(pos == 1)
		{
		__ASSERT_DEBUG(aDbObjName1 != NULL, __SQLPANIC2(ESqlPanicInternalError));
		return aDbObjName1;
		}
	return NULL;//Some database operations do not use table name
	}

//This function returns the database name which may be in aDbObjName1 or aDbName depending on aDbOpType value.
//The return value is "const char" pointer to a zero terminated string.
inline const char* DbOp2DbName(TInt aDbOpType, const char* aDbObjName1, const char* aDbName)
	{
	if(aDbOpType == SQLITE_DETACH || aDbOpType == SQLITE_ALTER_TABLE)
		{
		__ASSERT_DEBUG(aDbObjName1 != NULL, __SQLPANIC2(ESqlPanicInternalError));
		return aDbObjName1;
		}
	return aDbName;//It may be NULL for some database operations
	}

/**
This function performs pragma permission checks for non-secure and secure databases

@param aDbObjName1 Database, Table, View, Trigger, Index, Pragma or File name. It depends on the 
			values of aDbOpType argument. UTF8 encoded, zero-terminated.
@param aParamUsed ETrue if the pragma command has been executed with a parameter, EFalse otherwise
@param aSecure ETrue if the pragam check if for secure database, EFalse otherwise

@return SQLITE_OK		Access is allowed
@return SQLITE_DENY 	The entire SQL statement should be aborted
@return SQLITE_IGNORE	The column should be treated as it has NULL value

@internalComponent
 */ 
static TInt PragmaCheck(const char* aDbObjName1, TBool aParamUsed, TBool aSecure)
	{
	//Retreive the pragma name
	TPtrC8 DbObjName1(KNullDesC8);
	DbObjName1.Set(reinterpret_cast <const TUint8*> (aDbObjName1));
	
	//Access the pragma permissions table depending if its :-
	// 1) Secure or non-secure database.
	// 2) Parameter was used or not.
	for (TInt index=0; index<KMaxPragmaCommands; index++)
		{
		if (CompareNoCase8(DbObjName1,KPragmaCommands[index])== 0)
			{
			if (aSecure)
				{
				if(aParamUsed)
					return KPermissionsTable[index].iSecureWrite;
				else
					return KPermissionsTable[index].iSecureRead;
				}
			else
				{
				if(aParamUsed)
					return KPermissionsTable[index].iNonSecureWrite;
				else
					return KPermissionsTable[index].iNonSecureRead;
				}
			}
		}
	//If the pragma is not on the list then deny access
	return SQLITE_DENY;
	}


/**
This function performs additional permission checks for non-secure (private and public) databases

@param aDbOpType Database operation type, which needs to be authorized.
@param aDbObjName1 Database, Table, View, Trigger, Index, Pragma or File name. It depends on the 
			values of aDbOpType argument. UTF8 encoded, zero-terminated.
@param aDbObjName2 Table or Column name. It depends on the values of aDbOpType argument. UTF8 encoded, zero-terminated.

@return SQLITE_OK		Access is allowed
@return SQLITE_DENY 	The entire SQL statement should be aborted
@return SQLITE_IGNORE	The column should be treated as it has NULL value

@panic SqlDb 7 In _DEBUG mode. Unknown/invalid aDbOpType argument.

@internalComponent
 */ 
static TInt NonSecureChecks(TInt aDbOpType,const char* aDbObjName1, const char* aDbObjName2)
	{
	//=================================================================
	//	aDbOpType							aDbObjName1		aDbObjName2
	//=================================================================
	TInt res = SQLITE_OK;
	switch(aDbOpType)
		{
		case SQLITE_CREATE_INDEX://          Index Name      Table Name      
		case SQLITE_CREATE_TABLE://          Table Name      NULL            
		case SQLITE_CREATE_TRIGGER://        Trigger Name    Table Name      
		case SQLITE_CREATE_VIEW://           View Name       NULL            
		case SQLITE_DROP_INDEX://            Index Name      Table Name            
		case SQLITE_DROP_TABLE://            Table Name      NULL 
		case SQLITE_DROP_TRIGGER://          Trigger Name    Table Name      
		case SQLITE_DROP_VIEW://             View Name       NULL            
		case SQLITE_ALTER_TABLE://			 Database Name   Table Name
		case SQLITE_SELECT://                NULL            NULL            
		case SQLITE_TRANSACTION://           NULL            NULL          
		case SQLITE_DELETE://                Table Name      NULL
		case SQLITE_INSERT://                Table Name      NULL   
		case SQLITE_UPDATE://                Table Name      Column Name		
		case SQLITE_READ://                  Table Name      Column Name     
		case SQLITE_ATTACH://                Filename        NULL            
		case SQLITE_DETACH://                Database Name   NULL
		case SQLITE_REINDEX://				 Index Name      NULL
		case SQLITE_ANALYZE://				 Table Name      NULL
		case SQLITE_FUNCTION:
			break;
		case SQLITE_PRAGMA://                Pragma Name     1st arg or NULL 
			res = PragmaCheck(aDbObjName1, (aDbObjName2 != NULL), EFalse);
			break;
//All "temp" operations are handled earlier, in CSqlSrvDatabase::AuthorizeCallback(), where a check for "temp"
//database name is performed.
//      case SQLITE_CREATE_TEMP_INDEX://     Index Name      Table Name      
//      case SQLITE_CREATE_TEMP_TABLE://     Table Name      NULL            
//      case SQLITE_CREATE_TEMP_TRIGGER://   Trigger Name    Table Name      
//      case SQLITE_CREATE_TEMP_VIEW://      View Name       NULL            
//      case SQLITE_DROP_TEMP_INDEX://       Index Name      Table Name      
//      case SQLITE_DROP_TEMP_TABLE://       Table Name      NULL            
//      case SQLITE_DROP_TEMP_TRIGGER://     Trigger Name    Table Name      
//      case SQLITE_DROP_TEMP_VIEW://        View Name       NULL
//"CREATE VIRTUAL TABLE" and "DROP VIRTUAL TABLE" sql statements are not supported
//		case SQLITE_CREATE_VTABLE:
//		case SQLITE_DROP_VTABLE:
		default:
			__ASSERT_DEBUG(EFalse, __SQLPANIC2(ESqlPanicInternalError));
			break;
			}
	return res;
	}

/**
This function performs additional permission checks for secure databases

@param aSecurityPolicy Security policy corresponding to this database
@param aDbOpType Database operation type, which needs to be authorized.
@param aDbObjName1 Database, Table, View, Trigger, Index, Pragma or File name. It depends on the 
			values of aDbOpType argument. UTF8 encoded, zero-terminated.
@param aDbObjName2 Table or Column name. It depends on the values of aDbOpType argument. UTF8 encoded, zero-terminated.

@return SQLITE_OK		Access is allowed
@return SQLITE_DENY 	The entire SQL statement should be aborted
@return SQLITE_IGNORE	The column should be treated as it has NULL value

@panic SqlDb 7 In _DEBUG mode. Unknown/invalid aDbOpType argument.

@internalComponent
 */ 
static TInt SecureChecks(const CSqlSecurityPolicy* aSecurityPolicy,TInt aDbOpType,const char* aDbObjName1, const char* aDbObjName2)
	{
	TPtrC8 tblName(KNullDesC8);
	const char* tblNamePtr = DbOp2TableName(aDbOpType, aDbObjName1, aDbObjName2);
	if(tblNamePtr)
		{
		tblName.Set(reinterpret_cast <const TUint8*> (tblNamePtr));
		}
	
	//Under no circumstances is allowed to do any operation with the system tables.
	//(Even SQLITE_READ operation, because the system tables data is read at the moment when the database
	// is created/opened)
	if(::IsSystemTableName(tblName))
		{
		return SQLITE_DENY;
		}
	//=================================================================
	//	aDbOpType							aDbObjName1		aDbObjName2
	//=================================================================
	MSqlPolicyInspector& inspector = ::SqlServer().SecurityInspector();
	TSecurityPolicy schemaPolicy = aSecurityPolicy->DbPolicy(RSqlSecurityPolicy::ESchemaPolicy);
	TSecurityPolicy writePolicy = aSecurityPolicy->DbPolicy(RSqlSecurityPolicy::EWritePolicy);
	TSecurityPolicy readPolicy = aSecurityPolicy->DbPolicy(RSqlSecurityPolicy::EReadPolicy);
	TInt res = SQLITE_OK;
	switch(aDbOpType)
		{
		//"Database schema policy" check
		case SQLITE_CREATE_INDEX://          Index Name      Table Name      
		case SQLITE_CREATE_TABLE://          Table Name      NULL            
		case SQLITE_CREATE_TRIGGER://        Trigger Name    Table Name      
		case SQLITE_CREATE_VIEW://           View Name       NULL            
		case SQLITE_DROP_INDEX://            Index Name      Table Name      
		case SQLITE_DROP_TABLE://            Table Name      NULL            
		case SQLITE_DROP_TRIGGER://          Trigger Name    Table Name      
		case SQLITE_DROP_VIEW://             View Name       NULL            
		case SQLITE_ALTER_TABLE://			 Database Name   Table Name
			if(!inspector.Check(schemaPolicy))
				{
				res = SQLITE_DENY;	
				}
			break;
		//No policy check
		case SQLITE_SELECT://                NULL            NULL            
		case SQLITE_TRANSACTION://           NULL            NULL            
			break;
		//"Database schema policy" for sqlite tables
		//"Database schema policy" || "Database write policy" for user tables
		case SQLITE_DELETE://                Table Name      NULL            
		case SQLITE_INSERT://                Table Name      NULL            
		case SQLITE_UPDATE://                Table Name      Column Name
			if(!inspector.Check(schemaPolicy))
				{
				res = SQLITE_DENY;	
				if(!::IsSqliteTableName(tblName))
					{
					if(inspector.Check(writePolicy))
						{
						res = SQLITE_OK;
						}
					}
				}
			break;
		//"Database schema policy" || "Database read policy" || "Database write policy" for sqlite tables
		//"Database schema policy" || "Database read policy"  for user tables
		case SQLITE_READ://                  Table Name      Column Name     
			if(!(inspector.Check(schemaPolicy) || inspector.Check(readPolicy)))
				{
				res = SQLITE_DENY;	
				if(::IsSqliteTableName(tblName))
					{
					if(inspector.Check(writePolicy))
						{
						res = SQLITE_OK;
						}
					}
				}
			break;
		case SQLITE_PRAGMA://                Pragma Name     1st arg or NULL 
			res = PragmaCheck(aDbObjName1, (aDbObjName2 != NULL), ETrue);	
			break;
		case SQLITE_ATTACH://                Filename        NULL
		case SQLITE_DETACH://                Database Name   NULL
		//If the operation is SQLITE_ATTACH or SQLITE_DETACH, return SQLITE_DENY.
		//"ATTACH DATABASE"/"DETACH DATABASE" operations are performed by separate "attach/detach db" methods.
			res = SQLITE_DENY;	
			break;
		//No policy check
		case SQLITE_REINDEX://				Index Name      NULL
		case SQLITE_ANALYZE://				Table Name      NULL
			break;
		//No policy check
		case SQLITE_FUNCTION:
			break;
//All "temp" operations are handled earlier, in CSqlSrvDatabase::AuthorizeCallback(), where a check for "temp"
//database name is performed.
//      case SQLITE_CREATE_TEMP_INDEX://     Index Name      Table Name      
//      case SQLITE_CREATE_TEMP_TABLE://     Table Name      NULL            
//      case SQLITE_CREATE_TEMP_TRIGGER://   Trigger Name    Table Name      
//      case SQLITE_CREATE_TEMP_VIEW://      View Name       NULL            
//      case SQLITE_DROP_TEMP_INDEX://       Index Name      Table Name      
//      case SQLITE_DROP_TEMP_TABLE://       Table Name      NULL            
//      case SQLITE_DROP_TEMP_TRIGGER://     Trigger Name    Table Name      
//      case SQLITE_DROP_TEMP_VIEW://        View Name       NULL            
//"CREATE VIRTUAL TABLE" and "DROP VIRTUAL TABLE" sql statements are not supported
//		case SQLITE_CREATE_VTABLE:
//		case SQLITE_DROP_VTABLE:
		default:
			__ASSERT_DEBUG(EFalse, __SQLPANIC2(ESqlPanicInternalError));
			break;
		}
	return res;
	}

/**
This callback function is invoked by the SQLITE engine at SQL statement compile time 
for each attempt to access a column of a table in the database.

The callback returns SQLITE_OK if access is allowed, 
SQLITE_DENY if the entire SQL statement should be aborted with an error and 
SQLITE_IGNORE if the column should be treated as a NULL value.

@param aDb "This" pointer (to the rellated CSqlSrvDatabase object).
@param aDbOpType Database operation type, which needs to be authorized. It could be one of these:

@code
=================================================================
aDbOpType						aDbObjName1		aDbObjName2
=================================================================
SQLITE_CREATE_INDEX             Index Name      Table Name      
SQLITE_CREATE_TABLE             Table Name      NULL            
SQLITE_CREATE_TEMP_INDEX        Index Name      Table Name      
SQLITE_CREATE_TEMP_TABLE        Table Name      NULL            
SQLITE_CREATE_TEMP_TRIGGER      Trigger Name    Table Name      
SQLITE_CREATE_TEMP_VIEW         View Name       NULL            
SQLITE_CREATE_TRIGGER           Trigger Name    Table Name      
SQLITE_CREATE_VIEW              View Name       NULL            
SQLITE_DELETE                   Table Name      NULL            
SQLITE_DROP_INDEX               Index Name      Table Name      
SQLITE_DROP_TABLE               Table Name      NULL            
SQLITE_DROP_TEMP_INDEX          Index Name      Table Name      
SQLITE_DROP_TEMP_TABLE          Table Name      NULL            
SQLITE_DROP_TEMP_TRIGGER        Trigger Name    Table Name      
SQLITE_DROP_TEMP_VIEW           View Name       NULL            
SQLITE_DROP_TRIGGER             Trigger Name    Table Name      
SQLITE_DROP_VIEW                View Name       NULL            
SQLITE_INSERT                   Table Name      NULL            
SQLITE_PRAGMA                   Pragma Name     1st arg or NULL 
SQLITE_READ                     Table Name      Column Name     
SQLITE_SELECT                   NULL            NULL            
SQLITE_TRANSACTION              NULL            NULL            
SQLITE_UPDATE                   Table Name      Column Name     
SQLITE_ATTACH                   Filename        NULL            
SQLITE_DETACH                   Database Name   NULL 
SQLITE_ALTER_TABLE          	Database Name   Table Name
SQLITE_REINDEX              	Index Name      NULL
SQLITE_ANALYZE              	Table Name      NULL
SQLITE_CREATE_VTABLE			Table Name	  	Module Name	
SQLITE_DROP_VTABLE          	Table Name      Module Name
SQLITE_FUNCTION				    Function Name   NULL
=================================================================
@endcode

@param aDbObjName1 Database, Table, View, Trigger, Index, Pragma or File name. It depends on the 
			values of aDbOpType argument. UTF8 encoded, zero-terminated.
@param aDbObjName2 Table or Column name. It depends on the values of aDbOpType argument. UTF8 encoded, zero-terminated.
@param aDbName Database name - "main", "temp", etc. UTF8 encoded, zero-terminated.
@param aTrgOrViewName The name of the inner-most trigger or view that is responsible for the access
			attempt or NULL if this access attempt is directly from input SQL code. UTF8 encoded, zero-terminated.

@return SQLITE_OK		Access is allowed
@return SQLITE_DENY 	The entire SQL statement should be aborted
@return SQLITE_IGNORE	The column should be treated as it has NULL value

@panic SqlDb 4 In _DEBUG mode. The authorizer was called with NULL aDb argument.

@internalComponent
*/
TInt CSqlSrvDatabase::AuthorizeCallback(void* aDb, TInt aDbOpType, 
										const char* aDbObjName1, const char* aDbObjName2, 
										const char* aDbName, const char* aTrgOrViewName)
	{
	UNUSED_ARG(aTrgOrViewName);
 	__ASSERT_DEBUG(aDb != NULL, __SQLPANIC2(ESqlPanicBadArgument));
	
#ifdef _SQL_AUTHORIZER_TRACE_ENABLED
    enum TDbOpType {EOpCreateIndex = 1, EOpCreateTable, EOpCreateTempIndex, EOpCreateTempTable, 
        EOpCreateTempTrigger, EOpCreateTempView, EOpCreateTrigger, EOpCreateView, EOpDelete, EOpDropIndex, 
        EOpDropTable, EOpDropTempIndex, EOpDropTempTable, EOpDropTempTrigger, EOpDropTempView, EOpDropTrigger,
		EOpDropView, EOpInsert, EOpPragma, EOpRead, EOpSelect, EOpTransaction, EOpUpdate, EOpAttach, EOpDettach,
		EOpAlterTable, EOpReindex, EOpAnalyze, EOpCreateVTable, EOpDropVTable, EOpFunctionCall};
	TDbOpType dbOpType = static_cast <TDbOpType> (aDbOpType);//can be seen now in the debugger
	::PrintAuthorizerArguments(dbOpType, aDbObjName1, aDbObjName2, aDbName, aTrgOrViewName);
#endif

	CSqlSrvDatabase& db = *static_cast <CSqlSrvDatabase*> (aDb);

	//1. If the authorizer is currently disabled - return SQLITE_OK.
	//   (This happens when a database is attached/detached)
	if(db.iAuthorizerDisabled)
		{
		return SQLITE_OK;	
		}

	TPtrC8 dbName(KNullDesC8);
	const char* dbNamePtr = DbOp2DbName(aDbOpType, aDbObjName1, aDbName);//dbNamePtr is zero terminated
	if(dbNamePtr)
		{
		dbName.Set(reinterpret_cast <const TUint8*> (dbNamePtr));
		}
	aDbName = NULL;//No more use of aDbName argument inside the function.
	
	//2. If the database name is KTempDb, then allow the access. It is a local database 
	//   (for the client), deleted when closed.
	if(dbName.Compare(KTempDb8) == 0) 	//dbName is guaranteed to be in lower case if it is "temp",
		{								//so it is possible to use binary string comparison
		return SQLITE_OK;	
		}

	//3. Find the security policies. For DefaultAccess initialized with NULL.
	const CSqlSecurityPolicy* securityPolicy = NULL;
	if(dbName.Compare(KMainDb8) == 0||dbName.Length() == 0)	//dbName is guaranteed to be in lower case if it is "main",
		{								//so it is possible to use binary string comparison
		//4. This is the main database.
		securityPolicy = db.iSecurityPolicy;
		}
	else
		{
		//5. This is an attached database. Find the attached database security policies.
	    //dbNamePtr is used here because it is zero terminated
		TSqlAttachDbPair* attachDbPair = db.iAttachDbMap.Entry(reinterpret_cast <const TUint8*> (dbNamePtr));
		if(attachDbPair)
			{//secure database, find the security policies
			const TUint8* securityMapKey = attachDbPair->iData;
			RSqlSecurityMap& map = ::SqlServer().SecurityMap();
			TSqlSecurityPair* pair = map.Entry(securityMapKey);
			if(pair)
				{
				securityPolicy = pair->iData;
				}
			}
		}
		
	//Here we have: 
	// - valid database name (not NULL);
	
	//6. Default or Security Policy Checks
	return !securityPolicy ? NonSecureChecks(aDbOpType,aDbObjName1,aDbObjName2): SecureChecks(securityPolicy,aDbOpType,aDbObjName1,aDbObjName2);
	}
