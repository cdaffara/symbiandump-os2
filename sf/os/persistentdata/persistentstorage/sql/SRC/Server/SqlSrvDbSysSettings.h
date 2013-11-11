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

#ifndef __SQLSRVDBSYSSETTINGS_H__
#define __SQLSRVDBSYSSETTINGS_H__

#include "SqlAssert.h"	// ESqlPanicInternalError
#include "SqlUtil.h"	// TSqlCompactionMode

//Forward declarations
struct sqlite3;
struct sqlite3_stmt;
class CSqlSecurityPolicy;
class TSqlSrvFileData;
class RFs;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////        TSqlDbSysSettings class         ////////////////////////////////////////         
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Stores database system settings to database system settings table.
Stores database security policies to database security policy table.
Loads database system settings from database system settings table.
Loads database security policies from database security policy table.
Reindexes a database that is being 'Open'ed or 'Attach'ed if the collation
dll has changed, and updates the system settings table with the new dll name.
Configures a database that is being 'Open'ed according to the operations
specified in its configuration file, if such a file exists and has not yet
been processed, and updates the system settings table with the version of
the file that was processed.

@internalComponent
*/
NONSHARABLE_CLASS(TSqlDbSysSettings)
	{
public:
	TSqlDbSysSettings(sqlite3* aDbHandle);
	
	void StoreSecurityPolicyL(const CSqlSecurityPolicy& aSecurityPolicyCon);
	void StoreSettingsL(const TDesC& aDbName, const TDesC& aCollationDllName, TInt aDbConfigFileVersion, TSqlCompactionMode aCompactionMode);
	
	void LoadSecurityPolicyL(CSqlSecurityPolicy& aSecurityPolicyCon);
	void LoadSettingsL(const TDesC& aDbName, TDes& aCollationDllName, TInt& aDbConfigFileVersion, TSqlCompactionMode& aCompactionMode);

	void ReindexDatabaseL(const TDesC& aDbName, const TDesC& aCurrentCollationDllName);
	void ConfigureDatabaseL(TInt aStoredDbConfigFileVersion, const TSqlSrvFileData& aFileData, const TDesC& aDbName);
	
private:
	void StoreSecurityPoliciesL(const CSqlSecurityPolicy& aSecurityPolicyCon);
	void StoreSecurityPolicyL(sqlite3_stmt* aStmtHandle, TInt aObjType, const TDesC& aObjName, 
							  TInt aPolicyType, const TSecurityPolicy& aPolicy);
	TInt BindSecurityPolicyPrm(sqlite3_stmt* aStmtHandle, TInt aObjType, const TDesC& aObjName, 
							   TInt aPolicyType, const TSecurityPolicy& aPolicy);
	TSecurityPolicy ReadCurrSecurityPolicyL(sqlite3_stmt* aStmtHandle, TInt& aObjType, TPtrC& aObjName, TInt& aPolicyType);
	void StoreDefaultSecurityPolicy(CSqlSecurityPolicy& aSecurityPolicyCon, const TSecurityPolicy& aPolicy, TInt aDbPolicySetFlag);
	void StoreDbSecurityPolicyL(CSqlSecurityPolicy& aSecurityPolicyCon, TInt aPolicyType, 
								const TSecurityPolicy& aPolicy, TInt& aDbPolicySetFlag);
	void StoreDbObjSecurityPolicyL(CSqlSecurityPolicy& aSecurityPolicyCon, TInt aObjType, const TDesC& aObjName,
								   TInt aPolicyType, const TSecurityPolicy& aPolicy);
	
	void GetSettingsL(const TDesC& aDbName, TDes& aCollationDllName, TInt& aDbConfigFileVersion, TInt& aSettingsVersion, TSqlCompactionMode& aCompactionMode);	
	TBool SettingsTableExistsL(const TDesC& aDbName);
	TBool IndexExistsL(const TDesC& aDbName, const TDesC& aCollationName);
		
	void ExecuteConfigurationUpdateL(const TSqlSrvFileData& aFileData, const TDesC& aMatchingConfigFile,
								  	 TInt aDbConfigFileVersion, const TDesC& aDbName);
	void DoExecuteDbConfigFileOpsL(RFs& aFs, const TDesC& aConfigFilePath, const TDesC& aDbName);
	void ParseFileL(const TDesC& aBuffer, const TDesC& aDbName);
	void ProcessStatementL(const TDesC& aLine, const TDesC& aDbName);
		
private:
	sqlite3* 	iDbHandle;
	};

#endif//__SQLSRVDBSYSSETTINGS_H__
