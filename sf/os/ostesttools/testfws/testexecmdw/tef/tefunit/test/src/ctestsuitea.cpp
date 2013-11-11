/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/



/**
 @file CTestSuiteA.cpp
*/

#include "ctestsuitea.h"
#include "ctestsuiteb.h"

void CTestSuiteA::SetupL()
/**
 * SetupL
 */
	{
	}

void CTestSuiteA::TearDownL()
/**
 * TearDownL
 */
	{
	}

void CTestSuiteA::TestOne()
/**
 * TestOne
 */
	{
	INFO_PRINTF1(_L("Running SuiteA:One"));
	TInt theInt = 0;
	TBool res = iConfig.GetInt(_L("SectionOne"),_L("TheInt"),theInt);
	ASSERT_TRUE(res);

	TBool theBool = EFalse;
	res = iConfig.GetBool(_L("SectionOne"),_L("TheBool"), theBool);
	ASSERT_TRUE(res);

	// Test all the Logging functions
	INFO_PRINTF1(_L("Info Print Tests"));
	INFO_PRINTF2(_L("Int = %d"),theInt);
	INFO_PRINTF3(_L("Int = %d Bool = %d"),theInt,theBool);
	INFO_PRINTF4(_L("Int = %d Bool = %d Int = %d"),theInt,theBool,theInt);
	INFO_PRINTF5(_L("Int = %d Bool = %d Int = %d Bool = %d"),theInt,theBool,theInt, theBool);
	INFO_PRINTF6(_L("Int = %d Bool = %d Int = %d Bool = %d Int = %d"),theInt,theBool,theInt,theBool,theInt);
	INFO_PRINTF7(_L("Int = %d Bool = %d Int = %d Bool = %d Int = %d Bool = %d"),theInt,theBool,theInt,theBool, theInt, theBool);

	WARN_PRINTF1(_L("Warn Print Tests"));
	WARN_PRINTF2(_L("Int = %d"),theInt);
	WARN_PRINTF3(_L("Int = %d Bool = %d"),theInt,theBool);
	WARN_PRINTF4(_L("Int = %d Bool = %d Int = %d"),theInt,theBool,theInt);
	WARN_PRINTF5(_L("Int = %d Bool = %d Int = %d Bool = %d"),theInt,theBool,theInt, theBool);
	WARN_PRINTF6(_L("Int = %d Bool = %d Int = %d Bool = %d Int = %d"),theInt,theBool,theInt,theBool,theInt);
	WARN_PRINTF7(_L("Int = %d Bool = %d Int = %d Bool = %d Int = %d Bool = %d"),theInt,theBool,theInt,theBool, theInt, theBool);

	ERR_PRINTF1(_L("Err Print Tests"));
	ERR_PRINTF2(_L("Int = %d"),theInt);
	ERR_PRINTF3(_L("Int = %d Bool = %d"),theInt,theBool);
	ERR_PRINTF4(_L("Int = %d Bool = %d Int = %d"),theInt,theBool,theInt);
	ERR_PRINTF5(_L("Int = %d Bool = %d Int = %d Bool = %d"),theInt,theBool,theInt, theBool);
	ERR_PRINTF6(_L("Int = %d Bool = %d Int = %d Bool = %d Int = %d"),theInt,theBool,theInt,theBool,theInt);
	ERR_PRINTF7(_L("Int = %d Bool = %d Int = %d Bool = %d Int = %d Bool = %d"),theInt,theBool,theInt,theBool, theInt, theBool);
	}

void CTestSuiteA::TestTwo()
/**
 * TestTwo
 */
	{
	INFO_PRINTF1(_L("Running SuiteA:Two"));

	// Test all the configuration info is retrieved correctly from SectionOne
	TInt theInt = 0;
	TBool res = iConfig.GetInt(_L("SectionOne"),_L("TheInt"),theInt);
	ASSERT_TRUE(res);
	ASSERT_EQUALS( theInt, 999 );

	TBool theBool = EFalse;
	res = iConfig.GetBool(_L("SectionOne"),_L("TheBool"), theBool);
	ASSERT_TRUE(res);
	ASSERT_TRUE( theBool );

	TInt theHex = 0;
	res = iConfig.GetHex(_L("SectionOne"), _L("TheHex"), theHex);
	ASSERT_TRUE(res);
	ASSERT_EQUALS( theHex, (TInt)0x12345678 );

	TPtrC theString;
	res = iConfig.GetString(_L("SectionOne"), _L("TheString"), theString);
	ASSERT_TRUE(res);
	ASSERT_EQUALS( theString, _L("TheString") );

	// Test all the configuration info is retrieved correctly from SectionTwo
	res = iConfig.GetInt(_L("SectionTwo"),_L("TheInt"),theInt);
	ASSERT_TRUE(res);
	ASSERT_EQUALS( theInt, 666 );

	res = iConfig.GetBool(_L("SectionTwo"),_L("TheBool"), theBool);
	ASSERT_TRUE(res);
	ASSERT_FALSE( theBool );

	res = iConfig.GetHex(_L("SectionTwo"), _L("TheHex"), theHex);
	ASSERT_TRUE(res);
	ASSERT_EQUALS( theHex, (TInt)0x87654321 );

	res = iConfig.GetString(_L("SectionTwo"), _L("TheString"), theString);
	ASSERT_TRUE(res);
	ASSERT_EQUALS( theString, _L("TheStringReturns") );

	// Test all the asserts work
	TInt a = 5;
	TInt b = 5;
	ASSERT_EQUALS(a, b);

	CTestConfig* aPtr = NULL;
	ASSERT_NULL( aPtr );

	aPtr = &iConfig;
	ASSERT_NOT_NULL( aPtr );

	ASSERT_SAME( aPtr, &iConfig );

	aPtr = NULL;
	ASSERT_NOT_SAME( aPtr, &iConfig );

	TBool aBool = ETrue;
	ASSERT_TRUE(aBool);
	ASSERT_FALSE(!aBool);
	}

void CTestSuiteA::TestThree()
/**
 * TestThree
 */
	{
	INFO_PRINTF1(_L("Running SuiteA:Three"));

	// Test all the configuration info is retrieved correctly from SectionOne
	TInt theInt = 90021;
	TBool res = iConfig.WriteInt(_L("SectionWriteTest2"),_L("TheInt"),theInt);
	ASSERT_TRUE(res);

	TBool theBool = EFalse;
	res = iConfig.WriteBool(_L("SectionWriteTest2"),_L("TheBool"), theBool);
	ASSERT_TRUE(res);

	TInt theHex = 12345;
	res = iConfig.WriteHex(_L("SectionWriteTest2"), _L("TheHex"), theHex);
	ASSERT_TRUE(res);

	TPtrC theString(_L("WriteThisStringToSectionWriteTest2Section"));
	res = iConfig.WriteString(_L("SectionWriteTest2"), _L("TheString"), theString);
	ASSERT_TRUE(res);

	// Test all the configuration info is retrieved correctly from SectionTwo
	res = iConfig.WriteInt(_L("TheInt"),theInt);
	ASSERT_TRUE(res);

	res = iConfig.WriteBool(_L("TheBool"), theBool);
	ASSERT_TRUE(res);

	res = iConfig.WriteHex(_L("TheHex"), theHex);
	ASSERT_TRUE(res);

	res = iConfig.WriteString(_L("TheString"), theString);
	ASSERT_TRUE(res);
	}

CTestSuite* CTestSuiteA::CreateSuiteL( const TDesC& aName )
/**
 * CreateSuiteL
 *
 * @param aName - Suite name
 * @return - Suite
 */
	{
	SUB_SUITE;
#ifndef __GCCXML__
 	ADD_TEST_STEP( TestOne );
 	ADD_TEST_STEP( TestTwo );
 	ADD_TEST_STEP( TestThree );
#endif
#ifdef __GCCXML__
 	ADD_TEST_STEP_CLASS( CTestSuiteA, TestOne );
 	ADD_TEST_STEP_CLASS( CTestSuiteA, TestTwo );
 	ADD_TEST_STEP_CLASS( CTestSuiteA, TestThree );
#endif
 	ADD_TEST_SUITE( CTestSuiteB )
	END_SUITE;
	}
