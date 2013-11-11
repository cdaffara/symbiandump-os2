// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32test.h>
#include <f32file.h>
#include <badesca.h>

#include "srvrepos_noc.h"
#include "srvres.h"
#include "cachemgr.h"
#include "sessnotf.h"
#include "t_cenrep_helper.h"

#include "testexecute/SWI/src/tui.h"
#include <swi/launcher.h>
#include "swi/sisregistrysession.h"
#include "swi/sisregistrypackage.h"
#include "swi/sisregistryentry.h"


RTest TheTest(_L("Central Repository FOTx Unit Tests"));

_LIT(KCachedVersionFile,"C:\\private\\10202be9\\romversion\\romversion_info.txt");
_LIT(KModified2RepositoryCre,"z:\\private\\10202be9\\10033333_modified2.cre");
_LIT(KPersistsRepositoryCre,"C:\\private\\10202be9\\persists\\10033333.cre");
_LIT(KStoredSWIRepository,"z:\\private\\10202be9\\10033333_modified3.cre");
_LIT(KSWIInstallRepository,"C:\\private\\10202be9\\10033333.cre");
_LIT(KModifiedSwVersion, "z:\\private\\10202be9\\sw_modified.txt");
_LIT(KStoredRepository,"z:\\private\\10202be9\\10033340_reserved.cre");
_LIT(KStoredRepository2,"z:\\private\\10202be9\\10033341_orig.cre");
_LIT(KPersistsBasedRepository,"C:\\private\\10202be9\\persists\\10033340.cre");
_LIT(KPersistsBasedRepository2,"C:\\private\\10202be9\\persists\\10033341.cre");

_LIT(KRomRepositoryCre1,"z:\\private\\10202be9\\10043336_persists.cre");
_LIT(KRomRepositoryCre2,"z:\\private\\10202be9\\10043337_persists.cre");
_LIT(KRomRepositoryCre3,"z:\\private\\10202be9\\10043338_persists.cre");
_LIT(KPersistsRepositoryCre1,"C:\\private\\10202be9\\persists\\10043336.cre");
_LIT(KPersistsRepositoryCre2,"C:\\private\\10202be9\\persists\\10043337.cre");
_LIT(KPersistsRepositoryCre3,"C:\\private\\10202be9\\persists\\10043338.cre");


_LIT(KSisFile,"z:\\private\\10202be9\\RF1.sis");
_LIT(KSisFile2,"z:\\private\\10202be9\\RF4.sis");

const TUid KUidRomRepositoryList[]   = {0x10033333,0x10033340,0x10033341,0x10043336,0x10043337,0x10043338};
const TUint32 KNewSettingList[]      = {1000,1001,0x2300,0x02010000};
const TUint32 KModifiedSettingList[] = {0x04010200,0x04010400,0x03010100};
const TUint32 KDeletedSettingList[]  = {0x03010500,0x04010100,0x02010300};
const TUint32 KSWIBasedSettingList[] = {0x08010100};
const TInt KSettingValueList[]       = {1234,1111,12345,10,200,400};

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macroses and functions

LOCAL_C void DeleteFilesL()
	{
	_LIT( KOldInstallFiles, "c:\\private\\10202BE9\\*.cre" );
	_LIT( KOldPersistsFiles, "c:\\private\\10202BE9\\persists\\100*.cre" );
	_LIT( KOldRomVersionFiles, "c:\\private\\10202BE9\\romversion\\romversion_info.t*" );
	CFileMan* fm = CFileMan::NewL( TServerResources::iFs );
	CleanupStack::PushL( fm );

	TInt r = fm->Delete( KOldInstallFiles );
	if ( r != KErrNone && r != KErrNotFound && r != KErrPathNotFound )
		User::Leave(r);
	r = fm->Delete( KOldPersistsFiles);
	if ( r != KErrNone && r != KErrNotFound && r != KErrPathNotFound )
		User::Leave(r);
	r = fm->Delete( KOldRomVersionFiles,CFileMan::ERecurse);
	if ( r != KErrNone && r != KErrNotFound && r != KErrPathNotFound )
		User::Leave(r);

	CleanupStack::PopAndDestroy( fm );
	}

LOCAL_C void Check( TInt aValue, TInt aLine )
	{
	if ( !aValue )
		{
		TheTest( EFalse, aLine );
		}
	}

LOCAL_C void Check( TInt aValue, TInt aExpected, TInt aLine )
	{
	if ( aValue != aExpected )
		{
		RDebug::Print( _L( "*** Expected error: %d, got: %d\r\n"), aExpected, aValue );
		TheTest( EFalse, aLine );
		}
	}

#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

LOCAL_C void CleanUp()
	{
	// If cache manager is initialized and used before, we flush it
	if (TServerResources::iCacheManager)
		{
		TServerResources::iCacheManager->FlushCache();
		}
	// To get rid of the iOpenRepositories array leaking problem during OOM testing.
	TServerResources::iObserver->CloseiOpenRepositories();
	TServerResources::iObserver->Reset();
	// To get rid of the array leaking problems during OOM testing.
	TServerResources::iOwnerIdLookUpTable.Reset();
	User::After(KGeneralDelay);
	}

LOCAL_C void PerformSWIinstallL(const TDesC& aSisFileName)
{
//********************************************************
//Requires stub file to be set up before it will work
//requires techview
//********************************************************
   	using namespace Swi;
   	TUI* Ui = new(ELeave) TUI();
   	CInstallPrefs* prefs = CInstallPrefs::NewLC();
   	TInt err = Launcher::Install(*Ui, aSisFileName, *prefs);
   	delete Ui;
 	TEST2(err, KErrNone);
   	CleanupStack::PopAndDestroy();//prefs

    User::After(KGeneralDelay);

	// If cache manager is initialized and used before, we flush it
	CleanUp();
}

LOCAL_C void UserModifyRepositoryL()
{
	CSessionNotifier* notifier = new(ELeave)CSessionNotifier;
	CleanupStack::PushL(notifier);

    CServerRepository* repository = new(ELeave) CServerRepository();
    CleanupStack::PushL(repository);

    repository->OpenL(KUidRomRepositoryList[0],*notifier);

    User::LeaveIfError(repository->StartTransaction(EReadWriteTransaction));

	TInt err = repository->TransactionDeleteL(KDeletedSettingList[0]);
	TEST2(err,KErrNone);

	err = repository->TransactionCreateL(KNewSettingList[1],KSettingValueList[0],NULL);
	TEST2(err,KErrNone);
	err = repository->TransactionCreateL(KNewSettingList[2],KSettingValueList[0],NULL);
	TEST2(err,KErrNone);
	err = repository->TransactionDeleteL(KDeletedSettingList[2]);
 	TEST2(err,KErrNone);
 	err = repository->TransactionDeleteL(KNewSettingList[1]);//remove a setting added by the user
 	TEST2(err,KErrNone);
	err = repository->TransactionSetL(KModifiedSettingList[2],KSettingValueList[2]);
	TEST2(err,KErrNone);

	TUint32 tempKeyInfo;
	User::LeaveIfError(repository->CommitTransaction(tempKeyInfo));

	repository->Close();
	CleanupStack::PopAndDestroy(2);//repository, notifier
}

LOCAL_C void UserModifyRepository2L()
{
	//Have the user add,delete and modify some entries
	CSessionNotifier* notifier = new(ELeave)CSessionNotifier;
	CleanupStack::PushL(notifier);

    CServerRepository* repository = new(ELeave) CServerRepository();
    CleanupStack::PushL(repository);
    repository->OpenL(KUidRomRepositoryList[0],*notifier);

    User::LeaveIfError(repository->StartTransaction(EReadWriteTransaction));
   	//0x03010500 has been deleted by user in modified persisted repository.
    //but this rom based deleted setting, should have been reinstated by the SWI, which also contains it.
	TInt settingValue = 0;
	TInt r = repository->Get(KDeletedSettingList[2], settingValue);
	TEST(r == KErrNone);

	//delete the SWI and rom based setting, it should be reinstated after the rom update.
	r = repository->TransactionDeleteL(KDeletedSettingList[2]);
 	TEST2(r,KErrNone);
	//
	//Modify setting
	r = repository->TransactionSetL(KModifiedSettingList[2],KSettingValueList[2]);
	TEST2(r,KErrNone);
	//Add and delete a user setting
	r = repository->TransactionCreateL(KNewSettingList[1],KSettingValueList[0],NULL);
	TEST2(r,KErrNone);
	 r = repository->TransactionDeleteL(KNewSettingList[1]);//remove a setting added by the user
 	TEST2(r,KErrNone);
 	//Add a user setting
	r = repository->TransactionCreateL(KNewSettingList[2],KSettingValueList[0],NULL);
	TEST2(r,KErrNone);

	TUint32 tempKeyInfo;
	User::LeaveIfError(repository->CommitTransaction(tempKeyInfo));
	repository->Close();
	CleanupStack::PopAndDestroy(2);//repository, notifier
}

LOCAL_C void VerifyCorrectMergingL(TInt aVerifySwi = EFalse)
{
	//Check the merge content for correctness.
	CSessionNotifier* notifier = new(ELeave)CSessionNotifier;
	CleanupStack::PushL(notifier);

    CServerRepository* repository = new(ELeave) CServerRepository();
    CleanupStack::PushL(repository);

    repository->OpenL(KUidRomRepositoryList[0],*notifier);

	//Get the rom based user deleted setting, it should have been reinstated.
	TInt settingValue = 0;
	TInt r = repository->Get(KDeletedSettingList[2], settingValue);
	TEST(r == KErrNone);
	//Get the user based deleted setting, it should still be missing.
	r = repository->Get(KNewSettingList[1], settingValue);
	TEST(r == KErrNotFound);
	//Get the user added setting.
	r = repository->Get(KNewSettingList[2], settingValue);
	TEST(r == KErrNone);
	//Get user modified settings. It should contain the user modification
	r = repository->Get(KModifiedSettingList[2], settingValue);
	TEST(r == KErrNone);
	TEST(settingValue == KSettingValueList[2]);

	if(aVerifySwi)
	{
	//Get SWI modified settings for a clean rom value. It should contain the SWI modification
	settingValue = 0;
	r = repository->Get(KModifiedSettingList[0], settingValue);
	TEST(r == KErrNone);
	TEST(settingValue == KSettingValueList[2]);
	//Get SWI added setting
	r = repository->Get(KNewSettingList[0], settingValue);
	TEST(r == KErrNone);
	}
	repository->Close();
	CleanupStack::PopAndDestroy(2);//repository, notifier
}

/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-UT-4025
@SYMTestCaseDesc		This test verifies that a locally persisted copy of the rom version file
						is created after a call to CheckROMReflashL
@SYMTestPriority		High
@SYMTestActions			Remove locally persisted rom version file.
						Call CheckROMReflashL to simulate a reboot of cenrep.
						Check that locally persisted copy of rom version file is created.
@SYMTestExpectedResults Persisted copy of rom version file has been created.
@SYMCR					CR1198
*/
LOCAL_C void SYSLIB_CENTRALREPOSITORY_UT_4025L()
{
	TheTest.Next( _L( " @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-UT-4025 " ) );
	CFileMan* fm = CFileMan::NewL(TServerResources::iFs);

	// Remove locally stored rom version file if it exists
	TInt err = fm->Delete(KCachedVersionFile);
	if((err != KErrNotFound) && (err != KErrNone))
	  {
	  RDebug::Print( _L( "*** UnExpected error: %d\r\n"), err );
	  TheTest( EFalse, __LINE__ );
	  }

	delete fm;//fm
	CServerRepository::CheckROMReflashL();
	User::After(KGeneralDelay);//Allow the filesystem to write the files.

	TEntry entry;
	TEST(TServerResources::iFs.Entry(KCachedVersionFile,entry) == KErrNone);

}

/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-UT-4026
@SYMTestCaseDesc		This test verifies that when the locally persisted copy of the rom version file
						is equal to the rom version file then no merging takes place.
@SYMTestPriority		High
@SYMTestActions			Remove a rom based setting.
						Call CheckROMReflashL to simulate a reboot of cenrep.
						Check that the deleted value hasnt been reinstated.
@SYMTestExpectedResults Deleted value hasnt been reinstated, demonstrating that no merge took place.
@SYMCR					CR1198
*/
LOCAL_C void SYSLIB_CENTRALREPOSITORY_UT_4026L()
{
	TheTest.Next( _L( " @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-UT-4026 " ) );
	CFileMan* fm = CFileMan::NewL(TServerResources::iFs);

	//Initial cleanup
	fm->Delete(KPersistsRepositoryCre);
	delete fm;//fm

	CSessionNotifier* notifier = new(ELeave)CSessionNotifier;
	CleanupStack::PushL(notifier);

    CServerRepository* repository = new(ELeave) CServerRepository();
    CleanupStack::PushL(repository);

    repository->OpenL(KUidRomRepositoryList[0],*notifier);

	//Attempt to retrieve a setting before deleting it.
    User::LeaveIfError(repository->StartTransaction(EReadWriteTransaction));
	TInt settingValue = 0;
	TInt r = repository->Get(KDeletedSettingList[0], settingValue);
	TEST(r == KErrNone);

	repository->TransactionDeleteL(KDeletedSettingList[0]);
	TUint32 tempKeyInfo;
	User::LeaveIfError(repository->CommitTransaction(tempKeyInfo));

	repository->Close();
	CleanupStack::PopAndDestroy(2);//repository, notifier

	//Dont Copy modified stored rom version info so that no rom update should take place

	// flush cache
	CleanUp();

	CServerRepository::CheckROMReflashL();

	notifier = new(ELeave)CSessionNotifier;
	CleanupStack::PushL(notifier);

    repository = new(ELeave) CServerRepository();
    CleanupStack::PushL(repository);

    repository->OpenL(KUidRomRepositoryList[0],*notifier);

	r = repository->Get(KDeletedSettingList[0], settingValue);
	TEST(r == KErrNotFound);//It hasnt been reinstated.

	repository->Close();

	CleanupStack::PopAndDestroy(2);//repository, notifier

	// If cache manager is initialized and used before, we flush it
	CleanUp();
}

/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-UT-4027
@SYMTestCaseDesc		This test verifies that after a rom flash, rom and user modified values are merged correctly.
@SYMTestPriority		High
@SYMTestActions			Open a repository and create two settings.
						Delete a rom based setting.
						Delete on of the new settings.
						Remove a rom based setting.
						Modify a rom based setting.
						Call CheckROMReflashL to simulate a reboot of cenrep.
						Check that the deleted rom base value has been reinstated.
						Check that the deleted user setting hasnt been reinstated.

@SYMTestExpectedResults Deleted user value hasnt been reinstated.
						Deleted rom based value has been reinstated.
						Other User added setting is present.
						User modified setting contains user value.
@SYMCR					CR1198
*/
LOCAL_C void SYSLIB_CENTRALREPOSITORY_UT_4027L()
{
	TheTest.Next( _L( " @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-UT-4027 " ) );//Force a rom update for this repository.

	CFileMan* fm = CFileMan::NewL(TServerResources::iFs);
	CleanupStack::PushL(fm);

	//Initial cleanup
	fm->Delete(KPersistsRepositoryCre);

	//Copy of modified stored rom version info
	User::LeaveIfError(fm->Copy(KModifiedSwVersion,KCachedVersionFile,CFileMan::EOverWrite));
	User::LeaveIfError(fm->Attribs(KCachedVersionFile,0,KEntryAttReadOnly,TTime(0)));
	User::After(KGeneralDelay);

	UserModifyRepositoryL();

	// flush the cache manager contents.
	CleanUp();

	CServerRepository::CheckROMReflashL();

	VerifyCorrectMergingL(EFalse);

	//Final Cleanup
	fm->Delete(KPersistsRepositoryCre);

	CleanupStack::PopAndDestroy();//fm

	// If cache manager is initialized and used before, we flush it
	CleanUp();
}

/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-UT-4028
@SYMTestCaseDesc		This test verifies that repositories that exist in persists directory but have
						no basis in install or rom are removed after a rom update.
@SYMTestPriority		High
@SYMTestActions			Copy a repository into the persists directory that has no rom or SWI equivalent.
						Call CheckROMReflashL to simulate a reboot of cenrep.
						Verify that the persisted repository is removed.

@SYMTestExpectedResults Persisted repository is removed during rom update.
@SYMCR					CR1198
*/
LOCAL_C void SYSLIB_CENTRALREPOSITORY_UT_4028L()
{
	TheTest.Next( _L( " @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-UT-4028 " ) );
	CFileMan* fm = CFileMan::NewL(TServerResources::iFs);
	CleanupStack::PushL(fm);

	//Initial cleanup
	fm->Delete(KPersistsBasedRepository);

	//Place a repository in persists that has no basis in SWI or rom and it should be removed.
	//by the rom update as it will look like a rom based repository that has been removed by
	//the update.

	//Copy the repository that isnt in SWI or ROM to persists
	//Copy 100333340_reserved to 100333340.cre persists
	User::LeaveIfError(fm->Copy(KStoredRepository,KPersistsBasedRepository,CFileMan::EOverWrite));
	User::LeaveIfError(fm->Attribs(KPersistsBasedRepository,0,KEntryAttReadOnly,TTime(0)));
	User::After(KGeneralDelay);
	//Copy of modified stored rom version info

	User::LeaveIfError(fm->Copy(KModifiedSwVersion,KCachedVersionFile,CFileMan::EOverWrite));
	User::LeaveIfError(fm->Attribs(KCachedVersionFile,0,KEntryAttReadOnly,TTime(0)));
	User::After(KGeneralDelay);
	CleanupStack::PopAndDestroy();//fm

	CServerRepository::CheckROMReflashL();
	TEntry entry;

	TEST(TServerResources::iFs.Entry(KPersistsBasedRepository,entry) == KErrNotFound);

}

/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-UT-4029
@SYMTestCaseDesc		This test verifies that a repository is merged correctly with SWI and user changes after a rom update.
@SYMTestPriority		High
@SYMTestActions			Copy a prepared repository into the persists directory that contains clean
						settings that do not exist in its rom equivalent,missing settings that exist in
						its rom equivalent and user modified settings that differ from its rom equivalent.
						Perform an SWI install to modify and add some settings.
						The user adds,deletes and modifies some settings.
						Copy a modified rom version file over the locally persisted rom version file.
						Call CheckROMReflashL to simulate a reboot of cenrep.
@SYMTestExpectedResults Verify that the persisted repository clean settings that do not exist in rom equivalent are removed.
						Verify that settings in the rom repository that are not in the persisted repository are added.
						Verify that user modified settings retain user values.
						Verify that the rom based user deleted setting are reinstated.
						Verify that the user based deleted setting is not present.
						Verify the user added setting exists.
@SYMCR					CR1198
*/
LOCAL_C void SYSLIB_CENTRALREPOSITORY_UT_4029L()
{
	TheTest.Next( _L( " @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-UT-4029 " ) );
	CFileMan* fm = CFileMan::NewL(TServerResources::iFs);
	CleanupStack::PushL(fm);

	//Initial cleanup
	fm->Delete(KPersistsRepositoryCre);

	//Merge User, Rom and SWI settings.
	//Need to copy a repository into the persists, that has a variance from the rom based
	//repository to give the impression that some of the rom based entries have been modified,
	// removed and added by the rom update.
	//Copy a modified repository

	User::LeaveIfError(fm->Copy(KModified2RepositoryCre,KPersistsRepositoryCre,CFileMan::EOverWrite));
	User::LeaveIfError(fm->Attribs(KPersistsRepositoryCre,0,KEntryAttReadOnly,TTime(0)));
	User::After(KGeneralDelay);
	//Have SWI add and modify some entries.
    TFileName SisFileName;
    SisFileName.Copy(KSisFile);

    PerformSWIinstallL(SisFileName);
	User::After(KGeneralDelay*5);

	UserModifyRepository2L();

	//flush cachemanager contents
	CleanUp();

	User::LeaveIfError(fm->Copy(KModifiedSwVersion,KCachedVersionFile,CFileMan::EOverWrite));
	User::LeaveIfError(fm->Attribs(KCachedVersionFile,0,KEntryAttReadOnly,TTime(0)));
	User::After(KGeneralDelay);

	CleanupStack::PopAndDestroy();//fm

	CServerRepository::CheckROMReflashL();

	VerifyCorrectMergingL(ETrue);

	// If cache manager is initialized and used before, we flush it
	CleanUp();
}

/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-UT-4030
@SYMTestCaseDesc		A repository is removed from the ROM but has an SWI, it remains on the C:\
						but all clean settings that do not exist in the SWI install file are removed.
@SYMTestPriority		High
@SYMTestActions			Copy a prepared repository into the persists directory that has no basis
						in ROM.
						Perform an SWI, that adds new keys to the persisted repository.
						Have the user create a new entry.
						Have the user modify a rom based entry.
						Update the locally persisted rom version file.
						Call CheckROMReflashL to simulate a centrep reboot.
@SYMTestExpectedResults Verify the user modified rom based setting exists.
						Verify that a clean rom based setting is removed.
						Verify that a sample of the SWI based setting exists.
@SYMCR					CR1198
*/
LOCAL_C void SYSLIB_CENTRALREPOSITORY_UT_4030L()
{
	TheTest.Next( _L( " @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-UT-4030 " ) );
	CFileMan* fm = CFileMan::NewL(TServerResources::iFs);
	CleanupStack::PushL(fm);

	//Initial cleanup
	DeleteFilesL();

	//Remove ROM and Merge User and SWI settings.
	//Need to copy a prepared repository into the persists,that has more entries than the SWI based
	//repository to simulate that some of the rom based entries have been removed,
	//by the rom update. All the clean entries should be removed, leaving only user modified ones and common SWI ones.
	User::LeaveIfError(fm->Copy(KStoredRepository2,KPersistsBasedRepository2,CFileMan::EOverWrite));
	User::LeaveIfError(fm->Attribs(KPersistsBasedRepository2,0,KEntryAttReadOnly,TTime(0)));
	User::After(KGeneralDelay);
	//Have SWI add some entries 0x08010100-500.
    TFileName SisFileName;
    SisFileName.Copy(KSisFile2);

    PerformSWIinstallL(SisFileName);
	User::After(KGeneralDelay*2);

	//Have the user add and modify some rom based entries
	CSessionNotifier* notifier = new(ELeave)CSessionNotifier;
	CleanupStack::PushL(notifier);

    CServerRepository* repository = new(ELeave) CServerRepository();
    CleanupStack::PushL(repository);
    repository->OpenL(KUidRomRepositoryList[2],*notifier);

    User::LeaveIfError(repository->StartTransaction(EReadWriteTransaction));
   	//
	//Modify setting
	TInt r = repository->TransactionSetL(KModifiedSettingList[2],KSettingValueList[2]);
	TEST2(r,KErrNone);

 	//Add a setting
	r = repository->TransactionCreateL(KNewSettingList[2],KSettingValueList[0],NULL);
	TEST2(r,KErrNone);

 	//Get some clean rom based settings to show they are present
 	TInt settingValue = 0;
	r = repository->Get(KModifiedSettingList[0], settingValue);
	TEST(r == KErrNone);

 	//Get some clean rom based settings to show they are present
 	settingValue = 0;
	r = repository->Get(KModifiedSettingList[1], settingValue);
	TEST(r == KErrNone);

	TUint32 tempKeyInfo;
	User::LeaveIfError(repository->CommitTransaction(tempKeyInfo));
	repository->Close();
	CleanupStack::PopAndDestroy(2);//repository, notifier

	// If cache manager is initialized and used before, we flush it
	CleanUp();

	//Copy of modified stored rom version info
	User::LeaveIfError(fm->Copy(KModifiedSwVersion,KCachedVersionFile,CFileMan::EOverWrite));
	User::LeaveIfError(fm->Attribs(KCachedVersionFile,0,KEntryAttReadOnly,TTime(0)));
	User::After(KGeneralDelay);
	CleanupStack::PopAndDestroy();//fm

	CServerRepository::CheckROMReflashL();

	//Check the merge content for correctness.
	notifier = new(ELeave)CSessionNotifier;
	CleanupStack::PushL(notifier);

    repository = new(ELeave) CServerRepository();
    CleanupStack::PushL(repository);
    repository->OpenL(KUidRomRepositoryList[2],*notifier);

	//Get the rom based user modified setting it should still be there
	settingValue = 0;
	r = repository->Get(KModifiedSettingList[2], settingValue);
	TEST(r == KErrNone);

	//Get the rom only based clean setting, it should still be missing.
	r = repository->Get(KModifiedSettingList[0], settingValue);
	TEST(r == KErrNotFound);

	//Get the rom only based clean setting, it should still be missing.
	r = repository->Get(KModifiedSettingList[1], settingValue);
	TEST(r == KErrNotFound);

	//Get SWI based setting. It should be there
	r = repository->Get(KSWIBasedSettingList[0], settingValue);
	TEST(r == KErrNone);

	//Get user based setting. It should be there
	r = repository->Get(KNewSettingList[2], settingValue);
	TEST(r == KErrNone);
	repository->Close();
	CleanupStack::PopAndDestroy(2);//repository, notifier

	// If cache manager is initialized and used before, we flush it
	CleanUp();
}

/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-UT-4031
@SYMTestCaseDesc		Test that repository settings that are altered by SWI value are not overwritten by a rom flash.
						SWI values dominate ROM values.
@SYMTestPriority		High
@SYMTestActions			Copy a prepared repository into the persists directory and into the install directory.
						There exists and equivalent rom based version of this repository with different values.
						Update the locally persisted rom version file.
						Call CheckROMReflashL to simulate a centrep reboot.
@SYMTestExpectedResults Verify the user modified rom based setting exists.
						Verify that some settings that exists within ROM and SWI has SWI values.
@SYMCR					CR1198
*/
LOCAL_C void SYSLIB_CENTRALREPOSITORY_UT_4031L()
{
	TheTest.Next( _L( " @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-UT-4031 " ) );
	CFileMan* fm = CFileMan::NewL(TServerResources::iFs);
	CleanupStack::PushL(fm);

	//Initial cleanup
	fm->Delete(KPersistsRepositoryCre);
	fm->Delete(KSWIInstallRepository);

	//Simulate an SWI install by having the modified repository in the install and persists directory.
	User::LeaveIfError(fm->Copy(KStoredSWIRepository,KPersistsRepositoryCre,CFileMan::EOverWrite));
	User::LeaveIfError(fm->Attribs(KPersistsRepositoryCre,0,KEntryAttReadOnly,TTime(0)));
	User::After(KGeneralDelay);
	User::LeaveIfError(fm->Copy(KStoredSWIRepository,KSWIInstallRepository,CFileMan::EOverWrite));
	User::LeaveIfError(fm->Attribs(KSWIInstallRepository,0,KEntryAttReadOnly,TTime(0)));
	User::After(KGeneralDelay);

	//Copy of modified stored rom version info
	User::LeaveIfError(fm->Copy(KModifiedSwVersion,KCachedVersionFile,CFileMan::EOverWrite));
	User::LeaveIfError(fm->Attribs(KCachedVersionFile,0,KEntryAttReadOnly,TTime(0)));
	User::After(KGeneralDelay);
	CleanupStack::PopAndDestroy();//fm

	CServerRepository::CheckROMReflashL();

	CSessionNotifier* notifier = new(ELeave)CSessionNotifier;
	CleanupStack::PushL(notifier);

    CServerRepository* repository = new(ELeave) CServerRepository();
    CleanupStack::PushL(repository);
    repository->OpenL(KUidRomRepositoryList[0],*notifier);

   	//Get the rom based user modified setting it should still be there
	TInt settingValue = 0;
	TInt r = repository->Get(KModifiedSettingList[0], settingValue);
	TEST(r == KErrNone);
	TEST(settingValue == KSettingValueList[4]);

	//Get the rom and SWI setting it should have SWI value
	r = repository->Get(KModifiedSettingList[1], settingValue);
	TEST(r == KErrNone);
	TEST(settingValue == KSettingValueList[5]);

	//Get the rom and SWI setting it should have SWI value
	r = repository->Get(KModifiedSettingList[2], settingValue);
	TEST(r == KErrNone);
	TEST(settingValue == KSettingValueList[2]);

	repository->Close();
	CleanupStack::PopAndDestroy(2);//repository, notifier

	// If cache manager is initialized and used before, we flush it
	CleanUp();
}

/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-UT-4032
@SYMTestCaseDesc		This test checks that after a rom flash that contains a corrupt repository.
						The corrupt repository is skipped and the non corrupt repositories are processed as normal.
@SYMTestPriority		High
@SYMTestActions			Copy 3 prepared repositories into persists dir.
						Two user new settings are added[1000,1001].
						One user setting is deleted [1001].
						One rom setting is deleted [0x04010100].
						One rom setting is modified [0x04010200].
						Copy modified rom version file over local persisted version
						Call CheckRomFlash
@SYMTestExpectedResults Verfiy that the corrupt repository 10043337 is skipped and the other repositories are processed.
@SYMCR					CR1198
*/
LOCAL_C void SYSLIB_CENTRALREPOSITORY_UT_4032L()
{
	TheTest.Next( _L( " @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-UT-4032 " ) );
	CFileMan* fm = CFileMan::NewL(TServerResources::iFs);
	CleanupStack::PushL(fm);

	//Initial cleanup
	fm->Delete(KPersistsRepositoryCre1);
	fm->Delete(KPersistsRepositoryCre2);
	fm->Delete(KPersistsRepositoryCre3);
	//Copy of modified stored rom version info

	//Copy 10043336,10043337 and 10043338 prepared repositories into persists directory.
	//These repositories have a two user added,1 user deleted,1 rom deleted and 1 rom modified settings.
	User::LeaveIfError(fm->Copy(KRomRepositoryCre1,KPersistsRepositoryCre1,CFileMan::EOverWrite));
	User::LeaveIfError(fm->Attribs(KPersistsRepositoryCre1,0,KEntryAttReadOnly,TTime(0)));
	User::LeaveIfError(fm->Copy(KRomRepositoryCre2,KPersistsRepositoryCre2,CFileMan::EOverWrite));
	User::LeaveIfError(fm->Attribs(KPersistsRepositoryCre2,0,KEntryAttReadOnly,TTime(0)));
	User::LeaveIfError(fm->Copy(KRomRepositoryCre3,KPersistsRepositoryCre3,CFileMan::EOverWrite));
	User::LeaveIfError(fm->Attribs(KPersistsRepositoryCre3,0,KEntryAttReadOnly,TTime(0)));
	User::After(KGeneralDelay);

	//Copy of modified stored rom version info
	User::LeaveIfError(fm->Copy(KModifiedSwVersion,KCachedVersionFile,CFileMan::EOverWrite));
	User::LeaveIfError(fm->Attribs(KCachedVersionFile,0,KEntryAttReadOnly,TTime(0)));
	User::After(KGeneralDelay);
	CleanupStack::PopAndDestroy();//fm

	CServerRepository::CheckROMReflashL();

	CSessionNotifier* notifier = new(ELeave)CSessionNotifier;
	CleanupStack::PushL(notifier);

    CServerRepository* repository = new(ELeave) CServerRepository();
    CleanupStack::PushL(repository);

    TInt ii = 3;
	while(ii < 6)
		{
    	repository->OpenL(KUidRomRepositoryList[ii],*notifier);
		 //Get the user added setting it should still be there
		TInt settingValue = 0;
		TInt r = repository->Get(KNewSettingList[0], settingValue);
		TEST(r == KErrNone);

		//Get the rom added value
		r = repository->Get(KNewSettingList[3], settingValue);
		if(ii == 4)//Corrupted rom repository is skipped, so it wont have been added
			{
			TEST2(r, KErrNotFound);
			}
		else
			{
			TEST2(r, KErrNone);
			}

		//Get the rom deleted value, it will have been reinstated
		r = repository->Get(KDeletedSettingList[1], settingValue);
		if(ii == 4)//Corrupted rom repository is skipped, so it wont have been added
			{
			TEST2(r, KErrNotFound);
			}
		else
			{
			TEST2(r, KErrNone);
			}

		//Get the user added and deleted value, it shouldnt be there.
		r = repository->Get(KNewSettingList[1], settingValue);
		TEST2(r, KErrNotFound);

		// Close repository
		repository->Close();
		ii++;
		}

	CleanupStack::PopAndDestroy(2);//repository, notifier

	// If cache manager is initialized and used before, we flush it
	CleanUp();
}

/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-UT-4083
@SYMTestCaseDesc		This test verifies that after a rom flash, rom and user modified values are merged correctly
						and all policies/meta are set correctly
@SYMTestPriority		High
@SYMTestActions			The test is used to simulate the rom flashing.It starts with 2 ROM keyspaces, the 2nd rom keyspace
						CADACADA been the "updated" ROM keyspace. The test starts with loading old rom keyspace, modify
						keyspace and then persisted under the "updated" ROM keyspace UID. So next time when we open
						the "updated" ROM keyspace, a remerging will happen. The properites of the keyspace and the
						individual settings are verified wrt the policies and metadata
@SYMTestExpectedResults The merging is correct with policies and meta specified in new ROM correctly applied to the 
						persisted file
@SYMDEF					INC134085
*/

//class to allow direct access to TSecurityPolicy class
class TCompiledSecurityPolicy
{
public:
	TCompiledSecurityPolicy(TSecurityPolicy aPolicy):iPolicy(aPolicy){}
	TBool IsTypeFail()
		{
		return iPolicy.iType==TSecurityPolicy::ETypeFail;
		}
	TBool IsTypePass()
		{
		return iPolicy.iType==TSecurityPolicy::ETypePass;		
		}
public:		
TSecurityPolicy iPolicy;	
};

LOCAL_C void INC134085L()
	{
	TheTest.Next( _L( " @SYMTestCaseID:PDS-CENTRALREPOSITORY-UT-4083 " ) );
	CFileMan* fm = CFileMan::NewL(TServerResources::iFs);
	CleanupStack::PushL(fm);

	//orig_rom Test Uid
	const TUid KTestUid={0xCABACABA};
	//updated_rom Test Uid
	const TUid KUpdatedRomUid={0xCADACADA};

	//Initial cleanup
	_LIT(KPersistsRepositoryCre,"C:\\private\\10202be9\\persists\\cadacada.cre");	
	fm->Delete(KPersistsRepositoryCre);

	//Copy of modified stored rom version info
	User::LeaveIfError(fm->Copy(KModifiedSwVersion,KCachedVersionFile,CFileMan::EOverWrite));
	User::LeaveIfError(fm->Attribs(KCachedVersionFile,0,KEntryAttReadOnly,TTime(0)));
	User::After(KGeneralDelay);


	//-------------------------Modify one of the settings--------------------------------
	CSessionNotifier* notifier = new(ELeave)CSessionNotifier;
	CleanupStack::PushL(notifier);
    CServerRepository* repository = new(ELeave) CServerRepository();
    CleanupStack::PushL(repository);

	//Need to reset the Uid to the new ROM value to ensure when persisting it is persisted
	//under the new ROM Uid value in order to simulate the ROM Reflash later
    repository->OpenL(KTestUid,*notifier);
    repository->iUid=KUpdatedRomUid;
    repository->iRepository->SetUid(KUpdatedRomUid);
	TServerResources::iObserver->RemoveObserver(KTestUid,repository,0);
	TServerResources::iObserver->RemoveSharedRepositoryInfo(KTestUid);
		
   	TServerResources::iObserver->AddObserverL(KUpdatedRomUid, repository);
	TServerResources::iObserver->AddSharedRepositoryInfoL(KUpdatedRomUid); 

    //set a string key
    TBuf8<100> KTestString;
    KTestString.Copy(_L("voicemailboxnumber"));
  	User::LeaveIfError(repository->StartTransaction(EReadWriteTransaction));
	TInt err = repository->TransactionSetL(1,KTestString);
	TEST2(err,KErrNone);
	//a old ROM setting that has been modified by the user remains there even
	//when the new ROM does not contain that setting
	err = repository->TransactionSetL(0x300,5);
	TEST2(err,KErrNone);	
	TUint32 tempKeyInfo;
	User::LeaveIfError(repository->CommitTransaction(tempKeyInfo));

	repository->Close();
	CleanupStack::PopAndDestroy(2);//repository, notifier
	
	//--------------------------Simulate the ROM Reflash---------------------------------
	// flush the cache manager contents.
	CleanUp();
	CServerRepository::CheckROMReflashL();

	
	//--------------------------Validate the merging--------------------------------------
	notifier-new (ELeave)CSessionNotifier;
	CleanupStack::PushL(notifier);
    repository = new(ELeave) CServerRepository();
    CleanupStack::PushL(repository);
    repository->OpenL(KUpdatedRomUid,*notifier);
    
    //validate the keyspace global properties
    RSettingsArray& settings=repository->iRepository->iSimRep->SettingsArray();
    TEST2(settings.Count(),22);
    RArray<TUint32>& deletedArray=repository->iRepository->iSimRep->DeletedSettingsArray();
    TEST2(deletedArray.Count(),0)    ;
    RRangePolicyArray& rangePolicy=repository->iRepository->iSimRep->RangePolicyArray();
    TEST2(rangePolicy.Count(),1);
    RPointerArray<TSettingsAccessPolicy>& individualPolicy=repository->iRepository->iSimRep->SinglePolicyArray();
    TEST2(individualPolicy.Count(),20);    

    RDefaultMetaArray& rangeMeta=repository->iRepository->iSimRep->RangeMetaArray();
    TEST2(rangeMeta.Count(),1);    
    for (TInt i=0;i<settings.Count();i++)
    	{
    	TUint32 key=settings[i].Key();

    	//-------------validate the meta------------    	
    	//validate the meta only the modfiied setting will be not clean
    	if (key!=1 && key!=0x300)
       		TEST(settings[i].IsClean());
    	else
       		TEST(!settings[i].IsClean());
    	TUint32 meta=settings[i].Meta();
    	if (key==0 || key==0x11)
    		TEST(settings[i].Meta()==0x81000000);
    	//settings that have been modified
    	else if (key==1)
    		TEST(settings[i].Meta()==0x01000000);
    	//settings that used the range defined meta
    	else if (key==0x300)
    		TEST(settings[i].Meta()==300);
    	else
    		TEST(settings[i].Meta()==0x80000000);
    	
    	//------------validate the security policies-----
    	TCompiledSecurityPolicy r_tsp(*(settings[i].GetReadAccessPolicy()));
    	TEST(r_tsp.IsTypePass());
    	TCompiledSecurityPolicy w_tsp(*(settings[i].GetWriteAccessPolicy()));
    	if (key==5 || key==9 || key==16 || key==22)
    		TEST(w_tsp.IsTypeFail());    	
    	
    	}
      
    repository->Close();
	CleanupStack::PopAndDestroy(2);//repository, notifier
	CleanupStack::PopAndDestroy();//fm

	// If cache manager is initialized and used before, we flush it
	CleanUp();	
	}

LOCAL_C void FuncTestsL()
	{
	SYSLIB_CENTRALREPOSITORY_UT_4025L();
	SYSLIB_CENTRALREPOSITORY_UT_4026L();
	SYSLIB_CENTRALREPOSITORY_UT_4027L();
	SYSLIB_CENTRALREPOSITORY_UT_4028L();
	SYSLIB_CENTRALREPOSITORY_UT_4029L();
	SYSLIB_CENTRALREPOSITORY_UT_4030L();
	SYSLIB_CENTRALREPOSITORY_UT_4031L();
	SYSLIB_CENTRALREPOSITORY_UT_4032L();
	INC134085L();
	}

LOCAL_C void DoTestsL()
	{

	TheTest.Start( _L( "FOTA functionality" ) );
	FuncTestsL();
	TheTest.End();

	}

LOCAL_C void MainL()
	{
	__UHEAP_MARK;
	TheTest.Start( _L( "Fota Tests" ) );

	// create and install the active scheduler we need
	CActiveScheduler* s = new(ELeave) CActiveScheduler;
	CleanupStack::PushL( s );
	CActiveScheduler::Install( s );

	TServerResources::InitialiseL();

	DeleteFilesL();

	DoTestsL();

	DeleteFilesL();
	TServerResources::Close();

	// Cleanup the scheduler
	CleanupStack::PopAndDestroy( s );

	TheTest.End();
	TheTest.Close();
	__UHEAP_MARKEND;
	}

TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if ( !cleanup )
		return KErrNoMemory;

	TRAPD( err, MainL()  );
	TEST2(err, KErrNone);

	delete cleanup;
	__UHEAP_MARKEND;

	return 0;
	}

