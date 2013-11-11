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
 @file CXmlEngDomNodeSetTestServer.cpp
 @internalTechnology
*/

#include "CXmlEngDomNodeSetTestServer.h"

//Test Steps
#include "CXmlEngNodeSetTestContains.h"
#include "CXmlEngTestFormInstanceMapCreation.h"
#include "CXmlEngXpathCreation.h"
#include "CXmlEngTestLibXMLNodeCleanup.h"




/**
 @return - Instance of the test server
 */
CXmlEngDomNodeSetTestServer* CXmlEngDomNodeSetTestServer::NewL()
	{
	//Create the server object
	CXmlEngDomNodeSetTestServer * server = new (ELeave) CXmlEngDomNodeSetTestServer();
	
	//Push it to cleanup stack
	CleanupStack::PushL(server);
	
	//2nd phase construction
	server->ConstructL(KServerName);
	
	//Now pop the object from the cleanup stack
	CleanupStack::Pop(server);
	
	//Return the server object
	return server;
	}


void CXmlEngDomNodeSetTestServer::ConstructL(const TDesC& aName)
	{
	//Call the base class implementation
	CTestServer::ConstructL(aName);
	}



CTestStep* CXmlEngDomNodeSetTestServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep= NULL;
	
	if (aStepName == KXmlEngNodeTestSetContains)
		{
			//Create the test step object
			testStep = new CXmlEngNodeSetTestContains(aStepName);
		}
	else if (aStepName == KXmlEngCreateXFormInstanceMap)
		{
			//Create the test step object
			testStep = new CXmlEngTestFormInstanceMapCreation(aStepName);
		}
	else if (aStepName == KXmlEngXPathCreation)
		{
			//Create the test step object
			testStep = new CXmlEngXpathCreation(aStepName);
		}
	
	else if (aStepName == KXmlEngLibXMLNodeCleanup)
		{
			//Create the test step object
			testStep = new CXmlEngTestLibXMLNodeCleanup(aStepName);
		}
				
	//Return the test step object to the calling method
	return testStep;
	}

