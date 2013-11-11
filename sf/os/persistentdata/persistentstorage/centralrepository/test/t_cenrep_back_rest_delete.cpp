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

//#include <centralrepository.h>
#include <e32test.h>
#include <f32file.h>
#include <badesca.h>
#include "srvrepos_noc.h"
#include "srvres.h"
#include "cachemgr.h"
#include "backup.h"

RTest TheTest(_L("Central Repository Backup & Restore Delete Tests"));

enum FileSet
	{
	EOriginals,
	EChanged,
	ERemoved,
	EDEF058823L
	};

_LIT_SECURE_ID( SecureId1, 0x12345 );



CRepositoryBackupClient* backupClient;

CDesC8ArraySeg* originalData1;
CDesC8ArraySeg* changedData1;
CDesC8ArraySeg* originalData2;
CDesC8ArraySeg* changedData2;
CDesC8ArraySeg* changedData3;

CActiveScheduler* globalAS;
const TUid KUidBackupTestRepository1 	= { 0x00000100 };

const TUint32 KBackupInt1 = 0x02010100;
const TUint32 KBackupInt2 = 0x03010200;
const TInt KIntChangedValue2 = 200;
const TUint32 KBackupInt3 = 0x04010300;
const TInt KIntOrigValue3 = 30;

const TUint32 KBackupString1 = 0x02010600;
_LIT8(KStringChangedValue, "test changed");
const TUint32 KBackupString2 = 0x03010600;
const TUint32 KBackupReal1 = 0x02010700;
const TReal KRealOrigValue = 1.1;
const TUint32 KBackupReal2 = 0x03010700;
const TReal KRealChangedValue = 2.5;

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macroses and functions


LOCAL_C void DeleteFilesL()
	{
	_LIT( KOldInstallFiles, "c:\\private\\102081E4\\*.*" );
	_LIT( KOldPersistFiles, "c:\\private\\102081E4\\persists\\*.*" );

	CFileMan* fm = CFileMan::NewL( TServerResources::iFs );
	CleanupStack::PushL( fm );

	TInt r = fm->Delete( KOldInstallFiles );
	if ( r != KErrNone && r != KErrNotFound && r != KErrPathNotFound )
		User::Leave(r);
	r = fm->Delete( KOldPersistFiles );
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
	_LIT( KFileSrcPath, "\\private\\10202BE9\\" );
	_LIT( KInstallFileDestPath, "\\private\\102081E4\\" );
	_LIT( KPersistFileDestPath, "\\private\\102081E4\\persists\\" );
	_LIT( KEDEF058823LFile, "babababa.cre");
	_LIT( KDriveC, "c:" );
	_LIT( KDriveZ, "z:" );
	_LIT( KCreFiles, "*.cre" );
	_LIT( KAllFiles, "*.*" );
	_LIT( KTxtFilesForC_V1, "*.cra" );
	_LIT( KTxtFilesForC_V2, "*.crb" );

	//Use HBufC in preference to TBuf or TFileName to save stack usage
	HBufC* src_buf1 = HBufC::NewLC(KMaxFileName);
	TPtr src1(src_buf1->Des());
	HBufC* src_buf2 = HBufC::NewLC(KMaxFileName);
	TPtr src2(src_buf2->Des());
	HBufC* dest_buf1 = HBufC::NewLC(KMaxFileName);
	TPtr dest1(dest_buf1->Des());
	HBufC* dest_buf2 = HBufC::NewLC(KMaxFileName);
	TPtr dest2(dest_buf2->Des());

	TInt r;

	DeleteFilesL();

	CFileMan* fm = CFileMan::NewL( TServerResources::iFs );
	CleanupStack::PushL( fm );

	switch ( aFileSet )
		{
		case EOriginals:
			{
			dest1.Copy( KDriveC );
			dest1.Append( KInstallFileDestPath );
			dest1.Append( KCreFiles );
			r = fm->Delete( dest1 );
			if ( r != KErrNone && r != KErrNotFound && r != KErrPathNotFound )
				User::Leave( r );
			r = TServerResources::iFs.MkDirAll( dest1 );
			if ( r != KErrNone && r != KErrAlreadyExists )
				User::Leave( r );
			src1.Copy( KDriveZ );
			src1.Append( KFileSrcPath );
			src1.Append( KTxtFilesForC_V1 );
			User::LeaveIfError( fm->Copy( src1, dest1 ) );
			r = fm->Attribs( dest1, KEntryAttArchive, KEntryAttReadOnly, TTime( 0 ), CFileMan::ERecurse );
			TEST2( r, KErrNone );
			dest2.Copy( KDriveC );
			dest2.Append( KPersistFileDestPath );
			dest2.Append( KCreFiles );
			r = fm->Delete( dest2 );
			if ( r != KErrNone && r != KErrNotFound && r != KErrPathNotFound )
				User::Leave( r );
			r = TServerResources::iFs.MkDirAll( dest2 );
			if ( r != KErrNone && r != KErrAlreadyExists )
				User::Leave( r );
			src2.Copy( KDriveZ );
			src2.Append( KFileSrcPath );
			src2.Append( KTxtFilesForC_V1 );
			User::LeaveIfError( fm->Copy( src2, dest2 ) );
			r = fm->Attribs( dest2, KEntryAttArchive, KEntryAttReadOnly, TTime( 0 ), CFileMan::ERecurse );
			TEST2( r, KErrNone );
			break;
			}
		case EChanged:
			{
			dest1.Copy( KDriveC );
			dest1.Append( KInstallFileDestPath );
			dest1.Append( KCreFiles );
			r = fm->Delete( dest1 );
			if ( r != KErrNone && r != KErrNotFound && r != KErrPathNotFound )
				User::Leave( r );
			r = TServerResources::iFs.MkDirAll( dest1 );
			if ( r != KErrNone && r != KErrAlreadyExists )
				User::Leave( r );
			src1.Copy( KDriveZ );
			src1.Append( KFileSrcPath );
			src1.Append( KTxtFilesForC_V1 );
			User::LeaveIfError( fm->Copy( src1, dest1 ) );
			r = fm->Attribs( dest1, KEntryAttArchive, KEntryAttReadOnly, TTime( 0 ), CFileMan::ERecurse );
			TEST2( r, KErrNone );
			dest2.Copy( KDriveC );
			dest2.Append( KPersistFileDestPath );
			dest2.Append( KCreFiles );
			r = fm->Delete( dest2 );
			if ( r != KErrNone && r != KErrNotFound && r != KErrPathNotFound )
				User::Leave( r );
			r = TServerResources::iFs.MkDirAll( dest2 );
			if ( r != KErrNone && r != KErrAlreadyExists )
				User::Leave( r );
			src2.Copy( KDriveZ );
			src2.Append( KFileSrcPath );
			src2.Append( KTxtFilesForC_V2 );
			User::LeaveIfError( fm->Copy( src2, dest2 ) );
			r = fm->Attribs( dest2, KEntryAttArchive, KEntryAttReadOnly, TTime( 0 ), CFileMan::ERecurse );
			TEST2( r, KErrNone );
			break;
			}
		case ERemoved:
			{
			dest1.Copy( KDriveC );
			dest1.Append( KInstallFileDestPath );
			dest1.Append( KCreFiles );
			r = fm->Delete( dest1 );
			if ( r != KErrNone && r != KErrNotFound && r != KErrPathNotFound )
				User::Leave( r );
			r = TServerResources::iFs.MkDirAll( dest1 );
			if ( r != KErrNone && r != KErrAlreadyExists )
				User::Leave( r );
			src1.Copy( KDriveZ );
			src1.Append( KFileSrcPath );
			src1.Append( KTxtFilesForC_V1 );
			User::LeaveIfError( fm->Copy( src1, dest1 ) );
			r = fm->Attribs( dest1, KEntryAttArchive, KEntryAttReadOnly, TTime( 0 ), CFileMan::ERecurse );
			TEST2( r, KErrNone );
			dest2.Copy( KDriveC );
			dest2.Append( KPersistFileDestPath );
			dest2.Append( KCreFiles );
			r = fm->Delete( dest2 );
			if ( r != KErrNone && r != KErrNotFound && r != KErrPathNotFound )
				User::Leave( r );
			break;
			}
		case EDEF058823L:
		    {
			dest2.Copy( KDriveC );
			dest2.Append( KPersistFileDestPath );
			dest2.Append( KAllFiles );
			r = fm->Delete( dest2 );
			if ( r != KErrNone && r != KErrNotFound && r != KErrPathNotFound )
				User::Leave( r );
			r = TServerResources::iFs.MkDirAll( dest2 );
			if ( r != KErrNone && r != KErrAlreadyExists )
				User::Leave( r );
			src2.Copy( KDriveZ );
			src2.Append( KFileSrcPath );
			src2.Append( KEDEF058823LFile );
			User::LeaveIfError( fm->Copy( src2, dest2 ) );
			r = fm->Attribs( dest2, KEntryAttArchive, KEntryAttReadOnly, TTime( 0 ), CFileMan::ERecurse );
			TEST2( r, KErrNone );
		    break;
		    }
		default:
			RDebug::Print( _L( "Illegal parameter to function: %d\r\n" ), aFileSet );
			TheTest( EFalse, __LINE__ );
		}

	CleanupStack::PopAndDestroy( fm );
	CleanupStack::PopAndDestroy( 4 );
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
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-UT-1535
@SYMTestCaseDesc Test Backup functionality with reference to INC072238: centrep restores deleted content.
@SYMTestPriority High
@SYMTestActions  Test Backup functionality.
@SYMTestExpectedResults The test must not fail.
@SYMPREQ PREQ752
*/
LOCAL_C void BackupTestsL()
	{
	TheTest.Next( _L( " @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-UT-1535 Backup repository " ) );

	// create and install the active scheduler we need
	CActiveScheduler* s = new(ELeave) CActiveScheduler;
	CleanupStack::PushL( s );
	CActiveScheduler::Replace( s );

	backupClient = CRepositoryBackupClient::NewLC( TServerResources::iFs );
	TEST( backupClient != 0 );

	// These tests don't test Backup&Restore functionality over Secure Backup Server so cache management
	// is not possible. For that reason, cache is disabled manually.
	TServerResources::iCacheManager->DisableCache();
	DeleteFilesL();



	TheTest.Next( _L( "Backup original version" ) );
	// Install known files
	InstallFileSetL( EOriginals );

	CServerRepository* aServerRepo=new (ELeave)CServerRepository();
	CleanupStack::PushL(aServerRepo);
	CSessionNotifier* aSessionNotif=new (ELeave) CSessionNotifier();
	CleanupStack::PushL(aSessionNotif);

	// Open repository to delete a few sessions settings
	aServerRepo->OpenL(KUidBackupTestRepository1,*aSessionNotif);

	TUint32 transId = aServerRepo->StartTransaction(EReadWriteTransaction) ;
	aServerRepo->TransactionDeleteL(KBackupInt1) ;
	aServerRepo->TransactionDeleteL(KBackupString1) ;
	aServerRepo->TransactionDeleteL(KBackupReal2) ;
	aServerRepo->CommitTransaction(transId);
	aServerRepo->CommitChangesL() ;
	aServerRepo->Close();

	CleanupStack::PopAndDestroy(aSessionNotif);
	CleanupStack::PopAndDestroy(aServerRepo);

	// Backup the repository
	BackupRepositoryL( originalData1 );

	CleanupStack::PopAndDestroy( backupClient );
	// Cleanup the scheduler
	CleanupStack::PopAndDestroy( s );
	}


/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-UT-1536
@SYMTestCaseDesc Test Restore functionality with reference to INC072238: centrep restores deleted content.
@SYMTestPriority High
@SYMTestActions  Test Restore functionality.
@SYMTestExpectedResults The test must not fail.
@SYMPREQ PREQ752
*/

LOCAL_C void RestoreTestsL()
	{
	TheTest.Next( _L( " @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-UT-1536 Restore repository " ) );

	// create and install the active scheduler we need
	CActiveScheduler* s = new(ELeave) CActiveScheduler;
	CleanupStack::PushL( s );
	CActiveScheduler::Replace( s );

	backupClient = CRepositoryBackupClient::NewLC(TServerResources::iFs);
	TEST( backupClient != 0 );

	CServerRepository* aServerRepo=new (ELeave)CServerRepository();
	CleanupStack::PushL(aServerRepo);
	CSessionNotifier* aSessionNotif=new (ELeave) CSessionNotifier();
	CleanupStack::PushL(aSessionNotif);

	TServerResources::iCacheManager->DisableCache();

	DeleteFilesL();

	TheTest.Next( _L( "Restore original version over changed version" ) );
	InstallFileSetL( EChanged );
	RestoreRepositoryL( originalData1 );

	TheTest.Next( _L( "Confirm settings restored" ) );

	// Open merged repository to check for values of individual settings
	aServerRepo->OpenL(KUidBackupTestRepository1,*aSessionNotif);

	TInt r, a;

	// This setting was DELETED before backup so it shouldn't be present
	// after restore
	r = aServerRepo->Get(KBackupInt1, a);
	TEST2(r, KErrNotFound);


	r = aServerRepo->Get(KBackupInt2, a);
	TEST2(r, KErrNone);
	// This setting isn't backed up, so it must still have changed value
	TEST(a==KIntChangedValue2);

	r = aServerRepo->Get(KBackupInt3, a);
	TEST2(r, KErrNone);
	// This setting is backed up, so it must be restored to original
	TEST(a==KIntOrigValue3);

	TBuf8<KMaxBinaryLength> str;

	// This setting was DELETED before backup so it shouldn't be present
	// after restore
	r = aServerRepo->Get(KBackupString1, str);
	TEST(r==KErrNotFound);

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
	// This setting was deleted but isn't backed up, so it should still
	// exist and have the changed value
	TEST(realval==KRealChangedValue);
	aServerRepo->Close();

	DeleteFilesL();

	CleanupStack::PopAndDestroy(aSessionNotif);
	CleanupStack::PopAndDestroy(aServerRepo);

	CleanupStack::PopAndDestroy( backupClient );
	// Cleanup the scheduler
	CActiveScheduler::Replace( globalAS );
	CleanupStack::PopAndDestroy( s );
	}


// ---------------------------------------------------
// OomTest
//
// Function to convert a test into an OOM test

/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CT-0117-0001
@SYMTestCaseDesc Test functionality under OOM.
@SYMTestPriority High
@SYMTestActions  Test functionality under OOM.
@SYMTestExpectedResults The test must not fail.
@SYMPREQ PREQ752
*/
LOCAL_C void OomTest( void (*testFuncL)() )
	{
	TheTest.Next( _L( " @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-0117-0001 " ) );
	TInt error;
	TInt count = 0;

	do
		{
		User::__DbgMarkStart( RHeap::EUser );

		// find out the number of open handles
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

		User::__DbgSetAllocFail( RHeap::EUser, RHeap::EFailNext, ++count );

		TRAP( error, (testFuncL)() );

		User::__DbgSetAllocFail( RHeap::EUser, RHeap::ENone, 1 );

		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

		TEST(startProcessHandleCount == endProcessHandleCount);
		TEST(startThreadHandleCount  == endThreadHandleCount);

		User::__DbgMarkEnd( RHeap::EUser, 0 );
		} while( error == KErrNoMemory );

	_LIT( KTestFailed, "Out of memory test failure on iteration %d\n" );
	__ASSERT_ALWAYS( error == KErrNone, TheTest.Panic( error, KTestFailed, count ) );

	TheTest.Printf( _L( "Out of memory tests succeeded at heap failure rate of %i\n" ), count );
	TheTest.Printf( _L( "Process handle count preserved\n" ) );
	TheTest.Printf( _L( "Thread handle count preserved\n" ) );
	}





LOCAL_C void DoTestsL()
	{
	TheTest.Next( _L( "Do initialization" ) );
    InitializationL();

	TheTest.Next( _L( "Backup tests" ) );
	BackupTestsL();

	TheTest.Next( _L( "Restore tests" ) );
	RestoreTestsL();

	TheTest.Next( _L( "Cleanup" ) );
	TServerResources::iOwnerIdLookUpTable.Reset();
    Cleanup();
	}

LOCAL_C void MainL()
	{
	__UHEAP_MARK;

	TheTest.Start( _L( "Backup and restore tests" ) );
	// create and install the active scheduler we need for the cache manager in TServerResources::InitialiseL
	globalAS=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(globalAS);
	CActiveScheduler::Install(globalAS);
	TServerResources::InitialiseL();

	DoTestsL();

	TheTest.Next( _L(  "Out of memory tests" ) );
	OomTest( DoTestsL );

	TServerResources::Close();
	CleanupStack::PopAndDestroy(globalAS);

	TheTest.End();
	TheTest.Close();

	__UHEAP_MARKEND;
	}

TInt E32Main()
	{
#ifdef __SECURE_DATA__
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if ( !cleanup )
		return KErrNoMemory;

	TRAPD( err, MainL()  );
	if ( err != KErrNone )
		User::Panic( _L( "Testing failed: " ), err );

	delete cleanup;
	__UHEAP_MARKEND;
#endif

	return 0;
	}

