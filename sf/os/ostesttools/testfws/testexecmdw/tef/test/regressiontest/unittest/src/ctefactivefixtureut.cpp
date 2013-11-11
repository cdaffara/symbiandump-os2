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
 @file CTEFClientUT.cpp
*/

#include "ctefactivefixtureut.h"
#include <test/tefexportconst.h>
void CTEFActiveFixtureUT::SetupL()
/**
 * SetupL
 */
	{
	CActiveTestFixture::SetupL();
	}

void CTEFActiveFixtureUT::TearDownL()
/**
 * TearDownL
 */
	{
	CActiveTestFixture::TearDownL();
	}

void CTEFActiveFixtureUT::TestCTestConfig()
	{
	TInt theInt = 10;

	TBool res = iConfig.GetInt(_L("Int"),theInt);
	ASSERT_TRUE(res);
	ASSERT_TRUE(theInt == 10);

	TBool theBool = false;
	res = iConfig.WriteBool(_L("BoolFalse"),theBool);
	ASSERT_TRUE(res);

	res = iConfig.GetBool(_L("BoolFalse"),theBool);
	ASSERT_TRUE(res);
	ASSERT_TRUE(theBool == false);

	res = iConfig.GetBool(_L("BoolTrue"),theBool);
	ASSERT_TRUE(res);
	ASSERT_TRUE(theBool == true);

	theInt = 0xff;
	res = iConfig.WriteHex(_L("Hex"),theInt);
	ASSERT_TRUE(res);

	res = iConfig.GetHex(_L("Hex"),theInt);
	ASSERT_TRUE(res);
	ASSERT_TRUE(theInt == 0xff);

	_LIT(KTestString,"To Be A Drummer! # comments");
	TPtrC theString(KTestString);

	res = iConfig.WriteString(_L("String"),theString);
	ASSERT_TRUE(res);

	res = iConfig.GetString(_L("String"),theString);
	ASSERT_TRUE(res);
	ASSERT_FALSE(theString.Compare(KTestString) );
		
	test_Int64_Step_Suite();
	}
void CTEFActiveFixtureUT::test_Int64_Step_Suite()
	{
	test_TInt64_STEP(0); //<32 bit	
	test_TInt64_STEP(31); //<32 bit
	test_TInt64_STEP(32); //<32 bit 
	test_TInt64_STEP(33); //<64 bit	 
	test_TInt64_STEP(63); //<64 bit
	test_TInt64_STEP(64); //<64 bit
	}
void CTEFActiveFixtureUT::test_TInt64_STEP(TInt aShift)
	{
	TInt64 TheInt64,tmp;
	TheInt64 = (TInt64)1<<aShift ;
	iConfig.WriteInt(_L("TheInt64"),TheInt64);
	(void)iConfig.GetInt(_L("TheInt64"),tmp);
	ASSERT(tmp==TheInt64);
	}
void CTEFActiveFixtureUT::TestCTestConfig_Negtive()
	{
	TInt theInt = 10;

	iConfig.WriteInt(_L("Int"),theInt);
		
	(void)iConfig.GetInt(_L("Int"),theInt);
	
	TBool theBool = false;
	(void)iConfig.WriteBool(_L("BoolFalse"),theBool);
	
	(void)iConfig.GetBool(_L("BoolFalse"),theBool);
	
	(void)iConfig.GetBool(_L("BoolTrue"),theBool);

	theInt = 0xff;
	(void)iConfig.WriteHex(_L("Hex"),theInt);
	
	(void)iConfig.GetHex(_L("Hex"),theInt);	
	_LIT(KTestString,"To Be A Drummer! # comments");
	TPtrC theString(KTestString);

	(void)iConfig.WriteString(_L("String"),theString);	

	TBool res = iConfig.GetString(_L("String"),theString);

	ASSERT_TRUE(res);
	}
void CTEFActiveFixtureUT::TestUnitTimeout()
	{
	User::After(1*1000);
	}
void CTEFActiveFixtureUT::TestUnitTimeout_Negtive()
	{
	User::After(1000*1000*1000);
	}
void CTEFActiveFixtureUT::TestUnitLogger()
	{
	INFO_PRINTF1(_L("TEFUnit logging information!") );
	}

CTestSuite* CTEFActiveFixtureUT::CreateSuiteL(const TDesC& aName)
/**
 * CreateSuiteL
 *
 * @param aName - Suite name
 * @return - Suite
 */
	{
	SUB_SUITE
	ADD_ASYNC_TEST_STEP_CLASS( CTEFActiveFixtureUT,TestCTestConfig)
	ADD_ASYNC_TEST_STEP_CLASS( CTEFActiveFixtureUT,TestCTestConfig_Negtive)
	ADD_ASYNC_TEST_STEP_CLASS( CTEFActiveFixtureUT,TestUnitTimeout)
	ADD_ASYNC_TEST_STEP_CLASS( CTEFActiveFixtureUT,TestUnitLogger)
	ADD_ASYNC_TEST_STEP_CLASS( CTEFActiveFixtureUT,TestUnitTimeout_Negtive)
	END_SUITE
	}
