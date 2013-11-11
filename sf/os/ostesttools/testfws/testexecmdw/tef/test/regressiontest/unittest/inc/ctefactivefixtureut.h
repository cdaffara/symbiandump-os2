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
* @file CTEFClientUT.h
*
*/



#ifndef __CTEFActiveFixtureUT_H__
#define __CTEFActiveFixtureUT_H__

#include <test/tefunit.h>

class CTEFActiveFixtureUT : public CActiveTestFixture
	{
public:
	// SetUp and TearDown code 
	virtual void SetupL();
	virtual void TearDownL();

	// Tests
	void TestCTestConfig();
	void TestCTestConfig_Negtive();
	void TestUnitTimeout();
	void TestUnitLogger();
	void TestUnitTimeout_Negtive();
	void test_Int64_Step_Suite();
	void test_TInt64_STEP(TInt aShift);
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

private:
	};

#endif // __CTEFClientUT_H__
