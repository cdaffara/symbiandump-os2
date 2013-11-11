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
// DBMS server-session and support classes - "DBMS security" related - full support
// 
//

#include <s32file.h>
#include "D32Strings.h"
#include "SD_STD.H"
#include "Sd_DbList.h"

using namespace DBSC;

CDbsSession::CDbsSession() :
	iDbPolicyRqColl(TLinearOrder< TPair<TInt, TDbPolicyRequest> > (&Compare<TInt, TDbPolicyRequest>))
	{
	}

/**
New "DBMS security" related messages processed here!
@param aMessage DBMS server message
@param aDbsFunction DBMS server function code
@return An error code (< 0) or a DBMS server session object handle (EDbsDatabase, EDbsIncremental,...).
*/
TInt CDbsSession::ExtServiceL(const RMessage2& aMessage, TDbsFunction aDbsFunction)
	{
	TInt handle = 0;
	switch(aDbsFunction)
		{
	case EDbsCreateDatabase:
		handle=CreateDatabaseL(aMessage);
		break;
	case EDbsDatabaseList:
		handle=GetDatabaseListL(aMessage);
		break;
	case EDbsCopyDatabase:
		CopyDatabaseL(aMessage);
		break;
	case EDbsDeleteDatabase:
		DeleteDatabaseL(aMessage);
		break;
	case EDbsGetSecurityPolicy:
		GetSecurityPolicyL(aMessage);
		break;
	case EDbsGetBackupPath:
		GetBackupPathL(aMessage);
		break;
	case EDbsGetBackupPaths:
		handle=GetBackupPathsL(aMessage);
		break;
	default:
		handle = KErrNotSupported;
		break;
		}
	return handle;
	}

/**
Extracts aMessage's "aIndex" argument (which is expected to be a file name) and
stores it to CDbsServer::iFileName data member.
@param aIndex The index of RMessage parameter
@param aMessage
@return A descriptor of the file name,extracted from aMessage and stored in CDbsServer::iFileName.
*/
const TDesC& CDbsSession::ReadFileNameL(TInt aIndex, const RMessage2& aMessage)
	{
	TDes& name = Server().FileName();
	aMessage.ReadL(aIndex, name);
	return name;
	}

/**
Extracts database name (aMessage's arg 0) and database format string (aMessage's arg 1)
and use them to extract database properties, such as: database UID, access type (secure/non-secure),
full database file path, database format string, drive number.
@return A pointer to a TDbProps object, which contains some properties, extracted from the database name.
*/
TDbProps* CDbsSession::ExtractDbPropsLC(const RMessage2& aMessage)
	{
	const TDesC& dbName = ReadFileNameL(0, aMessage);
	const TDesC& dbFormat = ReadName0L(1, aMessage);
	return Server().DbPropsFactory().ExtractLC(dbName, dbFormat);
	}

/**
This method creates new EDbsDatabase type object.
The related MPolicy interface will be retrieved and
put together with the EDbsDatabase object in TEntry list.

The initial contact for a database. Open a database source
return the database handle for the client
*/
TInt CDbsSession::OpenDatabaseL(const RMessage2& aMessage)
	{
	TDbProps* dbProps = ExtractDbPropsLC(aMessage);
	const MPolicy* policy = Server().PolicyProxy().DbPolicyL(dbProps->iDbPolicyRequest);
	Server().PolicyProxy().CheckL(aMessage, *policy);
	TInt dbHandle = DoOpenDatabaseL(aMessage, *dbProps);
	CleanupStack::PopAndDestroy(dbProps);
	return dbHandle;
	}


/**
SYMBIAN_REMOVE_TRIVIAL_ENCRYPTION version of the method.
Opens a database. It is used by both: OpenDatabase() and CreateDatabase() methods.
@param aMessage DBMS server message:EDbsCreateDatabase or EDbsOpenDatabase.
@param aDbProps A TDbProps object created from the database name and format string.
@return A handle to the opened/created database object.
@leave One of the system-wide error codes.
*/
TInt CDbsSession::DoOpenDatabaseL(const RMessage2& aMessage, const TDbProps& aDbProps)
	{
	CDbsConnection* dbConnection = Sources().OpenLC(Server().Fs(), aDbProps.iPath, aDbProps.iFormatStr);
	CDbObject* dbObj = DoAuthenticateL(dbConnection, aMessage);
	dbConnection->Attach(dbObj);
	CleanupStack::Pop(dbConnection);

	//dbObj does not have to be pushed in the cleanup stack!
	//NewDbEntryL() will take care of its destruction, if the database entry cannot be created.
	//NewDbEntryL() will destroy the connection also in this case.
	TInt dbHandle = 0;
	NewDbEntryL(dbObj, aDbProps.iDbPolicyRequest, dbHandle);
	return dbHandle;
	}

//SYMBIAN_REMOVE_TRIVIAL_ENCRYPTION version of the method.
//Authenticates a database.
CDbObject* CDbsSession::DoAuthenticateL(CDbsConnection* aDbsConnection, const RMessage2&)
	{
	__ASSERT(aDbsConnection);
	CDbSource& src = aDbsConnection->Source().Source();
	return src.AuthenticateL();
	}
	

//Adds a new database entry to the session list of database session objects.
void CDbsSession::NewDbEntryL(CDbObject* aDbObject, const TDbPolicyRequest& aDbPolicyRequest, TInt& aDbHandle)
	{
	__ASSERT(aDbObject);
	const MPolicy* policy = Server().PolicyProxy().DbPolicyL(aDbPolicyRequest);

	aDbHandle = DoAdd(aDbObject, EDbsDatabase, policy);

	//Store the database uid for later use
	TInt err = iDbPolicyRqColl.Insert(aDbHandle, aDbPolicyRequest);
	if(err != KErrNone)
		{//If iDbPolicyRqColl.Insert() fails, then remove the object from TEntry list and then return.
		TEntry& e = Object(aDbHandle);
		Free(e);
		User::Leave(err);
		}
	}

/**
Converts RDbs::TPolicyType parameter value to the internally used DBSC::TPolicyType value.
@param aPolicyType Security policy type - client side
@return Security policy type used on the server side.
@leave KErrArgument if it is an invalid security policy type
*/
static TPolicyType ConvertPolicyTypeL(RDbs::TPolicyType aPolicyType)
	{
	TPolicyType policyType = static_cast <TPolicyType> (1 << aPolicyType);
	if(policyType > EPTLast || policyType <= EPTNone)
		{
		__LEAVE(KErrArgument);
		}
	return policyType;
	}

/**
Creates secure shared database.
@param aMessage DBMS server message: EDbsCreateDatabase.
@return A handle to the created database object.
@leave One of the system-wide error codes, including:
       KErrNotSupported An attempt to create non-secure shared database
	   KErrAlreadyExists The database with the supplied name already exists
*/
TInt CDbsSession::CreateDatabaseL(const RMessage2& aMessage)
	{
	TDbProps* dbProps = ExtractDbPropsLC(aMessage);
	if(dbProps->iDbPolicyRequest.iAccessType == EATNonSecure)
		{//This method works only for secure shared databases
		__LEAVE(KErrNotSupported);
		}
	const MPolicy* policy = Server().PolicyProxy().DbPolicyL(dbProps->iDbPolicyRequest);
	Server().PolicyProxy().CheckL(aMessage, *policy);
	//Leave if the file is already there
	::TEntry fileEntry;
	TBool dbFileExist = Server().Fs().Entry(dbProps->iPath, fileEntry) == KErrNone;
	if(dbFileExist)
		{
		__LEAVE(KErrAlreadyExists);
		}
	TInt dbHandle = 0;
	TRAPD(err, dbHandle = DoCreateDatabaseL(aMessage, *dbProps));
	if(err != KErrNone)
		{//Cleanup if the creation fails
		// Although the file delete below could return at error since we are
		// already on an error-path a design decision has been made to ignore the
		// error in favor of the one returned by DoCreateDatabaseL()

		// If a debug build - record error
		TInt fileDeleteErr = Server().Fs().Delete(dbProps->iPath);
		#ifdef _DEBUG
			if (fileDeleteErr != KErrNone)
			{
				RDebug::Print(_L("CDbsSession::CreateDatabaseL - Failed to delete file. Error = %d"), fileDeleteErr);
			}
		#endif

		__LEAVE(err);
		}
	CleanupStack::PopAndDestroy(dbProps);
	return dbHandle;
	}

//Creates secure shared database.
//Originaly, the database were always created on the client side, using ::CreateDatabaseL() call.
//I am not very sure how this function works and prefer to call ::CreateDatabaseL() to create
//the database on the server side, then delete it and the open it in the same way, as it 
//worked before for opening/sharing databases on the server side.
TInt CDbsSession::DoCreateDatabaseL(const RMessage2& aMessage, const TDbProps& aDbProps)
	{
	CDbDatabase* db = ::CreateDatabaseL(TDbFormat::ECreate, Server().Fs(), aDbProps.iPath, aDbProps.iFormatStr);
	delete db;
	TInt dbHandle = DoOpenDatabaseL(aMessage, aDbProps);
	return dbHandle;
	}

/**
Copies an existing secure shared database to a new database.
The new database will have the same security policy as the old one.
@param aMessage DBMS server message (EDbsCopyDatabase)
@leave One of the system-wide error codes, including KErrArgument - a null uid supplied
       as an argument.
*/
void CDbsSession::CopyDatabaseL(const RMessage2& aMessage)
	{
	RDbPropsFactory& dbPropsFactory = Server().DbPropsFactory();
	TUid uid;
	uid.iUid = aMessage.Int2();
	if(uid == KNullUid)
		{
		__LEAVE(KErrArgument);
		}
	//Do not change the order, because ReadFileNameL() uses the same place to store the names.
	const TDesC& srcDbName = ReadFileNameL(0, aMessage);
	TDbProps* srcDbProps = dbPropsFactory.ExtractLC(srcDbName, uid);
	const TDesC& destDbName = ReadFileNameL(1, aMessage);
	TDbProps* destDbProps = dbPropsFactory.ExtractLC(destDbName, uid);

	const MPolicy* policy = Server().PolicyProxy().DbPolicyL(srcDbProps->iDbPolicyRequest);
	Server().PolicyProxy().CheckL(aMessage, *policy);

	CFileMan* fileMan = CFileMan::NewL(Server().Fs());
	CleanupStack::PushL(fileMan);
	__LEAVE_IF_ERROR(fileMan->Copy(srcDbProps->iPath, destDbProps->iPath, 0));
	//"Copy" operation executed without errors. Now it is a time to turn off the read-only
	//flag of the target file (which may be on if the source files is on a read-only drive)
	__LEAVE_IF_ERROR(Server().Fs().SetAtt(destDbProps->iPath, 0, KEntryAttReadOnly));
	CleanupStack::PopAndDestroy(3);//srcDbProps, destDbProps, fileMan
	}

/**
Deletes secure shared database
@param aMessage DBMS server message (EDbsDeleteDatabase)
@leave One of the system-wide error codes, including KErrArgument - a null uid supplied
       as an argument.
*/
void CDbsSession::DeleteDatabaseL(const RMessage2& aMessage)
	{
	TUid uid;
	uid.iUid = aMessage.Int1();
	if(uid == KNullUid)
		{
		__LEAVE(KErrArgument);
		}
	const TDesC& dbName = ReadFileNameL(0, aMessage);
	TDbProps* dbProps = Server().DbPropsFactory().ExtractLC(dbName, uid);
	const MPolicy* policy = Server().PolicyProxy().DbPolicyL(dbProps->iDbPolicyRequest);
	Server().PolicyProxy().CheckL(aMessage, *policy);
	__LEAVE_IF_ERROR(Server().Fs().Delete(dbProps->iPath));
	CleanupStack::PopAndDestroy(dbProps);
	}

/**
Gets the list of names of datatbases, which have the same uid.
@param aMessage DBMS server message (EDbsDatabaseList)
@return A stream handle to a stream with the database names found.
@leave One of the system-wide error codes, including KErrArgument - a null uid supplied
       as an argument.
*/
TInt CDbsSession::GetDatabaseListL(const RMessage2& aMessage)
	{
	CDbNamesFactory* dbNamesFactory = CDbNamesFactory::NewLC();
	TDriveNumber driveNumber;
	TDbPolicyRequest dbPolicyRequest;
	CDbNamesFactory::ExtractArgs(aMessage, driveNumber, dbPolicyRequest);
	if(dbPolicyRequest.iUid == KNullUid)
		{
		__LEAVE(KErrArgument);
		}
	const MPolicy* policy = Server().PolicyProxy().DbPolicyL(dbPolicyRequest);
	Server().PolicyProxy().CheckL(aMessage, *policy);
	CDbDatabaseNames* dbNames = dbNamesFactory->DbNamesLC(driveNumber, dbPolicyRequest, Server().DbPropsFactory(), Server().Fs());
	//NewStreamL() will take care about destroying dbNames.
	TInt streamHandle = NewStreamL(dbNames, Externalizer(dbNames), aMessage, policy);
	CleanupStack::PopAndDestroy(dbNamesFactory);
	return streamHandle;
	}

/**
Gets database/table security policy.
@param aMessage DBMS server message (EDbsGetSecurityPolicy)
@leave One of the system-wide error codes, including KErrArgument - a null uid supplied
       as an argument.
*/
void CDbsSession::GetSecurityPolicyL(const RMessage2& aMessage)
	{
	//No security policy check.
	TUid dbUid = TUid::Uid(aMessage.Int0());
	if(dbUid == KNullUid)
		{
		__LEAVE(KErrArgument);
		}
	TPolicyType policyTypeRq = ::ConvertPolicyTypeL(static_cast <RDbs::TPolicyType> (aMessage.Int1() & ~KTablePolicyMaskBit));
	TBool tblPolicyRq = aMessage.Int1() & KTablePolicyMaskBit;
	if(tblPolicyRq)
		{
		ReadName0L(2, aMessage);
		if(Server().Name0() == KNullDesC)
			{
			__LEAVE(KErrArgument);
			}
		}
	TDbPolicyRequest dbPolicyRequest;
	dbPolicyRequest.iUid = dbUid;
	dbPolicyRequest.iAccessType = EATSecure;
	const MPolicy* policy = tblPolicyRq ? Server().PolicyProxy().TblPolicyL(dbPolicyRequest, Server().Name0()) : 
										  Server().PolicyProxy().DbPolicyL(dbPolicyRequest);
	__ASSERT(policy);
	TSecurityPolicy secPolicy;
	__LEAVE_IF_ERROR(policy->Get(policyTypeRq, secPolicy));
	aMessage.WriteL(3, secPolicy.Package());
	}

/**
The function extracts backup&restore process SID from aMessage argument (parameter 0).
@param aMessage DBMS server message - EDbsGetBackupPath or EDbsGetBackupPaths.
@return Backup&restore process SID
@leave KErrArgument 0 or ECapability_None backup&restore process SID
@internalComponent
*/
static TSecureId BackupSIDL(const RMessage2& aMessage)
	{
	TSecureId backupSID = TSecureId(aMessage.Int0());
	if(backupSID.iId == 0 || backupSID.iId == (TUint32)ECapability_None)
		{
		__LEAVE(KErrArgument);
		}
	return backupSID;
	}
	
/**
The function extracts database security policy UID from aMessage argument (parameter 1).
@param aMessage DBMS server message - EDbsGetBackupPath or EDbsGetBackupPaths.
@return Database security policy UID
@leave KErrArgument Null database security policy UID
@internalComponent
*/
static TUid SecurityPolicyUidL(const RMessage2& aMessage)
	{
	TUid dbUid = TUid::Uid(aMessage.Int1());
	if(dbUid == KNullUid)
		{
		__LEAVE(KErrArgument);
		}
	return dbUid;		
	}

/**
The function gets the backup&restore process SID from the related database security policy,
identified by aDbUid argument.
@param aPolicyProxy A reference to CPolicyProxy object, which might be asked for particular 
                    database or table policy.
@param aBackupSID Backup&restore process SID, extracted from RMessage2 object.
@param aDbUid Database security policy UID, extracted from RMessage2 object.
@return Backup&restore process SID, which is part of the database security policy.
@leave KErrPermissionDenied - the supplied process SID does not match the database backup&
						restore SID or the database backup&restore SID is 0 or ECapability_None. 
@internalComponent
*/
static TSecureId RegisteredBackupSIDL(CPolicyProxy& aPolicyProxy, TSecureId aBackupSID, TUid aDbUid)
	{
	TSecureId regBackupSID = aPolicyProxy.BackupSIDL(aDbUid);
	if((regBackupSID == 0 || regBackupSID == (TUint32)ECapability_None) || aBackupSID != regBackupSID)
		{
		__LEAVE(KErrPermissionDenied);
		}
	return regBackupSID;
	}
	
/**
The method will return via aMessage argument the full path to the secure shared database, 
which name is packed in aMessage argument too.
@param aMessage DBMS server message (EDbsGetBackupPath)
@leave One of the system-wide error codes, including:
		- KErrArgument - 0 or ECapability_None process SID, null UID, 
						 null or invalid database name,
						 the database is not secure shared database;
		- KErrNotFound - the database file does not exist;
		- KErrPermissionDenied - the supplied process SID does not match the database backup&
						 restore SID or the database backup&restore SID is 0 or ECapability_None. 
@deprecated
*/
void CDbsSession::GetBackupPathL(const RMessage2& aMessage)
	{
	//Backup&restore process SID
	TSecureId backupSID = ::BackupSIDL(aMessage);
	//Security policy UID
	TUid dbUid = ::SecurityPolicyUidL(aMessage);
	//Database name and drive, format: <drive>:<name>.<ext>
	ReadName0L(2, aMessage);
	if(Server().Name0() == KNullDesC)
		{
		__LEAVE(KErrArgument);
		}
	//Database path
	RDbPropsFactory& dbPropsFactory = Server().DbPropsFactory();
	TDbProps* dbProps = dbPropsFactory.ExtractLC(Server().Name0(), dbUid);
	if(dbProps->iDbPolicyRequest.iAccessType != EATSecure)
		{
		__LEAVE(KErrArgument);
		}
	//Check if the database file exists
	::TEntry fileEntry;
	TBool dbFileExist = Server().Fs().Entry(dbProps->iPath, fileEntry) == KErrNone;
	if(!dbFileExist)
		{
		__LEAVE(KErrNotFound);
		}
	//Get and check backup&restore SID 
	TSecureId regBackupSID = ::RegisteredBackupSIDL(Server().PolicyProxy(), backupSID, dbUid);
    //
	aMessage.WriteL(3, dbProps->iPath);
	//
	CleanupStack::PopAndDestroy(dbProps);
	}

/**
This function processes "aFileEntries" array, which is a result of TFindFile::FindWildByDir()
or TFindFile::FindWild() calls. In a loop the function will get an element from "aFileEntries" 
array, copy it to a temporary string adding the drive and the path, and will add that string
to "aDatabasePaths" array.
Note: If the created full file path length is bigger than KDbMaxStrLen characters, then the 
	  string will not be added to "aDatabasePaths" array!
@param aFileEntries An array of file names, result of TFindFile::FindWildByDir() or 
					TFindFile::FindWild() calls.
@param aFileSpec	A string, containing the drive and the directory of the file names in
					aFileEntries array.
@param aDatabasePaths  Output argument. Each file name from aFileEntries array will be "decorated"
					with the drive and path and then the created new string will be added to 
					aDatabasePaths array.
@leave One of the system-wide error codes, including KErrNoMemory.
@internalComponent
*/
static void ProcessFileEntriesL(CDir& aFileEntries, const TDesC& aFileSpec, 
								CDbStrings& aDatabasePaths)
	{
	TParse parse;
	__LEAVE_IF_ERROR(parse.Set(aFileSpec, NULL, NULL));
	TInt cnt = aFileEntries.Count();
	for(TInt i=0;i<cnt;++i)
		{
		TFileName fileName;
		fileName.Copy(parse.DriveAndPath());
		const ::TEntry& entry = aFileEntries[i];
		fileName.Append(entry.iName);
		if(fileName.Length() < KDbMaxStrLen)
			{
	    	aDatabasePaths.AddL(fileName);
			}
		}
	}

/**
Gets a list of paths of the databases, which have the same security policy uid.
@param aMessage DBMS server message (EDbsGetBackupPaths)
@return A stream handle to a stream with the database names found.
@leave One of the system-wide error codes, including:
		- KErrArgument - 0 or ECapability_None process SID, null database security policy UID;
		- KErrPermissionDenied - the supplied process SID does not match databases backup&
						 restore SID or databases backup&restore SID is 0 or ECapability_None. 
*/
TInt CDbsSession::GetBackupPathsL(const RMessage2& aMessage)
	{
	//Backup&restore process SID
	TSecureId backupSID = ::BackupSIDL(aMessage);
	//Security policy UID
	TUid dbUid = ::SecurityPolicyUidL(aMessage);
	//Get and check backup&restore SID 
	TSecureId regBackupSID = ::RegisteredBackupSIDL(Server().PolicyProxy(), backupSID, dbUid);
	//Get the related database security policy
	TDbPolicyRequest dbPolicyRequest = {dbUid, EATSecure};
	const MPolicy* policy = Server().PolicyProxy().DbPolicyL(dbPolicyRequest);
	//
	CDbStrings* dbPaths = CDbStrings::NewLC();
	//DBMS server - private data path. CDbServer::iFileName used as a storage for the path.
	__LEAVE_IF_ERROR(Server().Fs().PrivatePath(Server().FileName()));
	//Construct search pattern. CDbServer::iName1 used as a storage for the search pattern.
	RDbPropsFactory::ConstructCommonPart(dbUid, Server().Name1());
	Server().Name1().Append('*');
	//Search....
	TFindFile findFile(Server().Fs());
	CDir* fileEntries = NULL;
	TInt err = findFile.FindWildByDir(Server().Name1(), Server().FileName(), fileEntries);
	if(err == KErrNone)
		{
		do
			{
			__ASSERT(fileEntries);
			CleanupStack::PushL(fileEntries);
			::ProcessFileEntriesL(*fileEntries, findFile.File(), *dbPaths);
			CleanupStack::PopAndDestroy(fileEntries);
			fileEntries = NULL;
			} while(findFile.FindWild(fileEntries) == KErrNone);
		}
	if(err != KErrNotFound && err != KErrNone)
		{
		__LEAVE(err);
		}
	//NewStreamL() will take care about destroying dbPaths.
	TInt streamHandle = NewStreamL(dbPaths, Externalizer(dbPaths), aMessage, policy);
	return streamHandle;
	}
