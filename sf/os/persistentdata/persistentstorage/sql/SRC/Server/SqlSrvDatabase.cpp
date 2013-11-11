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

#include "SqlSrvFileData.h"		//TSqlSrvFileData
#include "SqlSrvMain.h"			//CSqlServer
#include "SqlSrvAuthorizer.h"	//MSqlPolicyInspector
#include "SqlSrvDatabase.h"
#include "SqlSrvStatement.h"
#include "SqlUtil.h"			//Panic codes, Sql2OsErrCode()
#include "SqlSrvUtil.h"			//Global server functions
#include "SqlCompact.h"
#include "SqlSrvResourceProfiler.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "SqlSrvDatabaseTraces.h"
#endif
#include "SqlTraceDef.h"

//
// The following macro disables the creation/loading of the settings table.
// It is for internal testing purposes only!
// 
//    __SQL_DISABLE_SYMBIAN_SETTINGS_TABLE__
// 
// This means that the database index will always be rebuilt when loaded by 
// by the server regardless of the current system collation/locale in use. 
// The benefit of enabling this macro is that a client can send PRAGMA 
// commands to the SQL server before any tables have been explicity created in 
// a NON-SECURE database (secure databases still automatically get a security 
// policy table).
//
// The macro is applied inside:  
//		CSqlSrvDatabase::StoreSettingsL
// 		CSqlSrvDatabase::ProcessSettingsL
// 
// We should inform the user at compile-time if this macro has been enabled:
#if defined(__SQL_DISABLE_SYMBIAN_SETTINGS_TABLE__)
#pragma message(">>> WARNING: Use of SYMBIAN_SETTINGS table has been disabled <<<")
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////        Local const data   ///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* KErrMsg1 = "Missing ESCAPE expression";
const char* KErrMsg2 = "ESCAPE expression must be a single character";

////////////////////////////////////////////////////////
//Attach/detach SQL statements (zero-terminated strings)
_LIT(KAttachDb, "ATTACH DATABASE :FileName AS :DbName\x0");
_LIT(KDetachDb, "DETACH DATABASE :DbName\x0");
////////////////////////////////////////////////////////
// Pragma SQL statements. The database names in all statements are quoted to avoid the "sql injection" threat.
// (At the moment there is no way to pass an invalid database name for these statements, because the database has to be attached
//  first and a parameter binding is used there. So, the quoting is just for safety and against changes in the future)
_LIT(KCacheSizePragma,	"PRAGMA \"%S\".cache_size=%d\x0");
_LIT(KPageSizePragma,	"PRAGMA \"%S\".page_size=%d\x0");
_LIT(KAutoVacuumPragma,	"PRAGMA \"%S\".auto_vacuum=%d\x0");
//_LIT(KPersist, "persist");
//_LIT(KPersistentJournalPragma, "PRAGMA \"%S\".journal_mode=%S\x0");
_LIT(KJournalSizeLimitPragma, "PRAGMA \"%S\".journal_size_limit=%d\x0");
////////////////////////////////////////////////////////
//"LIKE" - user defined function name
_LIT8(KStrLikeFuncName,  "LIKE\x0");

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////        Local functions    ///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Local function, used for comparing TSqlAttachDbPair objects.
//(TSqlAttachDbPair structure represents type of the objects, members
// of the map used for keeping the information regarding attached databases)
//
//Note that iKey members of aLeft and aRight function parameters are expected to be 
//UTF8 encoded, zero-terminated strings.
//
//The function will panic with panic code 7 in _DEBUG mode if iKey member of aLeft or
//aRight argument is NULL.
static TInt Compare(const TSqlAttachDbPair& aLeft, const TSqlAttachDbPair& aRight)
	{
	__ASSERT_DEBUG(aLeft.iKey != NULL && aRight.iKey != NULL, __SQLPANIC2(ESqlPanicInternalError));
	return ::CompareNoCase8(TPtrC8(aLeft.iKey), TPtrC8(aRight.iKey));
	}

//Local function, used for comparing TSqlCompactDbPair objects.
//(TSqlCompactDbPair structure represents type of the objects, members
// of the map used for keeping the information regarding compacted databases)
//
//Note that iKey members of aLeft and aRight function parameters are expected to be 
//UTF16 encoded strings.
//
//The function will panic with panic code 7 in _DEBUG mode if iKey member of aLeft or
//aRight argument is NULL.
static TInt Compare2(const TSqlCompactDbPair& aLeft, const TSqlCompactDbPair& aRight)
	{
	__ASSERT_DEBUG(aLeft.iKey != NULL && aRight.iKey != NULL, __SQLPANIC2(ESqlPanicInternalError));
	return ::CompareNoCase(*aLeft.iKey, *aRight.iKey);
	}

//Creates/opens database file (database file name in aFileData parameter) and initializes aDbHandle parameter.
//The database will be created either with UTF-16 or UTF-8 encoding, depending on the 
//TSqlSrvFileData::IsUTF16() property.
static void CreateDbHandleL(const TSqlSrvFileData& aFileData, sqlite3*& aDbHandle)
	{
	if(aFileData.ConfigParams().iDbEncoding == TSqlSrvConfigParams::EEncUtf8)
		{
		TBuf8<KMaxFileName + 1> fileNameZ;
		if(!::UTF16ZToUTF8Z(aFileData.FileNameZ(), fileNameZ))
			{
			__SQLLEAVE2(KErrGeneral);	
			}
		__SQLLEAVE_IF_ERROR2(::CreateDbHandle8(fileNameZ, aDbHandle));
		}
	else
		{
		__SQLLEAVE_IF_ERROR2(::CreateDbHandle16(aFileData.FileNameZ(), aDbHandle));
		}
	}
	
//LoadAttachedDbSecurityPolicyLC() creates a new CSqlSecurityPolicy object and initializes it with the 
//security policies read from the attached database file, which name is in aFileData parameter.
//The created database security policy object is placed in the cleanup stack.
//The caller is responsible for the destruction of the created and returned security policy object.
//This function is used to read the security policies of attached databases.
static CSqlSecurityPolicy* LoadAttachedDbSecurityPolicyLC(const TSqlSrvFileData& aFileData)
	{
	//Create new database security policy object and initialize it with a default security policy
	TSecurityPolicy defaultPolicy(TSecurityPolicy::EAlwaysFail);
	CSqlSecurityPolicy* dbPolicy = CSqlSecurityPolicy::NewLC(defaultPolicy);
	//This is an attached  database and has to be opened
	sqlite3* dbHandle = NULL;
	CreateDbHandleL(aFileData, dbHandle);
	CleanupStack::PushL(TCleanupItem(&CloseDbCleanup, dbHandle));
	//Read the security policies.
	TSqlDbSysSettings dbSysSettings(dbHandle);
	dbSysSettings.LoadSecurityPolicyL(*dbPolicy);
	CleanupStack::PopAndDestroy();//TCleanupItem(&CloseDbCleanup, dbHandle)
	return dbPolicy;
	}

//LoadDbSecurityPolicyLC() creates a new CSqlSecurityPolicy object and initializes it with the 
//security policies read from the database file.
//The created database security policy object is placed in the cleanup stack.
//The caller is responsible for destroying the returned database security policy object.
//The function is used to read the security policy of the main database.
static CSqlSecurityPolicy* LoadDbSecurityPolicyLC(sqlite3* aDbHandle)
	{
	__ASSERT_DEBUG(aDbHandle != NULL, __SQLPANIC2(ESqlPanicBadArgument));
	//Create new database security policy object and initialize it with a default security policy
	TSecurityPolicy defaultPolicy(TSecurityPolicy::EAlwaysFail);
	CSqlSecurityPolicy* dbPolicy = CSqlSecurityPolicy::NewLC(defaultPolicy);
	//Read the security policies.
	TSqlDbSysSettings dbSysSettings(aDbHandle);
	dbSysSettings.LoadSecurityPolicyL(*dbPolicy);
	return dbPolicy;
	}

//CreateStrCopyLC() makes a copy of aSrc string and places it in the cleanup stack.
//aSrc is expected to be UTF8 encoded, zero terminated string.
//The function panics in _DEBUG mode if aSrc is NULL.
static TUint8* CreateStrCopyLC(const TUint8* aSrc)
	{
	__ASSERT_DEBUG(aSrc != NULL, __SQLPANIC2(ESqlPanicBadArgument));
	TInt len = User::StringLength(aSrc) + 1;
	TUint8* copy = new (ELeave) TUint8[len];
	Mem::Copy(copy, aSrc, len);
	CleanupStack::PushL(copy);
	return copy;
	}

//EnableAuthorizer() function is used to reenable the authorizer callback
//during the stack cleanup.
static void EnableAuthorizer(void* aAuthorizerDisabled)
	{
	__ASSERT_DEBUG(aAuthorizerDisabled != NULL, __SQLPANIC2(ESqlPanicBadArgument));
	TBool* authorizerDisabled = static_cast <TBool*> (aAuthorizerDisabled);
	*authorizerDisabled = EFalse;
	}

//Used by DbFileCleanup()
NONSHARABLE_STRUCT(TDbFileCleanup)
	{
	TDbFileCleanup(const TSqlSrvFileData& aSqlSrvFileData, sqlite3*& aDbHandle) :
		iSqlSrvFileData(aSqlSrvFileData),
		iDbHandle(aDbHandle)
		{
		//aDbHandle can be NULL (it is a reference to a pointer and the pointer can be initialized later)
		}
	void Cleanup()
		{
		::CloseDbHandle(iDbHandle);//This operation also deletes the journal file
		iDbHandle = NULL;
		(void)iSqlSrvFileData.Fs().Delete(iSqlSrvFileData.FileName());
		}
private:
	const TSqlSrvFileData&	iSqlSrvFileData;
	sqlite3*& 				iDbHandle;
	};

//DbFileCleanup() is used to close and delete the database file during the stack cleanup, if
//CSqlSrvDatabase's ConstructL() method(s) leave (when creating a new database file).
static void DbFileCleanup(void* aDbFileCleanup)
	{
	__ASSERT_DEBUG(aDbFileCleanup != NULL, __SQLPANIC2(ESqlPanicBadArgument));
	TDbFileCleanup* dbFileCleanup = static_cast <TDbFileCleanup*> (aDbFileCleanup);
	dbFileCleanup->Cleanup();
	}

//Executes "PRAGMA" SQL statement + INTEGER value.
//Pragma parameters:
// aValue - integer pragma value to be set;
// aPragma - pragma statement, the format is: ""PRAGMA <database name>.<parameter name>=%d\x0""
// aDbName - "main" or the attached database name
//This function is used for setting "cache_size", "page_size", "auto_vacuum" pragmas.
//During the call the authorizer will be disabled.
static TInt ExecPragma(sqlite3* aDbHandle, TBool& aAuthorizerDisabled, const TDesC& aPragma, TInt aValue, const TDesC& aDbName = KMainDb16)
	{
    __SQLTRACE_INTERNALSEXPR(TPtrC pragmaprnptr(aPragma.Left(aPragma.Length() - 1)));
	SQL_TRACE_INTERNALS(OstTraceExt4(TRACE_INTERNALS, EXECPRAGMA_ENTRY, "Entry;0;ExecPragma;sqlite3*=0x%X;aPragma=%S;aValue=%d;aDbName=%S", (TUint)aDbHandle, __SQLPRNSTR(pragmaprnptr), aValue, __SQLPRNSTR(aDbName)));
	__ASSERT_DEBUG(aDbHandle != NULL, __SQLPANIC2(ESqlPanicBadArgument));
	TBuf<KMaxFileName + 64> pragmaSql;//(KMaxFileName + 64) characters buffer length is enough for the longest possible PRAGMA statement
	pragmaSql.Format(aPragma, &aDbName, aValue);
	TBool authorizerDisabledState = aAuthorizerDisabled;
	aAuthorizerDisabled	= ETrue;
	TInt err = DbExecStmt16(aDbHandle, pragmaSql);
	aAuthorizerDisabled = authorizerDisabledState;
	SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, EXECPRAGMA_EXIT, "Exit;0;ExecPragma;sqlite3*=0x%X;err=%d", (TUint)aDbHandle, err));
	return err;
	}

//The journal size limit is set to be at lest 16 pages and no less than 64 Kb.
static void SetJournalSizeLimitL(sqlite3* aDbHandle, TBool& aAuthorizerDisabled, TInt aPageSize, const TDesC& aDbName = KMainDb16)
	{
	__ASSERT_DEBUG(aDbHandle != NULL, __SQLPANIC2(ESqlPanicBadArgument));
	if(aPageSize == TSqlSrvConfigParams::KConfigPrmValueNotSet)
		{
		__SQLLEAVE_IF_ERROR2(DbPageSize(aDbHandle, aDbName, aPageSize));
		}
	const TInt KPageMultiplier = 16;
	const TInt KDefaultJournalSizeLimit = 64 * 1024;
	const TInt KMaxJournalSizeLimit = 512 * 1024;
	const TInt KJournalSizeLimit = Min((aPageSize * KPageMultiplier), KMaxJournalSizeLimit);
	if(KJournalSizeLimit > KDefaultJournalSizeLimit)
		{
		__SQLLEAVE_IF_ERROR2(::ExecPragma(aDbHandle, aAuthorizerDisabled, KJournalSizeLimitPragma, KJournalSizeLimit));
		}
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////   CSqlSrvDatabase class    /////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////   Object creation methods  /////////////////////////////////////////////

/**
Creates new CSqlSrvDatabase instance which creates and manages new secure SQL database.

@param aFileData A reference to a TSqlSrvFileData instance, which keeps the database name, a
     			 file session reference and some other database file related properties.
				 The format of the name must be:
				 \<drive\>:\<[SID]database file name excluding the path\>.
				 "[SID]" refers to SID of the application which creates the database.
@param aSecurityPolicy The database security policies container. 
					   CSqlSrvDatabase instance stores the pointer in the security policy map,
					   if it does not exist there already. The security policies map is owned by the CSqlServer class.

@return A pointer to the created CSqlSrvDatabase instance.

@see CSqlServer
@see TSqlSrvFileData
@see MSqlPolicyInspector
@see RSqlSecurityMap
@see CSqlSecurityPolicy

@leave KErrNoMemory, an out of memory condition has occurred;
	   KErrArgument, if a system table name found in the security policies (aSecurityPolicy);
	   KErrAlreadyExists, the file already exists;
       KErrNotReady, the drive does not exist or is not ready;
       KErrInUse, the file is already open;
       KErrPermissionDenied, the client application does not satisfy the relevant database security policies.
                      Note that the function may also leave with some other database specific 
                      errors categorised as ESqlDbError, and other system-wide error codes.
                      
@panic SqlDb 4 In _DEBUG mode if aFileData does not refer to a secure database file name.
@panic SqlDb 4 In _DEBUG mode if aSecurityPolicy is NULL.
*/
CSqlSrvDatabase* CSqlSrvDatabase::CreateSecureL(const TSqlSrvFileData& aFileData, CSqlSecurityPolicy* aSecurityPolicy)
	{
	SQL_TRACE_INTERNALS(OstTrace0(TRACE_INTERNALS, CSQLSRVDATABASE_CREATESECUREL_ENTRY, "Entry;0;CSqlSrvDatabase::CreateSecureL"));
	__ASSERT_DEBUG(aFileData.IsSecureFileNameFmt(), __SQLPANIC2(ESqlPanicBadArgument));
	__ASSERT_DEBUG(aSecurityPolicy != NULL, __SQLPANIC2(ESqlPanicBadArgument));
	if(!::SqlServer().SecurityInspector().Check(aSecurityPolicy->DbPolicy(RSqlSecurityPolicy::ESchemaPolicy)))
		{
		//The caller has no "SCHEMA" policy. Then the client is not given a permission to create the database.
		//Delete aSecurityPolicy since no database object is going to be created and the security policy object 
		//won't be put in the security policies map.
		delete aSecurityPolicy;
		__SQLLEAVE2(KErrPermissionDenied);
		}
	//What does happen with aSecurityPolicy instance?
	// If the database is created successfully, then a lookup will be made in the security policies map.
	// (the security policies map contains reference counted security policies)
	//    If the same security policy already exists in the map, then aSecurityPolicy will be deleted.
	//                                          The reference counter of the found policy will be incremented.
	//    If aSecurityPolicy is not in the map, then it will be put in the map and removed
	//                                          from the map when CSqlSrvDatabase object is destroyed.
	//											(the "remove" operation decrements the security policy counter
	//											and destroys the policy if it reaches 0)
	//
	//The security policy map pair is:
	//{secure_db_name, reference_counted db_security_policy}
	//The security policy is reference counted, because the same database can be shared between one or more
	//connections (clients), in which case only a single, reference counted instance of the database security
	//policy is kept in the map.
	CleanupStack::PushL(aSecurityPolicy);
	CSqlSrvDatabase* self = new (ELeave) CSqlSrvDatabase();
	CleanupStack::Pop(aSecurityPolicy);
	CleanupStack::PushL(self);
	self->ConstructCreateSecureL(aFileData, aSecurityPolicy);
	CleanupStack::Pop(self);
	SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, CSQLSRVDATABASE_CREATESECUREL_EXIT, "Exit;0x%X;CSqlSrvDatabase::CreateSecureL;sqlite3*=0x%X", (TUint)self, (TUint)self->iDbHandle));
	return self;
	}

/**
Creates new CSqlSrvDatabase instance which creates and manages new SQL database.

@param aFileData A reference to a TSqlSrvFileData instance, which keeps the database name, a
     			 file session reference and some other database file related properties.

@return A pointer to the created CSqlSrvDatabase instance.

@see CSqlServer
@see TSqlSrvFileData
@see MSqlPolicyInspector
@see RSqlSecurityMap
@see CSqlSecurityPolicy

@leave KErrNoMemory, an out of memory condition has occurred;
	   KErrArgument, the file name is a name of a secure database;
	   KErrAlreadyExists, the file already exists;
       KErrNotReady, the drive does not exist or is not ready;
       KErrInUse, the file is already open;
       KErrArgument, the file name refers to a secure database, but aSecurityPolicy is NULL;
                      Note that the function may also leave with some other database specific 
                      errors categorised as ESqlDbError, and other system-wide error codes.

@panic SqlDb 4 In _DEBUG mode if aFileData refers to a secure database file name.
*/
CSqlSrvDatabase* CSqlSrvDatabase::CreateL(const TSqlSrvFileData& aFileData)
	{
	SQL_TRACE_INTERNALS(OstTrace0(TRACE_INTERNALS, CSQLSRVDATABASE_CREATEL_ENTRY, "Entry;0;CSqlSrvDatabase::CreateL"));
	__ASSERT_DEBUG(!aFileData.IsSecureFileNameFmt(), __SQLPANIC2(ESqlPanicBadArgument));
	CSqlSrvDatabase* self = new (ELeave) CSqlSrvDatabase();
	CleanupStack::PushL(self);
	self->ConstructCreateL(aFileData);
	CleanupStack::Pop(self);
	SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, CSQLSRVDATABASE_CREATEL_EXIT, "Exit;0x%X;CSqlSrvDatabase::CreateL;sqlite3*=0x%X", (TUint)self, (TUint)self->iDbHandle));
	return self;
	}

/**
Creates new CSqlSrvDatabase instance which opens and manages an existing SQL database.

@param aFileData A reference to a TSqlSrvFileData instance, which keeps the database name, a
     			 file session reference and some other database file related properties.
				 If this is a secure database, then the format of the name must be:
				 \<drive\>:\<[SID]database file name excluding the path\>.
				 If this is a non-secure database, then the file name has to be the full database file name.
				 "[SID]" refers to SID of the application which created the database.
				 If this is application's private database, then the format of aFileData is as it is described
				 in TSqlSrvFileData::SetFromHandleL() comments.

@return A pointer to the created CSqlSrvDatabase instance.

@leave KErrNoMemory, an out of memory condition has occurred;
       KErrNotReady, the drive does not exist or is not ready;
       KErrNotFound, the database file does not exist;
       KErrInUse, the file is already open;
       KErrPermissionDenied, the client application does not satisfy the relevant database security policies.
                      Note that the function may also leave with some other database specific 
                      errors categorised as ESqlDbError, and other system-wide error codes.

@see CSqlServer
@see TSqlSrvFileData
@see MSqlPolicyInspector
@see RSqlSecurityMap
@see CSqlSecurityPolicy
@see TSqlSrvFileData::SetFromHandleL()
*/
CSqlSrvDatabase* CSqlSrvDatabase::OpenL(const TSqlSrvFileData& aFileData)
	{
	SQL_TRACE_INTERNALS(OstTrace0(TRACE_INTERNALS, CSQLSRVDATABASE_OPENL_ENTRY, "Entry;0;CSqlSrvDatabase::OpenL"));
	CSqlSrvDatabase* self = new (ELeave) CSqlSrvDatabase();
	CleanupStack::PushL(self);
	aFileData.IsSecureFileNameFmt() ? self->ConstructOpenSecureL(aFileData) : self->ConstructOpenL(aFileData);
	CleanupStack::Pop(self);
	SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, CSQLSRVDATABASE_OPENL_EXIT, "Exit;0x%X;CSqlSrvDatabase::OpenL;sqlite3*=0x%X", (TUint)self, (TUint)self->iDbHandle));
	return self;
	}

/**
Cleans up the allocated for the database connection memory and other resources.
*/
CSqlSrvDatabase::~CSqlSrvDatabase()
	{
	SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, CSQLSRVDATABASE_CSQLSRVDATABASE2_ENTRY, "Entry;0x%X;CSqlSrvDatabase::~CSqlSrvDatabase;sqlite3*=0x%X", (TUint)this, (TUint)iDbHandle));
	TSqlCompactDbMapIterator compactDbIt(iCompactDbMap);
	TSqlCompactDbPair compactDbPair;
	while(compactDbIt.Next(compactDbPair))
		{
	   __ASSERT_DEBUG(compactDbPair.iData, __SQLPANIC2(ESqlPanicInvalidObj));
		::SqlServer().Compactor().ReleaseEntry(*compactDbPair.iData);
		}
	iCompactDbMap.Close();
	//If iSecureDbName is not NULL, the current CSqlSrvDatabase object operates on a secure database.
	//The database security policy has to be removed from the security policy map.
	//(The "remove" operation actually decrements the security policy reference counter and destroys the policy
	//when the counter reaches 0. The counter value may be greater than 1 if the database is shared between
	//more than one connection)
	if(iSecureDbName)
		{
		::SqlServer().SecurityMap().Remove(iSecureDbName);
		//The security policy map owns iSecureDbName and iSecurityPolicy and is responsible for 
		//iSecureDbName and iSecurityPolicy destruction.
		}
    //The next step of the "resource release" process is to walk over iAttachDbMap entries, get the data part of
    //the found TSqlAttachDbPair objects, which is secure database name used as a key in iSecurityMap, and remove
    //the related entry from iSecurityMap. If the database is closed in normal circumstances, the iAttachDbMap
    //has no entries. But if the database client forgets to detach the used attached databases or if the Detach() call
    //fails (for example, with KErrNoMemory error), then at this point iAttachDbMap has one or more entries.
    //That means there are still some attached databases to this connection. This is not a problem, SQLite will take
    //care of them. The problem is that there are related entries in iSecurityMap map, owned by CSqlServer object,
    //and they won't be removed from the map till CSqlServer object is alive. This causes problems in OOM tests and in 
    //real life of the device.
    //For example, one database client opens "c:[11111111]a.db" and attaches "c:[11111111]b.db":
    // - c:[11111111]a.db database has been opened successfully. iSecurityMap has 1 entry:
    //            {"c:[11111111]a.db", <database security policy object>}.
    // - c:[11111111]b.db is attached to c:[11111111]a.db with name "db2". There will be 1 entry in iAttachDbMap:
    //            {"db2", "c:[11111111]a.db"} 
    //        and a new entry will be added to iSecurityMap:
    //            {"c:[11111111]b.db", <database security policy object>}. 2 entries in total in iSecurityMap.
    // - The database client attempts to detach the attached database but the opertaion fails during the execution
    //        of the DETACH sql statement. Both maps are intact.
    // - The database client attempts to close the database. The previous implementation of CSqlSrvDatabase::~CSqlSrvDatabase()
    //        would only remove "c:[11111111]a.db" entry from iSecurityMap and close the iAttachDbMap map.
    // The result: no opened or attached databases but there will be one entry in iSecurityMap: "c:[11111111]b.db".
    // OOM tests will report a memory leak in this case. On a real device, if "c:[11111111]b.db" gets deleted and
    // recreated again, unexpected memory leak will occur in CSqlSrvDatabase::ConstructCreateSecureL() because
    // the code there "expects" that is the first time when a "c:[11111111]b.db" entry is added to iSecurityMap.
    TSqlAttachDbMapIterator it(iAttachDbMap);
    TSqlAttachDbPair attachDbPair;
    while(it.Next(attachDbPair))
        {
        __ASSERT_DEBUG(attachDbPair.iData, __SQLPANIC2(ESqlPanicInvalidObj));
       ::SqlServer().SecurityMap().Remove(attachDbPair.iData);
        }
	iAttachDbMap.Close();
	::CloseDbHandle(iDbHandle);
	SQL_TRACE_INTERNALS(OstTrace1(TRACE_INTERNALS, CSQLSRVDATABASE_CSQLSRVDATABASE2_EXIT, "Exit;0x%X;CSqlSrvDatabase::~CSqlSrvDatabase", (TUint)this));
	}

/**
Initializes CSqlSrvDatabase data memebers with their default values.


@see MSqlPolicyInspector
@see RSqlSecurityMap
@see CSqlServer
*/
CSqlSrvDatabase::CSqlSrvDatabase() :
	iAttachDbMap(TSqlAttachDbLinearOrder(&Compare), TSqlAttachDbDestructor()),
	iCompactDbMap(TSqlCompactDbLinearOrder(&Compare2), TSqlCompactDbDestructor())
	{
	}
	
/**
Creates a new SQL database file and executes config pragmas.
This function is part of CSqlSrvDatabase instance initialization.
If the function leaves and the error is not KErrAlreadyExists, the database file will be deleted.

@param aFileData A reference to a TSqlSrvFileData instance, which keeps the database name, a
     			 file session reference and some other database file related properties.
				 If this is a secure database, then the format of the name must be:
				 \<drive\>:\<[SID]database file name excluding the path\>.
				 If this is a non-secure database, then the file name has to be the full database file name.
				 "[SID]" refers to SID of the application which creates the database.

@see TSqlSrvFileData

@leave KErrNoMemory, an out of memory condition has occurred;
       KErrAlreadyExists, the file already exists.
                      Note that the function may also leave with some other database specific 
                      errors categorised as ESqlDbError, and other system-wide error codes.
*/
void CSqlSrvDatabase::CreateNewDbFileL(const TSqlSrvFileData& aFileData)
	{
	__SQLTRACE_INTERNALSVAR(TPtrC fname = aFileData.FileName());
	SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, CSQLSRVDATABASE_CREATENEWDBFILEL, "0x%x;CSqlSrvDatabase::CreateNewDbFileL;fname=%S", (TUint)this, __SQLPRNSTR(fname)));
	if(::FileExists(aFileData.Fs(), aFileData.FileName()))
		{
		__SQLLEAVE(KErrAlreadyExists);	
		}		
	TDbFileCleanup dbFileCleanup(aFileData, iDbHandle);
	CleanupStack::PushL(TCleanupItem(&DbFileCleanup, &dbFileCleanup));
	::CreateDbHandleL(aFileData, iDbHandle);
	SetConfigL(aFileData.ConfigParams(), ETrue);
	CleanupStack::Pop();//DbFileCleanup
	}
	
/**
Opens an existing SQL database file and executes config pragmas.

This function is part of CSqlSrvDatabase instance initialization.

@param aFileData A reference to a TSqlSrvFileData instance, which keeps the database name, a
     			 file session reference and some other database file related properties.
				 If this is a secure database, then the format of the name must be:
				 \<drive\>:\<[SID]database file name excluding the path\>.
				 If this is a non-secure database, then the file name has to be the full database file name.
				 "[SID]" refers to SID of the application which creates the database.
				 If this is application's private database, then the format of aFileData is as it is described
				 in TSqlSrvFileData::SetFromHandleL() comments.
				
@leave KErrNoMemory, an out of memory condition has occurred;
       KErrNotFound, SQL database file not found.
                      Note that the function may also leave with some other database specific 
                      errors categorised as ESqlDbError, and other system-wide error codes.

@see TSqlSrvFileData
@see TSqlSrvFileData::SetFromHandleL()
*/
void CSqlSrvDatabase::OpenExistingDbFileL(const TSqlSrvFileData& aFileData)
	{
	__SQLTRACE_INTERNALSVAR(TPtrC fname = aFileData.FileName());
	SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, CSQLSRVDATABASE_OPENEXISTINGDBFILEL, "0x%x;CSqlSrvDatabase::OpenExistingDbFileL;fname=%S", (TUint)this, __SQLPRNSTR(fname)));
	if(!aFileData.ContainHandles())
		{//This check is valid only if the database is outside application's private data cage
		if(!::FileExists(aFileData.Fs(), aFileData.FileName()))
			{
			__SQLLEAVE(KErrNotFound);	
			}
		}
	::CreateDbHandleL(aFileData, iDbHandle);
	//this is an existing database, only the cache size can be changed, the page size is persistent database property.
	//But for private databases, opened on the client side - the page size can be set (for the "create database" operations).
	SetConfigL(aFileData.ConfigParams(), aFileData.ContainHandles());
	}
	
/**
Installs an authorizer callback function.

The callback function is invoked by the SQL parser at SQL statement compile time for each attempt 
to access a column of a table in the database and is used to assert the calling application's rights to
perform specific SQL operation.

This function is part of CSqlSrvDatabase instance initialization.

@leave The function may leave with some database specific errors categorised as ESqlDbError.
*/
void CSqlSrvDatabase::InstallAuthorizerL()
	{
	//Install the authorizer just once. "Install authorizer" may be expensive and dangerous operation because 
	//it will expire the already prepared statements.
	if(!iAuthorizerInstalled)
		{
		(void)sqlite3SymbianLastOsError();//clear last OS error
		TInt err = sqlite3_set_authorizer(iDbHandle, &CSqlSrvDatabase::AuthorizeCallback, this);
		__SQLLEAVE_IF_ERROR(::Sql2OsErrCode(err, sqlite3SymbianLastOsError()));
		}
	iAuthorizerInstalled = ETrue;
	}

#ifdef SYMBIAN_USE_SQLITE_VERSION_3_6_4
extern "C" int sqlite3RegisterInternalUtf8Like(sqlite3 *db);
#endif
/**
Installs user-defined functions. At the moment there is only one: LIKE.

Replacing the LIKE operator default implementation with user defined LIKE functions.
The default implementation need a replacement because it does not work correctly with UTF16 encoded strings.
*/
void CSqlSrvDatabase::InstallUDFsL()
	{
	//Registering user defined LIKE function with 2 parameters for UTF16 encoding
	TInt err = sqlite3_create_function(iDbHandle, reinterpret_cast <const char*> (KStrLikeFuncName().Ptr()),
									   2/*arg*/, SQLITE_UTF16, this /*user data*/, 
									   &CSqlSrvDatabase::LikeSqlFunc, NULL/*xStep*/, NULL/*xFinal*/);
	__SQLLEAVE_IF_ERROR(::Sql2OsErrCode(err, sqlite3SymbianLastOsError()));
	//Registering user defined LIKE function with 3 parameters for UTF16 encoding 
	err = sqlite3_create_function(iDbHandle, reinterpret_cast <const char*> (KStrLikeFuncName().Ptr()),
								  3/*arg*/, SQLITE_UTF16, this/*user data*/, 
								  &CSqlSrvDatabase::LikeSqlFunc, NULL/*xStep*/, NULL/*xFinal*/);
	__SQLLEAVE_IF_ERROR(::Sql2OsErrCode(err, sqlite3SymbianLastOsError()));
	
#ifdef SYMBIAN_USE_SQLITE_VERSION_3_6_4
	//Registering user defined LIKE function with 2 and 3 parameters for UTF8 encoding
	//Once registered, these will be treated as built-in functions 
	err = sqlite3RegisterInternalUtf8Like(iDbHandle);
	__SQLLEAVE_IF_ERROR(::Sql2OsErrCode(err, sqlite3SymbianLastOsError()));
#endif
	}

/**
Constructs a key for searching in the security policies map.

The key is UTF8 encoded, zero-terminated string.

Every time when CSqlSrvDatabase instance creates, opens or attaches a secure database, it updates
the contents of the security policies map (RSqlSecurityMap class), which is a single instance owned 
by the CSqlServer class.

@param aDbFileName Full secure database file name, from which the security policies map key 
				   will be constructed.

@return A const pointer to the constructed security map key. No memory is allocated for the key.

@leave KErrGeneral the UTF16 to UTF8 conversion of aDbFileName parameter failed.

@see RSqlSecurityMap
@see CSqlServer
*/
const TUint8* CSqlSrvDatabase::SecurityMapKeyL(const TDesC& aDbFileName)
	{
	//Form the map item key - the secure database name
	TParsePtrC parse(aDbFileName);//this call may panic if aDbFileName cannot be parsed. But aDbFileName was preprocessed by TSqlSrvFileData::Set
	TFileName secureDbName;
	secureDbName.Copy(parse.Drive());
	secureDbName.Append(parse.NameAndExt());
	secureDbName.Append(TChar(0));
	TPtr8 ptr(iFileNameBuf, sizeof(iFileNameBuf));
	if(!::UTF16ZToUTF8Z(secureDbName, ptr))
		{
		__SQLLEAVE(KErrGeneral);			
		}
	return iFileNameBuf;
	}

/**
Attaches a secure or non-secure database to the current database connection.

@param aFileData A reference to a TSqlSrvFileData instance, which keeps the database name, a
     			 file session reference and some other database file related properties.
				 If this is a secure database, then the format of the name must be:
				 \<drive\>:\<[SID]database file name excluding the path\>.
				 If this is a non-secure database, then the file name has to be the full database file name.
				 "[SID]" refers to SID of the application which creates the database.
@param aDbName Database name. It must be unique (per database connection). This is the name which can
               be used for accessing tables in the attached database. The syntax is "database-name.table-name".

@leave KErrNotFound, the database file which has to be attached does not exist.
       KErrPermissionDenied, the client application does not satisfy the relevant security policies of
       				  the attached database.
                      Note that the function may also leave with some other database specific 
                      errors categorised as ESqlDbError, and other system-wide error codes.

@see TSqlSrvFileData
*/
void CSqlSrvDatabase::AttachDbL(const TSqlSrvFileData& aFileData, const TDesC& aDbName)
	{
	SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, CSQLSRVDATABASE_ATTACHDBL_ENTRY, "Entry;0x%X;CSqlSrvDatabase::AttachDbL;aDbName=%S", (TUint)this, __SQLPRNSTR(aDbName)));
	if(!aFileData.ContainHandles())
		{//This check is valid only if the database is outside application's private data cage
		if(!::FileExists(aFileData.Fs(), aFileData.FileName()))
			{
			__SQLLEAVE(KErrNotFound);	
			}
		}
	if(!aFileData.IsSecureFileNameFmt())		
		{//A non-secure database to be attached -  execute the "ATTACH DB" SQL statement and initialize the attached database.
		InitAttachedDbL(aFileData, aDbName);
		}
	else
		{//A secure database has to be attached. This is a complex operation and if it fails, proper cleanup
		 //has to be performed. "state" variable keeps the state, after which the "attach db" operation failed.
		 //There are three things needed to be done when atatching a secure database:
		 // 1. Executing the "ATTACH DB" SQL statement and initializing the attached database
		 // 2. Updating the security policy map
		 // 3. Updating the {db name, logical db name} map
		 //The additional map (3) is needed because when the authorizer callback is called by SQLITE, the callback
		 //is given the logical database name, if that's an operation on an attached database. Since the security 
		 //policy map uses the database name as a key, the map (3) is used to find what is the physical database 
		 //name, which the logical database name points to.
		 //
		 //There is an additional step (0), which may happen when a secure database is attached to a 
		 //non-secure database. But this step does not require a cleanup.
		CSqlSrvDatabase::TAttachState state = CSqlSrvDatabase::EAStNone;
		const TUint8* securityMapKey = NULL;
		TRAPD(err, DoAttachSecureDbL(state, aFileData, aDbName, securityMapKey));
		if(err != KErrNone)
			{
			//Cleanup
			if(state > CSqlSrvDatabase::EAStNone)
				{
				(void)FinalizeAttachedDb(aDbName);
				if(state > CSqlSrvDatabase::EAStDbAttached)
					{
					::SqlServer().SecurityMap().Remove(securityMapKey);
					}
				}
			__SQLLEAVE(err);
			}
		}
	SQL_TRACE_INTERNALS(OstTrace1(TRACE_INTERNALS, CSQLSRVDATABASE_ATTACHDBL_EXIT, "Exit;0x%X;CSqlSrvDatabase::AttachDbL", (TUint)this));
	}

/**
Attaches a secure database to the existing connection.

The function also updates the following maps:
- Security policies map (CSqlServer::iSecurityMap), where a reference counted copy of database security policies
  is kept for each created/opened/attached database during the life time of the SQL server;
- Attached secure databases map (CSqlSrvDatabase::iAttachDbMap), where an information is kept what SQL database
  file name corresponds to a specific attached database name. This information is used by the authorizer callback
  function in order to find what database security policies have to be asserted when a SQL operation is issued
  for a particular attached database (the attached database name is identified always by its name, not the file name);

@param aState Output parameter - the attach progress state, used by the calling function to perform correctly 
			  the cleanup, if the attach operation fails.
			  It may have the following values set:
			  - CSqlSrvDatabase::EAStNone - no resources need to be freed;
			  - CSqlSrvDatabase::EAStDbAttached - the function was able to execute the "ATTACH DATABASE"
			      SQL statement before an error occured. The calling function has to execute "DETACH DATABASE"
			      SQL statement to detach the database;
			  - CSqlSrvDatabase::EAStSecurityMapUpdated - the function was able to update the security policies
			      map (CSqlServer::iSecurityMap) before an error occured. The calling function has to remove
			      the attached database security policies from the map and detach the database.
@param aFileData A reference to a TSqlSrvFileData instance, which keeps the database name, a
     			 file session reference and some other database file related properties.
				 The secure database name format must be:
				 \<drive\>:\<[SID]database file name excluding the path\>.
				 "[SID]" refers to SID of the application which creates the database.
@param aDbName Database name. It must be unique (per database connection). This is the name which can
               be used for accessing tables in the attached database. The syntax is "database-name.table-name".
@param aMapKey Output parameter, UTF8 encoded, zero-terminated string, which can be used for searching 
			   of the attached database security policies in the security policies map (CSqlServer::iSecurityMap). 
			   No memory is allocated for the key.

@see RSqlSecurityMap
@see RSqlAttachDbMap
@see CSqlServer
@see TSqlSrvFileData
@see CSqlSrvDatabase

@leave KErrNoMemory, an out of memory condition has occurred;
       KErrPermissionDenied, the client application does not satisfy the relevant security policies of
       				  the attached database.
                      Note that the function may also leave with some other database specific 
                      errors categorised as ESqlDbError, and other system-wide error codes.
*/
void CSqlSrvDatabase::DoAttachSecureDbL(CSqlSrvDatabase::TAttachState& aState, 
										const TSqlSrvFileData& aFileData, 
										const TDesC& aDbName, const TUint8*& aMapKey)
	{
	//Step 1: Attach the database and initialize the attached database
	InitAttachedDbL(aFileData, aDbName);
	aState = CSqlSrvDatabase::EAStDbAttached;
	//Step 2: Load the database security policy and update the security map
	const CSqlSecurityPolicy* securityPolicy = NULL;
	UpdateSecurityMapL(ETrue, aFileData, aMapKey, securityPolicy);
	aState = CSqlSrvDatabase::EAStSecurityMapUpdated;
	//Check that the caller has at least one of {Schema, Read, Write} policies.
	BasicSecurityPolicyCheckL(*securityPolicy);
	//Step 3: Update the attached databases map
	InsertInAttachDbMapL(aFileData.FileName(), aDbName);
	}
	
/**
Detaches a database from the current database connection.

The function also will search the security policies map (CSqlServer::iSecurityMap) and the attached databases
map (CSqlSrvDatabase::iAttachDbMap) for entries which correspond to the database ,
which is about to be detached, and will remove the entries.

@param aDbName Attached database name. It must be unique (per database connection).

@leave The function may leave with some database specific errors categorised as ESqlDbError, 
	   and other system-wide error codes.
	   
@see CSqlSrvDatabase::DoAttachSecureDbL()
@see RSqlSecurityMap
@see RSqlAttachDbMap
@see CSqlServer
@see CSqlSrvDatabase
*/
void CSqlSrvDatabase::DetachDbL(const TDesC& aDbName)
	{
	SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, CSQLSRVDATABASE_DETACHDBL_ENTRY, "Entry;0x%X;CSqlSrvDatabase::DetachDbL;aDbName=%S", (TUint)this, __SQLPRNSTR(aDbName)));
	TInt err = FinalizeAttachedDb(aDbName);
	if(err == KErrNone)
		{
		TRAP_IGNORE(RemoveFromMapsL(aDbName));
		}
	else
		{
		__SQLLEAVE(err);
		}
	SQL_TRACE_INTERNALS(OstTrace1(TRACE_INTERNALS, CSQLSRVDATABASE_DETACHDBL_EXIT, "Exit;0x%X;CSqlSrvDatabase::DetachDbL", (TUint)this));
	}

/**
Calculates and returns the database size. 

@param aDbName Attached database name or KNullDesC for the main database

@leave The function may leave with some database specific errors categorised as ESqlDbError, 
	   and other system-wide error codes.

@return Database size in bytes.
*/
TInt64 CSqlSrvDatabase::SizeL(const TDesC& aDbName)
	{
	iAuthorizerDisabled	= ETrue;
	CleanupStack::PushL(TCleanupItem(&EnableAuthorizer, &iAuthorizerDisabled));
	TInt pageCount = 0;
	__SQLLEAVE_IF_ERROR(::DbPageCount(iDbHandle, aDbName, pageCount));
	__ASSERT_DEBUG(pageCount >= 0, __SQLPANIC(ESqlPanicInternalError));
	CleanupStack::PopAndDestroy();
	return (TInt64)pageCount * PageSizeL(aDbName);
	}

/**
Retrieves the database free space. 

@param aDbName Attached database name or KNullDesC for the main database

@return Database free space in bytes.

@leave The function may leave with some database specific errors categorised as ESqlDbError, 
	   and other system-wide error codes.
*/
TInt64 CSqlSrvDatabase::FreeSpaceL(const TDesC& aDbName)
	{
	iAuthorizerDisabled	= ETrue;
	CleanupStack::PushL(TCleanupItem(&EnableAuthorizer, &iAuthorizerDisabled));
	TInt freePageCount = 0;
	__SQLLEAVE_IF_ERROR(::DbFreePageCount(iDbHandle, aDbName, freePageCount));
	CleanupStack::PopAndDestroy();
	__ASSERT_DEBUG(freePageCount >= 0, __SQLPANIC(ESqlPanicInternalError));
	return (TInt64)freePageCount * PageSizeL(aDbName);
	}

/**
Collects information regarding the current cache size, page, size, encoding, etc. and puts the values
in the aDest output string.

@param aDest Output parameter, where the result values will be stored. The string format is: "<val1>;<val2>...;".

@leave The function may leave with some database specific errors categorised as ESqlDbError, 
	   and other system-wide error codes.
*/
void CSqlSrvDatabase::QueryConfigL(TDes8& aDest)
	{
	iAuthorizerDisabled	= ETrue;
	CleanupStack::PushL(TCleanupItem(&EnableAuthorizer, &iAuthorizerDisabled));

	TInt cacheSize = 0;
	__SQLLEAVE_IF_ERROR(::DbCacheSize(iDbHandle, KNullDesC, cacheSize));
	
	TInt pageSize = 0;
	__SQLLEAVE_IF_ERROR(::DbPageSize(iDbHandle, KNullDesC, pageSize));

	TBuf8<16> encoding;
	__SQLLEAVE_IF_ERROR(::DbEncoding(iDbHandle, KNullDesC, encoding));

	TInt defaultSoftHeapLimit = TSqlSrvConfigParams::KDefaultSoftHeapLimitKb;

	TInt vacuum = 0;
	__SQLLEAVE_IF_ERROR(::DbVacuumMode(iDbHandle, KNullDesC, vacuum));

	aDest.AppendNum(cacheSize);	
	_LIT8(KSemiColon, ";");
	aDest.Append(KSemiColon);	
	aDest.AppendNum(pageSize);	
	aDest.Append(KSemiColon);	
	aDest.Append(encoding);	
	aDest.Append(KSemiColon);	
	aDest.AppendNum(defaultSoftHeapLimit);	
	aDest.Append(KSemiColon);	
	aDest.AppendNum(vacuum);	
	aDest.Append(KSemiColon);	
	
	CleanupStack::PopAndDestroy();
	}

/**
Compacts the database free space. 

@param aSize The requested database space to be compacted, in bytes.
			 If aSize value is RSqlDatabase::EMaxCompaction, then all free pages will be removed.
			 Note that the requested space to be compacted will be rounded up to the nearest page count,
			 e.g. request for removing 1 byte will remove one free page from the database file. 

@param aDbName Attached database name or KNullDesC for the main database

@return The size of the removed free space

@leave The function may leave with some database specific errors categorised as ESqlDbError, 
	   and other system-wide error codes.
*/
TInt CSqlSrvDatabase::CompactL(TInt aSize, const TDesC& aDbName)
	{
	SQL_TRACE_INTERNALS(OstTraceExt3(TRACE_INTERNALS, CSQLSRVDATABASE_COMPACTL_ENTRY, "Entry;0x%X;CSqlSrvDatabase::CompactL;aSize=%d;aDbName=%S", (TUint)this, aSize, __SQLPRNSTR(aDbName)));
	__ASSERT_DEBUG(aSize > 0 || aSize == RSqlDatabase::EMaxCompaction, __SQLPANIC(ESqlPanicBadArgument));
	TInt pageSize = PageSizeL(aDbName);//PageSizeL() will disable/enable the authorizer
	TInt pageCount = KMaxTInt;
	if(aSize > 0)
		{//64-bit calculations to avoid the overflow in case if (aSize + pageSize) >= KMaxTInt.
		pageCount = (TInt64)((TInt64)aSize + pageSize - 1) / pageSize;
		}
	if(pageCount > 0)
		{
		iAuthorizerDisabled	= ETrue;
		CleanupStack::PushL(TCleanupItem(&EnableAuthorizer, &iAuthorizerDisabled));
		__SQLLEAVE_IF_ERROR(::DbCompact(iDbHandle, aDbName, pageCount, pageCount));
		CleanupStack::PopAndDestroy();
		}
	SQL_TRACE_INTERNALS(OstTraceExt3(TRACE_INTERNALS, CSQLSRVDATABASE_COMPACTL_EXIT, "Exit;0x%X;CSqlSrvDatabase::CompactL;pageCount=%d;pageSize=%d", (TUint)this, pageCount, pageSize));
	return pageCount * pageSize;
	}

/**
This structure is used in case if the InitAttachedDbL() execution fails and the 
executed operations ("attach database", "init compact") have to be reverted calling
FinalizeAttachedDb().

@see CSqlSrvDatabase::InitAttachedDbL
@see CSqlSrvDatabase::FinalizeAttachedDb
@see CSqlSrvDatabase::AttachCleanup

@internalComponent
*/
NONSHARABLE_STRUCT(TAttachCleanup)
	{
	inline TAttachCleanup(CSqlSrvDatabase& aSelf, const TDesC& aDbName) :
		iSelf(aSelf),
		iDbName(aDbName)
		{
		}
	CSqlSrvDatabase& iSelf;
	const TDesC& iDbName;
	};

/**
Cleanup function. Calls FinalizeAttachedDb() if InitAttachedDbL() has failed.

@param aCleanup A pointer to a TAttachCleanup object that contains the needed for the cleanup information.

@see TAttachCleanup

@internalComponent
*/
void CSqlSrvDatabase::AttachCleanup(void* aCleanup)
	{
	TAttachCleanup* cleanup = reinterpret_cast <TAttachCleanup*> (aCleanup);
	__ASSERT_DEBUG(cleanup != NULL, __SQLPANIC2(ESqlPanicBadArgument));
	(void)cleanup->iSelf.FinalizeAttachedDb(cleanup->iDbName);
	}

/**
Forms and executes "ATTACH DATABASE" SQL statement.
If the database is not read-only:
 - Makes the attached database journal file persistent.
 - Initializes the attached database compaction mode.
 - The attached database will be reindexed if the default collation has been changed.

@param aFileData Attached database file data
@param aDbName Attached database name. It must be unique (per database connection).

@leave KErrNoMemory, an out of memory condition has occurred;
                      Note that the function may also leave with some other database specific 
                      errors categorised as ESqlDbError, and other system-wide error codes.
                      
@see InitCompactionL
@see ProcessSettingsL
*/
void CSqlSrvDatabase::InitAttachedDbL(const TSqlSrvFileData& aFileData, const TDesC& aDbName)
	{
	TPtrC dbFileName = aFileData.FileName();
	TBool readOnlyDbFile = aFileData.IsReadOnly();
	
	InstallAuthorizerL();
	iAuthorizerDisabled	= ETrue;
	CleanupStack::PushL(TCleanupItem(&EnableAuthorizer, &iAuthorizerDisabled));
	sqlite3_stmt* stmtHandle = StmtPrepare16L(iDbHandle, KAttachDb);
	TInt err = ::Sql2OsErrCode(sqlite3_bind_text16(stmtHandle, 1, dbFileName.Ptr(), dbFileName.Length() * sizeof(TUint16), SQLITE_STATIC), sqlite3SymbianLastOsError());
	if(err == KErrNone)
		{
		err = ::Sql2OsErrCode(sqlite3_bind_text16(stmtHandle, 2, aDbName.Ptr(), aDbName.Length() * sizeof(TUint16), SQLITE_STATIC), sqlite3SymbianLastOsError());
		if(err == KErrNone)
			{
			err = StmtExec(stmtHandle);
			}
		}
	(void)::FinalizeStmtHandle(stmtHandle);//sqlite3_finalize() fails only if an invalid statement handle is passed.
	CleanupStack::PopAndDestroy();//TCleanupItem(&EnableAuthorizer, &iAuthorizerDisabled)
	__SQLLEAVE_IF_ERROR(err);
	
	TAttachCleanup attachCleanup(*this, aDbName);
	CleanupStack::PushL(TCleanupItem(&CSqlSrvDatabase::AttachCleanup, &attachCleanup));
	
	SetConfigL(aFileData.ConfigParams(), EFalse, aDbName);
	
	if(!readOnlyDbFile)
		{
		iAuthorizerDisabled	= ETrue;
		CleanupStack::PushL(TCleanupItem(&EnableAuthorizer, &iAuthorizerDisabled));
		ProcessSettingsL(aFileData, aDbName);
		CleanupStack::PopAndDestroy();//TCleanupItem(&EnableAuthorizer, &iAuthorizerDisabled)
		}
	
	CleanupStack::Pop();//TCleanupItem(&CSqlSrvDatabase::AttachCleanup, &attachCleanup)
	}

/**
Forms and executes "DETACH DATABASE" SQL statement.
If the database was scheduled for background compacting, the related database entry will be removed from
the vaccum db map.

@param aDbName Attached database name. It must be unique (per database connection).

@return KErrNone, Operation completed successfully;
		KErrNoMemory, an out of memory condition has occurred.
                      Note that the function may also return some other database specific 
                      errors categorised as ESqlDbError, and other system-wide error codes.
*/
TInt CSqlSrvDatabase::FinalizeAttachedDb(const TDesC& aDbName)
	{
	ReleaseCompactEntry(aDbName);
	iAuthorizerDisabled	= ETrue;
	sqlite3_stmt* stmtHandle = NULL;
	TRAPD(err, stmtHandle = StmtPrepare16L(iDbHandle, KDetachDb));
	if(err == KErrNone)
		{
		err = ::Sql2OsErrCode(sqlite3_bind_text16(stmtHandle, 1, aDbName.Ptr(), aDbName.Length() * sizeof(TUint16), SQLITE_STATIC), sqlite3SymbianLastOsError());
		if(err == KErrNone)
			{
			err = StmtExec(stmtHandle);
			}
		}
	(void)::FinalizeStmtHandle(stmtHandle);//sqlite3_finalize() fails only if an invalid statement handle is passed.
	iAuthorizerDisabled	= EFalse;
	return err;
	}

/**
Updates the security policies map (CSqlServer::iSecurityMap).

Inserts a new item in the security map, or if such item already exists there - its reference counter will
be incremented.
The method guarantees that either the security map will be updated, or the method leaves and the security
policies map stays unchanged.

@param aAttachedDb True if the currently processed database is an attached database, false if it is the main db.
@param aFileData A reference to a TSqlSrvFileData instance, which keeps the database name, a
     			 file session reference and some other database file related properties.
				 The secure database name format must be:
				 \<drive\>:\<[SID]database file name excluding the path\>.
				 "[SID]" refers to SID of the application which creates the database.
@param aMapKey Output parameter. UTF8 encoded, zero-terminated string. On function exit cannot be NULL. 
               It will be set to point to the security policies map key. No memory is allocated for the key.
@param aSecurityPolicy Output parameter. On function exit cannot be NULL. It will be set to point to the security policies.

@leave KErrNoMemory, an out of memory condition has occurred;
                      Note that the function may also leave with some other database specific 
                      errors categorised as ESqlDbError, and other system-wide error codes.

@see RSqlSecurityMap
@see CSqlServer
@see TSqlSrvFileData
@see CSqlSecurityPolicy
*/
void CSqlSrvDatabase::UpdateSecurityMapL(TBool aAttachedDb, const TSqlSrvFileData& aFileData, 
										 const TUint8*& aMapKey, const CSqlSecurityPolicy*& aSecurityPolicy)
	{
	//Check if a copy of the database security policies is already in the security map.
	aMapKey = SecurityMapKeyL(aFileData.FileName());
	TSqlSecurityPair* pair = ::SqlServer().SecurityMap().Entry(aMapKey);
	if(pair)
		{
		//Yes, it is in the map. Increment the reference counter.
		//(It will be decremented when detaching the database).
		pair->iRefCounter.Increment();
		aMapKey = pair->iKey;
		aSecurityPolicy = pair->iData;
		}
	else
		{
		//No, it is not in the map. Read the security policies from the security policies tables and 
		//insert a new item in the map.
		__ASSERT_DEBUG(aMapKey != NULL, __SQLPANIC(ESqlPanicInternalError));
		aMapKey = ::CreateStrCopyLC(aMapKey);
		CSqlSecurityPolicy* securityPolicy = aAttachedDb ? ::LoadAttachedDbSecurityPolicyLC(aFileData) :
		                                                   ::LoadDbSecurityPolicyLC(iDbHandle);
	    __ASSERT_DEBUG(!::SqlServer().SecurityMap().Entry(aMapKey), __SQLPANIC2(ESqlPanicObjExists));
		__SQLLEAVE_IF_ERROR(::SqlServer().SecurityMap().Insert(aMapKey, securityPolicy));
		CleanupStack::Pop(2);//iSecurityMap owns aMapKey and securityPolicy objects
		aSecurityPolicy = securityPolicy;
		}
	__ASSERT_DEBUG(aMapKey != NULL, __SQLPANIC(ESqlPanicInternalError));
	__ASSERT_DEBUG(aSecurityPolicy != NULL, __SQLPANIC(ESqlPanicInternalError));
	}

/**
Removes attached secure database entries from the maps.

The function will search the security policies map (CSqlServer::iSecurityMap) and the attached databases
map (CSqlSrvDatabase::iAttachDbMap) for entries which correspond to the database with aDbName name,
and will remove the entries.

The sequence of the performed by the function operations is:
1. Looks for a map item which key is aDbName in iAttachDbMap map.
2. If such pair exists, the data part of the pair is used as a key in iSecurityMap map.
3. Remove the iSecurityMap map item pointed by the data part of the found pair.
4. Remove the iAttachDbMap map item pointed by aDbName.

@param aDbName Attached database name. It must be unique (per database connection).

@leave KErrGeneral It is not possible to convert aDbName parameter to UTF8 encoded string.

@see CSqlSrvDatabase::DoAttachDbL()
@see CSqlSrvDatabase::DoAttachSecureDbL()
@see RSqlSecurityMap
@see RSqlAttachDbMap
@see CSqlServer
@see CSqlSrvDatabase
*/
void CSqlSrvDatabase::RemoveFromMapsL(const TDesC& aDbName)
	{
	SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, CSQLSRVDATABASE_REMOVEFROMMAPSL, "0x%X;CSqlSrvDatabase::RemoveFromMapsL;aDbName=%S", (TUint)this, __SQLPRNSTR(aDbName)));
	TPtr8 ptr(iFileNameBuf, sizeof(iFileNameBuf));
	if(!::UTF16ToUTF8Z(aDbName, ptr))
		{
		__SQLLEAVE(KErrGeneral);	
		}
	TSqlAttachDbPair* attachDbPair = iAttachDbMap.Entry(iFileNameBuf);
	if(attachDbPair)
		{//aDbName refers to attached secure database
		::SqlServer().SecurityMap().Remove(attachDbPair->iData);
		iAttachDbMap.Remove(iFileNameBuf);
		}
	}

/**
Inserts a new entry in the attached databases map (CSqlSrvDatabase::iAttachDbMap).

The method guarantees that either a new [logical db name, secure db name] pair will be inserted in
iAttachDbMap or the method fails and the content of iAttachDbMap remains unchanged.

@param aDbFileName Database file name, including the path.
@param aDbName Database name.

@leave KErrNoMemory, an out of memory condition has occurred;
	   KErrGeneral, it is not possible to convert the function parameters to UTF8 encoded strings.

@see RSqlAttachDbMap
@see CSqlSrvDatabase
*/
void CSqlSrvDatabase::InsertInAttachDbMapL(const TDesC& aDbFileName, const TDesC& aDbName)
	{
	SQL_TRACE_INTERNALS(OstTraceExt3(TRACE_INTERNALS, CSQLSRVDATABASE_INSERTINATTACHDBMAPL, "0x%X;CSqlSrvDatabase::InsertInAttachDbMapL;aDbFileName=%S;aDbName=%S", (TUint)this, __SQLPRNSTR(aDbFileName), __SQLPRNSTR(aDbName)));
	//Convert aDbName to UTF8, zero-terminated name
	TPtr8 ptr(iFileNameBuf, sizeof(iFileNameBuf));
	if(!::UTF16ToUTF8Z(aDbName, ptr))
		{
		__SQLLEAVE(KErrGeneral);	
		}
	const TUint8* mapKey = ::CreateStrCopyLC(iFileNameBuf);
	const TUint8* mapData = SecurityMapKeyL(aDbFileName);
	mapData = ::CreateStrCopyLC(mapData);
	__ASSERT_DEBUG(!iAttachDbMap.Entry(mapKey), __SQLPANIC(ESqlPanicObjExists));
	__SQLLEAVE_IF_ERROR(iAttachDbMap.Insert(mapKey, mapData));
	CleanupStack::Pop(2);//iAttachDbMap owns mapKey amd mapData.
	}

/**
Processes the database settings that are currently stored in the settings table.
Makes the journal file persistent.
Initializes the database compaction mode.
Based on the current settings the database may be configured to become 'up-to-date'.
This configuration may include reindexing the database if the collation dll has
changed and/or executing database configuration file operations.

@param aFileData The file data object,
@param aDbName Logical database name: "main" for the main database or attached database name,
				        
@leave The function may leave with system-wide error codes or SQL errors of ESqlDbError type

@panic SqlDb 7 In _DEBUG mode if aFileData does not refer to a r/w database file.
*/
void CSqlSrvDatabase::ProcessSettingsL(const TSqlSrvFileData& aFileData, const TDesC& aDbName)
	{
	__ASSERT_DEBUG(!aFileData.IsReadOnly(), __SQLPANIC(ESqlPanicInternalError));
#if !defined(__SQL_DISABLE_SYMBIAN_SETTINGS_TABLE__)
	//Make the journal file persistent - done by SQLite automatically because the locking mode is EXCLUSIVE
	//__SQLLEAVE_IF_ERROR(::ExecPragma(iDbHandle, iAuthorizerDisabled, KPersistentJournalPragma, KPersist, aDbName));
	//Load the current settings
	TFileName storedCollationDllName;
	TInt storedDbConfigFileVer = -1;
	TInt currVacuumMode = -1;
	__SQLLEAVE_IF_ERROR(::DbVacuumMode(iDbHandle, aDbName, currVacuumMode));
	//currVacuumMode == ESqliteVacuumOff ==> This is a database created not by the SQL server
	TSqlCompactionMode compactionMode = currVacuumMode == ESqliteVacuumOff ? ESqlCompactionManual : ESqlCompactionNotSet;
	TSqlDbSysSettings dbSettings(iDbHandle);
	dbSettings.LoadSettingsL(aDbName, storedCollationDllName, storedDbConfigFileVer, compactionMode);
	if(currVacuumMode == ESqliteVacuumOff && compactionMode != ESqlCompactionManual)
		{//The database has been opened and the vacuum mode is "off". Then this is a database, not created by SQL 
		 //server or it is a corrupted database. The compaction mode read from the system table does not match the 
		 //database vacuum mode. Conclusion: this is a corrupted database.
		__SQLLEAVE(KErrCorrupt);
		}
	if(aFileData.ContainHandles() && aFileData.IsCreated())
		{
		compactionMode = aFileData.ConfigParams().iCompactionMode;
		if(compactionMode == ESqlCompactionNotSet)
			{
			compactionMode = KSqlDefaultCompactionMode;
			}
		//This is a just created private database. Store the compaction mode (the compaction mode may have been set using a config string).
		StoreSettingsL(storedCollationDllName, storedDbConfigFileVer, compactionMode);
		}
	//Init the database compaction mode
	InitCompactionL(compactionMode, aFileData.ConfigParams().iFreePageThresholdKb, aFileData.FileName(), 
					(TSqliteVacuumMode)currVacuumMode, aDbName);
	//Based on the current settings, apply any necessary configuration updates to the database
	ApplyConfigUpdatesL(storedCollationDllName, storedDbConfigFileVer, aFileData, aDbName);	
#endif // !(__SQL_DISABLE_SYMBIAN_SETTINGS_TABLE__)			
	}
		
/**
Applies any necessary configuration updates to the database, based on the current settings 
in the settings table and how the database is being used (i.e. 'Opened' or 'Attached'). 
The applied configuration may include:
- Reindexing the main database and all attached databases, if the collation dll has been changed.
After the reindexation the new collation dll name will be stored in the settings table of the database.
- Executing all supported operations on the database that are specified in a database configuration 
file, if such a file exists and has not already been processed. 
The settings table will updated with the current version of the database configuration file if the file 
is processed.

@param aStoredCollationDllName The name of the collation dll that is stored in the settings table,
@param aStoredDbConfigFileVersion The database configuration file version that is stored in the settings table,
@param aFileData   The file data object,
@param aDbName Logical database name: "main" for the main database or attached database name,

@leave The function may leave with system-wide error codes or SQL errors of ESqlDbError type
*/
void CSqlSrvDatabase::ApplyConfigUpdatesL(const TDesC& aStoredCollationDllName, const TInt& aStoredDbConfigFileVersion, 
										  const TSqlSrvFileData& aFileData, const TDesC& aDbName)
	{
	TSqlDbSysSettings dbSettings(iDbHandle);
	//Check whether reindexing is necessary
	if(::SqlServer().CollationDllName().CompareF(aStoredCollationDllName) != 0)
		{
		SQL_TRACE_INTERNALS(OstTraceExt3(TRACE_INTERNALS, CSQLSRVDATABASE_APPLYCONFIGUPDATES2L, "0x%X;CSqlSrvDatabase::ApplyConfigUpdatesL;Reindex db;aStoredCollationDllName=%S;aDbName=%S", (TUint)this, __SQLPRNSTR(aStoredCollationDllName), __SQLPRNSTR(aDbName)));
		dbSettings.ReindexDatabaseL(aDbName, ::SqlServer().CollationDllName());
		}

	//Perform any necessary configuration file updates to the database.
	//We do not want failures here to cause the database to fail
	//to be opened and so any leave error is TRAPed and ignored
	//(the error is logged in _DEBUG mode)
	TRAPD(err, dbSettings.ConfigureDatabaseL(aStoredDbConfigFileVersion, aFileData, aDbName));
	if(KErrNone != err)
		{
		SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, CSQLSRVDATABASE_APPLYCONFIGUPDATESL, "0x%X;CSqlSrvDatabase::ApplyConfigUpdatesL;ConfigureDatabaseL() failed with error code %d", (TUint)this, err));	
		}
	}

/**
Sets the "cache size" and "page size" parameter values, if their values are valid.
This is done formatting and executing PRAGMA statements.
@param aConfigParams   This object contains the cofiguration parameters
@param aSetPageSize If true, the page size will be set, otherwise not.
                    The aSetPageSize is set to true if:
                    1) The operation is "create database"
                    2) The operation is "open private database"
@param aLogicalDbName Parameter with default value of KNullDesC. If aLogicalDbName length is not 0, then the 
                      "cache_size" pragma will be executed on the attached database with aLogicalDbName name. 
*/
void CSqlSrvDatabase::SetConfigL(const TSqlSrvConfigParams& aConfigParams, TBool aSetPageSize, const TDesC& aLogicalDbName)
	{
	__ASSERT_DEBUG(aConfigParams.iPageSize == TSqlSrvConfigParams::KConfigPrmValueNotSet || aConfigParams.iPageSize >= 0, __SQLPANIC(ESqlPanicBadArgument));
	__ASSERT_DEBUG(aConfigParams.iCacheSize == TSqlSrvConfigParams::KConfigPrmValueNotSet || aConfigParams.iCacheSize >= 0, __SQLPANIC(ESqlPanicBadArgument));
	if(aSetPageSize && aConfigParams.iPageSize != TSqlSrvConfigParams::KConfigPrmValueNotSet)
		{
		__SQLLEAVE_IF_ERROR(::ExecPragma(iDbHandle, iAuthorizerDisabled, KPageSizePragma, aConfigParams.iPageSize));
		}
	
	const TDesC& logicalDbName = aLogicalDbName.Length() > 0 ? aLogicalDbName : KMainDb16;
	
	::SetJournalSizeLimitL(iDbHandle, iAuthorizerDisabled, aConfigParams.iPageSize, logicalDbName);

	//Setting the cache size.
	//Step 1: Check if aConfigParams.iCacheSize value is set. If it is set, then use it.
	if(aConfigParams.iCacheSize != TSqlSrvConfigParams::KConfigPrmValueNotSet)
		{
		__SQLLEAVE_IF_ERROR(::ExecPragma(iDbHandle, iAuthorizerDisabled, KCacheSizePragma, aConfigParams.iCacheSize, logicalDbName));
		}
	else
		{
	//Step 2: aConfigParams.iCacheSize value is not set. Then check if aConfigParams.iSoftHeapLimitKb value is set.
		if(aConfigParams.iSoftHeapLimitKb != TSqlSrvConfigParams::KConfigPrmValueNotSet)
			{
			__ASSERT_DEBUG(aConfigParams.iSoftHeapLimitKb >= TSqlSrvConfigParams::KMinSoftHeapLimitKb && 
			            aConfigParams.iSoftHeapLimitKb <= TSqlSrvConfigParams::KMaxSoftHeapLimitKb, __SQLPANIC(ESqlPanicInternalError));
	//Step 3: aConfigParams.iSoftHeapLimitKb value is set. Then use it to calculate the cache size. But we need the page size first.
    //        aLogicalDbName is used instead of logicalDbName because PageSizeL() if called with non-zero length name, 
	//        "thinks" it is the main database name. KMainDb16 will be interpreted as an attached database name. 
		TInt pageSize = PageSizeL(aLogicalDbName);
	//Step 4: Calculate the cache size.
			TInt cacheSize = ((TInt64)aConfigParams.iSoftHeapLimitKb * 1024) / pageSize;//"TInt64" cast is used because of a possible overflow
	//Step 5: Set the cache size.
			__SQLLEAVE_IF_ERROR(::ExecPragma(iDbHandle, iAuthorizerDisabled, KCacheSizePragma, cacheSize, logicalDbName));
			}
		}
	}

/**
Initializes the database compaction mode.
If the aCompactionMode parameter is ESqlCompactionBackground, the database with aDbFileName file name will be added
to the compactor for background compacting.

@param aCompactionMode The database compaction mode. See TSqlCompactionMode enum for the supported database compaction modes.
@param aFreePageThresholdKb Free page threshold. The background compaction won't start if the free pages size in Kb is less than
						  the free page threshold.
@param aDbFileName Database file name including full path
@param aCurrentVacuumMode The current SQLite vacuum mode, one of TSqliteVacuumMode enum item values.
						  If the current database vacuum mode is ESqliteVacuumOff, which means 
						  the database has been created not by the SQL server, 
						  then the "off" vacuum mode is kept unchanged.
@param aDbName "main" or the attached database name

@leave KErrNoMemory, an out of memory condition has occurred;
                     Note that the function may also leave with some other database specific 
                     errors categorised as ESqlDbError, and other system-wide error codes.

@see TSqlCompactionMode
@see CSqlCompactor
@see TSqliteVacuumMode

@panic SqlDb 4 In _DEBUG mode if aCompactionMode parameter value is invalid.
*/
void CSqlSrvDatabase::InitCompactionL(TSqlCompactionMode aCompactionMode, TInt aFreePageThresholdKb, 
									  const TDesC& aDbFileName, TSqliteVacuumMode aCurrentVacuumMode, const TDesC& aDbName)
	{
	__ASSERT_DEBUG(aCompactionMode == ESqlCompactionManual || aCompactionMode == ESqlCompactionBackground || aCompactionMode == ESqlCompactionAuto, __SQLPANIC(ESqlPanicBadArgument));
	__ASSERT_DEBUG(aCurrentVacuumMode == ESqliteVacuumOff || aCurrentVacuumMode == ESqliteVacuumAuto || 
			    aCurrentVacuumMode == ESqliteVacuumIncremental, __SQLPANIC(ESqlPanicBadArgument));
	__ASSERT_DEBUG(aFreePageThresholdKb >= 0, __SQLPANIC(ESqlPanicBadArgument));
	TSqliteVacuumMode newSqliteVacuumMode = aCompactionMode == ESqlCompactionAuto ? ESqliteVacuumAuto : ESqliteVacuumIncremental;
	if(aCurrentVacuumMode == ESqliteVacuumOff)
		{
		newSqliteVacuumMode = ESqliteVacuumOff;
		}
	if(aCurrentVacuumMode != newSqliteVacuumMode)	
		{
		__SQLLEAVE_IF_ERROR(::ExecPragma(iDbHandle, iAuthorizerDisabled, KAutoVacuumPragma, newSqliteVacuumMode, aDbName));
		}
	if(aCompactionMode == ESqlCompactionBackground)
		{
		NewCompactEntryL(aFreePageThresholdKb, aDbFileName, aDbName);
		}
	}

/**
Adds the aDbFileName database to the compactor object for background compacting.

@param aFreePageThresholdKb Free page threshold in Kb. The background compaction won't start if the total size of the free pages
						  is less than the free page threshold.
@param aDbFileName Database file name including full path
@param aDbName "main" or the attached database name

@leave KErrNoMemory, an out of memory condition has occurred;
                     Note that the function may also leave with some other database specific 
                     errors categorised as ESqlDbError, and other system-wide error codes.

@see CSqlCompactor
@see RSqlCompactDbMap 
*/
void CSqlSrvDatabase::NewCompactEntryL(TInt aFreePageThresholdKb, const TDesC& aDbFileName, const TDesC& aDbName)
	{
	SQL_TRACE_INTERNALS(OstTraceExt4(TRACE_INTERNALS, CSQLSRVDATABASE_NEWCOMPACTENTRYL_ENTRY, "Entry;0x%X;CSqlSrvDatabase::NewCompactEntryL;aFreePageThresholdKb=%d;aDbFileName=%S;aDbName=%S", (TUint)this, aFreePageThresholdKb, __SQLPRNSTR(aDbFileName), __SQLPRNSTR(aDbName)));	
	TSqlCompactSettings settings;
	settings.iFreePageThresholdKb = aFreePageThresholdKb;
	::SqlServer().Compactor().AddEntryL(aDbFileName, settings);
	TInt err = KErrNoMemory;
	HBufC* key = aDbName.Alloc();
	HBufC* data = aDbFileName.Alloc();
	if(key && data)
		{
	    __ASSERT_DEBUG(!iCompactDbMap.Entry(key), __SQLPANIC(ESqlPanicObjExists));
		err = iCompactDbMap.Insert(key, data);//returns the index of the new entry
		}
	if(err < 0) //If either "key" or "data" or both is NULL, then "err" is KErrNoMemory and the next "if" will be executed.
		{
		delete data;
		delete key;
		::SqlServer().Compactor().ReleaseEntry(aDbFileName);
		}
	SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, CSQLSRVDATABASE_NEWCOMPACTENTRYL_EXIT, "Exit;0x%X;CSqlSrvDatabase::NewCompactEntryL;err=%d", (TUint)this, err));	
	__SQLLEAVE_IF_ERROR(err);
	}

/**
Removes database (identified by its logical name) from the compactor.

@param aDbName "main" or the attached database name

@see CSqlCompactor
@see RSqlCompactDbMap 
*/
void CSqlSrvDatabase::ReleaseCompactEntry(const TDesC& aDbName)
	{
	TSqlCompactDbMapIterator compactDbIt(iCompactDbMap);
	TSqlCompactDbPair compactDbPair;
	while(compactDbIt.Next(compactDbPair))
		{
		if(::CompareNoCase(*compactDbPair.iKey, aDbName) == 0)
			{
			::SqlServer().Compactor().ReleaseEntry(*compactDbPair.iData);
			iCompactDbMap.Remove(compactDbPair.iKey);
			SQL_TRACE_INTERNALS(OstTrace1(TRACE_INTERNALS, CSQLSRVDATABASE_RELEASECOMPACTENTRY, "0x%X;CSqlSrvDatabase::ReleaseCompactEntry", (TUint)this));	
			break;
			}
		}
	}

/**
Cleanup function.
Used during the construction phase of the CSqlSrvDatabase instance when a new database is created.
If the database creation succeeds, the "init compaction" operation succeeds but some other init operation fail,
the just created database file has to be deleted. But before that the database has to be removed from the compactor,
because the compactor creates independent database connection that has to be closed before the database deletion.

The database will be removed from the compactor as a result of the call.

@param aCleanup A pointer to the CSqlSrvDatabase object
*/
void CSqlSrvDatabase::CompactCleanup(void* aCleanup)
	{
	CSqlSrvDatabase* self = reinterpret_cast <CSqlSrvDatabase*> (aCleanup);
	__ASSERT_DEBUG(self != NULL, __SQLPANIC2(ESqlPanicBadArgument));
    self->ReleaseCompactEntry(KMainDb16);
	}

/**
Retrieves the database page size.
If the request is for the main database page size and if the size is not retrieved yet, then the page size value will be 
cached in iPageSize data member.

@param aDbName Attached database name or KNullDesC for the main database

@leave KErrNoMemory, an out of memory condition has occurred;
                      Note that the function may also leave with some other database specific 
                      errors categorised as ESqlDbError, and other system-wide error codes.

@return The database page size 
*/
TInt CSqlSrvDatabase::PageSizeL(const TDesC& aDbName)
	{
	if(iPageSize > 0 && aDbName == KNullDesC)
		{
		return iPageSize;	
		}
	iAuthorizerDisabled	= ETrue;
	CleanupStack::PushL(TCleanupItem(&EnableAuthorizer, &iAuthorizerDisabled));
	TInt pageSize = 0;
	__SQLLEAVE_IF_ERROR(::DbPageSize(iDbHandle, aDbName, pageSize));
	CleanupStack::PopAndDestroy();
	__ASSERT_DEBUG(pageSize > 0, __SQLPANIC(ESqlPanicInternalError));
	if(aDbName == KNullDesC)
		{
		iPageSize = pageSize;	
		}
	return pageSize;
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////        ConstructL() methods      ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Second phase construction method. Creates a new secure database file.
If the function fails, the database file will be closed and deleted.

@param aFileData A reference to a TSqlSrvFileData instance, which keeps the database name, a
     			 file session reference and some other database file related properties.
				 If this is a secure database, then the format of the name must be:
				 \<drive\>:\<[SID]database file name excluding the path\>.
				 If this is a non-secure database, then the file name has to be the full database file name.
				 "[SID]" refers to SID of the application which creates the database.
@param aSecurityPolicy Database security policy

@panic SqlDb 4 In _DEBUG mode if aSecurityPolicy is NULL.
*/
void CSqlSrvDatabase::ConstructCreateSecureL(const TSqlSrvFileData& aFileData, CSqlSecurityPolicy* aSecurityPolicy)
	{
	__ASSERT_DEBUG(aSecurityPolicy != NULL, __SQLPANIC(ESqlPanicBadArgument));
	//Insert a new item in the security policies map.
	CleanupStack::PushL(aSecurityPolicy);
	const TUint8* mapKey = SecurityMapKeyL(aFileData.FileName());
	mapKey = ::CreateStrCopyLC(mapKey);
	__ASSERT_DEBUG(!::SqlServer().SecurityMap().Entry(mapKey), __SQLPANIC(ESqlPanicObjExists));
 	__SQLLEAVE_IF_ERROR(::SqlServer().SecurityMap().Insert(mapKey, aSecurityPolicy));
	CleanupStack::Pop(2);//iSecurityMap owns mapKey and aSecurityPolicy.
	iSecureDbName = mapKey;
	iSecurityPolicy = aSecurityPolicy;
	//
	DoCommonConstructCreateL(aFileData, ETrue);
	}

/**
Second phase construction method. Creates a new non-secure database file.
If the function fails, the database file will be closed and deleted.

@param aFileData A reference to a TSqlSrvFileData instance, which keeps the database name, a
     			 file session reference and some other database file related properties.
				 If this is a secure database, then the format of the name must be:
				 \<drive\>:\<[SID]database file name excluding the path\>.
				 If this is a non-secure database, then the file name has to be the full database file name.
				 "[SID]" refers to SID of the application which creates the database.
*/
void CSqlSrvDatabase::ConstructCreateL(const TSqlSrvFileData& aFileData)
	{
	DoCommonConstructCreateL(aFileData, EFalse);
	}

//Called by the two "Contruct&Create" methods: ConstructCreateL() and ConstructCreateSecureL().
//The aSecureDb parameter tells which method is the caller.
//The function performs common construction and initialization:
// - creates the database file
// - makes the journal file persistent
// - initializes the database compaction mode
// - stores the initial settings in the settings table, including the current collation dll name
// - stores the security settings and installs the authorizer, if aSecureDb is true
// - installs the user-defined functions
// - installs collations
//If the method fails and the error is not KErrAlreadyExists, the database file will be closed and deleted.
void CSqlSrvDatabase::DoCommonConstructCreateL(const TSqlSrvFileData& aFileData, TBool aSecureDb)
	{
	__ASSERT_DEBUG(!iDbHandle, __SQLPANIC(ESqlPanicInternalError));
	__ASSERT_DEBUG(aSecureDb ? iSecurityPolicy != NULL : ETrue, __SQLPANIC(ESqlPanicInternalError));
	CreateNewDbFileL(aFileData);
	TDbFileCleanup dbFileCleanup(aFileData, iDbHandle);
	CleanupStack::PushL(TCleanupItem(&DbFileCleanup, &dbFileCleanup));
	//Make the journal file persistent - done by SQLite automatically because the locking mode is EXCLUSIVE
	//::ExecPragma(iDbHandle, iAuthorizerDisabled, KPersistentJournalPragma, KPersist);
	//Init database compaction mode
	TSqlCompactionMode compactionMode = aFileData.ConfigParams().iCompactionMode;
	if(compactionMode == ESqlCompactionNotSet)
		{
		compactionMode = KSqlDefaultCompactionMode;
		}
	TInt currVacuumMode = -1;
	__SQLLEAVE_IF_ERROR(::DbVacuumMode(iDbHandle, KNullDesC, currVacuumMode));
	//currVacuumMode == ESqliteVacuumOff ==> This is a database created not by the SQL server
	InitCompactionL(compactionMode, aFileData.ConfigParams().iFreePageThresholdKb, aFileData.FileName(), (TSqliteVacuumMode)currVacuumMode);
	CleanupStack::PushL(TCleanupItem(&CSqlSrvDatabase::CompactCleanup, this));
	//Store the initial settings in the settings table (including the current collation dll name)
	StoreSettingsL(::SqlServer().CollationDllName(), KSqlNullDbConfigFileVersion, compactionMode);
	if(aSecureDb)
		{
		//Store the security policies in the security policies tables.
		TSqlDbSysSettings dbSysSettings(iDbHandle);
		dbSysSettings.StoreSecurityPolicyL(*iSecurityPolicy);
		}
	InstallAuthorizerL();
	InstallUDFsL();
	InstallCollationsL();
	CleanupStack::Pop(2);//CompactCleanup, DbFileCleanup
	SQLPROFILER_DB_CREATE((TUint)iDbHandle, aFileData.FileName());
	}

/**
Second phase construction method. Opens an existing  secure database file.

@param aFileData A reference to a TSqlSrvFileData instance, which keeps the database name, a
     			 file session reference and some other database file related properties.
				 If this is a secure database, then the format of the name must be:
				 \<drive\>:\<[SID]database file name excluding the path\>.
				 If this is a non-secure database, then the file name has to be the full database file name.
				 "[SID]" refers to SID of the application which creates the database.
*/
void CSqlSrvDatabase::ConstructOpenSecureL(const TSqlSrvFileData& aFileData)
	{
	DoCommonConstructOpenL(aFileData, ETrue);
	}

/**
Second phase construction method. Opens an existing  non-secure database file.

@param aFileData A reference to a TSqlSrvFileData instance, which keeps the database name, a
     			 file session reference and some other database file related properties.
				 If this is a secure database, then the format of the name must be:
				 \<drive\>:\<[SID]database file name excluding the path\>.
				 If this is a non-secure database, then the file name has to be the full database file name.
				 "[SID]" refers to SID of the application which creates the database.
				 If this is application's private database, then the format of aFileData is as it is described
				 in TSqlSrvFileData::SetFromHandleL() comments.
@see TSqlSrvFileData::SetFromHandleL()
*/
void CSqlSrvDatabase::ConstructOpenL(const TSqlSrvFileData& aFileData)
	{
	DoCommonConstructOpenL(aFileData, EFalse);
	}

//Opens a database and does all necessary initializations
//Called by the two "Contruct&Open" methods: ConstructOpenL() and ConstructOpenSecureL().
//The aSecureDb parameter tells which method is the caller.
//The function performs common construction and initialization:
// - opens the database file
// - installs the user-defined functions
// - installs collations
// - installs the authoriser callback
void CSqlSrvDatabase::DoCommonConstructOpenL(const TSqlSrvFileData& aFileData, TBool aSecureDb)
	{
	OpenExistingDbFileL(aFileData);//iDbHandle is valid after a successful call
	//The user-defined collations must be installed before the possible database reindexing!!!
	InstallCollationsL();
	if(!aFileData.IsReadOnly())
		{//No need to disable the authorizer since it is not installed yet.
		 //Make sure that the user-defined collation have been installed before the reindexing operation.
		ProcessSettingsL(aFileData, KMainDb16);
		}
	if(aSecureDb)
		{
		const TUint8* mapKey = NULL;
		//Load database security policy, update the security policy map
		UpdateSecurityMapL(EFalse, aFileData, mapKey, iSecurityPolicy);
		iSecureDbName = mapKey;//used in CSqlSrvDatabase destructor. 
		mapKey = NULL;//it is not used
		//Check that the caller has at least one of {Schema, Read, Write} policies.
		BasicSecurityPolicyCheckL(*iSecurityPolicy);
		}
	//Install user-defined functions.
	InstallUDFsL();
	
	//Install the authorizer.
	InstallAuthorizerL();
	
    SQLPROFILER_DB_OPEN((TUint)iDbHandle, aFileData.FileName());
	}

/*
Implementation of the like() SQL function.  This function implements
the user defined LIKE operator.  The first argument to the function is the
pattern and the second argument is the string.  So, the SQL statements:
A LIKE B
is implemented as like(B, A).

@param aContext Function call context;
@param aArgc Number of LIKE arguments: 2 for the standard LIKE operator, 3 for the LIKE operator with an ESCAPE clause;
@param aArgv LIKE arguments;

@internalComponent
*/
void CSqlSrvDatabase::LikeSqlFunc(sqlite3_context* aContext, int aArgc, sqlite3_value** aArgv)
	{
  	TUint escapeChar = 0;
	if(aArgc == 3)
		{
    	//The escape character string must consist of a single UTF16 character.
    	//Otherwise, return an error.
		const TUint16* esc = static_cast <const TUint16*> (sqlite3_value_text16(aArgv[2]));
		if(!esc)
			{
      		sqlite3_result_error(aContext, KErrMsg1, -1);
			return;
			}
		if(User::StringLength(esc) != 1)
			{
      		sqlite3_result_error(aContext, KErrMsg2, -1);
          	return;
			}
		escapeChar = *esc;			
    	}
	const TUint16* pattern   = static_cast <const TUint16*> (sqlite3_value_text16(aArgv[0]));
	const TUint16* candidate = static_cast <const TUint16*> (sqlite3_value_text16(aArgv[1]));
  	if(pattern && candidate)
		{
		TInt wildChar = '_';
		TInt wildSeqChar = '%';
		TPtrC16 patternStr(pattern, (TUint)sqlite3_value_bytes16(aArgv[0]) / sizeof(TUint16));
		TPtrC16 candidateStr(candidate, (TUint)sqlite3_value_bytes16(aArgv[1]) / sizeof(TUint16));
		TInt res = candidateStr.MatchC(patternStr, wildChar, wildSeqChar, escapeChar, 0/*collation level*/);
    	sqlite3_result_int(aContext, res >= 0);
		//RDebug::Print(_L("--res=%d, pattern=%S, candidate=%S\r\n"), res, &patternStr, &candidateStr);
		}
	}

