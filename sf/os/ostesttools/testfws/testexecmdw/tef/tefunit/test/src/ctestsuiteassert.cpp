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
 @file CTestSuiteAssert.cpp
*/

#include "ctestsuiteassert.h"

void CTestSuiteAssert::SetupL()
/**
 * SetupL
 */
	{
	}

void CTestSuiteAssert::TearDownL()
/**
 * TearDownL
 */
	{
	}

void CTestSuiteAssert::TestAssertEquals()
/**
 * TestAssertEquals
 */
	{
	INFO_PRINTF1(_L("Running SuiteA:AssertEquals"));
	ASSERT_EQUALS( 5, 10 );
	}

void CTestSuiteAssert::TestAssertNull()
/**
 * TestAssertNull
 */
	{
	INFO_PRINTF1(_L("Running SuiteA:AssertNull"));
	CTestConfig* aPtr = NULL;
	aPtr = &iConfig;
	ASSERT_NULL( aPtr );
	}

void CTestSuiteAssert::TestAssertNotNull()
/**
 * TestAssertNotNull
 */
	{
	INFO_PRINTF1(_L("Running SuiteA:AssertNotNull"));
	CTestConfig* aPtr = NULL;
	ASSERT_NOT_NULL( aPtr );
	}

void CTestSuiteAssert::TestAssertSame()
/**
 * TestAssertSame
 */
	{
	INFO_PRINTF1(_L("Running SuiteA:AssertSame"));
	CTestConfig* aPtr = NULL;
	ASSERT_SAME( aPtr, &iConfig );
	}

void CTestSuiteAssert::TestAssertNotSame()
/**
 * TestAssertNotSame
 */
	{
	INFO_PRINTF1(_L("Running SuiteA:AssertNotSame"));
	ASSERT_NOT_SAME( &iConfig, &iConfig );
	}

void CTestSuiteAssert::TestAssertTrue()
/**
 * TestAssertTrue
 */
	{
	INFO_PRINTF1(_L("Running SuiteA:AssertTrue"));
	ASSERT_TRUE( EFalse );
	}

void CTestSuiteAssert::TestAssertFalse()
/**
 * TestAssertFalse
 */
	{
	INFO_PRINTF1(_L("Running SuiteA:AssertFalse"));
	ASSERT_FALSE( ETrue );
	}

CTestSuite* CTestSuiteAssert::CreateSuiteL( const TDesC& aName )
/**
 * CreateSuiteL
 *
 * @param aName - Suite name
 * @return - Suite
 */
	{
	SUB_SUITE;
#ifndef __GCCXML__
 	ADD_TEST_STEP( TestAssertEquals );
 	ADD_TEST_STEP( TestAssertNull );
 	ADD_TEST_STEP( TestAssertNotNull );
 	ADD_TEST_STEP( TestAssertSame );
 	ADD_TEST_STEP( TestAssertNotSame );
 	ADD_TEST_STEP( TestAssertTrue );
 	ADD_TEST_STEP( TestAssertFalse );
#endif
#ifdef __GCCXML__
 	ADD_TEST_STEP_CLASS( CTestSuiteAssert, TestAssertEquals );
 	ADD_TEST_STEP_CLASS( CTestSuiteAssert, TestAssertNull );
 	ADD_TEST_STEP_CLASS( CTestSuiteAssert, TestAssertNotNull );
 	ADD_TEST_STEP_CLASS( CTestSuiteAssert, TestAssertSame );
 	ADD_TEST_STEP_CLASS( CTestSuiteAssert, TestAssertNotSame );
 	ADD_TEST_STEP_CLASS( CTestSuiteAssert, TestAssertTrue );
 	ADD_TEST_STEP_CLASS( CTestSuiteAssert, TestAssertFalse );
#endif	
 	END_SUITE;
	}
