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
// This file contains the implementation of test classe for TScheduleState2. 
// 
//

#include <e32test.h>
#include <f32file.h>

#include "SCHINFO.H"
#include <schinfointernal.h>

_LIT(KTestName,	"TScheduleState2 Tests");
RTest	TheTest(KTestName);

//
//
//Test macroses and functions

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



/**
State accessor for the TScheduleState2 object under test.
*/
class TScheduleState2_StateAccessor
	{
public :
	void TestDefaultConstructor();
	void TestCopyConstructorOverloadedConstructor();
	void TestNameSetName();
	void TestDueTimeSetDueTime();
	void TestPersistsSetPersists();
	void TestEnabledSetEnabled();
	void TestOperatorEqual();
	};
	



/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0224
@SYMTestCaseDesc 			Check the default constructor works properly
@SYMTestPriority 			low
@SYMTestActions  			Creates an instance of TScheduleState2 and check its data has been 
							set to default values.
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/	
void TScheduleState2_StateAccessor::TestDefaultConstructor()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0224 "));
	TScheduleState2 state;
	
	//name should be ""
	TEST(state.iName == _L(""));
	
	//iDueTime UTC, O
	TEST(state.iDueTime.GetUtcTime() == TTime(0));
	
	//iFlags should be 0
	TEST(state.iFlags == 0);
	
	//iReserved 0
	TEST(state.iReserved == 0);	
	}
	


/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0225
@SYMTestCaseDesc 			Checks the copy constructor and the overloaded constructor
@SYMTestPriority 			low
@SYMTestActions  			Creates an instance of TScheduleState2 using the overloaded constructor
							and another instance of TScheduleState2 using the copy constructor, then compares
							their data and makes sure they are equal.
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/	
void TScheduleState2_StateAccessor::TestCopyConstructorOverloadedConstructor()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0225 Test constructors "));	
	const TName name(_L("test_state"));
	TDateTime date(2005, EMay, 15, 8, 55, 0, 0);
	TTime time(date);
	TTsTime dueTime(time, EFalse); //TTsTime is Home time
	
	TScheduleState2 state1(name, dueTime, ETrue, ETrue);
	TScheduleState2 state2(state1);

	TEST(state1.iName == state2.iName);
	TEST(state1.iDueTime.GetLocalTime() == state2.iDueTime.GetLocalTime());
	TEST(state1.iFlags == state2.iFlags);
	TEST(state1.iReserved == state2.iReserved);
	
	}



/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0226
@SYMTestCaseDesc 			Checks SetName() and Name() functions of TScheduleState2
@SYMTestPriority 			low
@SYMTestActions  			Creates an instance of TScheduleState2, calls SetName() and Name()
							and Name() returns the same value as the one set initially.
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/
void TScheduleState2_StateAccessor::TestNameSetName()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0226 "));
	TScheduleState2 state;
	const TName name(_L("name"));
	state.SetName(name);
	
	TEST(state.Name() == name);
		
	}

/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0227
@SYMTestCaseDesc 			Checks SetDueTime() and DueTime()
@SYMTestPriority 			low
@SYMTestActions  			Creates an instance of TScheduleState2, sets its due time and checks
							the returned time is equel to one set initially.
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/
void TScheduleState2_StateAccessor::TestDueTimeSetDueTime()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0227 Test set and get due time APIs "));	

	TScheduleState2 state;
	
	TDateTime date(2005, EMay, 15, 8, 55, 0, 0);
	TTime time(date);
	TTsTime dueTime(time, EFalse); //TTsTime is Home time
	
	state.SetDueTime(dueTime);	
	TEST(time == state.DueTime().GetLocalTime());
	}
	


/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0228
@SYMTestCaseDesc 			Checks SetPersists() and Persists() functions 
@SYMTestPriority 			low
@SYMTestActions  			Creates an instance of TScheduleState2, calls SetPersists then Persists 
							and checks the returned value is equal to the one set initially.
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/
void TScheduleState2_StateAccessor::TestPersistsSetPersists()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0228 "));
	TScheduleState2 state;
	
	//set Persists to Etrue
	state.SetPersists(ETrue);
	TEST(state.Persists());
	
	//set Persists to EFalse
	state.SetPersists(EFalse);
	TEST(!state.Persists());	
	}
	
	

/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0229
@SYMTestCaseDesc 			Checks Enabled() and SetEnabled() functions 
@SYMTestPriority 			low
@SYMTestActions  			Creates an instance TScheduleState2, calls SetEnabled() then Enabled()
							and checks the returned value is equal to the one set initially.
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/
void TScheduleState2_StateAccessor::TestEnabledSetEnabled()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0229 "));
	TScheduleState2 state;
	
	//ETrue
	state.SetEnabled(ETrue);
	TEST(state.Enabled());
	
	//EFlase
	state.SetEnabled(EFalse);
	TEST(!state.Enabled());
			
	}
	
	

/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0230
@SYMTestCaseDesc 			Check the operator = works properly
@SYMTestPriority 			low
@SYMTestActions  			Creates an instance of 	TScheduleState2 using the overloaded constructor,
							then creates another instance of TScheduleState2 and assigns it the first instance 
							of TScheduleState2. The tests make sure both instances are equal.	
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/	
void TScheduleState2_StateAccessor::TestOperatorEqual()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0230 Test overloaded equals operator "));	
	
	const TName name(_L("name"));
	//set the current utc time to 8.55am, 15 May 2005 -Daylight savings apply on this date
	TDateTime date(2005, EMay, 15, 8, 55, 0, 0);
	TTime time(date);
	TTsTime dueTime(time, EFalse); //TTsTime is Home time
	
	TScheduleState2 state1(name, dueTime, ETrue, ETrue);
	TScheduleState2 state2 = state1;
	
	TEST(state1.iName == state2.iName);
	TEST(state1.iDueTime.GetLocalTime() == state2.iDueTime.GetLocalTime() );
	TEST(state1.iFlags == state2.iFlags);
	TEST(state1.iReserved == state2.iReserved);
	}


/**
Runs all the tests.
*/
static void RunTestsL()
	{
	TheTest.Start(_L("=== Start TScheduleState2 tests \n"));
	TScheduleState2_StateAccessor* entry = new (ELeave) TScheduleState2_StateAccessor;
	CleanupStack::PushL(entry);
	entry->TestDefaultConstructor();
	entry->TestCopyConstructorOverloadedConstructor();
	entry->TestNameSetName();
	entry->TestDueTimeSetDueTime();
	entry->TestPersistsSetPersists();
	entry->TestEnabledSetEnabled();
	entry->TestOperatorEqual();
	CleanupStack::PopAndDestroy();
	}

	
	
//***********************************************************************************
GLDEF_C TInt E32Main()
 {
	CTrapCleanup* tc = CTrapCleanup::New();
	TEST(tc != NULL);
	
	__UHEAP_MARK;

	TInt err;
	TheTest.Title();

	TRAP(err, ::RunTestsL())
	TEST2(err, KErrNone);
	
	TheTest.End();
	TheTest.Close();

	__UHEAP_MARKEND;
	
	delete tc;
	
	return(KErrNone);
	
	}

	
	
