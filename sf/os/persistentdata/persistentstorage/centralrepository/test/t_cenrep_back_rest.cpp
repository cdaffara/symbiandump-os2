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

#include <e32test.h>
#include <f32file.h>
#include <badesca.h>
#include "srvrepos_noc.h"
#include "srvres.h"
#include "cachemgr.h"
#include "backup.h"

RTest TheTest(_L("Central Repository Backup & Restore Tests"));

enum FileSet
	{
	EOriginals,
	EChanged,
	ERemoved,
	EDEF058823L
	};

_LIT_SECURE_ID( SecureId1, 0x12345 );

TBool oomRunning;
HBufC* iniRomDir=NULL;

CRepositoryBackupClient* backupClient;

CDesC8ArraySeg* originalData1;
CDesC8ArraySeg* changedData1;
CDesC8ArraySeg* originalData2;
CDesC8ArraySeg* changedData2;
CDesC8ArraySeg* changedData3;

CActiveScheduler* globalAS;

const TUid KUidBackupTestRepository 	= { 0xbabababa };
const TUid KUidBackupTestRepository1 	= { 0x00000100 };

const TUint32 KBackupInt1 = 0x02010100;
const TInt KIntOrigValue1 = 10;
const TUint32 KBackupInt2 = 0x03010200;
const TInt KIntChangedValue2 = 200;
const TUint32 KBackupInt3 = 0x04010300;
const TInt KIntOrigValue3 = 30;

const TUint32 KBackupString1 = 0x02010600;
_LIT8(KStringOrigValue, "test original");
_LIT8(KStringChangedValue, "test changed");
const TUint32 KBackupString2 = 0x03010600;
const TUint32 KBackupReal1 = 0x02010700;
const TReal KRealOrigValue = 1.1;
const TUint32 KBackupReal2 = 0x03010700;
const TReal KRealChangedValue = 2.5;

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macroses and functions


LOCAL_C void NextTest(TPtrC aMsg)
	{
	if (!oomRunning)
		{
		TheTest.Next( aMsg );
		}
	}

LOCAL_C void DeleteFilesL()
	{
	_LIT( KOldInstallFiles, "c:\\private\\102081E4\\*.*" );
	_LIT( KOldPersistFiles, "c:\\private\\102081E4\\persists\\*.*" );

	CFileMan* fm = CFileMan::NewL( TServerResources::iFs );
	CleanupStack::PushL( fm );
	
	TInt r = KErrNone;

	r = fm->Delete( KOldInstallFiles );
#ifndef __WINSCW__
	if (r == KErrAccessDenied)
		{
		// we are not able to delete the files on ARM if their attributes are not set correctly.
		// Only set attributes when got KErrAccessDenied because it's time consuming in OOM test.
		r = fm->Attribs( KOldInstallFiles, KEntryAttArchive, KEntryAttReadOnly, TTime( 0 ), CFileMan::ERecurse );
		User::LeaveIfError(r);
		r = fm->Delete( KOldInstallFiles );
		}
#endif
	if ( r != KErrNone && r != KErrNotFound && r != KErrPathNotFound )
		User::Leave(r);
	
	r = fm->Delete( KOldPersistFiles );
#ifndef __WINSCW__
	if (r == KErrAccessDenied)
		{
		// we are not able to delete the files on ARM if their attributes are not set correctly.
		// Only set attributes when got KErrAccessDenied because it's time consuming in OOM test.
		r = fm->Attribs( KOldPersistFiles, KEntryAttArchive, KEntryAttReadOnly, TTime( 0 ), CFileMan::ERecurse );
		User::LeaveIfError(r);
		r = fm->Delete( KOldPersistFiles );
		}
#endif
	if ( r != KErrNone && r != KErrNotFound && r != KErrPathNotFound )
		User::Leave(r);

	CleanupStack::PopAndDestroy( fm );
	}

LOCAL_C void CheckL( TInt aValue, TInt aLine )
	{
	if ( !aValue )
		{
		DeleteFilesL();
		TheTest( EFalse, aLine );
		}
	}

LOCAL_C void CheckL( TInt aValue, TInt aExpected, TInt aLine )
	{
	if ( aValue != aExpected )
		{
		DeleteFilesL();
		RDebug::Print( _L( "*** Expected error: %d, got: %d\r\n"), aExpected, aValue );
		TheTest( EFalse, aLine );
		}
	}

#define TEST(arg) ::CheckL((arg), __LINE__)
#define TEST2(aValue, aExpected) ::CheckL(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

LOCAL_C void InitializationL()
    {
    originalData1 = new(ELeave)CDesC8ArraySeg( 128 );
	CleanupStack::PushL( originalData1 );
	changedData1 = new(ELeave)CDesC8ArraySeg( 128 );
	CleanupStack::PushL( changedData1 );
	originalData2 = new(ELeave)CDesC8ArraySeg( 128 );
	CleanupStack::PushL( originalData2 );
	changedData2 = new(ELeave)CDesC8ArraySeg( 128 );
	CleanupStack::PushL( changedData2 );
	changedData3 = new(ELeave)CDesC8ArraySeg( 128 );
	CleanupStack::PushL( changedData3 );
    }

LOCAL_C void Cleanup()
    {
    CleanupStack::PopAndDestroy( changedData3 );
	CleanupStack::PopAndDestroy( changedData2 );
	CleanupStack::PopAndDestroy( originalData2 );
	CleanupStack::PopAndDestroy( changedData1 );
	CleanupStack::PopAndDestroy( originalData1 );
    }

LOCAL_C void InstallFileSetL( FileSet aFileSet )
	{
	_LIT(KSourceFiles_V1, "z:\\private\\10202BE9\\*.cra");
	_LIT(KSourceFiles_V2, "z:\\private\\10202BE9\\*.crb");
	_LIT(KDestForInstallFiles, "c:\\private\\102081E4\\*.cre");
	_LIT(KDestForPersistFiles, "c:\\private\\102081E4\\persists\\*.cre");
	_LIT(KDEF058823DestFile, "c:\\private\\102081E4\\persists\\*.*");
	_LIT(KDEF058823SrcFile, "z:\\private\\10202BE9\\babababa.cre");
	
	DeleteFilesL();
	// When the contents of the repository directories change, the cached iOwnerIdLookUpTable becomes invalid
	TServerResources::iOwnerIdLookUpTable.Reset();

	CFileMan* fm = CFileMan::NewL( TServerResources::iFs );
	CleanupStack::PushL( fm );
	
	TInt r;
		
	switch ( aFileSet )
		{
		case EOriginals:
			{		
			r = TServerResources::iFs.MkDirAll( KDestForPersistFiles );
			if ( r != KErrNone && r != KErrAlreadyExists )
				User::Leave( r );
			
			User::LeaveIfError( fm->Copy( KSourceFiles_V1, KDestForInstallFiles ) );
			User::LeaveIfError( fm->Copy( KSourceFiles_V1, KDestForPersistFiles ) );
			
			r = fm->Attribs( KDestForInstallFiles, KEntryAttArchive, KEntryAttReadOnly, TTime( 0 ), CFileMan::ERecurse );
			if(r == KErrNoMemory)
				User::Leave(r);
			TEST2( r, KErrNone );
			
			break;
			}
		case EChanged:
			{
			r = TServerResources::iFs.MkDirAll( KDestForPersistFiles );
			if ( r != KErrNone && r != KErrAlreadyExists )
				User::Leave( r );
			
			User::LeaveIfError( fm->Copy( KSourceFiles_V1, KDestForInstallFiles ) );		
			User::LeaveIfError( fm->Copy( KSourceFiles_V2, KDestForPersistFiles ) );
			
			r = fm->Attribs( KDestForInstallFiles, KEntryAttArchive, KEntryAttReadOnly, TTime( 0 ), CFileMan::ERecurse );
			if(r == KErrNoMemory)
				User::Leave(r);
			TEST2( r, KErrNone );
			
			break;
			}
		case ERemoved:
			{
			r = TServerResources::iFs.MkDirAll( KDestForInstallFiles );
			if ( r != KErrNone && r != KErrAlreadyExists )
				User::Leave( r );
			
			User::LeaveIfError( fm->Copy( KSourceFiles_V1, KDestForInstallFiles ) );
			
			r = fm->Attribs( KDestForInstallFiles, KEntryAttArchive, KEntryAttReadOnly, TTime( 0 ), CFileMan::ERecurse );
			if(r == KErrNoMemory)
				User::Leave(r);
			TEST2( r, KErrNone );
			
			break;
			}
		case EDEF058823L:
		    {
			r = TServerResources::iFs.MkDirAll( KDEF058823DestFile );
			if ( r != KErrNone && r != KErrAlreadyExists )
				User::Leave( r );

			User::LeaveIfError( fm->Copy( KDEF058823SrcFile, KDEF058823DestFile ) );
			
			r = fm->Attribs( KDEF058823DestFile, KEntryAttArchive, KEntryAttReadOnly, TTime( 0 ), CFileMan::ERecurse );
			if(r == KErrNoMemory)
				User::Leave(r);
			TEST2( r, KErrNone );
		    
			break;
		    }
		default:
			RDebug::Print( _L( "Illegal parameter to function: %d\r\n" ), aFileSet );
			TheTest( EFalse, __LINE__ );
		}

	CleanupStack::PopAndDestroy( fm );
	}

LOCAL_C void CleanupActiveScheduler(TAny* aShc)
	{
	CActiveScheduler::Replace( globalAS );
	delete aShc;
	}

LOCAL_C TBool StreamsAreIdenticalL( CDesC8ArraySeg* aDataStream1,  CDesC8ArraySeg* aDataStream2)
	{
	TInt count1 = 0;
	TInt count2 = 0;
	TInt element = 0;
	TInt length = 0;
	TInt total1 = 0;
	TInt total2 = 0;
	TBool retVal = EFalse;

	count1 = aDataStream1->Count();
	for ( element = 0; element < count1; element++ )
		{
		length = (*aDataStream1)[ element ].Length();
		total1 += length;
		}
	count2 = aDataStream2->Count();
	for ( element = 0; element < count2; element++ )
		{
		length = (*aDataStream2)[ element ].Length();
		total2 += length;
		}
	if ( total1 != total2 )
		return retVal;

	// Copying each array into a single descriptor before comparison is deliberate
	// to make it easier to inspect the contents in a debugger
	HBufC8* buffer1 = HBufC8::NewLC( total1 );
	TPtr8 tPtr1( buffer1->Des() );
	HBufC8* buffer2 = HBufC8::NewLC( total2 );
	TPtr8 tPtr2( buffer2->Des() );

	for ( element = 0; element < count1; element++ )
		{
		tPtr1.Append( (*aDataStream1)[ element ] );
		}
	for ( element = 0; element < count2; element++ )
		{
		tPtr2.Append( (*aDataStream2)[ element ] );
		}

	if ( tPtr1.Compare( tPtr2 ) == 0 )
		retVal = ETrue;
	CleanupStack::PopAndDestroy( buffer2 );
	CleanupStack::PopAndDestroy( buffer1 );
	return retVal;
	}


LOCAL_C void BackupRepositoryL( CDesC8ArraySeg* aDataStream )
	{
	backupClient->CompleteOwnerIdLookupTableL();
	backupClient->InitialiseGetProxyBackupDataL( SecureId1, EDriveC );

	HBufC8* transferBuffer = HBufC8::NewL( 128 );
	CleanupStack::PushL( transferBuffer );
	TPtr8 bufferToSend = transferBuffer->Des();
	TBool finished = EFalse;

	aDataStream->Reset();

	do
		{
		bufferToSend.Zero();
		backupClient->GetBackupDataSectionL( bufferToSend, finished );
		aDataStream->AppendL( bufferToSend );
		} while ( !finished );

	CleanupStack::PopAndDestroy( transferBuffer );
	}

LOCAL_C void RestoreRepositoryL( CDesC8ArraySeg* aDataStream )
	{
	backupClient->InitialiseRestoreProxyBaseDataL( SecureId1, EDriveC );

	HBufC8* transferBuffer = HBufC8::NewL( 128 );
	CleanupStack::PushL( transferBuffer );

	TBool finished = EFalse;
	TInt count = aDataStream->Count();
	TInt element = 0;

	for ( element = 0; element < count; element++ )
		{
		TPtrC8 ptr = (*aDataStream)[ element ];
		if (ptr.Size()>transferBuffer->Des().MaxSize())
			{
			transferBuffer = transferBuffer->ReAlloc(ptr.Size());
			}
		TPtr8 bufferToSend = transferBuffer->Des();
		bufferToSend.Zero();
		bufferToSend.Append( ptr );
		if ( element + 1 == count )
			finished = ETrue;
		backupClient->RestoreBaseDataSectionL( bufferToSend, finished );
		}
	backupClient->RestoreComplete( EDriveC );

	CleanupStack::PopAndDestroy( transferBuffer );
	}


/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CT-0115
@SYMTestCaseDesc Test Backup functionality.
@SYMTestPriority High
@SYMTestActions  Test Backup functionality.
@SYMTestExpectedResults The test must not fail.
@SYMPREQ PREQ752
         PREQ908
*/
LOCAL_C void BackupTestsL()
	{
	NextTest( _L( " @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-0115 Backup repository " ) );

	// create and install the active scheduler we need
	CActiveScheduler* s = new(ELeave) CActiveScheduler;
	
	TCleanupItem tc(CleanupActiveScheduler, s);
	CleanupStack::PushL(tc);
	
	CActiveScheduler::Replace( s );

	backupClient = CRepositoryBackupClient::NewLC( TServerResources::iFs );
	TEST( backupClient != 0 );

	// These tests don't test Backup&Restore functionality over Secure Backup Server so cache management
	// is not possible. For that reason, cache is disabled manually.
	TServerResources::iCacheManager->DisableCache();

	NextTest( _L( "Backup original version" ) );
	// Install known files
	InstallFileSetL( EOriginals );
	BackupRepositoryL( originalData1 );

	NextTest( _L( "Backup changed version" ) );
	InstallFileSetL( EChanged );
	BackupRepositoryL( changedData1 );

	DeleteFilesL();
	NextTest( _L( "Confirm streams different" ) );
	TEST( !StreamsAreIdenticalL( originalData1, changedData1 ) );

	CleanupStack::PopAndDestroy( backupClient );
	// Cleanup the scheduler
	CleanupStack::PopAndDestroy( s );
	}


/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CT-0116
@SYMTestCaseDesc Test Restore functionality.
@SYMTestPriority High
@SYMTestActions  Test Restore functionality.
@SYMTestExpectedResults The test must not fail.
@SYMPREQ PREQ752
         PREQ908
*/

/**
@SYMTestCaseID          SYSLIB-CENTRALREPOSITORY-CIT-1376
@SYMTestCaseDesc        Test for defect
@SYMTestPriority        High
@SYMTestActions         Test that default meta doesn't override individual meta
@SYMTestExpectedResults Test must not fail
@SYMDEF                 INC067080
*/
LOCAL_C void RestoreTestsL()
	{
	NextTest( _L( " @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-0116 " ) );
	NextTest( _L( " @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CIT-1376 Restore repository " ) );

	// create and install the active scheduler we need
	CActiveScheduler* s = new(ELeave) CActiveScheduler;
	
	TCleanupItem tc(CleanupActiveScheduler, s);
	CleanupStack::PushL(tc);
	
	CActiveScheduler::Replace( s );
	
	backupClient = CRepositoryBackupClient::NewLC(TServerResources::iFs);
	TEST( backupClient != 0 );

	CServerRepository* aServerRepo=new (ELeave)CServerRepository();
	CleanupStack::PushL(aServerRepo);
	CSessionNotifier* aSessionNotif=new (ELeave) CSessionNotifier();
	CleanupStack::PushL(aSessionNotif);

	TServerResources::iCacheManager->DisableCache();

	NextTest( _L( "Restore original version over changed version" ) );
	InstallFileSetL( EChanged );
	RestoreRepositoryL( originalData1 );

	NextTest( _L( "Confirm settings restored" ) );

	// Open merged repository to check for values of individual settings
	aServerRepo->OpenL(KUidBackupTestRepository1,*aSessionNotif);

	TInt r, a;
	r = aServerRepo->Get(KBackupInt1, a);
	TEST2(r, KErrNone);
	// This setting is backed up, so it must be restored to original
	TEST(a==KIntOrigValue1);

	r = aServerRepo->Get(KBackupInt2, a);
	TEST2(r, KErrNone);
	// This setting isn't backed up, so it must still have changed value
	TEST(a==KIntChangedValue2);

	r = aServerRepo->Get(KBackupInt3, a);
	TEST2(r, KErrNone);
	// This setting is backed up, so it must be restored to original
	TEST(a==KIntOrigValue3);

	TBuf8<KMaxBinaryLength> str;

	r = aServerRepo->Get(KBackupString1, str);
	TEST(r==KErrNone);
	// This setting is backed up, so it must be restored to original
	TEST(str==KStringOrigValue);

	r = aServerRepo->Get(KBackupString2, str);
	TEST(r==KErrNone);
	// This setting isn't backed up, so it must still have changed value
	TEST(str==KStringChangedValue);

	TReal realval;

	r = aServerRepo->Get(KBackupReal1, realval);
	TEST(r==KErrNone);
	// This setting is backed up, so it must be restored to original
	TEST(realval==KRealOrigValue);

	r = aServerRepo->Get(KBackupReal2, realval);
	TEST(r==KErrNone);
	// This setting isn't backed up, so it must still have changed value
	TEST(realval==KRealChangedValue);

	aServerRepo->Close();

	NextTest( _L( "Restore changed version over original version" ) );
	InstallFileSetL( EOriginals );
	RestoreRepositoryL( changedData1 );

	NextTest( _L( "Confirm settings restored 2" ) );

	// Open merged repository to check for values of individual settings
	aServerRepo->OpenL(KUidBackupTestRepository1,*aSessionNotif);

	r = aServerRepo->Get(KBackupInt1, a);
	TEST2(r, KErrNone);
	// This setting is not backed up, so it must still have original value
	TEST(a==KIntOrigValue1);

	r = aServerRepo->Get(KBackupInt2, a);
	TEST2(r, KErrNone);
	// This setting is backed up, so it must be restored to changed
	TEST(a==KIntChangedValue2);

	r = aServerRepo->Get(KBackupInt3, a);
	TEST2(r, KErrNone);
	// This setting is not backed up, so it must still have original value
	TEST(a==KIntOrigValue3);

	r = aServerRepo->Get(KBackupString1, str);
	TEST(r==KErrNone);
	// This setting is not backed up, so it must still have original value
	TEST(str==KStringOrigValue);

	r = aServerRepo->Get(KBackupString2, str);
	TEST(r==KErrNone);
	// This setting is backed up, so it must be restored to changed
	TEST(str==KStringChangedValue);

	r = aServerRepo->Get(KBackupReal1, realval);
	TEST(r==KErrNone);
	// This setting is not backed up, so it must still have original value
	TEST(realval==KRealOrigValue);

	r = aServerRepo->Get(KBackupReal2, realval);
	TEST(r==KErrNone);
	// This setting is backed up, so it must be restored to changed
	TEST(realval==KRealChangedValue);

	aServerRepo->Close();

	NextTest( _L( "Restore changed version of deleted repository" ) );
	InstallFileSetL( ERemoved );
	RestoreRepositoryL( changedData1 );

	NextTest( _L( "Confirm settings restored 3" ) );

	// Open merged repository to check for values of individual settings
	aServerRepo->OpenL(KUidBackupTestRepository1,*aSessionNotif);

	// This one should be the same as first one, only this time the values
	// to be merged had been read from the file in install directory instead
	// of the file in persists.

	r = aServerRepo->Get(KBackupInt1, a);
	TEST2(r, KErrNone);
	// This setting is backed up, so it must be restored to original
	TEST(a==KIntOrigValue1);

	r = aServerRepo->Get(KBackupInt2, a);
	TEST2(r, KErrNone);
	// This setting isn't backed up, so it must still have changed value
	TEST(a==KIntChangedValue2);

	r = aServerRepo->Get(KBackupInt3, a);
	TEST2(r, KErrNone);
	// This setting is backed up, so it must be restored to original
	TEST(a==KIntOrigValue3);

	r = aServerRepo->Get(KBackupString1, str);
	TEST(r==KErrNone);
	// This setting is backed up, so it must be restored to original
	TEST(str==KStringOrigValue);

	r = aServerRepo->Get(KBackupString2, str);
	TEST(r==KErrNone);
	// This setting isn't backed up, so it must still have changed value
	TEST(str==KStringChangedValue);

	r = aServerRepo->Get(KBackupReal1, realval);
	TEST(r==KErrNone);
	// This setting is backed up, so it must be restored to original
	TEST(realval==KRealOrigValue);

	r = aServerRepo->Get(KBackupReal2, realval);
	TEST(r==KErrNone);
	// This setting isn't backed up, so it must still have changed value
	TEST(realval==KRealChangedValue);

	aServerRepo->Close();

	DeleteFilesL();

	CleanupStack::PopAndDestroy(aSessionNotif);
	CleanupStack::PopAndDestroy(aServerRepo);

	CleanupStack::PopAndDestroy( backupClient );
	// Cleanup the scheduler
	CleanupStack::PopAndDestroy( s );
	}


// ---------------------------------------------------
// OomTest
//

/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-UT-4079
@SYMTestCaseDesc		Make the OOM test able to handle "Silent Fail" 
@SYMTestPriority		Medium
@SYMTestActions			Add a walk around function to handle the "Silent Fail" in OOM test, so that the
						testing loop won't exit at silent fail ponits.
@SYMTestExpectedResults	OOM could reach the last iteration needed
@SYMDEF					DEF130486
*/
// This is a walk around to fix the "Silent Fail" in OOM test
LOCAL_C TBool TestExit(const TInt& aErr)
	{
#ifdef _DEBUG
	TEST(aErr == KErrNone || aErr == KErrNoMemory);
	if (aErr == KErrNone )
		{
		// if the error is KErrNone and it is the exit point of testing loop then the next memory allocation must fail
		// otherwise this is a silent fail.
		TAny* const ptr = User::Alloc(1);
		if (ptr == NULL)
			return ETrue;
		User::Free(ptr);
		}
	return EFalse;
#else
	// User::__DbgSetAllocFail does not work in release build, this function should always return ETrue.
	TEST(aErr == KErrNone);
	return ETrue;
#endif 
	}

// We don't do full close & initialize because it's time consuming
LOCAL_C void ClearServerResources()
	{
	TServerResources::iObserver->CloseiOpenRepositories();
	TServerResources::iObserver->CloseiRepositoryInfo();
	TServerResources::iObserver->Reset();
	
	// To get rid of the array leaking problems during OOM testing.
	TServerResources::iOwnerIdLookUpTable.Reset();
	
	// change TServerResources::iRomDirectory back
	if (iniRomDir != TServerResources::iRomDirectory)
		{
		delete TServerResources::iRomDirectory;
		TServerResources::iRomDirectory = iniRomDir;
		}
	}

// a Wrap Function for OOM
LOCAL_C void WrapFunctionL(void (*testFunc)())
	{
    InitializationL();
    (testFunc)();
    Cleanup();
	}

// Function to convert a test into an OOM test
/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CT-0117
@SYMTestCaseDesc Test functionality under OOM.
@SYMTestPriority High
@SYMTestActions  Test functionality under OOM.
@SYMTestExpectedResults The test must not fail.
@SYMPREQ PREQ752
         PREQ908
*/
LOCAL_C void OomTest(void (*testFunc)())
	{
	oomRunning = ETrue;
	
	TInt error;
	TBool exit = EFalse;
	TInt count;
	
	// Number of continues KErrNone, this is used to control exiting just in case the
	// TestExit() doesn't catch the exit point and OOM goes to infinate loop.
	TInt contErrNone = 0; 
	
	for (count = 1; !exit; count++)
		{
		if(count%100 == 1)
			{
			TheTest.Printf( _L( "OOM test running at failure rates %d - %d\n" ), count, count + 99);
			}
		User::__DbgMarkStart( RHeap::EUser );
		
		// find out the number of open handles
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

		User::__DbgSetAllocFail( RHeap::EUser, RHeap::EFailNext, count );
		
		TRAP(error, WrapFunctionL(testFunc));
		
		if (error == KErrNone)
			contErrNone++;
		else
			contErrNone = 0;

		exit = (TestExit(error) || contErrNone >= 15);
			
		User::__DbgSetAllocFail( RHeap::EUser, RHeap::ENone, 1 );
		
		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

		TEST(startProcessHandleCount == endProcessHandleCount);
		TEST(startThreadHandleCount  == endThreadHandleCount);
		
		ClearServerResources();

		User::__DbgMarkEnd( RHeap::EUser, 0 );
		}	

	_LIT( KTestFailed, "Out of memory test failure on iteration %d\n" );
	__ASSERT_ALWAYS( error == KErrNone, TheTest.Panic( error, KTestFailed, count ) );

	TheTest.Printf( _L( "Out of memory tests succeeded at heap failure rate of %i\n" ), count );
	TheTest.Printf( _L( "Process handle count preserved\n" ) );
	TheTest.Printf( _L( "Thread handle count preserved\n" ) );
	
	oomRunning = EFalse;
	}

/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CIT-1388
@SYMTestCaseDesc CenRep backup data is not restored when repository doesn't already exist
@SYMTestPriority High
@SYMTestActions  backup original file, delete files, restore, check if successful
@SYMTestExpectedResults The test must not fail.
@SYMDEF INC066873
*/
LOCAL_C void INC066873L()
	{
	NextTest( _L( " @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CIT-1388 " ) );
	// create and install the active scheduler we need
	CActiveScheduler* s = new(ELeave) CActiveScheduler;
	
	TCleanupItem tc(CleanupActiveScheduler, s);
	CleanupStack::PushL(tc);
	
	CActiveScheduler::Replace( s );

	backupClient = CRepositoryBackupClient::NewLC(TServerResources::iFs);
	TEST( backupClient != 0 );
	TServerResources::iCacheManager->DisableCache();

	// Install known files
	InstallFileSetL( EOriginals );
	// backup known files,
	BackupRepositoryL( originalData1 );
	// delete files
	DeleteFilesL();

	// Create a iRomDirectory pointing to a ROM directory where repositories do not exist
	_LIT( KEmptyRomDirectory, "Z:\\private\\" );
	if (!oomRunning)
		{
		delete TServerResources::iRomDirectory ;
		}
	TServerResources::iRomDirectory = HBufC::NewL(KEmptyRomDirectory().Length());
	TPtr ptr(TServerResources::iRomDirectory->Des());
	ptr.Copy(KEmptyRomDirectory);

	// restore when other files do not exist
	RestoreRepositoryL( originalData1 );
	// read from restored file
	BackupRepositoryL( originalData2 );

	DeleteFilesL();
	// Test to see if previously backed up data is correctly restored
	TEST( StreamsAreIdenticalL( originalData1, originalData2 ) );

	CleanupStack::PopAndDestroy( backupClient );
	// Cleanup the scheduler
	CleanupStack::PopAndDestroy( s );
	}

/**
DEF058823: Central Repository client requests are unsafely blocked during Backup & Restore

Test checks if a transaction started before BUR process will be failed properly.
It also checks if changes made in a transaction before BUR process will be succesfully
reverted.

Other test for this defect can be found in t_cenrep_defects.cpp
*/
LOCAL_C void DEF058823L()
    {
    TInt err;
    TInt getValue;
    TUint32 KInt = 0x1;
    TUint32 KInt1 = 0x02010100;
    TUint32 errorId;
    const TInt KIntValue = 100;
    const TInt KIntOriginalValue = 1;
    const TInt KInt1OriginalValue = 10;

	// create and install the active scheduler we need
	CActiveScheduler* s = new(ELeave) CActiveScheduler;
	
	TCleanupItem tc(CleanupActiveScheduler, s);
	CleanupStack::PushL(tc);
	
	CActiveScheduler::Replace( s );

	backupClient = CRepositoryBackupClient::NewLC( TServerResources::iFs );
	TEST( backupClient != 0 );

	TServerResources::iCacheManager->DisableCache();

	// Notifier needed to open repositories.
	CSessionNotifier* notifier;
	notifier = new(ELeave)CSessionNotifier;
	CleanupStack::PushL(notifier);

    InstallFileSetL(EDEF058823L);
    //=========================tests during backup process============================

    /**

    Start transaction, set a setting and than perform a backup.
    Backup process should fail running transaction and discard
    its changes to the repository.

    */

    CServerRepository* repository = new(ELeave) CServerRepository();
    CleanupStack::PushL(repository);
    repository->OpenL(KUidBackupTestRepository,*notifier);

	// Begin concurrent read write transaction
	err = repository->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(err, KErrNone);

	//perform write operation before backup process
	err = repository->TransactionSetL(KInt, KIntValue);
	if (err == KErrNoMemory)
		User::Leave(err);
	TEST2(err, KErrNone);

	//this will perform backup process
    BackupRepositoryL(originalData1);

    //can not commit transaction because it was failed by a backup process
	err = repository->CommitTransaction(errorId);
   	TEST2(err,KErrLocked);

	err = repository->TransactionState();
	TEST2(err, ENoTransaction);

    //check if the data was successfully reverted
	err = repository->Get(KInt, getValue);
	TEST2(err, KErrNone);
	TEST(getValue == KIntOriginalValue);

    /**

    Start transaction,perform a backup than set a setting.
    Backup process should fail running transaction and its all
    consecutive operations.

    */

    // Begin concurrent read write transaction
	err = repository->StartTransaction(CRepository::EReadWriteTransaction);
	TEST2(err, KErrNone);

	//this will perform backup process
    BackupRepositoryL(originalData1);

    //backup process has failed current transaction thus the call
    //to the TransactionSetL would result in a panic.
    //client side simulation can be found in t_cenrep_defects.cpp
	err = repository->IsInActiveReadWriteTransaction();
	TEST(err==EFalse);

	//can not commit transaction because it was failed by a backup process
	err = repository->CommitTransaction(errorId);
   	TEST2(err,KErrLocked);

	err = repository->TransactionState();
	TEST2(err, ENoTransaction);

	repository->Close();

   	CleanupStack::PopAndDestroy(repository);
   	CleanupStack::PopAndDestroy(notifier);
  	CleanupStack::PopAndDestroy( backupClient );
	CleanupStack::PopAndDestroy( s );

	Cleanup();
    InitializationL();
   	//
   	//==============tests during restore process====================================
	//
    InstallFileSetL( EOriginals );

	// create and install the active scheduler we need
	s = new(ELeave) CActiveScheduler;
	
	TCleanupItem tc1(CleanupActiveScheduler, s);
	CleanupStack::PushL(tc1);
	
	CActiveScheduler::Replace( s );

	backupClient = CRepositoryBackupClient::NewLC( TServerResources::iFs );
	TEST( backupClient != 0 );

	TServerResources::iCacheManager->DisableCache();

	// Notifier needed to open repositories.
	notifier = new(ELeave)CSessionNotifier;
	CleanupStack::PushL(notifier);

    repository = new(ELeave) CServerRepository();
    CleanupStack::PushL(repository);
    repository->OpenL(KUidBackupTestRepository1,*notifier);

    /**

    Start transaction, set a setting and than perform a restore.
    restore process should fail running transaction and discard
    its changes to the repository.

    */

    //check original value
	err = repository->Get(KInt1, getValue);
	TEST2(err, KErrNone);
	TEST(getValue == KInt1OriginalValue);

	//backup repository
    BackupRepositoryL(originalData1);

	// Begin concurrent read write transaction
	err = repository->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(err, KErrNone);

	//try to perform another write operation before restore process
	err = repository->TransactionSetL(KInt1, KIntValue);
	if (err == KErrNoMemory)
		User::Leave(err);
		
	TEST2(err, KErrNone);

	err = repository->Get(KInt1, getValue);
	TEST2(err, KErrNone);
	TEST(getValue == KIntValue);

	RestoreRepositoryL( originalData1 );

    //can not commit transaction because it was failed by a restore process
	err = repository->CommitTransaction(errorId);
   	TEST2(err,KErrLocked);

	err = repository->TransactionState();
	TEST2(err, ENoTransaction);

    //check if original value was restored
	err = repository->Get(KInt1, getValue);
	TEST2(err, KErrNone);
	TEST(getValue == KInt1OriginalValue);

	repository->Close();
   	CleanupStack::PopAndDestroy(repository);
   	CleanupStack::PopAndDestroy(notifier);
  	CleanupStack::PopAndDestroy( backupClient );
	CleanupStack::PopAndDestroy( s );
	DeleteFilesL();
    }

/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-UT-3520
@SYMTestCaseDesc Exercise the unsupported methods that must be implemented for CRepositoryBackupClient
@SYMTestPriority High
@SYMTestActions  Call the unsupported methods and check that the appropriate error code is returned.
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF109469
*/
LOCAL_C void DEF109469L()
	{
	NextTest( _L( " @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-UT-3520 Check unsupported methods " ) );

	// create and install the active scheduler we need
	CActiveScheduler* s = new(ELeave) CActiveScheduler;
	
	TCleanupItem tc(CleanupActiveScheduler, s);
	CleanupStack::PushL(tc);
	
	CActiveScheduler::Replace( s );

	backupClient = CRepositoryBackupClient::NewLC( TServerResources::iFs );
	TEST( backupClient != 0 );

	TDriveNumber drive=EDriveC;							// Not required to test unsupported methods, initialised to fix compiler warning
	HBufC8* transferBuffer = HBufC8::NewL( 128 );
	CleanupStack::PushL( transferBuffer );
	TPtr8 bufferToSend = transferBuffer->Des();
	TBool finished ;
	TUint result;

	result=backupClient->GetDataChecksum(drive);
	TEST2(result,KArbitraryNumber);

	result=backupClient->GetExpectedDataSize(drive);
	TEST2(result,KArbitraryNumber);

	TRAPD( err, backupClient->GetSnapshotDataL( drive, bufferToSend, finished));
	TEST2(err,KErrNotSupported);

	TRAP( err, backupClient->InitialiseGetBackupDataL(drive));
	TEST2(err,KErrNotSupported);

	TRAP( err, backupClient->InitialiseRestoreBaseDataL(drive));
	TEST2(err,KErrNotSupported);

	TRAP( err, backupClient->InitialiseRestoreIncrementDataL(drive));
	TEST2(err,KErrNotSupported);

	TRAP( err, backupClient->RestoreIncrementDataSectionL(bufferToSend, finished));
	TEST2(err,KErrNotSupported);

	TRAP( err, backupClient->ReceiveSnapshotDataL(drive, bufferToSend, finished));
	TEST2(err,KErrNotSupported);

	//this method does nothing. Call it to exercise it which at least ensures that it doesn't panic
	//it shouldn't leave so don't trap it.
	backupClient->AllSnapshotsSuppliedL();

	CleanupStack::PopAndDestroy( transferBuffer );
	CleanupStack::PopAndDestroy( backupClient );
	// Cleanup the scheduler
	CleanupStack::PopAndDestroy( s );
	}

LOCAL_C void DoBackupRestoreTestsL()
	{
	NextTest( _L( "Backup tests" ) );
	BackupTestsL();

	NextTest( _L( "Restore tests" ) );
	RestoreTestsL();
	}

LOCAL_C void DoTestsL()
	{
	TServerResources::InitialiseL();
	
	NextTest( _L( "Do initialization" ) );
    InitializationL();
	
	DoBackupRestoreTestsL();
	
    NextTest( _L( "DEF058823 tests" ) );
	DEF058823L();

    NextTest( _L( "INC066873 tests" ) );
    INC066873L();

    NextTest( _L( "DEF109469: Title Central repository code coverage could be increased" ) );
    DEF109469L();
    
	NextTest( _L( "Cleanup" ) );
	Cleanup();

    TServerResources::Close();
	}

LOCAL_C void DoOomTestL()
	{
	NextTest( _L( " @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-0117 " ) );
	
	NextTest( _L( " OOM test for backup and restore tests " ) );
	TServerResources::InitialiseL();
	iniRomDir = TServerResources::iRomDirectory;
	OomTest(DoBackupRestoreTestsL);
	TServerResources::Close();
	
	NextTest( _L( " OOM test for DEF058823L " ) );
	TServerResources::InitialiseL();
	iniRomDir = TServerResources::iRomDirectory;
	OomTest(DEF058823L);
	TServerResources::Close();
	
	NextTest( _L( " OOM test for INC066873L " ) );
	TServerResources::InitialiseL();
	iniRomDir = TServerResources::iRomDirectory;
	OomTest(INC066873L);
	TServerResources::Close();
	
	NextTest( _L( " OOM test for DEF109469L " ) );
	TServerResources::InitialiseL();
	iniRomDir = TServerResources::iRomDirectory;
	OomTest(DEF109469L);
	TServerResources::Close();
	}

LOCAL_C void MainL()
	{
	__UHEAP_MARK;
	TheTest.Start( _L( "Backup and restore tests" ) );
	
	oomRunning = EFalse;

	// create and install the active scheduler we need for the cache manager in TServerResources::InitialiseL
	globalAS=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(globalAS);
	CActiveScheduler::Install(globalAS);

	DoTestsL();

	// TServerResources is left in an invalid state by previous tests!
	NextTest( _L(  "Out of memory tests" ) );
	DoOomTestL();

	CleanupStack::PopAndDestroy(globalAS);

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
	if ( err != KErrNone )
		User::Panic( _L( "Testing failed: " ), err );

	delete cleanup;
	__UHEAP_MARKEND;

	return 0;
	}

