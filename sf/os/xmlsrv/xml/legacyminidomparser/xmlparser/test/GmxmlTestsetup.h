// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __GMXMLTESTSETUP_H__
#define __GMXMLTESTSETUP_H__

#include <e32base.h>
#include "GmxmlTestUtils.h"

// The following must be defined by the test exe
GLREF_D RTest test;
LOCAL_C void doTestsL();

// Globals
CTrapCleanup* theCleanup = NULL;

TInt TestNum = 0;
class CGenericTestUtils;
CGenericTestUtils* theUtils = NULL;

// Constants

// Useful macros
TInt theError;


//**********************************
// CGenericTestUtils
//**********************************

class CGenericTestUtils : public CTestUtils
	{
public:
	static CGenericTestUtils* NewL(RTest& aTest);
	CGenericTestUtils(RTest& aTest);
	void Panic(TInt aPanic);
	//
public:
	static void CreateDataFilesL(const TDesC& aFilename, const TDesC& aMtmName, TUid aUid);
	static TBool WaitForKeyL(TTimeIntervalMicroSeconds32 aDelay, TKeyCode* aKeyCode = NULL);
	static void Start(const TDesC& aDes);
	static void Next(const TDesC& aDes);
	static void Complete();
	static void End();
	};

CGenericTestUtils* CGenericTestUtils::NewL(RTest& aTest)
	{
	CGenericTestUtils* self = new(ELeave)CGenericTestUtils(aTest);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

CGenericTestUtils::CGenericTestUtils(RTest& aTest)
: CTestUtils(aTest)
	{
	}

void CGenericTestUtils::Panic(TInt)
	{
	test(EFalse);
	}

void CGenericTestUtils::Start(const TDesC& aDes)
	{
	test.Start(aDes);
	theUtils->TestStart(++TestNum, aDes);
	}

void CGenericTestUtils::Next(const TDesC& aDes)
	{
	test.Next(aDes);
	theUtils->TestStart(++TestNum, aDes);
	}

void CGenericTestUtils::Complete()
	{
	theUtils->TestFinish(TestNum, KErrNone);
	}

void CGenericTestUtils::End()
	{
	test.End();
	}

//**********************************
// Global
//**********************************

LOCAL_C void SetupL()
	{
//	CActiveScheduler::Install(theScheduler = new(ELeave)CTestScheduler);

	// Create test class
	theUtils = CGenericTestUtils::NewL(test);

//	test.Printf(_L("Cleaning Message Folder\n"));
//	CGenericTestUtils::CleanMessageFolderL();
//	CGenericTestUtils::SetDriveL(EDriveC);
//	CGenericTestUtils::SetDebugL(ETrue);

	}

LOCAL_C void Close(TInt aRet)
	{
	if (theUtils)
		{
		if (!aRet)
			theUtils->TestHarnessCompleted();
		else
			theUtils->TestHarnessFailed(aRet);
		}
	delete theUtils;
	}

LOCAL_C void doMainL()
	{
	SetupL();		
	doTestsL();
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	theCleanup=CTrapCleanup::New();
	TRAPD(ret,doMainL());		
	Close(ret);
	delete theCleanup;	
	test.Console()->SetPos(0, 13);
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}

#endif
