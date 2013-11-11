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

#include "t_cenrep_helper.h"
#include <e32test.h>
#include <f32file.h>
#include "srvrepos_noc.h"
#include "srvres.h"
#include "cachemgr.h"
#include <bautils.h>
#include "setting.h"

//Forward declarations
class CTestObserver;

///////////////////////////////////////////////////////////////////////////////////////
// Globals

static RTest TheTest(_L("T_CenRepTrans"));
const TUid KTestRepositoryId = {0x00000010};
//CentralRepository server UID
static const TUid KCenRepServerUID = {0x10202BE9};

static CTestObserver* TheObserver = NULL;
static CServerRepository* TheServerRepository = NULL;

static const TUint32 KIntSettingID = 843;
static const TInt KIntSettingVal = 25;

static const TUint32 KRealSettingID = 844;
static const TReal KRealSettingVal = 8.76;
static const TReal KRealSettingVal2 = 19.234;

static const TUint32 KStringSettingID = 845;
_LIT8(KStringSettingVal, "a test string");
_LIT8(KStringSettingVal2, "another string");
_LIT8(KEmptyString, "");

_LIT(KTestFile1,	"C:\\PRIVATE\\10202BE9\\PERSISTS\\00000010.TXT");
_LIT(KCreTestFile1,	"C:\\PRIVATE\\10202BE9\\PERSISTS\\00000010.CRE");
_LIT(KTmpTestFile1,	"C:\\PRIVATE\\10202BE9\\PERSISTS\\00000010.TMP");


///////////////////////////////////////////////////////////////////////////////////////
//

//Delete file function.
//Used by the test application to cleanup the C drive at the end of the test.
static void DeleteFile(const TDesC& aFullName)
	{
	RFs fsSession;
	TInt err = fsSession.Connect();
	if(err == KErrNone)
		{
		TEntry entry;
		if(fsSession.Entry(aFullName, entry) == KErrNone)
			{
			RDebug::Print(_L("Deleting \"%S\" file.\n"), &aFullName);
			err = fsSession.SetAtt(aFullName, 0, KEntryAttReadOnly);
			if(err != KErrNone)
				{
				RDebug::Print(_L("Error %d changing \"%S\" file attributes.\n"), err, &aFullName);
				}
			err = fsSession.Delete(aFullName);
			if(err != KErrNone)
				{
				RDebug::Print(_L("Error %d deleting \"%S\" file.\n"), err, &aFullName);
				}
			}
		fsSession.Close();
		}
	else
		{
		RDebug::Print(_L("Error %d connecting file session. File: %S.\n"), err, &aFullName);
		}
	}

static void DeleteFiles()
	{
	::DeleteFile(KTestFile1);
	::DeleteFile(KCreTestFile1);
	::DeleteFile(KTmpTestFile1);
	}

static TInt GetFileTimeStamp(const TDesC& aFullName, TTime& aTime)
	{
	RFs fsSession;
	TInt err = fsSession.Connect();
	if(err == KErrNone)
		{
		TEntry entry;
		if(fsSession.Entry(aFullName, entry) == KErrNone)
			{
			aTime=entry.iModified;
			}
		fsSession.Close();
		}
	else
		{
		RDebug::Print(_L("Error %d connecting file session. File: %S.\n"), err, &aFullName);
		}
	return err;
	}


///////////////////////////////////////////////////////////////////////////////////////
// Test macros and functions.

static void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		::DeleteFiles();
		TheTest(EFalse, aLine);
		}
	}
static  void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		::DeleteFiles();
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

static void CheckTmpFileDeleted()
	{
	RFs fsSession;
	TInt err = fsSession.Connect();
	if(err == KErrNone)
		{
		// Allow time for file to be deleted
		User::After(500*1000);
		TEST2 (BaflUtils::FileExists (fsSession, KTmpTestFile1), EFalse);
		fsSession.Close();
		}
	else
		{
		RDebug::Print(_L("Error %d connecting file session.\n"), err);
		}
	}


///////////////////////////////////////////////////////////////////////////////////////
// CTestObserver class

//It is needed for the creation of CServerRepository object.
//Used also to track Get/Set/Create/Delete setting notifications, received
//from CServerRepository object.
class CTestObserver : public CBase, public MObserver
	{
public:
	CTestObserver();
	void Notify(TUint32 aId);

public:
	TBool iAssertNotification;

	};

CTestObserver::CTestObserver() :
	iAssertNotification(EFalse)
	{
	}

void CTestObserver::Notify(TUint32 aId)
	{
	RDebug::Print(_L("Notification! Id=%d.\n"), aId);
	if(iAssertNotification)
		{
		TEST(0);
		}
	}

///////////////////////////////////////////////////////////////////////////////////////
//

//Deletes the global TheObserver object. Sets it to NULL.
static void ReleaseObserver(TAny*)
	{
	delete TheObserver;
	TheObserver = NULL;
	}

//Deletes the global TheServerRepository object. Sets it to NULL.
static void ReleaseRepository(TAny*)
	{
	TheServerRepository->Close();
	delete TheServerRepository;
	TheServerRepository = NULL;
	TServerResources::iCacheManager->DisableCache(ETrue);
	TServerResources::iObserver->CloseiOpenRepositories();
	TServerResources::iCacheManager->EnableCache();
	}

//This function is called from CreateSetting_OOMtestL().
//It creates an integer, real or string setting in the repository
//with ID=KTestRepositoryId. All this is done in an OOM loop.
//The output from this test is: the repository file with ID = KTestRepositoryId
//will be copied from Z to C drive;
// An integer, real or string setting will be created in that repository.
//Changes in the repository will be stored on C drive (in the related repository file)
static void DoCreateSettingL(TBool aTrapping,TServerSetting::TType aSettingType)
	{
	TheObserver = new (ELeave) CTestObserver;
	TCleanupItem clnItem1(&ReleaseObserver, NULL);
	CleanupStack::PushL(clnItem1);

	TheServerRepository = new (ELeave) CServerRepository;
	TCleanupItem clnItem2(&ReleaseRepository, NULL);
	CleanupStack::PushL(clnItem2);

	// Turn OOM trapping on Repository Open on/off for testing purposes.
	TServerResources::iObserver->iTrapOOMOnOpen=aTrapping;
	// Open the repository
	TheServerRepository->OpenL(KTestRepositoryId, *TheObserver);

	// All write operations, including CreateL must be done in a transaction
	TheServerRepository->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TheServerRepository->CleanupCancelTransactionPushL();

	//Create the appropriate setting based on aValueType
	switch(aSettingType)
		{
		case TServerSetting::EInt:
			{
			// Create setting - fails if value already there (it must not be)
			User::LeaveIfError(TheServerRepository->TransactionCreateL(KIntSettingID, KIntSettingVal, NULL));

			// Try to get the value from the transaction cache
			TInt val = 0;
			User::LeaveIfError(TheServerRepository->Get(KIntSettingID, val));
			TEST2(val,KIntSettingVal);
			break;
			}

		case TServerSetting::EReal:
			{
			// Create setting - fails if value already there (it must not be)
			User::LeaveIfError(TheServerRepository->TransactionCreateL(KRealSettingID, KRealSettingVal, NULL));

			// Try to get the value from the transaction cache
			TReal val = 0;
			User::LeaveIfError(TheServerRepository->Get(KRealSettingID, val));
			TEST(val == KRealSettingVal);
			break;
			}

		case TServerSetting::EString:
			{
			// Create setting - fails if value already there (it must not be)
			User::LeaveIfError(TheServerRepository->TransactionCreateL(KStringSettingID, KStringSettingVal, NULL));

			// Try to get the value from the transaction cache
			TBuf8<16> bufVal(KEmptyString);
			User::LeaveIfError(TheServerRepository->Get(KStringSettingID, bufVal));
			TEST(bufVal == KStringSettingVal);
			break;
			}
		default:
			break;
		}

	// Commit the changes
	CleanupStack::Pop();
	TUint32 keyInfo;
	User::LeaveIfError(TheServerRepository->CommitTransaction(keyInfo));

	TServerResources::iObserver->iTrapOOMOnOpen=EFalse;
	// Close the repository
	TheServerRepository->Close();

	CleanupStack::PopAndDestroy(2);//TheObserver & TheServerRepository
	}

//This function is called from SetSetting_OOMtest().
//It expects that the previous test (CreateSetting_OOMtestL()) had completed successfully
//and there is a repository file (ID=0x10) on drive C and there is an integer setting
//in that repository (ID=KIntSettingID, Value=KIntSettingVal).
//The test will try to change the setting value in a transaction. Because the test is
//executed in an OOM loop, CommitChangesL() - will fail when called by SetL, leaving the
//repository content inconsistent - the in-memory repository content will be deleted
//(no settings), but in  the file the setting will exist and its value will still be KIntSettingVal.
static void DoSetIntSettingL()
	{
	TEST(TheObserver != NULL);
	TEST(TheServerRepository != NULL);
	// Check if the setting is there - and has (or is restored to) the original value
	TInt val = 0;
	User::LeaveIfError(TheServerRepository->Get(KIntSettingID, val));
	TEST2(val,KIntSettingVal);

	// Change the setting value
	// All write operations, including CreateL must be done in a transaction
	TheServerRepository->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TheServerRepository->CleanupCancelTransactionPushL();
	// Change the setting value
	User::LeaveIfError(TheServerRepository->TransactionSetL(KIntSettingID, KIntSettingVal + 1));

	// Check if the setting is there - it should be in transaction cache now
	User::LeaveIfError(TheServerRepository->Get(KIntSettingID, val));
	TEST2(val,(KIntSettingVal + 1));

	// Commit the changes
	CleanupStack::Pop();
	TUint32 keyInfo;
	User::LeaveIfError(TheServerRepository->CommitTransaction(keyInfo));

	// Check if the setting is there. The transaction was successful so the new value
	// should be returned.
	val = 0;
	User::LeaveIfError(TheServerRepository->Get(KIntSettingID, val));
	TEST2(val,(KIntSettingVal + 1));
	}

//This function is called from SetSetting_OOMtest().
//It expects that the previous test (CreateSetting_OOMtestL()) had completed successfully
//and there is a repository file (ID=0x10) on drive C and there is an rea; setting
//in that repository (ID=KRealSettingID, Value=KRealSettingVal).
//The test will try to change the setting value in a transaction. Because the test is
//executed in an OOM loop, CommitChangesL() - will fail when called by SetL, leaving the
//repository content inconsistent - the in-memory repository content will be deleted
//(no settings), but in  the file the setting will exist and its value will still be KRealSettingVal.
static void DoSetRealSettingL()
	{
	TEST(TheObserver != NULL);
	TEST(TheServerRepository != NULL);
	// Check if the setting is there - and has (or is restored to) the original value
	TReal val = 0;
	User::LeaveIfError(TheServerRepository->Get(KRealSettingID, val));
	TEST(val == KRealSettingVal);

	// Change the setting value
	// All write operations, including CreateL must be done in a transaction
	TheServerRepository->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TheServerRepository->CleanupCancelTransactionPushL();
	// Change the setting value
	User::LeaveIfError(TheServerRepository->TransactionSetL(KRealSettingID, KRealSettingVal2));

	// Check if the setting is there - it should be in transaction cache now
	User::LeaveIfError(TheServerRepository->Get(KRealSettingID, val));
	TEST(val == KRealSettingVal2);

	// Commit the changes
	CleanupStack::Pop();
	TUint32 keyInfo;
	User::LeaveIfError(TheServerRepository->CommitTransaction(keyInfo));

	// Check if the setting is there. The transaction was successful so the new value
	// should be returned.
	val = 0;
	User::LeaveIfError(TheServerRepository->Get(KRealSettingID, val));
	TEST(val == KRealSettingVal2);
	}

//This function is called from SetSetting_OOMtest().
//It expects that the previous test (CreateSetting_OOMtestL()) had completed successfully
//and there is a repository file (ID=0x10) on drive C and there is an string setting
//in that repository (ID=KStringSettingID, Value=KStringSettingVal).
//The test will try to change the setting value in a transaction. Because the test is
//executed in an OOM loop, CommitChangesL() - will fail when called by SetL, leaving the
//repository content inconsistent - the in-memory repository content will be deleted
//(no settings), but in  the file the setting will exist and its value will still
//be KStringSettingVal.
static void DoSetStringSettingL()
	{
	TEST(TheObserver != NULL);
	TEST(TheServerRepository != NULL);

	// Check if the setting is there - and has (or is restored to) the original value
	TBuf8<16> bufVal(KEmptyString);
	User::LeaveIfError(TheServerRepository->Get(KStringSettingID, bufVal));
	TEST(bufVal == KStringSettingVal);

	// Change the setting value
	// All write operations, including CreateL must be done in a transaction
	TheServerRepository->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TheServerRepository->CleanupCancelTransactionPushL();

	// Change the setting value
	User::LeaveIfError(TheServerRepository->TransactionSetL(KStringSettingID, KStringSettingVal2));

	// Check if the setting is there - it should be in transaction cache now
	User::LeaveIfError(TheServerRepository->Get(KStringSettingID, bufVal));
	TEST(bufVal == KStringSettingVal2);
	// Commit the changes
	CleanupStack::Pop();
	TUint32 keyInfo;
	User::LeaveIfError(TheServerRepository->CommitTransaction(keyInfo));

	// Check if the setting is there. The transaction was successful so the new value
	// should be returned.
	bufVal = KEmptyString;
	User::LeaveIfError(TheServerRepository->Get(KStringSettingID, bufVal));
	TEST(bufVal == KStringSettingVal2);
	}

//This function is called from SetSetting_OOMtest()in the case where previous
//DoSetSettingL() call failed with KErrNoMemory, leaving the repository object
//in inconsisten state - the in-memory presentation of the repository differs from
//the repository file content. The function will try to repair the repository,
//as it would have to happen in a real situation. Then it will check that
//the repository content is consistent, which means the tested setting value should
//be the same as it was before DoSetSettingL() call.
static void DoRecoverRepositoryL(TServerSetting::TType aSettingType)
	{
	//Repair the repository as it happens in a real situation
	//AccessL calls RestoreConsistencyL() indirectly
	//RestoreConsistencyL() is called in the production code from the session object,
	//ServiceL() implementation. So, it will be called before any other repository call
	//and it should restore the repository content - it should repair the repository
	//consitency.
	TServerResources::iObserver->AccessL(KTestRepositoryId);
	//Check if the setting is there - the old, pre-transactional value should be
	//in-memory now.
	switch(aSettingType)
		{
		case TServerSetting::EInt:
			{
			TInt val = 0;
			User::LeaveIfError(TheServerRepository->Get(KIntSettingID, val));
			TEST2(val,KIntSettingVal);
			break;
			}

		case TServerSetting::EReal:
			{
			TReal val = 0;
			User::LeaveIfError(TheServerRepository->Get(KRealSettingID, val));
			TEST(val == KRealSettingVal);
			break;
			}

		case TServerSetting::EString:
			{
			TBuf8<16> bufVal(KEmptyString);
			User::LeaveIfError(TheServerRepository->Get(KStringSettingID, bufVal));
			TEST(bufVal == KStringSettingVal);
			break;
			}
		default:
			break;
		}
	}

//Inits TServerResources object.
//It should be called once, before the creation of CServerRepository object.
static void InitEnvL()
	{
	TServerResources::InitialiseL();

	delete TServerResources::iDataDirectory; TServerResources::iDataDirectory = NULL;
	delete TServerResources::iRomDirectory; TServerResources::iRomDirectory = NULL;
	delete TServerResources::iInstallDirectory; TServerResources::iInstallDirectory = NULL;

	TServerResources::iDataDirectory = HBufC::NewL(KMaxFileName);
	TServerResources::iRomDirectory = HBufC::NewL(KMaxFileName);
	TServerResources::iInstallDirectory = HBufC::NewL(KMaxFileName);

	TBuf<32> uidName;
	uidName.Format(_L("%08X"), KCenRepServerUID.iUid);

	TPtr ptr1(TServerResources::iDataDirectory->Des());
	ptr1.Append(_L("C:\\PRIVATE\\"));
	ptr1.Append(uidName);
	ptr1.Append(_L("\\PERSISTS\\"));

	TPtr ptr2(TServerResources::iRomDirectory->Des());
	ptr2.Append(_L("Z:\\PRIVATE\\"));
	ptr2.Append(uidName);
	ptr2.Append(_L("\\"));

	TPtr ptr3(TServerResources::iInstallDirectory->Des());
	ptr3.Append(_L("C:\\PRIVATE\\"));
	ptr3.Append(uidName);
	ptr3.Append(_L("\\"));
	}

//Destroys TServerResources object.
//It should be called after the CServerRepository object was destroyed.
static void ReleaseEnv()
	{
	TServerResources::Close();
	}

//Creates global TheServerRepository and TheObserver objects
//It is used in SetSetting_OOMtest()
static void InitEnv2L()
	{
	TheObserver = new CTestObserver;
	TEST(TheObserver != NULL);

	TheServerRepository = new CServerRepository;
	TEST(TheServerRepository != NULL);

	TheServerRepository->OpenL(KTestRepositoryId, *TheObserver);
	}

//Destroys global TheServerRepository and TheObserver objects
//It is used in SetSetting_OOMtest()
static void ReleaseEnv2()
	{
	::ReleaseRepository(NULL);
	::ReleaseObserver(NULL);
	}

//Central repository test: it creates a setting in OOM conditions.
static void CreateSetting_OOMtestL(TBool aTrapping,TServerSetting::TType aSettingType)
	{
	TInt err;
	TRAP(err, ::InitEnvL());
	TEST2(err, KErrNone);
	for(TInt count=1;;++count)
		{
		CleanupCDriveL();
		__UHEAP_FAILNEXT(count);
		__UHEAP_MARK;

		TRAP(err, ::DoCreateSettingL(aTrapping,aSettingType));

		TServerResources::iOwnerIdLookUpTable.Reset();
		TServerResources::iObserver->Reset();

		__UHEAP_MARKEND;

		if(err == KErrNone)
			{
			RDebug::Print(_L("The test succeeded at heap failure rate=%d.\n"), count);
			break;
			}
		else if(err != KErrNoMemory)
			{
			TEST2(err, KErrNone);
			}
		}

	::ReleaseEnv();
	__UHEAP_RESET;
	}

//Central repository test: it sets new setting value in OOM conditions.
//Then ComitChangesL() is called to store the new setting's value in the repository
//file. Because of the simulated OOM conditions ComitChangesL() can fail, leaving the
//repository in inconsistent state.
//DoRecoverRepositoryL() is called to repair the repository and restore the consistency.
static void SetSetting_OOMtest(TServerSetting::TType aSettingType)
	{
	TTime before;
	TTime after;
	// Check that cre file exists and get modification time
	TEST2 (GetFileTimeStamp(KCreTestFile1, before), KErrNone);
	after=before;

	TInt err;
	TRAP(err, ::InitEnvL());
	TEST2(err, KErrNone);
	TRAP(err, ::InitEnv2L());
	TEST2(err, KErrNone);	
	for(TInt count=1;;++count)
		{
		__UHEAP_FAILNEXT(count);
		__UHEAP_MARK;

	switch(aSettingType)
		{
		case TServerSetting::EInt:
			{
			TRAP(err, ::DoSetIntSettingL());
			break;
			}
		case TServerSetting::EReal:
			{
			TRAP(err, ::DoSetRealSettingL());
			break;
			}
		case TServerSetting::EString:
			{
			TRAP(err, ::DoSetStringSettingL());
			break;
			}
		default:
			break;
		}

		// Get timestamp of cre file. It is now possible for the commit to
		// fail with KErrNoMemory after a successful write. If filestamp
		// changes, the cre file has been written and the test is complete
		TEST2 (GetFileTimeStamp(KCreTestFile1, after), KErrNone);
		if(before!=after)
			{
			err=KErrNone;
			}

		if(err == KErrNoMemory)
			{
			__UHEAP_MARKEND;
			//DoSetSettingL() call failed (so the transaction). Try to recover.
			TInt err2;
			TRAP(err2, ::DoRecoverRepositoryL(aSettingType));
			TEST2(err2, KErrNone);
			}
		else if(err == KErrNone)
			{
			//The heap cannot be checked at this point because memory may have been
			//allocated which must be freed by a call to ReleaseEnv2.
			//It is checked outside the loop instead
			RDebug::Print(_L("The test succeeded at heap failure rate=%d.\n"), count);
			break;
			}
		else
			{
			__UHEAP_MARKEND;
			TEST2(err, KErrNone);
			}
		// Check that tmp file generated by attempted commit is deleted
		// by RestoreConsistencyL() in DoRecoverRepositoryL().
		::CheckTmpFileDeleted();
		}
	// Check that no tmp file is left over
	::ReleaseEnv2();
	::ReleaseEnv();
	//This __UHEAP_MARKEND checks the heap after the call to DoSetxSettings
	//succeeds as calls to CommitTRansactions can allocate memory which must be freed
	//by calling ReleaseEnv2
	__UHEAP_MARKEND;
	__UHEAP_RESET;
	}

///////////////////////////////////////////////////////////////////////////////////////

void RunTestsL(TServerSetting::TType aSettingType)
	{

	TheTest.Next(_L("Create setting - OOM test - OOM Trapping off"));
	::CreateSetting_OOMtestL(EFalse,aSettingType);

	TheTest.Next(_L("Create setting - OOM test - OOM Trapping on"));
	::CreateSetting_OOMtestL(ETrue,aSettingType);

	TheTest.Next(_L("Set setting - transaction - OOM test"));
	::SetSetting_OOMtest(aSettingType);

	}

static void MainL()
	{
	// create and install the active scheduler we need for the cache manager in TServerResources::InitialiseL
	CActiveScheduler* cacheManagerAS=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(cacheManagerAS);
	CActiveScheduler::Install(cacheManagerAS);

	::DeleteFiles();

	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-LEGACY-T_CENREPTRANS-0001 Int Setting Tests "));
	RunTestsL(TServerSetting::EInt);

	TheTest.Next(_L("Real Setting Tests"));
	RunTestsL(TServerSetting::EReal);

	TheTest.Next(_L("String Setting Tests"));
	RunTestsL(TServerSetting::EString);

	TheTest.End();
	TheTest.Close();

	CleanupStack::PopAndDestroy(cacheManagerAS);
	}

TInt E32Main()
	{
	__UHEAP_MARK;

	CTrapCleanup* tc = CTrapCleanup::New();
	TEST(tc != NULL);

	TheTest.Title();
	TRAPD(err, ::MainL());
	TEST2(err, KErrNone);

	::DeleteFiles();

	delete tc;

	__UHEAP_MARKEND;

	User::Heap().Check();
	return KErrNone;
	}
