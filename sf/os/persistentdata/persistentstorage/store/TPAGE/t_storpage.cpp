// Copyright (c) 1998-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <s32cont.h>
#include <s32file.h>
#include <s32crypt.h>
#include <e32test.h>
#include <pbe.h>
#include "UP_STD.H"

const TInt KTestCleanupStack=0x20;

// This is a path specification and should not be used as is
_LIT(KFileLocationSpec, "Z:\\STOR-TST\\T_SPAGE.DAT");
_LIT(KPageFilePath, "C:\\TestSTOR\\T_SPAGEFILE.PAG");
_LIT(KPageFilePathOnly, "C:\\TestSTOR\\");

LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D RTest TheTest(_L("t_storpage"));
LOCAL_D RFs TheFs;
LOCAL_D CFileStore* TheStore;

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Tests macros and functions.
//If (!aValue) then the test will be panicked, the test data files will be deleted.
static void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		TheTest.Printf(_L("*** Boolean expression evaluated to false!\r\n"));
		TheTest(EFalse, aLine);
		}
	}
//If (aValue != aExpected) then the test will be panicked, the test data files will be deleted.
static void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		TheTest.Printf(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
//Use these to test conditions.
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1178
@SYMTestCaseDesc	    TPagedSet functionality test
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for insert,delete,contains,with and without duplicates operations
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void test1L(MPagePool& aPagePool)
	{
	const TInt KEntryCount=200;

	TPagedSet<TInt32> set;
	set.Connect(&aPagePool);
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1178 Insertion & Deletion "));

	TInt32 it=0;
	set.InsertL(it);
	TEST2(set.Count(), 1);
	TRAPD(r,set.InsertL(it));
	TEST2(r, KErrAlreadyExists);
	TEST2(set.Count(), 1);
	TEST(set.ContainsL(it));
	set.DeleteL(it);
	TEST2(set.Count(), 0);
	TRAP(r,set.DeleteL(it));
	TEST2(r, KErrNotFound);
	TEST2(set.Count(), 0);
	TEST(!set.ContainsL(it));

	TheTest.Next(_L("No duplicates"));
	TInt ii;
	for (ii=0;ii<KEntryCount;++ii)
		{
		it=ii;
		set.InsertL(it);
		}
	for (ii=0;ii<KEntryCount;++ii)
		{
		it=ii;
		TEST(set.ContainsL(it));
		}
	TEST2(set.Count(), KEntryCount);

	TheTest.Next(_L("Empty the set"));
	set.ClearL();
	TEST2(set.Count(), 0);
	for (ii=0;ii<KEntryCount;++ii)
		{
		it=ii;
		TEST(!set.ContainsL(it));
		}

	TheTest.End();
	}

struct TTest
	{
	inline TTest() {Mem::FillZ(this,sizeof(*this));}
	TUint32 iVal;
	TUint32 iPadding[14];
	};

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1179
@SYMTestCaseDesc	    Tests for large set of TUint32
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for inserting,contains,iteration,deletion operations
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void test2L(MPagePool& aPagePool)
	{
	const TInt KEntryCount=500;

	TPagedSet<TTest> set;
	set.Connect(&aPagePool);
	TTest item;

	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1179 Add items "));
	TUint32 jj=0;
	TInt32 ii;
	for (ii=KEntryCount;ii>0;--ii)
		{
		jj=(jj+17)%KEntryCount;
		item.iVal=jj;
		set.InsertL(item);
		}
	TEST2(set.Count(), KEntryCount);

	TheTest.Next(_L("Check contents"));
	for (ii=0;ii<KEntryCount;++ii)
		{
		item.iVal=ii;
		TEST(set.ContainsL(item));
		}

	TheTest.Next(_L("Iterate over items"));
	TUint8 *checkMap=(TUint8*)User::AllocLC(KEntryCount);
	Mem::FillZ(checkMap,KEntryCount);
	TPagedSetIter<TTest> iter(set);
	if (iter.ResetL())
		do	++checkMap[iter.AtL().iVal]; while (iter.NextL());
	for (ii=0;ii<KEntryCount;++ii)
		TEST2(checkMap[ii], 1);
	CleanupStack::PopAndDestroy();

	TheTest.Next(_L("Delete items"));
	jj=0;
	for (ii=KEntryCount;ii>KEntryCount/2;--ii)
		{
		jj=(jj+17)%KEntryCount;
		item.iVal=jj;
		set.DeleteL(item);
		}
	TEST2(set.Count(), KEntryCount/2);

	TheTest.Next(_L("Check contents"));
	for (;ii>0;--ii)
		{
		jj=(jj+17)%KEntryCount;
		item.iVal=jj;
		TEST(set.ContainsL(item));
		}
	jj=0;
	for (ii=KEntryCount;ii>KEntryCount/2;--ii)
		{
		jj=(jj+17)%KEntryCount;
		item.iVal=jj;
		TEST(!set.ContainsL(item));
		}

	TheTest.Next(_L("Delete items"));
	for (;ii>1;--ii)
		{
		jj=(jj+17)%KEntryCount;
		item.iVal=jj;
		set.DeleteL(item);
		}
	TEST2(set.Count(), 1);

	TheTest.Next(_L("Check contents"));
	jj=(jj+17)%KEntryCount;
	TPagedSetBiIter<TTest> biter(set);
	TEST(biter.FirstL());
	TEST2(biter.AtL().iVal, jj);
	TEST(!biter.NextL());
	TEST(biter.LastL());
	TEST2(biter.AtL().iVal ,jj);
	TEST(!biter.PreviousL());
	TPagedSetRIter<TTest> riter(set);
	TEST(riter.ResetL());
	TEST2(riter.AtL().iVal, jj);
	TEST(!riter.NextL());

	item.iVal=jj;
	set.DeleteL(item);
	TEST(!iter.ResetL());
	TEST2(set.Count(), 0);

	TheTest.End();
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-1180
@SYMTestCaseDesc	    Streaming tests
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for read and write operations on the streams
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void test3L(RStorePagePool& aPool)
	{
	const TInt KEntryCount=1000;
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-1180 Build set and stream out "));
	aPool.Create(*TheStore);
	TBool rc = aPool.HasAvailable();
	TEST(!rc);
	rc = aPool.IsEmpty();
	TEST(rc);
	TStorePagePoolToken token2(TStorePagePoolToken::EEmpty);
	token2 = aPool.Token();
	rc = token2.IsEmpty();
	TEST(rc);
	rc = token2.HasAvailable();
	TEST(!rc);
	
	TPagedSet<TInt32> set1;
	set1.Connect(&aPool);

	TInt ii;
	for (ii=0;ii<KEntryCount;ii++)
		{
		TInt32 it=ii;
		set1.InsertL(it);
		}
	aPool.FlushL();
	
	RStoreWriteStream out;
	TStreamId id=out.CreateLC(*TheStore);
	out<<aPool.Token();
	out<<set1.Token();
	out.CommitL();
	CleanupStack::PopAndDestroy();	// out
	aPool.Close();
//
	TheTest.Next(_L("Stream in and test set"));
	RStoreReadStream in;
	in.OpenLC(*TheStore,id);
	TStorePagePoolToken ptoken;
	in>>ptoken;
	aPool.Open(*TheStore,ptoken);
	TEST(!aPool.IsDirty());
	TPagedSetToken token;
	in>>token;
	TPagedSet<TInt32> set2(token);
	set2.Connect(&aPool);
	TEST(set2.IsIntact());
	CleanupStack::PopAndDestroy();	// in

	TEST2(set2.Count(), KEntryCount);
	for (ii=0;ii<KEntryCount;ii++)
		{
		TInt32 it=ii;
		set2.DeleteL(it);
		}
	TEST2(set2.Count(), 0);
	aPool.FlushL();
	aPool.Discard();
	aPool.ReclaimAllL();
	aPool.Close();
	TheTest.End();
	}

/**
@SYMTestCaseID          PDS-STORE-CT-4009
@SYMTestCaseDesc	    RFilePagePool tests
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for creating and opening RFilePagePool
@SYMTestExpectedResults RFilePagePool needs to be correctly created, replaced and opened.
@SYMDEF                 DEF135804
*/
LOCAL_C void test4L()
	{
	RFilePagePool testPage;
	TFileName tempPageFileName;
	RFs fs;
	TInt err;
	fs.Connect();
	err = fs.MkDirAll(KPageFilePathOnly);
	TEST(err==KErrNone||err==KErrAlreadyExists);
	err = fs.Delete(KPageFilePath);
	TEST(err==KErrNone||err==KErrNotFound);
	CPageCache* pcache = CPageCache::NewLC(2);
	//creating file
	TheTest.Printf(_L("Creating file for the page pool"));
	err = testPage.Create(fs, KPageFilePath, EFileWrite);
	TEST2(err, KErrNone);
	testPage.Set(*pcache);
	TheTest.Printf(_L("-> File created -> Closing "));
	testPage.Close();
	TheTest.Printf(_L("-> Closed "));
	//opening file, file should be present after successful creation
	TheTest.Printf(_L("Opening file for the page pool"));
	err = testPage.Open(fs,KPageFilePath, EFileWrite);
	TEST2(err, KErrNone);
	testPage.Set(*pcache);
	TheTest.Printf(_L("-> File opened -> Closing "));
	testPage.Close();
	TheTest.Printf(_L("-> Closed "));
	//try to replace already existing file
	//file should exist after successful creation
	TheTest.Printf(_L("Replacing file for the page pool"));
	err = testPage.Replace(fs, KPageFilePath, EFileWrite);
	TEST2(err, KErrNone);
	testPage.Set(*pcache);
	TheTest.Printf(_L("-> File replaced -> Closing "));
	testPage.Close();
	TheTest.Printf(_L("-> Closed "));
	//try to create temp file with unique name
	TheTest.Printf(_L("Creating temp unique file "));
	err = testPage.Temp(fs, KPageFilePathOnly, tempPageFileName, EFileWrite);
	TEST2(err, KErrNone);
	testPage.Set(*pcache);
	TheTest.Printf(_L("-> File created -> Closing "));
	testPage.Close();
	TheTest.Printf(_L("-> Closed "));
	//if file was propertly created we should be able to open it
	TheTest.Printf(_L("Opening temp unique file "));
	err = testPage.Open(fs, tempPageFileName, EFileWrite);
	TEST2(err, KErrNone);
	TheTest.Printf(_L("-> File opened -> Releasing "));
	testPage.Release();
	TheTest.Printf(_L("-> Released "));
	
	//open and flush temp file
	RFilePagePool testPage2(*pcache);
	err = testPage2.Open(fs, tempPageFileName, EFileWrite);
	TEST2(err, KErrNone);
	err = testPage2.Flush();
	TEST2(err, KErrNone);
	TRAP(err, testPage2.FlushL());
	TEST2(err, KErrNone);
	
	RFile& file = const_cast<RFile&>(testPage2.File());
	TFileName testIsSameFile;
	file.FullName(testIsSameFile);
	TEST2( testIsSameFile.Compare(tempPageFileName), 0);
	testPage2.Detach();
	file.Close();
	
	//attach and detach file
	file.Open(fs, testIsSameFile, EFileWrite|EFileShareReadersOrWriters);
	testPage2.Attach(file);
	testPage2.Detach();
	file.Close();
	testPage2.Close();
	
	CleanupStack::PopAndDestroy(pcache);
	fs.Close();
	}

/**
 * Struct needed in test5()
 */
struct SCachePage
	{
	TCachePage iPage[1];
	TUint8 iData[KPoolPageSize];
	};
/**
 * Class specially created to test protected API from RFilePagePool
 */
class RFilePagePoolTestClass: public RFilePagePool
	{
public:
	void CallProtectedWriteL(SCachePage& page)
		{
		TPageChange change=page.iPage[0].iChange;
		WriteL(page.iPage[0].iRef,&page.iPage[1],change);
		}
	void CallProtectedReadL(SCachePage& page)
		{
		ReadL(page.iPage[0].iRef,&page.iPage[1]);
		}
	TPageRef CallProtectedExtendL(SCachePage& page)
		{
		ExtendL(&page.iPage[1],EPageReclaimable);
		return page.iPage[0].iRef;
		}
	};

/**
@SYMTestCaseID          PDS-STORE-CT-4010
@SYMTestCaseDesc	    RFilePagePool protected API tests
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for read and write and extend operations
@SYMTestExpectedResults Cache pages should be properly written and properly read from RFilePagePoolTestClass
@SYMDEF                 DEF135804
*/
LOCAL_C void test5L()
	{
	SCachePage page;
	page.iPage[0].iRef = 1;
	page.iPage[0].iChange=EPageNoChange;
	
	RFilePagePoolTestClass fpp;
	RFs fs;
	fs.Connect();
	fs.MkDirAll(KPageFilePathOnly);
	fs.Delete(KPageFilePath);
	CPageCache* pcache = CPageCache::NewLC(2);
	//creating file
	TheTest.Printf(_L("Creating file "));
	TInt err = fpp.Create(fs, KPageFilePath, EFileWrite);
	TEST2(err, KErrNone);
	fpp.Set(*pcache);
	TheTest.Printf(_L("-> File created -> Testing protected API "));
	TRAP(err, fpp.CallProtectedWriteL(page));
	TEST2(err, KErrNone);
	TheTest.Printf(_L("-> CallProtectedWriteL() done "));
	TRAP(err, fpp.CallProtectedReadL(page));
	TEST2(err, KErrNone);
	TheTest.Printf(_L("-> CallProtectedReadL() done "));
	TRAP(err, fpp.CallProtectedExtendL(page));
	TEST2(err, KErrNone);
	TheTest.Printf(_L("-> CallProtectedExtendL() done -> Closing"));
	fpp.Close();
	TheTest.Printf(_L("-> Closed "));
	CleanupStack::PopAndDestroy(pcache);
	fs.Close();
	}

const TInt KCachePages=16;

LOCAL_C void testallL(RStorePagePool& aPool)
	{
	TheTest.Start(_L("Connecting page pool"));
	aPool.Set(*CPageCache::NewLC(KCachePages));
	aPool.Create(*TheStore);
	TheTest.Next(_L("Basic operations"));
	test1L(aPool);
	TheTest.Next(_L("Large set TUint32"));
	test2L(aPool);
	aPool.Discard();
	aPool.ReclaimAllL();
	aPool.Close();
	TheTest.Next(_L("Tokens and streaming"));
	test3L(aPool);
	CleanupStack::PopAndDestroy();	//cache
	TheTest.Next(_L("PDS-STORE-CT-4009: RFilePagePool tests"));
	test4L();
	TheTest.Next(_L("PDS-STORE-CT-4010: RFilePagePool protected API tests"));
	test5L();
	TheTest.End();
	}

/**
@SYMTestCaseID          PDS-STORE-CT-4021
@SYMTestCaseDesc	    RStorePagePool protected API tests
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for different constructors
@SYMTestExpectedResults Objects must be created successfully
@SYMDEF                 DEF135804
*/
LOCAL_C void testconstructionL(CPBEncryptSet* aKey)
	{
	TheTest.Next(_L("PDS-STORE-CT-4021: RStorePagePool protected API tests"));
	CPageCache* pcache = CPageCache::NewLC(KCachePages);
	TStorePagePoolToken token;
	RStorePagePool poolcached(*pcache);
	poolcached.Create(*TheStore);
	test1L(poolcached);
	poolcached.Discard();
	poolcached.ReclaimAllL();
	poolcached.Close();	
	RStorePagePool poolstream(*TheStore);
	poolstream.Set(*pcache);
	test1L(poolstream);
	poolstream.Discard();
	poolstream.ReclaimAllL();
	poolstream.Close();
	RStorePagePool poolstreamtoken(*TheStore, token);
	poolstreamtoken.Set(*pcache);
	test1L(poolstreamtoken);	
	poolstreamtoken.Close();
	RSecureStorePagePool securepoolcached( *pcache, *aKey );
	securepoolcached.Create(*TheStore);
	test1L(securepoolcached);
	securepoolcached.Discard();
	securepoolcached.ReclaimAllL();
	securepoolcached.Close();
	
	
	CleanupStack::PopAndDestroy();
	
	}

LOCAL_C void doMainL()
	{
	TheTest.Start(_L("Store PagePool"));
	TParsePtrC parse(KFileLocationSpec);
	
	TheStore=CPermanentFileStore::ReplaceLC(TheFs,parse.NameAndExt(),EFileRead|EFileWrite);
	TheStore->SetTypeL(TheStore->Layout());
	RStorePagePool pool1;
	testallL(pool1);
	TheTest.Next(_L("Secure PagePool"));

	CPBEncryptSet* key = CPBEncryptSet::NewLC(_L("the password"));
	RSecureStorePagePool pool2(*key);
	testallL(pool2);


	testconstructionL(key);
	
	CleanupStack::PopAndDestroy(key);
	TheStore->CommitL();
	CleanupStack::PopAndDestroy();	// store
	}

//
// Prepare the test directory.
//
LOCAL_C void setupTestDirectory()
    {
	TInt r=TheFs.Connect();
	TEST2(r, KErrNone);
//
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse parse;
	parse.Set(drive.Name(), &KFileLocationSpec, NULL);
	
	r=TheFs.MkDir(parse.DriveAndPath());
	TEST(r==KErrNone||r==KErrAlreadyExists);
	r=TheFs.SetSessionPath(parse.DriveAndPath());
	TEST2(r, KErrNone);
	}

//
// Initialise the cleanup stack.
//
LOCAL_C void setupCleanup()
    {
	TheTrapCleanup=CTrapCleanup::New();
	TEST(TheTrapCleanup!=NULL);
	TRAPD(r,\
		{\
		for (TInt i=KTestCleanupStack;i>0;i--)\
			CleanupStack::PushL((TAny*)1);\
		TEST2(r, KErrNone);\
		CleanupStack::Pop(KTestCleanupStack);\
		});
	TEST2(r, KErrNone);
	}



LOCAL_C void DeleteTestFiles()
	{

	RFs fs;
	TInt err = fs.Connect();
	if(err == KErrNone)
		{
		CDir* dir;
		fs.GetDir(KPageFilePathOnly, KEntryAttNormal , ESortNone, dir);
		for(TInt i=0; i< dir->Count();i++)
			{
			CDir& rdir = *dir;
			TFileName tf (KPageFilePathOnly);
			tf.Append(rdir[i].iName);
			err = fs.Delete( tf );
			if (err != KErrNone)
				{
				RDebug::Print(_L("Error %d deleting file \"%S\".\n"), err, &(rdir[i].iName));
				}
			else
				RDebug::Print(_L("File \"%S\" removed.\n"), &(rdir[i].iName));
			}
		delete dir;
		err = fs.RmDir(KPageFilePathOnly);
		if (err != KErrNone)
			{
			RDebug::Print(_L("Error %d deleting folder \"%S\".\n"), err, &KPageFilePathOnly);
			}
		fs.Close();
		}
	else
		{
		RDebug::Print(_L("Error %d connecting file session.\n"), err);
		}
	}

LOCAL_C void DeleteDataFile(const TDesC& aFullName)
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

GLDEF_C TInt E32Main()
	{
	TheTest.Title();
	setupTestDirectory();
	setupCleanup();
	__UHEAP_MARK;
//
	TRAPD(r,doMainL());
	TEST2(r, KErrNone);

	//deletion of data files must be before call to .End() - DEF047652
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
		TParse parse;
		parse.Set(drive.Name(), &KFileLocationSpec, NULL);
		::DeleteDataFile(parse.FullName());
		::DeleteTestFiles();
		
	TheTest.End();
//
	__UHEAP_MARKEND;

	delete TheTrapCleanup;
	TheFs.Close();
	TheTest.Close();
	return 0;
	}

