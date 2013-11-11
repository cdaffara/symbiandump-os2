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
* This contains CTestPlatSecServer. This class creates and initializes
* the test steps and return the pointer to TEF
*
*/



#if (!defined __TEST_PLATSEC_SERVER_H__)
#define __TEST_PLATSEC_SERVER_H__

//TEF include
#include <test/testexecuteserverbase.h>

/**
 This class creates and initializes the test steps and return 
 the pointer to TEF
*/

class CTestPlatSecServer : public CTestServer
{
public:
	//Two phase constructor
	static CTestPlatSecServer* NewL();

	//Creates the test step
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);

	//Destructor
	~CTestPlatSecServer();

protected:
	//Constructor
	CTestPlatSecServer();
};
#endif //__TEST_PLATSEC_SERVER_H__
