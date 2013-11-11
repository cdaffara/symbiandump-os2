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

#include "panic.h"
#include "shrepos.h"
#include "srvrepos_noc.h"
#include "srvres.h"
#include "cachemgr.h"
#include "sessnotf.h"
#include "srvPerf.h"
#include "srvreqs.h" 
#include "rstrepos.h"
#ifdef SYMBIAN_BAFL_SYSUTIL
#include <bafl/sysutil.h>
#endif
#include <e32def_private.h>


void CServerRepository::OpenL(TUid aUid, MObserver& aObserver, TBool aFailIfNotFound)
	{
	iNotifier = &aObserver;
	
	TServerResources::iObserver->iObservers.ReserveL(1);
	
	TServerResources::iObserver->AddSharedRepositoryInfoL(aUid);
	
	TRAPD( err, iRepository = TServerResources::iObserver->AccessL(aUid, aFailIfNotFound) );
	
	//store uid
	iUid = aUid;
	
	if (err == KErrNone)
	    {
        TRAP( err, TServerResources::iObserver->AddObserverL(aUid, this) );
	    }
	    
	if (err != KErrNone)
        {
        TServerResources::iObserver->RemoveSharedRepositoryInfo(aUid);
        User::Leave(err);
        }
	}

void CServerRepository::Close()
	{
	iRepository = NULL;

	TInt index = TServerResources::iObserver->FindOpenRepository(iUid);
	
	if (index>=0)
		{
		iRepository = TServerResources::iObserver->GetOpenRepository(index);
		}
	// cancel to ensure any read/write locks are released and transaction settings cleaned up

    CancelTransaction();

	
	TServerResources::iObserver->RemoveObserver(iUid, this, index);
	
	iNotifier = NULL;			
	}
	
/**
Notify about all changed keys stored in the specified reference to the
CRestoredRepository.

@param aRstRepos The reference to CRestoredRepository which holds the list 
of the changed keys.
*/	
void CServerRepository::RestoreNotify(const CRestoredRepository& aRstRepos)
	{
	const RArray<TUint32>& keys = aRstRepos.ChangedKeys();
	TInt count=keys.Count();
	for(TInt i = 0; i < count; i++)
		{
		 iRepository->Notify(keys[i]);
		}
	}
	
/**
Attempt to reset a single key to it's value in the file in the given location. Routine
attempts to find a .cre file first. If ( and only if ) a cre file doesn't exist the 
routine attempts to find a txt file. In the presence of multi rofs, it needs to perform
merging of all the rom keyspaces first before doing a reset, hence we are not able to perform
the reading line by line for efficiency purpose.
*/
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS	
void CServerRepository::ResetFromIniFileL(TUint32 aId, 
										  CIniFileIn::TIniFileOpenMode aIniFileOpenMode,
										  TBool& aKeyFound)
	{
	aKeyFound=EFalse;

	CSharedRepository* rep=NULL;
	// Attempt to reset key to the aLocation if exist
	//dont fail if repository not found
	TServerResources::iObserver->LoadRepositoryLC(iRepository->Uid(),EFalse,rep,aIniFileOpenMode);
	
	if (rep)
		{
		TServerSetting* s = rep->GetSettings().Find(aId);					
		if(s)
			{
			aKeyFound=ETrue;
			// Mark the setting as default again
			s->SetClean();
			iRepository->ResetAndPersistL(*s);
			s->SetAccessPolicy(GetFallbackAccessPolicy(aId));
			}
		}
	CleanupStack::PopAndDestroy(rep);
	}
#else
/**
Attempt to reset a single key to it's value in the file in the given location. Routine
attempts to find a .cre file first. If ( and only if ) a cre file doesn't exist the 
routine attempts to find a txt file.
Note that it would be possible to use LoadRepositoryLC here but for the txt file
that would take longer. This is because in LoadRepositoryLC the txt file is 
completely processed. The Reset specific txt file opening code below is quicker because 
it is just attempting to find the reset key.
*/
void CServerRepository::ResetFromIniFileL(TUint32 aId, 
										  TCentRepLocation aLocation,
										  TBool& aKeyFound)
	{
	aKeyFound=EFalse;

	// Attempt to reset key to value in cre file if it exists
	
	// Attempt to create a temporary repository from the cre file in aLocation
	CSharedRepository* rep = CSharedRepository::NewL(iRepository->Uid());
	CleanupStack::PushL(rep);
	TInt err = rep->CreateRepositoryFromCreFileL(aLocation);

	// Search for aId in the temporary repository
	if (err!=KErrNotFound)
		{		
		// Note that for all errors except KErrNotFound code leaves and doesn't
		// attempt to look for txt file. This is intentional. Code does not 
		// attempt to support coexisting cre and txt files.
		User::LeaveIfError(err);
		
		// Search for aId in the temporary repository
		TServerSetting* s = rep->GetSettings().Find(aId);					
		if(s)
			{
			aKeyFound=ETrue;
			// Mark the setting as default again
			s->SetClean();
			iRepository->ResetAndPersistL(*s);
			s->SetAccessPolicy(GetFallbackAccessPolicy(aId));
			}
			
		CleanupStack::PopAndDestroy(rep);
		return;
		}
	else
		{
		CleanupStack::PopAndDestroy(rep);
		}
		
	HBufC* fileName(NULL);	
	TServerResources::CreateRepositoryFileNameLC(fileName,iRepository->Uid(),aLocation,EIni);
	
	CIniFileIn* inputFile = 0;
	TInt r = CIniFileIn::NewLC(TServerResources::iFs,inputFile,*fileName);
	if(r==KErrNone)
		{
		//we don't want to read this stuff again... just skip over to get to settings!
		inputFile->SkipOwnerSectionL() ;
		inputFile->SkipTimeStampSectionL() ;
		inputFile->SkipDefaultMetaSectionL() ;
		inputFile->SkipPlatSecSectionL();
		
		// Find start of Main section
		inputFile->FindMainSectionL();
	
		TServerSetting s;
		TBool singleMetaFound=EFalse;
		TBool singleReadPolicyFound=EFalse;
		TBool singleWritePolicyFound=EFalse;
		TSecurityPolicy singleReadPolicy;
		TSecurityPolicy singleWritePolicy;

		// Note that calling CIniFile::ReadSettingL causes the single policy ( if it exists ) to be read from the
		// file being reset to, but doesn't update the single policy array, which is not required in the reset case. 
		while((r=inputFile->ReadSettingL(s,singleReadPolicy, singleWritePolicy, singleReadPolicyFound, singleWritePolicyFound, singleMetaFound)) == KErrNone)
			{	
			iRepository->SetMetaDataOnRead( s, singleMetaFound);			
			if(s.Key()==aId)
				{
				// Mark the setting as default again
				s.SetClean();
				iRepository->ResetAndPersistL(s);
				s.SetAccessPolicy(GetFallbackAccessPolicy(aId));
				aKeyFound = ETrue;
				break;
				}
			s.Reset();
			}

	
		}
	CleanupStack::PopAndDestroy(inputFile);	 // inputFile
	CleanupStack::PopAndDestroy(fileName);	 // filename
	}

#endif	

TInt CServerRepository::ResetL(TUint32 aId)
	{
	// not yet supported in transactions
	ASSERT(!IsInTransaction());

	// if setting has not changed, there nothing to do
	TServerSetting *targetSetting = GetSetting(aId) ;

	if (targetSetting)
		{
		if ((targetSetting->Meta() & KMetaDefaultValue))
			{
			return KErrNone;
			}
		}

	TInt error = KErrNone;
	TBool keyReset = EFalse;

	// Check for default value in any installed file first
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS
	ResetFromIniFileL(aId, CIniFileIn::EInstallOnly, keyReset);
#else
	ResetFromIniFileL(aId, EInstall, keyReset);
#endif	
	if (keyReset)
		return KErrNone;

	// Either we couldn't find a matching key or
	// there wasn't an installed file - try for a ROM
	// file
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS	
	ResetFromIniFileL(aId, CIniFileIn::ERomOnly, keyReset);
#else
	ResetFromIniFileL(aId, ERom, keyReset);
#endif	
	if (keyReset)
		return KErrNone;
	
	// No default value found in install or ROM file
	// delete the key!
	error = iRepository->DeleteAndPersist(aId);

	return error ;
	}


void CServerRepository::CacheRomVersionL(const TDesC& aFilename,TDesC8& aVersion)
	{

	RFile file;
	TInt err = KErrNone; 
	_LIT(KTmpPersistedRomVersionFile, "_:\\private\\10202be9\\romversion\\romversion_info.tmp");	
	TBuf<KMaxFileName> tmpPersistedRomVersionFileName;
	
	tmpPersistedRomVersionFileName.Copy(KTmpPersistedRomVersionFile);
	tmpPersistedRomVersionFileName[0] = RFs::GetSystemDriveChar();
	
	//Create a new empty tmp file.
    err = file.Replace( TServerResources::iFs, tmpPersistedRomVersionFileName,
	                     EFileWrite | EFileStreamText );
	if (err != KErrNone)
	       {
	       file.Close();
	       User::Leave(err);
	       }

    err = file.Write(aVersion);
	if (err != KErrNone)
		{                                                                                                  
	       file.Close();
	       User::Leave(err);
	    }
	    
    file.Close();
    
	User::LeaveIfError(TServerResources::iFs.Replace(tmpPersistedRomVersionFileName,aFilename));

	}

#ifdef SYMBIAN_BAFL_SYSUTIL
void CServerRepository::CheckROMReflashL()
	{
	TInt err=KErrNone;

	TBuf16<KSysUtilVersionTextLength> version;
	TBuf8<KSysUtilVersionTextLength*2> persistedCopyOfRomVersion;
	_LIT(KPersistedRomVersionFile, "_:\\private\\10202be9\\romversion\\romversion_info.txt");	
	TBuf<KMaxFileName> persistedRomVersionFileName;
	persistedRomVersionFileName.Copy(KPersistedRomVersionFile);
	persistedRomVersionFileName[0] = RFs::GetSystemDriveChar();
		
	TBuf8<KSysUtilVersionTextLength*2> eightBitVersion;

	
	if ((err = SysUtil::GetSWVersion(version)) == KErrNone )
		{
		eightBitVersion.Copy(version);//Converts to 8bit
		err = TServerResources::GetTextFromFile(persistedRomVersionFileName,persistedCopyOfRomVersion);		
        if(err == KErrNone)
        	{
        	if(eightBitVersion == persistedCopyOfRomVersion)//No rom update has occurred do nothing
        		{
        		return;
        		}
        	else //rom update detected process persists files.
        		{
        		//Call function with flag set to true causing Reflash merging activity.
        		ProcessPersistsRepositoriesL(ECenRepReflash);
        		}
        	}

		//create the persisted rom version file
		//if the persists files are successfully processed
		//if the persists file doesnt exist
		//if the persists file is corrupt
		//if the persists file is corrupt in such a way that its contents are too large.
		if (err == KErrNone || err == KErrNotFound || err == KErrPathNotFound || err == KErrCorrupt || err == KErrTooBig)
			{
			CServerRepository::CacheRomVersionL(persistedRomVersionFileName,eightBitVersion);
			}
		else
			{
			User::Leave(err);
			}
		}
	else
		{
		User::Leave(err);
		}
	}
#endif

void CServerRepository::RFSAllRepositoriesL()
	{
	ProcessPersistsRepositoriesL(ECenRepReset);	
	}
	
void CServerRepository::ProcessPersistsRepositoriesL(TPersistedRepActions aRomFlashOrReset)
	{
	// Read contents of persist directory to get a list of repositories
	TPtr dataDirectory = TServerResources::iDataDirectory->Des();
	RDir persistDir;
    CleanupClosePushL(persistDir);

	User::LeaveIfError(persistDir.Open(TServerResources::iFs, dataDirectory, KEntryAttNormal));

    TEntryArray dirEntries;
    TInt readError = KErrNone;
    
	while (readError != KErrEof)  
		{
	    readError = persistDir.Read(dirEntries);
    
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
				if (!TServerResources::GetUid(const_cast<TEntry&>(dirEntries[i]), uid))
					{
					CSessionNotifier notifier;
	
					// Create shared repository
					CServerRepository *repository = new(ELeave) CServerRepository;
					CleanupStack::PushL(repository);
	
					repository->OpenL(uid, notifier);
			
					//Handle ROM re-flash
					TInt err = KErrNone;
					if(aRomFlashOrReset==ECenRepReflash)
						{
						TRAP(err, repository->HandleReflashofRepositoryL());
						}
					else if(aRomFlashOrReset==ECenRepReset)
						{
						// Restore settings
						TRAP(err,repository->RFSRepositoryL());	
						}
					if(err != KErrNone)
					  {
					  if(err == KErrNoMemory)
					    {
                        repository->Close();
					    User::LeaveNoMemory();
					    }
				      else
					    {//Dont stop processing the rest of the persisted repositories becos one has a problem.
					     __CENTREP_TRACE1("CENTREP: CServerRepository::ProcessPersistsRepositoriesL - Error = %d", err);
					    }
				      }
	
					// delete repository.
					repository->Close();
					CleanupStack::PopAndDestroy(repository);
					}
	    		}
	    	}
		}
	
	CleanupStack::PopAndDestroy(&persistDir);
	}

TInt CServerRepository::RFSRepositoryL()
	{
	// for each key in combined ROM/Install restore
	TUid uid = iRepository->Uid();

	CSharedRepository* defaultRepository = 0;
	TInt err=KErrNone;
	
	//Determine if ROM and Install files exist
  	TBool romExists=TServerResources::RomFileExistsL(uid);
  	TBool installExists=TServerResources::InstallFileExistsL(uid);
  		
	if(romExists)
		{
		// Create a rep using the ROM file
		TServerResources::iObserver->LoadRepositoryLC(uid, ETrue, defaultRepository, CIniFileIn::ERomOnly);
		
		if(installExists)
			{			
			CSharedRepository *installRep = 0;
			// Create install rep for merging
			TServerResources::iObserver->LoadRepositoryLC(uid, ETrue, installRep, CIniFileIn::EInstallOnly);
		
			// If install and ROM exist create a merged rep to Reset against
			defaultRepository->MergeL(*installRep, ESWIUpgradeMerge);
		
			//pop and destroy install repository as this has now been 
			//merged with repository
			CleanupStack::PopAndDestroy(installRep);
			}		
		}
		
	else if(installExists)
		{		
		// Reset against install repository if only the install file exists
		TServerResources::iObserver->LoadRepositoryLC(uid, ETrue, defaultRepository, CIniFileIn::EInstallOnly);		
		}
	else
		{
        // The repository must exist in the ROM or install directory (or both). 
        ASSERT(romExists || installExists);
		// Reset against empty repository if neither ROM or install file are found
		defaultRepository = CSharedRepository::NewL(uid);
		CleanupStack::PushL(defaultRepository);
		}

	for(TInt i = 0; i < iRepository->GetSettings().Count(); i++)
		{
		// setting in persists
		TServerSetting* persistedSetting = &iRepository->GetSettings()[i];

		// If the clean is set on setting in the persist, nothing to do			
		if (persistedSetting->Meta() & KMetaDefaultValue)
			{
			continue;
			}

		TUint32 key = persistedSetting->Key();		
		// setting in ROM/install
		TServerSetting* defaultSetting = defaultRepository->GetSettings().Find(key);

		if (defaultSetting)
			{
			if ((defaultSetting->Meta() & KMetaRfsValue))
				{
				iRepository->ResetNoPersistL(*defaultSetting);
				}
			//remove from Reset repository
			defaultRepository->GetSettings().Remove(key);
			}
		else
			{
			// if setting has no default value (i.e. doesn't exist in any default file but RFS meta is 
			// set (using pre-set default range meta),  delete the setting 
			if ((persistedSetting->Meta() & KMetaRfsValue))			
				{
				iRepository->DeleteNoPersist(key);				
				}
			}
		}
	// search for remaining items in default file, because previous loop has already removed all items 
	// from the persists file
	for(TInt i = 0; i < defaultRepository->GetSettings().Count(); i++)
		{
		TServerSetting* defaultSetting = &defaultRepository->GetSettings()[i];

		if ((defaultSetting->Meta() & KMetaRfsValue) != KMetaRfsValue)
			{
			continue;
			}
		iRepository->ResetNoPersistL(*defaultSetting);
		}

	// Persist settings
	iRepository->CommitChangesL();

	CleanupStack::PopAndDestroy(defaultRepository);

	return err;
	}

	
TInt CServerRepository::HandleReflashofRepositoryL()
	{
	// for each key in persists repository
	TUid uid = iRepository->Uid();

	CSharedRepository* defaultRepository = 0;
	
	//Determine if ROM and Install files exist
  	TBool romExists=TServerResources::RomFileExistsL(uid);
	TBool installExists=TServerResources::InstallFileExistsL(uid);
 
	if(romExists)
		{
		// Create a rep using the ROM file
		TServerResources::iObserver->LoadRepositoryLC(uid, ETrue, defaultRepository, CIniFileIn::ERomOnly);

		if(installExists)//Then create a merged repository of rom and install settings
			{		
			CSharedRepository *installRep = 0;
			// Create install rep for merging
			TServerResources::iObserver->LoadRepositoryLC(uid, ETrue, installRep, CIniFileIn::EInstallOnly);
		
			// If install and ROM exist create a merged rep to Reset against
			defaultRepository->MergeL(*installRep, ESWIUpgradeMerge);
		
			//pop and destroy install repository as this has now been 
			//merged with the rom repository
			CleanupStack::PopAndDestroy(installRep);
			}		
		}		
	else if(installExists)//There was no ROM repository just an install repository
		{			
		// Reset against install repository if only the install file exists
		TServerResources::iObserver->LoadRepositoryLC(uid, ETrue, defaultRepository, CIniFileIn::EInstallOnly);		
		}
	else //If rom file and install files have been removed for this repository
		{//then remove the persists file.
			TServerResources::DeleteCentrepFileL(uid, EPersists, ECre);
			TServerResources::DeleteCentrepFileL(uid, EPersists, EIni);
			return KErrNone;
		}

	// Merge rom and/or install with persists repository
	iRepository->MergeL(*defaultRepository, ERomFlash);

	// Persist settings
	iRepository->CommitChangesL();

	CleanupStack::PopAndDestroy(defaultRepository);

	return KErrNone;
	}


TInt CServerRepository::ResetAllL()
	{
	// not yet supported in transactions
	ASSERT(!IsInTransaction());
	// fail all sessions' transactions first
	iRepository->FailAllTransactions(/*aExcludeTransactor*/NULL);

	TUid uid = iRepository->Uid();

	// Reset
 	 	
	// Create a rep using the ROM file
	CSharedRepository* rep = 0;
  	TBool romExists=TServerResources::RomFileExistsL(uid);
	if(romExists)
		{
		TServerResources::iObserver->LoadRepositoryLC(uid, ETrue, rep, CIniFileIn::ERomOnly);
		}
		
	// Create install rep for merging
	CSharedRepository *installRep = 0;
	TBool installExists=TServerResources::InstallFileExistsL(uid);
	if(installExists)
		{			
		TServerResources::iObserver->LoadRepositoryLC(uid, ETrue, installRep, CIniFileIn::EInstallOnly);
		}
		
	TInt err=KErrNone;
	if(	romExists && installExists)
		{
		// If install and ROM exist create a merged rep to Reset against
		rep->MergeL(*installRep, ESWIUpgradeMerge);
		err=iRepository->ResetAllNoPersistL(*rep);
		CleanupStack::PopAndDestroy(installRep);
		CleanupStack::PopAndDestroy(rep);
		}
	else if(romExists)
		{
		// Reset against ROM
		err=iRepository->ResetAllNoPersistL(*rep);
		CleanupStack::PopAndDestroy(rep);
		}
	else if(installExists)
		{
		// Reset against install
		err=iRepository->ResetAllNoPersistL(*installRep);
		CleanupStack::PopAndDestroy(installRep);
		}
	else
		{
		// Reset against empty repository
		rep = CSharedRepository::NewL(uid);
		CleanupStack::PushL(rep);
		err=iRepository->ResetAllNoPersistL(*rep);
		CleanupStack::PopAndDestroy(rep);
		}
	
	return err;
	}

// Handle install directory file update. 
void CServerRepository::HandleSWIUpdateL(TUid aUid, TTime aModified, CSessionNotifier &aNotifier)
	{		
	// A file create or update has just occurred in the SWI directory. 
	// Need to check if this is a new install. 
	
	if(TServerResources::PersistsFileExistsL(aUid) ||
	   TServerResources::RomFileExistsL(aUid))
		{	
		// Create a rep using the ROM or persists file
		OpenL(aUid, aNotifier);
		if(iRepository->IsTransactionActive())			
			{
			// Fail transactions on any currently open session
			iRepository->FailAllTransactions(NULL);
			}
			
		// Create install rep for merging
 		CSharedRepository *installRep = 0;
 		TRAPD( err, TServerResources::iObserver->LoadRepositoryLC(aUid, ETrue, installRep, CIniFileIn::EInstallOnly); CleanupStack::Pop(installRep) );
	
		if (err == KErrNone)
            {
            // Perform merge
            TRAP( err, iRepository->HandleUpdateMergeL(aModified, *installRep) );
            }
        if (installRep!=NULL)
            {
            delete installRep;
            }
        Close();
        User::LeaveIfError(err);
		}
	else	// No ROM or persists
		{
		// Create install rep for persisting
		OpenL(aUid, aNotifier);
		TRAPD(err, iRepository->CommitChangesL());
	    Close();
	    User::LeaveIfError(err);
		}
	}


// Handle install directory file delete 
void CServerRepository::HandleSWIDeleteL(TUid aUid, CSessionNotifier &aNotifier)
	{			
	// A file delete has just occurred in the SWI directory. If there is no ROM file
	// this is a complete uninstall, so delete persists file.Otherwise, do downgrade
	// merge.
	
	if(TServerResources::RomFileExistsL(aUid))		// ROM file, this is a downgrade uninstall
		{
		if(!TServerResources::PersistsFileExistsL(aUid))
			{
			// If we are downgrading the ROM, there should be a persists file because the
			// original upgrade should have created one.
			// However if there isn't a persists file, there's nothing to do, so just return
			return;
			}
			
		// Create a rep using the persists file
		OpenL(aUid, aNotifier);
		if(iRepository->IsTransactionActive())			
			{
			// Fail transactions on any currently open session
			iRepository->FailAllTransactions(NULL);
			}
		
		// Create ROM rep for merging
	 	CSharedRepository *romRep = 0;
		TRAPD( err, TServerResources::iObserver->LoadRepositoryLC(aUid, ETrue, romRep, CIniFileIn::ERomOnly); CleanupStack::Pop(romRep) );

		if (err == KErrNone)
		    {
            // Perform merge
            TRAP( err, iRepository->HandleDeleteMergeL(*romRep) );
		    }
		if (romRep!=NULL)
		    {
            delete romRep;
		    }
		Close();
		User::LeaveIfError(err);
		}
	else											// No ROM file, this is a complete uninstall
		{		
		if(TServerResources::PersistsFileExistsL(aUid))
			{
		 	TServerResources::DeleteCentrepFileL(aUid, EPersists, ECre);
		 	
			// Check if the repository was open
			TInt i = TServerResources::iObserver->FindOpenRepository(aUid);

			// If repository is open, fail all transactions 
			if(i>KErrNotFound)			   					  
				{
				OpenL(aUid, aNotifier);
				if(iRepository->IsTransactionActive())			
					{
					// Fail transactions on any currently open session
					iRepository->FailAllTransactions(NULL);
					}
				iRepository->ResetContent();
				Close();				
				}
			}
		}
	}

void CServerRepository::StoreRepositoryContentsL(CStreamStore& aStore, TStreamId & aSettingStreamId, TStreamId & aDeletedSettingsStreamId) const
	{
	StoreRepositorySettingValuesL(aStore, aSettingStreamId); // Stores current repository setting values
	
	RStoreWriteStream outStream;
	aDeletedSettingsStreamId = outStream.CreateLC(aStore); // Creates the write for settings stream
	iRepository->WriteDeletedSettingsStream(outStream) ;
	outStream.CommitL(); // Commits the stream
	CleanupStack::PopAndDestroy(&outStream); // Performs cleanup on the write stream object
	}

void CServerRepository::StoreRepositorySettingValuesL(CStreamStore& aStore, TStreamId & aSettingStreamId) const
	{
	RStoreWriteStream outStream;
	aSettingStreamId = outStream.CreateLC(aStore); // Creates the write stream
	iRepository->WriteBackupStream(outStream); // Only care about repository contents.
	outStream.CommitL(); // Commits the stream
	CleanupStack::PopAndDestroy(&outStream); // Performs cleanup on the write stream object
	}
	
void CServerRepository::RestoreRepositoryContentsL(CStreamStore& aStore, TStreamId aSettingStreamId, TStreamId aDeletedSettingsStreamId, CRestoredRepository& aRstRepos)
	{
	RestoreRepositorySettingValuesL(aStore, aSettingStreamId, aRstRepos);
	
	RStoreReadStream inStream;
	// If the backup contains a list of deleted settings read them in and apply them.
	if (aDeletedSettingsStreamId != KNullStreamId)
		{
		inStream.OpenLC(aStore, aDeletedSettingsStreamId); // Creates read stream for deleted settings (if available)

		TCardinality numDeletedSettings ;
		inStream >> numDeletedSettings ;
		
		for (TInt i = 0; i < numDeletedSettings; i++)
			{
			TUint32 settingToDelete ;
			inStream >> settingToDelete ;
			TInt err = iRepository->DeleteNoPersist(settingToDelete) ; 
			// Add the deleted key to the restored repository if it has existed before being deleted.
			// If it has not existed before being deleted, we do not add it to the list because nothing 
			// has changed.
			if(err == KErrNone)
				{
				aRstRepos.AddKeyL(settingToDelete);
				}
			}
		CleanupStack::PopAndDestroy(&inStream);            // Perform cleanup on the read stream object		
		}
	return;
	}

void CServerRepository::RestoreRepositorySettingValuesL(CStreamStore& aStore, TStreamId aSettingStreamId, CRestoredRepository& aRstRepos)
	{
	RStoreReadStream inStream;
	inStream.OpenLC(aStore, aSettingStreamId); // Creates the write stream
	iRepository->InternalizeL(inStream, aRstRepos); // Only care about repository contents.
	CleanupStack::PopAndDestroy(&inStream);    // Perform cleanup on the read stream object
	}

static void CancelTransactionCleanupOperation(TAny* aRepository)
	{
	static_cast<CServerRepository*>(aRepository)->CancelTransaction();
	}

// So CancelTransaction is called in case of Leave. Must pop with CleanupStack::Pop() or similar
void CServerRepository::CleanupCancelTransactionPushL()
	{
	CleanupStack::PushL(TCleanupItem(CancelTransactionCleanupOperation, this));
	}

/**
@internalTechnology
Check the range of security policies against RMessage
@return
	KErrNone if read access policies of all settings in array pass,
	KErrPermissionDenied if any single policy fails.
*/
TInt CServerRepository::CheckPermissions(RSettingPointerArray& aSettings, const TClientRequest& aMessage, const char* aDiagnostic, TBool aReadPolicy,TUint32& aErrId)
	{
	TInt error = KErrNone;
	TInt numSettings = aSettings.Count();
	for (TInt i = 0; i < numSettings; i++)
		{
		ASSERT(aSettings[i]);
		const TServerSetting& setting = *aSettings[i];
		if (aReadPolicy)
			{
			if (!aMessage.CheckPolicy(GetReadAccessPolicy(setting),aDiagnostic))
				{
				aErrId=setting.Key();	
				error = KErrPermissionDenied;
				break;
				}
			}
		else
			{
			if (!aMessage.CheckPolicy(GetWriteAccessPolicy(setting),aDiagnostic))
				{
				aErrId=setting.Key();			
				error = KErrPermissionDenied;
				break;
				}			
			}
		}
	return error;
	}

TInt CServerRepository::TransactionDeleteRangeL(const TClientRequest& aMessage, TUint32& aErrorKey)
	{
	// all write operations now done in a transaction
	ASSERT(IsInActiveReadWriteTransaction());
	TInt error = KErrNone;
	aErrorKey = KUnspecifiedKey;
		
	TUint32 partialKey = aMessage.Int0();
	TUint32 keyMask = aMessage.Int1();
	
	RSettingPointerArray settingsToDelete;
	CleanupClosePushL(settingsToDelete);	
	error = FindSettings(partialKey, keyMask, settingsToDelete);
	if (error==KErrNoMemory)
		User::LeaveNoMemory();
	
	//perform write security check first
	error=CheckPermissions(settingsToDelete,aMessage,__PLATSEC_DIAGNOSTIC_STRING("CenRep: CServerRepository::DeleteRangeL - Attempt made to delete a setting"),EFalse,aErrorKey);
	
	if (error==KErrNone)
		{
		TRAP(error,DeleteSettingsRangeL(settingsToDelete,partialKey,aErrorKey));
		if (error==KErrNoMemory)
			User::LeaveNoMemory();
		}
	CleanupStack::PopAndDestroy(&settingsToDelete);

	if ((error != KErrNone) && (error != KErrNotFound))
		{
		FailTransaction(error, aErrorKey);
		}
	return error;
	}

TInt CServerRepository::TransactionMoveL(const TClientRequest& aMessage, TUint32& aErrorKey)	
	{
	// all write operations now done in a transaction
	ASSERT(IsInActiveReadWriteTransaction());	
	//read the source and target partial key
	TKeyFilter sourceKeyIdentifier;
	TPckg<TKeyFilter> pSource(sourceKeyIdentifier);
	aMessage.Read(0, pSource);

	TKeyFilter targetKeyIdentifier;
	TPckg<TKeyFilter> pTarget(targetKeyIdentifier);
	aMessage.Read(1, pTarget);

	TUint32 sourceToTarget = (sourceKeyIdentifier.iPartialId & sourceKeyIdentifier.iIdMask) ^ (targetKeyIdentifier.iPartialId & targetKeyIdentifier.iIdMask);
	if (sourceToTarget==0)
		{
		return KErrNone;
		}
	
	//Need to get the list of source settings to perform some security policy check
	RSettingPointerArray sourceSettings;
	CleanupClosePushL(sourceSettings);
	TInt error=FindSettings(sourceKeyIdentifier.iPartialId & sourceKeyIdentifier.iIdMask, sourceKeyIdentifier.iIdMask, sourceSettings);
	
	//dont fail transaction if source settings is empty
	if ((error == KErrNone) && (sourceSettings.Count() == 0))
		{
		error = KErrNotFound;
		aErrorKey = sourceKeyIdentifier.iPartialId;
		CleanupStack::PopAndDestroy(&sourceSettings);
		TPckg<TUint32> p(aErrorKey);
		aMessage.WriteL(2, p);		
		return error;

		}
	if (error!=KErrNone)
		{
		aErrorKey = sourceKeyIdentifier.iPartialId;
		CleanupStack::PopAndDestroy(&sourceSettings);
		return error;				
		}	
		
	//Now validate against the security policy before doing the settings move
	error=CheckMovePermissions(sourceSettings,aMessage,sourceToTarget,aErrorKey);
	if (error!=KErrNone)
		{
		CleanupStack::PopAndDestroy(&sourceSettings);
		return error;		
		}	
	
	error =MoveL(sourceKeyIdentifier.iPartialId,targetKeyIdentifier.iPartialId,sourceKeyIdentifier.iIdMask,aErrorKey, sourceSettings);
	CleanupStack::PopAndDestroy(&sourceSettings);
	return error;	
	}

void CServerRepository::LoadIniRepL(CIniFileIn::TIniFileOpenMode aMode)
	{
	if (iIniRep == NULL)
		{
	 	CSharedRepository *rep = NULL;
		TServerResources::iObserver->LoadRepositoryLC(iUid, ETrue, rep, aMode);
		CleanupStack::Pop();
		iIniRep = rep;
		}
	}

TBool CServerRepository::GetMetaFromIni(TUint32 aKey, TUint32& aMeta)
	{
	// Note: cannot use iRepository even if 
	// iRepository->iSettings.IsDefault() is true.
	// The flag is not updated on TransactionCommit.
	if (iIniRep == NULL)
		{
		TInt err;
		TRAP(err, LoadIniRepL(CIniFileIn::EInstallOnly));
		if (err != KErrNone)
			{
			TRAP(err,LoadIniRepL(CIniFileIn::ERomOnly));
			}
		if (err != KErrNone)
			{
			return EFalse;
			}
		}

	ASSERT(iIniRep);
	TServerSetting* s = iIniRep->GetSettings().Find(aKey);					
	if (s)
		{
		aMeta = s->Meta();
		return ETrue;
		}

	return EFalse;
	}

void CServerRepository::RestoreInstallRepositoryL(TUid aUid, CStreamStore& aStore, TStreamId& aSettingStreamId, CRestoredRepository& aRstRepos)
	{
	iRepository = CSharedRepository::NewL(aUid);
	CleanupStack::PushL(iRepository);
	iUid = aUid;
	RestoreRepositorySettingValuesL(aStore, aSettingStreamId, aRstRepos);
	CommitChangesL(EInstall);
	CleanupStack::PopAndDestroy(iRepository);
	iRepository = NULL;
	}

void CServerRepository::BackupInstallRepositoryL(TUid aUid, CStreamStore& aStore, TStreamId& aSettingStreamId)
	{
	TServerResources::iObserver->LoadRepositoryLC(aUid, EFalse, iRepository, CIniFileIn::EInstallOnly);
	iUid = aUid;
	StoreRepositorySettingValuesL(aStore, aSettingStreamId);	
	CleanupStack::PopAndDestroy(iRepository);
	iRepository = NULL;
	}

TInt CServerRepository::CheckAccessPolicyBeforeMoving(const TClientRequest& aMessage, const TServerSetting* aSourceSetting, 
				TUint32 aSourceKey, const TServerSetting* aTargetSetting, TUint32 aTargetKey, TUint32& aErrorKey)
	{
	TInt error = KErrNone;
	
	if (aTargetSetting && !aTargetSetting->IsDeleted())
		{
		error=KErrAlreadyExists;
		aErrorKey=aTargetKey;
		}

	if (!aMessage.CheckPolicy(GetReadAccessPolicy(*aSourceSetting),
		__PLATSEC_DIAGNOSTIC_STRING("CenRep: CServerRepository::MoveL - Attempt made to read a setting")))
		{
		error = KErrPermissionDenied;
		aErrorKey = aSourceKey;
		}
	else if (!aMessage.CheckPolicy(GetWriteAccessPolicy(*aSourceSetting),
		__PLATSEC_DIAGNOSTIC_STRING("CenRep: CServerRepository::MoveL - Attempt made to delete a setting")))
		{
		error = KErrPermissionDenied;
		aErrorKey = aSourceKey;
		}
	else if (error == KErrAlreadyExists)
		{
		// set error to KErrPermissionDenied in preference to KErrAlreadyExists
		if (!aMessage.CheckPolicy(GetWriteAccessPolicy(*aTargetSetting),
			__PLATSEC_DIAGNOSTIC_STRING("CenRep: CServerRepository::MoveL - Attempt made to create a setting")))
			{
			error = KErrPermissionDenied;
			aErrorKey = aTargetKey;
			}
		}
	else if (!aMessage.CheckPolicy(GetFallbackWriteAccessPolicy(aTargetKey),
			__PLATSEC_DIAGNOSTIC_STRING("CenRep: CServerRepository::MoveL - Attempt made to create a setting")))
		{
		error = KErrPermissionDenied;
		aErrorKey = aTargetKey;
		}
	return error;
	}
