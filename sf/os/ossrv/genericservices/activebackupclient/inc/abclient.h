/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Declaration of MActiveBackupDataClient and CActiveBackupClient
* 
*
*/



/**
 @file
 @released
*/
#ifndef __ACTIVEBACKUPCLIENT_H__
#define __ACTIVEBACKUPCLIENT_H__

#include <e32base.h>
#include <f32file.h>
#include <e32cmn.h>
#include <connect/sbdefs.h>

namespace conn
	{
	
	class MActiveBackupDataClient
		/**
		MActiveBackupDataClient is a Mixin to be implemented by an Active Backup client.
		The client connects to the Secure Backup Server using the CActiveBackupClient
		class and provides an instance of MActiveBackupDataClient to be called for a
		range of functions.

		The bulk transfer of data and snapshots is expected to be by means of shared
		heaps for performance reasons so the API is expected to change in these areas.

		@released
		@publishedAll
		*/
		{
	public:

		/** 
		Empty virtual destructor to avoid memory leaks
		*/
		virtual ~MActiveBackupDataClient() {}

		///// Backup Methods /////

		/**
		This method informs the active backup data client that all snapshots have
		been supplied. If the client has not received a snapshot then it should
		perform a base backup.
		*/
		virtual void AllSnapshotsSuppliedL() = 0;

		/**
		This method receives all or part of a snapshot of data to allow calculation of an
		incremental backup.  The snapshot is one that was previously supplied by the data
		owner.  The snapshot data should be read from the location supplied.
		The snapshot data may be larger than the location supplied in which case the routine will
		be called repeatedly until all data has been supplied.

		Snapshot data will also be supplied as part of a restore operation.

		@param aDrive the drive being backed up
		@param aBuffer a pointer to the base of the location from whence data can be copied.
		@param aLastSection ETrue if this is the last section of snapshot data, else EFalse.
		@leave KErrNotSupported if the data owner does not support incremental backups.
		*/
		virtual void ReceiveSnapshotDataL(TDriveNumber aDrive, TDesC8& aBuffer, TBool aLastSection) = 0;

		/**
		This method returns the expected size of backup data that will be supplied.   If an
		incremental backup is underway then this method will not be called until after
		ReceiveSnapshotDataL().  The size data will be used for the purpose of tracking progess
		during a backup.  If it is inaccurate then the user may see irregular progress but the
		actual backup data will not be affected so it is acceptable to return an estimated
		value.

		@param aDrive the drive being backed up.
		@return the size of the data that will be returned
		*/
		virtual TUint GetExpectedDataSize(TDriveNumber aDrive) = 0;

		/**
		This method returns a snapshot of data to accompany a backup.  The snapshot is expected
		to contain details on files / data being backed up.  The format of the snapshot is only
		meaningful to the data owner.  The snapshot will be supplied if the data owner is asked
		for an incremental backup and for a restore operation.  The snapshot data should be
		copied to the location supplied.
		The snapshot data may be larger than the location supplied in which case the routine will
		be called repeatedly until all data has been retrieved.

		@param aDrive the drive being backed up
		@param aBuffer a pointer to the base of the location where data can be copied.
		@param aFinished on return ETrue if all data has been returned for this drive, else EFalse.
		@leave KErrNotSupported if the data owner does not support  incremental backups.
		*/
		virtual void GetSnapshotDataL(TDriveNumber aDrive, TPtr8& aBuffer, TBool& aFinished) = 0;

		/**
		This method prepares the implementor to return backup data.  It will be followed by a
		sequence of calls to request the actual data.

		@param aDrive the drive being backed up.
		*/
		virtual void InitialiseGetBackupDataL(TDriveNumber aDrive) = 0;

		/**
		This method requests a section of backup data.  InitialiseGetBackupDataL() will have been
		called prevously to specify the drive concerned.  The data returned may be base or
		incremental depending on the type of backup and the capability of the data owner.

		@param aBuffer a pointer to the base of the location where data can be copied.
		@param aFinished on return ETrue if all data has been returned for this drive, else EFalse.
		*/
		virtual void GetBackupDataSectionL(TPtr8& aBuffer, TBool& aFinished) = 0;

		///// Restore Methods /////

		/**
		This method prepares the implementor to receive base restore data for a drive.
		It will be followed by a sequence of calls to supply the actual data.

		@param aDrive the drive being restored.
		*/
		virtual void InitialiseRestoreBaseDataL(TDriveNumber aDrive) = 0;

		/**
		This method receives a section of base restore data.
		InitialiseRestoreBaseDataL() will have been called prevously to specify the drive concerned.

		@param aBuffer a pointer to the base of the location whence data can be read.
		@param aFinished ETrue if all data has been returned for this drive, else EFalse.
		*/
		virtual void RestoreBaseDataSectionL(TDesC8& aBuffer, TBool aFinished) = 0;

		/**
		This method prepares the implementor to receive incremental restore data for a drive.
		It will be followed by a sequence of calls to supply the actual data.  If multiple
		increments are supplied then this methid will be called before each increment.

		@param aDrive the drive being restored.
		*/
		virtual void InitialiseRestoreIncrementDataL(TDriveNumber aDrive) = 0;

		/**
		This method receives a section of increment restore data.
		InitialiseRestoreIncrementDataL() will have been called prevously to specify the 
		drive concerned.

		@param aBuffer a pointer to the base of the location whence data can be read.
		@param aFinished ETrue if all data has been returned for this increment, else EFalse.
		*/
		virtual void RestoreIncrementDataSectionL(TDesC8& aBuffer, TBool aFinished) = 0;

		/**
		This method is called when all data to be restored has been supplied.

		@param aDrive the drive being restored.
		*/
		virtual void RestoreComplete(TDriveNumber aDrive) = 0;

		/**
		This method prepares the implementor to return backup data on behalf of another data owner.
		It will be followed by a sequence of calls to request the actual data. This method is only
		for use by a proxy data manager that backs up data on behalf of other data owners.  There
		is no corresponding method for snapshots as it is assumed that a proxy data manager will
		only handle base data.

		@param aSID the data owner whose data is to be backed up
		@param aDrive the drive being backed up.
		*/
		virtual void InitialiseGetProxyBackupDataL(TSecureId aSID, TDriveNumber aDrive);

		/**
		This method prepares the implementor to receive base restore data for another data owner
		for a drive. It will be followed by a sequence of calls to supply the actual data.
		This method is only for use by a proxy data manager that restores up data on behalf of
		other data owners.  There is no corresponding method for incremental data as it is assumed
		that a proxy data manager will only handle base data.

		@param aSID the data owner whose data is to be restored
		@param aDrive the drive being restored.
		*/
		virtual void InitialiseRestoreProxyBaseDataL(TSecureId aSID, TDriveNumber aDrive);
		
		///// General Methods /////

		/**
		This method is called if copying of data is terminated prematurely to allow the 
		implementor to tidy up.  The same method applies to all types of data and to backup
		and restore
		*/
		virtual void TerminateMultiStageOperation() = 0;
		
		/**
		Gets an extended interface based on a supplied uid.
		
		@param aUid Uid which identifies an extended interface
		@return Pointer to an extended interface
		*/
		virtual TAny* GetExtendedInterface(const TInt32 aUid);

		///// Test Methods /////
		/**
		Gets a 32-bit checksum for its private data.
		This routine is for test purposes.  It must be implemented but an invariant checksum 
		value can be provided.  Some tests may cause checksum values to be compared.
    
		@param aDrive the drive containing data being checksummed
		@return the 32-bit checksum
		*/
		virtual TUint GetDataChecksum(TDriveNumber aDrive) = 0;
		};

	inline void MActiveBackupDataClient::InitialiseGetProxyBackupDataL(TSecureId /*aSID*/, TDriveNumber /*aDrive*/)
	/** Initialises the proxy backup data
	
	This is not supported here.
	
	@param aSID the secure Id
	@param aDrive the driver number
	@leave KErrNotSupported Always.
	*/
		{
		User::Leave(KErrNotSupported);
		}

	inline void MActiveBackupDataClient::InitialiseRestoreProxyBaseDataL(TSecureId /*aSID*/, TDriveNumber /*aDrive*/)
		{
	/** Initialises the proxy backup base data
	
	This is not supported here.
	
	@param aSID the secure Id
	@param aDrive the driver number
	@leave KErrNotSupported Always.
	*/
		User::Leave(KErrNotSupported);
		}
		
	inline TAny* MActiveBackupDataClient::GetExtendedInterface(const TInt32 /*aUid*/)
		{
		return NULL;
		}
				
	
	class RABClientSession;
	class CActiveBackupCallbackHandler;
	
	class CActiveBackupClient : public CBase
		/**
		CActiveBackupClient provides a connection to the Secure Backup Server for a data owning
		process.

		It can be used to obtain information about an active backup or restore operation. 
		It can also be used to signal to the Secure Backup Server when the data owner is ready
		for backup or restore.

		It is also used by data owners that implement active backup or restore to provide a
		MActiveBackupDataClient implementation.

		This class owns a RActiveBackupSessionImpl instance and publishes the 
		public API to the outside world. The reason for this facade class is twofold:

		@li Hiding the implementation details of RActiveBackupSessionImpl 

		@li Future binary compatibility

		@released
		@publishedAll
		*/
		{
	public:
		IMPORT_C static CActiveBackupClient* NewL();

		IMPORT_C static CActiveBackupClient* NewL(MActiveBackupDataClient* aClient);
		IMPORT_C ~CActiveBackupClient();
		IMPORT_C void BURModeInfoL(TDriveList& aDriveList, TBURPartType& aBackupType, TBackupIncType& aIncBackupType);
		IMPORT_C TBool DoesPartialBURAffectMeL();
		IMPORT_C void ConfirmReadyForBURL(TInt aErrorCode);

	private:
		CActiveBackupClient();
		void ConstructL();
		void ConstructL(MActiveBackupDataClient* aClient);

	private:
		/** Pointer to the client session R class that's wrapped up by this class */
		RABClientSession* iClientSession;
		
		/** Pointer to the Active Backup Callback Handler */
		CActiveBackupCallbackHandler* iABCallbackHandler;
		};

	} // end namespace

#endif // __ACTIVEBACKUPCLIENT_H__

