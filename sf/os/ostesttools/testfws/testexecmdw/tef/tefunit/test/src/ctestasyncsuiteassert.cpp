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
 @file CTestAsyncSuiteAssert.cpp
*/

#include "ctestasyncsuiteassert.h"

void CTestAsyncSuiteAssert::SetupL()
/**
 * SetupL
 */
	{
	}

void CTestAsyncSuiteAssert::TearDownL()
/**
 * TearDownL
 */
	{
	}

void CTestAsyncSuiteAssert::TestAssertEquals()
/**
 * TestAssertEquals
 */
	{
	INFO_PRINTF1(_L("Running AsyncSuiteAssert:AssertEquals"));
	ASSERT_EQUALS( 5, 10 );
	}

void CTestAsyncSuiteAssert::TestAssertNull()
/**
 * TestAssertNull
 */
	{
	INFO_PRINTF1(_L("Running AsyncSuiteAssert:AssertNull"));
	CTestConfig* aPtr = NULL;
	aPtr = &iConfig;
	ASSERT_NULL( aPtr );
	}

void CTestAsyncSuiteAssert::TestAssertNotNull()
/**
 * TestAssertNotNull
 */
	{
	INFO_PRINTF1(_L("Running AsyncSuiteAssert:AssertNotNull"));
	CTestConfig* aPtr = NULL;
	ASSERT_NOT_NULL( aPtr );
	}

void CTestAsyncSuiteAssert::TestAssertSame()
/**
 * TestAssertSame
 */
	{
	INFO_PRINTF1(_L("Running AsyncSuiteAssert:AssertSame"));
	CTestConfig* aPtr = NULL;
	ASSERT_SAME( aPtr, &iConfig );
	}

void CTestAsyncSuiteAssert::TestAssertNotSame()
/**
 * TestAssertNotSame
 */
	{
	INFO_PRINTF1(_L("Running AsyncSuiteAssert:AssertNotSame"));
	ASSERT_NOT_SAME( &iConfig, &iConfig );
	}

void CTestAsyncSuiteAssert::TestAssertTrue()
/**
 * TestAssertTrue
 */
	{
	INFO_PRINTF1(_L("Running AsyncSuiteAssert:AssertTrue"));
	ASSERT_TRUE( EFalse );
	}


void CTestAsyncSuiteAssert::TestAssertFalse()
/**
 * TestAssertfalse
 */
	{
	INFO_PRINTF1(_L("Running AsyncSuiteAssert:AssertFalse"));
	ASSERT_FALSE( ETrue );
	}

CTestSuite* CTestAsyncSuiteAssert::CreateSuiteL( const TDesC& aName )
/**
 * CreateSuiteL
 *
 * @param aName - Suite name
 * @return - Suite
 */
	{
	SUB_SUITE;
#ifndef __GCCXML__
 	ADD_ASYNC_TEST_STEP( TestAssertEquals );
 	ADD_ASYNC_TEST_STEP( TestAssertNull );
 	ADD_ASYNC_TEST_STEP( TestAssertNotNull );
 	ADD_ASYNC_TEST_STEP( TestAssertSame );
 	ADD_ASYNC_TEST_STEP( TestAssertNotSame );
 	ADD_ASYNC_TEST_STEP( TestAssertTrue );
 	ADD_ASYNC_TEST_STEP( TestAssertFalse );
#endif
#ifdef __GCCXML__
 	ADD_ASYNC_TEST_STEP_CLASS( CTestAsyncSuiteAssert, TestAssertEquals );
 	ADD_ASYNC_TEST_STEP_CLASS( CTestAsyncSuiteAssert, TestAssertNull );
 	ADD_ASYNC_TEST_STEP_CLASS( CTestAsyncSuiteAssert, TestAssertNotNull );
 	ADD_ASYNC_TEST_STEP_CLASS( CTestAsyncSuiteAssert, TestAssertSame );
 	ADD_ASYNC_TEST_STEP_CLASS( CTestAsyncSuiteAssert, TestAssertNotSame );
 	ADD_ASYNC_TEST_STEP_CLASS( CTestAsyncSuiteAssert, TestAssertTrue );
 	ADD_ASYNC_TEST_STEP_CLASS( CTestAsyncSuiteAssert, TestAssertFalse );
#endif
	END_SUITE;
	}
