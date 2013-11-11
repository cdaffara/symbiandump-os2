// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "stabilitytestclass.h"
#include "testdocuments.h"

static RTest test(_L("t_wbxmlsyncmlparserstabilitytests"));


static void DeleteTestDirL()
	{
	RFs fsSession;
	TInt err = fsSession.Connect();
	if(err == KErrNone)
		{
		CleanupClosePushL(fsSession);
		RDebug::Print(_L("Deleting \"%S\" directory.\n"), &KOpaqueDirectory);
		CFileMan* fileMan = CFileMan::NewL(fsSession);
		CleanupStack::PushL(fileMan);
		TInt err = fileMan->RmDir(KOpaqueDirectory);
		if(err == KErrPathNotFound)
			{
			err = KErrNone;
			}
		User::LeaveIfError(err);
		CleanupStack::PopAndDestroy(fileMan);
		CleanupStack::PopAndDestroy(&fsSession);
		}
	else
		{
		RDebug::Print(_L("Error %d connecting file session. Directory: %S.\n"), err, &KOpaqueDirectory);
		}
	}


/**
@SYMTestCaseID 		 		SYSLIB-XML-CT-3723
@SYMTestCaseDesc		    Parsing SyncML documents.
@SYMTestPriority 		    Medium
@SYMTestActions  		    Parsing syncml documents in the specified directory.
@SYMTestExpectedResults 	Parser should be able to cope with parsing the documents.
@SYMREQ 		 		 	REQ0000
*/
static void SyncmlTestsL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-CT-3723 SyncmlTestsL tests... "));

	__UHEAP_MARK;	
	CStabilityTestClass* syncml1_1TestWithOpaque = CStabilityTestClass::NewL(test, EFalse, KMaxChunkSize);
	CleanupStack::PushL(syncml1_1TestWithOpaque);
	syncml1_1TestWithOpaque->StabilityTestL(KSyncML1_1Directory(), KWbxmlExt(), &CStabilityTestClass::TestWholeL);
	syncml1_1TestWithOpaque->StabilityTestL(KSyncML1_1Directory(), KWbxmlExt(), &CStabilityTestClass::TestChunkL);
	CleanupStack::PopAndDestroy(syncml1_1TestWithOpaque);

	CStabilityTestClass* syncml1_1TestMax = CStabilityTestClass::NewL(test, EFalse, KMaxChunkSize);
	CleanupStack::PushL(syncml1_1TestMax);
	syncml1_1TestMax->StabilityTestL(KOpaqueDirectory(), 	 KWbxmlExt(), &CStabilityTestClass::TestWholeL);
	syncml1_1TestMax->StabilityTestL(KOpaqueDirectory(), 	 KWbxmlExt(), &CStabilityTestClass::TestChunkL);
	CleanupStack::PopAndDestroy(syncml1_1TestMax);

	//::DeleteTestDirL();
	CStabilityTestClass* syncml1_2TestWithOpaque = CStabilityTestClass::NewL(test, EFalse, KMaxChunkSize);
	CleanupStack::PushL(syncml1_2TestWithOpaque);
	syncml1_2TestWithOpaque->StabilityTestL(KSyncML1_2Directory(), KWbxmlExt(), &CStabilityTestClass::TestWholeL);
	syncml1_2TestWithOpaque->StabilityTestL(KSyncML1_2Directory(), KWbxmlExt(), &CStabilityTestClass::TestChunkL);
	CleanupStack::PopAndDestroy(syncml1_2TestWithOpaque);

	CStabilityTestClass* syncml1_2TestMax = CStabilityTestClass::NewL(test, EFalse, KMaxChunkSize);
	CleanupStack::PushL(syncml1_2TestMax);
	syncml1_2TestMax->StabilityTestL(KOpaqueDirectory(), 	 KWbxmlExt(), &CStabilityTestClass::TestWholeL);
	syncml1_2TestMax->StabilityTestL(KOpaqueDirectory(), 	 KWbxmlExt(), &CStabilityTestClass::TestChunkL);
	CleanupStack::PopAndDestroy(syncml1_2TestMax);

	//::DeleteTestDirL();

	__UHEAP_MARKEND;
	}


static void MainL()
	{
	SyncmlTestsL();

	}


TInt E32Main()
	{
	__UHEAP_MARK;
	test.Title();
	test.Start(_L("initialising"));

	CTrapCleanup* c=CTrapCleanup::New();

	// start the loader
	RFs fs;
	test (fs.Connect()==KErrNone);
	fs.Close();

	test (c!=0);
	TRAPD(r,MainL());
	test (r==KErrNone);
	delete c;
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return KErrNone;
	}
