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

#include <s32mem.h>
#include <s32file.h>
#include <e32test.h>

const TInt KTestCleanupStack=0x20;

// This is a path specification and should not be used as is
_LIT(KTestDirectFileName, "Z:\\STOR-TST\\T_STRDV.DAT");	//deletion of data files must be before call to .End() - DEF047652
_LIT(KTestPermanentFileName, "Z:\\STOR-TST\\T_STRPV.DAT");	//deletion of data files must be before call to .End() - DEF047652

// The following data will be written to the beginning of the store file in the length of
// sizeof(TCheckedUid) to corrupt it. Its length should be more than that number.
_LIT8(KTestJunkData, "Junk data for corrupting the store file");

LOCAL_D RTest Test(_L("t_storverify"));
LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D RFs TheFs;
LOCAL_D CFileStore* TheStore;
LOCAL_D RStoreWriteStream TheSink;

LOCAL_C void DeleteDataFile(const TDesC& aFullName);

/**
@SYMTestCaseID          SYSLIB-STORE-CT-3350
@SYMTestCaseDesc	    Direct file store verification test
@SYMTestPriority 	    High
@SYMTestActions  	    Open a corrupted direct store file
@SYMTestExpectedResults the function called leaves with KErrNotSupported
@SYMDEF                 DEF100757
*/
LOCAL_C void DirectFileL()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-3350 Creating Direct File Store "));
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse parse;
	parse.Set(drive.Name(), &KTestDirectFileName, NULL);
	
	// create a direct file store
	TheStore=CDirectFileStore::ReplaceLC(TheFs,parse.FullName(),EFileWrite);
	TheStore->SetTypeL(TheStore->Layout());
	TInt8 val=100;
	TStreamId id=TheSink.CreateL(*TheStore);
	TheSink<<val;
	TheSink.Close();
	TheStore->SetRootL(id);
	TheStore->CommitL();
	CleanupStack::PopAndDestroy(TheStore);

	// corrupt the store file
	RFile file;

	User::LeaveIfError(file.Open(TheFs,parse.FullName(),EFileWrite));
	CleanupClosePushL(file);
	User::LeaveIfError(file.Write(KTestJunkData,sizeof(TCheckedUid)));
	CleanupStack::PopAndDestroy(&file);

	// the CDirectFileStore::OpenLC should leave if passed the corrupted store file name
	TheStore = NULL;
	TRAPD(r, \
			{\
			TheStore=CDirectFileStore::OpenLC(TheFs,parse.FullName(),EFileRead);\
			CleanupStack::PopAndDestroy(TheStore); \
			}\
		);
	Test(r==KErrNotSupported);
	}

/**
@SYMTestCaseID          SYSLIB-STORE-CT-3351
@SYMTestCaseDesc	    Permanent file store verification test
@SYMTestPriority 	    High
@SYMTestActions  	    Open a corrupted permanent store file
@SYMTestExpectedResults the function called leaves with KErrNotSupported
@SYMDEF                 DEF100757
*/
LOCAL_C void PermFileL()
	{
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-STORE-CT-3351 Creating Permanent File Store "));

	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse parse;
	parse.Set(drive.Name(), &KTestPermanentFileName, NULL);
	
	// create a permanent file store
	TheStore=CPermanentFileStore::ReplaceLC(TheFs,parse.FullName(),EFileWrite);
	TheStore->SetTypeL(TheStore->Layout());
	TInt8 val=100;
	TStreamId id=TheSink.CreateL(*TheStore);
	TheSink<<val;
	TheSink.Close();
	TheStore->SetRootL(id);
	TheStore->CommitL();
	CleanupStack::PopAndDestroy(TheStore);

	// corrupt the new data file
	RFile file;
	User::LeaveIfError(file.Open(TheFs,parse.FullName(),EFileWrite));
	CleanupClosePushL(file);
	User::LeaveIfError(file.Write(KTestJunkData,sizeof(TCheckedUid)));
	CleanupStack::PopAndDestroy(&file);

	// the CPermanentFileStore::OpenLC should leave if passed the corrupted store file name
	TheStore = NULL;
	TRAPD(r, \
			{\
			TheStore=CPermanentFileStore::OpenLC(TheFs,parse.FullName(),EFileRead);\
			CleanupStack::PopAndDestroy(TheStore); \
			}\
		);
	Test(r==KErrNotSupported);
	}

LOCAL_C void doMainL()
	{
	DirectFileL();
	PermFileL();
	}

//
// Prepare the test directory.
//
LOCAL_C void setupTestDirectory()
    {
	TInt r=TheFs.Connect();
	Test(r==KErrNone);
	TPtrC testDir=_L("\\STOR-TST\\");
	r=TheFs.MkDir(testDir);
	Test(r==KErrNone||r==KErrAlreadyExists);
	r=TheFs.SetSessionPath(testDir);
	Test(r==KErrNone);
	}

//
// Initialise the cleanup stack.
//
LOCAL_C void setupCleanup()
    {
	TheTrapCleanup=CTrapCleanup::New();
	Test(TheTrapCleanup!=NULL);
	TRAPD(r,\
		{\
		for (TInt i=KTestCleanupStack;i>0;i--)\
			CleanupStack::PushL((TAny*)1);\
		Test(r==KErrNone);\
		CleanupStack::Pop(KTestCleanupStack);\
		});
	Test(r==KErrNone);
	}

//
// Delete a specified file
//
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

//
// Test the store verification.
//
GLDEF_C TInt E32Main()
    {
	Test.Title();
	setupTestDirectory();
	setupCleanup();
	__UHEAP_MARK;
	Test.Start(_L("Store Verification Test"));

	TRAPD(r,doMainL());

	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse parse;
	parse.Set(drive.Name(), &KTestDirectFileName, NULL);
	::DeleteDataFile(parse.FullName());
	
	parse.Set(drive.Name(), &KTestPermanentFileName, NULL);
	::DeleteDataFile(parse.FullName());

	Test(r==KErrNone);

	Test.End();

	__UHEAP_MARKEND;

	delete TheTrapCleanup;
	TheFs.Close();
	Test.Close();
	return 0;
    }

