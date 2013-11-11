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

#ifndef __SQLBUR_H__
#define __SQLBUR_H__

#include <e32base.h>
#include <f32file.h>
#include <f32file64.h>
#include <e32property.h>
#include <connect/abclient.h> // MactiveBackupDataClient
#include "SqlSrvBurInterface.h"

//Forward declarations
class CSqlBurCallback;

#ifdef SQL_BUR_PROPERTY_MONITOR_TEST

const TInt32 KSqlBurPropertyCategory = 0x10281e17;//the SQL server secure id (the bur tests have the same id, 
                                                  //in order to be able to operate with the property)
const TUint KSqlBurBackupRestoreKeyValue = 0x1234DDD1;

extern TInt TestModeSqlBurError;
#define SQL_BUR_TEST_SET_ERROR(err) TestModeSqlBurError = err 
//This macro is called at the end of CSqlBurEventMonitor::RunL()and CSqlBurEventMonitor::RunError() 
//CActiveScheduler::Stop() is called here to return the execution control back to the test code.
#define SQL_BUR_TEST_STOP() CActiveScheduler::Stop() 

#else

const TInt32 KSqlBurPropertyCategory = KUidSystemCategoryValue;
const TUint KSqlBurBackupRestoreKeyValue = conn::KUidBackupRestoreKey;

#define SQL_BUR_TEST_SET_ERROR(err) (void)0 
#define SQL_BUR_TEST_STOP() (void)0 

#endif

/**
The system category of the backup and restore property, that is used for notifying active backup clients
regarding beginning or the end of backup or restore process.
Different value is used in SQL tests, because only the secure backup server can set or get the value of
the  {KUidSystemCategoryValue, conn::KUidBackupRestoreKey} property. 
@internalComponent
*/
const TUid KSqlBurPropertyCategoryUid = {KSqlBurPropertyCategory};

/**
The backup and restore property key, that is used for notifying active backup clients
regarding beginning or the end of backup or restore process. 
Different value is used in SQL tests, because only the secure backup server can set or get the value of
the  {KUidSystemCategoryValue, conn::KUidBackupRestoreKey} property. 
@internalComponent
*/
const TUint KSqlBurBackupRestoreKey = KSqlBurBackupRestoreKeyValue;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////                     Backup database file header format                           ///////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////// No version (Version 0)
//  8 chars          8 chars          8 chars             up to 256 characters (512 bytes)
// <32-bit checksum><32-bit filesize><32-bit filenamelen><filename - UTF16 encoded>

//////             Version 2
//  8 chars          8 chars   4 chars     16 chars         8 chars             up to 256 characters (512 bytes)
// <32-bit checksum><FFFFAA55><Version N#><64-bit filesize><32-bit filenamelen><filename - UTF16 encoded>

const TInt KSqlBurHeaderVersion = 2;             //Current backup database file header version

const TUint32 KSqlBurMagicNum = 0xFFFFAA55;      //Magic number. If the "old database file size" field in the header
                                                 //has this value, then the header version is 2+
const TInt KSqlBurMaxHeaderSize = 256 + KMaxFileName; //The size of the buffer used for the operations on the header

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
An object of this class is created to monitor the backup & restore property
{KUidSystemCategory, KUidBackupRestoreKey}.
If the property gets set, the CSqlBurEventMonitor object will create a CSqlBurClient
instance to handle the backup or restore.  

@internalComponent
*/
class CSqlBurEventMonitor : public CActive
	{
public:
	static CSqlBurEventMonitor* NewL(MSqlSrvBurInterface& aBufInterface);
	virtual ~CSqlBurEventMonitor();
#ifdef SQL_BUR_PROPERTY_MONITOR_TEST
	inline conn::CActiveBackupClient* ActiveBackupClient();
	inline CSqlBurCallback* SqlBurCallback();
#endif
		
private:
	CSqlBurEventMonitor(MSqlSrvBurInterface& aInterface);
	void ConstructL();
	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);
	void CreateContentL();
	void DestroyContent();

private:
	RProperty 					iBurProperty;//B&R property published by the B&R server. SQL server subscribes for notifications
	MSqlSrvBurInterface& 		iBurInterface;//Provided by the SQL server
	conn::CActiveBackupClient*	iActiveBackupClient;//The connection with the B&R server
	CSqlBurCallback*			iSqlBurCallback;//A "Database file list" interface provided by the SQL server
	
	};

#ifdef SQL_BUR_PROPERTY_MONITOR_TEST

inline conn::CActiveBackupClient* CSqlBurEventMonitor::ActiveBackupClient()
	{
	return iActiveBackupClient;
	}

inline CSqlBurCallback* CSqlBurEventMonitor::SqlBurCallback()
	{
	return iSqlBurCallback;
	}

#endif

/**
This class is called by the Backup and Restore Framework
when a backup or restore is requested by the user
It implements the active proxy backup and restore as
defined in the MActiveBackupDataClient interface.

@internalComponent
*/
class CSqlBurCallback : public CBase, public conn::MActiveBackupDataClient
	{
public:
	static CSqlBurCallback* NewL(MSqlSrvBurInterface& aBufInterface);
	virtual ~CSqlBurCallback();
	
	//Implementations of virtuals from MActiveBackupDataClient - full backup
	virtual void InitialiseGetProxyBackupDataL(TSecureId aSid, TDriveNumber aDrive);
	virtual TUint GetExpectedDataSize(TDriveNumber aDrive);
	virtual void GetBackupDataSectionL(TPtr8& aBuffer, TBool& aFinished);

	//Implementations of virtuals from MActiveBackupDataClient - full restore
	virtual void InitialiseRestoreProxyBaseDataL(TSecureId aSid, TDriveNumber aDrive);
	virtual void RestoreComplete(TDriveNumber aDrive);
	virtual void RestoreBaseDataSectionL(TDesC8& aBuffer, TBool aFinished);

	virtual void TerminateMultiStageOperation();
	virtual TUint GetDataChecksum(TDriveNumber aDrive);	
	
	//Implementations of virtuals from MActiveBackupDataClient - incremental backup & restore - not supported
	virtual void AllSnapshotsSuppliedL();
	virtual void ReceiveSnapshotDataL(TDriveNumber aDrive, TDesC8& aBuffer, TBool aLastSection);
	virtual void GetSnapshotDataL(TDriveNumber aDrive, TPtr8& aBuffer, TBool& aFinished);
	virtual void InitialiseGetBackupDataL(TDriveNumber aDrive);
	virtual void InitialiseRestoreBaseDataL(TDriveNumber aDrive);
	virtual void InitialiseRestoreIncrementDataL(TDriveNumber aDrive);
	virtual void RestoreIncrementDataSectionL(TDesC8& aBuffer, TBool aFinished);
	
	// to validate successful BUR
	TInt CheckSum(const RFile64 &aOpenFile, TUint64& aCheckSum) const;
	
private:
	CSqlBurCallback(MSqlSrvBurInterface& aInterface);
	void CopyBufData(const TDesC8& aInBuf, TInt& aInBufReadPos, TDes& aOutBuf, TInt aDataLen);
	void BackupCleanup();
	TInt RestoreCleanup();
	void SetBackupError(TInt aError);
	
private:

	// state machine for backup
	enum
		{
		EBackupNoFileOpen=0, // not currently processing a file
		EBackupOpenNothingSent, // file open and ready, but nothing sent yet
		EBackupOpenPartHeaderSent, // part of the header is sent, but more remains
		EBackupOpenAllHeaderSent, // all of the header is sent, ready to send the data
		EBackupEndOfFile // all done, tidy up after backup
		};

	// state machine for restore
	// this is more complicated because we are driven by the backup engine
	// and have incomplete information most of the time
	enum
		{
		ERestoreExpectChecksum=0, 		// checksum marks the start of the next file
		ERestoreExpectOldFileSize, 		// the size of the file - backup file header version 0
		ERestoreExpectVersion, 			// backup header version
		ERestoreExpectFileSize, 		// the size of the file, backup file header version 2+
		ERestoreExpectFileNameSize, 	// the size of the file name
		ERestoreExpectFileName, 		// the name of the file to restore
		ERestoreExpectData, 			// now for the data
		ERestoreComplete 				// tidy up after restore
		};
		
	MSqlSrvBurInterface& iInterface;	//A "Database file list" interface provided by the SQL server
	RArray<HBufC*> iFileList; 			//An array with database file names for backup, provided by the SQL server 
	RFile64 iFile;						//A handle to file being backup/restored.
	TInt iFileIndex;					//The index of the file name in iFileList being processed at the moment 
	TUint iState;						//Backup or restore state machine - current state
	TBuf<KSqlBurMaxHeaderSize> iBuffer; //Used for the header data
	TInt iHeaderSent; 					//How many header bytes sent so far
	TUint32 iChecksum; 					//Database archive checksum - used by the restore.
	TInt64 iFileSize; 					//Restored database file size - used by the restore.
	TUint32 iFileNameSize; 				//Restored database file name size - used by the restore.
	TDriveNumber iRestoreDrive;			//The drive where the data is currently restored to.
	TSecureId iRestoreId;				//The secure id of the client which data is being restored at the moment
	TInt iBackupError;					//An error occured during the backup processing
	TFileName iRestoreDir;				//The directory where temporary files will be created during restore.
	TParse iParse;

	};

#endif // __SQLBUR_H__
