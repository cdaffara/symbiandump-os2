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

#include <hal.h>
#include "SqlSrvMain.h"			//CSqlServer
#include "SqlSrvStartup.h"		//SqlSrvVersion()
#include "SqlSrvSession.h"		//CSqlSrvSession
#include "SqlSrvDbSysSettings.h"//TSqlDbSysSettings
#include "SqlSrvStatementUtil.h"
#include "SqlSrvStatement.h"
#include "sqlite3.h"			//sqlite3_enable_shared_cache()
#include "SqliteSymbian.h"		//sqlite3SymbianLibInit()
#include "SqlCompact.h"
#include "SqlCompactConn.h"
#include "SqlSrvResourceProfiler.h"
#ifdef _DEBUG
#include <stdio.h>
#endif
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "SqlSrvMainTraces.h"
#endif
#include "SqlTraceDef.h"

#ifndef SQLSRV_STARTUP_TEST
static  
#endif
CSqlServer* TheServer = NULL;//The single CSqlServer instance

#ifdef _DEBUG
#define __SQLDEBUG_EXPR(expr) expr
#else
#define __SQLDEBUG_EXPR(expr)
#endif

_LIT(KMatchAllDbFiles, "*");
_LIT(KDefaultICollationDllName, "");

//Constants for enabling/disabling the shared cache
enum TSharedCacheState 
	{
	EDisableSharedCache = 0, 
	EEnableSharedCache = 1
	};

#ifdef SYSLIBS_TEST
	//The "lookaside" optimisation is disabled if SYSLIBS_TEST macro is defined. 
	//According to the SQLite authors recommendations, the OOM testing should be performed without this optimisation.
	const TInt KSqliteLookAsideCellSize = 0;
	const TInt KSqliteLookAsideCellCount = 0;
#else
	//SQLite, "fixed heap cell size" constants
	//SQLite will preallocate KSqliteLookAsideCellSize * KSqliteLookAsideCellCount bytes from the heap and
	//use the allocated block for all allocation requests with size <= KSqliteLookAsideCellSize.
	//The malloc()/free() request time is constant, if the cell is retrieved/returned from/to the "fixed cell size" block.
	const TInt KSqliteLookAsideCellSize = 128;
	const TInt KSqliteLookAsideCellCount = 512;
#endif

//Local function, used for comparing TSqlSecurityPair objects.
//The keys are expected to be UTF8 encoded, zero-terminated.
//
//The function will panic with panic code 7 in _DEBUG mode if the key part of aLeft or
//aRight argument is NULL.
static TInt Compare(const TSqlSecurityPair& aLeft, const TSqlSecurityPair& aRight)
	{
	__ASSERT_DEBUG(aLeft.iKey != NULL && aRight.iKey != NULL, __SQLPANIC2(ESqlPanicInternalError));
	return ::CompareNoCase8(TPtrC8(aLeft.iKey), TPtrC8(aRight.iKey));
	}
	
/**
Returns a reference to the sql server instance.

@return A reference to the sql server instance.

@panic SqlDb 2 If the sql server instance is NULL.

@internalComponent
*/
CSqlServer& SqlServer(void)
	{
	__ASSERT_ALWAYS(TheServer != NULL, __SQLPANIC2(ESqlPanicInvalidObj));		
	return *TheServer;
	}
	
/**
Creates new CSqlServer instance.
The created instance will be pushed in the cleanup stack.

@return A pointer to the created CSqlServer instance.

@leave KErrNoMemory, an out of memory condition has occured;
*/
CSqlServer* CSqlServer::NewLC()
	{
	SQL_TRACE_INTERNALS(OstTrace0(TRACE_INTERNALS, CSQLSERVER_NEWLC_ENTRY, "Entry;0;CSqlServer::NewLC"));
	CSqlServer* self = new (ELeave) CSqlServer;
	CleanupStack::PushL(self);
	self->ConstructL();
	SQL_TRACE_INTERNALS(OstTrace1(TRACE_INTERNALS, CSQLSERVER_NEWLC_EXIT, "Exit;0x%X;CSqlServer::NewLC", (TUint)self));
	return self;
	}
	
/**
Frees owned by CSqlServer memory and other resources.
*/
CSqlServer::~CSqlServer()
	{
	SQL_TRACE_INTERNALS(OstTrace1(TRACE_INTERNALS, CSQLSERVER_CSQLSERVER2_ENTRY, "Entry;0x%x;CSqlServer::~CSqlServer", (TUint)this));
	delete iCompactor;
	delete iBurEventMonitor;
	iDriveSpaceCol.ResetAndDestroy();
	sqlite3_soft_heap_limit(0);//Set to 0 the soft heap limit
	iSecurityMap.Close();
	(void)sqlite3_enable_shared_cache(static_cast <TInt> (EDisableSharedCache));
	iFlatBuf.Close();
	User::Free(iBuf);
	delete iDbConfigFiles;
	sqlite3SymbianLibFinalize();
	TheServer = NULL;
	SQL_TRACE_INTERNALS(OstTrace1(TRACE_INTERNALS, CSQLSERVER_CSQLSERVER2_EXIT, "Exit;0x%x;CSqlServer::~CSqlServer", (TUint)this));
	}

/**
@param aMinLen Requested minimal byte size of the flat buffer

@return A reference to the server's general purpose flat bufer. The buffer cannot keep a state between calls.
*/
RSqlBufFlat& CSqlServer::GetFlatBufL(TInt aMinLen)
	{
	__ASSERT_DEBUG(aMinLen >= 0, __SQLPANIC(ESqlPanicBadArgument));
	__SQLLEAVE_IF_ERROR(iFlatBuf.ReAlloc(aMinLen));
	SQLPROFILER_REPORT_ALLOC(iFlatBuf.MaxSize());
	return iFlatBuf;
	}

/**
Returns a 8-bit descriptor's reference to the server's general purpose buffer.
Note that the function may reallocate the buffer if the buffer length is smaller than the requested minimal length.

@param aMinLen Requested minimal 8-bit character length of the buffer

@return TDes8 reference to the server's general purpose bufer. The buffer cannot keep a state between calls.
*/
TDes8& CSqlServer::GetBuf8L(TInt aMinLen)
	{
	__ASSERT_DEBUG(aMinLen >= 0, __SQLPANIC(ESqlPanicBadArgument));
#ifdef _DEBUG
	TInt maxBufLen = iBufPtr8.MaxLength();
	maxBufLen = maxBufLen;
#endif
	if(iBufPtr8.MaxLength() < aMinLen)
		{
		__SQLLEAVE_IF_ERROR(ReAllocBuf(aMinLen));
		}
	SQLPROFILER_REPORT_ALLOC(iBufPtr8.MaxLength());
	return iBufPtr8;
	}
	
/**
Returns a 16-bit descriptor's reference to the server's general purpose buffer.
Note that the function may reallocate the buffer if the buffer length is smaller than the requested minimal length.

@param aMinLen Requested minimal 16-bit character length of the buffer

@return TDes16 reference to the server's general purpose bufer. The buffer cannot keep a state between calls.
*/
TDes16& CSqlServer::GetBuf16L(TInt aMinLen)
	{
	__ASSERT_DEBUG(aMinLen >= 0, __SQLPANIC(ESqlPanicBadArgument));
#ifdef _DEBUG
	TInt maxBufLen = iBufPtr16.MaxLength();
	maxBufLen = maxBufLen;
#endif
	if(iBufPtr16.MaxLength() < aMinLen)
		{
		__SQLLEAVE_IF_ERROR(ReAllocBuf(aMinLen * sizeof(TUint16)));
		}
	SQLPROFILER_REPORT_ALLOC(iBufPtr16.MaxLength());
	return iBufPtr16;
	}

/**
If iFlatBuf or iBuf allocated memory is more than KBufLimit bytes,
then that buffer will be reallocated down to KBufLimit size.
*/
void CSqlServer::MinimizeBuffers()
	{
	iFlatBuf.ResetAndMinimize();
#ifdef _DEBUG	
	const TInt KBufLimit = 64;
	const TUint8* oldBuf = iBuf;
#else
	const TInt KBufLimit = 8 * 1024;
#endif
	if(iBufPtr8.MaxSize() > KBufLimit)
		{
		(void)ReAllocBuf(KBufLimit);
		__ASSERT_DEBUG(oldBuf == iBuf, __SQLPANIC(ESqlPanicInternalError));
		}
	}

/**
Reallocates iBuf. iBuf content is not preserved.
Sets iBufPtr8 and iBufPtr16 to point to iBuf.

@param aNewBufSize The new buffer size in bytes

@return KErrNoMemory, an out of memory condition has occurred;
		KErrNone, the operation has completed successfully;
*/
TInt CSqlServer::ReAllocBuf(TInt aNewBufSize)
	{
	__ASSERT_DEBUG(aNewBufSize >= 0, __SQLPANIC(ESqlPanicBadArgument));
#ifdef _DEBUG	
	const TInt KMinBufSize = 8;
#else
	const TInt KMinBufSize = 2 * 1024;
#endif
	const TInt KNewBufSize = Max(aNewBufSize, KMinBufSize);
	TUint8* newBuf = static_cast <TUint8*> (User::ReAlloc(iBuf, KNewBufSize));
	if(newBuf)
		{
		iBuf = newBuf;
		iBufPtr8.Set(iBuf, 0, KNewBufSize);
		iBufPtr16.Set(reinterpret_cast <TUint16*> (iBuf), 0, (TUint)KNewBufSize / sizeof(TUint16));
		return KErrNone;
		}
	else
		{//The reallocation has failed, iBuf - not changed
		iBufPtr8.Zero();
		iBufPtr16.Zero();
		return KErrNoMemory;
		}
   	}

/**
Creates new CSqlSrvSession instance.
If SQLSRV_STARTUP_TEST macro is defined, then the function returns NULL.
The "real" implementation of the function is not used in this case because the used unit test will require 
a lot of cpp files to be included into the test build (t_sqlstartup).

@return A pointer to the created CSqlSrvSession instance.

@leave KErrNoMemory, an out of memory condition has occured;
       KErrNotSupported, the client side library version differs from the server version.
       
@see CSqlSrvSession
*/
CSession2* CSqlServer::NewSessionL(const TVersion& aVersion, const RMessage2&) const
    {
#ifdef SQLSRV_STARTUP_TEST
    aVersion.Name();//to prevent the compiler warning ("unused parameter").
    return NULL;
#else
    if(!User::QueryVersionSupported(::SqlSrvVersion(), aVersion))
        {
        User::Leave(KErrNotSupported);
        }
    CSqlSrvSession* sess = CSqlSrvSession::NewL();
    return sess;
#endif //SQLSRV_STARTUP_TEST
    }

/**
CSqlServer's active object priority.

@internalComponent
*/
const TInt KSqlServerPriority = CActive::EPriorityStandard;

/**
Initializes CSqlServer data members with default values.
*/
CSqlServer::CSqlServer() :
	CServer2(KSqlServerPriority, ESharableSessions),
	iSecurityMap(TSqlSecurityLinearOrder(&Compare), TSqlSecurityDestructor()),
	iBufPtr8(0, 0),
	iBufPtr16(0, 0)
	{
	}
	
/**
Initializes CSqlServer instance:
 - starts the server;
 - opens sqlite library;
 - initializes the file session instance;
 - creates server's private directory on the system drive;
 - enables sqlite shared cache;

@leave KErrNoMemory, an out of memory condition has occured;
		             Note that the function may also leave with some other database specific 
			         errors categorised as ESqlDbError.
*/
void CSqlServer::ConstructL()
	{
#ifndef SQLSRV_STARTUP_TEST
	//Start the server only in "normal" builds, not in the case where t_sqlstartup unit test tests directly
	//the SQL server startup code.
	StartL(KSqlSrvName);
#endif	
    SQLPROFILER_SERVER_START();
	//Configure the SQLite library
	TInt sqliteErr = sqlite3_config(SQLITE_CONFIG_LOOKASIDE, KSqliteLookAsideCellSize, KSqliteLookAsideCellCount);
    __SQLLEAVE_IF_ERROR(::Sql2OsErrCode(sqliteErr, KErrArgument));
	//Open SQLITE library - this must be the first call after StartL() (os_symbian.cpp, "TheAllocator" initialization rellated).
	__SQLLEAVE_IF_ERROR(sqlite3SymbianLibInit());
	//Create buffers
	__SQLLEAVE_IF_ERROR(iFlatBuf.SetCount(0));
	//Get collation dll name
	GetCollationDllNameL();
	//Get the system drive.
	TInt sysDrive = static_cast<TInt>(RFs::GetSystemDrive());
	//Get the server private data path.
	RFs& fs = sqlite3SymbianFs();
	TFileName serverPrivatePath;
	__SQLLEAVE_IF_ERROR(fs.PrivatePath(serverPrivatePath));
	DeleteTempFilesL(sysDrive, serverPrivatePath);
	//Load config file parameter values (if config file exists) and initialize iFileData.
	TParse parse;
	__SQLLEAVE_IF_ERROR(parse.Set(KSqlSrvDefaultConfigFile, &serverPrivatePath, NULL));
	//Store the names of any existing database config files in memory
	CacheDbConfigFileNamesL(fs, serverPrivatePath);
	//Initialise the file data object
	iFileData.InitL(fs, TDriveUnit(sysDrive).Name(), serverPrivatePath, parse.FullName(), iDbConfigFiles);
		
	//Set the soft heap limit (iFileData.ConfigParams() returns now a reference to the config file params, including the soft heap limit, if set)
	const TSqlSrvConfigParams& configParams = iFileData.ConfigParams();
	if(configParams.iSoftHeapLimitKb > 0)
		{
		__ASSERT_DEBUG(configParams.iSoftHeapLimitKb >= TSqlSrvConfigParams::KMinSoftHeapLimitKb &&
		            configParams.iSoftHeapLimitKb <= TSqlSrvConfigParams::KMaxSoftHeapLimitKb, __SQLPANIC(ESqlPanicInternalError));
		sqlite3_soft_heap_limit(configParams.iSoftHeapLimitKb * 1024);
		}
	//Enable shared cache
	(void)sqlite3SymbianLastOsError();//clear last OS error
	TInt err = sqlite3_enable_shared_cache(static_cast <TInt> (EEnableSharedCache));
	__SQLLEAVE_IF_ERROR(::Sql2OsErrCode(err, sqlite3SymbianLastOsError()));
	//Create an empty "drive space" collection
	iDriveSpaceCol.Create(fs);
	// Create the BUR instance
	iBurEventMonitor = CSqlBurEventMonitor::NewL(*this);
	//Compactor
	iCompactor = CSqlCompactor::NewL(&SqlCreateCompactConnL, KSqlCompactStepIntervalMs);
#ifdef _DEBUG
    //The following statements exist to prevent the failure of the OOM testing in debug mode.
	//The standard C library allocates some memory at the startup and stores a pointer to the allocated memory
	//in the TLS. During normal API OOM testing the SQL server is not restarted, it never goes down.
	//Then the TLS and the allocated memory are not released. In which case the OOM testing will fail
	//(because the standard C library performs a lazy initialization and the allocation and TLS usage will be made
	//at the point of first use of some C function. This is out of the control of the test code).
	//In order to avoid that, during the SQL server startup here, before the OOM test goes and checks what 
	//is the allocated memory at the beginning, a fake sprintf() call is made in order to force the mentioned above  
	//allocation in the standard C library.
	//All explanations above are true, except one case when the SQl server startup code is tested directly.
    #ifndef SQLSRV_STARTUP_TEST
	const TInt KAnyNumber	= 0xAA55; 
 	char tmp[32]; 
 	sprintf(tmp, "%04X", KAnyNumber);
    const TInt KGreatSize = 1024; 
 	__SQLLEAVE_IF_ERROR(ReAllocBuf(KGreatSize));
    #endif //SQLSRV_STARTUP_TEST 	
#endif //_DEBUG 	
	}

/**
Delete any temp files left the "temp" subdirectory in server's private directory.

The SQLite is configured to use shared page cache. When the shared page cache is enabled,
those temp files created by SQLite are deleted only when the database gets closed. However,
if during power down event the client application does not close the database, 
the temp files will never get deleted.
This is why the SQL server should deletes all temp files during its start-up.

Note that all errors exept KErrNoMemory are ignored in the function body, becasuse
the temp files deletion is not a critical operation to prevent the server start up.

@param aDriveNumber A drive number.
@param aServerPath A server's private path.

*/
void CSqlServer::DeleteTempFilesL(TInt aDriveNumber, const TDesC& aServerPath)const
    {
    _LIT(KTempFileDir, "temp");
    _LIT(KWildCard, "*.*");
    TDriveUnit drive(aDriveNumber); 
    TDriveName driveName = drive.Name();
    TParse parse;
    (void)parse.Set(aServerPath, &driveName, 0);//this call can't fail
    (void)parse.AddDir(KTempFileDir);//this call can't fail
    TFileName tempfileDir(parse.FullName());
    (void)parse.Set(KWildCard, &tempfileDir, 0);//this call can't fail
	RFs& fs = sqlite3SymbianFs();
	CFileMan* fm = CFileMan::NewL(fs);
	(void)fm->Delete(parse.FullName());
	delete fm;
    }

/**
Retrieves in iCollationDllName current(default) collation dll name.
see TExtendedLocale
*/
void CSqlServer::GetCollationDllNameL()
	{
	TExtendedLocale	extdlocale;
	extdlocale.LoadSystemSettings();
	TFileName fname;
	TParse fileName;
	TInt err = extdlocale.GetLocaleDllName(ELocaleCollateSetting, fname);
	if(err!= KErrNone)
		{
		iCollationDllName = KDefaultICollationDllName;
		}
	else
		{
		//only get the file name + extension 
		fileName.Set(fname, NULL, NULL);
		iCollationDllName = fileName.NameAndExt();
		}
	SQL_TRACE_INTERNALS(OstTraceExt3(TRACE_INTERNALS, CSQLSERVER_GETCOLLATIONDLLNAMEL, "0x%x;CSqlServer::GetCollationDllNameL;iCollationDllName=%S;err=%d", (TUint)this, __SQLPRNSTR(iCollationDllName), err));
	}
/**
Finds and caches the name of each database configuration file 
that exists in the server's private data cage on the Z: drive
*/
void CSqlServer::CacheDbConfigFileNamesL(RFs& aFs, const TDesC& aServerPrivatePath)
	{
	//Create an in-memory array holding the names of the database config files, if any exist
	TParse parseDbConfig;	
	__SQLLEAVE_IF_ERROR(parseDbConfig.Set(KSqlSrvDbConfigFileFormat, &aServerPrivatePath, NULL));
	TFileName configFilePath(parseDbConfig.FullName());	// get 'drive:\private path\cfg*' search string
	CDir* entryList = 0; // memory will be allocated for this in GetDir()
	TInt err = aFs.GetDir(configFilePath, KEntryAttNormal, ESortByName, entryList);
	if(err == KErrNone)
		{
		__ASSERT_DEBUG(entryList != NULL, __SQLPANIC(ESqlPanicInternalError));
		CleanupStack::PushL(entryList);
		if(entryList->Count() > 0)
			{	
			iDbConfigFiles = CDbConfigFiles::NewL(*entryList);
			}
		CleanupStack::PopAndDestroy(entryList);	
		}
	else
		{
		SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, CSQLSERVER_CACHEDDBCONFIGFILENAMESL, "0x%X;CSqlServer::CacheDbConfigFileNamesL;GetDir() failed with error code %d", (TUint)this, err));	
		__ASSERT_DEBUG(!entryList, __SQLPANIC(ESqlPanicInternalError));
		}
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////   MSqlPolicyInspector implementation  ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
/**
Implements MSqlPolicyInspector::Check() method.

@see MSqlPolicyInspector
@see MSqlPolicyInspector::Check()
*/
TBool CSqlServer::Check(const TSecurityPolicy& aPolicy) const
	{
#ifdef SQLSRV_STARTUP_TEST
	aPolicy.Package();//to prevent compiler warning
	return ETrue;
#else	
	return aPolicy.CheckPolicy(CServer2::Message());
#endif
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////   MSqlSrvBurInterface implementation   //////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Implements MSqlSrvBurInterface::Fs().

@return A reference to the file session instance.
*/
RFs& CSqlServer::Fs()
	{
	return iFileData.Fs();		
	}
	
/**
Implements MSqlSrvBurInterface::GetBackupListL().
Retrieves in aFileNameList parameter a list of secure database names (full database names, including path) 
which security UID matches aUid parameter.
No databases will be included into the list, if the drive is read-only.

@param aUid Database security UID.
@param aDrive The drive where the database search will be performed, in the SQL server private data cage.
@param aFileNameList An output parameter.
				 Each array entry represents the full name of a database in SQL server private data cage
				 on the specified drive (aDrive), which uid matches the aUid parameter.
				 
@leave KErrNoMemory, an out of memory condition has occured;
					 Note that the function may leave also with some other database specific or OS specific
					 error codes.
*/
void CSqlServer::GetBackUpListL(TSecureId aUid, TDriveNumber aDrive, RArray<HBufC*>& aFileNameList)
	{
	SQL_TRACE_INTERNALS(OstTraceExt3(TRACE_INTERNALS, CSQLSERVER_GETBACKUPLISTL_ENTRY, "Entry;0x%x;CSqlServer::GetBackUpListL;aDrive=%d;aUid=0x%X", (TUint)this, (TInt)aDrive, (TUint)aUid.iId));
	__ASSERT_DEBUG(aFileNameList.Count() == 0, __SQLPANIC(ESqlPanicBadArgument));
	RFs& fs = iFileData.Fs();
	//No files in the list if aDrive is a read-only drive
	TDriveInfo driveInfo;
	__SQLLEAVE_IF_ERROR(fs.Drive(driveInfo, aDrive));
	if(driveInfo.iDriveAtt & KDriveAttRom)
		{
		return;
		}
	//Compose the search path
	TDriveUnit driveUnit(aDrive);
	TDriveName driveName = driveUnit.Name();
	TFileName path;
	path.Copy(driveName);
	path.Append(iFileData.PrivatePath());
	//Include the aUid and the "*" mask
	TUidName uidName = (static_cast <TUid> (aUid)).Name();
	TBuf<KMaxUidName + sizeof(KMatchAllDbFiles)> fileNameMask(uidName);
	fileNameMask.Append(KMatchAllDbFiles);
	TParse parse;
	__SQLLEAVE_IF_ERROR(parse.Set(path, &fileNameMask, NULL)); 
	//Do the search
	TPtrC fullPath(parse.FullName());
	SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, CSQLSERVER_GETBACKUPLISTL_FULLPATH, "Exit;0x%x;CSqlServer::GetBackUpListL;fullPath=%S", (TUint)this, __SQLPRNSTR(fullPath)));
	CDir* fileNameCol = NULL;
	TInt err = fs.GetDir(fullPath, KEntryAttNormal, ESortNone, fileNameCol);
	if(err == KErrNotFound)
		{
		__ASSERT_DEBUG(!fileNameCol, __SQLPANIC(ESqlPanicInternalError));
		SQL_TRACE_INTERNALS(OstTrace1(TRACE_INTERNALS, CSQLSERVER_GETBACKUPLISTL_EXIT1, "Exit;0x%x;CSqlServer::GetBackUpListL;no files found", (TUint)this));
		return;
		}
	__SQLLEAVE_IF_ERROR(err);
	__ASSERT_DEBUG(fileNameCol != NULL, __SQLPANIC(ESqlPanicInternalError));
	CleanupStack::PushL(fileNameCol);
	TInt fileCount = fileNameCol->Count();
	__SQLLEAVE_IF_ERROR(aFileNameList.Reserve(fileCount));
	//Append the full database file paths to the file names list.
	for(TInt i=0;i<fileCount;++i)
		{
		const ::TEntry& entry = (*fileNameCol)[i];
		__ASSERT_DEBUG(!entry.IsDir(), __SQLPANIC(ESqlPanicInternalError));//RFs::GetDir() search attributes exclude directories (see the GetDir() call above).
		__SQLLEAVE_IF_ERROR(parse.Set(path, &entry.iName, NULL));
		TPtrC fname(parse.FullName());
		SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, CSQLSERVER_GETBACKUPLISTL, "0x%x;CSqlServer::GetBackUpListL;fname=%S", (TUint)this, __SQLPRNSTR(fname)));
		HBufC* fnameBuf = fname.AllocL();
		__SQLDEBUG_EXPR(err = )aFileNameList.Append(fnameBuf);
		__ASSERT_DEBUG(err == KErrNone, __SQLPANIC(ESqlPanicInternalError));
		}
	CleanupStack::PopAndDestroy(fileNameCol);
	SQL_TRACE_INTERNALS(OstTraceExt2(TRACE_INTERNALS, CSQLSERVER_GETBACKUPLISTL_EXIT2, "Exit;0x%x;CSqlServer::GetBackUpListL;file count=%d", (TUint)this, fileCount));
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////   SQL server startup   //////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SQLSRV_STARTUP_TEST

//Run the SQL server
static void RunServerL()
	{
	// naming the server thread after the server helps to debug panics
	User::LeaveIfError(User::RenameThread(KSqlSrvName));

	// create and install the active scheduler we need
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
	TheServer = CSqlServer::NewLC();
	RProcess::Rendezvous(KErrNone);
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(2, scheduler);//CSqlServer, scheduler
	}

// SQL server process entry point
TInt E32Main()
	{
	__UHEAP_MARK;

	CTrapCleanup* cleanup = CTrapCleanup::New();
	TInt err = KErrNoMemory;
	if(cleanup)
		{
		TRAP(err, ::RunServerL());
		delete cleanup;
		}

	__UHEAP_MARKEND;

	return err;
	}

#endif //SQLSRV_STARTUP_TEST
