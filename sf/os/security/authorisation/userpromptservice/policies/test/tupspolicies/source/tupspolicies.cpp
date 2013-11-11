/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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



#include <test/ctefunitserver.h>
#include <test/tefunit.h>

#include "tpolicyreader.h"
#include "tupsplugins.h"
#include "tserviceconfig.h"

_LIT( KServerName, "tupspolicies" );

const TTestName ServerName()
/**
ServerName
@return - The TEF server name
*/
	{
	TTestName serverName(KServerName);
	return serverName;
	}

CTestSuite* CreateTestSuiteL()
/**
Gets the top level test suite
@return - The top level suite
*/
	{
	START_SUITE
	ADD_TEST_SUITE(CTestPlugins);
	ADD_TEST_SUITE(CTestPolicyReader);
	END_SUITE
	}

GLDEF_C CTestStep* CreateTEFTestStep(const TDesC& aStepName, CTEFUnitServer& /* aServer */)
	{
	// Initialise test step object to NULL if no TEF steps are assigned
	CTestStep* testStep(0);
	if(aStepName == KServiceConfigStep)
		testStep = new CServiceConfigTestStep();
	return testStep;
	}
