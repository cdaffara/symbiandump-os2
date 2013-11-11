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
// @internalComponent
// 
//

#include <e32std.h>
#include <e32base.h>
#include <e32test.h>
#include <f32file.h>
#include <s32file.h>
#include "IniTemplate.h"
#include "IniParserImpl.h"

RTest test(_L("Ini Parser Unit Test"));
RFs TheRFs;

using namespace BSUL;

typedef void (*ClassFuncPtr8L) (void);

//utils class for testing
static void DoBasicTestL(ClassFuncPtr8L testFuncL, const TDesC& aTestDesc)
	{
	test.Next(aTestDesc);

	__UHEAP_MARK;
  	// find out the number of open handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

	(testFuncL)();

	// check that no handles have leaked
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;
	RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startProcessHandleCount == endProcessHandleCount);
	test(startThreadHandleCount  == endThreadHandleCount);

	__UHEAP_MARKEND;
	}

static void DoOOMTestL(ClassFuncPtr8L testFuncL, const TDesC& aTestDesc)
	{
	test.Next(aTestDesc);

	TInt err;
	TInt tryCount = 0;
	do
		{
		__UHEAP_MARK;
  		// find out the number of open handles
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

		__UHEAP_SETFAIL(RHeap::EDeterministic, ++tryCount);
		TRAP(err, testFuncL());
		__UHEAP_SETFAIL(RHeap::ENone, 0);

		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

		test(startProcessHandleCount == endProcessHandleCount);
		test(startThreadHandleCount  == endThreadHandleCount);

		__UHEAP_MARKEND;
		} while(err == KErrNoMemory);

 	test(err==KErrNone);
	test.Printf(_L("- succeeded at heap failure rate of %i\n"), tryCount);
	}

/**
@SYMTestCaseID	SYSLIB-BAFL-CT-1555
@SYMTestCaseDesc 	Test CIniKeyX
@SYMTestPriority 	High
@SYMTestActions  	Create, compare and delete CIniKeyX.
@SYMTestExpectedResults The test must not fail.
@SYMREQ PREQ505
*/
static void DoTest1L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-1555 "));
	//basic constructor
	CIniKey8* key1=CIniKey8::NewL(_L8("mykey"),_L8("myvalue"));
	CleanupStack::PushL(key1);
	CIniKey8* key2=CIniKey8::NewL(_L8("nykey"),_L8("myvalue"));
	CleanupStack::PushL(key2);

	//Compare key
	test(CIniKey8::CompareKey(*key1,*key2)<0);
	test(CIniKey8::CompareKey(*key2,*key1)>0);

	//test key name and value
	test(key1->KeyName().Compare(_L8("mykey"))==0);
	test(key1->KeyValue().Compare(_L8("myvalue"))==0);

	//SetKeyValue
	key1->SetKeyValue(_L8("newvalue"));
	test(key1->KeyValue().Compare(_L8("newvalue"))==0);

	CleanupStack::PopAndDestroy(2);
	}

/**
@SYMTestCaseID	SYSLIB-BAFL-CT-1556
@SYMTestCaseDesc 	Test CiniSectionX
@SYMTestPriority 	High
@SYMTestActions  	Test CiniSectionX by using compare section, InsertKeyL etc.
@SYMTestExpectedResults The test must not fail.
@SYMREQ PREQ505
*/
static void DoTest2L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-1556 "));
	//basic constructor
	CIniSection16* section1=CIniSection16::NewLC(_L("abc"));
	CIniSection16* section2=CIniSection16::NewLC(_L("cde"));
	CIniSection16* section3=CIniSection16::NewLC(_L("ABC"));

	//compare section
	test(CIniSection16::CompareSection(*section1,*section2)<0);
	test(CIniSection16::CompareSection(*section2,*section1)>0);
	//case sensitive
	test(CIniSection16::CompareSection(*section3,*section1)<0);

	//no key yet, this will create one
	section1->InsertKeyL(_L("newkey"),_L("XXXX"));

	//InsertKeyL(will leave with KErrAlreadyExists)
	TRAPD(err,section1->InsertKeyL(_L("newkey"),_L("keyvalue")));
	if (err==KErrNoMemory)
		User::LeaveNoMemory();
	test(err==KErrAlreadyExists);
	test(section1->KeyCount()==1);
	section1->InsertKeyL(_L("key2"),_L("keyval2"));
	test(section1->KeyCount()==2);

	//KeyValueL
	TPtrC value;
	TRAP(err,value.Set(section2->KeyValueL(_L("unknownkey"))));
	if (err==KErrNoMemory)
		User::LeaveNoMemory();
	test(err==KErrNotFound);
	value.Set(section1->KeyValueL(_L("newkey")));
	test(value.Compare(_L("XXXX"))==0);
	
	
	//FindKeyL(ordered list)
	CIniKey16* myKey=NULL;
	TRAP(err, myKey=section1->FindKeyL(_L("newkey")));
	if (err==KErrNoMemory)
		User::LeaveNoMemory();
	test(myKey!= NULL);
	TRAP(err,section1->FindKeyL(_L("tommy")));
	if (err==KErrNoMemory)
		User::LeaveNoMemory();
	test(err==KErrNotFound);

	//RemoveKeyL
	TRAP(err,section1->RemoveKeyL(_L("unknownkey")));
	if (err==KErrNoMemory)
		User::LeaveNoMemory();
	test(err==KErrNotFound);
	section1->RemoveKeyL(_L("key2"));
	test(section1->KeyCount()==1);

	CleanupStack::PopAndDestroy(3);
	}


static void DoTestL()
	{
	//basic test
	DoBasicTestL(&DoTest1L,_L("CIniKey basic test"));
	DoBasicTestL(&DoTest2L,_L("CIniSection basic test"));

	//oom test
	DoOOMTestL(&DoTest1L,_L("CIniKey OOM test"));
	DoOOMTestL(&DoTest2L,_L("CIniSection OOM test"));
	}


GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* trapCleanup=CTrapCleanup::New();
	test(TheRFs.Connect()==KErrNone);
	test.Start(_L("Ini Parser Unit Test"));

	TRAPD(error, DoTestL());
	test(error == KErrNone);


	TheRFs.Close();
	test.End();
	test.Close();
	delete trapCleanup;
	__UHEAP_MARKEND;
	return error;
	}

