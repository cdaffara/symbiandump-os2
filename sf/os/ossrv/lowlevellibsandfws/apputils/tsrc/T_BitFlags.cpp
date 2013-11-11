// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TBITFLAGS.CPP
// 
//

#include "T_BitFlags.h"

// System includes
#include <e32test.h>

// Literal constants
_LIT(KTestTitle, "TestBitFlags");

RTest TheTest(KTestTitle);


void TestBitFlags::TestSetAll()
	{
	iTestFlags.SetAll();
	}

void TestBitFlags::TestClearAll()
	{
	iTestFlags.ClearAll();
	}

void TestBitFlags::TestSetL(TInt aColor)
	{
	iTestFlags.Set(aColor);
	if	(!(iTestFlags.IsSet(aColor)))
		User::Leave(KErrGeneral);
	}

void TestBitFlags::TestClearL(TInt aColor)
	{
	iTestFlags.Clear(aColor);
	if	(iTestFlags.IsSet(aColor))
		User::Leave(KErrGeneral);
	}

void TestBitFlags::TestAssign(TInt aColor, TBool aSetOrClear)
	{
	iTestFlags.Assign(aColor, aSetOrClear);
	}

void TestBitFlags::TestToggleL(TInt aColor)
	{
	TBool isSet = iTestFlags.IsSet(aColor);
	iTestFlags.Toggle(aColor);
	if	(isSet == iTestFlags.IsSet(aColor))
		User::Leave(KErrGeneral);
	}

TBool TestBitFlags::TestOperator1(TInt aColor) //testing operator []
	{
	return iTestFlags.operator[](aColor);
	}

void TestBitFlags::TestOperator2() // test operator =
	{
	iFlagA.Set(TestBitFlags::EBlue);
	iFlagB = iFlagA;
	TheTest(iFlagB == iFlagA);

	iFlagB.Set(TestBitFlags::ERed);
	TheTest(!(iFlagB == iFlagA));
	}

void TestBitFlags::TestOperator3() //test operator ==
	{
	iFlagA.Set(TestBitFlags::EBlue);
	iFlagB.Set(TestBitFlags::EBlue);
	TheTest(iFlagA == iFlagB);

	iFlagB.Set(TestBitFlags::ERed);
	TheTest(!(iFlagB == iFlagA));
	}

TBool TestBitFlags::TestIsSet(TInt aColor)
	{
	return iTestFlags.IsSet(aColor);
	}

TBool TestBitFlags::TestIsClear(TInt aColor)
	{
	return iTestFlags.IsClear(aColor);
	}

TInt TestBitFlags::TestValue()
	{
	return iTestFlags.Value();
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0468
@SYMTestCaseDesc        Tests the behaviour of bits
@SYMTestPriority        High
@SYMTestActions         Tests for bits by setting and clearing
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void DoTest1()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0468 Pure Set and Clear and related functions only "));
	TestBitFlags test;
	TBool checkTestFlags;
	TInt  checkValue;

	TRAPD(errCode, test.TestSetL(TestBitFlags::EBlue));
	TheTest(errCode==KErrNone);

	checkTestFlags = test.TestIsSet(TestBitFlags::EBlue); //expect 1
	checkValue = test.TestValue(); //expect 1 (hex)
	TheTest(checkTestFlags == 1);
	TheTest(checkValue == 1);

	TRAP(errCode, test.TestSetL(TestBitFlags::EGreen));
	TheTest(errCode==KErrNone);
	checkTestFlags = test.TestIsSet(TestBitFlags::EGreen); //expect 4
	TheTest(checkTestFlags == 4);
	checkTestFlags = test.TestIsClear(TestBitFlags::EGreen); //expect 0 - means it hasnt been cleared
	checkValue = test.TestValue(); //expect 5 (hex)
	TheTest(checkTestFlags == 0);
	TheTest(checkValue == 5);

	TRAP(errCode, test.TestSetL(TestBitFlags::EYellow));
	TheTest(errCode==KErrNone);
	checkTestFlags = test.TestIsSet(TestBitFlags::EYellow); //expect 8
	checkValue = test.TestValue(); //expect d (hex)
	TheTest(checkTestFlags == 8);
	TheTest(checkValue == 13);

	test.TestClearL(TestBitFlags::EGreen);
	checkTestFlags = test.TestIsClear(TestBitFlags::EGreen); //expect 1
	checkValue = test.TestValue(); //expect 9 (hex)
	TheTest(checkTestFlags == 1);
	TheTest(checkValue == 9);

	checkTestFlags = test.TestIsClear(TestBitFlags::ERed); //expect 1 - means it has been cleared
	checkValue = test.TestValue(); //expect 9 (hex)
	TheTest(checkTestFlags == 1);
	TheTest(checkValue == 9);

	test.TestSetAll();
	checkTestFlags = test.TestIsSet(TestBitFlags::EBlue);
	TheTest(checkTestFlags == 1);
	checkTestFlags = test.TestIsSet(TestBitFlags::ERed);
	TheTest(checkTestFlags == 2);
	checkTestFlags = test.TestIsSet(TestBitFlags::EGreen);
	TheTest(checkTestFlags == 4);
	checkTestFlags = test.TestIsSet(TestBitFlags::EYellow);
	TheTest(checkTestFlags == 8);
	checkTestFlags = test.TestIsSet(TestBitFlags::EPurple);
	TheTest(checkTestFlags == 16);
	checkTestFlags = test.TestIsSet(TestBitFlags::EBlack);
	TheTest(checkTestFlags == 32);
	checkTestFlags = test.TestIsSet(TestBitFlags::EWhite);
	TheTest(checkTestFlags == 64);
	checkTestFlags = test.TestIsSet(TestBitFlags::EGrey);
	TheTest(checkTestFlags == 128);
	checkValue = test.TestValue(); //expect 0xffffffff (hex)
	TheTest(checkValue == -1);


	test.TestClearL(TestBitFlags::EBlue);
	checkTestFlags = test.TestIsClear(TestBitFlags::EBlue);
	TheTest(checkTestFlags == 1);
	test.TestClearL(TestBitFlags::ERed);
	checkTestFlags = test.TestIsClear(TestBitFlags::ERed);
	TheTest(checkTestFlags == 1);
	test.TestClearL(TestBitFlags::EGreen);
	checkTestFlags = test.TestIsClear(TestBitFlags::EGreen);
	TheTest(checkTestFlags == 1);
	test.TestClearL(TestBitFlags::EYellow);
	checkTestFlags = test.TestIsClear(TestBitFlags::EYellow);
	TheTest(checkTestFlags == 1);
	test.TestClearL(TestBitFlags::EPurple);
	checkTestFlags = test.TestIsClear(TestBitFlags::EPurple);
	TheTest(checkTestFlags == 1);
	test.TestClearL(TestBitFlags::EBlack);
	checkTestFlags = test.TestIsClear(TestBitFlags::EBlack);
	TheTest(checkTestFlags == 1);
	test.TestClearL(TestBitFlags::EWhite);
	checkTestFlags = test.TestIsClear(TestBitFlags::EWhite);
	TheTest(checkTestFlags == 1);
	test.TestClearL(TestBitFlags::EGrey);
	checkTestFlags = test.TestIsClear(TestBitFlags::EGrey);
	TheTest(checkTestFlags == 1);
	checkValue = test.TestValue(); //expect 0xffffff00
	TheTest(checkValue == -256);

	test.TestClearAll();
	checkValue = test.TestValue();
	TheTest(checkValue ==0);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0469
@SYMTestCaseDesc        Tests the behaviour of bits
@SYMTestPriority        High
@SYMTestActions         Tests for bits by reading them back (assign function)
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void DoTest2()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0469 Assign function "));
	TestBitFlags test;
	TBool checkTestFlags;
	TInt checkValue;

	test.TestAssign(TestBitFlags::ERed, 1);
	checkTestFlags = test.TestIsSet(TestBitFlags::ERed); //expect 2
	checkValue = test.TestValue(); //expect 2
	TheTest(checkTestFlags == 2);
	TheTest(checkValue == 2);

	test.TestAssign(TestBitFlags::EWhite, 1);
	checkTestFlags = test.TestIsSet(TestBitFlags::EWhite); //expect 64
	checkValue = test.TestValue(); //expect 66
	TheTest(checkTestFlags == 64);
	TheTest(checkValue == 66);

	test.TestAssign(TestBitFlags::ERed, 0);
	checkTestFlags = test.TestIsClear(TestBitFlags::ERed); //expect 1
	checkValue = test.TestValue(); //expect 64
	TheTest(checkTestFlags == 1);
	TheTest(checkValue == 64);

	test.TestAssign(TestBitFlags::ERed, 3); //test anything other than 1 will activate Set function
	checkTestFlags = test.TestIsSet(TestBitFlags::ERed); //expect 2
	checkValue = test.TestValue(); //expect 66
	TheTest(checkTestFlags == 2);
	TheTest(checkValue == 66);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0470
@SYMTestCaseDesc        Tests the behaviour of bits
@SYMTestPriority        High
@SYMTestActions         Tests for [] operator
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void DoTest3()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0470 operator [] "));

	TestBitFlags test;
	TBool checkTestFlags;

	TRAPD(errCode, test.TestSetL(TestBitFlags::EBlue));
	TheTest(errCode==KErrNone);

	checkTestFlags = test.TestOperator1(TestBitFlags::ERed); //0
	TheTest(checkTestFlags == 0);

	TRAP(errCode, test.TestSetL(TestBitFlags::EYellow));
	TheTest(errCode==KErrNone);

	checkTestFlags = test.TestOperator1(TestBitFlags::EGreen); //0
	TheTest(checkTestFlags == 0);

	checkTestFlags = test.TestOperator1(TestBitFlags::EBlack); //0
	TheTest(checkTestFlags == 0);

	checkTestFlags = test.TestOperator1(TestBitFlags::EYellow); //8
	TheTest(checkTestFlags == 8);

	checkTestFlags = test.TestOperator1(TestBitFlags::EBlue); //1
	TheTest(checkTestFlags == 1);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0471
@SYMTestCaseDesc        Tests the behaviour of bits
@SYMTestPriority        High
@SYMTestActions         Tests for == operator
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void DoTest4()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0471 Operator== "));
	TestBitFlags test;
	test.TestOperator3();
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0472
@SYMTestCaseDesc        Tests the behaviour of bits
@SYMTestPriority        High
@SYMTestActions         Tests for toggling the value
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void DoTest5()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0472 Toggle "));
	TestBitFlags test;
	TInt checkTestFlags;

	test.TestToggleL(TestBitFlags::EBlue);
	checkTestFlags = test.TestValue();
	TheTest(checkTestFlags == 1);

	test.TestToggleL(TestBitFlags::ERed);
	checkTestFlags = test.TestValue();
	TheTest(checkTestFlags == 3);

	test.TestToggleL(TestBitFlags::EBlue);
	checkTestFlags = test.TestValue();
	TheTest(checkTestFlags == 2);

	test.TestToggleL(TestBitFlags::EGreen);
	checkTestFlags = test.TestValue();
	TheTest(checkTestFlags == 6);

	test.TestToggleL(TestBitFlags::ERed);
	checkTestFlags = test.TestValue();
	TheTest(checkTestFlags == 4);

	test.TestToggleL(TestBitFlags::EGreen);
	checkTestFlags = test.TestValue();
	TheTest(checkTestFlags == 0);

	test.TestToggleL(TestBitFlags::EYellow);
	checkTestFlags = test.TestValue();
	TheTest(checkTestFlags == 8);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0473
@SYMTestCaseDesc        Tests the behaviour of bits
@SYMTestPriority        High
@SYMTestActions         Tests for assignment operator
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void DoTest6()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0473 Operator= "));
	TestBitFlags test;
	test.TestOperator2();
	}

// TestData

TTestEnum KEnumArray[] =
	{
	ETest1, ETest2, ETest3, ETest4, ETest5, ETest6, ETest7, ETest8,
	ETest9, ETest10, ETest11, ETest12, ETest13, ETest14, ETest15, ETest16,
	ETest17, ETest18, ETest19, ETest20, ETest21, ETest22, ETest23, ETest24,
	ETest25, ETest26, ETest27, ETest28, ETest29, ETest30, ETest31, ETest32
	};

/**
@SYMTestCaseID				SYSLIB-BAFL-CT-3387
@SYMTestCaseDesc			Tests the behaviour of template class when type is an enum.
@SYMTestPriority				High
@SYMTestActions				For each bit test each function in class for when T is an enum.
							It is important that this test includes the MSB bit because enums
							are stored as an unsigned int and the MSB is the signed bit.
@SYMTestExpectedResults		Flags must be set and reset to the expected values
@SYMDEF					DEF102233
*/
void DEF102233()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-3387 Tests for DEF102233 "));

	TInt arrayPos;
	TInt bitPos;
	for(TInt x = 0; x <= 31; x++)
		{
		arrayPos = x;
		bitPos = x;

		// Constructor
		TBitFlagsT<TTestEnum> myBitFlagMSB1(KEnumArray[arrayPos]);
		TheTest(myBitFlagMSB1.IsSet(bitPos));

		// Copy Constructor
		TBitFlagsT<TTestEnum> myBitFlagMSB2(myBitFlagMSB1);
		TheTest(myBitFlagMSB2.IsSet(bitPos));

		// SetAll
		myBitFlagMSB1.SetAll();
		TheTest(myBitFlagMSB1.IsSet(bitPos));

		// ClearAll
		myBitFlagMSB1.ClearAll();
		TheTest(!myBitFlagMSB1.IsSet(bitPos));

		// Set and Clear
		myBitFlagMSB1.Set(bitPos);
		TheTest(myBitFlagMSB1.IsSet(bitPos));
		myBitFlagMSB1.Clear(bitPos);
		TheTest(!myBitFlagMSB1.IsSet(bitPos));

		// Assign
		myBitFlagMSB1.Assign(bitPos, ETrue);
		TheTest(myBitFlagMSB1.IsSet(bitPos));
		myBitFlagMSB1.Assign(bitPos, EFalse);
		TheTest(!myBitFlagMSB1.IsSet(bitPos));

		// Toggle
		myBitFlagMSB1.Toggle(bitPos);
		TheTest(myBitFlagMSB1.IsSet(bitPos));
		myBitFlagMSB1.Toggle(bitPos);
		TheTest(!myBitFlagMSB1.IsSet(bitPos));

		// operator[]
		TheTest(!myBitFlagMSB1[arrayPos]);
		myBitFlagMSB1.Set(bitPos);
		TheTest(myBitFlagMSB1[arrayPos]);

		// operator=
		myBitFlagMSB2 = myBitFlagMSB1;
		TheTest(myBitFlagMSB2[arrayPos]);
		myBitFlagMSB1.Toggle(bitPos);
		myBitFlagMSB2 = myBitFlagMSB1;
		TheTest(!myBitFlagMSB2[arrayPos]);

		// operator==
		TheTest(myBitFlagMSB1 == myBitFlagMSB2);
		myBitFlagMSB1.Toggle(bitPos);
		myBitFlagMSB2.Toggle(bitPos);
		TheTest(myBitFlagMSB1 == myBitFlagMSB2);
		myBitFlagMSB1.Toggle(bitPos);
		TheTest(!(myBitFlagMSB1 == myBitFlagMSB2));

		// IsSet and IsClear
		TheTest(myBitFlagMSB1.IsClear(bitPos));
		myBitFlagMSB1.Toggle(bitPos);
		TheTest(myBitFlagMSB1.IsSet(bitPos));

		// Value and SetValue
		myBitFlagMSB1.ClearAll();
		myBitFlagMSB1.SetValue(KEnumArray[arrayPos]);
		TheTest(myBitFlagMSB1.Value() == KEnumArray[arrayPos]);

		// Value against a signed integer
		TInt signedInteger = KEnumArray[arrayPos];
		TheTest(myBitFlagMSB1.Value() == signedInteger);

		// Value against an unsigned integer
		TUint unsignedInteger = KEnumArray[arrayPos];
		TheTest(myBitFlagMSB1.Value() == unsignedInteger);

		// iFlags
		myBitFlagMSB1.ClearAll();
		myBitFlagMSB1.iFlags = KEnumArray[arrayPos];
		TheTest(myBitFlagMSB1.Value() == KEnumArray[arrayPos]);
		}
	// check MSB was tested
	TheTest(KEnumArray[arrayPos] == ETest32);
	TheTest(bitPos == 31);
	}

/**
@SYMTestCaseID				BASESRVCS-BAFL-CT-4078
@SYMTestCaseDesc			tests the IsSet API for out of bounds index values.
@SYMTestPriority			High
@SYMTestActions				query flag state statinig an index that is out of bound 
@SYMTestExpectedResults		should return EFalse for out of bound values
@SYMDEF						DEF130663
*/
void DEF130663()
	{
	// type is considered as an enum here 
	TheTest.Next(_L(" @SYMTestCaseID:BASESRVCS-BAFL-CT-4078 Tests for DEF130663 "));

	TInt arrayPos;
	TInt bitPos;
	for(TInt x = 0; x <= 31; x++)
		{
		arrayPos = x;
		bitPos = x;

		// Constructor
		TBitFlagsT<TTestEnum> myBitFlag(KEnumArray[arrayPos]);
		TheTest(myBitFlag.IsSet(bitPos));
		
		//clear all and set one paricular bit
		myBitFlag.ClearAll();
		myBitFlag.Set(bitPos);
		
		// check "out of bounds" values as well
		// expected behavior is that for all out of bound values the 
		// result should be EFalse for IsSet("out of bounds") call.
		// IsSet and all the functions directly calling IsSet are being tested here
		// this test in conjuction with the other cts listed here 
		// should be sufficient to validate the fix
		for(TInt i = 0; i < 64; i++)
			{
			if(i==x)
				{
				TheTest(myBitFlag.IsSet(i));
				TheTest(!myBitFlag.IsClear(i));
				TheTest(myBitFlag[i]);
				}
			else
				{
				TheTest(!myBitFlag.IsSet(i));
				TheTest(myBitFlag.IsClear(i));
				TheTest(!myBitFlag[i]);
				}
			}
		}
	// check MSB was tested
	TheTest(KEnumArray[arrayPos] == ETest32);
	TheTest(bitPos == 31);
	}

void RunTestsL()
	{
	DoTest1();
	DoTest2();
	DoTest3();
	DoTest4();
	DoTest5();
	DoTest6();
	DEF102233();
	DEF130663();
	}

TInt E32Main()
	{
	__UHEAP_MARK;

	CTrapCleanup* cleanup = CTrapCleanup::New();
	if	(!cleanup)
	return KErrNoMemory;

	TheTest.Start(_L("TBITFLAGS "));
	TheTest.Title();

	TRAPD(err, RunTestsL());
	TheTest(err == KErrNone);

	delete cleanup;
	TheTest.End();
	TheTest.Close();

	__UHEAP_MARKEND;
	return KErrNone;
	}
