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

#include <s32mem.h>
#include <s32file.h>
#include <e32test.h>
#include <e32base.h>
#include <e32cons.h>

static	RTest				TheTest(_L("t_storbench"));
static	CTrapCleanup*		TheTrapCleanup;
static	RFs					TheFs;

TFileName TheDirectStoreFilePath;
TFileName ThePermStoreFilePath;
TFileName TheStreamsFilePath;

static void DeleteDataFiles();

const TInt KTestCleanupStack=0x20;

class TElement
	{
public :
	TElement();
public :
	TBuf<256> iData;
	};

TElement::TElement()
	{
	_LIT(KTextBase,"BASE");
	iData = KTextBase;
	}

#include "T_BMStreams.inl"
#include "T_BMStreamStore.inl"
#include "T_BMDirectFileStore.inl"
#include "T_BMPermFileStore.inl"

//Tests macroses and functions.
//If (!aValue) then the test will be panicked, the test data files will be deleted.
static void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		DeleteDataFiles();
		TheTest(EFalse, aLine);
		}
	}
//If (aValue != aExpected) then the test will be panicked, the test data files will be deleted.
static void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		DeleteDataFiles();
		TheTest(EFalse, aLine);
		}
	}
//Use these to test conditions.
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

LOCAL_C void PerformanceTesting()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-STORE-LEGACY-STOREBENCHMARK-1204 Performance Testing for STORE Component "));
	// Call the performance tests
	TRAPD(err, doStreamingL());
	TEST2(err, KErrNone);

	TRAP(err, doStoreStreamsL());
	TEST2(err, KErrNone);

	TRAP(err, doDirectFileStoresL());
	TEST2(err, KErrNone);

	TRAP(err, doPermanentFileStoreL());
	TEST2(err, KErrNone);
	}

// Prepare the test directory.
LOCAL_C void setupTestDirectory()
    {
	TInt err=TheFs.Connect();
	TEST2(err, KErrNone);

	TPtrC testDir=_L("\\STOR-TST\\");
	err=TheFs.MkDir(testDir);
	TEST(err == KErrNone || err == KErrAlreadyExists);

	err=TheFs.SetSessionPath(testDir);
	TEST2(err, KErrNone);
	}

// Initialise the cleanup stack.
LOCAL_C void setupCleanup()
    {
	TheTrapCleanup=CTrapCleanup::New();
	TEST(TheTrapCleanup != NULL);
	TRAPD(err,\
		{\
		for (TInt i=KTestCleanupStack;i>0;i--)\
			CleanupStack::PushL((TAny*)1);\
		TEST(err==KErrNone);\
		CleanupStack::Pop(KTestCleanupStack);\
		});
	TEST2(err, KErrNone);
	}

static void DeleteDataFile(const TDesC& aFullName)
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
static void DeleteDataFiles()
	{
	DeleteDataFile(TheDirectStoreFilePath);
	DeleteDataFile(ThePermStoreFilePath);
	DeleteDataFile(TheStreamsFilePath);
	}

// Test the stream and stores
GLDEF_C TInt E32Main()
    {
	__UHEAP_MARK;
	TheTest.Title();
	setupTestDirectory();
	setupCleanup();

	TRAPD(err, PerformanceTesting());
	TEST2(err, KErrNone);

	DeleteDataFiles();

	TheTest.End();
	delete TheTrapCleanup;
	TheFs.Close();
	TheTest.Close();
	__UHEAP_MARKEND;

	return 0;
	}



