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
//

#include <e32def.h>
#include <e32property.h>
#include <s32file.h>
#include <connect/sbdefs.h>
#include "srvres.h"
#include "cachemgr.h"
#include "srvparams.h"
#include "shrepos.h"
#include "sessnotf.h"
#include "backup.h"

#define UNUSED_VAR(a) a = a

_LIT (KBackupFileName, "BACKUP") ;
_LIT (KRestoreFileName, "RESTORE") ;
_LIT(KBackupFileExt, ".bak");
_LIT(KRestoreFileExt, ".rst");


TRepositoryBackupState CRepositoryBackupClient::iBackupStatus ;


//
// Backup stream index class - Used to hold association between a UID (in
// our case the UID of a repository) and a stream ID - Can't use CStreamDictionary
// because that only lets you retrieve stream IDs by a (previously known) UID rather
// than iterate through its contentsretrieving UID/StreamID pairs...
// 


//
// CRepositoryBackupStreamIndex::AddL
//
// Add a new repository UID and stream ID pair to the index
void CRepositoryBackupStreamIndex::AddL(TUid aUid, TStreamId aSettingStreamId, TStreamId aDeletedSettingsStreamId, TStreamId aInstalledSettingsStreamId) 
	{
	TRepositoryBackupStreamIndexElement newIndexElement;
	newIndexElement.Set(aUid,aSettingStreamId, aDeletedSettingsStreamId, aInstalledSettingsStreamId) ;
	iStreamIndex.AppendL(newIndexElement);
	}


// 
// CRepositoryBackupStreamIndex::GetNext
//
// Iterate through the index retrieving the next Reposirory UID and Stream ID
// pair.
//
TInt CRepositoryBackupStreamIndex::GetNext(TUid& aUid, TStreamId& aSettingsStreamId, TStreamId& aDeletedSettingsStreamId, TStreamId& aInstalledSettingsStreamId)
	{
	TInt error = KErrNone ;
	if (iIndex < iStreamIndex.Count())
		{
		iStreamIndex[iIndex++].Get(aUid, aSettingsStreamId, aDeletedSettingsStreamId, aInstalledSettingsStreamId) ;
		}
	else
		{
		error = KErrNotFound ;
		}
	return error ;
	}


//
// Backup client class.
//
// Has Active object functionality to monitor the state of the publish and subscribe 
// flags associated with backup and restore and also implements MActiveBackupDataClient
// to perform active backup according to the "proxy data holder" model.
//



//
// Usual 2 phase construction factory NewL NewLC classes
//	
CRepositoryBackupClient* CRepositoryBackupClient::NewLC(RFs& aFs) 
	{
	CRepositoryBackupClient* me = new(ELeave)CRepositoryBackupClient(aFs);
	CleanupStack::PushL(me) ;
	me->ConstructL() ;
	return me ;
	}
	
	
CRepositoryBackupClient* CRepositoryBackupClient::NewL(RFs& aFs) 
	{
	CRepositoryBackupClient* me = CRepositoryBackupClient::NewLC(aFs) ;
	CleanupStack::Pop(me) ;
	return me ;
	}
	
		
		
// 
// Constructor - doesn't really do anything!
//
CRepositoryBackupClient::CRepositoryBackupClient(RFs& aFs) : CActive(EPriorityStandard), iFs(aFs), iRomScanDone(EFalse)
	{
	}


// 
// Phase 2 constructor
//	
void CRepositoryBackupClient::ConstructL()
	{
	// Create repository object
	iRepository = new(ELeave) CServerRepository;
	
	// Notifier needed to open repositories.		
	iNotifier = new(ELeave)CSessionNotifier ;

	// Attach to Backup/Restore Pub/Sub property.	
	User::LeaveIfError(iBackupRestoreProperty.Attach(KUidSystemCategory, KUidBackupRestoreKey)) ;
	
	// Add ourself to the active scheduler
	CActiveScheduler::Add(this) ;
	
	// Initialise backup/restore status
	iBackupStatus = ENoBackupActivty ;
	
	// Set active and request notification of changes to backup/restore
	// Pub/Sub property.
	StartL() ;
	}



//
// Destructor
//
CRepositoryBackupClient::~CRepositoryBackupClient()
	{
	Cancel();
	
	iBackupRestoreProperty.Close() ;
	
	if (iRepository)
		{
		iRepository->Close();
		delete(iRepository);
		}
		
	if (iNotifier)
		delete(iNotifier) ;
	
	if (iActiveBackupClient)
		delete(iActiveBackupClient) ;	
		
	iRestoredRepositoriesArray.ResetAndDestroy();
	iRestoredRepositoriesArray.Close();
	
	iFile.Close();
	}




// 
// DoCancel - mandatory for active objects.
//	
void CRepositoryBackupClient::DoCancel()
	{
	iBackupRestoreProperty.Cancel() ;
	}



//
// RunError
//
TInt CRepositoryBackupClient::RunError(TInt aError)
	{
	iRestoredRepositoriesArray.ResetAndDestroy();
	UNUSED_VAR(aError);
	return KErrNone;
	}




//
// Test BUR Pub/Sub property set status, and notify BUR that we're
// ready to go as appropriate. 
//
void CRepositoryBackupClient::TestBURstatusL(void)
	{
	TInt BURstatus ;
	TRepositoryBackupState lastBackupStatus = ENoBackupActivty;
	if (iBackupRestoreProperty.Get(BURstatus) != KErrNotFound)
		{
		BURstatus &= KBURPartTypeMask ;
		switch (BURstatus)
			{
			case EBURUnset:	// State not yet set. Treat as no backup/restore in progress.
			case EBURNormal:
				// No backup or restore in progress. Probably
				// means we've just completed an operation?
				
				lastBackupStatus = iBackupStatus;
				iBackupStatus = ENoBackupActivty ;
				
				// Back to normal, so enable cache
				TServerResources::iCacheManager->EnableCache();				
				// delete the CActiveBackupClient
				if (iActiveBackupClient)
					{
					delete iActiveBackupClient ;
					iActiveBackupClient = NULL ;
					}
					
				// Notify the changed keys if a restoration was just completed
				if((lastBackupStatus == ERestoreInProgress)) 
			    	{
				    for(TInt i = 0; i < iRestoredRepositoriesArray.Count(); i++)
				    	{
						iRepository->OpenL(iRestoredRepositoriesArray[i]->Uid(), *iNotifier, EFalse);
						iRepository->RestoreNotify(*iRestoredRepositoriesArray[i]);
						iRepository->Close();
						}
					iRestoredRepositoriesArray.ResetAndDestroy();
					}

				break ;
				
			case EBURBackupFull :
			case EBURBackupPartial :
				// We don't distinguish between full and partial backups
				// as the Backup engine will give us UIDs for all the
				// repository data owners that want their stuff backed up
				// anyway.
				
				// We don't want cache activity during backup
				TServerResources::iCacheManager->DisableCache();
				
				// Any (and all!) repositories which have been opened in the
				// course of system boot and normal operation will have been
				// added to TServerResources::iOwnerIdLookUpTable as they were
				// opened but there may well be repositories which need backing
				// up and haven't yet been opened so we need to make sure that
				// the lookup table is complete.
				CompleteOwnerIdLookupTableL();

				// Register with BUR engine
				if (!iActiveBackupClient)
					{
					iActiveBackupClient = CActiveBackupClient::NewL(this) ;
					}	
				iActiveBackupClient->ConfirmReadyForBURL(KErrNone);
				iBackupStatus = EBackupInProgress ;
				break ;
				
			case EBURRestoreFull :
			case EBURRestorePartial :
				// We don't distinguish between full and partial restore
				// either!
				
				// We don't want cache activity during restore either!
				TServerResources::iCacheManager->DisableCache();
				
				// Register with BUR engine
				if (!iActiveBackupClient)
					{
					iActiveBackupClient = CActiveBackupClient::NewL(this) ;
					}
				iActiveBackupClient->ConfirmReadyForBURL(KErrNone);
				iBackupStatus = ERestoreInProgress ;
				break ;
							
			}
		}
	}


//
// Request notification of changes in BUR Pub/Sub status
//
void CRepositoryBackupClient::StartL()
	{
	if (!IsActive())
		{
		TestBURstatusL();	
		NotifyChange();
		}
	}


//
// Request notification of changes in BUR Pub/Sub status
//
void CRepositoryBackupClient::NotifyChange()
	{
	// Watch for changes in the property state. 
	iBackupRestoreProperty.Subscribe(iStatus) ;
	SetActive();
	}

	
//
// Handle changes of backup state through publish/subscribe
//
void CRepositoryBackupClient::RunL()
	{	
	NotifyChange() ;
	TestBURstatusL();
	}


//
// We can't estimate data size without A) having the SID of the data owner who's data
// is to be backed up and B) going through the whole process of preparing the backup.
//
// The only sensible thing we can do is return an arbitrary value!
//
TUint CRepositoryBackupClient::GetExpectedDataSize(TDriveNumber /* aDrive */)
	{
	return KArbitraryNumber ;
	}	




//
// Called by BUR engine to request a chunk of backup data.
//	
void CRepositoryBackupClient::GetBackupDataSectionL(TPtr8& aBuffer, TBool& aFinished)
	{
	const TInt chunkSize = aBuffer.MaxSize() ;

	aFinished = EFalse ;
		
	// Pass a chunk of our prepared backup data in aBuffer
	User::LeaveIfError(iFile.Read(aBuffer, chunkSize)) ;
	TInt bytesRead = aBuffer.Length() ;

	// Check to see if this was the last chunk of data.
	if (bytesRead < chunkSize)
		{
		// Set "finished" flag so that BUR knows we're through...
		aFinished = ETrue ;
		
		// ...and then tidy up by closing and deleting the backup file.
		iFile.Close() ;
		TParse backupFilePath ;
		User::LeaveIfError(backupFilePath.Set(KBackupFileName, TServerResources::iBURDirectory, &KBackupFileExt));
		TInt fileDeleteErr=iFs.Delete(backupFilePath.FullName()) ;
		#ifdef _DEBUG
			if (fileDeleteErr != KErrNone)
			{
				RDebug::Print(_L("CRepositoryBackupClient::GetBackupDataSectionL - Failed to delete file. Error = %d"), fileDeleteErr);
			}
		#else
			UNUSED_VAR(fileDeleteErr);
		#endif

		}
	}

//
// CRepositoryBackupClient::RestoreComplete
//
// Called when a Complete set of backup data has been received and written
// to a file. We now need to open the file as a stream store, get the
// index (list of repository UID and corresponding stream ID pairs, and then
// reconstruct and save each repository in turn.
// 
void CRepositoryBackupClient::RestoreComplete(TDriveNumber /* aDrive */)
	{
	}


void CRepositoryBackupClient::RestoreRepositoryAndListL(TUid repositoryUid, CDirectFileStore* store, TStreamId settingsStreamId, TStreamId deletedSettingsStreamId, TInt& repIndex)
    {
    // Add the restored repository to the restored repositories list.
    // Pass its changed-keys list to further restoring functions to add entries for post-restoration notification.
    repIndex = AddRestoredRepositoryL(repositoryUid);
    iRepository->RestoreRepositoryContentsL(*store, settingsStreamId, deletedSettingsStreamId, *iRestoredRepositoriesArray[repIndex]);
    iRepository->CommitChangesL();
    }

	
//
// CRepositoryBackupClient::RestoreCompleteL
//
// Does the actual work of reconstructing repositories from backup data
//
//
void CRepositoryBackupClient::RestoreCompleteL()
	{
	// All restore data recived so we can now recreate the repositories from the
	// backup store
	// Attempt to open the restore file as a CDirectFileStore
	TParse restoreFilePath ;
	User::LeaveIfError(restoreFilePath.Set(KRestoreFileName, TServerResources::iBURDirectory, &KRestoreFileExt));
	CDirectFileStore* store = CDirectFileStore::OpenLC (iFs,restoreFilePath.FullName(), EFileRead|EFileShareReadersOnly);
	if (store->Type()[0] != KDirectFileStoreLayoutUid)
		{
		// store wasn't quite what we were expecting - can't return an error, can't leave
		// so all we can do is close the file, tidy up as best we can, and bail out!!!! 
		CleanupStack::PopAndDestroy(store);
		// If a debug build - record error
		TInt fileDeleteErr=iFs.Delete(restoreFilePath.FullName()) ;
		#ifdef _DEBUG
			if (fileDeleteErr != KErrNone)
			{
				RDebug::Print(_L("CRepositoryBackupClient::RestoreCompleteL - Failed to delete file. Error = %d"), fileDeleteErr);
			}
		#else
			UNUSED_VAR(fileDeleteErr);
		#endif

		User::Leave(KErrCorrupt);
		}		


	// Get the root stream and attempt to read a backup file header from it
	TStreamId rootStreamId = store->Root() ;
	RStoreReadStream rootStream ;
	RStoreReadStream indexStream ;
	rootStream.OpenLC(*store, rootStreamId);
	TRepositoryBackupStreamHeader header ;
	TRAPD(err, header.InternalizeL(rootStream)) ;
	
	// Check for a valid header by checking that the UID matches the UID
	// of Central Repository and that the version number is sane.
	if (err == KErrNotSupported)
	    {
   		// Not a valid header - assume it's an old style backup stream,
		// set extensions supported to none, set index stream to be
		// root stream and reset read pointer to beginning.
		iBackupExtensionsSupported = ENoBackupExtensions ;

		CleanupStack::PopAndDestroy(&rootStream) ;
		CleanupStack::PopAndDestroy(store) ;

		// Try re-opening as old-style backup stream with index
		// as root stream.		
		CDirectFileStore* store = CDirectFileStore::OpenLC (iFs,restoreFilePath.FullName(), EFileRead|EFileShareReadersOnly);
		indexStream.OpenLC(*store, rootStreamId) ;
	    }
	else
		{
	    // Got a valid header. Check for extensions supported by this
	    // stream and get stream to read index from
	    CleanupStack::PopAndDestroy(&rootStream) ;
	    iBackupExtensionsSupported = header.getBackupExtensionsSupported();	
	    TStreamId indexStreamId = header.getIndexStreamId() ;
	    indexStream.OpenLC (*store, indexStreamId) ; 		
		}
	
	CRepositoryBackupStreamIndex *restoreStreamIndex = CRepositoryBackupStreamIndex::NewLC();	
	restoreStreamIndex->InternalizeL(indexStream, iBackupExtensionsSupported);

	
	// Iterate through index and attempt restore of each repository stream
	// we find in it.
	restoreStreamIndex->Reset();	
	TUid repositoryUid;
	TStreamId settingsStreamId(KNullStreamIdValue);
	TStreamId deletedSettingsStreamId(KNullStreamIdValue);
	TStreamId installedSettingsStreamId(KNullStreamIdValue);
	
	while (restoreStreamIndex->GetNext(repositoryUid, settingsStreamId, deletedSettingsStreamId, installedSettingsStreamId) == KErrNone)
		{
		iRepository->OpenL(repositoryUid, *iNotifier, EFalse);
		iRepository->FailAllTransactions();
		TInt repIndex;
		TRAPD(err, RestoreRepositoryAndListL(repositoryUid, store, settingsStreamId, deletedSettingsStreamId, repIndex));
		iRepository->Close();
	    User::LeaveIfError(err);
		// If the backup contains an installed repository containing default values for the settings, read them in
		if (installedSettingsStreamId != KNullStreamId)
			{
			// create an empty repository in install directory, and restore the data from backup file
			iRepository->RestoreInstallRepositoryL(repositoryUid, *store, installedSettingsStreamId, *iRestoredRepositoriesArray[repIndex]);
			// remove the .ini install file (if exists) because it will clash with the restored file
			TServerResources::DeleteCentrepFileL(repositoryUid, EInstall, EIni);
			}
		}

	CleanupStack::PopAndDestroy(restoreStreamIndex) ;
	CleanupStack::PopAndDestroy(&indexStream);
	CleanupStack::PopAndDestroy(store);
	// If a debug build - record error
	TInt fileDeleteErr=iFs.Delete(restoreFilePath.FullName());
	#ifdef _DEBUG
		if (fileDeleteErr != KErrNone)
		{
			RDebug::Print(_L("CRepositoryBackupClient::RestoreCompleteL (2nd) - Failed to delete file. Error = %d"), fileDeleteErr);
		}
	#else
		UNUSED_VAR(fileDeleteErr);
	#endif

	}
	
//
// CRepositoryBackupClient::CompleteOwnerIdLookupTableL
//
// Open each repository in TServerResources::iDataDirectory.
// Save the Rep UID and Owner Id of the rep to be used by
// InitialiseGetProxyBackupDataL.
void CRepositoryBackupClient::CompleteOwnerIdLookupTableL()
	{

	// Read contents of persist, install, and ROM directories and
	// use them to build a list of repository candidates.
	RArray <TUint32> repositoryList ;
	CleanupClosePushL(repositoryList) ;

	for (TBackupDirectoryScan scanDir = EScanRom; scanDir <= EScanPersist; scanDir = (TBackupDirectoryScan)(scanDir+1))
		{
		TPtrC directoryName ;
		switch (scanDir)
			{
				case EScanRom :
					if (TServerResources::iRomDirectory)
						{
						directoryName.Set(TServerResources::iRomDirectory->Des()) ;						
						}
					else
						{
						// if ROM directory doesn't exist or there are no files, skip scanning
						continue;
						}
					break ;
					
				case EScanInstall :
					directoryName.Set(TServerResources::iInstallDirectory->Des()) ;
					break ;
					
				case EScanPersist :
					directoryName.Set(TServerResources::iDataDirectory->Des()) ;
					break ;				
			}
			
		RDir dir;
	    CleanupClosePushL(dir);
		User::LeaveIfError(dir.Open(iFs, directoryName, KEntryAttNormal));
	    
	    TEntryArray dirEntries;
	    TInt readError = KErrNone;
	    
		while (readError != KErrEof)  
			{
		    readError = dir.Read(dirEntries);
	    
		    if(readError != KErrNone && readError != KErrEof) 
		    	{
		    	User::Leave(readError);
		    	}
		    else
		    	{
		    	const TInt dirCount = dirEntries.Count();   
		    	for (TInt i=0; i<dirCount; i++)
		    		{
		    		// Attempt to extract a repository UID from directory entry
		    		TUid uid;
					TInt insertionError;
		    		if (KErrNone == TServerResources::GetUid(const_cast<TEntry&>(dirEntries[i]), uid))
		    			{
		    			insertionError=repositoryList.InsertInUnsignedKeyOrder(uid.iUid) ;
						// Should leave in all cases other than KErrNone or KErrAlreadyExists						
						if((insertionError != KErrNone) && (insertionError != KErrAlreadyExists ))
							{
							User::Leave(insertionError);
							}						
		    			}
		    		}
		    	}
			}
	    
		CleanupStack::PopAndDestroy(&dir);
		}
		
	// Open all repositories in turn. Save repository UID and owner ID
	// in lookup table.
	for(TInt i = 0; i<repositoryList.Count(); i++)
		{
		// Look to see if this repository already has an entry in the owner ID lookup table
		if ( TServerResources::FindOwnerIdLookupMapping(repositoryList[i]) == KErrNotFound)
			{
			// Need to TRAP here as otherwise if ANY repository fails to open
			// (e.g. due to corruption) it would cause the entire backup to
			// fail
			TRAPD(err, iRepository->OpenL(TUid::Uid(repositoryList[i]), *iNotifier));
			if (err == KErrNoMemory)
				{
				User::Leave(err) ;
				}
			
			else if (err == KErrNone)
				{
				// The act of opening a repository will cause it to add itself to the
				// Repository/Owner UID mapping table so we don't need to do anything
				// and can close it immediately!
				iRepository->Close();
				}
			}
		}
		
	CleanupStack::PopAndDestroy() ; // repositoryList	
	}

// CRepositoryBackupClient::InitialiseGetProxyBackupDataL
//
// Prepare data to be backed up. We get the Sid/Uid of the entity whos data
// is to be backed up. What we do is to open each repository in turn (identified
// by directory listing), check its owner, and if it matches the Sid/Uid we've
// been given by secure backup externalise it to a stream within a file store.
//
void CRepositoryBackupClient::InitialiseGetProxyBackupDataL(TSecureId aSID, TDriveNumber /* aDrive */) 
	{
	// Prepare data for backup.

	// Create file store
	TParse backupFilePath ;
	User::LeaveIfError(backupFilePath.Set(KBackupFileName, TServerResources::iBURDirectory, &KBackupFileExt));
	CDirectFileStore* store = CDirectFileStore::ReplaceLC(iFs, backupFilePath.FullName(),
	                                                      (EFileWrite | EFileShareExclusive));
	const TUid uid2  = KNullUid ;	                                                     
	store->SetTypeL(TUidType(KDirectFileStoreLayoutUid, uid2, KServerUid3)) ; 
	
	// Create a Backup Stream index
	CRepositoryBackupStreamIndex* backupStreamIndex = CRepositoryBackupStreamIndex::NewLC();	
	  
	// Find the reps owned by aSID
	for(TInt i = 0; i < TServerResources::iOwnerIdLookUpTable.Count(); ++i)
		{
		const TOwnerIdMapping& lookupTableEntry = TServerResources::iOwnerIdLookUpTable[i];
		
		if ( lookupTableEntry.iOwner == aSID )
			{
			TStreamId settingStreamId(KNullStreamIdValue);
			TStreamId deletedSettingsStreamId(KNullStreamIdValue);
			TStreamId installedSettingStreamId(KNullStreamIdValue);
			// Found one match, open the repository and externalise content.
			TUid uid = TUid::Uid(lookupTableEntry.iRepUid);
			TRAPD(err,iRepository->OpenL(uid, *iNotifier));
			if (err == KErrNoMemory)
				{
				User::Leave(err) ;
				}
			else if (err == KErrNone)
				{			
				iRepository->FailAllTransactions();
				// externalise repository contents
				iRepository->StoreRepositoryContentsL(*store, settingStreamId, deletedSettingsStreamId);
				// Close repository.
				iRepository->Close();
				}
				
			TBool installExists=TServerResources::InstallFileExistsL(uid);
			if(installExists)
				{			
				// load existing repository from install directory (default values installed post-build by SWI)
				// and externalise installed repository contents				
				TRAPD(err, iRepository->BackupInstallRepositoryL(uid, *store, installedSettingStreamId));
				// We trap and discard most errors to be able to continue backing up other repositories in the list
				if (err == KErrNoMemory)
					{
					User::Leave(err) ;
					}
				else if (err != KErrNone)
					{
					// If for any reason we haven't been able to back up the install repository, 
					// we create an empty stream to preserve the format
					installedSettingStreamId = KNullStreamIdValue;
					}
				}
			// Add all to store index
			backupStreamIndex->AddL(uid, settingStreamId, deletedSettingsStreamId, installedSettingStreamId) ;

			}
		}
	// Write the stream index/dictionary as root stream within the store
	// so we can access it when we do a restore later on
	RStoreWriteStream indexStream ;
	TStreamId indexStreamId = indexStream.CreateLC(*store) ;
	backupStreamIndex->ExternalizeL(indexStream) ;
	indexStream.CommitL() ;
	CleanupStack::PopAndDestroy(&indexStream) ;
	CleanupStack::PopAndDestroy (backupStreamIndex) ;
		
	// Create the Header and write it as the root stream within the store
	// so we can access it when we do a restore later on
	TRepositoryBackupStreamHeader header (indexStreamId) ;
	RStoreWriteStream rootStream ;
	TStreamId rootStreamId = rootStream.CreateLC(*store) ;
	header.ExternalizeL(rootStream) ;
	rootStream.CommitL() ;
	
	CleanupStack::PopAndDestroy(&rootStream) ;
	store->SetRootL(rootStreamId);
	store->CommitL();
	CleanupStack::PopAndDestroy(store) ; 
	
	// Attempt to open file containing store ready to read back and send to
	// BUR engine as a stream of bytes.
	User::LeaveIfError(iFile.Open(iFs, backupFilePath.FullName(), (EFileRead | EFileShareExclusive))) ;
	}




//
// CRepositoryBackupClient::InitialiseRestoreProxyBaseDataL
//
// Called when secure backup is about to start sending restore data.
//
void CRepositoryBackupClient::InitialiseRestoreProxyBaseDataL(TSecureId aSID, TDriveNumber /* aDrive*/)
	{
	// prepare for restore - Don't think we need to do anything here except prepare
	// data structures to receive incoming data
	
	// Save SID so we can check that it corresponds with the owner information 
	// in the restored data.
	iSid = aSID ;
	
	// Open file to receive restored data
	TParse restoreFilePath ;
	User::LeaveIfError(restoreFilePath.Set(KRestoreFileName, TServerResources::iBURDirectory, &KRestoreFileExt));
	User::LeaveIfError(iFile.Replace (iFs, restoreFilePath.FullName(), (EFileWrite | EFileShareExclusive)));
	}




//
// CRepositoryBackupClient::RestoreBaseDataSectionL
//
// Called when secure backup has a chunk of restore data for us. Last data
// segment identified by aFinished.
//
void CRepositoryBackupClient::RestoreBaseDataSectionL(TDesC8& aBuffer, TBool aFinished)
	{
	// Receive a chunk of restore data in aBuffer 
	User::LeaveIfError(iFile.Write (aBuffer)) ;
	if (aFinished)
		{
		iFile.Close() ;
		
		// All restore data recived so we can now recreate the repositories from the
		// backup store	
		RestoreCompleteL();	
		}
	}
	
void CRepositoryBackupClient::TerminateMultiStageOperation()
	{
	// Backup/Restore operation aborted!
	// Tidy up all temporary data.
	HBufC* burFileName = HBufC::New(KMaxFileName);
	if(burFileName==NULL)
		{
		return;
		}
	TPtr burFileNamePtr(burFileName->Des());
	iFile.FullName(burFileNamePtr); //get the full name of the temporary file
	iFile.Close(); // close the file
	// If a debug build - record error
	TInt fileDeleteErr=iFs.Delete(burFileNamePtr);
	#ifdef _DEBUG
		if (fileDeleteErr != KErrNone)
		{
			RDebug::Print(_L("CRepositoryBackupClient::TerminateMultiStageOperation - Failed to delete file. Error = %d"), fileDeleteErr);
		}
	#else
		UNUSED_VAR(fileDeleteErr);
	#endif

	delete burFileName;
	}


//
// CRepositoryBackupClient::GetDataChecksum
//
// Not required and we don't implement it.
//
TUint CRepositoryBackupClient::GetDataChecksum(TDriveNumber /* aDrive */) 
	{
	return KArbitraryNumber;
	}

// 
// CRepositoryBackupClient::GetSnapshotDataL
//
// Only required for incremental backup (which we don't support
//
void CRepositoryBackupClient::GetSnapshotDataL(TDriveNumber /* aDrive */, TPtr8& /* aBuffer */, TBool& /* aFinished */) 
	{
	User::Leave(KErrNotSupported) ;
	}

// 
// CRepositoryBackupClient::InitialiseGetBackupDataL
//
// Used by "normal" active backup to prepare data - we use
// InitialiseRestoreProxyBaseDataL so this shouldn't be called!
//	
void CRepositoryBackupClient::InitialiseGetBackupDataL(TDriveNumber /* aDrive */)
	{
	User::Leave(KErrNotSupported) ;
	}
	


void CRepositoryBackupClient::InitialiseRestoreBaseDataL(TDriveNumber /* aDrive */)
	{
	// Check this! I Don't think this method should get called as we're a proxy
	// and so implement InitialiseGetProxyBackupDataL!! 
	User::Leave(KErrNotSupported) ;		
	}
	



void CRepositoryBackupClient::InitialiseRestoreIncrementDataL(TDriveNumber /* aDrive */)
	{
	// Check this! I Don't think this method should get called as we're a proxy
	// so don't do incremental backup!! 
	User::Leave(KErrNotSupported) ;			
	}



void CRepositoryBackupClient::RestoreIncrementDataSectionL(TDesC8& /* aBuffer */, TBool /* aFinished */)
	{
	// Check this! I Don't think this method should get called as we're a proxy
	// so don't do incremental backup!! 
	User::Leave(KErrNotSupported) ;		
	}


//
// Incremental backup isn't supported for the proxy data holder model so this
// method should never be called.
//
// Not acceptable to leave even though it's an ...L function which we don't implement
void CRepositoryBackupClient::AllSnapshotsSuppliedL() 
	{
	;
	}



//
// Incremental backup not supported
//	
void CRepositoryBackupClient::ReceiveSnapshotDataL(TDriveNumber /* aDrive */, TDesC8& /* aBuffer */, TBool /* aLastSection */) 
	{
	User::Leave(KErrNotSupported) ;
	}

/**
Adds a new entry to the restored repository list with the specified repository uid.

Creates an object of the CRestoredRepository and inserts it to the list.
If a repository of the specified uid is already on the list, the created object will 
be deleted and no repeated entry will be inserted to the list.

@param aUid The uid of the repository to be added to this array.
@return The index of the new or existing entry within the list.
*/
TInt CRepositoryBackupClient::AddRestoredRepositoryL(TUid aUid)
	{
	CRestoredRepository* rstRepos = new(ELeave) CRestoredRepository(aUid);
	CleanupStack::PushL(rstRepos);
	TInt err = iRestoredRepositoriesArray.InsertInOrder(rstRepos, TLinearOrder<CRestoredRepository>(CRestoredRepository::CompareUids));
	if(err != KErrNone && err != KErrAlreadyExists)
		User::Leave(err);
	TInt index = iRestoredRepositoriesArray.FindInOrderL(rstRepos, TLinearOrder<CRestoredRepository>(CRestoredRepository::CompareUids));
	CleanupStack::Pop();
	
	if(err == KErrAlreadyExists)
		{
		delete rstRepos;
		}
	return index;
	}
