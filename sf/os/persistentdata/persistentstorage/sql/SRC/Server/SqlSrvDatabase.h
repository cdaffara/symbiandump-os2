// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SQLSRVDATABASE_H__
#define __SQLSRVDATABASE_H__

#include <e32base.h>
#include <sqldb.h>				//RSqlDatabase::TIsolationLevel
#include "SqlSrvSecurityMap.h"	
#include "sqlite3.h"
#include "SqliteSymbian.h"		//sqlite3SymbianLastOsError()
#include "SqlSrvStatementUtil.h"//Global sql statement related functions
#include "SqlSrvStrings.h"
#include "SqlSrvCollation.h"	//User defined collations
#include "SqlSrvDbSysSettings.h"//TSqlDbSysSettings
#include "SqlSecurityImpl.h"	//CSqlSecurityPolicy
#include "SqlUtil.h"			//TSqlLikeWildcard

//Forward declarations
class TSqlSrvFileData;
struct TSqlSrvConfigParams;

//////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////   CSqlSrvDatabase class    /////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

/**
SQL database connection manager. (SQL database handle)

CSqlSrvDatabase is a server side class which processes the client side requests for:
- creating a SQL database (CSqlSrvDatabase::Create());
- opening an existing SQL database (CSqlSrvDatabase::Open());
- retrieving the last SQL error message (CSqlSrvDatabase::LastErrorMessage());
- executing 8-bit and 16-bit SQL statements (CSqlSrvDatabase::ExecL());
- setting the transaction isolation level (CSqlSrvDatabase::SetIsolationLevelL());
- retrieving the database security policy (CSqlSrvDatabase::SecurityPolicy());
- attaching an existing SQL database to current connection (CSqlSrvDatabase::AttachDbL());
- detaching previously attached SQL database from current connection (CSqlSrvDatabase::DetachDbL());

CSqlSrvDatabase class also manages all platform security related activities for secure databases:
- loading/storing database security policies when creating/opening a database connection;
- asserting client rights to perform a specific database operation;
- maintaining security related internal data structures in tact;

@see CSqlSrvDatabase::Create()
@see CSqlSrvDatabase::Open()
@see CSqlSrvDatabase::LastErrorMessage()
@see CSqlSrvDatabase::ExecL()
@see CSqlSrvDatabase::SetIsolationLevelL()
@see CSqlSrvDatabase::SecurityPolicy()
@see CSqlSrvDatabase::AttachDbL()
@see CSqlSrvDatabase::DetachDbL()
@see CSqlSrvDatabase::LastChangesCount()

@internalComponent
*/
NONSHARABLE_CLASS(CSqlSrvDatabase) : public CBase
	{		
public:
	//Object creation methods
	static CSqlSrvDatabase* CreateSecureL(const TSqlSrvFileData& aFileData, CSqlSecurityPolicy* aSecurityPolicy);
	static CSqlSrvDatabase* CreateL(const TSqlSrvFileData& aFileData);
	static CSqlSrvDatabase* OpenL(const TSqlSrvFileData& aFileData);
	virtual ~CSqlSrvDatabase();
	//Interface methods
	inline TPtrC LastErrorMessage() const;
	inline void ExecL(TDes16& aSqlStmt);
	inline void ExecL(const TDesC8& aSqlStmt);
	inline void SetIsolationLevelL(RSqlDatabase::TIsolationLevel aLevel);
	inline sqlite3* RawDbHandle() const;
	inline const CSqlSecurityPolicy* SecurityPolicy() const;
	void AttachDbL(const TSqlSrvFileData& aFileData, const TDesC& aDbName);
	void DetachDbL(const TDesC& aDbName);
	inline TInt LastChangesCount();
	inline TInt64 LastInsertedRowId();
	inline TBool InTransaction() const;
	TInt64 SizeL(const TDesC& aDbName = KNullDesC);
	TInt64 FreeSpaceL(const TDesC& aDbName = KNullDesC);
	void QueryConfigL(TDes8& aDest);
	TInt CompactL(TInt aSize, const TDesC& aDbName = KNullDesC);
	static TInt AuthorizeCallback(void* aDb, TInt aDbOpType, 
						   		  const char* aDbObjName1, const char* aDbObjName2, 
						   		  const char* aDbName, const char* aTrgOrViewName);

private:
	CSqlSrvDatabase();
	void CreateNewDbFileL(const TSqlSrvFileData& aFileData);
	void OpenExistingDbFileL(const TSqlSrvFileData& aFileData);
	void InstallAuthorizerL();
	inline void InstallCollationsL();
	void InstallUDFsL();
	const TUint8* SecurityMapKeyL(const TDesC& aDbFileName);
	void UpdateSecurityMapL(TBool aAttachedDb, const TSqlSrvFileData& aFileData, 
							const TUint8*& aMapKey, const CSqlSecurityPolicy*& aSecurityPolicy);
	void RemoveFromMapsL(const TDesC& aDbName);
	void InsertInAttachDbMapL(const TDesC& aDbFileName, const TDesC& aDbName);
	void InitAttachedDbL(const TSqlSrvFileData& aFileData, const TDesC& aDbName);
	TInt FinalizeAttachedDb(const TDesC& aDbName);
	static void AttachCleanup(void* aCleanup);
	enum TAttachState {EAStNone, EAStDbAttached, EAStSecurityMapUpdated};
	void DoAttachSecureDbL(TAttachState& aState, const TSqlSrvFileData& aFileData, const TDesC& aDbName, const TUint8*& aMapKey);
	inline void BasicSecurityPolicyCheckL(const CSqlSecurityPolicy& aSecurityPolicy);
	inline void StoreSettingsL(const TDesC& aCollationDllName, TInt aDbConfigFileVersion, TSqlCompactionMode aCompactionMode);
	void ProcessSettingsL(const TSqlSrvFileData& aFileData, const TDesC& aDbName);
	void ApplyConfigUpdatesL(const TDesC& aStoredCollationDllName, const TInt& aStoredDbConfigFileVersion, 
							 const TSqlSrvFileData& aFileData, const TDesC& aDbName);
	void SetConfigL(const TSqlSrvConfigParams& aConfigParams, TBool aSetPageSize, const TDesC& aLogicalDbName = KNullDesC);
	void InitCompactionL(TSqlCompactionMode aCompactionMode, TInt aFreePageThresholdKb, 
						 const TDesC& aDbFileName, TSqliteVacuumMode aCurrentVacuumMode, const TDesC& aDbName = KMainDb16);
	void NewCompactEntryL(TInt aFreePageThresholdKb, const TDesC& aDbFileName, const TDesC& aDbName);
	void ReleaseCompactEntry(const TDesC& aDbName);
	static void CompactCleanup(void* aCleanup);
	TInt PageSizeL(const TDesC& aDbName = KNullDesC);
	//ConstructL() methods
	void ConstructCreateSecureL(const TSqlSrvFileData& aFileData, CSqlSecurityPolicy* aSecurityPolicy);
	void ConstructCreateL(const TSqlSrvFileData& aFileData);
	void DoCommonConstructCreateL(const TSqlSrvFileData& aFileData, TBool aSecureDb);
	void ConstructOpenSecureL(const TSqlSrvFileData& aFileData);
	void ConstructOpenL(const TSqlSrvFileData& aFileData);
	void DoCommonConstructOpenL(const TSqlSrvFileData& aFileData, TBool aSecureDb);
	//
	static void LikeSqlFunc(sqlite3_context* aContext, int aArgc, sqlite3_value** aArgv);
	
private:
	sqlite3*				iDbHandle;//The database handle, owned by CSqlSrvDatabase.
	const CSqlSecurityPolicy* iSecurityPolicy;//The main database security policy, not owned by CSqlSrvDatabase (owned by the CSqlServer::iSecurityMap object).
	TUint8 					iFileNameBuf[KMaxFileName + 1];//Temporary buffer for storing UTF8 encoded, zero terminated database names.
	TBool					iAuthorizerInstalled;//If non-zero then the authorizer callback is already installed.
												 //Fact: the authorizer callback must be installed for each 
												 //secure CSqlSrvDatabase object. But it is possible that
												 //      - there is one secure database object;
												 //		 - a second secure database is attached to the first one.
												 //		   in this case there is no need to install the authorizer again.
												 //
												 //Or there is another situation: non-secure main database + secure attached database.
	TBool					iAuthorizerDisabled; //See the comments for iAuthorizerInstalled.
												 //This flag is used only when attaching a secure database.
												 //Since during this operation the system tables are read/writen,
												 //The authorizer callback has to be temporary disabled.
	RSqlAttachDbMap			iAttachDbMap;//["dbName":"dbFileName"] map, owned by CSqlSrvDatabase.
	const TUint8*			iSecureDbName;//<Drive:><Name[.Ext]>, used as a key in the security map. Not owned by CSqlSrvDatabase.
	RSqlCompactDbMap		iCompactDbMap; //["dbName":"Compact db entry"] map, owned by CSqlSrvDatabase.
	TInt					iPageSize;	//Main database page size in bytes
	
	};

#include "SqlSrvDatabase.inl"

#endif//__SQLSRVDATABASE_H__
