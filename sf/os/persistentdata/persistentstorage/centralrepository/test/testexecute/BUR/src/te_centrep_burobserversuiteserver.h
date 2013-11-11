/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file
*/

#if (!defined __TE_CENTREP_BUR_OBSERVER_SERVER_H__)
#define __TE_CENTREP_BUR_OBSERVER_SERVER_H__
#include <test/testexecuteserverbase.h>

/**
This TEF server is designed to complete step 4 and step 6 the test of restore 
notification of Central Repository. 

The test process is:
1. Create repositories.
2. Backup repositories.
3. Modify repositories.
4. Request notifications
5. Restore repositories.
6. Wait for the notifications and check the result of the restore.

Step 4 and step 6 must share some data because the variables of type TRequestStatus 
will be accessed by CRepository::NotifyRequest() and User::WaitForRequest(). 
As well, step 4 and step 6 must be run in the same thread because both the 
connection to the cenrep server and the notification requests are thread specified. 
Otherwise, the notification requests will be canceled by the disconnection or 
thread termination.

Due to the above reason, this test sever must be loaded with the switch of "SharedData" 
as this command:
LOAD_SUITE te_centrep_burobserversuite -SharedData
*/
class CTe_centrep_BURObserverSuite : public CTestServer
	{
public:
	static CTe_centrep_BURObserverSuite * NewL();
	// Base class pure virtual override
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);

	};

#endif
