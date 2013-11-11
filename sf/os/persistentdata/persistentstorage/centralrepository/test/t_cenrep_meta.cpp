// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <centralrepository.h>
#include "t_cenrep_helper.h"
#include <e32debug.h>

RTest TheTest(_L("Central Repository Settings Tests"));
RFs fs;

const TUid KUidTestRepository = { 0x00000005 };
const TInt KIntOrigValue = 1;
const TUint32 KGlobalDefaultMeta = 0;
const TUint32 KRange2DefaultMeta = 2;
const TUint32 KMetaAtKeyPos24    = 4;

const TUint32 KDefMeta0SingleMeta0Key = 0x1;
const TUint32 KDefMeta0SingleMeta1Key = 0x2;
const TUint32 KDefMeta1SingleMeta0Key = 0x10;
const TUint32 KDefMeta1SingleMeta1Key = 0x11;
const TUint32 KMissingSingleMetaKey   = 0x20;

const TUint32 KPlatsecFailureKey      = 0x21;

const TUint32 KRange2KeyWithMeta4     = 0x24;

_LIT( KSrcTxaPath, "z:\\private\\10202BE9\\00000005.txa" );
_LIT( KSrcCacheIniPath, "z:\\private\\10202BE9\\centrepcache.ini9" );

_LIT( KInstallTxtPath, "C:\\private\\10202BE9\\00000005.txt" );
_LIT( KInstallCrePath, "C:\\private\\10202BE9\\00000005.cre" );
_LIT( KPersistsCrePath, "C:\\private\\10202BE9\\persists\\00000005.cre" );
_LIT( KCacheIniPath, "C:\\private\\10202BE9\\centrep.ini" );


TPtrC metaTestFiles[] =
	{
	TPtrC(KInstallTxtPath),
	TPtrC(KInstallCrePath),
	TPtrC(KPersistsCrePath),

	// This one last because we do not always delete it.
	TPtrC(KCacheIniPath)
    };

const TInt KNumMetaTestFiles = sizeof(metaTestFiles) / sizeof(TPtrC);

_LIT( KCentralRepositoryServerName, "Centralrepositorysrv");

//===========================================================
// Test macroses and functions
//===========================================================

LOCAL_C TInt DeleteFile(const TDesC& aFileName, CFileMan& aFileMan)
	{
	TInt r = aFileMan.Delete( aFileName );
	if ( r != KErrNone && r != KErrNotFound && r != KErrPathNotFound )
		{
		return r;
		}
	return KErrNone;
	}

LOCAL_C TInt DeleteFilesL(TPtrC aFileList[], TInt aNumFiles)
	{
	CFileMan* fm = CFileMan::NewL(fs);
	CleanupStack::PushL( fm );

	TInt ret = KErrNone;
	for (TInt i = 0; i < aNumFiles; i++)
		{
		TInt r = DeleteFile( aFileList[i], *fm );
		if (r != KErrNone)
			{
			ret = r;
			}
		}
	CleanupStack::PopAndDestroy( fm );
	return ret;
	}

LOCAL_C void TestCleanupL()
    {
    DeleteFilesL(metaTestFiles, KNumMetaTestFiles);
    KillProcess(KCentralRepositoryServerName);
    }

LOCAL_C void CheckL( TInt aValue, TInt aLine )
	{
	if ( !aValue )
		{
		TestCleanupL();
		TheTest( EFalse, aLine );
		}
	}

LOCAL_C void CheckL( TInt aValue, TInt aExpected, TInt aLine )
	{
	if ( aValue != aExpected )
		{
		TheTest.Printf( _L( "*** Expected value: %d, got: %d\r\n"), aExpected, aValue );
		TestCleanupL();
		TheTest( EFalse, aLine );
		}
	}

#define TEST(arg) ::CheckL((arg), __LINE__)
#define TEST2(aValue, aExpected) ::CheckL(aValue, aExpected, __LINE__)

LOCAL_C void CopyFileL(const TDesC& aSrc,
					   const TDesC& aDest,
					   TBool aClearROAttrib)
	{
	CFileMan* fm = CFileMan::NewL( fs );
	CleanupStack::PushL( fm );

	TInt ret = fm->Copy(aSrc, aDest);
	if (aClearROAttrib && (ret == KErrNone))
		{
		ret = fm->Attribs( aDest, KEntryAttArchive, KEntryAttReadOnly, TTime( 0 ), CFileMan::ERecurse );
		}

	User::LeaveIfError(ret);

	CleanupStack::PopAndDestroy( fm );
	}

//=============================================================
// class to encapsulate metadata test
class TMetadataTest
	{
public:
	inline TMetadataTest() {}
	~TMetadataTest();
	void InitialiseL();
	void InstallCreFileL();
	void RunTestsL();
	void RunSetTestL();
	void RunDeleteResetTestL();
	void RunDeleteCreateTestL();
	void RunDeleteCreateTest2L();
	void RunMoveTestL();

	void RunBasicGetMetaTestsL();

	TBool VerifyMetaDataUnchangedL();
	};

//==========================================
TMetadataTest::~TMetadataTest()
	{
	TRAPD(err, ::TestCleanupL());
	if (err != KErrNone)
	    {
	    RDebug::Printf("err = %d in ~TMetadataTest.", err);
	    }
	}

//==========================================
void TMetadataTest::InitialiseL()
	{
	::TestCleanupL();
	User::After(1000000); // pause after KillProcess
	::CopyFileL(KSrcTxaPath, KInstallTxtPath, ETrue);
	::CopyFileL(KSrcCacheIniPath, KCacheIniPath, ETrue);
	}

//==========================================
void TMetadataTest::InstallCreFileL()
	{
	// Ensure start with original 00000005.txa file.
	// Be careful. Do not delete centrep.ini
	::DeleteFilesL(metaTestFiles, KNumMetaTestFiles - 1);
	::CopyFileL(KSrcTxaPath, KInstallTxtPath, ETrue);

	CRepository* rep = CRepository::NewLC(KUidTestRepository);

	// Create/Delete to persist the .txt to .cre
	TInt r = rep->Create(0xA5A5, 1);
	TEST2(r, KErrNone);

	rep->Delete(0xA5A5);
	TEST2(r, KErrNone);

	CleanupStack::PopAndDestroy(rep);

	::CopyFileL(KPersistsCrePath, KInstallCrePath, EFalse);

	TPtrC onefile[1] = { TPtrC(KPersistsCrePath) };
	::DeleteFilesL(onefile, 1);
	}

//==========================================
void TMetadataTest::RunTestsL()
	{
	RunSetTestL();
	RunDeleteResetTestL();
	RunDeleteCreateTestL();
	RunDeleteCreateTest2L();
	RunMoveTestL();
	}

//============================================
TBool TMetadataTest::VerifyMetaDataUnchangedL()
	{
	CRepository* rep = CRepository::NewLC(KUidTestRepository);

	TInt ret = ETrue;
	TUint32 meta;

	TInt r = rep->GetMeta(KMissingSingleMetaKey, meta);
	TEST2(r, KErrNone);
	if ( meta !=  KRange2DefaultMeta)
		{
		TheTest.Printf(_L("Key: 0x%x, meta is %d"), KMissingSingleMetaKey, meta);
		ret = EFalse;
		}

	r = rep->GetMeta(KDefMeta0SingleMeta0Key, meta);
	TEST2(r, KErrNone);
	if ( meta !=  0)
		{
		TheTest.Printf(_L("Key: 0x%x, meta is %d"), KDefMeta0SingleMeta0Key, meta);
		ret = EFalse;
		}

	r = rep->GetMeta(KDefMeta0SingleMeta1Key, meta);
	TEST2(r, KErrNone);
	if ( meta !=  1)
		{
		TheTest.Printf(_L("Key: 0x%x, meta is %d"), KDefMeta0SingleMeta1Key, meta);
		ret = EFalse;
		}

	r = rep->GetMeta(KDefMeta1SingleMeta0Key, meta);
	TEST2(r, KErrNone);
	if ( meta !=  0)
		{
		TheTest.Printf(_L("Key: 0x%x, meta is %d"), KDefMeta1SingleMeta0Key, meta);
		ret = EFalse;
		}

	r = rep->GetMeta(KDefMeta1SingleMeta1Key, meta);
	TEST2(r, KErrNone);
	if ( meta !=  1)
		{
		TheTest.Printf(_L("Key: 0x%x, meta is %d"), KDefMeta1SingleMeta1Key, meta);
		ret = EFalse;
		}

	CleanupStack::PopAndDestroy(rep);
	return ret;
	}

//============================================
void TMetadataTest::RunBasicGetMetaTestsL()
	{
	TheTest.Next(_L("Basic functional tests on the GetMeta API\r\n"));

	CRepository* rep = CRepository::NewLC(KUidTestRepository);

	TUint32 meta;

	// GetMeta on non-existing key.
	const TUint32 KNonExistingKey = 0xFACE;
	TInt r = rep->GetMeta(KNonExistingKey, meta);
	TEST2(r, KErrNotFound);

	// Missing capability to read meta of following key.
	r = rep->GetMeta(KPlatsecFailureKey, meta);
	TEST2(r, KErrPermissionDenied);

	// GetMeta in transaction
	r = rep->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(r, KErrNone);
	rep->CleanupCancelTransactionPushL();

	r = rep->Set(KMissingSingleMetaKey, KIntOrigValue+1);
	TEST2(r, KErrNone);

	r = rep->GetMeta(KMissingSingleMetaKey, meta);
	TEST2(r, KErrNone);
	TEST2(meta, KRange2DefaultMeta);

	CleanupStack::PopAndDestroy(); // cancel transaction

	CleanupStack::PopAndDestroy(rep);
	}

//============================================
void TMetadataTest::RunMoveTestL()
	{
	TheTest.Next(_L("Metadata Move settings test\r\n"));

	const TUint32 KDefRangeKeyForCreate = 0x3A;
	const TUint32 KRange2KeyForCreate   = 0x2A;
	const TUint32 KDefRangeKeyForMove   = 0x3B;
	const TUint32 KRange2KeyForMove     = 0x2B;

	CRepository* rep = CRepository::NewLC(KUidTestRepository);

	TInt r = rep->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(r, KErrNone);
	rep->CleanupCancelTransactionPushL();

	r = rep->Create(KDefRangeKeyForCreate, KIntOrigValue);
	TEST2(r, KErrNone);

	r = rep->Create(KRange2KeyForCreate, KIntOrigValue);
	TEST2(r, KErrNone);

	CleanupStack::Pop(); // transaction
	TUint32 errKey;
	r = rep->CommitTransaction(errKey);
	TEST2(r, KErrNone);

	// Check that the keys are created with correct meta

	TUint32 meta;
	// This key is not covered by any rangemeta. The global
	// default meta should be used.
	r = rep->GetMeta(KDefRangeKeyForCreate, meta);
	TEST2(r, KErrNone);
	TEST2(meta, KGlobalDefaultMeta);

	// This key is covered by a rangemeta.
	r = rep->GetMeta(KRange2KeyForCreate, meta);
	TEST2(r, KErrNone);
	TEST2(meta, KRange2DefaultMeta);

	// move the created keys to different ranges
	r = rep->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(r, KErrNone);
	rep->CleanupCancelTransactionPushL();

	const TUint32 KMoveMask = 0x3F;
	r = rep->Move(KDefRangeKeyForCreate, KRange2KeyForMove, KMoveMask,
			errKey);
	TEST2(r, KErrNone);

	r = rep->Move(KRange2KeyForCreate, KDefRangeKeyForMove, KMoveMask,
			errKey);
	TEST2(r, KErrNone);

	CleanupStack::Pop(); // transaction
	r = rep->CommitTransaction(errKey);
	TEST2(r, KErrNone);

	// This key is moved from range where rangemeta is 2 to range
	// where meta is defined by the global default meta.
	r = rep->GetMeta(KDefRangeKeyForMove, meta);
	TEST2(r, KErrNone);
	TEST2(meta, KGlobalDefaultMeta);

	// This key is moved from range where meta is defined by the
	// global default meta to a range where rangemeta is 2.
	r = rep->GetMeta(KRange2KeyForMove, meta);
	TEST2(r, KErrNone);
	TEST2(meta, KRange2DefaultMeta);


	// Test Move a key to a position which has just been deleted but
	// not yet commited.

	// Create two keys to be moved. To save typing, no transaction.
	r = rep->Create(KDefRangeKeyForCreate, KIntOrigValue);
	TEST2(r, KErrNone);
	r = rep->Create(KRange2KeyForCreate, KIntOrigValue);
	TEST2(r, KErrNone);

	r = rep->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(r, KErrNone);
	rep->CleanupCancelTransactionPushL();

	// delete the targeted keys but do not commit
	r = rep->Delete(KDefRangeKeyForMove);
	TEST2(r, KErrNone);
	r = rep->Delete(KRange2KeyForMove);
	TEST2(r, KErrNone);

	// move the created keys to target positions
	r = rep->Move(KDefRangeKeyForCreate, KRange2KeyForMove, KMoveMask,
			errKey);
	TEST2(r, KErrNone);
	r = rep->Move(KRange2KeyForCreate, KDefRangeKeyForMove, KMoveMask,
			errKey);
	TEST2(r, KErrNone);

	CleanupStack::Pop(); // transaction
	r = rep->CommitTransaction(errKey);
	TEST2(r, KErrNone);


	// This key is moved from range where rangemeta is 2 to range
	// where meta is defined by the global default meta.
	r = rep->GetMeta(KDefRangeKeyForMove, meta);
	TEST2(r, KErrNone);
	TEST2(meta, KGlobalDefaultMeta);

	// This key is moved from range where meta is defined by the
	// global default meta to a range where rangemeta is 2.
	r = rep->GetMeta(KRange2KeyForMove, meta);
	TEST2(r, KErrNone);
	TEST2(meta, KRange2DefaultMeta);

	// Test Move a key to a position which has a setting defined
	// in the INI file. The metadata of this pre-existing key
	// is permanently attached to that key position and takes
	// precedent over defaultmeta.

	// Create the setting to be moved.
	r = rep->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(r, KErrNone);
	rep->CleanupCancelTransactionPushL();

	r = rep->Create(KRange2KeyForCreate, KIntOrigValue);
	TEST2(r, KErrNone);

	// delete the key occuping the destination of the move.
	r = rep->Delete(KRange2KeyWithMeta4);
	TEST2(r, KErrNone);

	CleanupStack::Pop(); // transaction
	r = rep->CommitTransaction(errKey);
	TEST2(r, KErrNone);

	// Verify target has been deleted.
	TInt a;
	r = rep->Get(KRange2KeyWithMeta4, a);
	TEST2(r, KErrNotFound);

	// move the created keys to target positions
	r = rep->Move(KRange2KeyForCreate, KRange2KeyWithMeta4, KMoveMask,
			errKey);
	TEST2(r, KErrNone);

	// This key is moved within range where rangemeta is 2.
	// Target position of the Move has a setting defined
	// in the INI file. The metadata of this pre-existing key
	// is permanently attached to that key position and takes
	// precedent over the rangemeta.
	r = rep->GetMeta(KRange2KeyWithMeta4, meta);
	TEST2(r, KErrNone);
	TEST2(meta, KMetaAtKeyPos24);

	// cleanup
	r = rep->Reset();
	TEST2(r, KErrNone);

	CleanupStack::PopAndDestroy(rep);
	}

//============================================
void TMetadataTest::RunSetTestL()
	{
	TheTest.Next(_L("Metadata Set settings test\r\n"));

	CRepository* rep = CRepository::NewLC(KUidTestRepository);

	TInt r;
	r = rep->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(r, KErrNone);
	rep->CleanupCancelTransactionPushL();

	const TInt KNewValue = KIntOrigValue + 1;
	r = rep->Set(KMissingSingleMetaKey, KNewValue);
	TEST2(r, KErrNone);
	r = rep->Set(KDefMeta0SingleMeta0Key, KNewValue);
	TEST2(r, KErrNone);
	r = rep->Set(KDefMeta0SingleMeta1Key, KNewValue);
	TEST2(r, KErrNone);
	r = rep->Set(KDefMeta1SingleMeta0Key, KNewValue);
	TEST2(r, KErrNone);
	r = rep->Set(KDefMeta1SingleMeta1Key, KNewValue);
	TEST2(r, KErrNone);

	CleanupStack::Pop(); // transaction
	TUint32 keyInfo;
	r = rep->CommitTransaction(keyInfo);
	TEST2(r, KErrNone);

	// check that the Set has changed the settings.
	TInt a;
	r = rep->Get(KMissingSingleMetaKey, a);
	TEST2(r, KErrNone);
	TEST2(a, KNewValue);

	r = rep->Get(KDefMeta0SingleMeta0Key, a);
	TEST2(r, KErrNone);
	TEST2(a, KNewValue);

	r = rep->Get(KDefMeta0SingleMeta1Key, a);
	TEST2(r, KErrNone);
	TEST2(a, KNewValue);

	r = rep->Get(KDefMeta1SingleMeta0Key, a);
	TEST2(r, KErrNone);
	TEST2(a, KNewValue);

	r = rep->Get(KDefMeta1SingleMeta1Key, a);
	TEST2(r, KErrNone);
	TEST2(a, KNewValue);

	// Check the Set's have not change the metadata
	TEST(VerifyMetaDataUnchangedL());

	// Take this opportunity to Reset the keys to exercise the
	// Set-then-Reset path.

	// Transaction not available for Reset.
	r = rep->Reset(KMissingSingleMetaKey);
	TEST2(r, KErrNone);
	r = rep->Reset(KDefMeta0SingleMeta0Key);
	TEST2(r, KErrNone);
	r = rep->Reset(KDefMeta0SingleMeta1Key);
	TEST2(r, KErrNone);
	r = rep->Reset(KDefMeta1SingleMeta0Key);
	TEST2(r, KErrNone);
	r = rep->Reset(KDefMeta1SingleMeta1Key);
	TEST2(r, KErrNone);

	CleanupStack::PopAndDestroy(rep);

	TEST(VerifyMetaDataUnchangedL());
	}

void TMetadataTest::RunDeleteCreateTestL()
	{
	TheTest.Next(_L("Metadata Delete-Create settings test\r\n"));

	CRepository* rep = CRepository::NewLC(KUidTestRepository);

	TInt r;
	r = rep->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(r, KErrNone);
	rep->CleanupCancelTransactionPushL();

	r = rep->Delete(KMissingSingleMetaKey);
	TEST2(r, KErrNone);
	r = rep->Delete(KDefMeta0SingleMeta0Key);
	TEST2(r, KErrNone);
	r = rep->Delete(KDefMeta0SingleMeta1Key);
	TEST2(r, KErrNone);
	r = rep->Delete(KDefMeta1SingleMeta0Key);
	TEST2(r, KErrNone);
	r = rep->Delete(KDefMeta1SingleMeta1Key);
	TEST2(r, KErrNone);

	CleanupStack::Pop(); // transaction
	TUint32 keyInfo;
	r = rep->CommitTransaction(keyInfo);
	TEST2(r, KErrNone);

	CleanupStack::PopAndDestroy(rep);

	// Now open the repository again and create the deleted keys.
	rep = CRepository::NewLC(KUidTestRepository);

	// Before Create, check if the Delete's actually happened.
	TInt a;
	r = rep->Get(KMissingSingleMetaKey, a);
	TEST2(r, KErrNotFound);
	r = rep->Get(KDefMeta0SingleMeta0Key, a);
	TEST2(r, KErrNotFound);
	r = rep->Get(KDefMeta0SingleMeta1Key, a);
	TEST2(r, KErrNotFound);
	r = rep->Get(KDefMeta1SingleMeta0Key, a);
	TEST2(r, KErrNotFound);
	r = rep->Get(KDefMeta1SingleMeta1Key, a);
	TEST2(r, KErrNotFound);

	r = rep->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(r, KErrNone);
	rep->CleanupCancelTransactionPushL();

	r = rep->Create(KMissingSingleMetaKey, KIntOrigValue);
	TEST2(r, KErrNone);
	r = rep->Create(KDefMeta0SingleMeta0Key, KIntOrigValue);
	TEST2(r, KErrNone);
	r = rep->Create(KDefMeta0SingleMeta1Key, KIntOrigValue);
	TEST2(r, KErrNone);
	r = rep->Create(KDefMeta1SingleMeta0Key, KIntOrigValue);
	TEST2(r, KErrNone);
	r = rep->Create(KDefMeta1SingleMeta1Key, KIntOrigValue);
	TEST2(r, KErrNone);

	CleanupStack::Pop(); // transaction
	r = rep->CommitTransaction(keyInfo);
	TEST2(r, KErrNone);

	CleanupStack::PopAndDestroy(rep);

	TEST(VerifyMetaDataUnchangedL());
	}

//=================================================
void TMetadataTest::RunDeleteCreateTest2L()
	{
	TheTest.Next(_L("Metadata Delete-Create settings (uncommitted) test\r\n"));

	CRepository* rep = CRepository::NewLC(KUidTestRepository);

	TInt r;
	r = rep->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(r, KErrNone);
	rep->CleanupCancelTransactionPushL();

	r = rep->Delete(KMissingSingleMetaKey);
	TEST2(r, KErrNone);
	r = rep->Delete(KDefMeta0SingleMeta0Key);
	TEST2(r, KErrNone);
	r = rep->Delete(KDefMeta0SingleMeta1Key);
	TEST2(r, KErrNone);
	r = rep->Delete(KDefMeta1SingleMeta0Key);
	TEST2(r, KErrNone);
	r = rep->Delete(KDefMeta1SingleMeta1Key);
	TEST2(r, KErrNone);

	// Next Create while Delete still in transaction.
	r = rep->Create(KMissingSingleMetaKey, KIntOrigValue);
	TEST2(r, KErrNone);
	r = rep->Create(KDefMeta0SingleMeta0Key, KIntOrigValue);
	TEST2(r, KErrNone);
	r = rep->Create(KDefMeta0SingleMeta1Key, KIntOrigValue);
	TEST2(r, KErrNone);
	r = rep->Create(KDefMeta1SingleMeta0Key, KIntOrigValue);
	TEST2(r, KErrNone);
	r = rep->Create(KDefMeta1SingleMeta1Key, KIntOrigValue);
	TEST2(r, KErrNone);

	// Make a dummy change in case the Delete/Create cancel each
	// other, ending up with nothing to commit.
	r = rep->Create(0xA5A5, KIntOrigValue);
	TEST2(r, KErrNone);

	CleanupStack::Pop(); // transaction
	TUint32 keyInfo;
	r = rep->CommitTransaction(keyInfo);
	TEST2(r, KErrNone);
	r = rep->Delete(0xA5A5);
	TEST2(r, KErrNone);

	CleanupStack::PopAndDestroy(rep);

	TEST(VerifyMetaDataUnchangedL());
	}

//===============================================
void TMetadataTest::RunDeleteResetTestL()
	{
	TheTest.Next(_L("Metadata Delete-Reset settings test\r\n"));

	CRepository* rep = CRepository::NewLC(KUidTestRepository);

	TInt r;
	r = rep->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TEST2(r, KErrNone);
	rep->CleanupCancelTransactionPushL();

	r = rep->Delete(KMissingSingleMetaKey);
	TEST2(r, KErrNone);
	r = rep->Delete(KDefMeta0SingleMeta0Key);
	TEST2(r, KErrNone);
	r = rep->Delete(KDefMeta0SingleMeta1Key);
	TEST2(r, KErrNone);
	r = rep->Delete(KDefMeta1SingleMeta0Key);
	TEST2(r, KErrNone);
	r = rep->Delete(KDefMeta1SingleMeta1Key);
	TEST2(r, KErrNone);

	CleanupStack::Pop(); // transaction
	TUint32 keyInfo;
	r = rep->CommitTransaction(keyInfo);
	TEST2(r, KErrNone);

	CleanupStack::PopAndDestroy(rep);

	// check if the delete's actually happened
	rep = CRepository::NewLC(KUidTestRepository);

	TInt a;
	r = rep->Get(KMissingSingleMetaKey, a);
	TEST2(r, KErrNotFound);
	r = rep->Get(KDefMeta0SingleMeta0Key, a);
	TEST2(r, KErrNotFound);
	r = rep->Get(KDefMeta0SingleMeta1Key, a);
	TEST2(r, KErrNotFound);
	r = rep->Get(KDefMeta1SingleMeta0Key, a);
	TEST2(r, KErrNotFound);
	r = rep->Get(KDefMeta1SingleMeta1Key, a);
	TEST2(r, KErrNotFound);

	// Next Reset on each deleted key.

	// Transaction not available for Reset.
	r = rep->Reset(KMissingSingleMetaKey);
	TEST2(r, KErrNone);
	r = rep->Reset(KDefMeta0SingleMeta0Key);
	TEST2(r, KErrNone);
	r = rep->Reset(KDefMeta0SingleMeta1Key);
	TEST2(r, KErrNone);
	r = rep->Reset(KDefMeta1SingleMeta0Key);
	TEST2(r, KErrNone);
	r = rep->Reset(KDefMeta1SingleMeta1Key);
	TEST2(r, KErrNone);

	CleanupStack::PopAndDestroy(rep);

	TEST(VerifyMetaDataUnchangedL());
	}

/**
@SYMTestCaseID SYSLIB-CENTRALREPOSITORY-CT-1717
@SYMTestCaseDesc Test Set, Delete, Create and Reset on a key with meta of 0 while defaultmeta is non-zero.
@SYMTestPriority High
@SYMTestActions 1. Do a Set. Verify meta does not change. 2. Delete and Create the key. Verify the non-zero defaultmeta is used. 3. Delete and Reset the key. Verify meta is zero.
@SYMTestExpectedResults The test must not fail.
@SYMDEF INC086298
*/
LOCAL_C void INC086298L()
	{
	TheTest.Printf(_L("INC086298 - Setting with no backup meta value is backed up\r\n"));

	TMetadataTest metaDataTester;
	metaDataTester.InitialiseL();

	metaDataTester.RunBasicGetMetaTestsL();

	TheTest.Printf( _L("Metadata tests on .txt file\r\n") );
	metaDataTester.RunTestsL();

	TheTest.Printf( _L("Metadata tests on .cre file\r\n") );
	metaDataTester.InstallCreFileL();
	metaDataTester.RunTestsL();
	}

LOCAL_C void MainL()
	{
	TheTest.Start( _L( " @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-1717 CentralRepository metadata test " ) );
	User::LeaveIfError(fs.Connect());

	CleanupClosePushL(fs);

    INC086298L();

	CleanupStack::PopAndDestroy(&fs);

	TheTest.End();
	TheTest.Close();
	}

TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if ( !cleanup )
		return KErrNoMemory;

	TRAPD( err, MainL()  );

	delete cleanup;
	__UHEAP_MARKEND;

	return err;
	}
