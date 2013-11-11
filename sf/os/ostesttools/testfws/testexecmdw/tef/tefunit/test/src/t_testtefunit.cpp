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
 @file T_TestUniTEF.cpp
*/

#include "ctestsuitea.h"
#include "ctestsuited.h"
#include "ctestsuiteassert.h"
#include "ctestasyncsuiteassert.h"
#include "samplestep.h"

_LIT( KServerName, "TEFUnitTE" );


GLDEF_C const TTestName ServerName()
/**
 * ServerName
 *
 * @return - The TEF server name
 */
	{
	TTestName serverName(KServerName);
	return serverName;
	}

GLDEF_C CTestSuite* CreateTestSuiteL()
/**
 * CreateTestSuiteL
 *
 *	Test Suite Hirarchy
 *
 *               TEFUnitSuite---------------------------------
 *              /           \                 \              \
 *           SuiteA       SuiteD           SuiteAssert  AsyncSuiteAssert
 *          /   |  \     /  |   \
 *      SuiteD One Two One Two  AsyncSuiteA
 *       /  \                     /    \
 *     One  Two                 One    Two
 *
 * @return - The top level suite
 */
	{
	START_SUITE
 	ADD_TEST_SUITE( CTestSuiteA )
 	ADD_TEST_SUITE( CTestSuiteD )
 	ADD_TEST_SUITE( CTestSuiteAssert )
 	ADD_TEST_SUITE( CTestAsyncSuiteAssert )
	END_SUITE
	}

GLDEF_C CTestStep* CreateTEFTestStep(const TDesC& aStepName, CTEFUnitServer& aServer)
	{
	// Initialise test step object to NULL if no TEF steps are assigned
	CTestStep* testStep = NULL;
	if(aStepName == KSampleStep1)
		testStep = new CSampleStep1();
	else if(aStepName == KSampleStep2)
		testStep = new CSampleStep2(aServer);
	else if(aStepName == KSampleStep3)
		testStep = new CSampleStep3(aServer);
	return testStep;
	}
