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
 @file CXmlEngDomUtilTestServer.cpp
 @internalTechnology
*/

#include "CXmlEngDomUtilTestServer.h"

//Different step objects
#include "CXmlEngDOMUtilStep1.h"
#include "CXmlEngNodeTestSetValue.h"
#include "CXmlEngNodeTestReplaceWith.h"
#include "CXmlEngDocumentTestCreateDocumentFragment.h"

/**
 @return - Instance of the test server
 */
CXmlEngDomUtilTestServer* CXmlEngDomUtilTestServer::NewL()
	{
	//Create the server object
	CXmlEngDomUtilTestServer * server = new (ELeave) CXmlEngDomUtilTestServer();
	
	//Push it to cleanup stack
	CleanupStack::PushL(server);
	
	//2nd phase construction
	server->ConstructL(KServerName);
	
	//Now pop the object from the cleanup stack
	CleanupStack::Pop(server);
	
	//Return the server object
	return server;
	}


void CXmlEngDomUtilTestServer::ConstructL(const TDesC& aName)
	{
	//Call the base class implementation
	CTestServer::ConstructL(aName);
	}



CTestStep* CXmlEngDomUtilTestServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep= NULL;
	
	if (aStepName == KXmlEngDomUtilStep1)
		{
			//Create the test step object
			testStep = new CXmlEngDOMUtilStep1(aStepName);
		}
	else if(aStepName == KXmlEngNodeTestSetValueStep)
		{
			//Create the test step object
			testStep = new CXmlEngNodeTestSetValue(aStepName);
		}
	
	else if(aStepName == KXmlEngNodeTestReplaceWithStep)
		{
			//Create the test step object
			testStep = new CXmlEngNodeTestReplaceWith(aStepName);
		}
	
	else if(aStepName == KXmlEngDocumentCreateDocumentFragment)
		{
			//Create the test step object
			testStep = new CXmlEngDocumentTestCreateDocumentFragment(aStepName);
		}
			
	//Return the test step object to the calling method
	return testStep;
	}

