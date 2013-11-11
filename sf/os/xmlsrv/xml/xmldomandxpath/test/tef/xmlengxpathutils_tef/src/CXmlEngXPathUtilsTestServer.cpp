// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file CXmlEngXPathUtilsTestServer.cpp
 @internalTechnology
*/
#include "CXmlEngXPathUtilsTestServer.h"

//Test step objects 
#include "CXmlEngXPathUtilsToBooleanTest.h"




/**
 @return - Instance of the test server
 */
CXmlEngXPathUtilsTestServer* CXmlEngXPathUtilsTestServer::NewL()
	{
	//Create the server object
	CXmlEngXPathUtilsTestServer * server = new (ELeave) CXmlEngXPathUtilsTestServer();
	
	//Push it to cleanup stack
	CleanupStack::PushL(server);
	
	//2nd phase construction
	server->ConstructL(KServerName);
	
	//Now pop the object from the cleanup stack
	CleanupStack::Pop(server);
	
	//Return the server object
	return server;
	}


void CXmlEngXPathUtilsTestServer::ConstructL(const TDesC& aName)
	{
	//Call the base class implementation
	CTestServer::ConstructL(aName);
	}



CTestStep* CXmlEngXPathUtilsTestServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep= NULL;

	if (aStepName == KXmlEngXPathUtilsToBooleanTest)
		{
			//Create the test step object
			testStep = new CXmlEngXPathUtilsToBooleanTest(aStepName);
		}

	//Return the test step object to the calling method
	return testStep;
	}
