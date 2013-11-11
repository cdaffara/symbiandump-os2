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



#ifndef __CTEFClientUT_H__
#define __CTEFClientUT_H__

#include <test/tefunit.h>

class CTEFClientSuiteUT : public CTestFixture 
	{
public:
	// SetUp and TearDown code 
	virtual void SetupL();
	virtual void TearDownL();

	// Tests
	void TestRunTestStepL();
	void TestOpenLongStepNameL();     
	void TestRunLongTestBlockCommandL();
	void TestRunLongTestStepCommandL();
	void TestLongServerName();
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

private:
	TInt OpenSession(RTestSession &aSession, const TDesC& aName);
	void ServerConnect();
private:
	RTestServ iServer;
	};

#endif // __CTEFClientUT_H__
