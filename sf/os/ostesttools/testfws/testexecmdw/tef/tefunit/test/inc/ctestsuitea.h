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
 @file CTestSuiteA.h
*/

#ifndef __TEST_SUITE_A__
#define __TEST_SUITE_A__

#include "tefunit.h"

class CTestSuiteA : public CTestFixture 
	{
public:
	// SetUp and TearDown code 
	virtual void SetupL();
	virtual void TearDownL();

	// Tests
	void TestOne();
	void TestTwo();
	void TestThree();
	void TestAssertEquals();
	void TestAssertNull();
	void TestAssertNotNull();
	void TestAssertSame();
	void TestAssertNotSame();
	void TestAssertTrue();
	void TestAssertFalse();

	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);
	};

#endif // __TEST_SUITE_A__
