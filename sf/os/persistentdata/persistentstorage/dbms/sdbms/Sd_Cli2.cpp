// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// DBMS client/server session class - "DBMS security" related - full security support
// 
//

#include "SD_STD.H"

/**
Retrieves a list of names of secure shared databases, which share the same security policy,
as determined by the supplied UID.
If a database name is longer than KDbMaxName, it will not be added to the list. 

@param aDrive The drive number to be searched.
@param aPolicyUid Database security policy UID.
@return A list with names of the found databases, which have the same database security uid.
		The database name output format is: \<name\>.\<ext\>. The caller is resonsible for deleting 
		the database names list.
@leave KErrNoMemory - not enough memory for the operation to be done
@leave KErrArgument - invalid UID parameter (including KNullUid value)
@leave KErrBadName - invalid drive number (not in A-Z range)
@leave KErrNotReady - the drive is not presented in the system
@leave Some other system-wide error codes

@publishedAll
@released
*/
EXPORT_C CDbDatabaseNames* RDbs::DatabaseNamesL(TDriveNumber aDrive, TUid aPolicyUid)
	{
	TIpcArgs args(aDrive, aPolicyUid.iUid);
	RReadStream in(HDbsBuf::NewLC(*this, EDbsDatabaseList, args));
	CDbDatabaseNames* dbNames = CDbDatabaseNames::NewLC();
	in >> *dbNames;
	CleanupStack::Pop(dbNames);
	CleanupStack::PopAndDestroy();//HDbsBuf
	return dbNames;
	}

/**
Copies an existing secure shared database to a new secure shared database. 
The new database will have the same security policy as the old one.
The maximum length of the target database name (with the extension) is KDbMaxName.

@param aSrcDbName Source database name (\<drive\>:\<name\>.\<ext\> format)
@param aDestDbName Destination database name (\<drive\>:\<name\>.\<ext\> format)
@param aPolicyUid The database security policy UID. The destination database will have 
                  the same policy UID.
@return KErrNone if successful, otherwise one of the system-wide error codes, including:
		KErrPermissionDenied - the caller has not enough rights to do the operation or
		                   the destination drive is a ROM drive;
		KErrArgument - invalid source or destination database names (null name, too long name, only drive letter);
		               invalid or null UID;
		KErrNotReady - the drive in database name is not presented in the system;
		KErrNotFound - the source database not found;
		KErrInUse - the source database is in use;
		KErrAlreadyExists - the destination database already exists;
 		KErrNoMemory - not enough memory for the operation to be done;

@capability Note For a secure shared database, the caller must satisfy the schema 
            access policy for the database.

@publishedAll
@released
*/
EXPORT_C TInt RDbs::CopyDatabase(const TDesC& aSrcDbName, const TDesC& aDestDbName, TUid aPolicyUid)
	{
	TIpcArgs args(&aSrcDbName, &aDestDbName, aPolicyUid.iUid);
	return SendReceive(DbsMessage(EDbsCopyDatabase, KDbsSessionHandle), args);
	}

/**
Deletes an existing secure shared database.

@param aDbName Source database name (\<drive\>:\<name\>.\<ext\> format)
@param aPolicyUid Database security policy UID.
@return KErrNone if successful, otherwise one of the system-wide error codes, including:
		KErrInUse (if the database is in use at the moment);
		KErrNotFound - the database not found;
		KErrPermissionDenied - the caller has not enough rights to do the operation;

@capability Note For a secure shared database, the caller must satisfy the schema 
            access policy for the database.

@publishedAll
@released
*/
EXPORT_C TInt RDbs::DeleteDatabase(const TDesC& aDbName, TUid aPolicyUid)
	{
	TIpcArgs args(&aDbName, aPolicyUid.iUid);
	return SendReceive(DbsMessage(EDbsDeleteDatabase, KDbsSessionHandle), args);
	}

/**
Returns in aPolicy output parameter requested database/table security policy of type aPolicyType.
@param aPolicyUid Database security policy UID
@param aTableName Table name.
@param aMask Bit-field: it includes ther policy type: EReadPolicy, EWritePolicy, ESchemaPolicy
             and the request type - database or table.
@param aPolicy It will be initialized with the requested security policy data after a successfull call.
@return KErrNone if successful, otherwise some of the system-wide error codes, including:
				KErrArgument - some of the arguments has an invalid value.
				KErrNotSupported - the method has been called with aMask containing ESchemaPolicy
				                   for a table object.

@publishedAll
@released
*/
TInt RDbs::GetPolicy(TUid aPolicyUid, const TDesC& aTableName, TUint aMask, 
					 TSecurityPolicy& aPolicy)
	{
	TBuf8<sizeof(TSecurityPolicy)> spData;
	TIpcArgs args(aPolicyUid.iUid, aMask, &aTableName, &spData);
	TInt err = SendReceive(DbsMessage(EDbsGetSecurityPolicy, KDbsSessionHandle), args);
	if(err == KErrNone)
		{
		err = aPolicy.Set(spData);
		}
	return err;
	}

/**
Returns in the aDbPolicy output parameter the requested database security policy of type aPolicyType.

@param aPolicyUid Database security policy UID.
@param aPolicyType Policy type: EReadPolicy, EWritePolicy, ESchemaPolicy.
@param aDbPolicy It will be initialized with the requested security policy data after a successfull call.
@return KErrNone if successful, otherwise one of the system-wide error codes, including
				KErrArgument - some of the arguments has an invalid value.

@publishedAll
@released
*/
EXPORT_C TInt RDbs::GetDatabasePolicy(TUid aPolicyUid, TPolicyType aPolicyType, 
									  TSecurityPolicy& aDbPolicy)
	{
	return GetPolicy(aPolicyUid, KNullDesC, aPolicyType, aDbPolicy);
	}

/**
Returns in the aTablePolicy output parameter the requested table security policy of type aPolicyType.

@param aPolicyUid Database security policy UID.
@param aTableName Table name.
@param aPolicyType Policy type: EReadPolicy, EWritePolicy.
@param aTablePolicy It will be initialized with the requested security policy data after a successfull call.
@return KErrNone if successful, otherwise one of the system-wide error codes, including:
		KErrArgument - some of the arguments has an invalid value.
		KErrNotSupported - the method has been called with aPolicyType = ESchemaPolicy;

@publishedAll
@released
*/
EXPORT_C TInt RDbs::GetTablePolicy(TUid aPolicyUid, const TDesC& aTableName, TPolicyType aPolicyType, 
								   TSecurityPolicy& aTablePolicy)
	{
	return GetPolicy(aPolicyUid, aTableName, aPolicyType | KTablePolicyMaskBit, aTablePolicy);
	}

/**
Returns in the aDbPolicy and aTablePolicy output parameters the requested database and table 
security policies of type aPolicyType.

@param aPolicyUid Database security policy UID.
@param aTableName Table name.
@param aPolicyType Policy type: EReadPolicy, EWritePolicy.
@param aDbPolicy It will be initialized with the requested security policy data after a successfull call.
@param aTablePolicy It will be initialized with the requested security policy data after a successfull call.
@return KErrNone if successful, otherwise one of the system-wide error codes, including:
		KErrArgument - some of the arguments has an invalid value.
		KErrNotSupported - the method has been called with aPolicyType = ESchemaPolicy;

@publishedAll
@released
*/
EXPORT_C TInt RDbs::GetTablePolicies(TUid aPolicyUid, const TDesC& aTableName, TPolicyType aPolicyType, 
									 TSecurityPolicy& aDbPolicy, TSecurityPolicy& aTablePolicy)
	{
	TInt err = GetDatabasePolicy(aPolicyUid, aPolicyType, aDbPolicy);
	if(err == KErrNone)
		{
		err = GetTablePolicy(aPolicyUid, aTableName, aPolicyType, aTablePolicy);
		}
	return err;
	}

/**
The method will fill out aBackupPath argument with the full path of aDbName secure
shared database.
@param aRequesterSID Security ID of the process which is supposed to backup or restore 
					 the database. 0 or ECapability_None are invalid values for 
					 aRequesterSID parameter.
@param aDbName       Secure shared database name, which path will be set in aBackupPath
                     parameter. The name's format is \<drive\>:\<name\>.\<ext\>
@param aDbPolicyUid  Database security policy UID.
@param aBackupPath   An output parameter. After a successfull call, the DBMS server
 					 will fill out the full database path there. aBackupPath must offer
 					 enough space to get the whole database path. Probably the best
 					 aBackupPath length is KMaxPath value.
@return KErrNone if successful, otherwise one of the system-wide error codes, including:
		- KErrArgument - 0 or ECapability_None process SID, null UID, 
						 null or invalid database name,
						 the database is not secure shared database;
		- KErrNotFound - the database file does not exist;
		- KErrPermissionDenied - the supplied process SID does not match the database backup&
						 restore SID or the database backup&restore SID is 0 or ECapability_None. 
@deprecated
*/
EXPORT_C TInt RDbs::GetBackupPath(TSecureId aRequesterSID, const TDesC& aDbName, 
								  TUid aDbPolicyUid, TDes& aBackupPath)
	{
	TIpcArgs args(aRequesterSID.iId, aDbPolicyUid.iUid, &aDbName, &aBackupPath);
	return SendReceive(DbsMessage(EDbsGetBackupPath, KDbsSessionHandle), args);
	}

/**
Retrieves a list of paths of secure shared databases, which share the same security policy,
as determined by the supplied aDbPolicyUid parameter.
Note: If there is a database file which full path length is bigger than KDbMaxStrLen characters, 
	  then this file will not be added to the returned CDbStrings array.

@param aRequesterSID Security ID of the process which is supposed to backup or restore 
					 the database. 0 and ECapability_None are invalid values for 
					 aRequesterSID parameter.
@param aDbPolicyUid  Database security policy UID.
@return A list with paths of the found databases, which have the same database security uid.
		The caller is resonsible for deleting the database paths list.
@leave KErrArgument - 0 or ECapability_None process SID, null database security UID.
@leave KErrPermissionDenied - the supplied process SID does not match the database backup&
						 restore SID or the database backup&restore SID is 0 or ECapability_None. 
@leave Some other system-wide error codes

@publishedAll
@released
*/
EXPORT_C CDbStrings* RDbs::BackupPathsL(TSecureId aRequesterSid, TUid aDbPolicyUid)
	{
	TIpcArgs args(aRequesterSid.iId, aDbPolicyUid.iUid);
	RReadStream in(HDbsBuf::NewLC(*this, EDbsGetBackupPaths, args));
	CDbStrings* dbPaths = CDbStrings::NewLC();
	in >> *dbPaths;
	CleanupStack::Pop(dbPaths);
	CleanupStack::PopAndDestroy();//HDbsBuf
	return dbPaths;
	}

/**
Creates a secure shared database.
Max allowed database name length (with the extension) is KDbMaxName symbols.

In this "client-server" mode the database can be shared with the other clients.

For creating a non-secure database, see RDbNamedDatabase::Create(), which first
argument is a RFs reference (or RDbNamedDatabase::Replace()).

@param aDbs A reference to DBMS session instance.
@param aDatabase Database name. The name format is: \<drive\>:\<name\>.\<ext\>
@param aFormat Database format string. The string format is: "SECURE[UID]", where UID
			   is the database security policy UID. "SECURE" keyword is case insensitive.
@return KErrNone if successful otherwise one of the system-wide error codes, including:
		KErrAlreadyExists - the database already exists;
		KErrNotSupported - invalid format string;
		KErrArgument - bad argument, including null/invaluid uids, the database name includes a path;
		KErrPermissionDenied - the caller has not enough rights to do the operation;

@capability Note For a secure shared database, the caller must satisfy the schema 
            access policy for the database.

@see RDbNamedDatabase::Create(RFs& aFs, const TDesC& aSource, const TDesC& aFormat)
@see RDbNamedDatabase::Replace(RFs& aFs, const TDesC& aSource, const TDesC& aFormat)

@publishedAll
@released
*/
EXPORT_C TInt RDbNamedDatabase::Create(RDbs& aDbs, const TDesC& aDatabase, const TDesC& aFormat)
	{
	TRAPD(r,iDatabase=CDbsSecureDatabase::NewL(aDbs,aDatabase,aFormat));
	return r;
	}
