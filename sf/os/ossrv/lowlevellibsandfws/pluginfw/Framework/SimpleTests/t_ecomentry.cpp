
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
#include <s32file.h>
#include <bautils.h>

#include "EComEntry.h"


RTest TheTest(_L("t_ecomentry.exe"));
_LIT(KTestFile,"Z:\\Test\\");
_LIT(KFile,"Z:\\Test\\Data\\EComTest.aif");
_LIT(KTestFileChange,"C:\\EComEntryChangedFile");

// functions for checking results
static void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
        TheTest(EFalse, aLine);
		}
	}

static  void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
        RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}


#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

class CEComEntryTest // codescanner::missingcclass
	{
public:
	static void TestEComEntryL();
	static void TestEComEntryOOML();
	};

void CEComEntryTest::TestEComEntryL()

	{
	TheTest.Next(_L("Create EComEntry Object....."));
	__UHEAP_MARK;


	TInt err=KErrNone;
	RFs fs;
	err = fs.Connect();
	CleanupClosePushL(fs);

	TEST(err==KErrNone);
	TEntry info,infoSecond;
	TTime time;
	time.UniversalTime();

	//get entry details for test file
	err=fs.Entry(KFile,info);
	TEST(err==KErrNone);

	//get entry details for existing file
	err=fs.Entry(KTestFile,infoSecond);
	TEST2(err,KErrNone);
	fs.Close();
	CleanupStack::Pop(&fs);

	//create ecomentry object

	CEComEntry* entry= CEComEntry::NewL(info.iName,info.iType[1],info.iType[2]);
	CleanupStack::PushL(entry);

	// not pushed onto cleanup stack as SetNameL will take ownership
	HBufC* buf= HBufC::NewL(25);
	*buf=KTestFileChange;

	delete entry->iName;
	entry->iName=buf;
	entry->SetModified(time);

	TEST(entry->GetName()==buf->Des());
	TEST(entry->GetModified()==time);

	CleanupStack::PopAndDestroy(entry);
	__UHEAP_MARKEND;
	}


// out of memory test
void CEComEntryTest::TestEComEntryOOML()
	{
	TheTest.Next(_L("EComEntry Out of Memory Test....."));
	TInt processHandlesS = 0;
	TInt threadHandlesS = 0;
	TInt processHandlesE = 0;
	TInt threadHandlesE = 0;
	RThread().HandleCount(processHandlesS, threadHandlesS);
	for(TInt count=1;;++count)
		{
		// Setting Heap failure for OOM test
		__UHEAP_SETFAIL(RHeap::EDeterministic, count);
		__UHEAP_MARK;

		TRAPD(err,TestEComEntryL());
		if(err == KErrNoMemory)
			{
			__UHEAP_MARKEND;
			}
		else if(err == KErrNone)
			{
			__UHEAP_MARKEND;
			RDebug::Print(_L("The test succeeded at heap failure rate=%d.\n"), count);
			break;
			}
		else
			{
			__UHEAP_MARKEND;
			TEST2(err, KErrNone);
			}
		__UHEAP_RESET;
		}
	__UHEAP_RESET;
	RThread().HandleCount(processHandlesE, threadHandlesE);
	TEST(processHandlesS == processHandlesE);
	TEST(threadHandlesS == threadHandlesE);

	}


GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	TheTest.Title();
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-ECOM-LEGACY-ECOMENTRY-0001 Starting tests... "));

	CTrapCleanup* cleanup = CTrapCleanup::New();

	TRAPD(err2, CEComEntryTest::TestEComEntryOOML());
	TheTest(err2==KErrNone);

	delete cleanup;


	TheTest.End();
	TheTest.Close();
	__UHEAP_MARKEND;
	return(0);
	}
