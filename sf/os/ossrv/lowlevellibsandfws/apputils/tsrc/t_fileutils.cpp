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
// Tests EikFileUtils::SortByTable() function.
// 
//

#include <e32test.h>
#include <f32file.h>
#include <bautils.h>

LOCAL_D RTest test(_L("t_fileutils.exe"));

const TInt KSortTableUid1=0x3f631;
const TInt KSortTableUid2=0x276a9;

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0486
@SYMTestCaseDesc        Tests for the sorting of files containing an array of UIDs.
@SYMTestPriority        High
@SYMTestActions         Tests BaflUtils::SortByTable() function.
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void TestSortByTableL()
	{
	CDir* theFileList;
	TInt theAttribs=KEntryAttMatchMask|KEntryAttAllowUid;
	TFileName theCurrentPath = _L("Z:\\system\\documents\\baflsortbytabletest\\");

    RFs fileSession;
    User::LeaveIfError(fileSession.Connect());
	CleanupClosePushL(fileSession);

	TInt ret=fileSession.GetDir(theCurrentPath,theAttribs,ESortByName,theFileList);
	test(ret==KErrNone);
	CleanupStack::PushL(theFileList);

	CBaflFileSortTable*theTable = new (ELeave) CBaflFileSortTable();
	CleanupStack::PushL(theTable);

	TInt insertErr;

	TRAP(insertErr,theTable->InsertL(0,TUid::Uid(KSortTableUid1)));
	test(insertErr==KErrNone);

	TRAP(insertErr,theTable->InsertL(1,TUid::Uid(KSortTableUid2)));
	test(insertErr==KErrNone);

	test.Printf(_L("\nGet the names of the files sorted by alphabetical order\n"));
	for(TInt theIndex1 = 0; theIndex1 < theFileList->Count(); theIndex1++)
		{
		TEntry	theEntry = (*theFileList)[theIndex1];
		TUid	theUid1		= theEntry[2];
		test.Printf(_L("The %d file in list %S, Uid = 0x%x\n"), theIndex1 + 1, &theEntry.iName, theUid1.iUid);
		switch(theIndex1)
			{
		case 0:
			test(theEntry.iName == _L("addcert.rsc"));
			break;
		case 1:
			test(theEntry.iName == _L("help.rsc"));
			break;
		case 2:
			test(theEntry.iName == _L("msgeditor.rsc"));
			break;
		case 3:
			test(theEntry.iName == _L("smlprogress.rsc"));
			break;
		default:
			break;
			}
		}

	TInt sortErr = BaflUtils::SortByTable(*theFileList, theTable);
	test(sortErr==KErrNone);

	test.Printf(_L("\nGet the names of the files sorted by the Uid's table\n"));
	for(TInt theIndex = 0; theIndex < theFileList->Count(); theIndex++)
		{
		TEntry	theEntry = (*theFileList)[theIndex];
		TUid	theUid1		= theEntry[2];

		test.Printf(_L("The %d file in list %S, Uid = 0x%x\n"), theIndex + 1, &theEntry.iName, theUid1.iUid);
		switch(theIndex)
			{
		case 0:
			test(theEntry.iName == _L("msgeditor.rsc"));
			break;
		case 1:
			test(theEntry.iName == _L("help.rsc"));
			break;
		case 2:
			test(theEntry.iName == _L("addcert.rsc"));
			break;
		case 3:
			test(theEntry.iName == _L("smlprogress.rsc"));
			break;
		default:
			break;
			}
		}
	CleanupStack::PopAndDestroy(3, &fileSession);
	}


void DoTestsL()
	{
	__UHEAP_MARK;

	TRAPD(testsortErr,TestSortByTableL());
	test(testsortErr==KErrNone);

	__UHEAP_MARKEND;
	}


GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;

	test.Title();
	test.Start(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0486 SORT BY TABLE TEST "));

	// get clean-up stack
	CTrapCleanup* cleanup = CTrapCleanup::New();

	test (cleanup!=0);

	TRAPD(err,DoTestsL());
	test(err==KErrNone);

	delete cleanup;
	test.End();
	test.Close();

	__UHEAP_MARKEND;
	return(KErrNone);
	}
